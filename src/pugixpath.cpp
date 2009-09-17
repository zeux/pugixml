///////////////////////////////////////////////////////////////////////////////
//
// Pug Improved XML Parser - Version 0.41
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

#ifndef PUGIXML_NO_XPATH

#include <algorithm>

#include <assert.h>

#include <stdio.h>
#include <math.h>
#include <float.h>
#include <ctype.h>
#include <string.h>

#if defined(_MSC_VER)
#	pragma warning(disable: 4127) // conditional expression is constant
#	pragma warning(disable: 4702) // unreachable code
#	pragma warning(disable: 4996) // this function or variable may be unsafe
#endif

namespace
{
	using namespace pugi;
		
	enum chartype
	{
		ct_space = 1,			// \r, \n, space, tab
		ct_start_symbol = 2,	// Any symbol > 127, a-z, A-Z, _, :
		ct_digit = 4,			// 0-9
		ct_symbol = 8			// Any symbol > 127, a-z, A-Z, 0-9, _, :, -, .
	};
	
	const unsigned char chartype_table[256] =
	{
		0,  0,  0,  0,  0,  0,  0,  0,     0,  1,  1,  0,  0,  1,  0,  0,     // 0-15
		0,  0,  0,  0,  0,  0,  0,  0,     0,  0,  0,  0,  0,  0,  0,  0,     // 16-31
		1,  0,  0,  0,  0,  0,  0,  0,     0,  0,  0,  0,  0,  8,  8,  0,     // 32-47
		12, 12, 12, 12, 12, 12, 12, 12,    12, 12, 10, 0,  0,  0,  0,  0,     // 48-63
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
	
	bool is_chartype(char c, chartype ct)
	{
		return !!(chartype_table[static_cast<unsigned char>(c)] & ct);
	}

	bool starts_with(const std::string& s, const char* pattern)
	{
		return s.compare(0, strlen(pattern), pattern) == 0;
	}

	std::string string_value(const xpath_node& na)
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
				std::string result;

				xml_node c = n.first_child();
				
				while (c)
				{
					if (c.type() == node_pcdata || c.type() == node_cdata)
						result += c.value();
				
					if (c.first_child())
						c = c.first_child();
					else if (c.next_sibling())
						c = c.next_sibling();
					else
					{
						while (c && c != n) c = c.parent();
						
						if (c == n) break;
						
						c = c.next_sibling();
					}
				}
				
				return result;
			}
			
			default:
				return "";
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
    
	struct document_order_comparator
	{
		bool operator()(const xpath_node& lhs, const xpath_node& rhs) const
		{
			unsigned int lo = lhs.attribute() ? lhs.attribute().document_order() : lhs.node().document_order();
			unsigned int ro = rhs.attribute() ? rhs.attribute().document_order() : rhs.node().document_order();
			
			if (lo != 0 && ro != 0)
				return lo < ro;

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
	
	/* From trio
	 *
	 * Endian-agnostic indexing macro.
	 *
	 * The value of internalEndianMagic, when converted into a 64-bit
	 * integer, becomes 0x0706050403020100 (we could have used a 64-bit
	 * integer value instead of a double, but not all platforms supports
	 * that type). The value is automatically encoded with the correct
	 * endianess by the compiler, which means that we can support any
	 * kind of endianess. The individual bytes are then used as an index
	 * for the IEEE 754 bit-patterns and masks.
	 */
	#define DOUBLE_INDEX(x) (((unsigned char *)&internal_endian_magic)[7-(x)])

	static const double internal_endian_magic = 7.949928895127363e-275;

	static const unsigned char ieee_754_exponent_mask[] = { 0x7F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

	static const unsigned char ieee_754_mantissa_mask[] = { 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

	static const unsigned char ieee_754_qnan_array[] = { 0x7F, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
	
	bool is_special(double value, bool& has_mantissa)
	{
		bool is_special_quantity = true;

		has_mantissa = false;

		for (unsigned int i = 0; i < sizeof(double); ++i)
		{
			unsigned char current = ((unsigned char *)&value)[DOUBLE_INDEX(i)];
			
			is_special_quantity = is_special_quantity && (current & ieee_754_exponent_mask[i]) == ieee_754_exponent_mask[i];
			has_mantissa = has_mantissa || (current & ieee_754_mantissa_mask[i]) != 0;
		}
		
		return is_special_quantity;
	}

	double gen_nan()
	{
#if FLT_RADIX == 2 && DBL_MAX_EXP == 1024 && DBL_MANT_DIG == 53
		// IEEE 754
		
		double result = 0;

		for (unsigned int i = 0; i < sizeof(double); ++i)
		{
			((unsigned char *)&result)[DOUBLE_INDEX(i)] = ieee_754_qnan_array[i];
		}
		
		return result;
#else
		const volatile double zero = 0.0;
		return zero / zero;
#endif
	}
	
	bool is_nan(double value)
	{
#if defined(__USE_ISOC99)
		return isnan(value);
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && !defined(__COMO__)
		return !!_isnan(value);
#elif FLT_RADIX == 2 && DBL_MAX_EXP == 1024 && DBL_MANT_DIG == 53
		// IEEE 754

		bool has_mantissa;
		
		bool is_special_quantity = is_special(value, has_mantissa);
  
		return (is_special_quantity && has_mantissa);
#else
		return value != value;
#endif
	}
	
	bool is_inf(double value)
	{
#if defined(__USE_ISOC99)
		return !isfinite(value);
#elif (defined(_MSC_VER) || defined(__BORLANDC__)) && !defined(__COMO__)
		return !_finite(value);
#elif FLT_RADIX == 2 && DBL_MAX_EXP == 1024 && DBL_MANT_DIG == 53
		// IEEE 754
  
		bool has_mantissa;
		
		bool is_special_quantity = is_special(value, has_mantissa);
  
		return (is_special_quantity && !has_mantissa);
#else
		return value + 1 == value && value - 1 == value;
#endif
	}
	
	bool convert_number_to_boolean(double value)
	{
		return (value != 0 && !is_nan(value));
	}
	
	std::string convert_number_to_string(double value)
	{
		if (is_nan(value)) return "NaN";
		else if (is_inf(value)) return value < 0 ? "-Infinity" : "Infinity";
		
		char buf[100];
		
		if (value == (int)value) sprintf(buf, "%d", (int)value);
		else
		{
			sprintf(buf, "%f", value);
			
			// trim trailing zeros after decimal point
			if (strchr(buf, '.'))
			{
				char* ptr = buf + strlen(buf) - 1;
				for (; *ptr == '0'; --ptr) ;
				*(ptr+1) = 0;
			}
		}
		
		return std::string(buf);
	}
	
	double convert_string_to_number(const char* string)
	{
		while (is_chartype(*string, ct_space)) ++string;
		
		double sign = 1;
		
		if (*string == '-')
		{
			sign = -1;
			++string;
		}
		
		double r = 0;
		
		if (!*string) return gen_nan();
		
		while (is_chartype(*string, ct_digit))
		{
			r = r * 10 + (*string - '0');
			++string;
		}
		
		if (*string)
		{
			if (is_chartype(*string, ct_space))
			{
				while (is_chartype(*string, ct_space)) ++string;
				if (*string) return gen_nan();
			}
			
			if (*string != '.') return gen_nan();
			
			++string;
			
			double power = 0.1;
			
			while (is_chartype(*string, ct_digit))
			{
				r += power * (*string - '0');
				power /= 10;
				++string;
			}
			
			while (is_chartype(*string, ct_space)) ++string;
			if (*string) return gen_nan();
		}
		
		return r * sign;
	}
	
	double ieee754_round(double value)
	{
		return is_nan(value) ? value : floor(value + 0.5);
	}
	
	const char* local_name(const char* name)
	{
		const char* p = strchr(name, ':');
		
		return p ? p + 1 : name;
	}
	
	const char* namespace_uri(const xml_node& node)
	{
		const char* pos = strchr(node.name(), ':');
		
		std::string ns = "xmlns";
		
		if (pos)
		{
			ns += ':';
			ns.append(node.name(), pos);
		}
		
		xml_node p = node.parent();
		
		while (p)
		{
			xml_attribute a = p.attribute(ns.c_str());
			
			if (a) return a.value();
			
			p = p.parent();
		}
		
		return "";
	}

	const char* namespace_uri(const xml_attribute& attr, const xml_node& parent)
	{
		const char* pos = strchr(attr.name(), ':');
		
		// Default namespace does not apply to attributes
		if (!pos) return "";
		
		std::string ns = "xmlns:";
		ns.append(attr.name(), pos);
		
		xml_node p = parent;
		
		while (p)
		{
			xml_attribute a = p.attribute(ns.c_str());
			
			if (a) return a.value();
			
			p = p.parent();
		}
		
		return "";
	}

	template <class T> struct equal_to
	{
		bool operator()(const T& lhs, const T& rhs) const
		{
			return lhs == rhs;
		}
	};

	template <class T> struct not_equal_to
	{
		bool operator()(const T& lhs, const T& rhs) const
		{
			return lhs != rhs;
		}
	};

	template <class T> struct greater
	{
		bool operator()(const T& lhs, const T& rhs) const
		{
			return lhs > rhs;
		}
	};
	
	template <class T> struct less
	{
		bool operator()(const T& lhs, const T& rhs) const
		{
			return lhs < rhs;
		}
	};

	template <class T> struct greater_equal
	{
		bool operator()(const T& lhs, const T& rhs) const
		{
			return lhs >= rhs;
		}
	};

	template <class T> struct less_equal
	{
		bool operator()(const T& lhs, const T& rhs) const
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
	
	bool xpath_node::operator==(const xpath_node& n) const
	{
		return m_node == n.m_node && m_attribute == n.m_attribute;
	}
	
	bool xpath_node::operator!=(const xpath_node& n) const
	{
		return m_node != n.m_node || m_attribute != n.m_attribute;
	}

	xpath_node_set::xpath_node_set(): m_type(type_unsorted), m_begin(&m_storage), m_end(&m_storage), m_eos(&m_storage + 1), m_using_storage(true)
	{
	}

	xpath_node_set::~xpath_node_set()
	{
		if (!m_using_storage) delete[] m_begin;
	}
		
	xpath_node_set::xpath_node_set(const xpath_node_set& ns): m_type(type_unsorted), m_begin(&m_storage), m_end(&m_storage), m_eos(&m_storage + 1), m_using_storage(true)
	{
		*this = ns;
	}
	
	xpath_node_set& xpath_node_set::operator=(const xpath_node_set& ns)
	{
		if (!m_using_storage) delete[] m_begin;
		
		m_begin = m_end = m_eos = 0;
		
		if (ns.size() == 1)
		{
			m_storage = *ns.m_begin;
			m_begin = &m_storage;
			m_end = m_eos = &m_storage + 1;
			m_using_storage = true;
		}
		else
		{
			m_using_storage = false;
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
		
	xpath_node_set::iterator xpath_node_set::mut_begin()
	{
		return m_begin;
	}
	
	xpath_node_set::const_iterator xpath_node_set::begin() const
	{
		return m_begin;
	}
		
	xpath_node_set::iterator xpath_node_set::mut_end()
	{
		return m_end;
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

	template <typename Iterator> void xpath_node_set::append(Iterator begin, Iterator end)
	{
		size_t count = std::distance(begin, end);
		size_t size = m_end - m_begin;
		size_t capacity = m_eos - m_begin;
		
		if (capacity < size + count)
		{
			if (capacity < 2) capacity = 2;
			
			while (capacity < size + count) capacity += capacity / 2;
			
			xpath_node* storage = new xpath_node[capacity];
			std::copy(m_begin, m_end, storage);
			
			if (!m_using_storage) delete[] m_begin;
			
			m_using_storage = false;
			
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
		lex_double_dot
	};

	class xpath_lexer
	{
	private:
		const char* m_cur;

		char* m_cur_lexeme_contents;
		size_t m_clc_size;
		size_t m_clc_capacity;

		lexeme_t m_cur_lexeme;

		void contents_clear()
		{
			m_clc_size = 0;
		}

		void contents_push(char c)
		{
			if (m_clc_size == m_clc_capacity)
			{
				if (!m_clc_capacity) m_clc_capacity = 16;
				else m_clc_capacity *= 2;

				char* s = new char[m_clc_capacity + 1];
				if (m_cur_lexeme_contents) strcpy(s, m_cur_lexeme_contents);
				
				delete[] m_cur_lexeme_contents;
				m_cur_lexeme_contents = s;
			}

			m_cur_lexeme_contents[m_clc_size++] = c;
			m_cur_lexeme_contents[m_clc_size] = 0;
		}

	public:
		explicit xpath_lexer(const char* query): m_cur(query)
		{
			m_clc_capacity = m_clc_size = 0;
			m_cur_lexeme_contents = 0;

			next();
		}
		
		~xpath_lexer()
		{
			delete[] m_cur_lexeme_contents;
		}
		
		const char* state() const
		{
			return m_cur;
		}
		
		void reset(const char* state)
		{
			m_cur = state;
			next();
		}

		void next()
		{
			contents_clear();

			while (is_chartype(*m_cur, ct_space)) ++m_cur;

			switch (*m_cur)
			{
			case 0:
				m_cur_lexeme = lex_none;
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
				else if (is_chartype(*(m_cur+1), ct_digit))
				{
					contents_push('0');
					contents_push('.');

					++m_cur;

					while (is_chartype(*m_cur, ct_digit))
						contents_push(*m_cur++);
					
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
				char terminator = *m_cur;

				++m_cur;

				while (*m_cur && *m_cur != terminator)
					contents_push(*m_cur++);
				
				if (!*m_cur)
					m_cur_lexeme = lex_none;
				else
				{
					m_cur += 1;
					m_cur_lexeme = lex_quoted_string;
				}

				break;
			}

			default:
				if (is_chartype(*m_cur, ct_digit))
				{
					while (is_chartype(*m_cur, ct_digit))
						contents_push(*m_cur++);
				
					if (*m_cur == '.' && is_chartype(*(m_cur+1), ct_digit))
					{
						contents_push(*m_cur++);

						while (is_chartype(*m_cur, ct_digit))
							contents_push(*m_cur++);
					}

					m_cur_lexeme = lex_number;
				}
				else if (is_chartype(*m_cur, ct_start_symbol))
				{
					while (is_chartype(*m_cur, ct_symbol))
						contents_push(*m_cur++);
				
					while (is_chartype(*m_cur, ct_space)) ++m_cur;

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

		const char* contents() const
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
		ast_variable,					// variable value
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

	enum ast_rettype_t
	{
		ast_type_none,
		ast_type_node_set,
		ast_type_number,
		ast_type_string,
		ast_type_boolean
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
		
		ast_rettype_t m_rettype;

		// tree node structure
		xpath_ast_node* m_left;
		xpath_ast_node*	m_right;
		xpath_ast_node* m_third;
		xpath_ast_node* m_next;

		// variable name for ast_variable
		// string value for ast_constant
		// node test for ast_step (node name/namespace/node type/pi target)
		const char* m_contents;

		// for t_step / t_predicate
		axis_t m_axis;
		nodetest_t m_test;
		
		xpath_ast_node(const xpath_ast_node&);
		xpath_ast_node& operator=(const xpath_ast_node&);

		template <class Cbool, class Cdouble, class Cstring> struct compare_eq
		{
			static bool run(xpath_ast_node* lhs, xpath_ast_node* rhs, xpath_context& c)
			{
				if (lhs->rettype() != ast_type_node_set && rhs->rettype() != ast_type_node_set)
				{
					if (lhs->rettype() == ast_type_boolean || rhs->rettype() == ast_type_boolean)
						return Cbool()(lhs->eval_boolean(c), rhs->eval_boolean(c));
					else if (lhs->rettype() == ast_type_number || rhs->rettype() == ast_type_number)
						return Cdouble()(lhs->eval_number(c), rhs->eval_number(c));
					else if (lhs->rettype() == ast_type_string || rhs->rettype() == ast_type_string)
						return Cstring()(lhs->eval_string(c), rhs->eval_string(c));
					else
					{
						assert(!"Wrong types");
						return false;
					}
				}
				else if (lhs->rettype() == ast_type_node_set && rhs->rettype() == ast_type_node_set)
				{
					xpath_node_set ls = lhs->eval_node_set(c);
					xpath_node_set rs = rhs->eval_node_set(c);
					
					for (xpath_node_set::const_iterator li = ls.begin(); li != ls.end(); ++li)
					for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
					{
						if (Cstring()(string_value(*li), string_value(*ri)))
							return true;
					}
					
					return false;
				}
				else if (lhs->rettype() != ast_type_node_set && rhs->rettype() == ast_type_node_set)
				{
					if (lhs->rettype() == ast_type_boolean)
						return Cbool()(lhs->eval_boolean(c), rhs->eval_boolean(c));
					else if (lhs->rettype() == ast_type_number)
					{
						double l = lhs->eval_number(c);
						xpath_node_set rs = rhs->eval_node_set(c);
						
						for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
						{
							if (Cdouble()(l, convert_string_to_number(string_value(*ri).c_str())) == true)
								return true;
						}
						
						return false;
					}
					else if (lhs->rettype() == ast_type_string)
					{
						std::string l = lhs->eval_string(c);
						xpath_node_set rs = rhs->eval_node_set(c);

						for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
						{
							if (Cstring()(l, string_value(*ri)) == true)
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
				else if (lhs->rettype() == ast_type_node_set && rhs->rettype() != ast_type_node_set)
				{
					if (rhs->rettype() == ast_type_boolean)
						return Cbool()(lhs->eval_boolean(c), rhs->eval_boolean(c));
					else if (rhs->rettype() == ast_type_number)
					{
						xpath_node_set ls = lhs->eval_node_set(c);
						double r = rhs->eval_number(c);

						for (xpath_node_set::const_iterator li = ls.begin(); li != ls.end(); ++li)
						{
							if (Cdouble()(convert_string_to_number(string_value(*li).c_str()), r) == true)
								return true;
						}
						
						return false;
					}
					else if (rhs->rettype() == ast_type_string)
					{
						xpath_node_set ls = lhs->eval_node_set(c);
						std::string r = rhs->eval_string(c);

						for (xpath_node_set::const_iterator li = ls.begin(); li != ls.end(); ++li)
						{
							if (Cstring()(string_value(*li), r) == true)
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
				else
				{
					assert(!"Wrong types");
					return false;
				}
			}
		};

		template <class Cdouble> struct compare_rel
		{
			static bool run(xpath_ast_node* lhs, xpath_ast_node* rhs, xpath_context& c)
			{
				if (lhs->rettype() != ast_type_node_set && rhs->rettype() != ast_type_node_set)
					return Cdouble()(lhs->eval_number(c), rhs->eval_number(c));
				else if (lhs->rettype() == ast_type_node_set && rhs->rettype() == ast_type_node_set)
				{
					xpath_node_set ls = lhs->eval_node_set(c);
					xpath_node_set rs = rhs->eval_node_set(c);
					
					for (xpath_node_set::const_iterator li = ls.begin(); li != ls.end(); ++li)
					{
						double l = convert_string_to_number(string_value(*li).c_str());
						
						for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
						{
							if (Cdouble()(l, convert_string_to_number(string_value(*ri).c_str())) == true)
								return true;
						}
					}
					
					return false;
				}
				else if (lhs->rettype() != ast_type_node_set && rhs->rettype() == ast_type_node_set)
				{
					double l = lhs->eval_number(c);
					xpath_node_set rs = rhs->eval_node_set(c);
						
					for (xpath_node_set::const_iterator ri = rs.begin(); ri != rs.end(); ++ri)
					{
						if (Cdouble()(l, convert_string_to_number(string_value(*ri).c_str())) == true)
							return true;
					}
					
					return false;
				}
				else if (lhs->rettype() == ast_type_node_set && rhs->rettype() != ast_type_node_set)
				{
					xpath_node_set ls = lhs->eval_node_set(c);
					double r = rhs->eval_number(c);

					for (xpath_node_set::const_iterator li = ls.begin(); li != ls.end(); ++li)
					{
						if (Cdouble()(convert_string_to_number(string_value(*li).c_str()), r) == true)
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
		};

		void apply_predicate(xpath_node_set& ns, size_t first, xpath_ast_node* expr, const xpath_context& context)
		{
			xpath_context c;
			c.root = context.root;
			
			size_t i = 0;
			size_t size = ns.size() - first;
				
			xpath_node_set::iterator last = ns.mut_begin() + first;
				
			// remove_if... or well, sort of
			for (xpath_node_set::iterator it = last; it != ns.end(); ++it, ++i)
			{
				c.n = *it;
				c.position = i + 1;
				c.size = size;
				
				if (expr->rettype() == ast_type_number)
				{
					if (expr->eval_number(c) == i + 1)
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
			// There are no attribute nodes corresponding to attributes that declare namespaces
			// That is, "xmlns:..." or "xmlns"
			if (!strncmp(a.name(), "xmlns", 5) && (a.name()[5] == 0 || a.name()[5] == ':')) return;
			
			switch (m_test)
			{
			case nodetest_name:
				if (!strcmp(a.name(), m_contents)) ns.push_back(xpath_node(a, parent));
				break;
				
			case nodetest_type_node:
			case nodetest_all:
				ns.push_back(xpath_node(a, parent));
				break;
				
			case nodetest_all_in_namespace:
				if (!strncmp(a.name(), m_contents, strlen(m_contents)) && a.name()[strlen(m_contents)] == ':')
					ns.push_back(xpath_node(a, parent));
				break;
			
			default:
				;
			}
		}
		
		void step_push(xpath_node_set& ns, const xml_node& n)
		{
			switch (m_test)
			{
			case nodetest_name:
				if (n.type() == node_element && !strcmp(n.name(), m_contents)) ns.push_back(n);
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
				if (n.type() == node_pi && !strcmp(n.name(), m_contents))
					ns.push_back(n);
				break;
				
			case nodetest_all:
				if (n.type() == node_element)
					ns.push_back(n);
				break;
				
			case nodetest_all_in_namespace:
				if (n.type() == node_element && !strncmp(n.name(), m_contents, strlen(m_contents)) &&
					n.name()[strlen(m_contents)] == ':')
					ns.push_back(n);
				break;
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
							// leaf node
							step_push(ns, cur);
							
							if (cur.previous_sibling())
								cur = cur.previous_sibling();
							else
							{
								do 
								{
									cur = cur.parent();
									if (!cur) break;
								
									step_push(ns, cur);
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
				
			default:
				assert(!"Unimplemented axis");
			}
		}
		
		template <class T> void step_fill(xpath_node_set& ns, const xml_attribute& a, const xml_node& p, T)
		{
			const axis_t axis = T::axis;

			switch (axis)
			{
			case axis_ancestor:
			case axis_ancestor_or_self:
			{
				ns.m_type = ns.empty() ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_unsorted;

				if (axis == axis_ancestor_or_self)
					step_push(ns, a, p);

				xml_node cur = p;
				
				while (cur)
				{
					step_push(ns, cur);
					
					cur = cur.parent();
				}
				
				break;
			}
			
			default:
				assert(!"Unimplemented axis");
			}
		}
		
		template <class T> void step_do(xpath_node_set& ns, xpath_context& c, T v)
		{
			const axis_t axis = T::axis;

			switch (axis)
			{
			case axis_parent:
				if (m_left)
				{
					xpath_node_set s = m_left->eval_node_set(c);
					
					for (xpath_node_set::const_iterator it = s.begin(); it != s.end(); ++it)
					{
						xml_node p = it->parent();
						if (p)
						{
							size_t s = ns.size();
							
							step_push(ns, p);
							
							apply_predicates(ns, s, c);
						}
					}
				}
				else
				{
					xml_node p = c.n.parent();
					if (p)
					{
						step_push(ns, p);
						
						apply_predicates(ns, 0, c);
					}
				}

				break;
				
			case axis_self:
				if (m_left)
				{
					xpath_node_set s = m_left->eval_node_set(c);
					
					for (xpath_node_set::const_iterator it = s.begin(); it != s.end(); ++it)
					{
						size_t s = ns.size();
						
						if (it->attribute()) step_push(ns, it->attribute(), it->parent());
						else step_push(ns, it->node());
						
						apply_predicates(ns, s, c);
					}
				}
				else
				{
					if (c.n.node()) step_push(ns, c.n.node());
					else step_push(ns, c.n.attribute(), c.n.parent());
					
					apply_predicates(ns, 0, c);
				}

				break;
				
			case axis_namespace:
				break;
				
			case axis_ancestor:
			case axis_ancestor_or_self:
				if (m_left)
				{
					xpath_node_set s = m_left->eval_node_set(c);
							
					for (xpath_node_set::const_iterator it = s.begin(); it != s.end(); ++it)
					{
						size_t s = ns.size();
						
						if (it->node())
							step_fill(ns, it->node(), v);
						else
							step_fill(ns, it->attribute(), it->parent(), v);
							
						apply_predicates(ns, s, c);
					}
				}
				else
				{
					if (c.n.node()) step_fill(ns, c.n.node(), v);
					else step_fill(ns, c.n.attribute(), c.n.parent(), v);
					
					apply_predicates(ns, 0, c);
				}
				
				break;
		
			case axis_following:
			case axis_following_sibling:
			case axis_preceding:
			case axis_preceding_sibling:
			case axis_attribute:
			case axis_child:
			case axis_descendant:
			case axis_descendant_or_self:
				if (m_left)
				{
					xpath_node_set s = m_left->eval_node_set(c);
					
					for (xpath_node_set::const_iterator it = s.begin(); it != s.end(); ++it)
					{
						size_t s = ns.size();
						
						if (it->node())
							step_fill(ns, it->node(), v);
						
						apply_predicates(ns, s, c);
					}
				}
				else if (c.n.node())
				{
					step_fill(ns, c.n.node(), v);
					
					apply_predicates(ns, 0, c);
				}
				
				break;
			
			default:
				assert(!"Unimplemented axis");
			}
		}
		
		void set_contents(const char* value, xpath_allocator& a)
		{
			if (value)
			{
				char* c = static_cast<char*>(a.alloc(strlen(value) + 1));
				strcpy(c, value);
				m_contents = c;
			}
			else m_contents = 0;
		}
	public:
		xpath_ast_node(ast_type_t type, const char* contents, xpath_allocator& a): m_type(type), m_rettype(ast_type_none), m_contents(0)
		{
			set_contents(contents, a);
		}
		
		xpath_ast_node(ast_type_t type, xpath_ast_node* left, xpath_ast_node* right, axis_t axis): m_type(type),
			m_rettype(ast_type_none), m_left(left), m_right(right), m_third(0), m_next(0), m_contents(0),
			m_axis(axis)
		{
		}

		xpath_ast_node(ast_type_t type, xpath_ast_node* left = 0, xpath_ast_node* right = 0, xpath_ast_node* third = 0): m_type(type),
			m_rettype(ast_type_none), m_left(left), m_right(right), m_third(third), m_next(0), m_contents(0)
		{
		}

		xpath_ast_node(ast_type_t type, xpath_ast_node* left, axis_t axis, nodetest_t test, const char* contents, xpath_allocator& a):
			m_type(type), m_rettype(ast_type_none), m_left(left), m_right(0), m_third(0), m_next(0),
			m_contents(0), m_axis(axis), m_test(test)
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

		bool eval_boolean(xpath_context& c)
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
				return compare_eq<equal_to<bool>, equal_to<double>, equal_to<std::string> >::run(m_left, m_right, c);

			case ast_op_not_equal:
				return compare_eq<not_equal_to<bool>, not_equal_to<double>, not_equal_to<std::string> >::run(m_left, m_right, c);
	
			case ast_op_less:
				return compare_rel<less<double> >::run(m_left, m_right, c);
			
			case ast_op_greater:
				return compare_rel<greater<double> >::run(m_left, m_right, c);

			case ast_op_less_or_equal:
				return compare_rel<less_equal<double> >::run(m_left, m_right, c);
			
			case ast_op_greater_or_equal:
				return compare_rel<greater_equal<double> >::run(m_left, m_right, c);

			case ast_func_starts_with:
				return starts_with(m_left->eval_string(c), m_right->eval_string(c).c_str());

			case ast_func_contains:
				return m_left->eval_string(c).find(m_right->eval_string(c)) != std::string::npos;

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
				
				std::string lang = m_left->eval_string(c);
				
				xml_node n = c.n.node();
				
				while (n.type() != node_document)
				{
					xml_attribute a = n.attribute("xml:lang");
					
					if (a)
					{
						const char* value = a.value();
						
						// strnicmp / strncasecmp is not portable
						for (std::string::iterator it = lang.begin(); it != lang.end(); ++it)
						{
							if (tolower(*it) != tolower(*value)) return false;
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
				case ast_type_number:
					return convert_number_to_boolean(eval_number(c));
					
				case ast_type_string:
					return !eval_string(c).empty();
					
				case ast_type_node_set:				
					return !eval_node_set(c).empty();
					
				default:
					assert(!"Wrong expression for ret type boolean");
					return false;
				}
			}
			}
		}

		double eval_number(xpath_context& c)
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
				// correct except for negative zero (it returns positive zero instead of negative)
				return ieee754_round(m_left->eval_number(c));
			
			default:
			{
				switch (m_rettype)
				{
				case ast_type_boolean:
					return eval_boolean(c) ? 1 : 0;
					
				case ast_type_string:
					return convert_string_to_number(eval_string(c).c_str());
					
				case ast_type_node_set:
					return convert_string_to_number(eval_string(c).c_str());
					
				default:
					assert(!"Wrong expression for ret type number");
					return 0;
				}
				
			}
			}
		}
		
		std::string eval_string(xpath_context& c)
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
				if (ns.empty()) return "";
				
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
				if (ns.empty()) return "";
				
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
				if (ns.empty()) return "";
				
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
				std::string r = m_left->eval_string(c);
				
				for (xpath_ast_node* n = m_right; n; n = n->m_next)
					r += n->eval_string(c);
			
				return r;
			}

			case ast_func_substring_before:
			{
				std::string s = m_left->eval_string(c);
				std::string::size_type pos = s.find(m_right->eval_string(c));
				
				if (pos == std::string::npos) return "";
				else return std::string(s.begin(), s.begin() + pos);
			}
			
			case ast_func_substring_after:
			{
				std::string s = m_left->eval_string(c);
				std::string p = m_right->eval_string(c);
				
				std::string::size_type pos = s.find(p);
				
				if (pos == std::string::npos) return "";
				else return std::string(s.begin() + pos + p.length(), s.end());
			}

			case ast_func_substring_2:
			{
				std::string s = m_left->eval_string(c);
				double first = ieee754_round(m_right->eval_number(c));
				
				if (is_nan(first)) return ""; // NaN
				else if (first >= s.length() + 1) return "";
				
				size_t pos = first < 1 ? 1 : (size_t)first;
				
				return s.substr(pos - 1);
			}
			
			case ast_func_substring_3:
			{
				std::string s = m_left->eval_string(c);
				double first = ieee754_round(m_right->eval_number(c));
				double last = first + ieee754_round(m_third->eval_number(c));
				
				if (is_nan(first) || is_nan(last)) return "";
				else if (first >= s.length() + 1) return "";
				else if (first >= last) return "";
				
				size_t pos = first < 1 ? 1 : (size_t)first;
				size_t end = last >= s.length() + 1 ? s.length() + 1 : (size_t)last;

				size_t size_requested = end - pos;
				size_t size_to_end = s.length() - pos + 1;
				
				return s.substr(pos - 1, size_requested < size_to_end ? size_requested : size_to_end);
			}

			case ast_func_normalize_space_0:
			case ast_func_normalize_space_1:
			{
				std::string s = m_type == ast_func_normalize_space_0 ? string_value(c.n) : m_left->eval_string(c);
				
				std::string r;
				r.reserve(s.size());
				
				for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
				{
					if (is_chartype(*it, ct_space))
					{
						if (!r.empty() && r[r.size() - 1] != ' ')
							r += ' ';
					}
					else r += *it;
				}
				
				std::string::size_type pos = r.find_last_not_of(' ');
				if (pos == std::string::npos) r = "";
				else r.erase(r.begin() + pos + 1, r.end());
			
				return r;
			}

			case ast_func_translate:
			{
				std::string s = m_left->eval_string(c);
				std::string from = m_right->eval_string(c);
				std::string to = m_third->eval_string(c);
				
				for (std::string::iterator it = s.begin(); it != s.end(); )
				{
					std::string::size_type pos = from.find(*it);
					
					if (pos != std::string::npos && pos >= to.length())
						it = s.erase(it);
					else if (pos != std::string::npos)
						*it = to[pos];
				}
				
				return s;
			}

			default:
			{
				switch (m_rettype)
				{
				case ast_type_boolean:
					return eval_boolean(c) ? "true" : "false";
					
				case ast_type_number:
					return convert_number_to_string(eval_number(c));
					
				case ast_type_node_set:
				{
					xpath_node_set ns = eval_node_set(c);
					return ns.empty() ? std::string("") : string_value(ns.first());
				}
				
				default:
					assert(!"Wrong expression for ret type string");
					return "";
				}
			}
			}
		}

		xpath_node_set eval_node_set(xpath_context& c)
		{
			switch (m_type)
			{
			case ast_op_union:
			{
				xpath_node_set ls = m_left->eval_node_set(c);
				xpath_node_set rs = m_right->eval_node_set(c);
				
				ls.append(rs.begin(), rs.end());
				
				ls.remove_duplicates();
				
				return ls;
			}

			case ast_filter:
			{
				xpath_node_set set = m_left->eval_node_set(c);
				set.sort();
				
				xpath_context oc = c;
			
				size_t i = 0;
				
				xpath_node_set::iterator last = set.mut_begin();
				
				// remove_if... or well, sort of
				for (xpath_node_set::const_iterator it = set.begin(); it != set.end(); ++it, ++i)
				{
					c.n = *it;
					c.position = i + 1;
					c.size = set.size();
				
					if (m_right->rettype() == ast_type_number)
					{
						if (m_right->eval_number(c) == i + 1)
							*last++ = *it;
					}
					else if (m_right->eval_boolean(c))
						*last++ = *it;
				}
			
				c = oc;
				
				set.truncate(last);
			
				return set;
			}
			
			case ast_filter_posinv:
			{
				xpath_node_set set = m_left->eval_node_set(c);
				
				xpath_context oc = c;
			
				size_t i = 0;
				
				xpath_node_set::iterator last = set.mut_begin();
				
				// remove_if... or well, sort of
				for (xpath_node_set::const_iterator it = set.begin(); it != set.end(); ++it, ++i)
				{
					c.n = *it;
					c.position = i + 1;
					c.size = set.size();
				
					if (m_right->eval_boolean(c))
						*last++ = *it;
				}
			
				c = oc;
				
				set.truncate(last);
			
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

				default:
					assert(!"Axis not implemented");
					return xpath_node_set();
				}
				
				ns.remove_duplicates();
				
				return ns;
			}

			case ast_step_root:
			{
				xpath_node_set ns;
			
				ns.push_back(c.root);
				
				apply_predicates(ns, 0, c);
			
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
			
			case ast_variable:
				throw xpath_exception("Semantics error: variables are not supported");
				
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

		void check_semantics()
		{
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
				m_left->check_semantics();
				m_right->check_semantics();
				m_rettype = ast_type_boolean;
				break;
				
			case ast_op_add:
			case ast_op_subtract:
			case ast_op_multiply:
			case ast_op_divide:
			case ast_op_mod:
				m_left->check_semantics();
				m_right->check_semantics();
				m_rettype = ast_type_number;
				break;
				
			case ast_op_negate:
				m_left->check_semantics();
				m_rettype = ast_type_number;
				break;
			
			case ast_op_union:
				m_left->check_semantics();
				m_right->check_semantics();
				if (m_left->rettype() != ast_type_node_set || m_right->rettype() != ast_type_node_set)
					throw xpath_exception("Semantics error: union operator has to be applied to node sets");
				m_rettype = ast_type_node_set;
				break;
			
			case ast_filter:
			case ast_filter_posinv:
				m_left->check_semantics();
				m_right->check_semantics();
				if (m_left->rettype() != ast_type_node_set)
					throw xpath_exception("Semantics error: predicate has to be applied to node set");
				m_rettype = ast_type_node_set;
				
				if (!m_right->contains(ast_func_position) && m_right->rettype() != ast_type_number)
					m_type = ast_filter_posinv;
				break;
			
			case ast_predicate:
				m_left->check_semantics();
				m_rettype = ast_type_node_set;
				break;

			case ast_variable:
				throw xpath_exception("Semantics error: variable are not supported");
				
			case ast_string_constant:
				m_rettype = ast_type_string;
				break;
				
			case ast_number_constant:
				m_rettype = ast_type_number;
				break;
				
			case ast_func_last:
			case ast_func_position:
				m_rettype = ast_type_number;
				break;
		
			case ast_func_count:
				m_left->check_semantics();
				if (m_left->rettype() != ast_type_node_set)
					throw xpath_exception("Semantics error: count() has to be applied to node set");
				m_rettype = ast_type_number;
				break;
				
			case ast_func_id:
				m_left->check_semantics();
				m_rettype = ast_type_node_set;
				break;
				
			case ast_func_local_name_0:
			case ast_func_local_name_1:
			case ast_func_namespace_uri_0:
			case ast_func_namespace_uri_1:
			case ast_func_name_0:
			case ast_func_name_1:
				if (m_left)
				{
					m_left->check_semantics();
					if (m_left->rettype() != ast_type_node_set)
						throw xpath_exception("Semantics error: function has to be applied to node set");
				}
				m_rettype = ast_type_string;
				break;
				
			case ast_func_string_0:
			case ast_func_string_1:
				if (m_left) m_left->check_semantics();
				m_rettype = ast_type_string;
				break;
				
			case ast_func_concat:
			{
				m_left->check_semantics();
				
				for (xpath_ast_node* n = m_right; n; n = n->m_next)
					n->check_semantics();
					
				m_rettype = ast_type_string;
				break;
			}
			
			case ast_func_starts_with:
			case ast_func_contains:
				m_left->check_semantics();
				m_right->check_semantics();
				m_rettype = ast_type_boolean;
				break;
				
			case ast_func_substring_before:
			case ast_func_substring_after:
			case ast_func_substring_2:
			case ast_func_substring_3:
				m_left->check_semantics();
				m_right->check_semantics();
				if (m_third) m_third->check_semantics();
				m_rettype = ast_type_string;
				break;
		
			case ast_func_string_length_0:
			case ast_func_string_length_1:
				if (m_left) m_left->check_semantics();
				m_rettype = ast_type_number;
				break;
				
			case ast_func_normalize_space_0:
			case ast_func_normalize_space_1:
			case ast_func_translate:
				if (m_left) m_left->check_semantics();
				if (m_right) m_right->check_semantics();
				if (m_third) m_third->check_semantics();
				m_rettype = ast_type_string;
				break;
		
			case ast_func_boolean:
			case ast_func_not:
			case ast_func_true:
			case ast_func_false:
			case ast_func_lang:
				if (m_left) m_left->check_semantics();
				m_rettype = ast_type_boolean;
				break;
			
			case ast_func_number_0:
			case ast_func_number_1:
				if (m_left) m_left->check_semantics();
				m_rettype = ast_type_number;
				break;
		
			case ast_func_sum:
				m_left->check_semantics();
				if (m_left->rettype() != ast_type_node_set)
					throw xpath_exception("Semantics error: sum() has to be applied to node set");
				m_rettype = ast_type_number;
				break;
				
			case ast_func_floor:
			case ast_func_ceiling:
			case ast_func_round:
				if (m_left) m_left->check_semantics();
				m_rettype = ast_type_number;
				break;

			case ast_step:
			{
				if (m_left)
				{
					m_left->check_semantics();
					if (m_left->rettype() != ast_type_node_set)
						throw xpath_exception("Semantics error: step has to be applied to node set");
				}
				
				for (xpath_ast_node* n = m_right; n; n = n->m_next)
					n->check_semantics();
				
				m_rettype = ast_type_node_set;
				break;
			}
				
			case ast_step_root:
				m_rettype = ast_type_node_set;
				break;
			
			default:
				throw xpath_exception("Unknown semantics error");
			}
		}
		
		ast_rettype_t rettype() const
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
	    
	    // PrimaryExpr ::= VariableReference | '(' Expr ')' | Literal | Number | FunctionCall
	    xpath_ast_node* parse_primary_expression()
	    {
	    	switch (m_lexer.current())
	    	{
	    	case lex_var_ref:
	    	{
	    		m_lexer.next();

	    		if (m_lexer.current() != lex_string)
	    			throw xpath_exception("incorrect variable reference");

				xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_variable, m_lexer.contents(), m_alloc);
				m_lexer.next();

				return n;
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
				xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_string_constant, m_lexer.contents(), m_alloc);
				m_lexer.next();

				return n;
			}

			case lex_number:
			{
				xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_number_constant, m_lexer.contents(), m_alloc);
				m_lexer.next();

				return n;
			}

			case lex_string:
			{
				xpath_ast_node* args[4];
				size_t argc = 0;
				
				std::string function = m_lexer.contents();
				m_lexer.next();
				
				bool func_concat = (function == "concat");
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
				
				ast_type_t type = ast_none;
				
				switch (function[0])
				{
				case 'b':
				{
					if (function == "boolean" && argc == 1)
						type = ast_func_boolean;
						
					break;
				}
				
				case 'c':
				{
					if (function == "count" && argc == 1)
						type = ast_func_count;
					else if (function == "contains" && argc == 2)
						type = ast_func_contains;
					else if (function == "concat" && argc == 2)
					{
						// set_next was done earlier
						return new (m_alloc.node()) xpath_ast_node(ast_func_concat, args[0], args[1]);
					}
					else if (function == "ceiling" && argc == 1)
						type = ast_func_ceiling;
						
					break;
				}
				
				case 'f':
				{
					if (function == "false" && argc == 0)
						type = ast_func_false;
					else if (function == "floor" && argc == 1)
						type = ast_func_floor;
						
					break;
				}
				
				case 'i':
				{
					if (function == "id" && argc == 1)
						type = ast_func_id;
						
					break;
				}
				
				case 'l':
				{
					if (function == "last" && argc == 0)
						type = ast_func_last;
					else if (function == "lang" && argc == 1)
						type = ast_func_lang;
					else if (function == "local-name" && argc <= 1)
						type = argc == 0 ? ast_func_local_name_0 : ast_func_local_name_1;
				
					break;
				}
				
				case 'n':
				{
					if (function == "name" && argc <= 1)
						type = argc == 0 ? ast_func_name_0 : ast_func_name_1;
					else if (function == "namespace-uri" && argc <= 1)
						type = argc == 0 ? ast_func_namespace_uri_0 : ast_func_namespace_uri_1;
					else if (function == "normalize-space" && argc <= 1)
						type = argc == 0 ? ast_func_normalize_space_0 : ast_func_normalize_space_1;
					else if (function == "not" && argc == 1)
						type = ast_func_not;
					else if (function == "number" && argc <= 1)
						type = argc == 0 ? ast_func_number_0 : ast_func_number_1;
				
					break;
				}
				
				case 'p':
				{
					if (function == "position" && argc == 0)
						type = ast_func_position;
					
					break;
				}
				
				case 'r':
				{
					if (function == "round" && argc == 1)
						type = ast_func_round;

					break;
				}
				
				case 's':
				{
					if (function == "string" && argc <= 1)
						type = argc == 0 ? ast_func_string_0 : ast_func_string_1;
					else if (function == "string-length" && argc <= 1)
						type = argc == 0 ? ast_func_string_length_0 : ast_func_string_length_1;
					else if (function == "starts-with" && argc == 2)
						type = ast_func_starts_with;
					else if (function == "substring-before" && argc == 2)
						type = ast_func_substring_before;
					else if (function == "substring-after" && argc == 2)
						type = ast_func_substring_after;
					else if (function == "substring" && (argc == 2 || argc == 3))
						type = argc == 2 ? ast_func_substring_2 : ast_func_substring_3;
					else if (function == "sum" && argc == 1)
						type = ast_func_sum;

					break;
				}
				
				case 't':
				{
					if (function == "translate" && argc == 3)
						type = ast_func_translate;
					else if (function == "true" && argc == 0)
						type = ast_func_true;
						
					break;
				}
				
				}
				
				if (type != ast_none)
				{
					switch (argc)
					{
					case 0: return new (m_alloc.node()) xpath_ast_node(type);
					case 1: return new (m_alloc.node()) xpath_ast_node(type, args[0]);
					case 2: return new (m_alloc.node()) xpath_ast_node(type, args[0], args[1]);
					case 3: return new (m_alloc.node()) xpath_ast_node(type, args[0], args[1], args[2]);
					}
				}
				
				throw xpath_exception("Unrecognized function or wrong parameter count");
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

	    		n = new (m_alloc.node()) xpath_ast_node(ast_filter, n, parse_expression(), axis_child);

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
			axis_t axis;
			
			if (m_lexer.current() == lex_axis_attribute)
			{
				axis = axis_attribute;
				
				m_lexer.next();
			}
			else if (m_lexer.current() == lex_dot)
			{
				m_lexer.next();
				
				return new (m_alloc.node()) xpath_ast_node(ast_step, set, axis_self, nodetest_type_node, 0, m_alloc);
			}
			else if (m_lexer.current() == lex_double_dot)
			{
				m_lexer.next();
				
				return new (m_alloc.node()) xpath_ast_node(ast_step, set, axis_parent, nodetest_type_node, 0, m_alloc);
			}
			else // implied child axis
				axis = axis_child;
	    
			nodetest_t nt_type;
			std::string nt_name;
			
			if (m_lexer.current() == lex_string)
			{
				// node name test
				nt_name = m_lexer.contents();
				m_lexer.next();
				
				// possible axis name here - check.
				if (nt_name.find("::") == std::string::npos && m_lexer.current() == lex_string && m_lexer.contents()[0] == ':' && m_lexer.contents()[1] == ':')
				{
					nt_name += m_lexer.contents();
					m_lexer.next();
				}
				
				// possible namespace test
				if (m_lexer.current() == lex_string && m_lexer.contents()[0] == ':')
				{
					std::string::size_type colon_pos = nt_name.find(':');
					
					// either there is no : in current string or there is, but it's :: and there's nothing more
					if (colon_pos == std::string::npos ||
						(colon_pos + 1 < nt_name.size() && nt_name[colon_pos + 1] == ':' &&
						 nt_name.find(':', colon_pos + 2) == std::string::npos))
					{
						nt_name += m_lexer.contents();
						m_lexer.next();
					}
				}
				
				bool axis_specified = true;
				
				switch (nt_name[0])
				{
				case 'a':
					if (starts_with(nt_name, "ancestor::")) axis = axis_ancestor;
					else if (starts_with(nt_name, "ancestor-or-self::")) axis = axis_ancestor_or_self;
					else if (starts_with(nt_name, "attribute::")) axis = axis_attribute;
					else axis_specified = false;
					
					break;
				
				case 'c':
					if (starts_with(nt_name, "child::")) axis = axis_child;
					else axis_specified = false;
					
					break;
				
				case 'd':
					if (starts_with(nt_name, "descendant::")) axis = axis_descendant;
					else if (starts_with(nt_name, "descendant-or-self::")) axis = axis_descendant_or_self;
					else axis_specified = false;
					
					break;
				
				case 'f':
					if (starts_with(nt_name, "following::")) axis = axis_following;
					else if (starts_with(nt_name, "following-sibling::")) axis = axis_following_sibling;
					else axis_specified = false;
					
					break;
				
				case 'n':
					if (starts_with(nt_name, "namespace::")) axis = axis_namespace;
					else axis_specified = false;
					
					break;
				
				case 'p':
					if (starts_with(nt_name, "parent::")) axis = axis_parent;
					else if (starts_with(nt_name, "preceding::")) axis = axis_preceding;
					else if (starts_with(nt_name, "preceding-sibling::")) axis = axis_preceding_sibling;
					else axis_specified = false;
					
					break;
				
				case 's':
					if (starts_with(nt_name, "self::")) axis = axis_ancestor_or_self;
					else axis_specified = false;
					
					break;

				default:
					axis_specified = false;
				}
				
				if (axis_specified)
				{
					nt_name.erase(0, nt_name.find("::") + 2);
				}
				
				if (nt_name.empty() && m_lexer.current() == lex_string)
				{
					nt_name += m_lexer.contents();
					m_lexer.next();
				}

				// node type test or processing-instruction
				if (m_lexer.current() == lex_open_brace)
				{
					m_lexer.next();
					
					if (m_lexer.current() == lex_close_brace)
					{
						m_lexer.next();
						
						if (nt_name == "node")
							nt_type = nodetest_type_node;
						else if (nt_name == "text")
							nt_type = nodetest_type_text;
						else if (nt_name == "comment")
							nt_type = nodetest_type_comment;
						else if (nt_name == "processing-instruction")
							nt_type = nodetest_type_pi;
						else
							throw xpath_exception("Unrecognized node type");
						
						nt_name.erase(nt_name.begin(), nt_name.end());
					}
					else if (nt_name == "processing-instruction")
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
				// namespace *
				else if (m_lexer.current() == lex_multiply)
				{
					// Only strings of form 'namespace:*' are permitted
					if (nt_name.empty())
						nt_type = nodetest_all;
					else
					{
						if (nt_name.find(':') != nt_name.size() - 1)
							throw xpath_exception("Wrong namespace-like node test");
						
						nt_name.erase(nt_name.size() - 1);
						
						nt_type = nodetest_all_in_namespace;
					}
					
					m_lexer.next();
				}
				else nt_type = nodetest_name;
			}
			else if (m_lexer.current() == lex_multiply)
			{
				nt_type = nodetest_all;
				m_lexer.next();
			}
			else throw xpath_exception("Unrecognized node test");
			
			xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_step, set, axis, nt_type, nt_name.c_str(), m_alloc);
			
			xpath_ast_node* last = 0;
			
			while (m_lexer.current() == lex_open_square_brace)
			{
				m_lexer.next();
				
				xpath_ast_node* pred = new (m_alloc.node()) xpath_ast_node(ast_predicate, parse_expression(), 0, axis);
				
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
					n = new (m_alloc.node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0, m_alloc);
				
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
				const char* state = m_lexer.state();
				
				m_lexer.next();
				
				xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_step_root);
				
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
				
				xpath_ast_node* n = new (m_alloc.node()) xpath_ast_node(ast_step_root);
				n = new (m_alloc.node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0, m_alloc);
				
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
	    			const char* state = m_lexer.state();
	    			
	    			while (*state && *state <= 32) ++state;
	    			
	    			if (*state != '(') return parse_location_path();
	    		}
	    		
	    		xpath_ast_node* n = parse_filter_expression();

	    		if (m_lexer.current() == lex_slash || m_lexer.current() == lex_double_slash)
	    		{
					lexeme_t l = m_lexer.current();
	    			m_lexer.next();
	    			
					if (l == lex_double_slash)
						n = new (m_alloc.node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0, m_alloc);
	
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

	    		n = new (m_alloc.node()) xpath_ast_node(ast_op_union, n, parse_union_expression());
	    	}

	    	return n;
	    }

	    // UnaryExpr ::= UnionExpr | '-' UnaryExpr
	    xpath_ast_node* parse_unary_expression()
	    {
	    	if (m_lexer.current() == lex_minus)
	    	{
	    		m_lexer.next();

	    		return new (m_alloc.node()) xpath_ast_node(ast_op_negate, parse_unary_expression());
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
	    		   (!strcmp(m_lexer.contents(), "mod") || !strcmp(m_lexer.contents(), "div"))))
	    	{
	    		ast_type_t op = m_lexer.current() == lex_multiply ? ast_op_multiply :
	    			!strcmp(m_lexer.contents(), "div") ? ast_op_divide : ast_op_mod;
	    		m_lexer.next();

	    		n = new (m_alloc.node()) xpath_ast_node(op, n, parse_unary_expression());
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

	    		n = new (m_alloc.node()) xpath_ast_node(l == lex_plus ? ast_op_add : ast_op_subtract, n, parse_multiplicative_expression());
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

	    		n = new (m_alloc.node()) xpath_ast_node(l == lex_less ? ast_op_less : l == lex_greater ? ast_op_greater :
	    						l == lex_less_or_equal ? ast_op_less_or_equal : ast_op_greater_or_equal,
	    						n, parse_additive_expression());
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

	    		n = new (m_alloc.node()) xpath_ast_node(l == lex_equal ? ast_op_equal : ast_op_not_equal, n, parse_relational_expression());
	    	}

	    	return n;
	    }
	    
	    // AndExpr ::= EqualityExpr | AndExpr 'and' EqualityExpr
	    xpath_ast_node* parse_and_expression()
	    {
	    	xpath_ast_node* n = parse_equality_expression();

	    	while (m_lexer.current() == lex_string && !strcmp(m_lexer.contents(), "and"))
	    	{
	    		m_lexer.next();

	    		n = new (m_alloc.node()) xpath_ast_node(ast_op_and, n, parse_equality_expression());
	    	}

	    	return n;
	    }

	    // OrExpr ::= AndExpr | OrExpr 'or' AndExpr
	    xpath_ast_node* parse_or_expression()
	    {
	    	xpath_ast_node* n = parse_and_expression();

	    	while (m_lexer.current() == lex_string && !strcmp(m_lexer.contents(), "or"))
	    	{
	    		m_lexer.next();

	    		n = new (m_alloc.node()) xpath_ast_node(ast_op_or, n, parse_and_expression());
	    	}

	    	return n;
	    }
		
		// Expr ::= OrExpr
		xpath_ast_node* parse_expression()
		{
			return parse_or_expression();
		}

	public:
		explicit xpath_parser(const char* query, xpath_allocator& alloc): m_alloc(alloc), m_lexer(query)
		{
		}

		xpath_ast_node* parse()
		{
			xpath_ast_node* result = parse_expression();
			
			if (m_lexer.current() != lex_none)
			{
				// there are still unparsed tokens left, error
				throw xpath_exception("Incorrect query");
			}
			
			return result;
		}
	};

	xpath_query::xpath_query(const char* query): m_alloc(0), m_root(0)
	{
		compile(query);
	}

	xpath_query::~xpath_query()
	{
		delete m_alloc;
	}

	void xpath_query::compile(const char* query)
	{
		delete m_alloc;
		m_alloc = new xpath_allocator;

		xpath_parser p(query, *m_alloc);

		m_root = p.parse();
		m_root->check_semantics();
	}

	bool xpath_query::evaluate_boolean(const xml_node& n)
	{
		if (!m_root) return false;
		
		xpath_context c;
		
		c.root = n.root();
		c.n = n;
		c.position = 1;
		c.size = 1;
		
		return m_root->eval_boolean(c);
	}
	
	double xpath_query::evaluate_number(const xml_node& n)
	{
		if (!m_root) return gen_nan();
		
		xpath_context c;
		
		c.root = n.root();
		c.n = n;
		c.position = 1;
		c.size = 1;
		
		return m_root->eval_number(c);
	}
	
	std::string xpath_query::evaluate_string(const xml_node& n)
	{
		if (!m_root) return std::string();
		
		xpath_context c;
		
		c.root = n.root();
		c.n = n;
		c.position = 1;
		c.size = 1;
		
		return m_root->eval_string(c);
	}
	
	xpath_node_set xpath_query::evaluate_node_set(const xml_node& n)
	{
		if (!m_root) return xpath_node_set();
		
		xpath_context c;
		
		c.root = n.root();
		c.n = n;
		c.position = 1;
		c.size = 1;
		
		return m_root->eval_node_set(c);
	}

	xpath_node xml_node::select_single_node(const char* query) const
	{
		xpath_query q(query);
		return select_single_node(q);
	}

	xpath_node xml_node::select_single_node(xpath_query& query) const
	{
		xpath_node_set s = query.evaluate_node_set(*this);
		return s.empty() ? xpath_node() : s.first();
	}

	xpath_node_set xml_node::select_nodes(const char* query) const
	{
		xpath_query q(query);
		return select_nodes(q);
	}

	xpath_node_set xml_node::select_nodes(xpath_query& query) const
	{
		return query.evaluate_node_set(*this);
	}
}

#endif
