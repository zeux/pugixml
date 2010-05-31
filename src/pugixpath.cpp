/**
 * pugixml parser - version 0.7
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

#ifndef PUGIXML_NO_XPATH

#include <assert.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <string.h>

#ifdef PUGIXML_WCHAR_MODE
#	include <wchar.h>
#endif

// int32_t
#if !defined(_MSC_VER) || _MSC_VER >= 1600
#	include <stdint.h>
#else
typedef __int32 int32_t;
#endif


#if defined(_MSC_VER)
#	pragma warning(disable: 4127) // conditional expression is constant
#	pragma warning(disable: 4702) // unreachable code
#	pragma warning(disable: 4996) // this function or variable may be unsafe
#endif

#include <algorithm>
#include <string>

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

namespace
{
	using namespace pugi;
		
	enum chartypex
	{
		ctx_space = 1,			// \r, \n, space, tab
		ctx_start_symbol = 2,	// Any symbol > 127, a-z, A-Z, _
		ctx_digit = 4,			// 0-9
		ctx_symbol = 8			// Any symbol > 127, a-z, A-Z, 0-9, _, -, .
	};
	
	const unsigned char chartypex_table[256] =
	{
		0,  0,  0,  0,  0,  0,  0,  0,     0,  1,  1,  0,  0,  1,  0,  0,     // 0-15
		0,  0,  0,  0,  0,  0,  0,  0,     0,  0,  0,  0,  0,  0,  0,  0,     // 16-31
		1,  0,  0,  0,  0,  0,  0,  0,     0,  0,  0,  0,  0,  8,  8,  0,     // 32-47
		12, 12, 12, 12, 12, 12, 12, 12,    12, 12, 0,  0,  0,  0,  0,  0,     // 48-63
		0,  10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10,    // 64-79
		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 0,  0,  0,  0,  10,    // 80-95
		0,  10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10,    // 96-111
		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 0,  0,  0,  0,  0,     // 112-127

		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10,    // 128+
		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10,
		10, 10, 10, 10, 10, 10, 10, 10,    10, 10, 10, 10, 10, 10, 10, 10
	};
	
#ifdef PUGIXML_WCHAR_MODE
	#define IS_CHARTYPEX(c, ct) ((static_cast<unsigned int>(c) < 128 ? chartypex_table[static_cast<unsigned int>(c)] : chartypex_table[128]) & (ct))
#else
	#define IS_CHARTYPEX(c, ct) (chartypex_table[static_cast<unsigned char>(c)] & (ct))
#endif

	bool starts_with(const char_t* string, const char_t* pattern)
	{
		while (*pattern && *string == *pattern)
		{
			string++;
			pattern++;
		}

		return *pattern == 0;
	}

	const char_t* find_char(const char_t* s, char_t c)
	{
	#ifdef PUGIXML_WCHAR_MODE
		return wcschr(s, c);
	#else
		return ::strchr(s, c);
	#endif
	}

	string_t string_value(const xpath_node& na)
	{
		if (na.attribute())
			return na.attribute().value();
		else
		{
			const xml_node& n = na.node();

			switch (n.type())
			{
			case node_pcdata:
			case node_cdata:
			case node_comment:
			case node_pi:
				return n.value();
			
			case node_document:
			case node_element:
			{
				string_t result;

				xml_node cur = n.first_child();
				
				if (cur)
				{
					do 
					{
						if (cur.type() == node_pcdata || cur.type() == node_cdata)
							result += cur.value();
						
						if (cur.first_child())
							cur = cur.first_child();
						else if (cur.next_sibling())
							cur = cur.next_sibling();
						else
						{
							// Borland C++ workaround
							while (!cur.next_sibling() && cur != n && (bool)cur.parent())
								cur = cur.parent();
						
							if (cur != n)
								cur = cur.next_sibling();
						}
					}
					while (cur && cur != n);
				}
				
				return result;
			}
			
			default:
				return string_t();
			}
		}
	}
	
	unsigned int node_height(xml_node n)
	{
	    unsigned int result = 0;
	    
	    while (n)
	    {
	        ++result;
	        n = n.parent();
	    }
	    
	    return result;
	}
	
	// precondition: node_height of ln is <= node_height of rn, ln != rn
	bool node_is_before(xml_node ln, unsigned int lh, xml_node rn, unsigned int rh)
	{
	    assert(lh <= rh);
	    
	    while (lh < rh)
	    {
	        --rh;
	        rn = rn.parent();
	    }
	    
	    if (ln == rn) return true;
	    
	    while (ln.parent() != rn.parent())
	    {
	        ln = ln.parent();
	        rn = rn.parent();
	    }
	    
        for (; ln; ln = ln.next_sibling())
            if (ln == rn)
                return true;
                
        return false;
    }

    bool node_is_ancestor(xml_node parent, xml_node node)
    {
    	while (node && node != parent) node = node.parent();

    	return parent && node == parent;
    }
    
	struct document_order_comparator
	{
		bool operator()(const xpath_node& lhs, const xpath_node& rhs) const
		{
			xml_node ln = lhs.node(), rn = rhs.node();

			if (lhs.attribute() && rhs.attribute())
			{
				if (lhs.parent() == rhs.parent())
				{
				    for (xml_attribute a = lhs.attribute(); a; a = a.next_attribute())
				        if (a == rhs.attribute())
				            return true;
				    
				    return false;
				}
				
				ln = lhs.parent();
				rn = rhs.parent();
			}
			else if (lhs.attribute())
			{
				if (lhs.parent() == rhs.node()) return false;
				
				ln = lhs.parent();
			}
			else if (rhs.attribute())
			{
				if (rhs.parent() == lhs.node()) return true;
				
				rn = rhs.parent();
			}

			if (ln == rn) return false;
			
			unsigned int lh = node_height(ln);
			unsigned int rh = node_height(rn);
			
			return (lh <= rh) ? node_is_before(ln, lh, rn, rh) : !node_is_before(rn, rh, ln, lh);
		}
	};

	struct duplicate_comparator
	{
		bool operator()(const xpath_node& lhs, const xpath_node& rhs) const
		{
			if (lhs.attribute()) return rhs.attribute() ? lhs.attribute() < rhs.attribute() : true;
			else return rhs.attribute() ? false : lhs.node() < rhs.node();
		}
	};
	
	double gen_nan()
	{
	#if defined(__STDC_IEC_559__) || ((FLT_RADIX - 0 == 2) && (FLT_MAX_EXP - 0 == 128) && (FLT_MANT_DIG - 0 == 24))
		union { float f; int32_t i; } u[sizeof(float) == sizeof(int32_t) ? 1 : -1];
		u[0].i = 0x7fc00000;
		return u[0].f;
	#else
		// fallback
		const volatile double zero = 0.0;
		return zero / zero;
	#endif
	}
	
	bool is_nan(double value)
	{
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		return !!_isnan(value);
	#elif defined(FP_NAN)
		return fpclassify(value) == FP_NAN;
	#else
		// fallback
		const volatile double v = value;
		return v != v;
	#endif
	}
	
	const char_t* convert_number_to_string_special(double value)
	{
	#if defined(_MSC_VER) || defined(__BORLANDC__)
		if (_finite(value)) return (value == 0) ? PUGIXML_TEXT("0") : 0;
		if (_isnan(value)) return PUGIXML_TEXT("NaN");
		return PUGIXML_TEXT("-Infinity") + (value > 0);
	#elif defined(FP_NAN) && defined(FP_INFINITE) && defined(FP_ZERO)
		switch (fpclassify(value))
		{
		case FP_NAN:
			return PUGIXML_TEXT("NaN");

		case FP_INFINITE:
			return PUGIXML_TEXT("-Infinity") + (value > 0);

		case FP_ZERO:
			return PUGIXML_TEXT("0");

		default:
			return 0;
		}
	#else
		// fallback
		const volatile double v = value;

		if (v == 0) return PUGIXML_TEXT("0");
		if (v != v) return PUGIXML_TEXT("NaN");
		if (v * 2 == v) return PUGIXML_TEXT("-Infinity") + (value > 0);
		return 0;
	#endif
	}
	
	bool convert_number_to_boolean(double value)
	{
		return (value != 0 && !is_nan(value));
	}
	
	string_t convert_number_to_string(double value)
	{
		const char_t* special = convert_number_to_string_special(value);
		if (special) return special;
		
		char buf[512];
		sprintf(buf, "%f", value);
			
		// trim trailing zeros after decimal point
		if (strchr(buf, '.'))
		{
			char* ptr = buf + strlen(buf) - 1;
			for (; *ptr == '0'; --ptr) ;

			// trim leftover decimal point (for integer numbers)
			if (*ptr == '.') --ptr;

			*(ptr+1) = 0;
		}

	#ifdef PUGIXML_WCHAR_MODE
		wchar_t wbuf[512];
		impl::widen_ascii(wbuf, buf);
		
		return string_t(wbuf);
	#else
		return string_t(buf);
	#endif
	}
	
	bool check_string_to_number_format(const char_t* string)
	{
		// parse leading whitespace
		while (IS_CHARTYPEX(*string, ctx_space)) ++string;

		// parse sign
		if (*string == '-') ++string;

		if (!*string) return false;

		// if there is no integer part, there should be a decimal part with at least one digit
		if (!IS_CHARTYPEX(string[0], ctx_digit) && (string[0] != '.' || !IS_CHARTYPEX(string[1], ctx_digit))) return false;

		// parse integer part
		while (IS_CHARTYPEX(*string, ctx_digit)) ++string;

		// parse decimal part
		if (*string == '.')
		{
			++string;

			while (IS_CHARTYPEX(*string, ctx_digit)) ++string;
		}

		// parse trailing whitespace
		while (IS_CHARTYPEX(*string, ctx_space)) ++string;

		return *string == 0;
	}

	double convert_string_to_number(const char_t* string)
	{
		// check string format
		if (!check_string_to_number_format(string)) return gen_nan();

		// parse string
	#ifdef PUGIXML_WCHAR_MODE
		return wcstod(string, 0);
	#else
		return atof(string);
	#endif
	}
	
	double round_nearest(double value)
	{
		return floor(value + 0.5);
	}

	double round_nearest_nzero(double value)
	{
		// same as round_nearest, but returns -0 for [-0.5, -0]
		// ceil is used to differentiate between +0 and -0 (we return -0 for [-0.5, -0] and +0 for +0)
		return (value >= -0.5 && value <= 0) ? ceil(value) : floor(value + 0.5);
	}
	
	const char_t* local_name(const char_t* name)
	{
		const char_t* p = find_char(name, ':');
		
		return p ? p + 1 : name;
	}
	
	const char_t* namespace_uri(const xml_node& node)
	{
		const char_t* pos = find_char(node.name(), ':');
		
		string_t ns = PUGIXML_TEXT("xmlns");
		
		if (pos)
		{
			ns += ':';
			ns.append(node.name(), pos);
		}
		
		xml_node p = node;
		
		while (p)
		{
			xml_attribute a = p.attribute(ns.c_str());
			
			if (a) return a.value();
			
			p = p.parent();
		}
		
		return PUGIXML_TEXT("");
	}

	const char_t* namespace_uri(const xml_attribute& attr, const xml_node& parent)
	{
		const char_t* pos = find_char(attr.name(), ':');
		
		// Default namespace does not apply to attributes
		if (!pos) return PUGIXML_TEXT("");
		
		string_t ns = PUGIXML_TEXT("xmlns:");
		ns.append(attr.name(), pos);
		
		xml_node p = parent;
		
		while (p)
		{
			xml_attribute a = p.attribute(ns.c_str());
			
			if (a) return a.value();
			
			p = p.parent();
		}
		
		return PUGIXML_TEXT("");
	}

	struct equal_to
	{
		template <typename T> bool operator()(const T& lhs, const T& rhs) const
		{
			return lhs == rhs;
		}
	};

	struct not_equal_to
	{
		template <typename T> bool operator()(const T& lhs, const T& rhs) const
		{
			return lhs != rhs;
		}
	};

	struct less
	{
		template <typename T> bool operator()(const T& lhs, const T& rhs) const
		{
			return lhs < rhs;
		}
	};

	struct less_equal
	{
		template <typename T> bool operator()(const T& lhs, const T& rhs) const
		{
			return lhs <= rhs;
		}
	};
}

namespace pugi
{
	xpath_exception::xpath_exception(const char* message): m_message(message)
	{
	}
	
	const char* xpath_exception::what() const throw()
	{
		return m_message;
	}
	
	const size_t xpath_memory_block_size = 4096;		///< Memory block size, 4 kb

	class xpath_allocator
	{
		// disable copy ctor and assignment
		xpath_allocator(const xpath_allocator&);
		xpath_allocator& operator=(const xpath_allocator&);

		struct memory_block
		{	
			memory_block(): next(0), size(0)
			{
			}
	
			memory_block* next;
			size_t size;
	
			char data[xpath_memory_block_size];
		};
		
		memory_block* m_root;
		
	public:
		xpath_allocator(): m_root(0)
		{
			m_root = new memory_block;
		}
		
		~xpath_allocator()
		{
			while (m_root)
			{
				memory_block* cur = m_root->next;
				delete m_root;
				m_root = cur;
			}
		}

		void* alloc(size_t size)
		{
			// align size so that we're able to store pointers in subsequent blocks
			size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

			if (m_root->size + size <= xpath_memory_block_size)
			{
				void* buf = m_root->data + m_root->size;
				m_root->size += size;
				return buf;
			}
			else
			{
				memory_block* block;
				
				if (size > xpath_memory_block_size)
					block = static_cast<memory_block*>(operator new(size + sizeof(memory_block) - xpath_memory_block_size));
				else
					block = new memory_block;
					
				block->next = m_root;
				block->size = size;
				
				m_root = block;
				
				return block->data;
			}
		}

		void* node();
	};

	xpath_node::xpath_node()
	{
	}
		
	xpath_node::xpath_node(const xml_node& node): m_node(node)
	{
	}
		
	xpath_node::xpath_node(const xml_attribute& attribute, const xml_node& parent): m_node(parent), m_attribute(attribute)
	{
	}

	xml_node xpath_node::node() const
	{
		return m_attribute ? xml_node() : m_node;
	}
		
	xml_attribute xpath_node::attribute() const
	{
		return m_attribute;
	}
	
	xml_node xpath_node::parent() const
	{
		return m_attribute ? m_node : m_node.parent();
	}

	xpath_node::operator xpath_node::unspecified_bool_type() const
	{
		return (m_node || m_attribute) ? &xpath_node::m_node : 0;
	}
	
	bool xpath_node::operator!() const
	{
		return !(m_node || m_attribute);
	}

	bool xpath_node::operator==(const xpath_node& n) const
	{
		return m_node == n.m_node && m_attribute == n.m_attribute;
	}
	
	bool xpath_node::operator!=(const xpath_node& n) const
	{
		return m_node != n.m_node || m_attribute != n.m_attribute;
	}

#ifdef __BORLANDC__
	bool operator&&(const xpath_node& lhs, bool rhs)
	{
		return (bool)lhs && rhs;
	}

	bool operator||(const xpath_node& lhs, bool rhs)
	{
		return (bool)lhs || rhs;
	}
#endif

	xpath_node_set::xpath_node_set(): m_type(type_unsorted), m_begin(&m_storage), m_end(&m_storage), m_eos(&m_storage + 1)
	{
	}

	xpath_node_set::~xpath_node_set()
	{
		if (m_begin != &m_storage) delete[] m_begin;
	}
		
	xpath_node_set::xpath_node_set(const xpath_node_set& ns): m_type(type_unsorted), m_begin(&m_storage), m_end(&m_storage), m_eos(&m_storage + 1)
	{
		*this = ns;
	}
	
	xpath_node_set& xpath_node_set::operator=(const xpath_node_set& ns)
	{
		if (&ns == this) return *this;

		if (m_begin != &m_storage) delete[] m_begin;
		
		m_begin = m_end = m_eos = 0;
		m_type = ns.m_type;
		
		if (ns.size() == 1)
		{
			m_storage = *ns.m_begin;
			m_begin = &m_storage;
			m_end = m_eos = &m_storage + 1;
		}
		else
		{
			append(ns.begin(), ns.end());
		}
		
		return *this;
	}	

	xpath_node_set::type_t xpath_node_set::type() const
	{
		return m_type;
	}
		
	size_t xpath_node_set::size() const
	{
		return m_end - m_begin;
	}
		
	bool xpath_node_set::empty() const
	{
		return size() == 0;
	}
		
	xpath_node xpath_node_set::operator[](size_t index) const
	{
		return (index >= size()) ? xpath_node() : m_begin[index];
	}

	xpath_node_set::iterator xpath_node_set::mut_begin()
	{
		return m_begin;
	}
	
	xpath_node_set::const_iterator xpath_node_set::begin() const
	{
		return m_begin;
	}
		
	xpath_node_set::const_iterator xpath_node_set::end() const
	{
		return m_end;
	}
	
	void xpath_node_set::sort(bool reverse)
	{
		std::sort(m_begin, m_end, document_order_comparator());
		
		if (reverse)
			std::reverse(m_begin, m_end);
			
		m_type = reverse ? type_sorted_reverse : type_sorted;
	}

	void xpath_node_set::push_back(const xpath_node& n)
	{
		if (m_end == m_eos)
			append(&n, &n + 1);
		else
		{
			*m_end = n;
			++m_end;
		}
	}

	void xpath_node_set::append(const_iterator begin, const_iterator end)
	{
		if (begin == end) return;

		size_t count = end - begin;
		size_t size = m_end - m_begin;
		size_t capacity = m_eos - m_begin;
		
		if (capacity < size + count)
		{
			if (capacity < 2) capacity = 2;
			
			while (capacity < size + count) capacity += capacity / 2;
			
			xpath_node* storage = new xpath_node[capacity];
			std::copy(m_begin, m_end, storage);
			
			if (m_begin != &m_storage) delete[] m_begin;
			
			m_begin = storage;
			m_end = storage + size;
			m_eos = storage + capacity;
		}
		
		std::copy(begin, end, m_end);
		m_end += count;
	}

	void xpath_node_set::truncate(iterator it)
	{
		m_end = it;
	}

	xpath_node xpath_node_set::first() const
	{
		if (empty()) return xpath_node();

		switch (m_type)
		{
		case type_sorted: return *m_begin;
		case type_sorted_reverse: return *(m_end - 1);
		case type_unsorted: return *std::min_element(begin(), end(), document_order_comparator());
		default: return xpath_node();
		}
	}

	void xpath_node_set::remove_duplicates()
	{
		if (m_type == type_unsorted)
		{
			std::sort(m_begin, m_end, duplicate_comparator());
		}
		
		truncate(std::unique(m_begin, m_end));
	}

	struct xpath_context
	{
		xml_node root;
		xpath_node n;
		size_t position, size;
	};

	enum lexeme_t
	{
		lex_none = 0,
		lex_equal,
		lex_not_equal,
		lex_less,
		lex_greater,
		lex_less_or_equal,
		lex_greater_or_equal,
		lex_plus,
		lex_minus,
		lex_multiply,
		lex_union,
		lex_var_ref,
		lex_open_brace,
		lex_close_brace,
		lex_quoted_string,
		lex_number,
		lex_slash,
		lex_double_slash,
		lex_open_square_brace,
		lex_close_square_brace,
		lex_string,
		lex_comma,
		lex_axis_attribute,
		lex_dot,
		lex_double_dot,
		lex_double_colon,
		lex_eof
	};

	struct xpath_lexer_string
	{
		const char_t* begin;
		const char_t* end;

		xpath_lexer_string(): begin(0), end(0)
		{
		}

		bool operator==(const char_t* other) const
		{
			size_t length = static_cast<size_t>(end - begin);

			if (!impl::strequalrange(other, begin, length)) return false;
			
			return other[length] == 0;
		}
	};

	class xpath_lexer
	{
		// disable copy ctor and assignment
		xpath_lexer(const xpath_lexer&);
		xpath_lexer& operator=(const xpath_lexer&);

	private:
		const char_t* m_cur;
		xpath_lexer_string m_cur_lexeme_contents;

		lexeme_t m_cur_lexeme;

		void contents_clear()
		{
			m_cur_lexeme_contents = xpath_lexer_string();
		}

	public:
		explicit xpath_lexer(const char_t* query): m_cur(query)
		{
			next();
		}
		
		const char_t* state() const
		{
			return m_cur;
		}
		
		void reset(const char_t* state)
		{
			m_cur = state;
			next();
		}

		void next()
		{
			contents_clear();

			while (IS_CHARTYPEX(*m_cur, ctx_space)) ++m_cur;

			switch (*m_cur)
			{
			case 0:
				m_cur_lexeme = lex_eof;
				break;
			
			case '>':
				if (*(m_cur+1) == '=')
				{
					m_cur += 2;
					m_cur_lexeme = lex_greater_or_equal;
				}
				else
				{
					m_cur += 1;
					m_cur_lexeme = lex_greater;
				}
				break;

			case '<':
				if (*(m_cur+1) == '=')
				{
					m_cur += 2;
					m_cur_lexeme = lex_less_or_equal;
				}
				else
				{
					m_cur += 1;
					m_cur_lexeme = lex_less;
				}
				break;

			case '!':
				if (*(m_cur+1) == '=')
				{
					m_cur += 2;
					m_cur_lexeme = lex_not_equal;
				}
				else
				{
					m_cur_lexeme = lex_none;
				}
				break;

			case '=':
				m_cur += 1;
				m_cur_lexeme = lex_equal;

				break;
			
			case '+':
				m_cur += 1;
				m_cur_lexeme = lex_plus;

				break;

			case '-':
				m_cur += 1;
				m_cur_lexeme = lex_minus;

				break;

			case '*':
				m_cur += 1;
				m_cur_lexeme = lex_multiply;

				break;

			case '|':
				m_cur += 1;
				m_cur_lexeme = lex_union;

				break;

			case '$':
				m_cur += 1;
				m_cur_lexeme = lex_var_ref;

				break;
			
			case '(':
				m_cur += 1;
				m_cur_lexeme = lex_open_brace;

				break;

			case ')':
				m_cur += 1;
				m_cur_lexeme = lex_close_brace;

				break;
			
			case '[':
				m_cur += 1;
				m_cur_lexeme = lex_open_square_brace;

				break;

			case ']':
				m_cur += 1;
				m_cur_lexeme = lex_close_square_brace;

				break;

			case ',':
				m_cur += 1;
				m_cur_lexeme = lex_comma;

				break;

			case '/':
				if (*(m_cur+1) == '/')
				{
					m_cur += 2;
					m_cur_lexeme = lex_double_slash;
				}
				else
				{
					m_cur += 1;
					m_cur_lexeme = lex_slash;
				}
				break;
		
			case '.':
				if (*(m_cur+1) == '.')
				{
					m_cur += 2;
					m_cur_lexeme = lex_double_dot;
				}
				else if (IS_CHARTYPEX(*(m_cur+1), ctx_digit))
				{
					m_cur_lexeme_contents.begin = m_cur; // .

					++m_cur;

					while (IS_CHARTYPEX(*m_cur, ctx_digit)) m_cur++;

					m_cur_lexeme_contents.end = m_cur;
					
					m_cur_lexeme = lex_number;
				}
				else
				{
					m_cur += 1;
					m_cur_lexeme = lex_dot;
				}
				break;

			case '@':
				m_cur += 1;
				m_cur_lexeme = lex_axis_attribute;

				break;

			case '"':
			case '\'':
			{
				char_t terminator = *m_cur;

				++m_cur;

				m_cur_lexeme_contents.begin = m_cur;
				while (*m_cur && *m_cur != terminator) m_cur++;
				m_cur_lexeme_contents.end = m_cur;
				
				if (!*m_cur)
					m_cur_lexeme = lex_none;
				else
				{
					m_cur += 1;
					m_cur_lexeme = lex_quoted_string;
				}

				break;
			}

			case ':':
				if (*(m_cur+1) == ':')
				{
					m_cur += 2;
					m_cur_lexeme = lex_double_colon;
				}
				else
				{
					m_cur_lexeme = lex_none;
				}
				break;

			default:
				if (IS_CHARTYPEX(*m_cur, ctx_digit))
				{
					m_cur_lexeme_contents.begin = m_cur;

					while (IS_CHARTYPEX(*m_cur, ctx_digit)) m_cur++;
				
					if (*m_cur == '.')
					{
						m_cur++;

						while (IS_CHARTYPEX(*m_cur, ctx_digit)) m_cur++;
					}

					m_cur_lexeme_contents.end = m_cur;

					m_cur_lexeme = lex_number;
				}
				else if (IS_CHARTYPEX(*m_cur, ctx_start_symbol))
				{
					m_cur_lexeme_contents.begin = m_cur;

					while (IS_CHARTYPEX(*m_cur, ctx_symbol)) m_cur++;

					if (m_cur[0] == ':')
					{
						if (m_cur[1] == '*') // namespace test ncname:*
						{
							m_cur += 2; // :*
						}
						else if (IS_CHARTYPEX(m_cur[1], ctx_symbol)) // namespace test qname
						{
							m_cur++; // :

							while (IS_CHARTYPEX(*m_cur, ctx_symbol)) m_cur++;
						}
					}

					m_cur_lexeme_contents.end = m_cur;
				
					while (IS_CHARTYPEX(*m_cur, ctx_space)) ++m_cur;

					m_cur_lexeme = lex_string;
				}
				else
				{
					throw xpath_exception("Unrecognized token");
				}
			}
		}

		lexeme_t current() const
		{
			return m_cur_lexeme;
		}

		const xpath_lexer_string& contents() const
		{
			return m_cur_lexeme_contents;
		}
	};

	enum ast_type_t
	{
		ast_none,
		ast_op_or,						// left or right
		ast_op_and,						// left and right
		ast_op_equal,					// left = right
		ast_op_not_equal, 				// left != right
		ast_op_less,					// left < right
		ast_op_greater,					// left > right
		ast_op_less_or_equal,			// left <= right
		ast_op_greater_or_equal,		// left >= right
		ast_op_add,						// left + right
		ast_op_subtract,				// left - right
		ast_op_multiply,				// left * right
		ast_op_divide,					// left / right
		ast_op_mod,						// left % right
		ast_op_negate,					// left - right
		ast_op_union,					// left | right
		ast_predicate,					// apply predicate to set; next points to next predicate
		ast_filter,						// select * from left where right
		ast_filter_posinv,				// select * from left where right; proximity position invariant
		ast_string_constant,			// string constant
		ast_number_constant,			// number constant
		ast_func_last,					// last()
		ast_func_position,				// position()
		ast_func_count,					// count(left)
		ast_func_id,					// id(left)
		ast_func_local_name_0,			// local-name()
		ast_func_local_name_1,			// local-name(left)
		ast_func_namespace_uri_0,		// namespace-uri()
		ast_func_namespace_uri_1,		// namespace-uri(left)
		ast_func_name_0,				// name()
		ast_func_name_1,				// name(left)
		ast_func_string_0,				// string()
		ast_func_string_1,				// string(left)
		ast_func_concat,				// concat(left, right, siblings)
		ast_func_starts_with,			// starts_with(left, right)
		ast_func_contains,				// contains(left, right)
		ast_func_substring_before,		// substring-before(left, right)
		ast_func_substring_after,		// substring-after(left, right)
		ast_func_substring_2,			// substring(left, right)
		ast_func_substring_3,			// substring(left, right, third)
		ast_func_string_length_0,		// string-length()
		ast_func_string_length_1,		// string-length(left)
		ast_func_normalize_space_0,		// normalize-space()
		ast_func_normalize_space_1,		// normalize-space(left)
		ast_func_translate,				// translate(left, right, third)
		ast_func_boolean,				// boolean(left)
		ast_func_not,					// not(left)
		ast_func_true,					// true()
		ast_func_false,					// false()
		ast_func_lang,					// lang(left)
		ast_func_number_0,				// number()
		ast_func_number_1,				// number(left)
		ast_func_sum,					// sum(left)
		ast_func_floor,					// floor(left)
		ast_func_ceiling,				// ceiling(left)
		ast_func_round,					// round(left)
		ast_step,						// process set left with step
		ast_step_root					// select root node
	};

	enum axis_t
	{
		axis_ancestor,
		axis_ancestor_or_self,
		axis_attribute,
		axis_child,
		axis_descendant,
		axis_descendant_or_self,
		axis_following,
		axis_following_sibling,
		axis_namespace,
		axis_parent,
		axis_preceding,
		axis_preceding_sibling,
		axis_self
	};
	
	enum nodetest_t
	{
		nodetest_none,
		nodetest_name,
		nodetest_type_node,
		nodetest_type_comment,
		nodetest_type_pi,
		nodetest_type_text,
		nodetest_pi,
		nodetest_all,
		nodetest_all_in_namespace
	};

	template <axis_t N> struct axis_to_type
	{
		static const axis_t axis;
	};

	template <axis_t N> const axis_t axis_to_type<N>::axis = N;
		
	class xpath_ast_node
	{
	private:
		ast_type_t m_type;
		
		xpath_type_t m_rettype;

		// tree node structure
		xpath_ast_node* m_left;
		xpath_ast_node* m_right;
		xpath_ast_node* m_third;
		xpath_ast_node* m_next;

		// string value for ast_constant
		// node test for ast_step (node name/namespace/node type/pi target)
		const char_t* m_contents;

		// for t_step / t_predicate
		axis_t m_axis;
		nodetest_t m_test;
		
		xpath_ast_node(const xpath_ast_node&);
		xpath_ast_node& operator=(const xpath_ast_node&);

		template <class Comp> static bool compare_eq(xpath_ast_node* lhs, xpath_ast_node* rhs, const xpath_context& c, const Comp& comp)
		{
			xpath_type_t lt = lhs->rettype(), rt = rhs->rettype();

			if (lt != xpath_type_node_set && rt != xpath_type_node_set)
			{
				if (lt == xpath_type_boolean || rt == xpath_type_boolean)
					return comp(lhs->eval_boolean(c), rhs->eval_boolean(c));
				else if (lt == xpath_type_number || rt == xpath_type_number)
					return comp(lhs->eval_number(c), rhs->eval_number(c));
				else if (lt == xpath_type_string || rt == xpath_type_string)
					return comp(lhs->eval_string(c), rhs->eval_string(c));
			}
			else if (lt == xpath_type_node_set && rt == xpath_type_node_set)
			{
				xpath_node_set ls = lhs->eval_node_set(c);
				xpath_node_set rs = rhs->eval_node_set(c);

				for (xpath_node_set::const_iterator li = ls.begin(); li != ls.end(); ++li)
					for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
					{
						if (comp(string_value(*li), string_value(*ri)))
							return true;
					}

				return false;
			}
			else
			{
				if (lt == xpath_type_node_set)
				{
					std::swap(lhs, rhs);
					std::swap(lt, rt);
				}

				if (lt == xpath_type_boolean)
					return comp(lhs->eval_boolean(c), rhs->eval_boolean(c));
				else if (lt == xpath_type_number)
				{
					double l = lhs->eval_number(c);
					xpath_node_set rs = rhs->eval_node_set(c);

					for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
					{
						if (comp(l, convert_string_to_number(string_value(*ri).c_str())))
							return true;
					}

					return false;
				}
				else if (lt == xpath_type_string)
				{
					string_t l = lhs->eval_string(c);
					xpath_node_set rs = rhs->eval_node_set(c);

					for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
					{
						if (comp(l, string_value(*ri)))
							return true;
					}

					return false;
				}
			}

			assert(!"Wrong types");
			return false;
		}

		template <class Comp> static bool compare_rel(xpath_ast_node* lhs, xpath_ast_node* rhs, const xpath_context& c, const Comp& comp)
		{
			xpath_type_t lt = lhs->rettype(), rt = rhs->rettype();

			if (lt != xpath_type_node_set && rt != xpath_type_node_set)
				return comp(lhs->eval_number(c), rhs->eval_number(c));
			else if (lt == xpath_type_node_set && rt == xpath_type_node_set)
			{
				xpath_node_set ls = lhs->eval_node_set(c);
				xpath_node_set rs = rhs->eval_node_set(c);

				for (xpath_node_set::const_iterator li = ls.begin(); li != ls.end(); ++li)
				{
					double l = convert_string_to_number(string_value(*li).c_str());

					for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
					{
						if (comp(l, convert_string_to_number(string_value(*ri).c_str())))
							return true;
					}
				}

				return false;
			}
			else if (lt != xpath_type_node_set && rt == xpath_type_node_set)
			{
				double l = lhs->eval_number(c);
				xpath_node_set rs = rhs->eval_node_set(c);

				for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
				{
					if (comp(l, convert_string_to_number(string_value(*ri).c_str())))
						return true;
				}

				return false;
			}
			else if (lt == xpath_type_node_set && rt != xpath_type_node_set)
			{
				xpath_node_set ls = lhs->eval_node_set(c);
				double r = rhs->eval_number(c);

				for (xpath_node_set::const_iterator li = ls.begin(); li != ls.end(); ++li)
				{
					if (comp(convert_string_to_number(string_value(*li).c_str()), r))
						return true;
				}

				return false;
			}
			else
			{
				assert(!"Wrong types");
				return false;
			}
		}

		void apply_predicate(xpath_node_set& ns, size_t first, xpath_ast_node* expr, const xpath_context& context)
		{
			xpath_context c;
			c.root = context.root;
			
			size_t i = 1;
			size_t size = ns.size() - first;
				
			xpath_node_set::iterator last = ns.mut_begin() + first;
				
			// remove_if... or well, sort of
			for (xpath_node_set::iterator it = last; it != ns.end(); ++it, ++i)
			{
				c.n = *it;
				c.position = i;
				c.size = size;
				
				if (expr->rettype() == xpath_type_number)
				{
					if (expr->eval_number(c) == i)
						*last++ = *it;
				}
				else if (expr->eval_boolean(c))
					*last++ = *it;
			}
			
			ns.truncate(last);
		}

		void apply_predicates(xpath_node_set& ns, size_t first, const xpath_context& context)
		{
			if (ns.size() <= first) return;
			
			for (xpath_ast_node* pred = m_right; pred; pred = pred->m_next)
			{
				apply_predicate(ns, first, pred->m_left, context);
			}
		}

		void step_push(xpath_node_set& ns, const xml_attribute& a, const xml_node& parent)
		{
			if (!a) return;

			// There are no attribute nodes corresponding to attributes that declare namespaces
			// That is, "xmlns:..." or "xmlns"
			if (starts_with(a.name(), PUGIXML_TEXT("xmlns")) && (a.name()[5] == 0 || a.name()[5] == ':')) return;
			
			switch (m_test)
			{
			case nodetest_name:
				if (impl::strequal(a.name(), m_contents)) ns.push_back(xpath_node(a, parent));
				break;
				
			case nodetest_type_node:
			case nodetest_all:
				ns.push_back(xpath_node(a, parent));
				break;
				
			case nodetest_all_in_namespace:
				if (starts_with(a.name(), m_contents))
					ns.push_back(xpath_node(a, parent));
				break;
			
			default:
				;
			}
		}
		
		void step_push(xpath_node_set& ns, const xml_node& n)
		{
			if (!n) return;

			switch (m_test)
			{
			case nodetest_name:
				if (n.type() == node_element && impl::strequal(n.name(), m_contents)) ns.push_back(n);
				break;
				
			case nodetest_type_node:
				ns.push_back(n);
				break;
				
			case nodetest_type_comment:
				if (n.type() == node_comment)
					ns.push_back(n);
				break;
				
			case nodetest_type_text:
				if (n.type() == node_pcdata || n.type() == node_cdata)
					ns.push_back(n);
				break;
				
			case nodetest_type_pi:
				if (n.type() == node_pi)
					ns.push_back(n);
				break;
									
			case nodetest_pi:
				if (n.type() == node_pi && impl::strequal(n.name(), m_contents))
					ns.push_back(n);
				break;
				
			case nodetest_all:
				if (n.type() == node_element)
					ns.push_back(n);
				break;
				
			case nodetest_all_in_namespace:
				if (n.type() == node_element && starts_with(n.name(), m_contents))
					ns.push_back(n);
				break;

			default:
				assert(!"Unknown axis");
			} 
		}

		template <class T> void step_fill(xpath_node_set& ns, const xml_node& n, T)
		{
			const axis_t axis = T::axis;

			switch (axis)
			{
			case axis_attribute:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;
				
				for (xml_attribute a = n.first_attribute(); a; a = a.next_attribute())
					step_push(ns, a, n);
				
				break;
			}
			
			case axis_child:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				for (xml_node c = n.first_child(); c; c = c.next_sibling())
					step_push(ns, c);
					
				break;
			}
			
			case axis_descendant:
			case axis_descendant_or_self:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;
				
				if (axis == axis_descendant_or_self)
					step_push(ns, n);
					
				xml_node cur = n.first_child();
				
				if (cur)
				{
					do 
					{
						step_push(ns, cur);
						
						if (cur.first_child())
							cur = cur.first_child();
						else if (cur.next_sibling())
							cur = cur.next_sibling();
						else
						{
							// Borland C++ workaround
							while (!cur.next_sibling() && cur != n && (bool)cur.parent())
								cur = cur.parent();
						
							if (cur != n)
								cur = cur.next_sibling();
						}
					}
					while (cur && cur != n);
				}
				
				break;
			}
			
			case axis_following_sibling:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				for (xml_node c = n.next_sibling(); c; c = c.next_sibling())
					step_push(ns, c);
				
				break;
			}
			
			case axis_preceding_sibling:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_unsorted;

				for (xml_node c = n.previous_sibling(); c; c = c.previous_sibling())
					step_push(ns, c);
				
				break;
			}
			
			case axis_following:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;
				
				xml_node cur = n;
				
				// exit from this node so that we don't include descendants
				while (cur && !cur.next_sibling()) cur = cur.parent();
				cur = cur.next_sibling();

				if (cur)
				{
					for (;;)
					{
						step_push(ns, cur);
	
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

				break;
			}

			case axis_preceding:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_unsorted;

				xml_node cur = n;
				
				while (cur && !cur.previous_sibling()) cur = cur.parent();
				cur = cur.previous_sibling();
				
				if (cur)
				{
					for (;;)
					{
						if (cur.last_child())
							cur = cur.last_child();
						else
						{
							// leaf node, can't be ancestor
							step_push(ns, cur);
							
							if (cur.previous_sibling())
								cur = cur.previous_sibling();
							else
							{
								do 
								{
									cur = cur.parent();
									if (!cur) break;
									
									if (!node_is_ancestor(cur, n)) step_push(ns, cur);
								}
								while (!cur.previous_sibling());
													
								cur = cur.previous_sibling();
								
								if (!cur) break;
							}
						}
					}
				}
				
				break;
			}
			
			case axis_ancestor:
			case axis_ancestor_or_self:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_unsorted;

				if (axis == axis_ancestor_or_self)
					step_push(ns, n);

				xml_node cur = n.parent();
				
				while (cur)
				{
					step_push(ns, cur);
					
					cur = cur.parent();
				}
				
				break;
			}

			case axis_self:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				step_push(ns, n);

				break;
			}

			case axis_parent:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				if (n.parent()) step_push(ns, n.parent());

				break;
			}
				
			default:
				assert(!"Unimplemented axis");
			}
		}
		
		template <class T> void step_fill(xpath_node_set& ns, const xml_attribute& a, const xml_node& p, T v)
		{
			const axis_t axis = T::axis;

			switch (axis)
			{
			case axis_ancestor:
			case axis_ancestor_or_self:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_unsorted;

				if (axis == axis_ancestor_or_self && m_test == nodetest_type_node) // reject attributes based on principal node type test
					step_push(ns, a, p);

				xml_node cur = p;
				
				while (cur)
				{
					step_push(ns, cur);
					
					cur = cur.parent();
				}
				
				break;
			}

			case axis_descendant_or_self:
			case axis_self:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				if (m_test == nodetest_type_node) // reject attributes based on principal node type test
					step_push(ns, a, p);

				break;
			}

			case axis_following:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;
				
				xml_node cur = p;
				
				for (;;)
				{
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

					step_push(ns, cur);
				}

				break;
			}

			case axis_parent:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				step_push(ns, p);

				break;
			}

			case axis_preceding:
			{
				// preceding:: axis does not include attribute nodes and attribute ancestors (they are the same as parent's ancestors), so we can reuse node preceding
				step_fill(ns, p, v);
				break;
			}
			
			default:
				assert(!"Unimplemented axis");
			}
		}
		
		template <class T> void step_do(xpath_node_set& ns, const xpath_context& c, T v)
		{
			const axis_t axis = T::axis;

			assert(ns.empty());

			switch (axis)
			{
			case axis_ancestor:
			case axis_ancestor_or_self:
			case axis_descendant_or_self:
			case axis_following:
			case axis_parent:
			case axis_preceding:
			case axis_self:
				if (m_left)
				{
					xpath_node_set s = m_left->eval_node_set(c);
							
					for (xpath_node_set::const_iterator it = s.begin(); it != s.end(); ++it)
					{
						size_t size = ns.size();
						
						if (it->node())
							step_fill(ns, it->node(), v);
						else
							step_fill(ns, it->attribute(), it->parent(), v);
							
						apply_predicates(ns, size, c);
					}
				}
				else
				{
					if (c.n.node()) step_fill(ns, c.n.node(), v);
					else step_fill(ns, c.n.attribute(), c.n.parent(), v);
					
					apply_predicates(ns, 0, c);
				}
				
				break;
		
			case axis_following_sibling:
			case axis_preceding_sibling:
			case axis_attribute:
			case axis_child:
			case axis_descendant:
				if (m_left)
				{
					xpath_node_set s = m_left->eval_node_set(c);
					
					for (xpath_node_set::const_iterator it = s.begin(); it != s.end(); ++it)
					{
						size_t size = ns.size();
						
						if (it->node())
							step_fill(ns, it->node(), v);
						
						apply_predicates(ns, size, c);
					}
				}
				else if (c.n.node())
				{
					step_fill(ns, c.n.node(), v);
					
					apply_predicates(ns, 0, c);
				}
				
				break;
			
			case axis_namespace:
				break;
				
			default:
				assert(!"Unimplemented axis");
			}
		}
		
		void set_contents(const xpath_lexer_string& value, xpath_allocator& a)
		{
			if (value.begin)
			{
				size_t length = static_cast<size_t>(value.end - value.begin);

				char_t* c = static_cast<char_t*>(a.alloc((length + 1) * sizeof(char_t)));
				memcpy(c, value.begin, length * sizeof(char_t));
				c[length] = 0;

				m_contents = c;
			}
			else m_contents = 0;
		}
	public:
		xpath_ast_node(ast_type_t type, xpath_type_t rettype, const xpath_lexer_string& contents, xpath_allocator& a):
			m_type(type), m_rettype(rettype), m_left(0), m_right(0), m_third(0), m_next(0), m_contents(0),
			m_axis(axis_self), m_test(nodetest_none)
		{
			set_contents(contents, a);
		}
		
		xpath_ast_node(ast_type_t type, xpath_ast_node* left, xpath_ast_node* right, axis_t axis):
			m_type(type), m_rettype(xpath_type_node_set), m_left(left), m_right(right), m_third(0), m_next(0), m_contents(0),
			m_axis(axis), m_test(nodetest_none)
		{
		}

		xpath_ast_node(ast_type_t type, xpath_type_t rettype, xpath_ast_node* left = 0, xpath_ast_node* right = 0, xpath_ast_node* third = 0):
			m_type(type), m_rettype(rettype), m_left(left), m_right(right), m_third(third), m_next(0), m_contents(0),
			m_axis(axis_self), m_test(nodetest_none)
		{
		}

		xpath_ast_node(ast_type_t type, xpath_ast_node* left, axis_t axis, nodetest_t test, const xpath_lexer_string& contents, xpath_allocator& a):
			m_type(type), m_rettype(xpath_type_node_set), m_left(left), m_right(0), m_third(0), m_next(0), m_contents(0),
			m_axis(axis), m_test(test)
		{
			set_contents(contents, a);
		}

		void set_next(xpath_ast_node* value)
		{
			m_next = value;
		}

		void set_right(xpath_ast_node* value)
		{
			m_right = value;
		}

		bool eval_boolean(const xpath_context& c)
		{
			switch (m_type)
			{
			case ast_op_or:
				if (m_left->eval_boolean(c)) return true;
				else return m_right->eval_boolean(c);
				
			case ast_op_and:
				if (!m_left->eval_boolean(c)) return false;
				else return m_right->eval_boolean(c);
				
			case ast_op_equal:
				return compare_eq(m_left, m_right, c, equal_to());

			case ast_op_not_equal:
				return compare_eq(m_left, m_right, c, not_equal_to());
	
			case ast_op_less:
				return compare_rel(m_left, m_right, c, less());
			
			case ast_op_greater:
				return compare_rel(m_right, m_left, c, less());

			case ast_op_less_or_equal:
				return compare_rel(m_left, m_right, c, less_equal());
			
			case ast_op_greater_or_equal:
				return compare_rel(m_right, m_left, c, less_equal());

			case ast_func_starts_with:
				return starts_with(m_left->eval_string(c).c_str(), m_right->eval_string(c).c_str());

			case ast_func_contains:
			{
				string_t lr = m_left->eval_string(c);
				string_t rr = m_right->eval_string(c);

				return rr.empty() || lr.find(rr) != string_t::npos;
			}

			case ast_func_boolean:
				return m_left->eval_boolean(c);
				
			case ast_func_not:
				return !m_left->eval_boolean(c);
				
			case ast_func_true:
				return true;
				
			case ast_func_false:
				return false;

			case ast_func_lang:
			{
				if (c.n.attribute()) return false;
				
				string_t lang = m_left->eval_string(c);
				
				for (xml_node n = c.n.node(); n; n = n.parent())
				{
					xml_attribute a = n.attribute(PUGIXML_TEXT("xml:lang"));
					
					if (a)
					{
						const char_t* value = a.value();
						
						// strnicmp / strncasecmp is not portable
						for (const char_t* lit = lang.c_str(); *lit; ++lit)
						{
							if (tolower(*lit) != tolower(*value)) return false;
							++value;
						}
						
						return *value == 0 || *value == '-';
					}
				}
				
				return false;
			}

			default:
			{
				switch (m_rettype)
				{
				case xpath_type_number:
					return convert_number_to_boolean(eval_number(c));
					
				case xpath_type_string:
					return !eval_string(c).empty();
					
				case xpath_type_node_set:				
					return !eval_node_set(c).empty();
					
				default:
					assert(!"Wrong expression for ret type boolean");
					return false;
				}
			}
			}
		}

		double eval_number(const xpath_context& c)
		{
			switch (m_type)
			{
			case ast_op_add:
				return m_left->eval_number(c) + m_right->eval_number(c);
				
			case ast_op_subtract:
				return m_left->eval_number(c) - m_right->eval_number(c);

			case ast_op_multiply:
				return m_left->eval_number(c) * m_right->eval_number(c);

			case ast_op_divide:
				return m_left->eval_number(c) / m_right->eval_number(c);

			case ast_op_mod:
				return fmod(m_left->eval_number(c), m_right->eval_number(c));

			case ast_op_negate:
				return -m_left->eval_number(c);

			case ast_number_constant:
				return convert_string_to_number(m_contents);

			case ast_func_last:
				return (double)c.size;
			
			case ast_func_position:
				return (double)c.position;

			case ast_func_count:
				return (double)m_left->eval_node_set(c).size();
			
			case ast_func_string_length_0:
				return (double)string_value(c.n).size();
			
			case ast_func_string_length_1:
				return (double)m_left->eval_string(c).size();
			
			case ast_func_number_0:
				return convert_string_to_number(string_value(c.n).c_str());
			
			case ast_func_number_1:
				return m_left->eval_number(c);

			case ast_func_sum:
			{
				double r = 0;
				
				xpath_node_set ns = m_left->eval_node_set(c);
				
				for (xpath_node_set::const_iterator it = ns.begin(); it != ns.end(); ++it)
					r += convert_string_to_number(string_value(*it).c_str());
			
				return r;
			}

			case ast_func_floor:
			{
				double r = m_left->eval_number(c);
				
				return r == r ? floor(r) : r;
			}

			case ast_func_ceiling:
			{
				double r = m_left->eval_number(c);
				
				return r == r ? ceil(r) : r;
			}

			case ast_func_round:
				return round_nearest_nzero(m_left->eval_number(c));
			
			default:
			{
				switch (m_rettype)
				{
				case xpath_type_boolean:
					return eval_boolean(c) ? 1 : 0;
					
				case xpath_type_string:
					return convert_string_to_number(eval_string(c).c_str());
					
				case xpath_type_node_set:
					return convert_string_to_number(eval_string(c).c_str());
					
				default:
					assert(!"Wrong expression for ret type number");
					return 0;
				}
				
			}
			}
		}
		
		string_t eval_string(const xpath_context& c)
		{
			switch (m_type)
			{
			case ast_string_constant:
				return m_contents;
			
			case ast_func_local_name_0:
			{
				xpath_node na = c.n;
				
				if (na.attribute()) return local_name(na.attribute().name());
				else return local_name(na.node().name());
			}

			case ast_func_local_name_1:
			{
				xpath_node_set ns = m_left->eval_node_set(c);
				if (ns.empty()) return string_t();
				
				xpath_node na = ns.first();
				
				if (na.attribute()) return local_name(na.attribute().name());
				else return local_name(na.node().name());
			}

			case ast_func_name_0:
			{
				xpath_node na = c.n;
				
				if (na.attribute()) return na.attribute().name();
				else return na.node().name();
			}

			case ast_func_name_1:
			{
				xpath_node_set ns = m_left->eval_node_set(c);
				if (ns.empty()) return string_t();
				
				xpath_node na = ns.first();
				
				if (na.attribute()) return na.attribute().name();
				else return na.node().name();
			}

			case ast_func_namespace_uri_0:
			{
				xpath_node na = c.n;
				
				if (na.attribute()) return namespace_uri(na.attribute(), na.parent());
				else return namespace_uri(na.node());
			}

			case ast_func_namespace_uri_1:
			{
				xpath_node_set ns = m_left->eval_node_set(c);
				if (ns.empty()) return string_t();
				
				xpath_node na = ns.first();
				
				if (na.attribute()) return namespace_uri(na.attribute(), na.parent());
				else return namespace_uri(na.node());
			}

			case ast_func_string_0:
				return string_value(c.n);

			case ast_func_string_1:
				return m_left->eval_string(c);

			case ast_func_concat:
			{
				string_t r = m_left->eval_string(c);
				
				for (xpath_ast_node* n = m_right; n; n = n->m_next)
					r += n->eval_string(c);
			
				return r;
			}

			case ast_func_substring_before:
			{
				string_t s = m_left->eval_string(c);
				string_t::size_type pos = s.find(m_right->eval_string(c));
				
				if (pos == string_t::npos) return string_t();
				else return string_t(s.begin(), s.begin() + pos);
			}
			
			case ast_func_substring_after:
			{
				string_t s = m_left->eval_string(c);
				string_t p = m_right->eval_string(c);
				
				string_t::size_type pos = s.find(p);
				
				if (pos == string_t::npos) return string_t();
				else return string_t(s.begin() + pos + p.length(), s.end());
			}

			case ast_func_substring_2:
			{
				string_t s = m_left->eval_string(c);
				double first = round_nearest(m_right->eval_number(c));
				
				if (is_nan(first)) return string_t(); // NaN
				else if (first >= s.length() + 1) return string_t();
				
				size_t pos = first < 1 ? 1 : (size_t)first;
				
				return s.substr(pos - 1);
			}
			
			case ast_func_substring_3:
			{
				string_t s = m_left->eval_string(c);
				double first = round_nearest(m_right->eval_number(c));
				double last = first + round_nearest(m_third->eval_number(c));
				
				if (is_nan(first) || is_nan(last)) return string_t();
				else if (first >= s.length() + 1) return string_t();
				else if (first >= last) return string_t();
				
				size_t pos = first < 1 ? 1 : (size_t)first;
				size_t end = last >= s.length() + 1 ? s.length() + 1 : (size_t)last;

				size_t size_requested = end - pos;
				size_t size_to_end = s.length() - pos + 1;
				
				return s.substr(pos - 1, size_requested < size_to_end ? size_requested : size_to_end);
			}

			case ast_func_normalize_space_0:
			case ast_func_normalize_space_1:
			{
				string_t s = m_type == ast_func_normalize_space_0 ? string_value(c.n) : m_left->eval_string(c);
				
				string_t r;
				r.reserve(s.size());
				
				for (string_t::const_iterator it = s.begin(); it != s.end(); ++it)
				{
					if (IS_CHARTYPEX(*it, ctx_space))
					{
						if (!r.empty() && r[r.size() - 1] != ' ')
							r += ' ';
					}
					else r += *it;
				}
				
				string_t::size_type pos = r.find_last_not_of(' ');
				if (pos == string_t::npos) r = string_t();
				else r.erase(r.begin() + pos + 1, r.end());
			
				return r;
			}

			case ast_func_translate:
			{
				string_t s = m_left->eval_string(c);
				string_t from = m_right->eval_string(c);
				string_t to = m_third->eval_string(c);
				
				for (string_t::iterator it = s.begin(); it != s.end(); )
				{
					string_t::size_type pos = from.find(*it);
					
					if (pos == string_t::npos)
						++it;
					else if (pos >= to.length())
						it = s.erase(it);
					else
						*it++ = to[pos];
				}
				
				return s;
			}

			default:
			{
				switch (m_rettype)
				{
				case xpath_type_boolean:
					return eval_boolean(c) ? PUGIXML_TEXT("true") : PUGIXML_TEXT("false");
					
				case xpath_type_number:
					return convert_number_to_string(eval_number(c));
					
				case xpath_type_node_set:
				{
					xpath_node_set ns = eval_node_set(c);
					return ns.empty() ? string_t() : string_value(ns.first());
				}
				
				default:
					assert(!"Wrong expression for ret type string");
					return string_t();
				}
			}
			}
		}

		xpath_node_set eval_node_set(const xpath_context& c)
		{
			switch (m_type)
			{
			case ast_op_union:
			{
				xpath_node_set ls = m_left->eval_node_set(c);
				xpath_node_set rs = m_right->eval_node_set(c);
				
				// we can optimize merging two sorted sets, but this is a very rare operation, so don't bother
  		        ls.m_type = xpath_node_set::type_unsorted;

				ls.append(rs.begin(), rs.end());
				
				ls.remove_duplicates();
				
				return ls;
			}

			case ast_filter:
			case ast_filter_posinv:
			{
				xpath_node_set set = m_left->eval_node_set(c);

				// either expression is a number or it contains position() call; sort by document order
				if (m_type == ast_filter) set.sort();

				apply_predicate(set, 0, m_right, c);
			
				return set;
			}
			
			case ast_func_id:
				return xpath_node_set();
			
			case ast_step:
			{
				xpath_node_set ns;
				
				switch (m_axis)
				{
				case axis_ancestor:
					step_do(ns, c, axis_to_type<axis_ancestor>());
					break;
					
				case axis_ancestor_or_self:
					step_do(ns, c, axis_to_type<axis_ancestor_or_self>());
					break;

				case axis_attribute:
					step_do(ns, c, axis_to_type<axis_attribute>());
					break;

				case axis_child:
					step_do(ns, c, axis_to_type<axis_child>());
					break;
				
				case axis_descendant:
					step_do(ns, c, axis_to_type<axis_descendant>());
					break;

				case axis_descendant_or_self:
					step_do(ns, c, axis_to_type<axis_descendant_or_self>());
					break;

				case axis_following:
					step_do(ns, c, axis_to_type<axis_following>());
					break;
				
				case axis_following_sibling:
					step_do(ns, c, axis_to_type<axis_following_sibling>());
					break;
				
				case axis_namespace:
					step_do(ns, c, axis_to_type<axis_namespace>());
					break;
				
				case axis_parent:
					step_do(ns, c, axis_to_type<axis_parent>());
					break;
				
				case axis_preceding:
					step_do(ns, c, axis_to_type<axis_preceding>());
					break;

				case axis_preceding_sibling:
					step_do(ns, c, axis_to_type<axis_preceding_sibling>());
					break;
				
				case axis_self:
					step_do(ns, c, axis_to_type<axis_self>());
					break;
				}
				
				ns.remove_duplicates();
				
				return ns;
			}

			case ast_step_root:
			{
				xpath_node_set ns;
			
				if (c.root)
				{
  	            	ns.push_back(c.root);
				
					apply_predicates(ns, 0, c);
				}

				return ns;
			}

			default:
				assert(!"Wrong expression for ret type node set");
				return xpath_node_set();
			}
		}
		
		bool contains(ast_type_t type)
		{
			if (m_type == type) return true;
			
			switch (m_type)
			{
			case ast_op_or:
			case ast_op_and:
			case ast_op_equal:
			case ast_op_not_equal:
			case ast_op_less:
			case ast_op_greater:
			case ast_op_less_or_equal:
			case ast_op_greater_or_equal:
			case ast_op_add:
			case ast_op_subtract:
			case ast_op_multiply:
			case ast_op_divide:
			case ast_op_mod:
			case ast_op_negate:
				return m_left->contains(type) || m_right->contains(type);
			
			case ast_op_union:
			case ast_predicate:
			case ast_filter:
			case ast_filter_posinv:
				return false;
			
			case ast_string_constant:
			case ast_number_constant:
			case ast_func_last:
			case ast_func_position:
				return false;
		
			case ast_func_count:
			case ast_func_id:
			case ast_func_local_name_0:
			case ast_func_local_name_1:
			case ast_func_namespace_uri_0:
			case ast_func_namespace_uri_1:
			case ast_func_name_0:
			case ast_func_name_1:
			case ast_func_string_0:
			case ast_func_string_1:
				if (m_left) return m_left->contains(type);
				return false;
				
			case ast_func_concat:
			{
				if (m_left->contains(type)) return true;
				
				for (xpath_ast_node* n = m_right; n; n = n->m_next)
					if (n->contains(type)) return true;
					
				return false;
			}
			
			case ast_func_starts_with:
			case ast_func_contains:
			case ast_func_substring_before:
			case ast_func_substring_after:
			case ast_func_substring_2:
			case ast_func_substring_3:
			case ast_func_string_length_0:
			case ast_func_string_length_1:
			case ast_func_normalize_space_0:
			case ast_func_normalize_space_1:
			case ast_func_translate:
			case ast_func_boolean:
			case ast_func_not:
			case ast_func_true:
			case ast_func_false:
			case ast_func_lang:
			case ast_func_number_0:
			case ast_func_number_1:
			case ast_func_sum:
			case ast_func_floor:
			case ast_func_ceiling:
			case ast_func_round:
				if (m_left && m_left->contains(type)) return true;
				if (m_right && m_right->contains(type)) return true;
				if (m_third && m_third->contains(type)) return true;

				return false;
		
			case ast_step:
			case ast_step_root:
				return false;
			
			default:
				throw xpath_exception("Unknown semantics error");
#ifdef __DMC__				
				return false; // Digital Mars C++
#endif
			}
		}

		xpath_type_t rettype() const
		{
			return m_rettype;
		}
	};

	void* xpath_allocator::node()
	{
		return alloc(sizeof(xpath_ast_node));
	}

	class xpath_parser
	{
	private:
	    xpath_allocator& m_alloc;
	    xpath_lexer m_lexer;

		xpath_parser(const xpath_parser&);
		xpath_parser& operator=(const xpath_parser&);

		xpath_ast_node* parse_function_helper(ast_type_t type0, ast_type_t type1, size_t argc, xpath_ast_node* args[4])
		{
			assert(argc <= 1);

			if (argc == 1 && args[0]->rettype() != xpath_type_node_set) throw xpath_exception("Function has to be applied to node set");

			return new (m_alloc.node()) xpath_ast_node(argc == 0 ? type0 : type1, xpath_type_string, args[0]);
		}

		xpath_ast_node* parse_function(const xpath_lexer_string& name, size_t argc, xpath_ast_node* args[4])
		{
			switch (name.begin[0])
			{
			case 'b':
				if (name == PUGIXML_TEXT("boolean") && argc == 1)
					return new (m_alloc.node()) xpath_ast_node(ast_func_boolean, xpath_type_boolean, args[0]);
					
				break;
			
			case 'c':
				if (name == PUGIXML_TEXT("count") && argc == 1)
				{
					if (args[0]->rettype() != xpath_type_node_set) throw xpath_exception("count() has to be applied to node set");
					return new (m_alloc.node()) xpath_ast_node(ast_func_count, xpath_type_number, args[0]);
				}
				else if (name == PUGIXML_TEXT("contains") && argc == 2)
					return new (m_alloc.node()) xpath_ast_node(ast_func_contains, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("concat") && argc == 2)
					return new (m_alloc.node()) xpath_ast_node(ast_func_concat, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("ceiling") && argc == 1)
					return new (m_alloc.node()) xpath_ast_node(ast_func_ceiling, xpath_type_number, args[0]);
					
				break;
			
			case 'f':
				if (name == PUGIXML_TEXT("false") && argc == 0)
					return new (m_alloc.node()) xpath_ast_node(ast_func_false, xpath_type_boolean);
				else if (name == PUGIXML_TEXT("floor") && argc == 1)
					return new (m_alloc.node()) xpath_ast_node(ast_func_floor, xpath_type_number, args[0]);
					
				break;
			
			case 'i':
				if (name == PUGIXML_TEXT("id") && argc == 1)
					return new (m_alloc.node()) xpath_ast_node(ast_func_id, xpath_type_node_set, args[0]);
					
				break;
			
			case 'l':
				if (name == PUGIXML_TEXT("last") && argc == 0)
					return new (m_alloc.node()) xpath_ast_node(ast_func_last, xpath_type_number);
				else if (name == PUGIXML_TEXT("lang") && argc == 1)
					return new (m_alloc.node()) xpath_ast_node(ast_func_lang, xpath_type_boolean, args[0]);
				else if (name == PUGIXML_TEXT("local-name") && argc <= 1)
					return parse_function_helper(ast_func_local_name_0, ast_func_local_name_1, argc, args);
			
				break;
			
			case 'n':
				if (name == PUGIXML_TEXT("name") && argc <= 1)
					return parse_function_helper(ast_func_name_0, ast_func_name_1, argc, args);
				else if (name == PUGIXML_TEXT("namespace-uri") && argc <= 1)
					return parse_function_helper(ast_func_namespace_uri_0, ast_func_namespace_uri_1, argc, args);
				else if (name == PUGIXML_TEXT("normalize-space") && argc <= 1)
					return new (m_alloc.node()) xpath_ast_node(argc == 0 ? ast_func_normalize_space_0 : ast_func_normalize_space_1, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("not") && argc == 1)
					return new (m_alloc.node()) xpath_ast_node(ast_func_not, xpath_type_boolean, args[0]);
				else if (name == PUGIXML_TEXT("number") && argc <= 1)
					return new (m_alloc.node()) xpath_ast_node(argc == 0 ? ast_func_number_0 : ast_func_number_1, xpath_type_number, args[0]);
			
				break;
			
			case 'p':
				if (name == PUGIXML_TEXT("position") && argc == 0)
					return new (m_alloc.node()) xpath_ast_node(ast_func_position, xpath_type_number);
				
				break;
			
			case 'r':
				if (name == PUGIXML_TEXT("round") && argc == 1)
					return new (m_alloc.node()) xpath_ast_node(ast_func_round, xpath_type_number, args[0]);

				break;
			
			case 's':
				if (name == PUGIXML_TEXT("string") && argc <= 1)
					return new (m_alloc.node()) xpath_ast_node(argc == 0 ? ast_func_string_0 : ast_func_string_1, xpath_type_string, args[0]);
				else if (name == PUGIXML_TEXT("string-length") && argc <= 1)
					return new (m_alloc.node()) xpath_ast_node(argc == 0 ? ast_func_string_length_0 : ast_func_string_length_1, xpath_type_string, args[0]);
				else if (name == PUGIXML_TEXT("starts-with") && argc == 2)
					return new (m_alloc.node()) xpath_ast_node(ast_func_starts_with, xpath_type_boolean, args[0], args[1]);
				else if (name == PUGIXML_TEXT("substring-before") && argc == 2)
					return new (m_alloc.node()) xpath_ast_node(ast_func_substring_before, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("substring-after") && argc == 2)
					return new (m_alloc.node()) xpath_ast_node(ast_func_substring_after, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("substring") && (argc == 2 || argc == 3))
					return new (m_alloc.node()) xpath_ast_node(argc == 2 ? ast_func_substring_2 : ast_func_substring_3, xpath_type_string, args[0], args[1], args[2]);
				else if (name == PUGIXML_TEXT("sum") && argc == 1)
				{
					if (args[0]->rettype() != xpath_type_node_set) throw xpath_exception("sum() has to be applied to node set");
					return new (m_alloc.node()) xpath_ast_node(ast_func_sum, xpath_type_number, args[0]);
				}

				break;
			
			case 't':
				if (name == PUGIXML_TEXT("translate") && argc == 3)
					return new (m_alloc.node()) xpath_ast_node(ast_func_translate, xpath_type_string, args[0], args[1], args[2]);
				else if (name == PUGIXML_TEXT("true") && argc == 0)
					return new (m_alloc.node()) xpath_ast_node(ast_func_true, xpath_type_boolean);
					
				break;
			}

			throw xpath_exception("Unrecognized function or wrong parameter count");
		}

		axis_t parse_axis_name(const xpath_lexer_string& name, bool& specified)
		{
			specified = true;

			switch (name.begin[0])
			{
			case 'a':
				if (name == PUGIXML_TEXT("ancestor"))
					return axis_ancestor;
				else if (name == PUGIXML_TEXT("ancestor-or-self"))
					return axis_ancestor_or_self;
				else if (name == PUGIXML_TEXT("attribute"))
					return axis_attribute;
				
				break;
			
			case 'c':
				if (name == PUGIXML_TEXT("child"))
					return axis_child;
				
				break;
			
			case 'd':
				if (name == PUGIXML_TEXT("descendant"))
					return axis_descendant;
				else if (name == PUGIXML_TEXT("descendant-or-self"))
					return axis_descendant_or_self;
				
				break;
			
			case 'f':
				if (name == PUGIXML_TEXT("following"))
					return axis_following;
				else if (name == PUGIXML_TEXT("following-sibling"))
					return axis_following_sibling;
				
				break;
			
			case 'n':
				if (name == PUGIXML_TEXT("namespace"))
					return axis_namespace;
				
				break;
			
			case 'p':
				if (name == PUGIXML_TEXT("parent"))
					return axis_parent;
				else if (name == PUGIXML_TEXT("preceding"))
					return axis_preceding;
				else if (name == PUGIXML_TEXT("preceding-sibling"))
					return axis_preceding_sibling;
				
				break;
			
			case 's':
				if (name == PUGIXML_TEXT("self"))
					return axis_self;
				
				break;
			}

			specified = false;
			return axis_child;
		}

		nodetest_t parse_node_test_type(const xpath_lexer_string& name)
		{
			switch (name.begin[0])
			{
			case 'c':
				if (name == PUGIXML_TEXT("comment"))
					return nodetest_type_comment;

				break;

			case 'n':
				if (name == PUGIXML_TEXT("node"))
					return nodetest_type_node;

				break;

			case 'p':
				if (name == PUGIXML_TEXT("processing-instruction"))
					return nodetest_type_pi;

				break;

			case 't':
				if (name == PUGIXML_TEXT("text"))
					return nodetest_type_text;

				break;
			}

			return nodetest_none;
		}

	    // PrimaryExpr ::= VariableReference | '(' Expr ')' | Literal | Number | FunctionCall
	    xpath_ast_node* parse_primary_expression()
	    {
	    	switch (m_lexer.current())
	    	{
	    	case lex_var_ref:
	    	{
				throw xpath_exception("variables are not supported");

			#ifdef __DMC__	    	
	    		return 0; // Digital Mars C++
			#endif
			}

			case lex_open_brace:
			{
				m_lexer.next();

				xpath_ast_node* n = parse_expression();

				if (m_lexer.current() != lex_close_brace)
					throw xpath_exception("unmatched braces");

				m_lexer.next();

				return n;
			}

			case lex_quoted_string:
			{
				xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_string_constant, xpath_type_string, m_lexer.contents(), m_alloc);
				m_lexer.next();

				return n;
			}

			case lex_number:
			{
				xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_number_constant, xpath_type_number, m_lexer.contents(), m_alloc);
				m_lexer.next();

				return n;
			}

			case lex_string:
			{
				xpath_ast_node* args[4] = {};
				size_t argc = 0;
				
				xpath_lexer_string function = m_lexer.contents();
				m_lexer.next();
				
				bool func_concat = (function == PUGIXML_TEXT("concat"));
				xpath_ast_node* last_concat = 0;
				
				if (m_lexer.current() != lex_open_brace)
					throw xpath_exception("Unrecognized function call");
				m_lexer.next();

				if (m_lexer.current() != lex_close_brace)
					args[argc++] = parse_expression();

				while (m_lexer.current() != lex_close_brace)
				{
					if (m_lexer.current() != lex_comma)
						throw xpath_exception("no comma between function arguments");
					m_lexer.next();
					
					xpath_ast_node* n = parse_expression();
					
					if (func_concat)
					{
						if (argc < 2) args[argc++] = last_concat = n;
						else
						{
							last_concat->set_next(n);
							last_concat = n;
						}
					}
					else if (argc >= 4)
						throw xpath_exception("Too many function arguments");
					else
						args[argc++] = n;
				}
				
				m_lexer.next();

				return parse_function(function, argc, args);
			}

	    	default:
	    		throw xpath_exception("unrecognizable primary expression");
			#ifdef __DMC__	    	
	    		return 0; // Digital Mars C++
			#endif
	    	}
	    }
	    
	    // FilterExpr ::= PrimaryExpr | FilterExpr Predicate
	    // Predicate ::= '[' PredicateExpr ']'
	    // PredicateExpr ::= Expr
	    xpath_ast_node* parse_filter_expression()
	    {
	    	xpath_ast_node* n = parse_primary_expression();

	    	while (m_lexer.current() == lex_open_square_brace)
	    	{
	    		m_lexer.next();

				xpath_ast_node* expr = parse_expression();

				if (n->rettype() != xpath_type_node_set) throw xpath_exception("Predicate has to be applied to node set");

				bool posinv = expr->rettype() != xpath_type_number && !expr->contains(ast_func_position);

	    		n = new (m_alloc.node()) xpath_ast_node(posinv ? ast_filter_posinv : ast_filter, n, expr, axis_child);

	    		if (m_lexer.current() != lex_close_square_brace)
	    			throw xpath_exception("Unmatched square brace");
	    	
	    		m_lexer.next();
	    	}
	    	
	    	return n;
	    }
	    
	    // Step ::= AxisSpecifier NodeTest Predicate* | AbbreviatedStep
	    // AxisSpecifier ::= AxisName '::' | '@'?
	    // NodeTest ::= NameTest | NodeType '(' ')' | 'processing-instruction' '(' Literal ')'
	    // NameTest ::= '*' | NCName ':' '*' | QName
	    // AbbreviatedStep ::= '.' | '..'
	    xpath_ast_node* parse_step(xpath_ast_node* set)
	    {
			if (set && set->rettype() != xpath_type_node_set)
				throw xpath_exception("Step has to be applied to node set");

			bool axis_specified = false;
			axis_t axis = axis_child; // implied child axis

			if (m_lexer.current() == lex_axis_attribute)
			{
				axis = axis_attribute;
				axis_specified = true;
				
				m_lexer.next();
			}
			else if (m_lexer.current() == lex_dot)
			{
				m_lexer.next();
				
				return new (m_alloc.node()) xpath_ast_node(ast_step, set, axis_self, nodetest_type_node, xpath_lexer_string(), m_alloc);
			}
			else if (m_lexer.current() == lex_double_dot)
			{
				m_lexer.next();
				
				return new (m_alloc.node()) xpath_ast_node(ast_step, set, axis_parent, nodetest_type_node, xpath_lexer_string(), m_alloc);
			}
	    
			nodetest_t nt_type = nodetest_none;
			xpath_lexer_string nt_name;
			
			if (m_lexer.current() == lex_string)
			{
				// node name test
				nt_name = m_lexer.contents();
				m_lexer.next();

				// was it an axis name?
				if (m_lexer.current() == lex_double_colon)
				{
					// parse axis name
					if (axis_specified) throw xpath_exception("Two axis specifiers in one step");

					axis = parse_axis_name(nt_name, axis_specified);

					if (!axis_specified) throw xpath_exception("Unknown axis");

					// read actual node test
					m_lexer.next();

					if (m_lexer.current() == lex_multiply)
					{
						nt_type = nodetest_all;
						nt_name = xpath_lexer_string();
						m_lexer.next();
					}
					else if (m_lexer.current() == lex_string)
					{
						nt_name = m_lexer.contents();
						m_lexer.next();
					}
					else throw xpath_exception("Unrecognized node test");
				}
				
				if (nt_type == nodetest_none)
				{
					// node type test or processing-instruction
					if (m_lexer.current() == lex_open_brace)
					{
						m_lexer.next();
						
						if (m_lexer.current() == lex_close_brace)
						{
							m_lexer.next();

							nt_type = parse_node_test_type(nt_name);

							if (nt_type == nodetest_none) throw xpath_exception("Unrecognized node type");
							
							nt_name = xpath_lexer_string();
						}
						else if (nt_name == PUGIXML_TEXT("processing-instruction"))
						{
							if (m_lexer.current() != lex_quoted_string)
								throw xpath_exception("Only literals are allowed as arguments to processing-instruction()");
						
							nt_type = nodetest_pi;
							nt_name = m_lexer.contents();
							m_lexer.next();
							
							if (m_lexer.current() != lex_close_brace)
								throw xpath_exception("Unmatched brace near processing-instruction()");
							m_lexer.next();
						}
						else
							throw xpath_exception("Unmatched brace near node type test");

					}
					// QName or NCName:*
					else
					{
						const char_t* colon_pos = std::char_traits<char_t>::find(nt_name.begin, static_cast<size_t>(nt_name.end - nt_name.begin), ':');

						if (colon_pos && colon_pos + 2 == nt_name.end && colon_pos[1] == '*') // NCName:*
						{
							nt_name.end--; // erase *
							
							nt_type = nodetest_all_in_namespace;
						}
						else nt_type = nodetest_name;
					}
				}
			}
			else if (m_lexer.current() == lex_multiply)
			{
				nt_type = nodetest_all;
				m_lexer.next();
			}
			else throw xpath_exception("Unrecognized node test");
			
			xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_step, set, axis, nt_type, nt_name, m_alloc);
			
			xpath_ast_node* last = 0;
			
			while (m_lexer.current() == lex_open_square_brace)
			{
				m_lexer.next();
				
				xpath_ast_node* expr = parse_expression();

				xpath_ast_node* pred = new (m_alloc.node()) xpath_ast_node(ast_predicate, expr, 0, axis);
				
				if (m_lexer.current() != lex_close_square_brace)
	    			throw xpath_exception("unmatched square brace");
				m_lexer.next();
				
				if (last) last->set_next(pred);
				else n->set_right(pred);
				
				last = pred;
			}
			
			return n;
	    }
	    
	    // RelativeLocationPath ::= Step | RelativeLocationPath '/' Step | RelativeLocationPath '//' Step
	    xpath_ast_node* parse_relative_location_path(xpath_ast_node* set)
	    {
			xpath_ast_node* n = parse_step(set);
			
			while (m_lexer.current() == lex_slash || m_lexer.current() == lex_double_slash)
			{
				lexeme_t l = m_lexer.current();
				m_lexer.next();

				if (l == lex_double_slash)
					n = new (m_alloc.node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, xpath_lexer_string(), m_alloc);
				
				n = parse_step(n);
			}
			
			return n;
	    }
	    
	    // LocationPath ::= RelativeLocationPath | AbsoluteLocationPath
	    // AbsoluteLocationPath ::= '/' RelativeLocationPath? | '//' RelativeLocationPath
	    xpath_ast_node* parse_location_path()
	    {
			if (m_lexer.current() == lex_slash)
			{
				// Save state for next lexeme - that is, whatever follows '/'
				const char_t* state = 0; // gcc3 "variable might be used uninitialized in this function" bug workaround
				state = m_lexer.state();
				
				m_lexer.next();
				
				xpath_ast_node* n = 0; // gcc3 "variable might be used uninitialized in this function" bug workaround
				n = new (m_alloc.node()) xpath_ast_node(ast_step_root, xpath_type_node_set);
				
				try
				{
					n = parse_relative_location_path(n);
				}
				catch (const xpath_exception&)
				{
					m_lexer.reset(state);
				}
				
				return n;
			}
			else if (m_lexer.current() == lex_double_slash)
			{
				m_lexer.next();
				
				xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_step_root, xpath_type_node_set);
				n = new (m_alloc.node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, xpath_lexer_string(), m_alloc);
				
				return parse_relative_location_path(n);
			}
			else
			{
				return parse_relative_location_path(0);
			}
	    }
	    
	    // PathExpr ::= LocationPath
	    //				| FilterExpr
	    //				| FilterExpr '/' RelativeLocationPath
	    //				| FilterExpr '//' RelativeLocationPath
	    xpath_ast_node* parse_path_expression()
	    {
			// Clarification.
			// PathExpr begins with either LocationPath or FilterExpr.
			// FilterExpr begins with PrimaryExpr
			// PrimaryExpr begins with '$' in case of it being a variable reference,
			// '(' in case of it being an expression, string literal, number constant or
			// function call.

			if (m_lexer.current() == lex_var_ref || m_lexer.current() == lex_open_brace || 
				m_lexer.current() == lex_quoted_string || m_lexer.current() == lex_number ||
				m_lexer.current() == lex_string)
	    	{
	    		if (m_lexer.current() == lex_string)
	    		{
	    			// This is either a function call, or not - if not, we shall proceed with location path
	    			const char_t* state = m_lexer.state();
	    			
					while (IS_CHARTYPEX(*state, ctx_space)) ++state;
	    			
	    			if (*state != '(') return parse_location_path();

					// This looks like a function call; however this still can be a node-test. Check it.
					if (parse_node_test_type(m_lexer.contents()) != nodetest_none) return parse_location_path();
	    		}
	    		
	    		xpath_ast_node* n = parse_filter_expression();

	    		if (m_lexer.current() == lex_slash || m_lexer.current() == lex_double_slash)
	    		{
					lexeme_t l = m_lexer.current();
	    			m_lexer.next();
	    			
					if (l == lex_double_slash)
					{
						if (n->rettype() != xpath_type_node_set) throw xpath_exception("Step has to be applied to node set");

						n = new (m_alloc.node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, xpath_lexer_string(), m_alloc);
					}
	
	    			// select from location path
	    			return parse_relative_location_path(n);
	    		}

	    		return n;
	    	}
	    	else return parse_location_path();
	    }

	    // UnionExpr ::= PathExpr | UnionExpr '|' PathExpr
	    xpath_ast_node* parse_union_expression()
	    {
	    	xpath_ast_node* n = parse_path_expression();

	    	while (m_lexer.current() == lex_union)
	    	{
	    		m_lexer.next();

				xpath_ast_node* expr = parse_union_expression();

				if (n->rettype() != xpath_type_node_set || expr->rettype() != xpath_type_node_set)
					throw xpath_exception("Union operator has to be applied to node sets");

	    		n = new (m_alloc.node()) xpath_ast_node(ast_op_union, xpath_type_node_set, n, expr);
	    	}

	    	return n;
	    }

	    // UnaryExpr ::= UnionExpr | '-' UnaryExpr
	    xpath_ast_node* parse_unary_expression()
	    {
	    	if (m_lexer.current() == lex_minus)
	    	{
	    		m_lexer.next();

				xpath_ast_node* expr = parse_unary_expression();

	    		return new (m_alloc.node()) xpath_ast_node(ast_op_negate, xpath_type_number, expr);
	    	}
	    	else return parse_union_expression();
	    }
	    
	    // MultiplicativeExpr ::= UnaryExpr
	    //						  | MultiplicativeExpr '*' UnaryExpr
	    //						  | MultiplicativeExpr 'div' UnaryExpr
	    //						  | MultiplicativeExpr 'mod' UnaryExpr
	    xpath_ast_node* parse_multiplicative_expression()
	    {
	    	xpath_ast_node* n = parse_unary_expression();

	    	while (m_lexer.current() == lex_multiply || (m_lexer.current() == lex_string &&
	    		   (m_lexer.contents() == PUGIXML_TEXT("mod") || m_lexer.contents() == PUGIXML_TEXT("div"))))
	    	{
	    		ast_type_t op = m_lexer.current() == lex_multiply ? ast_op_multiply :
	    			m_lexer.contents().begin[0] == 'd' ? ast_op_divide : ast_op_mod;
	    		m_lexer.next();

				xpath_ast_node* expr = parse_unary_expression();

	    		n = new (m_alloc.node()) xpath_ast_node(op, xpath_type_number, n, expr);
	    	}

	    	return n;
	    }

	    // AdditiveExpr ::= MultiplicativeExpr
	    //					| AdditiveExpr '+' MultiplicativeExpr
	    //					| AdditiveExpr '-' MultiplicativeExpr
	    xpath_ast_node* parse_additive_expression()
	    {
	    	xpath_ast_node* n = parse_multiplicative_expression();

	    	while (m_lexer.current() == lex_plus || m_lexer.current() == lex_minus)
	    	{
	    		lexeme_t l = m_lexer.current();

	    		m_lexer.next();

				xpath_ast_node* expr = parse_multiplicative_expression();

	    		n = new (m_alloc.node()) xpath_ast_node(l == lex_plus ? ast_op_add : ast_op_subtract, xpath_type_number, n, expr);
	    	}

	    	return n;
	    }

	    // RelationalExpr ::= AdditiveExpr
	    //					  | RelationalExpr '<' AdditiveExpr
	    //					  | RelationalExpr '>' AdditiveExpr
	    //					  | RelationalExpr '<=' AdditiveExpr
	    //					  | RelationalExpr '>=' AdditiveExpr
	    xpath_ast_node* parse_relational_expression()
	    {
	    	xpath_ast_node* n = parse_additive_expression();

	    	while (m_lexer.current() == lex_less || m_lexer.current() == lex_less_or_equal || 
	    		   m_lexer.current() == lex_greater || m_lexer.current() == lex_greater_or_equal)
	    	{
	    		lexeme_t l = m_lexer.current();
	    		m_lexer.next();

				xpath_ast_node* expr = parse_additive_expression();

	    		n = new (m_alloc.node()) xpath_ast_node(l == lex_less ? ast_op_less : l == lex_greater ? ast_op_greater :
	    						l == lex_less_or_equal ? ast_op_less_or_equal : ast_op_greater_or_equal, xpath_type_boolean, n, expr);
	    	}

	    	return n;
	    }
	    
	    // EqualityExpr ::= RelationalExpr
	    //					| EqualityExpr '=' RelationalExpr
	    //					| EqualityExpr '!=' RelationalExpr
	    xpath_ast_node* parse_equality_expression()
	    {
	    	xpath_ast_node* n = parse_relational_expression();

	    	while (m_lexer.current() == lex_equal || m_lexer.current() == lex_not_equal)
	    	{
	    		lexeme_t l = m_lexer.current();

	    		m_lexer.next();

				xpath_ast_node* expr = parse_relational_expression();

	    		n = new (m_alloc.node()) xpath_ast_node(l == lex_equal ? ast_op_equal : ast_op_not_equal, xpath_type_boolean, n, expr);
	    	}

	    	return n;
	    }
	    
	    // AndExpr ::= EqualityExpr | AndExpr 'and' EqualityExpr
	    xpath_ast_node* parse_and_expression()
	    {
	    	xpath_ast_node* n = parse_equality_expression();

	    	while (m_lexer.current() == lex_string && m_lexer.contents() == PUGIXML_TEXT("and"))
	    	{
	    		m_lexer.next();

				xpath_ast_node* expr = parse_equality_expression();

	    		n = new (m_alloc.node()) xpath_ast_node(ast_op_and, xpath_type_boolean, n, expr);
	    	}

	    	return n;
	    }

	    // OrExpr ::= AndExpr | OrExpr 'or' AndExpr
	    xpath_ast_node* parse_or_expression()
	    {
	    	xpath_ast_node* n = parse_and_expression();

	    	while (m_lexer.current() == lex_string && m_lexer.contents() == PUGIXML_TEXT("or"))
	    	{
	    		m_lexer.next();

				xpath_ast_node* expr = parse_and_expression();

	    		n = new (m_alloc.node()) xpath_ast_node(ast_op_or, xpath_type_boolean, n, expr);
	    	}

	    	return n;
	    }
		
		// Expr ::= OrExpr
		xpath_ast_node* parse_expression()
		{
			return parse_or_expression();
		}

	public:
		explicit xpath_parser(const char_t* query, xpath_allocator& alloc): m_alloc(alloc), m_lexer(query)
		{
		}

		xpath_ast_node* parse()
		{
			xpath_ast_node* result = parse_expression();
			
			if (m_lexer.current() != lex_eof)
			{
				// there are still unparsed tokens left, error
				throw xpath_exception("Incorrect query");
			}
			
			return result;
		}
	};

	xpath_query::xpath_query(const char_t* query): m_alloc(0), m_root(0)
	{
		compile(query);
	}

	xpath_query::~xpath_query()
	{
		delete m_alloc;
	}

	void xpath_query::compile(const char_t* query)
	{
		delete m_alloc;
		m_alloc = new xpath_allocator;

		xpath_parser p(query, *m_alloc);

		m_root = p.parse();
	}

	xpath_type_t xpath_query::return_type() const
	{
		if (!m_root) return xpath_type_none;

		return m_root->rettype();
	}

	bool xpath_query::evaluate_boolean(const xml_node& n) const
	{
		if (!m_root) return false;
		
		xpath_context c;
		
		c.root = n.root();
		c.n = n;
		c.position = 1;
		c.size = 1;
		
		return m_root->eval_boolean(c);
	}
	
	double xpath_query::evaluate_number(const xml_node& n) const
	{
		if (!m_root) return gen_nan();
		
		xpath_context c;
		
		c.root = n.root();
		c.n = n;
		c.position = 1;
		c.size = 1;
		
		return m_root->eval_number(c);
	}
	
	string_t xpath_query::evaluate_string(const xml_node& n) const
	{
		if (!m_root) return string_t();
		
		xpath_context c;
		
		c.root = n.root();
		c.n = n;
		c.position = 1;
		c.size = 1;
		
		return m_root->eval_string(c);
	}
	
	xpath_node_set xpath_query::evaluate_node_set(const xml_node& n) const
	{
		if (!m_root) return xpath_node_set();
		if (m_root->rettype() != xpath_type_node_set) throw xpath_exception("Expression does not evaluate to node set");
		
		xpath_context c;
		
		c.root = n.root();
		c.n = n;
		c.position = 1;
		c.size = 1;
		
		return m_root->eval_node_set(c);
	}

	xpath_node xml_node::select_single_node(const char_t* query) const
	{
		xpath_query q(query);
		return select_single_node(q);
	}

	xpath_node xml_node::select_single_node(const xpath_query& query) const
	{
		xpath_node_set s = query.evaluate_node_set(*this);
		return s.empty() ? xpath_node() : s.first();
	}

	xpath_node_set xml_node::select_nodes(const char_t* query) const
	{
		xpath_query q(query);
		return select_nodes(q);
	}

	xpath_node_set xml_node::select_nodes(const xpath_query& query) const
	{
		return query.evaluate_node_set(*this);
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
