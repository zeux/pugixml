///////////////////////////////////////////////////////////////////////////////
//
// Pug Improved XML Parser - Version 0.4
// --------------------------------------------------------
// Copyright (C) 2006-2009, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
// This work is based on the pugxml parser, which is:
// Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
// Released into the Public Domain. Use at your own risk.
// See pugxml.xml for further information, history, etc.
// Contributions by Neville Franks (readonly@getsoft.com).
//
///////////////////////////////////////////////////////////////////////////////

#include "pugixml.hpp"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

// For placement new
#include <new>

#if !defined(PUGIXML_NO_XPATH) && defined(PUGIXML_NO_EXCEPTIONS)
#error No exception mode can't be used with XPath support
#endif

#ifndef PUGIXML_NO_STL
#	include <istream>
#	include <ostream>
#endif

#ifdef _MSC_VER
#	pragma warning(disable: 4127) // conditional expression is constant
#	pragma warning(disable: 4996) // this function or variable may be unsafe
#endif

#ifdef __BORLANDC__
#	pragma warn -8008 // condition is always false
#	pragma warn -8066 // unreachable code
#endif

#ifdef __BORLANDC__
// BC workaround
using std::memmove;
using std::memcpy;
#endif

#define STATIC_ASSERT(cond) { static const char condition_failed[(cond) ? 1 : -1] = {0}; (void)condition_failed[0]; }

namespace
{
	void* default_allocate(size_t size)
	{
		return malloc(size);
	}

	void default_deallocate(void* ptr)
	{
		free(ptr);
	}

	pugi::allocation_function global_allocate = default_allocate;
	pugi::deallocation_function global_deallocate = default_deallocate;
}

namespace pugi
{
	struct xml_document_struct;

	class xml_allocator
	{
	public:
		xml_allocator(xml_memory_block* root): _root(root)
		{
		}

		xml_document_struct* allocate_document();
		xml_node_struct* allocate_node(xml_node_type type);
		xml_attribute_struct* allocate_attribute();

	private:
		xml_memory_block* _root;

		void* memalloc(size_t size)
		{
			if (_root->size + size <= memory_block_size)
			{
				void* buf = _root->data + _root->size;
				_root->size += size;
				return buf;
			}
			else
			{
				void* new_block = global_allocate(sizeof(xml_memory_block));

				_root->next = new (new_block) xml_memory_block();
				_root = _root->next;

				_root->size = size;

				return _root->data;
			}
		}
	};

	/// A 'name=value' XML attribute structure.
	struct xml_attribute_struct
	{
		/// Default ctor
		xml_attribute_struct(): name_insitu(true), value_insitu(true), document_order(0), name(0), value(0), prev_attribute(0), next_attribute(0)
		{
		}

		void destroy()
		{
			if (!name_insitu) delete[] name;
			if (!value_insitu) delete[] value;
		}
	
		bool		name_insitu : 1;
		bool		value_insitu : 1;
		unsigned int document_order : 30; ///< Document order value

		char*		name;			///< Pointer to attribute name.
		char*		value;			///< Pointer to attribute value.

		xml_attribute_struct* prev_attribute;	///< Previous attribute
		xml_attribute_struct* next_attribute;	///< Next attribute
	};

	/// An XML document tree node.
	struct xml_node_struct
	{
		/// Default ctor
		/// \param type - node type
		xml_node_struct(xml_node_type type = node_element): type(type), name_insitu(true), value_insitu(true), document_order(0), parent(0), name(0), value(0), first_child(0), last_child(0), prev_sibling(0), next_sibling(0), first_attribute(0), last_attribute(0)
		{
		}

		void destroy()
		{
			if (!name_insitu) delete[] name;
			if (!value_insitu) delete[] value;

			for (xml_attribute_struct* attr = first_attribute; attr; attr = attr->next_attribute)
				attr->destroy();

			for (xml_node_struct* node = first_child; node; node = node->next_sibling)
				node->destroy();
		}

		xml_node_struct* append_node(xml_allocator& alloc, xml_node_type type = node_element)
		{
			xml_node_struct* child = alloc.allocate_node(type);
			child->parent = this;
			
			if (last_child)
			{
				last_child->next_sibling = child;
				child->prev_sibling = last_child;
				last_child = child;
			}
			else first_child = last_child = child;
			
			return child;
		}

		xml_attribute_struct* append_attribute(xml_allocator& alloc)
		{
			xml_attribute_struct* a = alloc.allocate_attribute();

			if (last_attribute)
			{
				last_attribute->next_attribute = a;
				a->prev_attribute = last_attribute;
				last_attribute = a;
			}
			else first_attribute = last_attribute = a;
			
			return a;
		}

		unsigned int			type : 3;				///< Node type; see xml_node_type.
		bool					name_insitu : 1;
		bool					value_insitu : 1;
		unsigned int			document_order : 27;	///< Document order value

		xml_node_struct*		parent;					///< Pointer to parent

		char*					name;					///< Pointer to element name.
		char*					value;					///< Pointer to any associated string data.

		xml_node_struct*		first_child;			///< First child
		xml_node_struct*		last_child;				///< Last child
		
		xml_node_struct*		prev_sibling;			///< Left brother
		xml_node_struct*		next_sibling;			///< Right brother
		
		xml_attribute_struct*	first_attribute;		///< First attribute
		xml_attribute_struct*	last_attribute;			///< Last attribute
	};

	struct xml_document_struct: public xml_node_struct
	{
		xml_document_struct(): xml_node_struct(node_document), allocator(0), buffer(0)
		{
		}

		xml_allocator allocator;
		const char* buffer;
	};

	xml_document_struct* xml_allocator::allocate_document()
	{
		return new (memalloc(sizeof(xml_document_struct))) xml_document_struct;
	}

	xml_node_struct* xml_allocator::allocate_node(xml_node_type type)
	{
		return new (memalloc(sizeof(xml_node_struct))) xml_node_struct(type);
	}

	xml_attribute_struct* xml_allocator::allocate_attribute()
	{
		return new (memalloc(sizeof(xml_attribute_struct))) xml_attribute_struct;
	}
}

namespace
{	
	using namespace pugi;

	const unsigned char UTF8_BYTE_MASK = 0xBF;
	const unsigned char UTF8_BYTE_MARK = 0x80;
	const unsigned char UTF8_BYTE_MASK_READ = 0x3F;
	const unsigned char UTF8_FIRST_BYTE_MARK[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };

	enum chartype
	{
		ct_parse_pcdata = 1,	// \0, &, \r, <
		ct_parse_attr = 2,		// \0, &, \r, ', "
		ct_parse_attr_ws = 4,	// \0, &, \r, ', ", \n, space, tab
		ct_space = 8,			// \r, \n, space, tab
		ct_parse_cdata = 16,	// \0, ], >, \r
		ct_parse_comment = 32,	// \0, -, >, \r
		ct_symbol = 64,			// Any symbol > 127, a-z, A-Z, 0-9, _, :, -, .
		ct_start_symbol = 128	// Any symbol > 127, a-z, A-Z, _, :
	};

	const unsigned char chartype_table[256] =
	{
		55,  0,   0,   0,   0,   0,   0,   0,      0,   12,  12,  0,   0,   63,  0,   0,   // 0-15
		0,   0,   0,   0,   0,   0,   0,   0,      0,   0,   0,   0,   0,   0,   0,   0,   // 16-31
		12,  0,   6,   0,   0,   0,   7,   6,      0,   0,   0,   0,   0,   96,  64,  0,   // 32-47
		64,  64,  64,  64,  64,  64,  64,  64,     64,  64,  192, 0,   1,   0,   48,  0,   // 48-63
		0,   192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 64-79
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 0,   0,   16,  0,   192, // 80-95
		0,   192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 96-111
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 0, 0, 0, 0, 0,           // 112-127

		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 128+
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
		192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192
	};
	
	bool is_chartype(char c, chartype ct)
	{
		return !!(chartype_table[static_cast<unsigned char>(c)] & ct);
	}

	bool strcpy_insitu(char*& dest, bool& insitu, const char* source)
	{
		size_t source_size = strlen(source);

		if (dest && strlen(dest) >= source_size)
		{
			strcpy(dest, source);
			
			return true;
		}
		else
		{
			char* buf = static_cast<char*>(global_allocate(source_size + 1));
			if (!buf) return false;

			strcpy(buf, source);

			if (!insitu) global_deallocate(dest);
			
			dest = buf;
			insitu = false;

			return true;
		}
	}

	// Get the size that is needed for strutf16_utf8 applied to all s characters
	size_t strutf16_utf8_size(const wchar_t* s)
	{
		size_t length = 0;

		for (; *s; ++s)
		{
			unsigned int ch = *s;

			if (ch < 0x80) length += 1;
			else if (ch < 0x800) length += 2;
			else if (ch < 0x10000) length += 3;
			else if (ch < 0x200000) length += 4;
		}

		return length;
	}

	// Write utf16 char to stream, return position after the last written char
	// \return position after last char
	char* strutf16_utf8(char* s, unsigned int ch)
	{
		unsigned int length;

		if (ch < 0x80) length = 1;
		else if (ch < 0x800) length = 2;
		else if (ch < 0x10000) length = 3;
		else if (ch < 0x200000) length = 4;
		else return s;
	
		s += length;

		// Scary scary fall throughs.
		switch (length)
		{
			case 4:
				*--s = (char)((ch | UTF8_BYTE_MARK) & UTF8_BYTE_MASK); 
				ch >>= 6;
			case 3:
				*--s = (char)((ch | UTF8_BYTE_MARK) & UTF8_BYTE_MASK); 
				ch >>= 6;
			case 2:
				*--s = (char)((ch | UTF8_BYTE_MARK) & UTF8_BYTE_MASK); 
				ch >>= 6;
			case 1:
				*--s = (char)(ch | UTF8_FIRST_BYTE_MARK[length]);
		}
		
		return s + length;
	}

	// Get the size that is needed for strutf8_utf16 applied to all s characters
	size_t strutf8_utf16_size(const char* s)
	{
		size_t length = 0;

		for (; *s; ++s)
		{
			unsigned char ch = static_cast<unsigned char>(*s);

			if (ch < 0x80 || (ch >= 0xC0 && ch < 0xFC)) ++length;
		}

		return length;
	}

	// Read utf16 char from utf8 stream, return position after the last read char
	// \return position after the last char
	const char* strutf8_utf16(const char* s, unsigned int& ch)
	{
		unsigned int length;

		const unsigned char* str = reinterpret_cast<const unsigned char*>(s);

		if (*str < UTF8_BYTE_MARK)
		{
			ch = *str;
			return s + 1;
		}
		else if (*str < 0xC0)
		{
			ch = ' ';
			return s + 1;
		}
		else if (*str < 0xE0) length = 2;
		else if (*str < 0xF0) length = 3;
		else if (*str < 0xF8) length = 4;
		else if (*str < 0xFC) length = 5;
		else
		{
			ch = ' ';
			return s + 1;
		}

		ch = (*str++ & ~UTF8_FIRST_BYTE_MARK[length]);
	
		// Scary scary fall throughs.
		switch (length) 
		{
			case 5:
				ch <<= 6;
				ch += (*str++ & UTF8_BYTE_MASK_READ);
			case 4:
				ch <<= 6;
				ch += (*str++ & UTF8_BYTE_MASK_READ);
			case 3:
				ch <<= 6;
				ch += (*str++ & UTF8_BYTE_MASK_READ);
			case 2:
				ch <<= 6;
				ch += (*str++ & UTF8_BYTE_MASK_READ);
		}
		
		return reinterpret_cast<const char*>(str);
	}

	template <bool _1> struct opt1_to_type
	{
		static const bool o1;
	};

	template <bool _1> const bool opt1_to_type<_1>::o1 = _1;

	template <bool _1, bool _2> struct opt2_to_type
	{
		static const bool o1;
		static const bool o2;
	};

	template <bool _1, bool _2> const bool opt2_to_type<_1, _2>::o1 = _1;
	template <bool _1, bool _2> const bool opt2_to_type<_1, _2>::o2 = _2;

	template <bool _1, bool _2, bool _3, bool _4> struct opt4_to_type
	{
		static const bool o1;
		static const bool o2;
		static const bool o3;
		static const bool o4;
	};

	template <bool _1, bool _2, bool _3, bool _4> const bool opt4_to_type<_1, _2, _3, _4>::o1 = _1;
	template <bool _1, bool _2, bool _3, bool _4> const bool opt4_to_type<_1, _2, _3, _4>::o2 = _2;
	template <bool _1, bool _2, bool _3, bool _4> const bool opt4_to_type<_1, _2, _3, _4>::o3 = _3;
	template <bool _1, bool _2, bool _3, bool _4> const bool opt4_to_type<_1, _2, _3, _4>::o4 = _4;

	struct gap
	{
		char* end;
		size_t size;
			
		gap(): end(0), size(0)
		{
		}
			
		// Push new gap, move s count bytes further (skipping the gap).
		// Collapse previous gap.
		void push(char*& s, size_t count)
		{
			if (end) // there was a gap already; collapse it
			{
				// Move [old_gap_end, new_gap_start) to [old_gap_start, ...)
				memmove(end - size, end, s - end);
			}
				
			s += count; // end of current gap
				
			// "merge" two gaps
			end = s;
			size += count;
		}
			
		// Collapse all gaps, return past-the-end pointer
		char* flush(char* s)
		{
			if (end)
			{
				// Move [old_gap_end, current_pos) to [old_gap_start, ...)
				memmove(end - size, end, s - end);

				return s - size;
			}
			else return s;
		}
	};
	
	char* strconv_escape(char* s, gap& g)
	{
		char* stre = s + 1;

		switch (*stre)
		{
			case '#':	// &#...
			{
				unsigned int ucsc = 0;

				++stre;

				if (*stre == 'x') // &#x... (hex code)
				{
					++stre;
					
					while (*stre)
					{
						if (*stre >= '0' && *stre <= '9')
							ucsc = 16 * ucsc + (*stre++ - '0');
						else if (*stre >= 'A' && *stre <= 'F')
							ucsc = 16 * ucsc + (*stre++ - 'A' + 10);
						else if (*stre >= 'a' && *stre <= 'f')
							ucsc = 16 * ucsc + (*stre++ - 'a' + 10);
						else if (*stre == ';')
							break;
						else // cancel
							return stre;
					}

					if (*stre != ';') return stre;
						
					++stre;
				}
				else	// &#... (dec code)
				{
					while (*stre >= '0' && *stre <= '9')
						ucsc = 10 * ucsc + (*stre++ - '0');

					if (*stre != ';') return stre;
						
					++stre;
				}

				s = strutf16_utf8(s, ucsc);
					
				g.push(s, stre - s);
				return stre;
			}
			case 'a':	// &a
			{
				++stre;

				if (*stre == 'm') // &am
				{
					if (*++stre == 'p' && *++stre == ';') // &amp;
					{
						*s++ = '&';
						++stre;
							
						g.push(s, stre - s);
						return stre;
					}
				}
				else if (*stre == 'p') // &ap
				{
					if (*++stre == 'o' && *++stre == 's' && *++stre == ';') // &apos;
					{
						*s++ = '\'';
						++stre;

						g.push(s, stre - s);
						return stre;
					}
				}
				break;
			}
			case 'g': // &g
			{
				if (*++stre == 't' && *++stre == ';') // &gt;
				{
					*s++ = '>';
					++stre;
					
					g.push(s, stre - s);
					return stre;
				}
				break;
			}
			case 'l': // &l
			{
				if (*++stre == 't' && *++stre == ';') // &lt;
				{
					*s++ = '<';
					++stre;
						
					g.push(s, stre - s);
					return stre;
				}
				break;
			}
			case 'q': // &q
			{
				if (*++stre == 'u' && *++stre == 'o' && *++stre == 't' && *++stre == ';') // &quot;
				{
					*s++ = '"';
					++stre;
					
					g.push(s, stre - s);
					return stre;
				}
				break;
			}
		}
		
		return stre;
	}

	char* strconv_comment(char* s)
	{
		if (!*s) return 0;
		
		gap g;
		
		while (true)
		{
			while (!is_chartype(*s, ct_parse_comment)) ++s;
		
			if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
			{
				*s++ = '\n'; // replace first one with 0x0a
				
				if (*s == '\n') g.push(s, 1);
			}
			else if (*s == '-' && *(s+1) == '-' && *(s+2) == '>') // comment ends here
			{
				*g.flush(s) = 0;
				
				return s + 3;
			}
			else if (*s == 0)
			{
				return 0;
			}
			else ++s;
		}
	}

	char* strconv_cdata(char* s)
	{
		if (!*s) return 0;
			
		gap g;
			
		while (true)
		{
			while (!is_chartype(*s, ct_parse_cdata)) ++s;
			
			if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
			{
				*s++ = '\n'; // replace first one with 0x0a
				
				if (*s == '\n') g.push(s, 1);
			}
			else if (*s == ']' && *(s+1) == ']' && *(s+2) == '>') // CDATA ends here
			{
				*g.flush(s) = 0;
				
				return s + 1;
			}
			else if (*s == 0)
			{
				return 0;
			}
			else ++s;
		}
	}
		
	template <typename opt2> char* strconv_pcdata_t(char* s, opt2)
	{
		const bool opt_eol = opt2::o1;
		const bool opt_escape = opt2::o2;

		if (!*s) return 0;

		gap g;
		
		while (true)
		{
			while (!is_chartype(*s, ct_parse_pcdata)) ++s;
				
			if (opt_eol && *s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
			{
				*s++ = '\n'; // replace first one with 0x0a
				
				if (*s == '\n') g.push(s, 1);
			}
			else if (opt_escape && *s == '&')
			{
				s = strconv_escape(s, g);
			}
			else if (*s == '<') // PCDATA ends here
			{
				*g.flush(s) = 0;
				
				return s + 1;
			}
			else if (*s == 0)
			{
				return s;
			}
			else ++s;
		}
	}

	char* strconv_pcdata(char* s, unsigned int optmask)
	{
		STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20);

		switch ((optmask >> 4) & 3) // get bitmask for flags (eol escapes)
		{
		case 0: return strconv_pcdata_t(s, opt2_to_type<0, 0>());
		case 1: return strconv_pcdata_t(s, opt2_to_type<0, 1>());
		case 2: return strconv_pcdata_t(s, opt2_to_type<1, 0>());
		case 3: return strconv_pcdata_t(s, opt2_to_type<1, 1>());
		default: return 0; // should not get here
		}
	}

	template <typename opt4> char* strconv_attribute_t(char* s, char end_quote, opt4)
	{
		const bool opt_wconv = opt4::o1;
		const bool opt_wnorm = opt4::o2;
		const bool opt_eol = opt4::o3;
		const bool opt_escape = opt4::o4;

		if (!*s) return 0;
			
		gap g;

		// Trim whitespaces
		if (opt_wnorm)
		{
			char* str = s;
			
			while (is_chartype(*str, ct_space)) ++str;
			
			if (str != s)
				g.push(s, str - s);
		}

		while (true)
		{
			while (!is_chartype(*s, (opt_wnorm || opt_wconv) ? ct_parse_attr_ws : ct_parse_attr)) ++s;
			
			if (opt_wnorm && is_chartype(*s, ct_space))
			{
				*s++ = ' ';
	
				if (is_chartype(*s, ct_space))
				{
					char* str = s + 1;
					while (is_chartype(*str, ct_space)) ++str;
					
					g.push(s, str - s);
				}
			}
			else if (opt_wconv && is_chartype(*s, ct_space))
			{
				if (opt_eol)
				{
					if (*s == '\r')
					{
						*s++ = ' ';
				
						if (*s == '\n') g.push(s, 1);
					}
					else *s++ = ' ';
				}
				else *s++ = ' ';
			}
			else if (opt_eol && *s == '\r')
			{
				*s++ = '\n';
				
				if (*s == '\n') g.push(s, 1);
			}
			else if (*s == end_quote)
			{
				char* str = g.flush(s);
				
				if (opt_wnorm)
				{
					do *str-- = 0;
					while (is_chartype(*str, ct_space));
				}
				else *str = 0;
			
				return s + 1;
			}
			else if (opt_escape && *s == '&')
			{
				s = strconv_escape(s, g);
			}
			else if (!*s)
			{
				return 0;
			}
			else ++s;
		}
	}
	
	char* strconv_attribute(char* s, char end_quote, unsigned int optmask)
	{
		STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20 && parse_wnorm_attribute == 0x40 && parse_wconv_attribute == 0x80);
		
		switch ((optmask >> 4) & 15) // get bitmask for flags (wconv wnorm eol escapes)
		{
		case 0:  return strconv_attribute_t(s, end_quote, opt4_to_type<0, 0, 0, 0>());
		case 1:  return strconv_attribute_t(s, end_quote, opt4_to_type<0, 0, 0, 1>());
		case 2:  return strconv_attribute_t(s, end_quote, opt4_to_type<0, 0, 1, 0>());
		case 3:  return strconv_attribute_t(s, end_quote, opt4_to_type<0, 0, 1, 1>());
		case 4:  return strconv_attribute_t(s, end_quote, opt4_to_type<0, 1, 0, 0>());
		case 5:  return strconv_attribute_t(s, end_quote, opt4_to_type<0, 1, 0, 1>());
		case 6:  return strconv_attribute_t(s, end_quote, opt4_to_type<0, 1, 1, 0>());
		case 7:  return strconv_attribute_t(s, end_quote, opt4_to_type<0, 1, 1, 1>());
		case 8:  return strconv_attribute_t(s, end_quote, opt4_to_type<1, 0, 0, 0>());
		case 9:  return strconv_attribute_t(s, end_quote, opt4_to_type<1, 0, 0, 1>());
		case 10: return strconv_attribute_t(s, end_quote, opt4_to_type<1, 0, 1, 0>());
		case 11: return strconv_attribute_t(s, end_quote, opt4_to_type<1, 0, 1, 1>());
		case 12: return strconv_attribute_t(s, end_quote, opt4_to_type<1, 1, 0, 0>());
		case 13: return strconv_attribute_t(s, end_quote, opt4_to_type<1, 1, 0, 1>());
		case 14: return strconv_attribute_t(s, end_quote, opt4_to_type<1, 1, 1, 0>());
		case 15: return strconv_attribute_t(s, end_quote, opt4_to_type<1, 1, 1, 1>());
		default: return 0; // should not get here
		}
	}

	struct xml_parser
	{
		xml_allocator& alloc;
		
		// Parser utilities.
		#define SKIPWS()			{ while (is_chartype(*s, ct_space)) ++s; }
		#define OPTSET(OPT)			( optmsk & OPT )
		#define PUSHNODE(TYPE)		{ cursor = cursor->append_node(alloc,TYPE); }
		#define POPNODE()			{ cursor = cursor->parent; }
		#define SCANFOR(X)			{ while (*s != 0 && !(X)) ++s; }
		#define SCANWHILE(X)		{ while ((X)) ++s; }
		#define ENDSEG()			{ ch = *s; *s = 0; ++s; }
		#define CHECK_ERROR()		{ if (*s == 0) return false; }
		
		xml_parser(xml_allocator& alloc): alloc(alloc)
		{
		}
		
		bool parse(char* s, xml_node_struct* xmldoc, unsigned int optmsk = parse_default)
		{
			if (!s || !xmldoc) return false;

			// UTF-8 BOM
			if ((unsigned char)*s == 0xEF && (unsigned char)*(s+1) == 0xBB && (unsigned char)*(s+2) == 0xBF)
				s += 3;
				
			char ch = 0;
			xml_node_struct* cursor = xmldoc;
			char* mark = s;

			while (*s != 0)
			{
				if (*s == '<')
				{
					++s;

				LOC_TAG:
					if (*s == '?') // '<?...'
					{
						++s;

						if (!is_chartype(*s, ct_start_symbol)) // bad PI
							return false;
						else if (OPTSET(parse_pi) || OPTSET(parse_declaration))
						{
							mark = s;
							SCANWHILE(is_chartype(*s, ct_symbol)); // Read PI target
							CHECK_ERROR();

							if (!is_chartype(*s, ct_space) && *s != '?') // Target has to end with space or ?
								return false;

							ENDSEG();
							CHECK_ERROR();

							if (ch == '?') // nothing except target present
							{
								if (*s != '>') return false;
								++s;

								// stricmp / strcasecmp is not portable
								if ((mark[0] == 'x' || mark[0] == 'X') && (mark[1] == 'm' || mark[1] == 'M')
									&& (mark[2] == 'l' || mark[2] == 'L') && mark[3] == 0)
								{
									if (OPTSET(parse_declaration))
									{
										PUSHNODE(node_declaration);

										cursor->name = mark;

										POPNODE();
									}
								}
								else if (OPTSET(parse_pi))
								{
									PUSHNODE(node_pi); // Append a new node on the tree.

									cursor->name = mark;

									POPNODE();
								}
							}
							// stricmp / strcasecmp is not portable
							else if ((mark[0] == 'x' || mark[0] == 'X') && (mark[1] == 'm' || mark[1] == 'M')
								&& (mark[2] == 'l' || mark[2] == 'L') && mark[3] == 0)
							{
								if (OPTSET(parse_declaration))
								{
									PUSHNODE(node_declaration);

									cursor->name = mark;

									// scan for tag end
									mark = s;

									SCANFOR(*s == '?' && *(s+1) == '>'); // Look for '?>'.
									CHECK_ERROR();

									// replace ending ? with / to terminate properly
									*s = '/';

									// parse attributes
									s = mark;

									goto LOC_ATTRIBUTES;
								}
							}
							else
							{
								if (OPTSET(parse_pi))
								{
									PUSHNODE(node_pi); // Append a new node on the tree.

									cursor->name = mark;
								}

								if (is_chartype(ch, ct_space))
								{
									SKIPWS();
									CHECK_ERROR();
	
									mark = s;
								}
								else mark = 0;

								SCANFOR(*s == '?' && *(s+1) == '>'); // Look for '?>'.
								CHECK_ERROR();

								ENDSEG();
								CHECK_ERROR();

								++s; // Step over >

								if (OPTSET(parse_pi))
								{
									cursor->value = mark;

									POPNODE();
								}
							}
						}
						else // not parsing PI
						{
							SCANFOR(*s == '?' && *(s+1) == '>'); // Look for '?>'.
							CHECK_ERROR();

							s += 2;
						}
					}
					else if (*s == '!') // '<!...'
					{
						++s;

						if (*s == '-') // '<!-...'
						{
							++s;

							if (*s == '-') // '<!--...'
							{
								++s;
								
								if (OPTSET(parse_comments))
								{
									PUSHNODE(node_comment); // Append a new node on the tree.
									cursor->value = s; // Save the offset.
								}

								if (OPTSET(parse_eol) && OPTSET(parse_comments))
								{
									s = strconv_comment(s);
									
									if (!s) return false;
								}
								else
								{
									// Scan for terminating '-->'.
									SCANFOR(*s == '-' && *(s+1) == '-' && *(s+2) == '>');
									CHECK_ERROR();
								
									if (OPTSET(parse_comments))
										*s = 0; // Zero-terminate this segment at the first terminating '-'.
									
									s += 3; // Step over the '\0->'.
								}
								
								if (OPTSET(parse_comments))
								{
									POPNODE(); // Pop since this is a standalone.
								}
							}
							else return false;
						}
						else if(*s == '[')
						{
							// '<![CDATA[...'
							if(*++s=='C' && *++s=='D' && *++s=='A' && *++s=='T' && *++s=='A' && *++s == '[')
							{
								++s;
								
								if (OPTSET(parse_cdata))
								{
									PUSHNODE(node_cdata); // Append a new node on the tree.
									cursor->value = s; // Save the offset.

									if (OPTSET(parse_eol))
									{
										s = strconv_cdata(s);
										
										if (!s) return false;
									}
									else
									{
										// Scan for terminating ']]>'.
										SCANFOR(*s == ']' && *(s+1) == ']' && *(s+2) == '>');
										CHECK_ERROR();

										ENDSEG(); // Zero-terminate this segment.
										CHECK_ERROR();
									}

									POPNODE(); // Pop since this is a standalone.
								}
								else // Flagged for discard, but we still have to scan for the terminator.
								{
									// Scan for terminating ']]>'.
									SCANFOR(*s == ']' && *(s+1) == ']' && *(s+2) == '>');
									CHECK_ERROR();

									++s;
								}

								s += 2; // Step over the last ']>'.
							}
							else return false;
						}
						else if (*s=='D' && *++s=='O' && *++s=='C' && *++s=='T' && *++s=='Y' && *++s=='P' && *++s=='E')
						{
							++s;

							SKIPWS(); // Eat any whitespace.
							CHECK_ERROR();

						LOC_DOCTYPE:
							SCANFOR(*s == '\'' || *s == '"' || *s == '[' || *s == '>');
							CHECK_ERROR();

							if (*s == '\'' || *s == '"') // '...SYSTEM "..."
							{
								ch = *s++;
								SCANFOR(*s == ch);
								CHECK_ERROR();

								++s;
								goto LOC_DOCTYPE;
							}

							if(*s == '[') // '...[...'
							{
								++s;
								unsigned int bd = 1; // Bracket depth counter.
								while (*s!=0) // Loop till we're out of all brackets.
								{
									if (*s == ']') --bd;
									else if (*s == '[') ++bd;
									if (bd == 0) break;
									++s;
								}
							}
							
							SCANFOR(*s == '>');
							CHECK_ERROR();

							++s;
						}
						else return false;
					}
					else if (is_chartype(*s, ct_start_symbol)) // '<#...'
					{
						PUSHNODE(node_element); // Append a new node to the tree.

						cursor->name = s;

						SCANWHILE(is_chartype(*s, ct_symbol)); // Scan for a terminator.
						CHECK_ERROR();

						ENDSEG(); // Save char in 'ch', terminate & step over.
						CHECK_ERROR();

						if (ch == '/') // '<#.../'
						{
							if (*s != '>') return false;
							
							POPNODE(); // Pop.

							++s;
						}
						else if (ch == '>')
						{
							// end of tag
						}
						else if (is_chartype(ch, ct_space))
						{
						LOC_ATTRIBUTES:
						    while (*s)
						    {
								SKIPWS(); // Eat any whitespace.
								CHECK_ERROR();
						
								if (is_chartype(*s, ct_start_symbol)) // <... #...
								{
									xml_attribute_struct* a = cursor->append_attribute(alloc); // Make space for this attribute.
									a->name = s; // Save the offset.

									SCANWHILE(is_chartype(*s, ct_symbol)); // Scan for a terminator.
									CHECK_ERROR();

									ENDSEG(); // Save char in 'ch', terminate & step over.
									CHECK_ERROR();

									if (is_chartype(ch, ct_space))
									{
										SKIPWS(); // Eat any whitespace.
										CHECK_ERROR();

										ch = *s;
										++s;
									}
									
									if (ch == '=') // '<... #=...'
									{
										SKIPWS(); // Eat any whitespace.
										CHECK_ERROR();

										if (*s == '\'' || *s == '"') // '<... #="...'
										{
											ch = *s; // Save quote char to avoid breaking on "''" -or- '""'.
											++s; // Step over the quote.
											a->value = s; // Save the offset.

											s = strconv_attribute(s, ch, optmsk);
										
											if (!s) return false;

											// After this line the loop continues from the start;
											// Whitespaces, / and > are ok, symbols are wrong,
											// everything else will be detected
											if (is_chartype(*s, ct_start_symbol)) return false;
										}
										else return false;
									}
									else return false;
								}
								else if (*s == '/')
								{
									++s;

									if (*s != '>') return false;
							
									POPNODE(); // Pop.

									++s;

									break;
								}
								else if (*s == '>')
								{
									++s;

									break;
								}
								else return false;
							}
						}
						else return false;
					}
					else if (*s == '/')
					{
						++s;

						if (!cursor) return false;

						char* name = cursor->name;
						if (!name) return false;
						
						while (*s && is_chartype(*s, ct_symbol))
						{
							if (*s++ != *name++) return false;
						}

						if (*name) return false;
							
						POPNODE(); // Pop.

						SKIPWS();
						CHECK_ERROR();

						if (*s != '>') return false;
						++s;
					}
					else return false;
				}
				else
				{
					mark = s; // Save this offset while searching for a terminator.

					SKIPWS(); // Eat whitespace if no genuine PCDATA here.

					if ((mark == s || !OPTSET(parse_ws_pcdata)) && (!*s || *s == '<'))
					{
						continue;
					}

					s = mark;
							
					if (static_cast<xml_node_type>(cursor->type) != node_document)
					{
						PUSHNODE(node_pcdata); // Append a new node on the tree.
						cursor->value = s; // Save the offset.

						s = strconv_pcdata(s, optmsk);
								
						if (!s) return false;
								
						POPNODE(); // Pop since this is a standalone.
						
						if (!*s) break;
					}
					else
					{
						SCANFOR(*s == '<'); // '...<'
						if (!*s) break;
						
						++s;
					}

					// We're after '<'
					goto LOC_TAG;
				}
			}

			if (cursor != xmldoc) return false;
			
			return true;
		}
		
	private:
		xml_parser(const xml_parser&);
		const xml_parser& operator=(const xml_parser&);
	};

	// Compare lhs with [rhs_begin, rhs_end)
	int strcmprange(const char* lhs, const char* rhs_begin, const char* rhs_end)
	{
		while (*lhs && rhs_begin != rhs_end && *lhs == *rhs_begin)
		{
			++lhs;
			++rhs_begin;
		}
		
		if (rhs_begin == rhs_end && *lhs == 0) return 0;
		else return 1;
	}
	
	// Character set pattern match.
	int strcmpwild_cset(const char** src, const char** dst)
	{
		int find = 0, excl = 0, star = 0;
		
		if (**src == '!')
		{
			excl = 1;
			++(*src);
		}
		
		while (**src != ']' || star == 1)
		{
			if (find == 0)
			{
				if (**src == '-' && *(*src-1) < *(*src+1) && *(*src+1) != ']' && star == 0)
				{
					if (**dst >= *(*src-1) && **dst <= *(*src+1))
					{
						find = 1;
						++(*src);
					}
				}
				else if (**src == **dst) find = 1;
			}
			++(*src);
			star = 0;
		}

		if (excl == 1) find = (1 - find);
		if (find == 1) ++(*dst);
	
		return find;
	}

	// Wildcard pattern match.
	int strcmpwild_astr(const char** src, const char** dst)
	{
		int find = 1;
		++(*src);
		while ((**dst != 0 && **src == '?') || **src == '*')
		{
			if(**src == '?') ++(*dst);
			++(*src);
		}
		while (**src == '*') ++(*src);
		if (**dst == 0 && **src != 0) return 0;
		if (**dst == 0 && **src == 0) return 1;
		else
		{
			if (impl::strcmpwild(*src,*dst))
			{
				do
				{
					++(*dst);
					while(**src != **dst && **src != '[' && **dst != 0) 
						++(*dst);
				}
				while ((**dst != 0) ? impl::strcmpwild(*src,*dst) : 0 != (find=0));
			}
			if (**dst == 0 && **src == 0) find = 1;
			return find;
		}
	}

	// Output facilities
	struct xml_buffered_writer
	{
		xml_buffered_writer(const xml_buffered_writer&);
		xml_buffered_writer& operator=(const xml_buffered_writer&);

		xml_buffered_writer(xml_writer& writer): writer(writer), bufsize(0)
		{
		}

		~xml_buffered_writer()
		{
			flush();
		}

		void flush()
		{
			if (bufsize > 0) writer.write(buffer, bufsize);
			bufsize = 0;
		}

		void write(const void* data, size_t size)
		{
			if (bufsize + size > sizeof(buffer))
			{
				flush();

				if (size > sizeof(buffer))
				{
					writer.write(data, size);
					return;
				}
			}
			else
			{
				memcpy(buffer + bufsize, data, size);
				bufsize += size;
			}
		}

		void write(const char* data)
		{
			write(data, strlen(data));
		}

		void write(char data)
		{
			if (bufsize + 1 > sizeof(buffer)) flush();

			buffer[bufsize++] = data;
		}

		xml_writer& writer;
		char buffer[8192];
		size_t bufsize;
	};

	template <typename opt1> void text_output_escaped(xml_buffered_writer& writer, const char* s, opt1)
	{
		const bool attribute = opt1::o1;

		while (*s)
		{
			const char* prev = s;
			
			// While *s is a usual symbol
			while (*s && *s != '&' && *s != '<' && *s != '>' && (*s != '"' || !attribute)
					&& (*s < 0 || *s >= 32 || (*s == '\r' && !attribute) || (*s == '\n' && !attribute) || *s == '\t'))
				++s;
		
			writer.write(prev, static_cast<size_t>(s - prev));

			switch (*s)
			{
				case 0: break;
				case '&':
					writer.write("&amp;");
					++s;
					break;
				case '<':
					writer.write("&lt;");
					++s;
					break;
				case '>':
					writer.write("&gt;");
					++s;
					break;
				case '"':
					writer.write("&quot;");
					++s;
					break;
				case '\r':
					writer.write("&#13;");
					++s;
					break;
				case '\n':
					writer.write("&#10;");
					++s;
					break;
				default: // s is not a usual symbol
				{
					unsigned int ch = (unsigned char)*s++;

					char buf[8];
					sprintf(buf, "&#%u;", ch);

					writer.write(buf);
				}
			}
		}
	}

	void node_output(xml_buffered_writer& writer, const xml_node& node, const char* indent, unsigned int flags, unsigned int depth)
	{
		if ((flags & format_indent) != 0 && (flags & format_raw) == 0)
			for (unsigned int i = 0; i < depth; ++i) writer.write(indent);

		switch (node.type())
		{
		case node_document:
		{
			for (xml_node n = node.first_child(); n; n = n.next_sibling())
				node_output(writer, n, indent, flags, depth);
			break;
		}
			
		case node_element:
		{
			writer.write('<');
			writer.write(node.name());

			for (xml_attribute a = node.first_attribute(); a; a = a.next_attribute())
			{
				writer.write(' ');
				writer.write(a.name());
				writer.write('=');
				writer.write('"');

				text_output_escaped(writer, a.value(), opt1_to_type<1>());

				writer.write('"');
			}

			if (flags & format_raw)
			{
				if (!node.first_child())
					writer.write(" />");
				else
				{
					writer.write('>');

					for (xml_node n = node.first_child(); n; n = n.next_sibling())
						node_output(writer, n, indent, flags, depth + 1);

					writer.write('<');
					writer.write('/');
					writer.write(node.name());
					writer.write('>');
				}
			}
			else if (!node.first_child())
				writer.write(" />\n");
			else if (node.first_child() == node.last_child() && node.first_child().type() == node_pcdata)
			{
				writer.write('>');

				text_output_escaped(writer, node.first_child().value(), opt1_to_type<0>());

				writer.write('<');
				writer.write('/');
				writer.write(node.name());
				writer.write('>');
				writer.write('\n');
			}
			else
			{
				writer.write('>');
				writer.write('\n');
				
				for (xml_node n = node.first_child(); n; n = n.next_sibling())
					node_output(writer, n, indent, flags, depth + 1);

				if ((flags & format_indent) != 0 && (flags & format_raw) == 0)
					for (unsigned int i = 0; i < depth; ++i) writer.write(indent);
				
				writer.write('<');
				writer.write('/');
				writer.write(node.name());
				writer.write('>');
				writer.write('\n');
			}

			break;
		}
		
		case node_pcdata:
			text_output_escaped(writer, node.value(), opt1_to_type<0>());
			break;

		case node_cdata:
			writer.write("<![CDATA[");
			writer.write(node.value());
			writer.write("]]>");
			if ((flags & format_raw) == 0) writer.write('\n');
			break;

		case node_comment:
			writer.write("<!--");
			writer.write(node.value());
			writer.write("-->");
			if ((flags & format_raw) == 0) writer.write('\n');
			break;

		case node_pi:
			writer.write("<?");
			writer.write(node.name());
			if (node.value()[0])
			{
				writer.write(' ');
				writer.write(node.value());
			}
			writer.write("?>");
			if ((flags & format_raw) == 0) writer.write('\n');
			break;
		
		case node_declaration:
		{
			writer.write("<?");
			writer.write(node.name());

			for (xml_attribute a = node.first_attribute(); a; a = a.next_attribute())
			{
				writer.write(' ');
				writer.write(a.name());
				writer.write('=');
				writer.write('"');

				text_output_escaped(writer, a.value(), opt1_to_type<1>());

				writer.write('"');
			}

			writer.write("?>");
			if ((flags & format_raw) == 0) writer.write('\n');
			break;
		}

		default:
			assert(false);
		}
	}

	void recursive_copy_skip(xml_node& dest, const xml_node& source, const xml_node& skip)
	{
		assert(dest.type() == source.type());

		switch (source.type())
		{
		case node_element:
		{
			dest.set_name(source.name());

			for (xml_attribute a = source.first_attribute(); a; a = a.next_attribute())
				dest.append_attribute(a.name()).set_value(a.value());

			for (xml_node c = source.first_child(); c; c = c.next_sibling())
			{
				if (c == skip) continue;

				xml_node cc = dest.append_child(c.type());
				assert(cc);

				recursive_copy_skip(cc, c, skip);
			}

			break;
		}

		case node_pcdata:
		case node_cdata:
		case node_comment:
		case node_pi:
			dest.set_value(source.value());
			break;

		case node_declaration:
			dest.set_name(source.name());
			dest.set_value(source.value());
			break;

		default:
			assert(false);
		}
	}
}

namespace pugi
{
	namespace impl
	{
		// Compare two strings, with globbing, and character sets.
		int strcmpwild(const char* src, const char* dst)
		{
			int find = 1;
			for(; *src != 0 && find == 1 && *dst != 0; ++src)
			{
				switch (*src)
				{
					case '?': ++dst; break;
					case '[': ++src; find = strcmpwild_cset(&src,&dst); break;
					case '*': find = strcmpwild_astr(&src,&dst); --src; break;
					default : find = (int) (*src == *dst); ++dst;
				}
			}
			while (*src == '*' && find == 1) ++src;
			return (find == 1 && *dst == 0 && *src == 0) ? 0 : 1;
		}
	}

	xml_writer_file::xml_writer_file(void* file): file(file)
	{
	}

	void xml_writer_file::write(const void* data, size_t size)
	{
		fwrite(data, size, 1, static_cast<FILE*>(file));
	}

#ifndef PUGIXML_NO_STL
	xml_writer_stream::xml_writer_stream(std::ostream& stream): stream(&stream)
	{
	}

	void xml_writer_stream::write(const void* data, size_t size)
	{
		stream->write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
	}
#endif

	xml_tree_walker::xml_tree_walker(): _depth(0)
	{
	}
	
	xml_tree_walker::~xml_tree_walker()
	{
	}

	int xml_tree_walker::depth() const
	{
		return _depth;
	}

	bool xml_tree_walker::begin(xml_node&)
	{
		return true;
	}

	bool xml_tree_walker::end(xml_node&)
	{
		return true;
	}

	xml_attribute::xml_attribute(): _attr(0)
	{
	}

	xml_attribute::xml_attribute(xml_attribute_struct* attr): _attr(attr)
	{
	}

#ifdef __MWERKS__
	xml_attribute::operator xml_attribute::unspecified_bool_type() const
	{
      	return empty() ? 0 : &xml_attribute::empty;
   	}
#else
	xml_attribute::operator xml_attribute::unspecified_bool_type() const
	{
      	return empty() ? 0 : &xml_attribute::_attr;
   	}
#endif

   	bool xml_attribute::operator!() const
   	{
   		return empty();
   	}

	bool xml_attribute::operator==(const xml_attribute& r) const
	{
		return (_attr == r._attr);
	}
	
	bool xml_attribute::operator!=(const xml_attribute& r) const
	{
		return (_attr != r._attr);
	}

	bool xml_attribute::operator<(const xml_attribute& r) const
	{
		return (_attr < r._attr);
	}
	
	bool xml_attribute::operator>(const xml_attribute& r) const
	{
		return (_attr > r._attr);
	}
	
	bool xml_attribute::operator<=(const xml_attribute& r) const
	{
		return (_attr <= r._attr);
	}
	
	bool xml_attribute::operator>=(const xml_attribute& r) const
	{
		return (_attr >= r._attr);
	}

   	xml_attribute xml_attribute::next_attribute() const
   	{
    	return _attr ? xml_attribute(_attr->next_attribute) : xml_attribute();
   	}

    xml_attribute xml_attribute::previous_attribute() const
    {
    	return _attr ? xml_attribute(_attr->prev_attribute) : xml_attribute();
    }

	int xml_attribute::as_int() const
	{
		if(empty() || !_attr->value) return 0;
		return atoi(_attr->value);
	}

	unsigned int xml_attribute::as_uint() const
	{
		if(empty() || !_attr->value) return 0;
		int result = atoi(_attr->value);
		return result < 0 ? 0 : static_cast<unsigned int>(result);
	}

	double xml_attribute::as_double() const
	{
		if(empty() || !_attr->value) return 0.0;
		return atof(_attr->value);
	}

	float xml_attribute::as_float() const
	{
		if(empty() || !_attr->value) return 0.0f;
		return (float)atof(_attr->value);
	}

	bool xml_attribute::as_bool() const
	{
		if(empty() || !_attr->value) return false;
		if(*(_attr->value))
		{
			return // Only look at first char:
			(
				*(_attr->value) == '1' || // 1*
				*(_attr->value) == 't' || // t* (true)
				*(_attr->value) == 'T' || // T* (true|true)
				*(_attr->value) == 'y' || // y* (yes)
				*(_attr->value) == 'Y' // Y* (Yes|YES)
			)
				? true : false; // Return true if matches above, else false.
		}
		else return false;
	}

	bool xml_attribute::empty() const
	{
		return (_attr == 0);
	}

	const char* xml_attribute::name() const
	{
		return (!empty() && _attr->name) ? _attr->name : "";
	}

	const char* xml_attribute::value() const
	{
		return (!empty() && _attr->value) ? _attr->value : "";
	}

	unsigned int xml_attribute::document_order() const
	{
		return empty() ? 0 : _attr->document_order;
	}

	xml_attribute& xml_attribute::operator=(const char* rhs)
	{
		set_value(rhs);
		return *this;
	}
	
	xml_attribute& xml_attribute::operator=(int rhs)
	{
		char buf[128];
		sprintf(buf, "%d", rhs);
		set_value(buf);
		return *this;
	}

	xml_attribute& xml_attribute::operator=(unsigned int rhs)
	{
		char buf[128];
		sprintf(buf, "%u", rhs);
		set_value(buf);
		return *this;
	}

	xml_attribute& xml_attribute::operator=(double rhs)
	{
		char buf[128];
		sprintf(buf, "%g", rhs);
		set_value(buf);
		return *this;
	}
	
	xml_attribute& xml_attribute::operator=(bool rhs)
	{
		set_value(rhs ? "true" : "false");
		return *this;
	}

	bool xml_attribute::set_name(const char* rhs)
	{
		if (empty()) return false;
		
		bool insitu = _attr->name_insitu;
		bool res = strcpy_insitu(_attr->name, insitu, rhs);
		_attr->name_insitu = insitu;
		
		return res;
	}
		
	bool xml_attribute::set_value(const char* rhs)
	{
		if (empty()) return false;

		bool insitu = _attr->value_insitu;
		bool res = strcpy_insitu(_attr->value, insitu, rhs);
		_attr->value_insitu = insitu;
		
		return res;
	}

#ifdef __BORLANDC__
	bool operator&&(const xml_attribute& lhs, bool rhs)
	{
		return lhs ? rhs : false;
	}

	bool operator||(const xml_attribute& lhs, bool rhs)
	{
		return lhs ? true : rhs;
	}
#endif

	xml_node::xml_node(): _root(0)
	{
	}

	xml_node::xml_node(xml_node_struct* p): _root(p)
	{
	}
	
#ifdef __MWERKS__
	xml_node::operator xml_node::unspecified_bool_type() const
	{
      	return empty() ? 0 : &xml_node::empty;
   	}
#else
	xml_node::operator xml_node::unspecified_bool_type() const
	{
      	return empty() ? 0 : &xml_node::_root;
   	}
#endif

   	bool xml_node::operator!() const
   	{
   		return empty();
   	}

	xml_node::iterator xml_node::begin() const
	{
		return iterator(_root->first_child);
	}

	xml_node::iterator xml_node::end() const
	{
		return iterator(0, _root->last_child);
	}
	
	xml_node::attribute_iterator xml_node::attributes_begin() const
	{
		return attribute_iterator(_root->first_attribute);
	}

	xml_node::attribute_iterator xml_node::attributes_end() const
	{
		return attribute_iterator(0, _root->last_attribute);
	}

	bool xml_node::operator==(const xml_node& r) const
	{
		return (_root == r._root);
	}

	bool xml_node::operator!=(const xml_node& r) const
	{
		return (_root != r._root);
	}

	bool xml_node::operator<(const xml_node& r) const
	{
		return (_root < r._root);
	}
	
	bool xml_node::operator>(const xml_node& r) const
	{
		return (_root > r._root);
	}
	
	bool xml_node::operator<=(const xml_node& r) const
	{
		return (_root <= r._root);
	}
	
	bool xml_node::operator>=(const xml_node& r) const
	{
		return (_root >= r._root);
	}

	bool xml_node::empty() const
	{
		return (_root == 0);
	}
	
	xml_allocator& xml_node::get_allocator() const
	{
		xml_node_struct* r = root()._root;

		return static_cast<xml_document_struct*>(r)->allocator;
	}

	const char* xml_node::name() const
	{
		return (!empty() && _root->name) ? _root->name : "";
	}

	xml_node_type xml_node::type() const
	{
		return _root ? static_cast<xml_node_type>(_root->type) : node_null;
	}
	
	const char* xml_node::value() const
	{
		return (!empty() && _root->value) ? _root->value : "";
	}
	
	xml_node xml_node::child(const char* name) const
	{
		if (!empty())
			for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
				if (i->name && !strcmp(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::child_w(const char* name) const
	{
		if (!empty())
			for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
				if (i->name && !impl::strcmpwild(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_attribute xml_node::attribute(const char* name) const
	{
		if (!_root) return xml_attribute();

		for (xml_attribute_struct* i = _root->first_attribute; i; i = i->next_attribute)
			if (i->name && !strcmp(name, i->name))
				return xml_attribute(i);
		
		return xml_attribute();
	}
	
	xml_attribute xml_node::attribute_w(const char* name) const
	{
		if (!_root) return xml_attribute();

		for (xml_attribute_struct* i = _root->first_attribute; i; i = i->next_attribute)
			if (i->name && !impl::strcmpwild(name, i->name))
				return xml_attribute(i);
		
		return xml_attribute();
	}

	xml_node xml_node::next_sibling(const char* name) const
	{
		if(empty()) return xml_node();
		
		for (xml_node_struct* i = _root->next_sibling; i; i = i->next_sibling)
			if (i->name && !strcmp(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::next_sibling_w(const char* name) const
	{
		if(empty()) return xml_node();
		
		for (xml_node_struct* i = _root->next_sibling; i; i = i->next_sibling)
			if (i->name && !impl::strcmpwild(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::next_sibling() const
	{
		if(empty()) return xml_node();
		
		if (_root->next_sibling) return xml_node(_root->next_sibling);
		else return xml_node();
	}

	xml_node xml_node::previous_sibling(const char* name) const
	{
		if (empty()) return xml_node();
		
		for (xml_node_struct* i = _root->prev_sibling; i; i = i->prev_sibling)
			if (i->name && !strcmp(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::previous_sibling_w(const char* name) const
	{
		if (empty()) return xml_node();
		
		for (xml_node_struct* i = _root->prev_sibling; i; i = i->prev_sibling)
			if (i->name && !impl::strcmpwild(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::previous_sibling() const
	{
		if(empty()) return xml_node();
		
		if (_root->prev_sibling) return xml_node(_root->prev_sibling);
		else return xml_node();
	}

	xml_node xml_node::parent() const
	{
		return empty() ? xml_node() : xml_node(_root->parent);
	}

	xml_node xml_node::root() const
	{
		xml_node r = *this;
		while (r && r.parent()) r = r.parent();
		return r;
	}

	const char* xml_node::child_value() const
	{
		if (!empty())
			for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
				if ((static_cast<xml_node_type>(i->type) == node_pcdata || static_cast<xml_node_type>(i->type) == node_cdata) && i->value)
					return i->value;
		return "";
	}

	const char* xml_node::child_value(const char* name) const
	{
		return child(name).child_value();
	}

	const char* xml_node::child_value_w(const char* name) const
	{
		return child_w(name).child_value();
	}

	xml_attribute xml_node::first_attribute() const
	{
		return _root ? xml_attribute(_root->first_attribute) : xml_attribute();
	}

	xml_attribute xml_node::last_attribute() const
	{
		return _root ? xml_attribute(_root->last_attribute) : xml_attribute();
	}

	xml_node xml_node::first_child() const
	{
		if (_root) return xml_node(_root->first_child);
		else return xml_node();
	}

	xml_node xml_node::last_child() const
	{
		if (_root) return xml_node(_root->last_child);
		else return xml_node();
	}

	bool xml_node::set_name(const char* rhs)
	{
		switch (type())
		{
		case node_pi:
		case node_declaration:
		case node_element:
		{
			bool insitu = _root->name_insitu;
			bool res = strcpy_insitu(_root->name, insitu, rhs);
			_root->name_insitu = insitu;
		
			return res;
		}

		default:
			return false;
		}
	}
		
	bool xml_node::set_value(const char* rhs)
	{
		switch (type())
		{
		case node_pi:
		case node_cdata:
		case node_pcdata:
		case node_comment:
		{
			bool insitu = _root->value_insitu;
			bool res = strcpy_insitu(_root->value, insitu, rhs);
			_root->value_insitu = insitu;
		
			return res;
		}

		default:
			return false;
		}
	}

	xml_attribute xml_node::append_attribute(const char* name)
	{
		if (type() != node_element && type() != node_declaration) return xml_attribute();
		
		xml_attribute a(_root->append_attribute(get_allocator()));
		a.set_name(name);
		
		return a;
	}

	xml_attribute xml_node::insert_attribute_before(const char* name, const xml_attribute& attr)
	{
		if ((type() != node_element && type() != node_declaration) || attr.empty()) return xml_attribute();
		
		// check that attribute belongs to *this
		xml_attribute_struct* cur = attr._attr;

		while (cur->prev_attribute) cur = cur->prev_attribute;

		if (cur != _root->first_attribute) return xml_attribute();

		xml_attribute a(get_allocator().allocate_attribute());
		a.set_name(name);

		if (attr._attr->prev_attribute)
			attr._attr->prev_attribute->next_attribute = a._attr;
		else
			_root->first_attribute = a._attr;
		
		a._attr->prev_attribute = attr._attr->prev_attribute;
		a._attr->next_attribute = attr._attr;
		attr._attr->prev_attribute = a._attr;
				
		return a;
	}

	xml_attribute xml_node::insert_attribute_after(const char* name, const xml_attribute& attr)
	{
		if ((type() != node_element && type() != node_declaration) || attr.empty()) return xml_attribute();
		
		// check that attribute belongs to *this
		xml_attribute_struct* cur = attr._attr;

		while (cur->prev_attribute) cur = cur->prev_attribute;

		if (cur != _root->first_attribute) return xml_attribute();

		xml_attribute a(get_allocator().allocate_attribute());
		a.set_name(name);

		if (attr._attr->next_attribute)
			attr._attr->next_attribute->prev_attribute = a._attr;
		else
			_root->last_attribute = a._attr;
		
		a._attr->next_attribute = attr._attr->next_attribute;
		a._attr->prev_attribute = attr._attr;
		attr._attr->next_attribute = a._attr;

		return a;
	}

	xml_attribute xml_node::append_copy(const xml_attribute& proto)
	{
		if (!proto) return xml_attribute();

		xml_attribute result = append_attribute(proto.name());
		result.set_value(proto.value());

		return result;
	}

	xml_attribute xml_node::insert_copy_after(const xml_attribute& proto, const xml_attribute& attr)
	{
		if (!proto) return xml_attribute();

		xml_attribute result = insert_attribute_after(proto.name(), attr);
		result.set_value(proto.value());

		return result;
	}

	xml_attribute xml_node::insert_copy_before(const xml_attribute& proto, const xml_attribute& attr)
	{
		if (!proto) return xml_attribute();

		xml_attribute result = insert_attribute_before(proto.name(), attr);
		result.set_value(proto.value());

		return result;
	}

	xml_node xml_node::append_child(xml_node_type type)
	{
		if ((this->type() != node_element && this->type() != node_document) || type == node_document || type == node_null) return xml_node();
		
		return xml_node(_root->append_node(get_allocator(), type));
	}

	xml_node xml_node::insert_child_before(xml_node_type type, const xml_node& node)
	{
		if ((this->type() != node_element && this->type() != node_document) || type == node_document || type == node_null) return xml_node();
		if (node.parent() != *this) return xml_node();
	
		xml_node n(get_allocator().allocate_node(type));
		n._root->parent = _root;
		
		if (node._root->prev_sibling)
			node._root->prev_sibling->next_sibling = n._root;
		else
			_root->first_child = n._root;
		
		n._root->prev_sibling = node._root->prev_sibling;
		n._root->next_sibling = node._root;
		node._root->prev_sibling = n._root;

		return n;
	}

	xml_node xml_node::insert_child_after(xml_node_type type, const xml_node& node)
	{
		if ((this->type() != node_element && this->type() != node_document) || type == node_document || type == node_null) return xml_node();
		if (node.parent() != *this) return xml_node();
	
		xml_node n(get_allocator().allocate_node(type));
		n._root->parent = _root;
	
		if (node._root->next_sibling)
			node._root->next_sibling->prev_sibling = n._root;
		else
			_root->last_child = n._root;
		
		n._root->next_sibling = node._root->next_sibling;
		n._root->prev_sibling = node._root;
		node._root->next_sibling = n._root;

		return n;
	}

	xml_node xml_node::append_copy(const xml_node& proto)
	{
		xml_node result = append_child(proto.type());

		if (result) recursive_copy_skip(result, proto, result);

		return result;
	}

	xml_node xml_node::insert_copy_after(const xml_node& proto, const xml_node& node)
	{
		xml_node result = insert_child_after(proto.type(), node);

		if (result) recursive_copy_skip(result, proto, result);

		return result;
	}

	xml_node xml_node::insert_copy_before(const xml_node& proto, const xml_node& node)
	{
		xml_node result = insert_child_before(proto.type(), node);

		if (result) recursive_copy_skip(result, proto, result);

		return result;
	}

	void xml_node::remove_attribute(const char* name)
	{
		remove_attribute(attribute(name));
	}

	void xml_node::remove_attribute(const xml_attribute& a)
	{
		if (empty()) return;

		// check that attribute belongs to *this
		xml_attribute_struct* attr = a._attr;

		while (attr->prev_attribute) attr = attr->prev_attribute;

		if (attr != _root->first_attribute) return;

		if (a._attr->next_attribute) a._attr->next_attribute->prev_attribute = a._attr->prev_attribute;
		else _root->last_attribute = a._attr->prev_attribute;
		
		if (a._attr->prev_attribute) a._attr->prev_attribute->next_attribute = a._attr->next_attribute;
		else _root->first_attribute = a._attr->next_attribute;

		a._attr->destroy();
	}

	void xml_node::remove_child(const char* name)
	{
		remove_child(child(name));
	}

	void xml_node::remove_child(const xml_node& n)
	{
		if (empty() || n.parent() != *this) return;

		if (n._root->next_sibling) n._root->next_sibling->prev_sibling = n._root->prev_sibling;
		else _root->last_child = n._root->prev_sibling;
		
		if (n._root->prev_sibling) n._root->prev_sibling->next_sibling = n._root->next_sibling;
		else _root->first_child = n._root->next_sibling;
        
        n._root->destroy();
	}

	xml_node xml_node::find_child_by_attribute(const char* name, const char* attr_name, const char* attr_value)
	{
		if (empty()) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->name && !strcmp(name, i->name))
			{
				for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
					if (!strcmp(attr_name, a->name) && !strcmp(attr_value, a->value))
						return xml_node(i);
			}

		return xml_node();
	}

	xml_node xml_node::find_child_by_attribute_w(const char* name, const char* attr_name, const char* attr_value)
	{
		if (empty()) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->name && !impl::strcmpwild(name, i->name))
			{
				for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
					if (!impl::strcmpwild(attr_name, a->name) && !impl::strcmpwild(attr_value, a->value))
						return xml_node(i);
			}

		return xml_node();
	}

	xml_node xml_node::find_child_by_attribute(const char* attr_name, const char* attr_value)
	{
		if (empty()) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
				if (!strcmp(attr_name, a->name) && !strcmp(attr_value, a->value))
					return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::find_child_by_attribute_w(const char* attr_name, const char* attr_value)
	{
		if (empty()) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
				if (!impl::strcmpwild(attr_name, a->name) && !impl::strcmpwild(attr_value, a->value))
					return xml_node(i);

		return xml_node();
	}

#ifndef PUGIXML_NO_STL
	std::string xml_node::path(char delimiter) const
	{
		std::string path;

		xml_node cursor = *this; // Make a copy.
		
		path = cursor.name();

		while (cursor.parent())
		{
			cursor = cursor.parent();
			
			std::string temp = cursor.name();
			temp += delimiter;
			temp += path;
			path.swap(temp);
		}

		return path;
	}
#endif

	xml_node xml_node::first_element_by_path(const char* path, char delimiter) const
	{
		xml_node found = *this; // Current search context.

		if (empty() || !path || !path[0]) return found;

		if (path[0] == delimiter)
		{
			// Absolute path; e.g. '/foo/bar'
			while (found.parent()) found = found.parent();
			++path;
		}

		const char* path_segment = path;

		while (*path_segment == delimiter) ++path_segment;

		const char* path_segment_end = path_segment;

		while (*path_segment_end && *path_segment_end != delimiter) ++path_segment_end;

		if (path_segment == path_segment_end) return found;

		const char* next_segment = path_segment_end;

		while (*next_segment == delimiter) ++next_segment;

		if (*path_segment == '.' && path_segment + 1 == path_segment_end)
			return found.first_element_by_path(next_segment, delimiter);
		else if (*path_segment == '.' && *(path_segment+1) == '.' && path_segment + 2 == path_segment_end)
			return found.parent().first_element_by_path(next_segment, delimiter);
		else
		{
			for (xml_node_struct* j = found._root->first_child; j; j = j->next_sibling)
			{
				if (j->name && !strcmprange(j->name, path_segment, path_segment_end))
				{
					xml_node subsearch = xml_node(j).first_element_by_path(next_segment, delimiter);

					if (subsearch) return subsearch;
				}
			}

			return xml_node();
		}
	}

	bool xml_node::traverse(xml_tree_walker& walker)
	{
		walker._depth = 0;
		
		if (!walker.begin(*this)) return false;

		xml_node cur = first_child();
				
		if (cur)
		{
			do 
			{
				if (!walker.for_each(cur))
					return false;
						
				if (cur.first_child())
				{
					++walker._depth;
					cur = cur.first_child();
				}
				else if (cur.next_sibling())
					cur = cur.next_sibling();
				else
				{
					// Borland C++ workaround
					while (!cur.next_sibling() && cur != *this && (bool)cur.parent())
					{
						--walker._depth;
						cur = cur.parent();
					}
						
					if (cur != *this)
						cur = cur.next_sibling();
				}
			}
			while (cur && cur != *this);
		}

		if (!walker.end(*this)) return false;
		
		return true;
	}

	unsigned int xml_node::document_order() const
	{
		return empty() ? 0 : _root->document_order;
	}

	void xml_node::precompute_document_order_impl()
	{
		if (type() != node_document) return;

		unsigned int current = 1;
		xml_node cur = *this;

		for (;;)
		{
			cur._root->document_order = current++;
			
			for (xml_attribute a = cur.first_attribute(); a; a = a.next_attribute())
				a._attr->document_order = current++;
					
			if (cur.first_child())
				cur = cur.first_child();
			else if (cur.next_sibling())
				cur = cur.next_sibling();
			else
			{
				while (cur && !cur.next_sibling()) cur = cur.parent();
				cur = cur.next_sibling();
				
				if (!cur) break;
			}
		}
	}

	void xml_node::print(xml_writer& writer, const char* indent, unsigned int flags, unsigned int depth)
	{
		if (empty()) return;

		xml_buffered_writer buffered_writer(writer);

		node_output(buffered_writer, *this, indent, flags, depth);
	}

	int xml_node::offset_debug() const
	{
		xml_node_struct* r = root()._root;

		if (!r) return -1;

		const char* buffer = static_cast<xml_document_struct*>(r)->buffer;

		if (!buffer) return -1;

		switch (type())
		{
		case node_document:
			return 0;

		case node_element:
		case node_declaration:
			return _root->name_insitu ? _root->name - buffer : -1;

		case node_pcdata:
		case node_cdata:
		case node_comment:
		case node_pi:
			return _root->value_insitu ? _root->value - buffer : -1;

		default:
			return -1;
		}
	}

#ifdef __BORLANDC__
	bool operator&&(const xml_node& lhs, bool rhs)
	{
		return lhs ? rhs : false;
	}

	bool operator||(const xml_node& lhs, bool rhs)
	{
		return lhs ? true : rhs;
	}
#endif

	xml_node_iterator::xml_node_iterator()
	{
	}

	xml_node_iterator::xml_node_iterator(const xml_node& node): _wrap(node)
	{
	}

	xml_node_iterator::xml_node_iterator(xml_node_struct* ref): _wrap(ref)
	{
	}
		
	xml_node_iterator::xml_node_iterator(xml_node_struct* ref, xml_node_struct* prev): _prev(prev), _wrap(ref)
	{
	}

	bool xml_node_iterator::operator==(const xml_node_iterator& rhs) const
	{
		return (_wrap == rhs._wrap);
	}
	
	bool xml_node_iterator::operator!=(const xml_node_iterator& rhs) const
	{
		return (_wrap != rhs._wrap);
	}

	xml_node& xml_node_iterator::operator*()
	{
		return _wrap;
	}

	xml_node* xml_node_iterator::operator->()
	{
		return &_wrap;
	}

	const xml_node_iterator& xml_node_iterator::operator++()
	{
		_prev = _wrap;
		_wrap = xml_node(_wrap._root->next_sibling);
		return *this;
	}

	xml_node_iterator xml_node_iterator::operator++(int)
	{
		xml_node_iterator temp = *this;
		++*this;
		return temp;
	}

	const xml_node_iterator& xml_node_iterator::operator--()
	{
		if (_wrap._root) _wrap = xml_node(_wrap._root->prev_sibling);
		else _wrap = _prev;
		return *this;
	}

	xml_node_iterator xml_node_iterator::operator--(int)
	{
		xml_node_iterator temp = *this;
		--*this;
		return temp;
	}

	xml_attribute_iterator::xml_attribute_iterator()
	{
	}

	xml_attribute_iterator::xml_attribute_iterator(const xml_attribute& attr): _wrap(attr)
	{
	}

	xml_attribute_iterator::xml_attribute_iterator(xml_attribute_struct* ref): _wrap(ref)
	{
	}
		
	xml_attribute_iterator::xml_attribute_iterator(xml_attribute_struct* ref, xml_attribute_struct* prev): _prev(prev), _wrap(ref)
	{
	}

	bool xml_attribute_iterator::operator==(const xml_attribute_iterator& rhs) const
	{
		return (_wrap == rhs._wrap);
	}
	
	bool xml_attribute_iterator::operator!=(const xml_attribute_iterator& rhs) const
	{
		return (_wrap != rhs._wrap);
	}

	xml_attribute& xml_attribute_iterator::operator*()
	{
		return _wrap;
	}

	xml_attribute* xml_attribute_iterator::operator->()
	{
		return &_wrap;
	}

	const xml_attribute_iterator& xml_attribute_iterator::operator++()
	{
		_prev = _wrap;
		_wrap = xml_attribute(_wrap._attr->next_attribute);
		return *this;
	}

	xml_attribute_iterator xml_attribute_iterator::operator++(int)
	{
		xml_attribute_iterator temp = *this;
		++*this;
		return temp;
	}

	const xml_attribute_iterator& xml_attribute_iterator::operator--()
	{
		if (_wrap._attr) _wrap = xml_attribute(_wrap._attr->prev_attribute);
		else _wrap = _prev;
		return *this;
	}

	xml_attribute_iterator xml_attribute_iterator::operator--(int)
	{
		xml_attribute_iterator temp = *this;
		--*this;
		return temp;
	}

	xml_memory_block::xml_memory_block(): next(0), size(0)
	{
	}

	xml_document::xml_document(): _buffer(0)
	{
		create();
	}

	xml_document::~xml_document()
	{
		destroy();
	}

	void xml_document::create()
	{
		xml_allocator alloc(&_memory);
		
		_root = alloc.allocate_document(); // Allocate a new root.
		xml_allocator& a = static_cast<xml_document_struct*>(_root)->allocator;
		a = alloc;
	}

	void xml_document::destroy()
	{
		global_deallocate(_buffer);
		_buffer = 0;

		if (_root) _root->destroy();

		xml_memory_block* current = _memory.next;

		while (current)
		{
			xml_memory_block* next = current->next;
			global_deallocate(current);
			current = next;
		}
		
		_memory.next = 0;
		_memory.size = 0;

		create();
	}

#ifndef PUGIXML_NO_STL
	bool xml_document::load(std::istream& stream, unsigned int options)
	{
		destroy();

		if (!stream.good()) return false;

		std::streamoff length, pos = stream.tellg();
		stream.seekg(0, std::ios::end);
		length = stream.tellg();
		stream.seekg(pos, std::ios::beg);

		if (!stream.good()) return false;

		char* s = static_cast<char*>(global_allocate(length + 1));
		if (!s) return false;

		stream.read(s, length);

		if (stream.gcount() > length || stream.gcount() == 0)
		{
			global_deallocate(s);
			return false;
		}

		s[stream.gcount()] = 0;

		return parse(transfer_ownership_tag(), s, options); // Parse the input string.
	}
#endif

	bool xml_document::load(const char* contents, unsigned int options)
	{
		destroy();

		char* s = static_cast<char*>(global_allocate(strlen(contents) + 1));
		if (!s) return false;

		strcpy(s, contents);

		return parse(transfer_ownership_tag(), s, options); // Parse the input string.
	}

	bool xml_document::load_file(const char* name, unsigned int options)
	{
		destroy();

		FILE* file = fopen(name, "rb");
		if (!file) return false;

		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		fseek(file, 0, SEEK_SET);

		if (length < 0)
		{
			fclose(file);
			return false;
		}
		
		char* s = static_cast<char*>(global_allocate(length + 1));

		if (!s)
		{
			fclose(file);
			return false;
		}

		size_t read = fread(s, (size_t)length, 1, file);
		fclose(file);

		if (read != 1)
		{
			global_deallocate(s);
			return false;
		}

		s[length] = 0;
		
		return parse(transfer_ownership_tag(), s, options); // Parse the input string.
	}

	bool xml_document::parse(char* xmlstr, unsigned int options)
	{
		destroy();

		// for offset_debug
		static_cast<xml_document_struct*>(_root)->buffer = xmlstr;

		xml_allocator& alloc = static_cast<xml_document_struct*>(_root)->allocator;
		
		xml_parser parser(alloc);
		
		return parser.parse(xmlstr, _root, options); // Parse the input string.
	}
		
	bool xml_document::parse(const transfer_ownership_tag&, char* xmlstr, unsigned int options)
	{
		bool res = parse(xmlstr, options);

		if (res) _buffer = xmlstr;
		else global_deallocate(xmlstr);

		return res;
	}

	void xml_document::save(xml_writer& writer, const char* indent, unsigned int flags)
	{
		xml_buffered_writer buffered_writer(writer);

		if (flags & format_write_bom_utf8)
		{
			static const unsigned char utf8_bom[] = {0xEF, 0xBB, 0xBF};
			buffered_writer.write(utf8_bom, 3);
		}

		if (!(flags & format_no_declaration))
		{
			buffered_writer.write("<?xml version=\"1.0\"?>");
			if (!(flags & format_raw)) buffered_writer.write("\n");
		}

		node_output(buffered_writer, *this, indent, flags, 0);
	}

	bool xml_document::save_file(const char* name, const char* indent, unsigned int flags)
	{
		FILE* file = fopen(name, "wb");
		if (!file) return false;

		xml_writer_file writer(file);
		save(writer, indent, flags);

		fclose(file);

		return true;
	}

	void xml_document::precompute_document_order()
	{
		precompute_document_order_impl();
	}

#ifndef PUGIXML_NO_STL
	std::string as_utf8(const wchar_t* str)
	{
		std::string result;
		result.reserve(strutf16_utf8_size(str));
	  
		for (; *str; ++str)
		{
			char buffer[6];
	  	
			result.append(buffer, strutf16_utf8(buffer, *str));
		}
	  	
	  	return result;
	}
	
	std::wstring as_utf16(const char* str)
	{
		std::wstring result;
		result.reserve(strutf8_utf16_size(str));

		for (; *str;)
		{
			unsigned int ch;
			str = strutf8_utf16(str, ch);
			result += (wchar_t)ch;
		}

		return result;
	}
#endif

    void set_memory_management_functions(allocation_function allocate, deallocation_function deallocate)
    {
    	global_allocate = allocate;
    	global_deallocate = deallocate;
    }
}
