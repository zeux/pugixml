/**
 * pugixml parser - version 0.6
 * --------------------------------------------------------
 * Copyright (C) 2006-2010, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
 * Report bugs and download new versions at http://code.google.com/p/pugixml/
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 *
 * This work is based on the pugxml parser, which is:
 * Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
 */

#include "pugixml.hpp"

#if !defined(PUGIXML_NO_XPATH) && defined(PUGIXML_NO_EXCEPTIONS)
#error No exception mode can not be used with XPath support
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <wchar.h>

#ifndef PUGIXML_NO_STL
#	include <istream>
#	include <ostream>
#	include <string>
#endif

// For placement new
#include <new>

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

// uintptr_t
#if defined(__BORLANDC__) || defined(__MWERKS__) || defined(__DMC__)
#	include <stdint.h>
#elif defined(_MSC_VER) && _MSC_VER < 1300
typedef size_t uintptr_t;
#endif

// Inlining controls
#if defined(_MSC_VER) && _MSC_VER >= 1300
#	define PUGIXML_NO_INLINE __declspec(noinline)
#elif defined(__GNUC__)
#	define PUGIXML_NO_INLINE __attribute__((noinline))
#else
#	define PUGIXML_NO_INLINE 
#endif

// Simple static assertion
#define STATIC_ASSERT(cond) { static const char condition_failed[(cond) ? 1 : -1] = {0}; (void)condition_failed[0]; }

// Memory allocation
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

// String utilities prototypes
namespace pugi
{
	namespace impl
	{
		size_t strlen(const char_t* s);
		void strcpy(char_t* dst, const char_t* src);
		bool strequalrange(const char_t* lhs, const char_t* rhs, size_t count);
		void widen_ascii(wchar_t* dest, const char* source);
	}
}

// String utilities
namespace pugi
{
	namespace impl
	{
		// Get string length
		size_t strlen(const char_t* s)
		{
		#ifdef PUGIXML_WCHAR_MODE
			return wcslen(s);
		#else
			return ::strlen(s);
		#endif
		}

		// Copy one string into another
		void strcpy(char_t* dst, const char_t* src)
		{
		#ifdef PUGIXML_WCHAR_MODE
			wcscpy(dst, src);
		#else
			::strcpy(dst, src);
		#endif
		}

		// Compare two strings
		bool PUGIXML_FUNCTION strequal(const char_t* src, const char_t* dst)
		{
		#ifdef PUGIXML_WCHAR_MODE
			return wcscmp(src, dst) == 0;
		#else
			return strcmp(src, dst) == 0;
		#endif
		}

		// Compare lhs with [rhs_begin, rhs_end)
		bool strequalrange(const char_t* lhs, const char_t* rhs, size_t count)
		{
			for (size_t i = 0; i < count; ++i)
				if (lhs[i] != rhs[i])
					return false;
		
			return true;
		}
		
		// Character set pattern match.
		static bool strequalwild_cset(const char_t** src, const char_t** dst)
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
		
			return find == 0;
		}

		// Wildcard pattern match.
		static bool strequalwild_astr(const char_t** src, const char_t** dst)
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
				if (!impl::strequalwild(*src,*dst))
				{
					do
					{
						++(*dst);
						while(**src != **dst && **src != '[' && **dst != 0) 
							++(*dst);
					}
					while ((**dst != 0) ? !impl::strequalwild(*src,*dst) : 0 != (find=0));
				}
				if (**dst == 0 && **src == 0) find = 1;
				return find == 0;
			}
		}

		// Compare two strings, with globbing, and character sets.
		bool PUGIXML_FUNCTION strequalwild(const char_t* src, const char_t* dst)
		{
			int find = 1;
			for(; *src != 0 && find == 1 && *dst != 0; ++src)
			{
				switch (*src)
				{
					case '?': ++dst; break;
					case '[': ++src; find = !strequalwild_cset(&src,&dst); break;
					case '*': find = !strequalwild_astr(&src,&dst); --src; break;
					default : find = (int) (*src == *dst); ++dst;
				}
			}
			while (*src == '*' && find == 1) ++src;
			return (find == 1 && *dst == 0 && *src == 0);
		}

#ifdef PUGIXML_WCHAR_MODE
		// Convert string to wide string, assuming all symbols are ASCII
		void widen_ascii(wchar_t* dest, const char* source)
		{
			for (const char* i = source; *i; ++i) *dest++ = *i;
			*dest = 0;
		}
#endif
	}
}

namespace pugi
{
	struct xml_document_struct;

	static const uintptr_t xml_memory_page_alignment = 32;
	static const uintptr_t xml_memory_page_pointer_mask = ~(xml_memory_page_alignment - 1);
	static const uintptr_t xml_memory_page_name_allocated_mask = 16;
	static const uintptr_t xml_memory_page_value_allocated_mask = 8;
	static const uintptr_t xml_memory_page_type_mask = 7;

	struct xml_memory_string_header
	{
		xml_memory_page* page;
		size_t full_size;
	};

	struct xml_allocator
	{
		xml_allocator(xml_memory_page* root): _root(root), _busy_size(root ? root->busy_size : 0)
		{
		}

		~xml_allocator()
		{
			if (_root) _root->busy_size = _busy_size;
		}

		xml_memory_page* allocate_page(size_t data_size)
		{
		#ifdef __GNUC__
			// To avoid offsetof warning (xml_memory_page is not POD)
			xml_memory_page* dummy_page = 0;
			size_t size = ((size_t)(uintptr_t)&dummy_page->data) + data_size;
		#else
			size_t size = offsetof(xml_memory_page, data) + data_size;
		#endif

			// allocate block with some alignment, leaving memory for worst-case padding
			void* memory = global_allocate(size + xml_memory_page_alignment);

			// align upwards to page boundary
			void* page_memory = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(memory) + (xml_memory_page_alignment - 1)) & ~(xml_memory_page_alignment - 1));

			// prepare page structure
			xml_memory_page* page = new (page_memory) xml_memory_page();

			page->memory = memory;
			page->allocator = this;

			return page;
		}

		static void deallocate_page(xml_memory_page* page)
		{
			global_deallocate(page->memory);
		}

		PUGIXML_NO_INLINE void* allocate_memory_oob(size_t size, xml_memory_page*& out_page)
		{
			const size_t large_allocation_threshold = xml_memory_page_size / 4;

			xml_memory_page* page = allocate_page(size <= large_allocation_threshold ? xml_memory_page_size : size);

			if (size <= large_allocation_threshold)
			{
				_root->busy_size = _busy_size;

				// insert page at the end of linked list
				page->prev = _root;
				_root->next = page;
				_root = page;

				_busy_size = size;
			}
			else
			{
				// insert page before the end of linked list
				assert(_root->prev);

				page->prev = _root->prev;
				page->next = _root;

				_root->prev->next = page;
				_root->prev = page;
			}

			// allocate inside page
			page->busy_size = size;

			out_page = page;
			return page->data;
		}

		void* allocate_memory(size_t size, xml_memory_page*& out_page)
		{
			if (_busy_size + size > xml_memory_page_size) return allocate_memory_oob(size, out_page);

			void* buf = _root->data + _busy_size;

			_busy_size += size;

			out_page = _root;

			return buf;
		}

		void deallocate_memory(void* ptr, size_t size, xml_memory_page* page)
		{
			assert(ptr >= page->data && ptr < page->data + xml_memory_page_size);
			(void)!ptr;

			if (page == _root) page->busy_size = _busy_size;

			page->freed_size += size;
			assert(page->freed_size <= page->busy_size);

			if (page->freed_size == page->busy_size)
			{
				if (page->next == 0)
				{
					assert(_root == page);

					// top page freed, just reset sizes
					page->busy_size = page->freed_size = 0;
					_busy_size = 0;
				}
				else
				{
					assert(_root != page);
					assert(page->prev);

					// remove from the list
					page->prev->next = page->next;
					page->next->prev = page->prev;

					// deallocate
					deallocate_page(page);
				}
			}
		}

		char_t* allocate_string(size_t length)
		{
			// get actual size, rounded up to pointer alignment boundary
			size_t size = ((length * sizeof(char_t)) + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1);

			// allocate memory for string and header block
			size_t full_size = sizeof(xml_memory_string_header) + size;

			xml_memory_page* page;
			xml_memory_string_header* header = static_cast<xml_memory_string_header*>(allocate_memory(full_size, page));

			// setup header
			header->page = page;
			header->full_size = full_size;

			return reinterpret_cast<char_t*>(header + 1);
		}

		void deallocate_string(char_t* string)
		{
			// get header
			xml_memory_string_header* header = reinterpret_cast<xml_memory_string_header*>(string) - 1;

			// deallocate
			deallocate_memory(header, header->full_size, header->page);
		}

		xml_memory_page* _root;
		size_t _busy_size;
	};

	/// A 'name=value' XML attribute structure.
	struct xml_attribute_struct
	{
		/// Default ctor
		xml_attribute_struct(xml_memory_page* page): header(reinterpret_cast<uintptr_t>(page)), name(0), value(0), prev_attribute_c(0), next_attribute(0)
		{
		}

		uintptr_t header;

		char_t* name;	///< Pointer to attribute name.
		char_t*	value;	///< Pointer to attribute value.

		xml_attribute_struct* prev_attribute_c;	///< Previous attribute (cyclic list)
		xml_attribute_struct* next_attribute;	///< Next attribute
	};

	/// An XML document tree node.
	struct xml_node_struct
	{
		/// Default ctor
		/// \param type - node type
		xml_node_struct(xml_memory_page* page, xml_node_type type): header(reinterpret_cast<uintptr_t>(page) | type), parent(0), name(0), value(0), first_child(0), prev_sibling_c(0), next_sibling(0), first_attribute(0)
		{
		}

		uintptr_t header;

		xml_node_struct*		parent;					///< Pointer to parent

		char_t*					name;					///< Pointer to element name.
		char_t*					value;					///< Pointer to any associated string data.

		xml_node_struct*		first_child;			///< First child
		
		xml_node_struct*		prev_sibling_c;			///< Left brother (cyclic list)
		xml_node_struct*		next_sibling;			///< Right brother
		
		xml_attribute_struct*	first_attribute;		///< First attribute
	};

	struct xml_document_struct: public xml_node_struct
	{
		xml_document_struct(xml_memory_page* page): xml_node_struct(page, node_document), allocator(0), buffer(0)
		{
		}

		xml_allocator allocator;
		const char_t* buffer;
	};
}

// Low-level DOM operations
namespace
{
	using namespace pugi;

	inline xml_attribute_struct* allocate_attribute(xml_allocator& alloc)
	{
		xml_memory_page* page;
		void* memory = alloc.allocate_memory(sizeof(xml_attribute_struct), page);

		return new (memory) xml_attribute_struct(page);
	}

	inline xml_node_struct* allocate_node(xml_allocator& alloc, xml_node_type type)
	{
		xml_memory_page* page;
		void* memory = alloc.allocate_memory(sizeof(xml_node_struct), page);

		return new (memory) xml_node_struct(page, type);
	}

	inline xml_document_struct* allocate_document(xml_allocator& alloc)
	{
		xml_memory_page* page;
		void* memory = alloc.allocate_memory(sizeof(xml_document_struct), page);

		return new (memory) xml_document_struct(page);
	}

	inline void destroy_attribute(xml_attribute_struct* a, xml_allocator& alloc)
	{
		uintptr_t header = a->header;

		if (header & xml_memory_page_name_allocated_mask) alloc.deallocate_string(a->name);
		if (header & xml_memory_page_value_allocated_mask) alloc.deallocate_string(a->value);

		alloc.deallocate_memory(a, sizeof(xml_attribute_struct), reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask));
	}

	inline void destroy_node(xml_node_struct* n, xml_allocator& alloc)
	{
		uintptr_t header = n->header;

		if (header & xml_memory_page_name_allocated_mask) alloc.deallocate_string(n->name);
		if (header & xml_memory_page_value_allocated_mask) alloc.deallocate_string(n->value);

		for (xml_attribute_struct* attr = n->first_attribute; attr; )
		{
			xml_attribute_struct* next = attr->next_attribute;

			destroy_attribute(attr, alloc);

			attr = next;
		}

		for (xml_node_struct* child = n->first_child; child; )
		{
			xml_node_struct* next = child->next_sibling;

			destroy_node(child, alloc);

			child = next;
		}

		alloc.deallocate_memory(n, sizeof(xml_node_struct), reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask));
	}

	PUGIXML_NO_INLINE xml_node_struct* append_node(xml_node_struct* node, xml_allocator& alloc, xml_node_type type = node_element)
	{
		xml_node_struct* child = allocate_node(alloc, type);
		child->parent = node;

		xml_node_struct* first_child = node->first_child;
			
		if (first_child)
		{
			xml_node_struct* last_child = first_child->prev_sibling_c;

			last_child->next_sibling = child;
			child->prev_sibling_c = last_child;
			first_child->prev_sibling_c = child;
		}
		else
		{
			node->first_child = child;
			child->prev_sibling_c = child;
		}
			
		return child;
	}

	PUGIXML_NO_INLINE xml_attribute_struct* append_attribute_ll(xml_node_struct* node, xml_allocator& alloc)
	{
		xml_attribute_struct* a = allocate_attribute(alloc);

		xml_attribute_struct* first_attribute = node->first_attribute;

		if (first_attribute)
		{
			xml_attribute_struct* last_attribute = first_attribute->prev_attribute_c;

			last_attribute->next_attribute = a;
			a->prev_attribute_c = last_attribute;
			first_attribute->prev_attribute_c = a;
		}
		else
		{
			node->first_attribute = a;
			a->prev_attribute_c = a;
		}
			
		return a;
	}
}

// Unicode utilities
namespace pugi
{
	namespace impl
	{
		typedef unsigned char char8_t;
		typedef unsigned short char16_t;
		typedef unsigned int char32_t;

		inline char16_t endian_swap(char16_t value)
		{
			return static_cast<char16_t>(((value & 0xff) << 8) | (value >> 8));
		}

		inline char32_t endian_swap(char32_t value)
		{
			return ((value & 0xff) << 24) | ((value & 0xff00) << 8) | ((value & 0xff0000) >> 8) | (value >> 24);
		}

		struct utf8_counter
		{
			typedef size_t value_type;

			static value_type low(value_type result, char32_t ch)
			{
				// U+0000..U+007F
				if (ch < 0x80) return result + 1;
				// U+0080..U+07FF
				else if (ch < 0x800) return result + 2;
				// U+0800..U+FFFF
				else return result + 3;
			}

			static value_type high(value_type result, char32_t)
			{
				// U+10000..U+10FFFF
				return result + 4;
			}
		};

		struct utf8_writer
		{
			typedef char8_t* value_type;

			static value_type low(value_type result, char32_t ch)
			{
				// U+0000..U+007F
				if (ch < 0x80)
				{
					*result = static_cast<char8_t>(ch);
					return result + 1;
				}
				// U+0080..U+07FF
				else if (ch < 0x800)
				{
					result[0] = static_cast<char8_t>(0xC0 | (ch >> 6));
					result[1] = static_cast<char8_t>(0x80 | (ch & 0x3F));
					return result + 2;
				}
				// U+0800..U+FFFF
				else
				{
					result[0] = static_cast<char8_t>(0xE0 | (ch >> 12));
					result[1] = static_cast<char8_t>(0x80 | ((ch >> 6) & 0x3F));
					result[2] = static_cast<char8_t>(0x80 | (ch & 0x3F));
					return result + 3;
				}
			}

			static value_type high(value_type result, char32_t ch)
			{
				// U+10000..U+10FFFF
				result[0] = static_cast<char8_t>(0xF0 | (ch >> 18));
				result[1] = static_cast<char8_t>(0x80 | ((ch >> 12) & 0x3F));
				result[2] = static_cast<char8_t>(0x80 | ((ch >> 6) & 0x3F));
				result[3] = static_cast<char8_t>(0x80 | (ch & 0x3F));
				return result + 4;
			}

			static value_type any(value_type result, char32_t ch)
			{
				return (ch < 0x10000) ? low(result, ch) : high(result, ch);
			}
		};

		struct utf16_counter
		{
			typedef size_t value_type;

			static value_type low(value_type result, char32_t)
			{
				return result + 1;
			}

			static value_type high(value_type result, char32_t)
			{
				return result + 2;
			}
		};

		struct utf16_writer
		{
			typedef char16_t* value_type;

			static value_type low(value_type result, char32_t ch)
			{
				*result = static_cast<char16_t>(ch);

				return result + 1;
			}

			static value_type high(value_type result, char32_t ch)
			{
				char32_t msh = (char32_t)(ch - 0x10000) >> 10;
				char32_t lsh = (char32_t)(ch - 0x10000) & 0x3ff;

				result[0] = static_cast<char16_t>(0xD800 + msh);
				result[1] = static_cast<char16_t>(0xDC00 + lsh);

				return result + 2;
			}

			static value_type any(value_type result, char32_t ch)
			{
				return (ch < 0x10000) ? low(result, ch) : high(result, ch);
			}
		};

		struct utf32_counter
		{
			typedef size_t value_type;

			static value_type low(value_type result, char32_t)
			{
				return result + 1;
			}

			static value_type high(value_type result, char32_t)
			{
				return result + 1;
			}
		};

		struct utf32_writer
		{
			typedef char32_t* value_type;

			static value_type low(value_type result, char32_t ch)
			{
				*result = ch;

				return result + 1;
			}

			static value_type high(value_type result, char32_t ch)
			{
				*result = ch;

				return result + 1;
			}

			static value_type any(value_type result, char32_t ch)
			{
				*result = ch;

				return result + 1;
			}
		};

		template <size_t size> struct wchar_selector;

		template <> struct wchar_selector<2>
		{
			typedef char16_t type;
			typedef utf16_counter counter;
			typedef utf16_writer writer;
		};

		template <> struct wchar_selector<4>
		{
			typedef char32_t type;
			typedef utf32_counter counter;
			typedef utf32_writer writer;
		};

		typedef wchar_selector<sizeof(wchar_t)>::counter wchar_counter;
		typedef wchar_selector<sizeof(wchar_t)>::writer wchar_writer;

		template <typename Traits> static inline typename Traits::value_type decode_utf8_block(const char8_t* data, size_t size, typename Traits::value_type result, Traits = Traits())
		{
			const char8_t utf8_byte_mask = 0x3f;

			const char8_t* end = data + size;

			while (data < end)
			{
				char8_t lead = *data;

				// 0xxxxxxx -> U+0000..U+007F
				if (lead < 0x80)
				{
					result = Traits::low(result, lead);
					data += 1;
				}
				// 110xxxxx -> U+0080..U+07FF
				else if ((unsigned)(lead - 0xC0) < 0x20 && data + 1 < end && (data[1] & 0xc0) == 0x80)
				{
					result = Traits::low(result, ((lead & ~0xC0) << 6) | (data[1] & utf8_byte_mask));
					data += 2;
				}
				// 1110xxxx -> U+0800-U+FFFF
				else if ((unsigned)(lead - 0xE0) < 0x10 && data + 2 < end && (data[1] & 0xc0) == 0x80 && (data[2] & 0xc0) == 0x80)
				{
					result = Traits::low(result, ((lead & ~0xE0) << 12) | ((data[1] & utf8_byte_mask) << 6) | (data[2] & utf8_byte_mask));
					data += 3;
				}
				// 11110xxx -> U+10000..U+10FFFF
				else if ((unsigned)(lead - 0xF0) < 0x08 && data + 3 < end && (data[1] & 0xc0) == 0x80 && (data[2] & 0xc0) == 0x80 && (data[3] & 0xc0) == 0x80)
				{
					result = Traits::high(result, ((lead & ~0xF0) << 18) | ((data[1] & utf8_byte_mask) << 12) | ((data[2] & utf8_byte_mask) << 6) | (data[3] & utf8_byte_mask));
					data += 4;
				}
				// 10xxxxxx or 11111xxx -> invalid
				else
				{
					data += 1;
				}
			}

			return result;
		}

		template <typename Traits, typename opt1> static inline typename Traits::value_type decode_utf16_block(const char16_t* data, size_t size, typename Traits::value_type result, opt1, Traits = Traits())
		{
			const bool swap = opt1::o1;

			const char16_t* end = data + size;

			while (data < end)
			{
				char16_t lead = swap ? endian_swap(*data) : *data;

				// U+0000..U+D7FF
				if (lead < 0xD800)
				{
					result = Traits::low(result, lead);
					data += 1;
				}
				// U+E000..U+FFFF
				else if ((unsigned)(lead - 0xE000) < 0x2000)
				{
					result = Traits::low(result, lead);
					data += 1;
				}
				// surrogate pair lead
				else if ((unsigned)(lead - 0xD800) < 0x400 && data + 1 < end)
				{
					char16_t next = swap ? endian_swap(data[1]) : data[1];

					if ((unsigned)(next - 0xDC00) < 0x400)
					{
						result = Traits::high(result, 0x10000 + ((lead & 0x3ff) << 10) + (next & 0x3ff));
						data += 2;
					}
					else
					{
						data += 1;
					}
				}
				else
				{
					data += 1;
				}
			}

			return result;
		}

		template <typename Traits, typename opt1> static inline typename Traits::value_type decode_utf32_block(const char32_t* data, size_t size, typename Traits::value_type result, opt1, Traits = Traits())
		{
			const bool swap = opt1::o1;

			const char32_t* end = data + size;

			while (data < end)
			{
				char32_t lead = swap ? endian_swap(*data) : *data;

				// U+0000..U+FFFF
				if (lead < 0x10000)
				{
					result = Traits::low(result, lead);
					data += 1;
				}
				// U+10000..U+10FFFF
				else
				{
					result = Traits::high(result, lead);
					data += 1;
				}
			}

			return result;
		}

		template <typename T> inline void convert_utf_endian_swap(T* result, const T* data, size_t length)
		{
			for (size_t i = 0; i < length; ++i) result[i] = endian_swap(data[i]);
		}

		inline void convert_wchar_endian_swap(wchar_t* result, const wchar_t* data, size_t length)
		{
			for (size_t i = 0; i < length; ++i) result[i] = static_cast<wchar_t>(endian_swap(static_cast<wchar_selector<sizeof(wchar_t)>::type>(data[i])));
		}
	}
}

namespace
{	
	using namespace pugi;

	enum chartype_t
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

	inline bool is_chartype(char_t c, chartype_t ct)
	{
	#ifdef PUGIXML_WCHAR_MODE
		unsigned int ch = static_cast<unsigned int>(c);

		return !!((ch < 128 ? chartype_table[ch] : chartype_table[128]) & ct);
	#else
		return !!(chartype_table[static_cast<unsigned char>(c)] & ct);
	#endif
	}

	enum output_chartype_t
	{
		oct_special_pcdata = 1,   // Any symbol >= 0 and < 32 (except \t, \r, \n), &, <, >
		oct_special_attr = 2      // Any symbol >= 0 and < 32 (except \t), &, <, >, "
	};

	const unsigned char output_chartype_table[256] =
	{
		3, 3, 3, 3, 3, 3, 3, 3,    3, 0, 2, 3, 3, 2, 3, 3,  // 0-15
		3, 3, 3, 3, 3, 3, 3, 3,    3, 3, 3, 3, 3, 3, 3, 3,  // 16-31
		0, 0, 2, 0, 0, 0, 3, 0,    0, 0, 0, 0, 0, 0, 0, 0,  // 32-47
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 3, 0, 3, 0,  // 48-63

		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,  // 64-128
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,

		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,  // 128+
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0,    0, 0, 0, 0, 0, 0, 0, 0,
	};
	
	inline bool is_output_chartype(char_t c, output_chartype_t ct)
	{
	#ifdef PUGIXML_WCHAR_MODE
		unsigned int ch = static_cast<unsigned int>(c);

		return !!((ch < 128 ? output_chartype_table[ch] : output_chartype_table[128]) & ct);
	#else
		return !!(output_chartype_table[static_cast<unsigned char>(c)] & ct);
	#endif
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

	bool is_little_endian()
	{
		unsigned int ui = 1;

		return *reinterpret_cast<unsigned char*>(&ui) == 1;
	}

	encoding_t get_wchar_encoding()
	{
		STATIC_ASSERT(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

		if (sizeof(wchar_t) == 2)
			return is_little_endian() ? encoding_utf16_le : encoding_utf16_be;
		else 
			return is_little_endian() ? encoding_utf32_le : encoding_utf32_be;
	}

	encoding_t get_buffer_encoding(encoding_t encoding, const void* contents, size_t size)
	{
		// replace wchar encoding with utf implementation
		if (encoding == encoding_wchar) return get_wchar_encoding();

		// replace utf16 encoding with utf16 with specific endianness
		if (encoding == encoding_utf16) return is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

		// replace utf32 encoding with utf32 with specific endianness
		if (encoding == encoding_utf32) return is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

		// only do autodetection if no explicit encoding is requested
		if (encoding != encoding_auto) return encoding;

		// try to guess encoding (based on XML specification, Appendix F.1)
		const impl::char8_t* data = static_cast<const impl::char8_t*>(contents);

		// look for BOM in first few bytes
		if (size > 4 && data[0] == 0 && data[1] == 0 && data[2] == 0xfe && data[3] == 0xff) return encoding_utf32_be;
		if (size > 4 && data[0] == 0xff && data[1] == 0xfe && data[2] == 0 && data[3] == 0) return encoding_utf32_le;
		if (size > 2 && data[0] == 0xfe && data[1] == 0xff) return encoding_utf16_be;
		if (size > 2 && data[0] == 0xff && data[1] == 0xfe) return encoding_utf16_le;
		if (size > 3 && data[0] == 0xef && data[1] == 0xbb && data[2] == 0xbf) return encoding_utf8;

		// look for <, <? or <?xm in various encodings
		if (size > 4 && data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 0x3c) return encoding_utf32_be;
		if (size > 4 && data[0] == 0x3c && data[1] == 0 && data[2] == 0 && data[3] == 0) return encoding_utf32_le;
		if (size > 4 && data[0] == 0 && data[1] == 0x3c && data[2] == 0 && data[3] == 0x3f) return encoding_utf16_be;
		if (size > 4 && data[0] == 0x3c && data[1] == 0 && data[2] == 0x3f && data[3] == 0) return encoding_utf16_le;
		if (size > 4 && data[0] == 0x3c && data[1] == 0x3f && data[2] == 0x78 && data[3] == 0x6d) return encoding_utf8;

		// look for utf16 < followed by node name (this may fail, but is better than utf8 since it's zero terminated so early)
		if (size > 2 && data[0] == 0 && data[1] == 0x3c) return encoding_utf16_be;
		if (size > 2 && data[0] == 0x3c && data[1] == 0) return encoding_utf16_le;

		// no known BOM detected, assume utf8
		return encoding_utf8;
	}

	bool get_mutable_buffer(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
	{
		if (is_mutable)
		{
			out_buffer = static_cast<char_t*>(const_cast<void*>(contents));
		}
		else
		{
			void* buffer = global_allocate(size > 0 ? size : 1);
			if (!buffer) return false;

			memcpy(buffer, contents, size);

			out_buffer = static_cast<char_t*>(buffer);
		}

		out_length = size / sizeof(char_t);

		return true;
	}

#ifdef PUGIXML_WCHAR_MODE
	inline bool need_endian_swap_utf(encoding_t le, encoding_t re)
	{
		return (le == encoding_utf16_be && re == encoding_utf16_le) || (le == encoding_utf16_le && re == encoding_utf16_be) ||
		       (le == encoding_utf32_be && re == encoding_utf32_le) || (le == encoding_utf32_le && re == encoding_utf32_be);
	}

	bool convert_buffer_endian_swap(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
	{
		const char_t* data = static_cast<const char_t*>(contents);
	
		if (is_mutable)
		{
			out_buffer = const_cast<char_t*>(data);
		}
		else
		{
			out_buffer = static_cast<char_t*>(global_allocate(size > 0 ? size : 1));
			if (!out_buffer) return false;
		}

		out_length = size / sizeof(char_t);

		impl::convert_wchar_endian_swap(out_buffer, data, out_length);

		return true;
	}

	bool convert_buffer_utf8(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size)
	{
		const impl::char8_t* data = static_cast<const impl::char8_t*>(contents);

		// first pass: get length in wchar_t units
		out_length = impl::decode_utf8_block<impl::wchar_counter>(data, size, 0);

		// allocate buffer of suitable length
		out_buffer = static_cast<char_t*>(global_allocate((out_length > 0 ? out_length : 1) * sizeof(char_t)));
		if (!out_buffer) return false;

		// second pass: convert utf8 input to wchar_t
		impl::wchar_writer::value_type out_begin = reinterpret_cast<impl::wchar_writer::value_type>(out_buffer);
		impl::wchar_writer::value_type out_end = impl::decode_utf8_block<impl::wchar_writer>(data, size, out_begin);

		assert(out_end == out_begin + out_length);
		(void)!out_end;

		return true;
	}

	template <typename opt1> bool convert_buffer_utf16(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt1)
	{
		const impl::char16_t* data = static_cast<const impl::char16_t*>(contents);
		size_t length = size / sizeof(impl::char16_t);

		// first pass: get length in wchar_t units
		out_length = impl::decode_utf16_block<impl::wchar_counter>(data, length, 0, opt1());

		// allocate buffer of suitable length
		out_buffer = static_cast<char_t*>(global_allocate((out_length > 0 ? out_length : 1) * sizeof(char_t)));
		if (!out_buffer) return false;

		// second pass: convert utf16 input to wchar_t
		impl::wchar_writer::value_type out_begin = reinterpret_cast<impl::wchar_writer::value_type>(out_buffer);
		impl::wchar_writer::value_type out_end = impl::decode_utf16_block<impl::wchar_writer>(data, length, out_begin, opt1());

		assert(out_end == out_begin + out_length);
		(void)!out_end;

		return true;
	}

	template <typename opt1> bool convert_buffer_utf32(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt1)
	{
		const impl::char32_t* data = static_cast<const impl::char32_t*>(contents);
		size_t length = size / sizeof(impl::char32_t);

		// first pass: get length in wchar_t units
		out_length = impl::decode_utf32_block<impl::wchar_counter>(data, length, 0, opt1());

		// allocate buffer of suitable length
		out_buffer = static_cast<char_t*>(global_allocate((out_length > 0 ? out_length : 1) * sizeof(char_t)));
		if (!out_buffer) return false;

		// second pass: convert utf32 input to wchar_t
		impl::wchar_writer::value_type out_begin = reinterpret_cast<impl::wchar_writer::value_type>(out_buffer);
		impl::wchar_writer::value_type out_end = impl::decode_utf32_block<impl::wchar_writer>(data, length, out_begin, opt1());

		assert(out_end == out_begin + out_length);
		(void)!out_end;

		return true;
	}

	bool convert_buffer(char_t*& out_buffer, size_t& out_length, encoding_t encoding, const void* contents, size_t size, bool is_mutable)
	{
		// get native encoding
		encoding_t wchar_encoding = get_wchar_encoding();

		// fast path: no conversion required
		if (encoding == wchar_encoding) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

		// only endian-swapping is required
		if (need_endian_swap_utf(encoding, wchar_encoding)) return convert_buffer_endian_swap(out_buffer, out_length, contents, size, is_mutable);

		// source encoding is utf8
		if (encoding == encoding_utf8) return convert_buffer_utf8(out_buffer, out_length, contents, size);

		// source encoding is utf16
		if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
		{
			encoding_t native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

			return (native_encoding == encoding) ?
				convert_buffer_utf16(out_buffer, out_length, contents, size, opt1_to_type<false>()) :
				convert_buffer_utf16(out_buffer, out_length, contents, size, opt1_to_type<true>());
		}

		// source encoding is utf32
		if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
		{
			encoding_t native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

			return (native_encoding == encoding) ?
				convert_buffer_utf32(out_buffer, out_length, contents, size, opt1_to_type<false>()) :
				convert_buffer_utf32(out_buffer, out_length, contents, size, opt1_to_type<true>());
		}

		// invalid encoding combination (this can't happen)
		assert(false);

		return false;
	}
#else
	template <typename opt1> bool convert_buffer_utf16(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt1)
	{
		const impl::char16_t* data = static_cast<const impl::char16_t*>(contents);
		size_t length = size / sizeof(impl::char16_t);

		// first pass: get length in utf8 units
		out_length = impl::decode_utf16_block<impl::utf8_counter>(data, length, 0, opt1());

		// allocate buffer of suitable length
		out_buffer = static_cast<char_t*>(global_allocate((out_length > 0 ? out_length : 1) * sizeof(char_t)));
		if (!out_buffer) return false;

		// second pass: convert utf16 input to utf8
		impl::char8_t* out_begin = reinterpret_cast<impl::char8_t*>(out_buffer);
		impl::char8_t* out_end = impl::decode_utf16_block<impl::utf8_writer>(data, length, out_begin, opt1());

		assert(out_end == out_begin + out_length);
		(void)!out_end;

		return true;
	}

	template <typename opt1> bool convert_buffer_utf32(char_t*& out_buffer, size_t& out_length, const void* contents, size_t size, opt1)
	{
		const impl::char32_t* data = static_cast<const impl::char32_t*>(contents);
		size_t length = size / sizeof(impl::char32_t);

		// first pass: get length in utf8 units
		out_length = impl::decode_utf32_block<impl::utf8_counter>(data, length, 0, opt1());

		// allocate buffer of suitable length
		out_buffer = static_cast<char_t*>(global_allocate((out_length > 0 ? out_length : 1) * sizeof(char_t)));
		if (!out_buffer) return false;

		// second pass: convert utf32 input to utf8
		impl::char8_t* out_begin = reinterpret_cast<impl::char8_t*>(out_buffer);
		impl::char8_t* out_end = impl::decode_utf32_block<impl::utf8_writer>(data, length, out_begin, opt1());

		assert(out_end == out_begin + out_length);
		(void)!out_end;

		return true;
	}

	bool convert_buffer(char_t*& out_buffer, size_t& out_length, encoding_t encoding, const void* contents, size_t size, bool is_mutable)
	{
		// fast path: no conversion required
		if (encoding == encoding_utf8) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

		// source encoding is utf16
		if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
		{
			encoding_t native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

			return (native_encoding == encoding) ?
				convert_buffer_utf16(out_buffer, out_length, contents, size, opt1_to_type<false>()) :
				convert_buffer_utf16(out_buffer, out_length, contents, size, opt1_to_type<true>());
		}

		// source encoding is utf32
		if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
		{
			encoding_t native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

			return (native_encoding == encoding) ?
				convert_buffer_utf32(out_buffer, out_length, contents, size, opt1_to_type<false>()) :
				convert_buffer_utf32(out_buffer, out_length, contents, size, opt1_to_type<true>());
		}

		// invalid encoding combination (this can't happen)
		assert(false);

		return false;
	}
#endif

	bool strcpy_insitu(char_t*& dest, uintptr_t& header, uintptr_t header_mask, const char_t* source)
	{
		size_t source_length = impl::strlen(source);

		if (dest && impl::strlen(dest) >= source_length)
		{
			impl::strcpy(dest, source);
			
			return true;
		}
		else
		{
			xml_allocator* alloc = reinterpret_cast<xml_memory_page*>(header & xml_memory_page_pointer_mask)->allocator;

			char_t* buf = alloc->allocate_string(source_length + 1);
			if (!buf) return false;

			impl::strcpy(buf, source);

			if (header & header_mask) alloc->deallocate_string(dest);
			
			dest = buf;
			header |= header_mask;

			return true;
		}
	}

	struct gap
	{
		char_t* end;
		size_t size;
			
		gap(): end(0), size(0)
		{
		}
			
		// Push new gap, move s count bytes further (skipping the gap).
		// Collapse previous gap.
		void push(char_t*& s, size_t count)
		{
			if (end) // there was a gap already; collapse it
			{
				// Move [old_gap_end, new_gap_start) to [old_gap_start, ...)
				memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));
			}
				
			s += count; // end of current gap
				
			// "merge" two gaps
			end = s;
			size += count;
		}
			
		// Collapse all gaps, return past-the-end pointer
		char_t* flush(char_t* s)
		{
			if (end)
			{
				// Move [old_gap_end, current_pos) to [old_gap_start, ...)
				memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));

				return s - size;
			}
			else return s;
		}
	};
	
	char_t* strconv_escape(char_t* s, gap& g)
	{
		char_t* stre = s + 1;

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

			#ifdef PUGIXML_WCHAR_MODE
				s = reinterpret_cast<char_t*>(impl::wchar_writer::any(reinterpret_cast<impl::wchar_writer::value_type>(s), ucsc));
			#else
				s = reinterpret_cast<char_t*>(impl::utf8_writer::any(reinterpret_cast<impl::char8_t*>(s), ucsc));
			#endif
					
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

	// Utility macro for last character handling
	#define ENDSWITH(c, e) ((c) == (e) || ((c) == 0 && endch == (e)))

	char_t* strconv_comment(char_t* s, char_t endch)
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
			else if (s[0] == '-' && s[1] == '-' && ENDSWITH(s[2], '>')) // comment ends here
			{
				*g.flush(s) = 0;
				
				return s + (s[2] == '>' ? 3 : 2);
			}
			else if (*s == 0)
			{
				return 0;
			}
			else ++s;
		}
	}

	char_t* strconv_cdata(char_t* s, char_t endch)
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
			else if (s[0] == ']' && s[1] == ']' && ENDSWITH(s[2], '>')) // CDATA ends here
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
	
	typedef char_t* (*strconv_pcdata_t)(char_t*);
		
	template <typename opt2> struct strconv_pcdata_impl
	{
		static char_t* parse(char_t* s)
		{
			const bool opt_eol = opt2::o1;
			const bool opt_escape = opt2::o2;

			gap g;
			
			while (true)
			{
				while (!is_chartype(*s, ct_parse_pcdata)) ++s;
					
				if (*s == '<') // PCDATA ends here
				{
					*g.flush(s) = 0;
					
					return s + 1;
				}
				else if (opt_eol && *s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
				{
					*s++ = '\n'; // replace first one with 0x0a
					
					if (*s == '\n') g.push(s, 1);
				}
				else if (opt_escape && *s == '&')
				{
					s = strconv_escape(s, g);
				}
				else if (*s == 0)
				{
					return s;
				}
				else ++s;
			}
		}
	};
	
	strconv_pcdata_t get_strconv_pcdata(unsigned int optmask)
	{
		STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20);

		switch ((optmask >> 4) & 3) // get bitmask for flags (eol escapes)
		{
		case 0: return strconv_pcdata_impl<opt2_to_type<0, 0> >::parse;
		case 1: return strconv_pcdata_impl<opt2_to_type<0, 1> >::parse;
		case 2: return strconv_pcdata_impl<opt2_to_type<1, 0> >::parse;
		case 3: return strconv_pcdata_impl<opt2_to_type<1, 1> >::parse;
		default: return 0; // should not get here
		}
	}

	typedef char_t* (*strconv_attribute_t)(char_t*, char_t);
	
	template <typename opt4> struct strconv_attribute_impl
	{
		static char_t* parse(char_t* s, char_t end_quote)
		{
			const bool opt_wconv = opt4::o1;
			const bool opt_wnorm = opt4::o2;
			const bool opt_eol = opt4::o3;
			const bool opt_escape = opt4::o4;

			gap g;

			// trim leading whitespaces
			if (opt_wnorm && is_chartype(*s, ct_space))
			{
				char_t* str = s;
				
				do ++str;
				while (is_chartype(*str, ct_space));
				
				g.push(s, str - s);
			}

			while (true)
			{
				while (!is_chartype(*s, (opt_wnorm || opt_wconv) ? ct_parse_attr_ws : ct_parse_attr)) ++s;
				
				if (*s == end_quote)
				{
					char_t* str = g.flush(s);
					
					if (opt_wnorm)
					{
						do *str-- = 0;
						while (is_chartype(*str, ct_space));
					}
					else *str = 0;
				
					return s + 1;
				}
				else if (opt_wnorm && is_chartype(*s, ct_space))
				{
					*s++ = ' ';
		
					if (is_chartype(*s, ct_space))
					{
						char_t* str = s + 1;
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
	};

	strconv_attribute_t get_strconv_attribute(unsigned int optmask)
	{
		STATIC_ASSERT(parse_escapes == 0x10 && parse_eol == 0x20 && parse_wconv_attribute == 0x80);
		
		switch ((optmask >> 4) & 15) // get bitmask for flags (wconv wnorm eol escapes)
		{
		case 0:  return strconv_attribute_impl<opt4_to_type<0, 0, 0, 0> >::parse;
		case 1:  return strconv_attribute_impl<opt4_to_type<0, 0, 0, 1> >::parse;
		case 2:  return strconv_attribute_impl<opt4_to_type<0, 0, 1, 0> >::parse;
		case 3:  return strconv_attribute_impl<opt4_to_type<0, 0, 1, 1> >::parse;
		case 4:  return strconv_attribute_impl<opt4_to_type<0, 1, 0, 0> >::parse;
		case 5:  return strconv_attribute_impl<opt4_to_type<0, 1, 0, 1> >::parse;
		case 6:  return strconv_attribute_impl<opt4_to_type<0, 1, 1, 0> >::parse;
		case 7:  return strconv_attribute_impl<opt4_to_type<0, 1, 1, 1> >::parse;
		case 8:  return strconv_attribute_impl<opt4_to_type<1, 0, 0, 0> >::parse;
		case 9:  return strconv_attribute_impl<opt4_to_type<1, 0, 0, 1> >::parse;
		case 10: return strconv_attribute_impl<opt4_to_type<1, 0, 1, 0> >::parse;
		case 11: return strconv_attribute_impl<opt4_to_type<1, 0, 1, 1> >::parse;
		case 12: return strconv_attribute_impl<opt4_to_type<1, 1, 0, 0> >::parse;
		case 13: return strconv_attribute_impl<opt4_to_type<1, 1, 0, 1> >::parse;
		case 14: return strconv_attribute_impl<opt4_to_type<1, 1, 1, 0> >::parse;
		case 15: return strconv_attribute_impl<opt4_to_type<1, 1, 1, 1> >::parse;
		default: return 0; // should not get here
		}
	}

	inline xml_parse_result make_parse_result(xml_parse_status status, ptrdiff_t offset, unsigned int line)
	{
		xml_parse_result result = {status, offset, line, encoding_auto};
		return result;
	}

	#define MAKE_PARSE_RESULT(status) make_parse_result(status, 0, __LINE__)

	struct xml_parser
	{
		xml_allocator alloc;
		
		// Parser utilities.
		#define SKIPWS()			{ while (is_chartype(*s, ct_space)) ++s; }
		#define OPTSET(OPT)			( optmsk & OPT )
		#define PUSHNODE(TYPE)		{ cursor = append_node(cursor, alloc, TYPE); }
		#define POPNODE()			{ cursor = cursor->parent; }
		#define SCANFOR(X)			{ while (*s != 0 && !(X)) ++s; }
		#define SCANWHILE(X)		{ while ((X)) ++s; }
		#define ENDSEG()			{ ch = *s; *s = 0; ++s; }
		#define THROW_ERROR(err, m)	return make_parse_result(err, m - buffer_start, __LINE__)
		#define CHECK_ERROR(err, m)	{ if (*s == 0) THROW_ERROR(err, m); }
		
		xml_parser(const xml_allocator& alloc): alloc(alloc)
		{
		}

		// DOCTYPE consists of nested sections of the following possible types:
		// <!-- ... -->, <? ... ?>, "...", '...'
		// <![...]]>
		// <!...>
		// First group can not contain nested groups
		// Second group can contain nested groups of the same type
		// Third group can contain all other groups
		xml_parse_result parse_doctype_primitive(char_t*& s, char_t* buffer_start)
		{
			if (*s == '"' || *s == '\'')
			{
				// quoted string
				char_t ch = *s++;
				SCANFOR(*s == ch);
				if (!*s) THROW_ERROR(status_bad_doctype, s);

				s++;
			}
			else if (s[0] == '<' && s[1] == '?')
			{
				// <? ... ?>
				s += 2;
				SCANFOR(s[0] == '?' && s[1] == '>'); // no need for ENDSWITH because ?> can't terminate proper doctype
				if (!*s) THROW_ERROR(status_bad_doctype, s);

				s += 2;
			}
			else if (s[0] == '<' && s[1] == '!' && s[2] == '-' && s[3] == '-')
			{
				s += 4;
				SCANFOR(s[0] == '-' && s[1] == '-' && s[2] == '>'); // no need for ENDSWITH because --> can't terminate proper doctype
				if (!*s) THROW_ERROR(status_bad_doctype, s);

				s += 4;
			}
			else THROW_ERROR(status_bad_doctype, s);

			THROW_ERROR(status_ok, s);
		}

		xml_parse_result parse_doctype_ignore(char_t*& s, char_t* buffer_start)
		{
			assert(s[0] == '<' && s[1] == '!' && s[2] == '[');
			s++;

			while (*s)
			{
				if (s[0] == '<' && s[1] == '!' && s[2] == '[')
				{
					// nested ignore section
					xml_parse_result res = parse_doctype_ignore(s, buffer_start);

					if (!res) return res;
				}
				else if (s[0] == ']' && s[1] == ']' && s[2] == '>')
				{
					// ignore section end
					s += 3;

					THROW_ERROR(status_ok, s);
				}
				else s++;
			}

			THROW_ERROR(status_bad_doctype, s);
		}

		xml_parse_result parse_doctype(char_t*& s, char_t* buffer_start, char_t endch, bool toplevel)
		{
			assert(s[0] == '<' && s[1] == '!');
			s++;

			while (*s)
			{
				if (s[0] == '<' && s[1] == '!' && s[2] != '-')
				{
					if (s[2] == '[')
					{
						// ignore
						xml_parse_result res = parse_doctype_ignore(s, buffer_start);

						if (!res) return res;
					}
					else
					{
						// some control group
						xml_parse_result res = parse_doctype(s, buffer_start, endch, false);

						if (!res) return res;
					}
				}
				else if (s[0] == '<' || s[0] == '"' || s[0] == '\'')
				{
					// unknown tag (forbidden), or some primitive group
					xml_parse_result res = parse_doctype_primitive(s, buffer_start);

					if (!res) return res;
				}
				else if (*s == '>')
				{
					s++;

					THROW_ERROR(status_ok, s);
				}
				else s++;
			}

			THROW_ERROR((toplevel && endch == '>') ? status_ok : status_bad_doctype, s);
		}

		xml_parse_result parse_exclamation(char_t*& ref_s, xml_node_struct* cursor, unsigned int optmsk, char_t* buffer_start, char_t endch)
		{
			// load into registers
			char_t* s = ref_s;

			// parse node contents, starting with exclamation mark
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
						s = strconv_comment(s, endch);

						if (!s) THROW_ERROR(status_bad_comment, cursor->value);
					}
					else
					{
						// Scan for terminating '-->'.
						SCANFOR(s[0] == '-' && s[1] == '-' && ENDSWITH(s[2], '>'));
						CHECK_ERROR(status_bad_comment, s);

						if (OPTSET(parse_comments))
							*s = 0; // Zero-terminate this segment at the first terminating '-'.

						s += (s[2] == '>' ? 3 : 2); // Step over the '\0->'.
					}

					if (OPTSET(parse_comments))
					{
						POPNODE(); // Pop since this is a standalone.
					}
				}
				else THROW_ERROR(status_bad_comment, s);
			}
			else if (*s == '[')
			{
				// '<![CDATA[...'
				if (*++s=='C' && *++s=='D' && *++s=='A' && *++s=='T' && *++s=='A' && *++s == '[')
				{
					++s;

					if (OPTSET(parse_cdata))
					{
						PUSHNODE(node_cdata); // Append a new node on the tree.
						cursor->value = s; // Save the offset.

						if (OPTSET(parse_eol))
						{
							s = strconv_cdata(s, endch);

							if (!s) THROW_ERROR(status_bad_cdata, cursor->value);
						}
						else
						{
							// Scan for terminating ']]>'.
							SCANFOR(s[0] == ']' && s[1] == ']' && ENDSWITH(s[2], '>'));
							CHECK_ERROR(status_bad_cdata, s);

							*s++ = 0; // Zero-terminate this segment.
						}

						POPNODE(); // Pop since this is a standalone.
					}
					else // Flagged for discard, but we still have to scan for the terminator.
					{
						// Scan for terminating ']]>'.
						SCANFOR(s[0] == ']' && s[1] == ']' && ENDSWITH(s[2], '>'));
						CHECK_ERROR(status_bad_cdata, s);

						++s;
					}

					s += (s[1] == '>' ? 2 : 1); // Step over the last ']>'.
				}
				else THROW_ERROR(status_bad_cdata, s);
			}
			else if (s[0] == 'D' && s[1] == 'O' && s[2] == 'C' && s[3] == 'T' && s[4] == 'Y' && s[5] == 'P' && ENDSWITH(s[6], 'E'))
			{
				if (s[6] != 'E') THROW_ERROR(status_bad_doctype, s);

				s -= 2;

				xml_parse_result res = parse_doctype(s, buffer_start, endch, true);

				if (!res) return res;
			}
			else if (*s == 0 && endch == '-') THROW_ERROR(status_bad_comment, s);
			else if (*s == 0 && endch == '[') THROW_ERROR(status_bad_cdata, s);
			else THROW_ERROR(status_unrecognized_tag, s);

			// store from registers
			ref_s = s;

			THROW_ERROR(status_ok, s);
		}

		xml_parse_result parse_question(char_t*& ref_s, xml_node_struct*& ref_cursor, unsigned int optmsk, char_t* buffer_start, char_t endch)
		{
			// load into registers
			char_t* s = ref_s;
			xml_node_struct* cursor = ref_cursor;
			char_t ch = 0;

			// parse node contents, starting with question mark
			++s;

			if (!is_chartype(*s, ct_start_symbol)) // bad PI
				THROW_ERROR(status_bad_pi, s);
			else if (OPTSET(parse_pi) || OPTSET(parse_declaration))
			{
				char_t* mark = s;
				SCANWHILE(is_chartype(*s, ct_symbol)); // Read PI target
				CHECK_ERROR(status_bad_pi, s);

				if (!is_chartype(*s, ct_space) && *s != '?') // Target has to end with space or ?
					THROW_ERROR(status_bad_pi, s);

				ENDSEG();
				if (*s == 0 && endch != '>') THROW_ERROR(status_bad_pi, s);

				if (ch == '?') // nothing except target present
				{
					if (!ENDSWITH(*s, '>')) THROW_ERROR(status_bad_pi, s);
					s += (*s == '>');

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

						SCANFOR(s[0] == '?' && ENDSWITH(s[1], '>')); // Look for '?>'.
						CHECK_ERROR(status_bad_pi, s);

						// replace ending ? with / to terminate properly
						*s = '/';

						// parse attributes
						s = mark;

						// we exit from this function with cursor at node_declaration, which is a signal to parse() to go to LOC_ATTRIBUTES
					}
				}
				else
				{
					if (OPTSET(parse_pi))
					{
						PUSHNODE(node_pi); // Append a new node on the tree.

						cursor->name = mark;
					}

					// ch is a whitespace character, skip whitespaces
					SKIPWS();
					CHECK_ERROR(status_bad_pi, s);

					mark = s;

					SCANFOR(s[0] == '?' && ENDSWITH(s[1], '>')); // Look for '?>'.
					CHECK_ERROR(status_bad_pi, s);

					ENDSEG();

					s += (*s == '>'); // Step over >

					if (OPTSET(parse_pi))
					{
						cursor->value = mark;

						POPNODE();
					}
				}
			}
			else // not parsing PI
			{
				SCANFOR(s[0] == '?' && ENDSWITH(s[1], '>')); // Look for '?>'.
				CHECK_ERROR(status_bad_pi, s);

				s += (s[1] == '>' ? 2 : 1);
			}

			// store from registers
			ref_s = s;
			ref_cursor = cursor;

			THROW_ERROR(status_ok, s);
		}

		xml_parse_result parse(char_t* s, xml_node_struct* xmldoc, unsigned int optmsk, char_t endch)
		{
			strconv_attribute_t strconv_attribute = get_strconv_attribute(optmsk);
			strconv_pcdata_t strconv_pcdata = get_strconv_pcdata(optmsk);
			
			char_t* buffer_start = s;

			char_t ch = 0;
			xml_node_struct* cursor = xmldoc;
			char_t* mark = s;

			while (*s != 0)
			{
				if (*s == '<')
				{
					++s;

				LOC_TAG:
					if (is_chartype(*s, ct_start_symbol)) // '<#...'
					{
						PUSHNODE(node_element); // Append a new node to the tree.

						cursor->name = s;

						SCANWHILE(is_chartype(*s, ct_symbol)); // Scan for a terminator.
						ENDSEG(); // Save char in 'ch', terminate & step over.

						if (ch == '>')
						{
							// end of tag
						}
						else if (is_chartype(ch, ct_space))
						{
						LOC_ATTRIBUTES:
						    while (true)
						    {
								SKIPWS(); // Eat any whitespace.
						
								if (is_chartype(*s, ct_start_symbol)) // <... #...
								{
									xml_attribute_struct* a = append_attribute_ll(cursor, alloc); // Make space for this attribute.
									a->name = s; // Save the offset.

									SCANWHILE(is_chartype(*s, ct_symbol)); // Scan for a terminator.
									CHECK_ERROR(status_bad_attribute, s);

									ENDSEG(); // Save char in 'ch', terminate & step over.
									CHECK_ERROR(status_bad_attribute, s);

									if (is_chartype(ch, ct_space))
									{
										SKIPWS(); // Eat any whitespace.
										CHECK_ERROR(status_bad_attribute, s);

										ch = *s;
										++s;
									}
									
									if (ch == '=') // '<... #=...'
									{
										SKIPWS(); // Eat any whitespace.

										if (*s == '"' || *s == '\'') // '<... #="...'
										{
											ch = *s; // Save quote char to avoid breaking on "''" -or- '""'.
											++s; // Step over the quote.
											a->value = s; // Save the offset.

											s = strconv_attribute(s, ch);
										
											if (!s) THROW_ERROR(status_bad_attribute, a->value);

											// After this line the loop continues from the start;
											// Whitespaces, / and > are ok, symbols and EOF are wrong,
											// everything else will be detected
											if (is_chartype(*s, ct_start_symbol)) THROW_ERROR(status_bad_attribute, s);
										}
										else THROW_ERROR(status_bad_attribute, s);
									}
									else THROW_ERROR(status_bad_attribute, s);
								}
								else if (*s == '/')
								{
									++s;
									
									if (*s == '>')
									{
										POPNODE();
										s++;
										break;
									}
									else if (*s == 0 && endch == '>')
									{
										POPNODE();
										break;
									}
									else THROW_ERROR(status_bad_start_element, s);
								}
								else if (*s == '>')
								{
									++s;

									break;
								}
								else if (*s == 0 && endch == '>')
								{
									break;
								}
								else THROW_ERROR(status_bad_start_element, s);
							}

							// !!!
						}
						else if (ch == '/') // '<#.../'
						{
							if (!ENDSWITH(*s, '>')) THROW_ERROR(status_bad_start_element, s);

							POPNODE(); // Pop.

							s += (*s == '>');
						}
						else if (ch == 0)
						{
							// we stepped over null terminator, backtrack & handle closing tag
							--s;
							
							if (endch != '>') THROW_ERROR(status_bad_start_element, s);
						}
						else THROW_ERROR(status_bad_start_element, s);
					}
					else if (*s == '/')
					{
						++s;

						char_t* name = cursor->name;
						if (!name) THROW_ERROR(status_end_element_mismatch, s);
						
						while (is_chartype(*s, ct_symbol))
						{
							if (*s++ != *name++) THROW_ERROR(status_end_element_mismatch, s);
						}

						if (*name)
						{
							if (*s == 0 && name[0] == endch && name[1] == 0) THROW_ERROR(status_bad_end_element, s);
							else THROW_ERROR(status_end_element_mismatch, s);
						}
							
						POPNODE(); // Pop.

						SKIPWS();

						if (*s == 0)
						{
							if (endch != '>') THROW_ERROR(status_bad_end_element, s);
						}
						else
						{
							if (*s != '>') THROW_ERROR(status_bad_end_element, s);
							++s;
						}
					}
					else if (*s == '?') // '<?...'
					{
						xml_parse_result quest_result = parse_question(s, cursor, optmsk, buffer_start, endch);

						if (!quest_result) return quest_result;

						if (cursor && (cursor->header & xml_memory_page_type_mask) == node_declaration) goto LOC_ATTRIBUTES;
					}
					else if (*s == '!') // '<!...'
					{
						xml_parse_result excl_result = parse_exclamation(s, cursor, optmsk, buffer_start, endch);

						if (!excl_result) return excl_result;
					}
					else if (*s == 0 && endch == '?') THROW_ERROR(status_bad_pi, s);
					else THROW_ERROR(status_unrecognized_tag, s);
				}
				else
				{
					mark = s; // Save this offset while searching for a terminator.

					SKIPWS(); // Eat whitespace if no genuine PCDATA here.

					if ((!OPTSET(parse_ws_pcdata) || mark == s) && (*s == '<' || !*s))
					{
						continue;
					}

					s = mark;
							
					if (cursor->parent)
					{
						PUSHNODE(node_pcdata); // Append a new node on the tree.
						cursor->value = s; // Save the offset.

						s = strconv_pcdata(s);
								
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

			// check that last tag is closed
			if (cursor != xmldoc) THROW_ERROR(status_end_element_mismatch, s);
			
			THROW_ERROR(status_ok, s);
		}

		static xml_parse_result parse(char_t* buffer, size_t length, xml_node_struct* xmldoc, unsigned int optmsk)
		{
			// store buffer for offset_debug
			static_cast<xml_document_struct*>(xmldoc)->buffer = buffer;

			// early-out for empty documents
			if (length == 0) return MAKE_PARSE_RESULT(status_ok);

			// create parser on stack
			xml_allocator& alloc = static_cast<xml_document_struct*>(xmldoc)->allocator;
			
			xml_parser parser(alloc);

			// save last character and make buffer zero-terminated (speeds up parsing)
			char_t endch = buffer[length - 1];
			buffer[length - 1] = 0;
			
			// perform actual parsing
			xml_parse_result result = parser.parse(buffer, xmldoc, optmsk, endch);

			// update allocator state
			alloc = parser.alloc;

			// since we removed last character, we have to handle the only possible false positive
			if (result && endch == '<')
			{
				char_t* buffer_start = buffer;

				// there's no possible well-formed document with < at the end
				THROW_ERROR(status_unrecognized_tag, buffer_start + length);
			}

			return result;
		}
	};

	// Output facilities
	encoding_t get_write_native_encoding()
	{
	#ifdef PUGIXML_WCHAR_MODE
		return get_wchar_encoding();
	#else
		return encoding_utf8;
	#endif
	}

	encoding_t get_write_encoding(encoding_t encoding)
	{
		// replace wchar encoding with utf implementation
		if (encoding == encoding_wchar) return get_wchar_encoding();

		// replace utf16 encoding with utf16 with specific endianness
		if (encoding == encoding_utf16) return is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

		// replace utf32 encoding with utf32 with specific endianness
		if (encoding == encoding_utf32) return is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

		// only do autodetection if no explicit encoding is requested
		if (encoding != encoding_auto) return encoding;

		// assume utf8 encoding
		return encoding_utf8;
	}

#ifdef PUGIXML_WCHAR_MODE
	size_t get_valid_length(const char_t* data, size_t length)
	{
		assert(length > 0);

		// discard last character if it's the lead of a surrogate pair 
		return (sizeof(wchar_t) == 2 && (unsigned)(static_cast<impl::char16_t>(data[length - 1]) - 0xD800) < 0x400) ? length - 1 : length;
	}

	size_t convert_buffer(char* result, const char_t* data, size_t length, encoding_t encoding)
	{
		// only endian-swapping is required
		if (need_endian_swap_utf(encoding, get_wchar_encoding()))
		{
			impl::convert_wchar_endian_swap(reinterpret_cast<char_t*>(result), data, length);

			return length * sizeof(char_t);
		}
	
		// convert to utf8
		if (encoding == encoding_utf8)
		{
			impl::char8_t* dest = reinterpret_cast<impl::char8_t*>(result);

			impl::char8_t* end = sizeof(wchar_t) == 2 ?
				impl::decode_utf16_block<impl::utf8_writer>(reinterpret_cast<const impl::char16_t*>(data), length, dest, opt1_to_type<false>()) :
				impl::decode_utf32_block<impl::utf8_writer>(reinterpret_cast<const impl::char32_t*>(data), length, dest, opt1_to_type<false>());

			return static_cast<size_t>(end - dest);
		}

		// convert to utf16
		if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
		{
			impl::char16_t* dest = reinterpret_cast<impl::char16_t*>(result);

			// convert to native utf16
			impl::char16_t* end = impl::decode_utf32_block<impl::utf16_writer>(reinterpret_cast<const impl::char32_t*>(data), length, dest, opt1_to_type<false>());

			// swap if necessary
			encoding_t native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

			if (native_encoding != encoding) impl::convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

			return static_cast<size_t>(end - dest) * sizeof(impl::char16_t);
		}

		// convert to utf32
		if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
		{
			impl::char32_t* dest = reinterpret_cast<impl::char32_t*>(result);

			// convert to native utf32
			impl::char32_t* end = impl::decode_utf16_block<impl::utf32_writer>(reinterpret_cast<const impl::char16_t*>(data), length, dest, opt1_to_type<false>());

			// swap if necessary
			encoding_t native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

			if (native_encoding != encoding) impl::convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

			return static_cast<size_t>(end - dest) * sizeof(impl::char32_t);
		}

		// invalid encoding combination (this can't happen)
		assert(false);

		return 0;
	}
#else
	size_t get_valid_length(const char_t* data, size_t length)
	{
		assert(length > 4);

		for (size_t i = 1; i <= 4; ++i)
		{
			impl::char8_t ch = static_cast<impl::char8_t>(data[length - i]);

			// either a standalone character or a leading one
			if ((ch & 0xc0) != 0x80) return length - i;
		}

		// there are four non-leading characters at the end, sequence tail is broken so might as well process the whole chunk
		return length;
	}

	size_t convert_buffer(char* result, const char_t* data, size_t length, encoding_t encoding)
	{
		if (encoding == encoding_utf16_be || encoding == encoding_utf16_le)
		{
			impl::char16_t* dest = reinterpret_cast<impl::char16_t*>(result);

			// convert to native utf16
			impl::char16_t* end = impl::decode_utf8_block<impl::utf16_writer>(reinterpret_cast<const impl::char8_t*>(data), length, dest);

			// swap if necessary
			encoding_t native_encoding = is_little_endian() ? encoding_utf16_le : encoding_utf16_be;

			if (native_encoding != encoding) impl::convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

			return static_cast<size_t>(end - dest) * sizeof(impl::char16_t);
		}

		if (encoding == encoding_utf32_be || encoding == encoding_utf32_le)
		{
			impl::char32_t* dest = reinterpret_cast<impl::char32_t*>(result);

			// convert to native utf32
			impl::char32_t* end = impl::decode_utf8_block<impl::utf32_writer>(reinterpret_cast<const impl::char8_t*>(data), length, dest);

			// swap if necessary
			encoding_t native_encoding = is_little_endian() ? encoding_utf32_le : encoding_utf32_be;

			if (native_encoding != encoding) impl::convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

			return static_cast<size_t>(end - dest) * sizeof(impl::char32_t);
		}

		// invalid encoding combination (this can't happen)
		assert(false);

		return 0;
	}
#endif

	class xml_buffered_writer
	{
		xml_buffered_writer(const xml_buffered_writer&);
		xml_buffered_writer& operator=(const xml_buffered_writer&);

	public:
		xml_buffered_writer(xml_writer& writer, encoding_t user_encoding): writer(writer), bufsize(0), encoding(get_write_encoding(user_encoding))
		{
		}

		~xml_buffered_writer()
		{
			flush();
		}

		void flush()
		{
			flush(buffer, bufsize);
			bufsize = 0;
		}

		void flush(const char_t* data, size_t size)
		{
			if (size == 0) return;

			// fast path, just write data
			if (encoding == get_write_native_encoding())
				writer.write(data, size * sizeof(char_t));
			else
			{
				// convert chunk
				size_t result = convert_buffer(scratch, data, size, encoding);
				assert(result <= sizeof(scratch));

				// write data
				writer.write(scratch, result);
			}
		}

		void write(const char_t* data, size_t length)
		{
			if (bufsize + length > bufcapacity)
			{
				// flush the remaining buffer contents
				flush();

				// handle large chunks
				if (length > bufcapacity)
				{
					if (encoding == get_write_native_encoding())
					{
						// fast path, can just write data chunk
						writer.write(data, length * sizeof(char_t));
						return;
					}

					// need to convert in suitable chunks
					while (length > bufcapacity)
					{
						// get chunk size by selecting such number of characters that are guaranteed to fit into scratch buffer
						// and form a complete codepoint sequence (i.e. discard start of last codepoint if necessary)
						size_t chunk_size = get_valid_length(data, bufcapacity);

						// convert chunk and write
						flush(data, chunk_size);

						// iterate
						data += chunk_size;
						length -= chunk_size;
					}

					// small tail is copied below
					bufsize = 0;
				}
			}

			memcpy(buffer + bufsize, data, length * sizeof(char_t));
			bufsize += length;
		}

		void write(const char_t* data)
		{
			write(data, impl::strlen(data));
		}

		void write(char_t d0)
		{
			if (bufsize + 1 > bufcapacity) flush();

			buffer[bufsize + 0] = d0;
			bufsize += 1;
		}

		void write(char_t d0, char_t d1)
		{
			if (bufsize + 2 > bufcapacity) flush();

			buffer[bufsize + 0] = d0;
			buffer[bufsize + 1] = d1;
			bufsize += 2;
		}

		void write(char_t d0, char_t d1, char_t d2)
		{
			if (bufsize + 3 > bufcapacity) flush();

			buffer[bufsize + 0] = d0;
			buffer[bufsize + 1] = d1;
			buffer[bufsize + 2] = d2;
			bufsize += 3;
		}

		void write(char_t d0, char_t d1, char_t d2, char_t d3)
		{
			if (bufsize + 4 > bufcapacity) flush();

			buffer[bufsize + 0] = d0;
			buffer[bufsize + 1] = d1;
			buffer[bufsize + 2] = d2;
			buffer[bufsize + 3] = d3;
			bufsize += 4;
		}

		void write(char_t d0, char_t d1, char_t d2, char_t d3, char_t d4)
		{
			if (bufsize + 5 > bufcapacity) flush();

			buffer[bufsize + 0] = d0;
			buffer[bufsize + 1] = d1;
			buffer[bufsize + 2] = d2;
			buffer[bufsize + 3] = d3;
			buffer[bufsize + 4] = d4;
			bufsize += 5;
		}

		void write(char_t d0, char_t d1, char_t d2, char_t d3, char_t d4, char_t d5)
		{
			if (bufsize + 6 > bufcapacity) flush();

			buffer[bufsize + 0] = d0;
			buffer[bufsize + 1] = d1;
			buffer[bufsize + 2] = d2;
			buffer[bufsize + 3] = d3;
			buffer[bufsize + 4] = d4;
			buffer[bufsize + 5] = d5;
			bufsize += 6;
		}

		// utf8 maximum expansion: x4 (-> utf32)
		// utf16 maximum expansion: x2 (-> utf32)
		// utf32 maximum expansion: x1
		enum { bufcapacity = 2048 };

		char_t buffer[bufcapacity];
		char scratch[4 * bufcapacity];

		xml_writer& writer;
		size_t bufsize;
		encoding_t encoding;
	};

	void write_bom(xml_writer& writer, encoding_t encoding)
	{
		switch (encoding)
		{
		case encoding_utf8:
			writer.write("\xef\xbb\xbf", 3);
			break;

		case encoding_utf16_be:
			writer.write("\xfe\xff", 2);
			break;

		case encoding_utf16_le:
			writer.write("\xff\xfe", 2);
			break;

		case encoding_utf32_be:
			writer.write("\x00\x00\xfe\xff", 4);
			break;

		case encoding_utf32_le:
			writer.write("\xff\xfe\x00\x00", 4);
			break;

		default:
			// invalid encoding (this should not happen)
			assert(false);
		}
	}

	void text_output_escaped(xml_buffered_writer& writer, const char_t* s, output_chartype_t type)
	{
		while (*s)
		{
			const char_t* prev = s;
			
			// While *s is a usual symbol
			while (!is_output_chartype(*s, type)) ++s;
		
			writer.write(prev, static_cast<size_t>(s - prev));

			switch (*s)
			{
				case 0: break;
				case '&':
					writer.write('&', 'a', 'm', 'p', ';');
					++s;
					break;
				case '<':
					writer.write('&', 'l', 't', ';');
					++s;
					break;
				case '>':
					writer.write('&', 'g', 't', ';');
					++s;
					break;
				case '"':
					writer.write('&', 'q', 'u', 'o', 't', ';');
					++s;
					break;
				default: // s is not a usual symbol
				{
					unsigned int ch = static_cast<unsigned int>(*s++);
					assert(ch < 32);

					writer.write('&', '#', static_cast<char_t>((ch / 10) + '0'), static_cast<char_t>((ch % 10) + '0'), ';');
				}
			}
		}
	}

	void node_output(xml_buffered_writer& writer, const xml_node& node, const char_t* indent, unsigned int flags, unsigned int depth)
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
				writer.write('=', '"');

				text_output_escaped(writer, a.value(), oct_special_attr);

				writer.write('"');
			}

			if (flags & format_raw)
			{
				if (!node.first_child())
					writer.write(' ', '/', '>');
				else
				{
					writer.write('>');

					for (xml_node n = node.first_child(); n; n = n.next_sibling())
						node_output(writer, n, indent, flags, depth + 1);

					writer.write('<', '/');
					writer.write(node.name());
					writer.write('>');
				}
			}
			else if (!node.first_child())
				writer.write(' ', '/', '>', '\n');
			else if (node.first_child() == node.last_child() && node.first_child().type() == node_pcdata)
			{
				writer.write('>');

				text_output_escaped(writer, node.first_child().value(), oct_special_pcdata);

				writer.write('<', '/');
				writer.write(node.name());
				writer.write('>', '\n');
			}
			else
			{
				writer.write('>', '\n');
				
				for (xml_node n = node.first_child(); n; n = n.next_sibling())
					node_output(writer, n, indent, flags, depth + 1);

				if ((flags & format_indent) != 0 && (flags & format_raw) == 0)
					for (unsigned int i = 0; i < depth; ++i) writer.write(indent);
				
				writer.write('<', '/');
				writer.write(node.name());
				writer.write('>', '\n');
			}

			break;
		}
		
		case node_pcdata:
			text_output_escaped(writer, node.value(), oct_special_pcdata);
			if ((flags & format_raw) == 0) writer.write('\n');
			break;

		case node_cdata:
			writer.write('<', '!', '[', 'C', 'D');
			writer.write('A', 'T', 'A', '[');
			writer.write(node.value());
			writer.write(']', ']', '>');
			if ((flags & format_raw) == 0) writer.write('\n');
			break;

		case node_comment:
			writer.write('<', '!', '-', '-');
			writer.write(node.value());
			writer.write('-', '-', '>');
			if ((flags & format_raw) == 0) writer.write('\n');
			break;

		case node_pi:
			writer.write('<', '?');
			writer.write(node.name());
			if (node.value()[0])
			{
				writer.write(' ');
				writer.write(node.value());
			}
			writer.write('?', '>');
			if ((flags & format_raw) == 0) writer.write('\n');
			break;
		
		case node_declaration:
		{
			writer.write('<', '?');
			writer.write(node.name());

			for (xml_attribute a = node.first_attribute(); a; a = a.next_attribute())
			{
				writer.write(' ');
				writer.write(a.name());
				writer.write('=', '"');

				text_output_escaped(writer, a.value(), oct_special_attr);

				writer.write('"');
			}

			writer.write('?', '>');
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
			dest.set_value(source.value());
			break;

		case node_pi:
			dest.set_name(source.name());
			dest.set_value(source.value());
			break;

		case node_declaration:
		{
			dest.set_name(source.name());

			for (xml_attribute a = source.first_attribute(); a; a = a.next_attribute())
				dest.append_attribute(a.name()).set_value(a.value());

			break;
		}

		default:
			assert(false);
		}
	}

#ifndef PUGIXML_NO_STL
	template <typename T> xml_parse_result load_stream_impl(xml_document& doc, std::basic_istream<T, std::char_traits<T> >& stream, unsigned int options, encoding_t encoding)
	{
		if (!stream.good()) return MAKE_PARSE_RESULT(status_io_error);

		// get length of remaining data in stream
		std::streamoff pos = stream.tellg();
		stream.seekg(0, std::ios::end);
		std::streamoff length = stream.tellg() - pos;
		stream.seekg(pos, std::ios::beg);

		if (!stream.good() || pos < 0 || length < 0) return MAKE_PARSE_RESULT(status_io_error);

		// read stream data into memory
		size_t read_length = static_cast<size_t>(length);

		T* s = static_cast<T*>(global_allocate((read_length > 0 ? read_length : 1) * sizeof(T)));
		if (!s) return MAKE_PARSE_RESULT(status_out_of_memory);

		stream.read(s, static_cast<std::streamsize>(read_length));

		// check for errors
		size_t actual_length = static_cast<size_t>(stream.gcount());
		assert(actual_length <= read_length);

		if (read_length > 0 && actual_length == 0)
		{
			global_deallocate(s);
			return MAKE_PARSE_RESULT(status_io_error);
		}

		// load data from buffer
		return doc.load_buffer_inplace_own(s, actual_length * sizeof(T), options, encoding);
	}
#endif
}

namespace pugi
{
	xml_writer_file::xml_writer_file(void* file): file(file)
	{
	}

	void xml_writer_file::write(const void* data, size_t size)
	{
		fwrite(data, size, 1, static_cast<FILE*>(file));
	}

#ifndef PUGIXML_NO_STL
	xml_writer_stream::xml_writer_stream(std::basic_ostream<char, std::char_traits<char> >& stream): narrow_stream(&stream), wide_stream(0)
	{
	}

	xml_writer_stream::xml_writer_stream(std::basic_ostream<wchar_t, std::char_traits<wchar_t> >& stream): narrow_stream(0), wide_stream(&stream)
	{
	}

	void xml_writer_stream::write(const void* data, size_t size)
	{
		if (narrow_stream)
		{
			assert(!wide_stream);
			narrow_stream->write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(size));
		}
		else
		{
			assert(wide_stream);
			assert(size % sizeof(wchar_t) == 0);

			wide_stream->write(reinterpret_cast<const wchar_t*>(data), static_cast<std::streamsize>(size / sizeof(wchar_t)));
		}
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

	xml_attribute::operator xml_attribute::unspecified_bool_type() const
	{
#ifdef __MWERKS__
      	return _attr ? &xml_attribute::empty : 0;
#else
      	return _attr ? &xml_attribute::_attr : 0;
#endif
   	}

   	bool xml_attribute::operator!() const
   	{
   		return !_attr;
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
    	return _attr && _attr->prev_attribute_c->next_attribute ? xml_attribute(_attr->prev_attribute_c) : xml_attribute();
    }

	int xml_attribute::as_int() const
	{
		if (!_attr || !_attr->value) return 0;

	#ifdef PUGIXML_WCHAR_MODE
		return (int)wcstol(_attr->value, 0, 10);
	#else
		return atoi(_attr->value);
	#endif
	}

	unsigned int xml_attribute::as_uint() const
	{
		if (!_attr || !_attr->value) return 0;

	#ifdef PUGIXML_WCHAR_MODE
		int result = (int)wcstol(_attr->value, 0, 10);
	#else
		int result = atoi(_attr->value);
	#endif

		return result < 0 ? 0 : static_cast<unsigned int>(result);
	}

	double xml_attribute::as_double() const
	{
		if (!_attr || !_attr->value) return 0;

	#ifdef PUGIXML_WCHAR_MODE
		return wcstod(_attr->value, 0);
	#else
		return atof(_attr->value);
	#endif
	}

	float xml_attribute::as_float() const
	{
		if (!_attr || !_attr->value) return 0;

	#ifdef PUGIXML_WCHAR_MODE
		return (float)wcstod(_attr->value, 0);
	#else
		return (float)atof(_attr->value);
	#endif
	}

	bool xml_attribute::as_bool() const
	{
		if (!_attr || !_attr->value) return false;

		// only look at first char
		char_t first = *_attr->value;

		// 1*, t* (true), T* (True), y* (yes), Y* (YES)
		return (first == '1' || first == 't' || first == 'T' || first == 'y' || first == 'Y');
	}

	bool xml_attribute::empty() const
	{
		return !_attr;
	}

	const char_t* xml_attribute::name() const
	{
		return (_attr && _attr->name) ? _attr->name : PUGIXML_TEXT("");
	}

	const char_t* xml_attribute::value() const
	{
		return (_attr && _attr->value) ? _attr->value : PUGIXML_TEXT("");
	}

	unsigned int xml_attribute::document_order() const
	{
		return 0;
	}

	xml_attribute& xml_attribute::operator=(const char_t* rhs)
	{
		set_value(rhs);
		return *this;
	}
	
	xml_attribute& xml_attribute::operator=(int rhs)
	{
		set_value(rhs);
		return *this;
	}

	xml_attribute& xml_attribute::operator=(unsigned int rhs)
	{
		set_value(rhs);
		return *this;
	}

	xml_attribute& xml_attribute::operator=(double rhs)
	{
		set_value(rhs);
		return *this;
	}
	
	xml_attribute& xml_attribute::operator=(bool rhs)
	{
		set_value(rhs);
		return *this;
	}

	bool xml_attribute::set_name(const char_t* rhs)
	{
		if (!_attr) return false;
		
		return strcpy_insitu(_attr->name, _attr->header, xml_memory_page_name_allocated_mask, rhs);
	}
		
	bool xml_attribute::set_value(const char_t* rhs)
	{
		if (!_attr) return false;

		return strcpy_insitu(_attr->value, _attr->header, xml_memory_page_value_allocated_mask, rhs);
	}

	bool xml_attribute::set_value(int rhs)
	{
		char buf[128];
		sprintf(buf, "%d", rhs);
	
	#ifdef PUGIXML_WCHAR_MODE
		char_t wbuf[128];
		impl::widen_ascii(wbuf, buf);

		return set_value(wbuf);
	#else
		return set_value(buf);
	#endif
	}

	bool xml_attribute::set_value(unsigned int rhs)
	{
		char buf[128];
		sprintf(buf, "%u", rhs);

	#ifdef PUGIXML_WCHAR_MODE
		char_t wbuf[128];
		impl::widen_ascii(wbuf, buf);

		return set_value(wbuf);
	#else
		return set_value(buf);
	#endif
	}

	bool xml_attribute::set_value(double rhs)
	{
		char buf[128];
		sprintf(buf, "%g", rhs);

	#ifdef PUGIXML_WCHAR_MODE
		char_t wbuf[128];
		impl::widen_ascii(wbuf, buf);

		return set_value(wbuf);
	#else
		return set_value(buf);
	#endif
	}
	
	bool xml_attribute::set_value(bool rhs)
	{
		return set_value(rhs ? PUGIXML_TEXT("true") : PUGIXML_TEXT("false"));
	}

#ifdef __BORLANDC__
	bool operator&&(const xml_attribute& lhs, bool rhs)
	{
		return (bool)lhs && rhs;
	}

	bool operator||(const xml_attribute& lhs, bool rhs)
	{
		return (bool)lhs || rhs;
	}
#endif

	xml_node::xml_node(): _root(0)
	{
	}

	xml_node::xml_node(xml_node_struct* p): _root(p)
	{
	}
	
	xml_node::operator xml_node::unspecified_bool_type() const
	{
#ifdef __MWERKS__
      	return _root ? &xml_node::empty : 0;
#else
      	return _root ? &xml_node::_root : 0;
#endif
   	}

   	bool xml_node::operator!() const
   	{
   		return !_root;
   	}

	xml_node::iterator xml_node::begin() const
	{
		return _root ? iterator(_root->first_child) : iterator();
	}

	xml_node::iterator xml_node::end() const
	{
		return _root && _root->first_child ? iterator(0, _root->first_child->prev_sibling_c) : iterator();
	}
	
	xml_node::attribute_iterator xml_node::attributes_begin() const
	{
		return _root ? attribute_iterator(_root->first_attribute) : attribute_iterator();
	}

	xml_node::attribute_iterator xml_node::attributes_end() const
	{
		return _root && _root->first_attribute ? attribute_iterator(0, _root->first_attribute->prev_attribute_c) : attribute_iterator();
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
		return !_root;
	}
	
	xml_allocator& xml_node::get_allocator() const
	{
		assert(_root);

		return *reinterpret_cast<xml_memory_page*>(_root->header & xml_memory_page_pointer_mask)->allocator;
	}

	const char_t* xml_node::name() const
	{
		return (_root && _root->name) ? _root->name : PUGIXML_TEXT("");
	}

	xml_node_type xml_node::type() const
	{
		return _root ? static_cast<xml_node_type>(_root->header & xml_memory_page_type_mask) : node_null;
	}
	
	const char_t* xml_node::value() const
	{
		return (_root && _root->value) ? _root->value : PUGIXML_TEXT("");
	}
	
	xml_node xml_node::child(const char_t* name) const
	{
		if (!_root) return xml_node();

		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->name && impl::strequal(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::child_w(const char_t* name) const
	{
		if (!_root) return xml_node();

		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->name && impl::strequalwild(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_attribute xml_node::attribute(const char_t* name) const
	{
		if (!_root) return xml_attribute();

		for (xml_attribute_struct* i = _root->first_attribute; i; i = i->next_attribute)
			if (i->name && impl::strequal(name, i->name))
				return xml_attribute(i);
		
		return xml_attribute();
	}
	
	xml_attribute xml_node::attribute_w(const char_t* name) const
	{
		if (!_root) return xml_attribute();

		for (xml_attribute_struct* i = _root->first_attribute; i; i = i->next_attribute)
			if (i->name && impl::strequalwild(name, i->name))
				return xml_attribute(i);
		
		return xml_attribute();
	}

	xml_node xml_node::next_sibling(const char_t* name) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->next_sibling; i; i = i->next_sibling)
			if (i->name && impl::strequal(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::next_sibling_w(const char_t* name) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->next_sibling; i; i = i->next_sibling)
			if (i->name && impl::strequalwild(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::next_sibling() const
	{
		if (!_root) return xml_node();
		
		if (_root->next_sibling) return xml_node(_root->next_sibling);
		else return xml_node();
	}

	xml_node xml_node::previous_sibling(const char_t* name) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->prev_sibling_c; i->next_sibling; i = i->prev_sibling_c)
			if (i->name && impl::strequal(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::previous_sibling_w(const char_t* name) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->prev_sibling_c; i->next_sibling; i = i->prev_sibling_c)
			if (i->name && impl::strequalwild(name, i->name)) return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::previous_sibling() const
	{
		if (!_root) return xml_node();
		
		if (_root->prev_sibling_c->next_sibling) return xml_node(_root->prev_sibling_c);
		else return xml_node();
	}

	xml_node xml_node::parent() const
	{
		return _root ? xml_node(_root->parent) : xml_node();
	}

	xml_node xml_node::root() const
	{
		xml_node r = *this;
		while (r && r.parent()) r = r.parent();
		return r;
	}

	const char_t* xml_node::child_value() const
	{
		if (!_root) return PUGIXML_TEXT("");
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
		{
			xml_node_type type = static_cast<xml_node_type>(i->header & xml_memory_page_type_mask);

			if (i->value && (type == node_pcdata || type == node_cdata))
				return i->value;
		}

		return PUGIXML_TEXT("");
	}

	const char_t* xml_node::child_value(const char_t* name) const
	{
		return child(name).child_value();
	}

	const char_t* xml_node::child_value_w(const char_t* name) const
	{
		if (!_root) return PUGIXML_TEXT("");

		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->name && impl::strequalwild(name, i->name)) return xml_node(i).child_value();

		return PUGIXML_TEXT("");
	}

	xml_attribute xml_node::first_attribute() const
	{
		return _root ? xml_attribute(_root->first_attribute) : xml_attribute();
	}

	xml_attribute xml_node::last_attribute() const
	{
		return _root && _root->first_attribute ? xml_attribute(_root->first_attribute->prev_attribute_c) : xml_attribute();
	}

	xml_node xml_node::first_child() const
	{
		return _root ? xml_node(_root->first_child) : xml_node();
	}

	xml_node xml_node::last_child() const
	{
		return _root && _root->first_child ? xml_node(_root->first_child->prev_sibling_c) : xml_node();
	}

	bool xml_node::set_name(const char_t* rhs)
	{
		switch (type())
		{
		case node_pi:
		case node_declaration:
		case node_element:
		{
			return strcpy_insitu(_root->name, _root->header, xml_memory_page_name_allocated_mask, rhs);
		}

		default:
			return false;
		}
	}
		
	bool xml_node::set_value(const char_t* rhs)
	{
		switch (type())
		{
		case node_pi:
		case node_cdata:
		case node_pcdata:
		case node_comment:
		{
			return strcpy_insitu(_root->value, _root->header, xml_memory_page_value_allocated_mask, rhs);
		}

		default:
			return false;
		}
	}

	xml_attribute xml_node::append_attribute(const char_t* name)
	{
		if (type() != node_element && type() != node_declaration) return xml_attribute();
		
		xml_attribute a(append_attribute_ll(_root, get_allocator()));
		a.set_name(name);
		
		return a;
	}

	xml_attribute xml_node::insert_attribute_before(const char_t* name, const xml_attribute& attr)
	{
		if ((type() != node_element && type() != node_declaration) || attr.empty()) return xml_attribute();
		
		// check that attribute belongs to *this
		xml_attribute_struct* cur = attr._attr;

		while (cur->prev_attribute_c->next_attribute) cur = cur->prev_attribute_c;

		if (cur != _root->first_attribute) return xml_attribute();

		xml_attribute a(allocate_attribute(get_allocator()));
		a.set_name(name);

		if (attr._attr->prev_attribute_c->next_attribute)
			attr._attr->prev_attribute_c->next_attribute = a._attr;
		else
			_root->first_attribute = a._attr;
		
		a._attr->prev_attribute_c = attr._attr->prev_attribute_c;
		a._attr->next_attribute = attr._attr;
		attr._attr->prev_attribute_c = a._attr;
				
		return a;
	}

	xml_attribute xml_node::insert_attribute_after(const char_t* name, const xml_attribute& attr)
	{
		if ((type() != node_element && type() != node_declaration) || attr.empty()) return xml_attribute();
		
		// check that attribute belongs to *this
		xml_attribute_struct* cur = attr._attr;

		while (cur->prev_attribute_c->next_attribute) cur = cur->prev_attribute_c;

		if (cur != _root->first_attribute) return xml_attribute();

		xml_attribute a(allocate_attribute(get_allocator()));
		a.set_name(name);

		if (attr._attr->next_attribute)
			attr._attr->next_attribute->prev_attribute_c = a._attr;
		else
			_root->first_attribute->prev_attribute_c = a._attr;
		
		a._attr->next_attribute = attr._attr->next_attribute;
		a._attr->prev_attribute_c = attr._attr;
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
		
		return xml_node(append_node(_root, get_allocator(), type));
	}

	xml_node xml_node::insert_child_before(xml_node_type type, const xml_node& node)
	{
		if ((this->type() != node_element && this->type() != node_document) || type == node_document || type == node_null) return xml_node();
		if (node.parent() != *this) return xml_node();
	
		xml_node n(allocate_node(get_allocator(), type));
		n._root->parent = _root;
		
		if (node._root->prev_sibling_c->next_sibling)
			node._root->prev_sibling_c->next_sibling = n._root;
		else
			_root->first_child = n._root;
		
		n._root->prev_sibling_c = node._root->prev_sibling_c;
		n._root->next_sibling = node._root;
		node._root->prev_sibling_c = n._root;

		return n;
	}

	xml_node xml_node::insert_child_after(xml_node_type type, const xml_node& node)
	{
		if ((this->type() != node_element && this->type() != node_document) || type == node_document || type == node_null) return xml_node();
		if (node.parent() != *this) return xml_node();
	
		xml_node n(allocate_node(get_allocator(), type));
		n._root->parent = _root;
	
		if (node._root->next_sibling)
			node._root->next_sibling->prev_sibling_c = n._root;
		else
			_root->first_child->prev_sibling_c = n._root;
		
		n._root->next_sibling = node._root->next_sibling;
		n._root->prev_sibling_c = node._root;
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

	void xml_node::remove_attribute(const char_t* name)
	{
		remove_attribute(attribute(name));
	}

	void xml_node::remove_attribute(const xml_attribute& a)
	{
		if (!_root || !a._attr) return;

		// check that attribute belongs to *this
		xml_attribute_struct* attr = a._attr;

		while (attr->prev_attribute_c->next_attribute) attr = attr->prev_attribute_c;

		if (attr != _root->first_attribute) return;

		if (a._attr->next_attribute) a._attr->next_attribute->prev_attribute_c = a._attr->prev_attribute_c;
		else if (_root->first_attribute) _root->first_attribute->prev_attribute_c = a._attr->prev_attribute_c;
		
		if (a._attr->prev_attribute_c->next_attribute) a._attr->prev_attribute_c->next_attribute = a._attr->next_attribute;
		else _root->first_attribute = a._attr->next_attribute;

		destroy_attribute(a._attr, get_allocator());
	}

	void xml_node::remove_child(const char_t* name)
	{
		remove_child(child(name));
	}

	void xml_node::remove_child(const xml_node& n)
	{
		if (!_root || n.parent() != *this) return;

		if (n._root->next_sibling) n._root->next_sibling->prev_sibling_c = n._root->prev_sibling_c;
		else if (_root->first_child) _root->first_child->prev_sibling_c = n._root->prev_sibling_c;
		
		if (n._root->prev_sibling_c->next_sibling) n._root->prev_sibling_c->next_sibling = n._root->next_sibling;
		else _root->first_child = n._root->next_sibling;
        
        destroy_node(n._root, get_allocator());
	}

	xml_node xml_node::find_child_by_attribute(const char_t* name, const char_t* attr_name, const char_t* attr_value) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->name && impl::strequal(name, i->name))
			{
				for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
					if (impl::strequal(attr_name, a->name) && impl::strequal(attr_value, a->value))
						return xml_node(i);
			}

		return xml_node();
	}

	xml_node xml_node::find_child_by_attribute_w(const char_t* name, const char_t* attr_name, const char_t* attr_value) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			if (i->name && impl::strequalwild(name, i->name))
			{
				for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
					if (impl::strequalwild(attr_name, a->name) && impl::strequalwild(attr_value, a->value))
						return xml_node(i);
			}

		return xml_node();
	}

	xml_node xml_node::find_child_by_attribute(const char_t* attr_name, const char_t* attr_value) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
				if (impl::strequal(attr_name, a->name) && impl::strequal(attr_value, a->value))
					return xml_node(i);

		return xml_node();
	}

	xml_node xml_node::find_child_by_attribute_w(const char_t* attr_name, const char_t* attr_value) const
	{
		if (!_root) return xml_node();
		
		for (xml_node_struct* i = _root->first_child; i; i = i->next_sibling)
			for (xml_attribute_struct* a = i->first_attribute; a; a = a->next_attribute)
				if (impl::strequalwild(attr_name, a->name) && impl::strequalwild(attr_value, a->value))
					return xml_node(i);

		return xml_node();
	}

#ifndef PUGIXML_NO_STL
	string_t xml_node::path(char_t delimiter) const
	{
		string_t path;

		xml_node cursor = *this; // Make a copy.
		
		path = cursor.name();

		while (cursor.parent())
		{
			cursor = cursor.parent();
			
			string_t temp = cursor.name();
			temp += delimiter;
			temp += path;
			path.swap(temp);
		}

		return path;
	}
#endif

	xml_node xml_node::first_element_by_path(const char_t* path, char_t delimiter) const
	{
		xml_node found = *this; // Current search context.

		if (!_root || !path || !path[0]) return found;

		if (path[0] == delimiter)
		{
			// Absolute path; e.g. '/foo/bar'
			while (found.parent()) found = found.parent();
			++path;
		}

		const char_t* path_segment = path;

		while (*path_segment == delimiter) ++path_segment;

		const char_t* path_segment_end = path_segment;

		while (*path_segment_end && *path_segment_end != delimiter) ++path_segment_end;

		if (path_segment == path_segment_end) return found;

		const char_t* next_segment = path_segment_end;

		while (*next_segment == delimiter) ++next_segment;

		if (*path_segment == '.' && path_segment + 1 == path_segment_end)
			return found.first_element_by_path(next_segment, delimiter);
		else if (*path_segment == '.' && *(path_segment+1) == '.' && path_segment + 2 == path_segment_end)
			return found.parent().first_element_by_path(next_segment, delimiter);
		else
		{
			for (xml_node_struct* j = found._root->first_child; j; j = j->next_sibling)
			{
				if (j->name && impl::strequalrange(j->name, path_segment, static_cast<size_t>(path_segment_end - path_segment)))
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
		walker._depth = -1;
		
		if (!walker.begin(*this)) return false;

		xml_node cur = first_child();
				
		if (cur)
		{
			++walker._depth;

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
		return 0;
	}

	void xml_node::print(xml_writer& writer, const char_t* indent, unsigned int flags, encoding_t encoding, unsigned int depth) const
	{
		if (!_root) return;

		xml_buffered_writer buffered_writer(writer, encoding);

		node_output(buffered_writer, *this, indent, flags, depth);
	}

#ifndef PUGIXML_NO_STL
	void xml_node::print(std::basic_ostream<char, std::char_traits<char> >& stream, const char_t* indent, unsigned int flags, encoding_t encoding, unsigned int depth) const
	{
		if (!_root) return;

		xml_writer_stream writer(stream);

		print(writer, indent, flags, encoding, depth);
	}

	void xml_node::print(std::basic_ostream<wchar_t, std::char_traits<wchar_t> >& stream, const char_t* indent, unsigned int flags, unsigned int depth) const
	{
		if (!_root) return;

		xml_writer_stream writer(stream);

		print(writer, indent, flags, encoding_wchar, depth);
	}
#endif

	ptrdiff_t xml_node::offset_debug() const
	{
		xml_node_struct* r = root()._root;

		if (!r) return -1;

		const char_t* buffer = static_cast<xml_document_struct*>(r)->buffer;

		if (!buffer) return -1;

		switch (type())
		{
		case node_document:
			return 0;

		case node_element:
		case node_declaration:
		case node_pi:
			return (_root->header & xml_memory_page_name_allocated_mask) ? -1 : _root->name - buffer;

		case node_pcdata:
		case node_cdata:
		case node_comment:
			return (_root->header & xml_memory_page_value_allocated_mask) ? -1 : _root->value - buffer;

		default:
			return -1;
		}
	}

#ifdef __BORLANDC__
	bool operator&&(const xml_node& lhs, bool rhs)
	{
		return (bool)lhs && rhs;
	}

	bool operator||(const xml_node& lhs, bool rhs)
	{
		return (bool)lhs || rhs;
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
		if (_wrap._root) _wrap = _wrap.previous_sibling();
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
		if (_wrap._attr) _wrap = _wrap.previous_attribute();
		else _wrap = _prev;
		return *this;
	}

	xml_attribute_iterator xml_attribute_iterator::operator--(int)
	{
		xml_attribute_iterator temp = *this;
		--*this;
		return temp;
	}

	xml_memory_page::xml_memory_page(): allocator(0), memory(0), prev(0), next(0), busy_size(0), freed_size(0)
	{
	}

	const char* xml_parse_result::description() const
	{
		switch (status)
		{
		case status_ok: return "No error";

		case status_file_not_found: return "File was not found";
		case status_io_error: return "Error reading from file/stream";
		case status_out_of_memory: return "Could not allocate memory";
		case status_internal_error: return "Internal error occured";

		case status_unrecognized_tag: return "Could not determine tag type";

		case status_bad_pi: return "Error parsing document declaration/processing instruction";
		case status_bad_comment: return "Error parsing comment";
		case status_bad_cdata: return "Error parsing CDATA section";
		case status_bad_doctype: return "Error parsing document type declaration";
		case status_bad_pcdata: return "Error parsing PCDATA section";
		case status_bad_start_element: return "Error parsing start element tag";
		case status_bad_attribute: return "Error parsing element attribute";
		case status_bad_end_element: return "Error parsing end element tag";
		case status_end_element_mismatch: return "Start-end tags mismatch";

		default: return "Unknown error";
		}
	}

	xml_document::xml_document(): _buffer(0)
	{
		create();
	}

	xml_document::~xml_document()
	{
		destroy();

		if (_memory.next)
		{
			assert(!_memory.next->next);

			xml_allocator::deallocate_page(_memory.next);
		}
	}

	void xml_document::create()
	{
		destroy();

		// initialize sentinel page
		_memory.busy_size = xml_memory_page_size;

		// allocate new root
		xml_allocator alloc(_memory.next ? _memory.next : &_memory);
		
		_root = allocate_document(alloc);

		_root->prev_sibling_c = _root;

		// setup allocator
		xml_allocator& a = static_cast<xml_document_struct*>(_root)->allocator;
		a = alloc;

		// setup page
		assert(_memory.next);
		_memory.next->allocator = &a;
	}

	void xml_document::destroy()
	{
		// destroy static storage
		if (_buffer)
		{
			global_deallocate(_buffer);
			_buffer = 0;
		}

		// destroy dynamic storage, leave sentinel page and next page (if any)
		if (_root)
		{
			assert(_memory.next);

			// destroy all pages
			for (xml_memory_page* page = _memory.next->next; page; )
			{
				xml_memory_page* next = page->next;

				xml_allocator::deallocate_page(page);

				page = next;
			}

			// cleanup next page
			_memory.next->allocator = 0;
			_memory.next->next = 0;
			_memory.next->busy_size = _memory.next->freed_size = 0;

			_root = 0;
		}
		else
		{
			assert(!_memory.next);
		}
	}

#ifndef PUGIXML_NO_STL
	xml_parse_result xml_document::load(std::basic_istream<char, std::char_traits<char> >& stream, unsigned int options, encoding_t encoding)
	{
		create();

		return load_stream_impl(*this, stream, options, encoding);
	}

	xml_parse_result xml_document::load(std::basic_istream<wchar_t, std::char_traits<wchar_t> >& stream, unsigned int options)
	{
		create();

		return load_stream_impl(*this, stream, options, encoding_wchar);
	}
#endif

	xml_parse_result xml_document::load(const char_t* contents, unsigned int options)
	{
		create();
		
		// Force native encoding (skip autodetection)
	#ifdef PUGIXML_WCHAR_MODE
		encoding_t encoding = encoding_wchar;
	#else
		encoding_t encoding = encoding_utf8;
	#endif

		return load_buffer(contents, impl::strlen(contents) * sizeof(char_t), options, encoding);
	}

	xml_parse_result xml_document::parse(char* xmlstr, unsigned int options)
	{
		return load_buffer_inplace(xmlstr, strlen(xmlstr), options, encoding_utf8);
	}
		
	xml_parse_result xml_document::parse(const transfer_ownership_tag&, char* xmlstr, unsigned int options)
	{
		return load_buffer_inplace_own(xmlstr, strlen(xmlstr), options, encoding_utf8);
	}

	xml_parse_result xml_document::load_file(const char* name, unsigned int options, encoding_t encoding)
	{
		create();

		FILE* file = fopen(name, "rb");
		if (!file) return MAKE_PARSE_RESULT(status_file_not_found);

		fseek(file, 0, SEEK_END);
		long length = ftell(file);
		fseek(file, 0, SEEK_SET);

		if (length < 0)
		{
			fclose(file);
			return MAKE_PARSE_RESULT(status_io_error);
		}
		
		char* s = static_cast<char*>(global_allocate(length > 0 ? length : 1));

		if (!s)
		{
			fclose(file);
			return MAKE_PARSE_RESULT(status_out_of_memory);
		}

		size_t read = fread(s, 1, (size_t)length, file);
		fclose(file);

		if (read != (size_t)length)
		{
			global_deallocate(s);
			return MAKE_PARSE_RESULT(status_io_error);
		}
		
		return load_buffer_inplace_own(s, length, options, encoding);
	}

	xml_parse_result xml_document::load_buffer_impl(void* contents, size_t size, unsigned int options, encoding_t encoding, bool is_mutable, bool own)
	{
		create();

		// get actual encoding
		encoding_t buffer_encoding = get_buffer_encoding(encoding, contents, size);

		// get private buffer
		char_t* buffer = 0;
		size_t length = 0;

		if (!convert_buffer(buffer, length, buffer_encoding, contents, size, is_mutable)) return MAKE_PARSE_RESULT(status_out_of_memory);
		
		// delete original buffer if we performed a conversion
		if (own && buffer != contents) global_deallocate(contents);

		// parse
		xml_parse_result res = xml_parser::parse(buffer, length, _root, options);

		// remember encoding
		res.encoding = buffer_encoding;

		// grab onto buffer if it's our buffer, user is responsible for deallocating contens himself
		if (own || buffer != contents) _buffer = buffer;

		return res;
	}

	xml_parse_result xml_document::load_buffer(const void* contents, size_t size, unsigned int options, encoding_t encoding)
	{
		return load_buffer_impl(const_cast<void*>(contents), size, options, encoding, false, false);
	}

	xml_parse_result xml_document::load_buffer_inplace(void* contents, size_t size, unsigned int options, encoding_t encoding)
	{
		return load_buffer_impl(contents, size, options, encoding, true, false);
	}
		
	xml_parse_result xml_document::load_buffer_inplace_own(void* contents, size_t size, unsigned int options, encoding_t encoding)
	{
		return load_buffer_impl(contents, size, options, encoding, true, true);
	}

	void xml_document::save(xml_writer& writer, const char_t* indent, unsigned int flags, encoding_t encoding) const
	{
		if (flags & format_write_bom) write_bom(writer, get_write_encoding(encoding));

		xml_buffered_writer buffered_writer(writer, encoding);

		if (!(flags & format_no_declaration))
		{
			buffered_writer.write(PUGIXML_TEXT("<?xml version=\"1.0\"?>"));
			if (!(flags & format_raw)) buffered_writer.write('\n');
		}

		node_output(buffered_writer, *this, indent, flags, 0);
	}

#ifndef PUGIXML_NO_STL
	void xml_document::save(std::basic_ostream<char, std::char_traits<char> >& stream, const char_t* indent, unsigned int flags, encoding_t encoding) const
	{
		xml_writer_stream writer(stream);

		save(writer, indent, flags, encoding);
	}

	void xml_document::save(std::basic_ostream<wchar_t, std::char_traits<wchar_t> >& stream, const char_t* indent, unsigned int flags) const
	{
		xml_writer_stream writer(stream);

		save(writer, indent, flags, encoding_wchar);
	}
#endif

	bool xml_document::save_file(const char* name, const char_t* indent, unsigned int flags, encoding_t encoding) const
	{
		FILE* file = fopen(name, "wb");
		if (!file) return false;

		xml_writer_file writer(file);
		save(writer, indent, flags, encoding);

		fclose(file);

		return true;
	}

	void xml_document::precompute_document_order()
	{
	}

#ifndef PUGIXML_NO_STL
	std::string PUGIXML_FUNCTION as_utf8(const wchar_t* str)
	{
		STATIC_ASSERT(sizeof(wchar_t) == 2 || sizeof(wchar_t) == 4);

		size_t length = wcslen(str);

		// first pass: get length in utf8 characters
		size_t size = sizeof(wchar_t) == 2 ?
			impl::decode_utf16_block<impl::utf8_counter>(reinterpret_cast<const impl::char16_t*>(str), length, 0, opt1_to_type<false>()) :
			impl::decode_utf32_block<impl::utf8_counter>(reinterpret_cast<const impl::char32_t*>(str), length, 0, opt1_to_type<false>());

		// allocate resulting string
		std::string result;
		result.resize(size);

		// second pass: convert to utf8
		if (size > 0)
		{
			impl::char8_t* begin = reinterpret_cast<impl::char8_t*>(&result[0]);
			impl::char8_t* end = sizeof(wchar_t) == 2 ?
				impl::decode_utf16_block<impl::utf8_writer>(reinterpret_cast<const impl::char16_t*>(str), length, begin, opt1_to_type<false>()) :
				impl::decode_utf32_block<impl::utf8_writer>(reinterpret_cast<const impl::char32_t*>(str), length, begin, opt1_to_type<false>());
	  	
			// truncate invalid output
			assert(begin <= end && static_cast<size_t>(end - begin) <= result.size());
			result.resize(static_cast<size_t>(end - begin));
		}

	  	return result;
	}
	
	std::wstring PUGIXML_FUNCTION as_utf16(const char* str)
	{
		return as_wide(str);
	}

	std::wstring PUGIXML_FUNCTION as_wide(const char* str)
	{
		const impl::char8_t* data = reinterpret_cast<const impl::char8_t*>(str);
		size_t size = strlen(str);

		// first pass: get length in wchar_t
		size_t length = impl::decode_utf8_block<impl::wchar_counter>(data, size, 0);

		// allocate resulting string
		std::wstring result;
		result.resize(length);

		// second pass: convert to wchar_t
		if (length > 0)
		{
			impl::wchar_writer::value_type begin = reinterpret_cast<impl::wchar_writer::value_type>(&result[0]);
			impl::wchar_writer::value_type end = impl::decode_utf8_block<impl::wchar_writer>(data, size, begin);

			// truncate invalid output
			assert(begin <= end && static_cast<size_t>(end - begin) <= result.size());
			result.resize(static_cast<size_t>(end - begin));
		}

		return result;
	}
#endif

    void PUGIXML_FUNCTION set_memory_management_functions(allocation_function allocate, deallocation_function deallocate)
    {
    	global_allocate = allocate;
    	global_deallocate = deallocate;
    }

    allocation_function PUGIXML_FUNCTION get_memory_allocation_function()
    {
    	return global_allocate;
    }

    deallocation_function PUGIXML_FUNCTION get_memory_deallocation_function()
    {
    	return global_deallocate;
    }
}

#if !defined(PUGIXML_NO_STL) && (defined(_MSC_VER) || defined(__ICC))
namespace std
{
	// Workarounds for (non-standard) iterator category detection for older versions (MSVC7/IC8 and earlier)
	std::bidirectional_iterator_tag _Iter_cat(const pugi::xml_node_iterator&)
	{
		return std::bidirectional_iterator_tag();
	}

	std::bidirectional_iterator_tag _Iter_cat(const pugi::xml_attribute_iterator&)
	{
		return std::bidirectional_iterator_tag();
	}
}
#endif

/**
 * Copyright (c) 2006-2010 Arseny Kapoulkine
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
