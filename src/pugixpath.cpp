/**
 * pugixml parser - version 0.9
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <setjmp.h>
#include <ctype.h>
#include <math.h>
#include <float.h>

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
#	pragma warning(disable: 4611) // interaction between '_setjmp' and C++ object destruction is non-portable
#	pragma warning(disable: 4324) // structure was padded due to __declspec(align())
#	pragma warning(disable: 4996) // this function or variable may be unsafe
#endif

#ifdef __INTEL_COMPILER
#	pragma warning(disable: 1478 1786) // function was declared "deprecated"
#endif

#ifdef __SNC__
#	pragma diag_suppress=237 // controlling expression is constant
#endif

#include <string>

// String utilities prototypes
namespace pugi
{
	namespace impl
	{
		size_t strlen(const char_t* s);
		bool strequalrange(const char_t* lhs, const char_t* rhs, size_t count);
		void widen_ascii(wchar_t* dest, const char* source);
	}
}

// STL replacements
namespace pstd
{
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

	template <typename T> void swap(T& lhs, T& rhs)
	{
		T temp = lhs;
		lhs = rhs;
		rhs = temp;
	}

	template <typename I, typename J> void copy(I begin, I end, J target)
	{
		while (begin != end) *target++ = *begin++;
	}

	template <typename I, typename T> I find(I begin, I end, T elem)
	{
		for (I it = begin; it != end; ++it)
			if (*it == elem)
				return it;

		return end;
	}

	template <typename I, typename Pred> I min_element(I begin, I end, const Pred& pred)
	{
		I result = begin;

		for (I it = begin + 1; it != end; ++it)
			if (pred(*it, *result))
				result = it;

		return result;
	}

	template <typename I> void reverse(I begin, I end)
	{
		while (begin + 1 < end) swap(*begin++, *--end);
	}

	template <typename I> I unique(I begin, I end)
	{
		// fast skip head
		while (begin + 1 < end && *begin != *(begin + 1)) begin++;

		if (begin == end) return begin;

		// last written element
		I write = begin++; 

		// merge unique elements
		while (begin != end)
		{
			if (*begin != *write)
				*++write = *begin++;
			else
				begin++;
		}

		// past-the-end (write points to live element)
		return write + 1;
	}

	template <typename I, typename Pred> void sort(I begin, I end, const Pred& pred)
	{
		while (begin != end) swap(*begin, *min_element(begin, end, pred)), begin++;
	}
}

namespace
{
	using namespace pugi;

	class xpath_string
	{
		const char_t* _buffer;
		bool _uses_heap;

		static char_t* duplicate_string(const char_t* string, size_t length)
		{
			char_t* result = static_cast<char_t*>(get_memory_allocation_function()((length + 1) * sizeof(char_t)));
			if (!result) return 0; // $$ out of memory

			memcpy(result, string, length * sizeof(char_t));
			result[length] = 0;

			return result;
		}

		static char_t* duplicate_string(const char_t* string)
		{
			return duplicate_string(string, impl::strlen(string));
		}

		void _swap(xpath_string& o)
		{
			pstd::swap(_buffer, o._buffer);
			pstd::swap(_uses_heap, o._uses_heap);
		}

	public:
		xpath_string(): _buffer(PUGIXML_TEXT("")), _uses_heap(false)
		{
		}

		~xpath_string()
		{
			if (_uses_heap) get_memory_deallocation_function()(const_cast<char_t*>(_buffer));
		}

		explicit xpath_string(const char_t* str)
		{
			bool empty = (*str == 0);

			_buffer = empty ? PUGIXML_TEXT("") : duplicate_string(str);
			_uses_heap = !empty;
		}

		explicit xpath_string(const char_t* str, bool use_heap): _buffer(str), _uses_heap(use_heap)
		{
		}

		xpath_string(const char_t* begin, const char_t* end)
		{
			assert(begin <= end);

			if (begin != end)
			{
				_buffer = duplicate_string(begin, static_cast<size_t>(end - begin));
				_uses_heap = true;
			}
			else
			{
				_buffer = PUGIXML_TEXT("");
				_uses_heap = false;
			}
		}

		xpath_string(const xpath_string& o)
		{
			_buffer = o._uses_heap ? duplicate_string(o._buffer) : o._buffer;
			_uses_heap = o._uses_heap;
		}

		xpath_string& operator=(const xpath_string& o)
		{
			xpath_string temp(o);
			_swap(temp);

			return *this;
		}

		void append(const xpath_string& o)
		{
			// skip empty sources
			if (!*o._buffer) return;

			// fast append for empty target and constant source
			if (!*_buffer && !o._uses_heap)
			{
				_buffer = o._buffer;
				_uses_heap = false;
			}
			else
			{
				// need to make heap copy
				size_t target_length = impl::strlen(_buffer);
				size_t source_length = impl::strlen(o._buffer);
				size_t length = target_length + source_length;

				// allocate new buffer
				char_t* result = static_cast<char_t*>(get_memory_allocation_function()((length + 1) * sizeof(char_t)));
				if (!result) return; // $$ out of memory

				// append both strings in the new buffer
				memcpy(result, _buffer, target_length * sizeof(char_t));
				memcpy(result + target_length, o._buffer, source_length * sizeof(char_t));
				result[length] = 0;

				// finalize
				xpath_string(result, true)._swap(*this);
			}
		}

		const char_t* c_str() const
		{
			return _buffer;
		}

		size_t length() const
		{
			return impl::strlen(_buffer);
		}
		
		char_t* data()
		{
			// make private heap copy
			if (!_uses_heap)
			{
				_buffer = duplicate_string(_buffer);
				_uses_heap = true;
			}

			return const_cast<char_t*>(_buffer);
		}

		bool empty() const
		{
			return *_buffer == 0;
		}

		bool operator==(const xpath_string& o) const
		{
			return impl::strequal(_buffer, o._buffer);
		}

		bool operator!=(const xpath_string& o) const
		{
			return !impl::strequal(_buffer, o._buffer);
		}
	};

	xpath_string xpath_string_const(const char_t* str)
	{
		return xpath_string(str, false);
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
		return strchr(s, c);
	#endif
	}

	const char_t* find_substring(const char_t* s, const char_t* p)
	{
	#ifdef PUGIXML_WCHAR_MODE
		return wcsstr(s, p);
	#else
		return strstr(s, p);
	#endif
	}

	xpath_string string_value(const xpath_node& na)
	{
		if (na.attribute())
			return xpath_string_const(na.attribute().value());
		else
		{
			const xml_node& n = na.node();

			switch (n.type())
			{
			case node_pcdata:
			case node_cdata:
			case node_comment:
			case node_pi:
				return xpath_string_const(n.value());
			
			case node_document:
			case node_element:
			{
				xpath_string result;

				xml_node cur = n.first_child();
				
				while (cur && cur != n)
				{
					if (cur.type() == node_pcdata || cur.type() == node_cdata)
						result.append(xpath_string_const(cur.value()));

					if (cur.first_child())
						cur = cur.first_child();
					else if (cur.next_sibling())
						cur = cur.next_sibling();
					else
					{
						while (!cur.next_sibling() && cur != n)
							cur = cur.parent();

						if (cur != n) cur = cur.next_sibling();
					}
				}
				
				return result;
			}
			
			default:
				return xpath_string();
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

			const void* lo = lhs.attribute() ? lhs.attribute().document_order() : ln.document_order();
			const void* ro = rhs.attribute() ? rhs.attribute().document_order() : rn.document_order();

			if (lo && ro) return lo < ro;

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
	#elif defined(fpclassify) && defined(FP_NAN)
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
	#elif defined(fpclassify) && defined(FP_NAN) && defined(FP_INFINITE) && defined(FP_ZERO)
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
	
	void truncate_zeros(char* begin, char* end)
	{
		while (begin != end && end[-1] == '0') end--;

		*end = 0;
	}

	// gets mantissa digits in the form of 0.xxxxx with 0. implied and the exponent
#if defined(_MSC_VER) && _MSC_VER >= 1400
	void convert_number_to_mantissa_exponent(double value, char* buffer, size_t buffer_size, char** out_mantissa, int* out_exponent)
	{
		// get base values
		int sign, exponent;
		_ecvt_s(buffer, buffer_size, value, DBL_DIG + 1, &exponent, &sign);

		// truncate redundant zeros
		truncate_zeros(buffer, buffer + strlen(buffer));

		// fill results
		*out_mantissa = buffer;
		*out_exponent = exponent;
	}
#else
	void convert_number_to_mantissa_exponent(double value, char* buffer, size_t buffer_size, char** out_mantissa, int* out_exponent)
	{
		// get a scientific notation value with IEEE DBL_DIG decimals
		sprintf(buffer, "%.*e", DBL_DIG, value);
		assert(strlen(buffer) < buffer_size);
		(void)!buffer_size;

		// get the exponent (possibly negative)
		char* exponent_string = strchr(buffer, 'e');
		assert(exponent_string);

		int exponent = atoi(exponent_string + 1);

		// extract mantissa string: skip sign
		char* mantissa = buffer[0] == '-' ? buffer + 1 : buffer;
		assert(mantissa[0] != '0' && mantissa[1] == '.');

		// divide mantissa by 10 to eliminate integer part
		mantissa[1] = mantissa[0];
		mantissa++;
		exponent++;

		// remove extra mantissa digits and zero-terminate mantissa
		truncate_zeros(mantissa, exponent_string);

		// fill results
		*out_mantissa = mantissa;
		*out_exponent = exponent;
	}
#endif

	xpath_string convert_number_to_string(double value)
	{
		// try special number conversion
		const char_t* special = convert_number_to_string_special(value);
		if (special) return xpath_string_const(special);

		// get mantissa + exponent form
		char mantissa_buffer[64];

		char* mantissa;
		int exponent;
		convert_number_to_mantissa_exponent(value, mantissa_buffer, sizeof(mantissa_buffer), &mantissa, &exponent);

		// make the number!
		char_t result[512];
		char_t* s = result;

		// sign
		if (value < 0) *s++ = '-';

		// integer part
		if (exponent <= 0)
		{
			*s++ = '0';
		}
		else
		{
			while (exponent > 0)
			{
				assert(*mantissa == 0 || (unsigned)(*mantissa - '0') <= 9);
				*s++ = *mantissa ? *mantissa++ : '0';
				exponent--;
			}
		}

		// fractional part
		if (*mantissa)
		{
			// decimal point
			*s++ = '.';

			// extra zeroes from negative exponent
			while (exponent < 0)
			{
				*s++ = '0';
				exponent++;
			}

			// extra mantissa digits
			while (*mantissa)
			{
				assert((unsigned)(*mantissa - '0') <= 9);
				*s++ = *mantissa++;
			}
		}

		// zero-terminate
		assert(s < result + sizeof(result) / sizeof(result[0]));
		*s = 0;

		return xpath_string(result);
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

	bool convert_string_to_number(const char_t* begin, const char_t* end, double* out_result)
	{
		char_t buffer[32];

		size_t length = static_cast<size_t>(end - begin);
		char_t* scratch = buffer;

		if (length >= sizeof(buffer) / sizeof(buffer[0]))
		{
			// need to make dummy on-heap copy
			scratch = static_cast<char_t*>(get_memory_allocation_function()((length + 1) * sizeof(char_t)));
			if (!scratch) return false;
		}

		// copy string to zero-terminated buffer and perform conversion
		memcpy(scratch, begin, length * sizeof(char_t));
		scratch[length] = 0;

		*out_result = convert_string_to_number(scratch);

		// free dummy buffer
		if (scratch != buffer) get_memory_deallocation_function()(scratch);

		return true;
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
	
	const char_t* qualified_name(const xpath_node& node)
	{
		return node.attribute() ? node.attribute().name() : node.node().name();
	}
	
	const char_t* local_name(const xpath_node& node)
	{
		const char_t* name = qualified_name(node);
		const char_t* p = find_char(name, ':');
		
		return p ? p + 1 : name;
	}

	struct namespace_uri_predicate
	{
		const char_t* prefix;
		size_t prefix_length;

		namespace_uri_predicate(const char_t* name)
		{
			const char_t* pos = find_char(name, ':');

			prefix = pos ? name : 0;
			prefix_length = pos ? static_cast<size_t>(pos - name) : 0;
		}

		bool operator()(const xml_attribute& a) const
		{
			const char_t* name = a.name();

			if (!starts_with(name, PUGIXML_TEXT("xmlns"))) return false;

			return prefix ? name[5] == ':' && impl::strequalrange(name + 6, prefix, prefix_length) : name[5] == 0;
		}
	};

	const char_t* namespace_uri(const xml_node& node)
	{
		namespace_uri_predicate pred = node.name();
		
		xml_node p = node;
		
		while (p)
		{
			xml_attribute a = p.find_attribute(pred);
			
			if (a) return a.value();
			
			p = p.parent();
		}
		
		return PUGIXML_TEXT("");
	}

	const char_t* namespace_uri(const xml_attribute& attr, const xml_node& parent)
	{
		namespace_uri_predicate pred = attr.name();
		
		// Default namespace does not apply to attributes
		if (!pred.prefix) return PUGIXML_TEXT("");
		
		xml_node p = parent;
		
		while (p)
		{
			xml_attribute a = p.find_attribute(pred);
			
			if (a) return a.value();
			
			p = p.parent();
		}
		
		return PUGIXML_TEXT("");
	}

	const char_t* namespace_uri(const xpath_node& node)
	{
		return node.attribute() ? namespace_uri(node.attribute(), node.parent()) : namespace_uri(node.node());
	}

	void normalize_space(char_t* buffer)
	{
		char_t* write = buffer;

		for (char_t* it = buffer; *it; )
		{
			char_t ch = *it++;

			if (IS_CHARTYPEX(ch, ctx_space))
			{
				// replace whitespace sequence with single space
				while (IS_CHARTYPEX(*it, ctx_space)) it++;

				// avoid leading spaces
				if (write != buffer) *write++ = ' ';
			}
			else *write++ = ch;
		}

		// remove trailing space
		if (write != buffer && IS_CHARTYPEX(write[-1], ctx_space)) write--;

		// zero-terminate
		*write = 0;
	}

	void translate(char_t* buffer, const char_t* from, const char_t* to)
	{
		size_t to_length = impl::strlen(to);

		char_t* write = buffer;

		while (*buffer)
		{
		#ifdef __DMC__
			volatile // explicitly store to local to work around DMC bug (it loads 4 bytes from buffer otherwise)
		#endif
			char_t ch = *buffer++;

			const char_t* pos = find_char(from, ch);

			if (!pos)
				*write++ = ch; // do not process
			else if (static_cast<size_t>(pos - from) < to_length)
				*write++ = to[pos - from]; // replace
		}

		// zero-terminate
		*write = 0;
	}
}

namespace pugi
{
#ifndef PUGIXML_NO_EXCEPTIONS
	xpath_exception::xpath_exception(const xpath_parse_result& result): _result(result)
	{
		assert(result.error);
	}
	
	const char* xpath_exception::what() const throw()
	{
		return _result.error;
	}

	const xpath_parse_result& xpath_exception::result() const
	{
		return _result;
	}
#endif
	
	const size_t xpath_memory_block_size = 4096;

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
		
		memory_block* _root;
		memory_block _first;
		
	public:
		static xpath_allocator* create()
		{
			void* memory = get_memory_allocation_function()(sizeof(xpath_allocator));

			return new (memory) xpath_allocator();
		}

		static void destroy(xpath_allocator* alloc)
		{
			if (!alloc) return;
			
			// free all allocated pages
			memory_block* cur = alloc->_root;
			memory_block* first = &alloc->_first;

			while (cur != first)
			{
				memory_block* next = cur->next;

				get_memory_deallocation_function()(cur);

				cur = next;
			}

			// free allocator memory (with the first page)
			get_memory_deallocation_function()(alloc);
		}

		xpath_allocator(): _root(&_first)
		{
		}
		
		void* allocate(size_t size)
		{
			// align size so that we're able to store pointers in subsequent blocks
			size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

			if (_root->size + size <= xpath_memory_block_size)
			{
				void* buf = _root->data + _root->size;
				_root->size += size;
				return buf;
			}
			else
			{
				size_t block_data_size = (size > xpath_memory_block_size) ? size : xpath_memory_block_size;
				size_t block_size = block_data_size + offsetof(memory_block, data);

				memory_block* block = static_cast<memory_block*>(get_memory_allocation_function()(block_size));
				if (!block) return 0;
				
				block->next = _root;
				block->size = size;
				
				_root = block;
				
				return block->data;
			}
		}
	};

	xpath_node::xpath_node()
	{
	}
		
	xpath_node::xpath_node(const xml_node& node): _node(node)
	{
	}
		
	xpath_node::xpath_node(const xml_attribute& attribute, const xml_node& parent): _node(parent), _attribute(attribute)
	{
	}

	xml_node xpath_node::node() const
	{
		return _attribute ? xml_node() : _node;
	}
		
	xml_attribute xpath_node::attribute() const
	{
		return _attribute;
	}
	
	xml_node xpath_node::parent() const
	{
		return _attribute ? _node : _node.parent();
	}

	xpath_node::operator xpath_node::unspecified_bool_type() const
	{
		return (_node || _attribute) ? &xpath_node::_node : 0;
	}
	
	bool xpath_node::operator!() const
	{
		return !(_node || _attribute);
	}

	bool xpath_node::operator==(const xpath_node& n) const
	{
		return _node == n._node && _attribute == n._attribute;
	}
	
	bool xpath_node::operator!=(const xpath_node& n) const
	{
		return _node != n._node || _attribute != n._attribute;
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

	xpath_node_set::xpath_node_set(): _type(type_unsorted), _begin(&_storage), _end(&_storage), _eos(&_storage + 1)
	{
	}

	xpath_node_set::~xpath_node_set()
	{
		if (_begin != &_storage) delete[] _begin;
	}
		
	xpath_node_set::xpath_node_set(const xpath_node_set& ns): _type(type_unsorted), _begin(&_storage), _end(&_storage), _eos(&_storage + 1)
	{
		*this = ns;
	}
	
	xpath_node_set& xpath_node_set::operator=(const xpath_node_set& ns)
	{
		if (&ns == this) return *this;

		if (_begin != &_storage) delete[] _begin;
		
		_begin = _end = _eos = 0;
		_type = ns._type;
		
		if (ns.size() == 1)
		{
			_storage = *ns._begin;
			_begin = &_storage;
			_end = _eos = &_storage + 1;
		}
		else
		{
			append(ns.begin(), ns.end());
		}
		
		return *this;
	}	

	xpath_node_set::type_t xpath_node_set::type() const
	{
		return _type;
	}
		
	size_t xpath_node_set::size() const
	{
		return _end - _begin;
	}
		
	bool xpath_node_set::empty() const
	{
		return size() == 0;
	}
		
	const xpath_node& xpath_node_set::operator[](size_t index) const
	{
		assert(index < size());
		return _begin[index];
	}

	xpath_node_set::iterator xpath_node_set::mut_begin()
	{
		return _begin;
	}
	
	xpath_node_set::const_iterator xpath_node_set::begin() const
	{
		return _begin;
	}
		
	xpath_node_set::const_iterator xpath_node_set::end() const
	{
		return _end;
	}
	
	void xpath_node_set::sort(bool reverse)
	{
		pstd::sort(_begin, _end, document_order_comparator());
		
		if (reverse) pstd::reverse(_begin, _end);
			
		_type = reverse ? type_sorted_reverse : type_sorted;
	}

	void xpath_node_set::push_back(const xpath_node& n)
	{
		if (_end == _eos)
			append(&n, &n + 1);
		else
		{
			*_end = n;
			++_end;
		}
	}

	void xpath_node_set::append(const_iterator begin, const_iterator end)
	{
		if (begin == end) return;

		size_t count = end - begin;
		size_t size = _end - _begin;
		size_t capacity = _eos - _begin;
		
		if (capacity < size + count)
		{
			if (capacity < 2) capacity = 2;
			
			while (capacity < size + count) capacity += capacity / 2;
			
			xpath_node* storage = new xpath_node[capacity];
			pstd::copy(_begin, _end, storage);
			
			if (_begin != &_storage) delete[] _begin;
			
			_begin = storage;
			_end = storage + size;
			_eos = storage + capacity;
		}
		
		pstd::copy(begin, end, _end);
		_end += count;
	}

	void xpath_node_set::truncate(iterator it)
	{
		_end = it;
	}

	xpath_node xpath_node_set::first() const
	{
		if (empty()) return xpath_node();

		switch (_type)
		{
		case type_sorted: return *_begin;
		case type_sorted_reverse: return *(_end - 1);
		case type_unsorted: return *pstd::min_element(_begin, _end, document_order_comparator());
		default: return xpath_node();
		}
	}

	void xpath_node_set::remove_duplicates()
	{
		if (_type == type_unsorted)
		{
			pstd::sort(_begin, _end, duplicate_comparator());
		}
		
		truncate(pstd::unique(_begin, _end));
	}

	struct xpath_context
	{
		xpath_node n;
		size_t position, size;

		xpath_context(const xpath_node& n, size_t position, size_t size): n(n), position(position), size(size)
		{
		}
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

			return impl::strequalrange(other, begin, length);
		}
	};

	class xpath_lexer
	{
		const char_t* _cur;
		const char_t* _cur_lexeme_pos;
		xpath_lexer_string _cur_lexeme_contents;

		lexeme_t _cur_lexeme;

	public:
		explicit xpath_lexer(const char_t* query): _cur(query)
		{
			next();
		}
		
		const char_t* state() const
		{
			return _cur;
		}
		
		void next()
		{
			const char_t* cur = _cur;

			while (IS_CHARTYPEX(*cur, ctx_space)) ++cur;

			// save lexeme position for error reporting
			_cur_lexeme_pos = cur;

			switch (*cur)
			{
			case 0:
				_cur_lexeme = lex_eof;
				break;
			
			case '>':
				if (*(cur+1) == '=')
				{
					cur += 2;
					_cur_lexeme = lex_greater_or_equal;
				}
				else
				{
					cur += 1;
					_cur_lexeme = lex_greater;
				}
				break;

			case '<':
				if (*(cur+1) == '=')
				{
					cur += 2;
					_cur_lexeme = lex_less_or_equal;
				}
				else
				{
					cur += 1;
					_cur_lexeme = lex_less;
				}
				break;

			case '!':
				if (*(cur+1) == '=')
				{
					cur += 2;
					_cur_lexeme = lex_not_equal;
				}
				else
				{
					_cur_lexeme = lex_none;
				}
				break;

			case '=':
				cur += 1;
				_cur_lexeme = lex_equal;

				break;
			
			case '+':
				cur += 1;
				_cur_lexeme = lex_plus;

				break;

			case '-':
				cur += 1;
				_cur_lexeme = lex_minus;

				break;

			case '*':
				cur += 1;
				_cur_lexeme = lex_multiply;

				break;

			case '|':
				cur += 1;
				_cur_lexeme = lex_union;

				break;

			case '$':
				cur += 1;
				_cur_lexeme = lex_var_ref;

				break;
			
			case '(':
				cur += 1;
				_cur_lexeme = lex_open_brace;

				break;

			case ')':
				cur += 1;
				_cur_lexeme = lex_close_brace;

				break;
			
			case '[':
				cur += 1;
				_cur_lexeme = lex_open_square_brace;

				break;

			case ']':
				cur += 1;
				_cur_lexeme = lex_close_square_brace;

				break;

			case ',':
				cur += 1;
				_cur_lexeme = lex_comma;

				break;

			case '/':
				if (*(cur+1) == '/')
				{
					cur += 2;
					_cur_lexeme = lex_double_slash;
				}
				else
				{
					cur += 1;
					_cur_lexeme = lex_slash;
				}
				break;
		
			case '.':
				if (*(cur+1) == '.')
				{
					cur += 2;
					_cur_lexeme = lex_double_dot;
				}
				else if (IS_CHARTYPEX(*(cur+1), ctx_digit))
				{
					_cur_lexeme_contents.begin = cur; // .

					++cur;

					while (IS_CHARTYPEX(*cur, ctx_digit)) cur++;

					_cur_lexeme_contents.end = cur;
					
					_cur_lexeme = lex_number;
				}
				else
				{
					cur += 1;
					_cur_lexeme = lex_dot;
				}
				break;

			case '@':
				cur += 1;
				_cur_lexeme = lex_axis_attribute;

				break;

			case '"':
			case '\'':
			{
				char_t terminator = *cur;

				++cur;

				_cur_lexeme_contents.begin = cur;
				while (*cur && *cur != terminator) cur++;
				_cur_lexeme_contents.end = cur;
				
				if (!*cur)
					_cur_lexeme = lex_none;
				else
				{
					cur += 1;
					_cur_lexeme = lex_quoted_string;
				}

				break;
			}

			case ':':
				if (*(cur+1) == ':')
				{
					cur += 2;
					_cur_lexeme = lex_double_colon;
				}
				else
				{
					_cur_lexeme = lex_none;
				}
				break;

			default:
				if (IS_CHARTYPEX(*cur, ctx_digit))
				{
					_cur_lexeme_contents.begin = cur;

					while (IS_CHARTYPEX(*cur, ctx_digit)) cur++;
				
					if (*cur == '.')
					{
						cur++;

						while (IS_CHARTYPEX(*cur, ctx_digit)) cur++;
					}

					_cur_lexeme_contents.end = cur;

					_cur_lexeme = lex_number;
				}
				else if (IS_CHARTYPEX(*cur, ctx_start_symbol))
				{
					_cur_lexeme_contents.begin = cur;

					while (IS_CHARTYPEX(*cur, ctx_symbol)) cur++;

					if (cur[0] == ':')
					{
						if (cur[1] == '*') // namespace test ncname:*
						{
							cur += 2; // :*
						}
						else if (IS_CHARTYPEX(cur[1], ctx_symbol)) // namespace test qname
						{
							cur++; // :

							while (IS_CHARTYPEX(*cur, ctx_symbol)) cur++;
						}
					}

					_cur_lexeme_contents.end = cur;
				
					_cur_lexeme = lex_string;
				}
				else
				{
					_cur_lexeme = lex_none;
				}
			}

			_cur = cur;
		}

		lexeme_t current() const
		{
			return _cur_lexeme;
		}

		const char_t* current_pos() const
		{
			return _cur_lexeme_pos;
		}

		const xpath_lexer_string& contents() const
		{
			assert(_cur_lexeme == lex_number || _cur_lexeme == lex_string || _cur_lexeme == lex_quoted_string);

			return _cur_lexeme_contents;
		}
	};

	enum ast_type_t
	{
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
		// node type
		char _type;
		char _rettype;

		// for ast_step / ast_predicate
		char _axis;
		char _test;

		// tree node structure
		xpath_ast_node* _left;
		xpath_ast_node* _right;
		xpath_ast_node* _next;

		union
		{
			// value for ast_string_constant
			const char_t* string;
			// value for ast_number_constant
			double number;
			// node test for ast_step (node name/namespace/node type/pi target)
			const char_t* nodetest;
		} _data;

		xpath_ast_node(const xpath_ast_node&);
		xpath_ast_node& operator=(const xpath_ast_node&);

		template <class Comp> static bool compare_eq(xpath_ast_node* lhs, xpath_ast_node* rhs, const xpath_context& c, const Comp& comp)
		{
			xpath_value_type lt = lhs->rettype(), rt = rhs->rettype();

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
					pstd::swap(lhs, rhs);
					pstd::swap(lt, rt);
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
					xpath_string l = lhs->eval_string(c);
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
			xpath_value_type lt = lhs->rettype(), rt = rhs->rettype();

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

		void apply_predicate(xpath_node_set& ns, size_t first, xpath_ast_node* expr)
		{
			assert(ns.size() >= first);

			size_t i = 1;
			size_t size = ns.size() - first;
				
			xpath_node_set::iterator last = ns.mut_begin() + first;
				
			// remove_if... or well, sort of
			for (xpath_node_set::iterator it = last; it != ns.end(); ++it, ++i)
			{
				xpath_context c(*it, i, size);
			
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

		void apply_predicates(xpath_node_set& ns, size_t first)
		{
			if (ns.size() == first) return;
			
			for (xpath_ast_node* pred = _right; pred; pred = pred->_next)
			{
				apply_predicate(ns, first, pred->_left);
			}
		}

		void step_push(xpath_node_set& ns, const xml_attribute& a, const xml_node& parent)
		{
			if (!a) return;

			const char_t* name = a.name();

			// There are no attribute nodes corresponding to attributes that declare namespaces
			// That is, "xmlns:..." or "xmlns"
			if (starts_with(name, PUGIXML_TEXT("xmlns")) && (name[5] == 0 || name[5] == ':')) return;
			
			switch (_test)
			{
			case nodetest_name:
				if (impl::strequal(name, _data.nodetest)) ns.push_back(xpath_node(a, parent));
				break;
				
			case nodetest_type_node:
			case nodetest_all:
				ns.push_back(xpath_node(a, parent));
				break;
				
			case nodetest_all_in_namespace:
				if (starts_with(name, _data.nodetest))
					ns.push_back(xpath_node(a, parent));
				break;
			
			default:
				;
			}
		}
		
		void step_push(xpath_node_set& ns, const xml_node& n)
		{
			if (!n) return;

			switch (_test)
			{
			case nodetest_name:
				if (n.type() == node_element && impl::strequal(n.name(), _data.nodetest)) ns.push_back(n);
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
				if (n.type() == node_pi && impl::strequal(n.name(), _data.nodetest))
					ns.push_back(n);
				break;
				
			case nodetest_all:
				if (n.type() == node_element)
					ns.push_back(n);
				break;
				
			case nodetest_all_in_namespace:
				if (n.type() == node_element && starts_with(n.name(), _data.nodetest))
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
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;
				
				for (xml_attribute a = n.first_attribute(); a; a = a.next_attribute())
					step_push(ns, a, n);
				
				break;
			}
			
			case axis_child:
			{
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				for (xml_node c = n.first_child(); c; c = c.next_sibling())
					step_push(ns, c);
					
				break;
			}
			
			case axis_descendant:
			case axis_descendant_or_self:
			{
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;
				
				if (axis == axis_descendant_or_self)
					step_push(ns, n);
					
				xml_node cur = n.first_child();
				
				while (cur && cur != n)
				{
					step_push(ns, cur);
					
					if (cur.first_child())
						cur = cur.first_child();
					else if (cur.next_sibling())
						cur = cur.next_sibling();
					else
					{
						while (!cur.next_sibling() && cur != n)
							cur = cur.parent();
					
						if (cur != n) cur = cur.next_sibling();
					}
				}
				
				break;
			}
			
			case axis_following_sibling:
			{
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				for (xml_node c = n.next_sibling(); c; c = c.next_sibling())
					step_push(ns, c);
				
				break;
			}
			
			case axis_preceding_sibling:
			{
				ns._type = ns.empty() ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_unsorted;

				for (xml_node c = n.previous_sibling(); c; c = c.previous_sibling())
					step_push(ns, c);
				
				break;
			}
			
			case axis_following:
			{
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				xml_node cur = n;

				// exit from this node so that we don't include descendants
				while (cur && !cur.next_sibling()) cur = cur.parent();
				cur = cur.next_sibling();

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

				break;
			}

			case axis_preceding:
			{
				ns._type = ns.empty() ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_unsorted;

				xml_node cur = n;

				while (cur && !cur.previous_sibling()) cur = cur.parent();
				cur = cur.previous_sibling();

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

				break;
			}
			
			case axis_ancestor:
			case axis_ancestor_or_self:
			{
				ns._type = ns.empty() ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_unsorted;

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
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				step_push(ns, n);

				break;
			}

			case axis_parent:
			{
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

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
				ns._type = ns.empty() ? xpath_node_set::type_sorted_reverse : xpath_node_set::type_unsorted;

				if (axis == axis_ancestor_or_self && _test == nodetest_type_node) // reject attributes based on principal node type test
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
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

				if (_test == nodetest_type_node) // reject attributes based on principal node type test
					step_push(ns, a, p);

				break;
			}

			case axis_following:
			{
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;
				
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
				ns._type = ns.empty() ? xpath_node_set::type_sorted : xpath_node_set::type_unsorted;

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
				if (_left)
				{
					xpath_node_set s = _left->eval_node_set(c);
							
					for (xpath_node_set::const_iterator it = s.begin(); it != s.end(); ++it)
					{
						size_t size = ns.size();
						
						if (it->node())
							step_fill(ns, it->node(), v);
						else
							step_fill(ns, it->attribute(), it->parent(), v);
							
						apply_predicates(ns, size);
					}
				}
				else
				{
					if (c.n.node()) step_fill(ns, c.n.node(), v);
					else step_fill(ns, c.n.attribute(), c.n.parent(), v);
					
					apply_predicates(ns, 0);
				}
				
				break;
		
			case axis_following_sibling:
			case axis_preceding_sibling:
			case axis_attribute:
			case axis_child:
			case axis_descendant:
				if (_left)
				{
					xpath_node_set s = _left->eval_node_set(c);
					
					for (xpath_node_set::const_iterator it = s.begin(); it != s.end(); ++it)
					{
						size_t size = ns.size();
						
						if (it->node())
							step_fill(ns, it->node(), v);
						
						apply_predicates(ns, size);
					}
				}
				else if (c.n.node())
				{
					step_fill(ns, c.n.node(), v);
					
					apply_predicates(ns, 0);
				}
				
				break;
			
			case axis_namespace:
				break;
				
			default:
				assert(!"Unimplemented axis");
			}
		}
		
	public:
		xpath_ast_node(ast_type_t type, xpath_value_type rettype, const char_t* value):
			_type((char)type), _rettype((char)rettype), _axis(0), _test(0), _left(0), _right(0), _next(0)
		{
			assert(type == ast_string_constant);
			_data.string = value;
		}

		xpath_ast_node(ast_type_t type, xpath_value_type rettype, double value):
			_type((char)type), _rettype((char)rettype), _axis(0), _test(0), _left(0), _right(0), _next(0)
		{
			assert(type == ast_number_constant);
			_data.number = value;
		}
		
		xpath_ast_node(ast_type_t type, xpath_value_type rettype, xpath_ast_node* left = 0, xpath_ast_node* right = 0):
			_type((char)type), _rettype((char)rettype), _axis(0), _test(0), _left(left), _right(right), _next(0)
		{
		}

		xpath_ast_node(ast_type_t type, xpath_ast_node* left, axis_t axis, nodetest_t test, const char_t* contents):
			_type((char)type), _rettype(xpath_type_node_set), _axis((char)axis), _test((char)test), _left(left), _right(0), _next(0)
		{
			_data.nodetest = contents;
		}

		void set_next(xpath_ast_node* value)
		{
			_next = value;
		}

		void set_right(xpath_ast_node* value)
		{
			_right = value;
		}

		bool eval_boolean(const xpath_context& c)
		{
			switch (_type)
			{
			case ast_op_or:
				if (_left->eval_boolean(c)) return true;
				else return _right->eval_boolean(c);
				
			case ast_op_and:
				if (!_left->eval_boolean(c)) return false;
				else return _right->eval_boolean(c);
				
			case ast_op_equal:
				return compare_eq(_left, _right, c, pstd::equal_to());

			case ast_op_not_equal:
				return compare_eq(_left, _right, c, pstd::not_equal_to());
	
			case ast_op_less:
				return compare_rel(_left, _right, c, pstd::less());
			
			case ast_op_greater:
				return compare_rel(_right, _left, c, pstd::less());

			case ast_op_less_or_equal:
				return compare_rel(_left, _right, c, pstd::less_equal());
			
			case ast_op_greater_or_equal:
				return compare_rel(_right, _left, c, pstd::less_equal());

			case ast_func_starts_with:
				return starts_with(_left->eval_string(c).c_str(), _right->eval_string(c).c_str());

			case ast_func_contains:
			{
				xpath_string lr = _left->eval_string(c);
				xpath_string rr = _right->eval_string(c);

				return find_substring(lr.c_str(), rr.c_str()) != 0;
			}

			case ast_func_boolean:
				return _left->eval_boolean(c);
				
			case ast_func_not:
				return !_left->eval_boolean(c);
				
			case ast_func_true:
				return true;
				
			case ast_func_false:
				return false;

			case ast_func_lang:
			{
				if (c.n.attribute()) return false;
				
				xpath_string lang = _left->eval_string(c);
				
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
				switch (_rettype)
				{
				case xpath_type_number:
					return convert_number_to_boolean(eval_number(c));
					
				case xpath_type_string:
					return !eval_string(c).empty();
					
				case xpath_type_node_set:				
					return !eval_node_set(c).empty();
					
				default:
					assert(!"Wrong expression for return type boolean");
					return false;
				}
			}
			}
		}

		double eval_number(const xpath_context& c)
		{
			switch (_type)
			{
			case ast_op_add:
				return _left->eval_number(c) + _right->eval_number(c);
				
			case ast_op_subtract:
				return _left->eval_number(c) - _right->eval_number(c);

			case ast_op_multiply:
				return _left->eval_number(c) * _right->eval_number(c);

			case ast_op_divide:
				return _left->eval_number(c) / _right->eval_number(c);

			case ast_op_mod:
				return fmod(_left->eval_number(c), _right->eval_number(c));

			case ast_op_negate:
				return -_left->eval_number(c);

			case ast_number_constant:
				return _data.number;

			case ast_func_last:
				return (double)c.size;
			
			case ast_func_position:
				return (double)c.position;

			case ast_func_count:
				return (double)_left->eval_node_set(c).size();
			
			case ast_func_string_length_0:
				return (double)string_value(c.n).length();
			
			case ast_func_string_length_1:
				return (double)_left->eval_string(c).length();
			
			case ast_func_number_0:
				return convert_string_to_number(string_value(c.n).c_str());
			
			case ast_func_number_1:
				return _left->eval_number(c);

			case ast_func_sum:
			{
				double r = 0;
				
				xpath_node_set ns = _left->eval_node_set(c);
				
				for (xpath_node_set::const_iterator it = ns.begin(); it != ns.end(); ++it)
					r += convert_string_to_number(string_value(*it).c_str());
			
				return r;
			}

			case ast_func_floor:
			{
				double r = _left->eval_number(c);
				
				return r == r ? floor(r) : r;
			}

			case ast_func_ceiling:
			{
				double r = _left->eval_number(c);
				
				return r == r ? ceil(r) : r;
			}

			case ast_func_round:
				return round_nearest_nzero(_left->eval_number(c));
			
			default:
			{
				switch (_rettype)
				{
				case xpath_type_boolean:
					return eval_boolean(c) ? 1 : 0;
					
				case xpath_type_string:
					return convert_string_to_number(eval_string(c).c_str());
					
				case xpath_type_node_set:
					return convert_string_to_number(eval_string(c).c_str());
					
				default:
					assert(!"Wrong expression for return type number");
					return 0;
				}
				
			}
			}
		}
		
		xpath_string eval_string(const xpath_context& c)
		{
			switch (_type)
			{
			case ast_string_constant:
				return xpath_string_const(_data.string);
			
			case ast_func_local_name_0:
			{
				xpath_node na = c.n;
				
				return xpath_string_const(local_name(na));
			}

			case ast_func_local_name_1:
			{
				xpath_node_set ns = _left->eval_node_set(c);
				xpath_node na = ns.first();
				
				return xpath_string_const(local_name(na));
			}

			case ast_func_name_0:
			{
				xpath_node na = c.n;
				
				return xpath_string_const(qualified_name(na));
			}

			case ast_func_name_1:
			{
				xpath_node_set ns = _left->eval_node_set(c);
				xpath_node na = ns.first();
				
				return xpath_string_const(qualified_name(na));
			}

			case ast_func_namespace_uri_0:
			{
				xpath_node na = c.n;
				
				return xpath_string_const(namespace_uri(na));
			}

			case ast_func_namespace_uri_1:
			{
				xpath_node_set ns = _left->eval_node_set(c);
				xpath_node na = ns.first();
				
				return xpath_string_const(namespace_uri(na));
			}

			case ast_func_string_0:
				return string_value(c.n);

			case ast_func_string_1:
				return _left->eval_string(c);

			case ast_func_concat:
			{
				xpath_string r = _left->eval_string(c);
				
				for (xpath_ast_node* n = _right; n; n = n->_next)
					r.append(n->eval_string(c));
			
				return r;
			}

			case ast_func_substring_before:
			{
				xpath_string s = _left->eval_string(c);
				xpath_string p = _right->eval_string(c);

				const char_t* pos = find_substring(s.c_str(), p.c_str());
				
				return pos ? xpath_string(s.c_str(), pos) : xpath_string();
			}
			
			case ast_func_substring_after:
			{
				xpath_string s = _left->eval_string(c);
				xpath_string p = _right->eval_string(c);
				
				const char_t* pos = find_substring(s.c_str(), p.c_str());
				
				return pos ? xpath_string(pos + p.length()) : xpath_string();
			}

			case ast_func_substring_2:
			{
				xpath_string s = _left->eval_string(c);
				double first = round_nearest(_right->eval_number(c));
				
				if (is_nan(first)) return xpath_string(); // NaN
				else if (first >= s.length() + 1) return xpath_string();
				
				size_t pos = first < 1 ? 1 : (size_t)first;
				
				return xpath_string(s.c_str() + (pos - 1));
			}
			
			case ast_func_substring_3:
			{
				xpath_string s = _left->eval_string(c);
				size_t s_length = s.length();

				double first = round_nearest(_right->eval_number(c));
				double last = first + round_nearest(_right->_next->eval_number(c));
				
				if (is_nan(first) || is_nan(last)) return xpath_string();
				else if (first >= s_length + 1) return xpath_string();
				else if (first >= last) return xpath_string();
				
				size_t pos = first < 1 ? 1 : (size_t)first;
				size_t end = last >= s_length + 1 ? s_length + 1 : (size_t)last;

				size_t size_requested = end - pos;
				size_t size_to_end = s_length - pos + 1;

				size_t size = size_requested < size_to_end ? size_requested : size_to_end;
				
				return xpath_string(s.c_str() + pos - 1, s.c_str() + pos - 1 + size);
			}

			case ast_func_normalize_space_0:
			{
				xpath_string s = string_value(c.n);

				normalize_space(s.data());

				return s;
			}

			case ast_func_normalize_space_1:
			{
				xpath_string s = _left->eval_string(c);

				normalize_space(s.data());
			
				return s;
			}

			case ast_func_translate:
			{
				xpath_string s = _left->eval_string(c);
				xpath_string from = _right->eval_string(c);
				xpath_string to = _right->_next->eval_string(c);

				translate(s.data(), from.c_str(), to.c_str());

				return s;
			}

			default:
			{
				switch (_rettype)
				{
				case xpath_type_boolean:
					return xpath_string_const(eval_boolean(c) ? PUGIXML_TEXT("true") : PUGIXML_TEXT("false"));
					
				case xpath_type_number:
					return convert_number_to_string(eval_number(c));
					
				case xpath_type_node_set:
				{
					xpath_node_set ns = eval_node_set(c);
					return ns.empty() ? xpath_string() : string_value(ns.first());
				}
				
				default:
					assert(!"Wrong expression for return type string");
					return xpath_string();
				}
			}
			}
		}

		xpath_node_set eval_node_set(const xpath_context& c)
		{
			switch (_type)
			{
			case ast_op_union:
			{
				xpath_node_set ls = _left->eval_node_set(c);
				xpath_node_set rs = _right->eval_node_set(c);
				
				// we can optimize merging two sorted sets, but this is a very rare operation, so don't bother
  		        ls._type = xpath_node_set::type_unsorted;

				ls.append(rs.begin(), rs.end());
				
				ls.remove_duplicates();
				
				return ls;
			}

			case ast_filter:
			case ast_filter_posinv:
			{
				xpath_node_set set = _left->eval_node_set(c);

				// either expression is a number or it contains position() call; sort by document order
				if (_type == ast_filter) set.sort();

				apply_predicate(set, 0, _right);
			
				return set;
			}
			
			case ast_func_id:
				return xpath_node_set();
			
			case ast_step:
			{
				xpath_node_set ns;
				
				switch (_axis)
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
				assert(!_right); // root step can't have any predicates

				xpath_node_set ns;

				if (c.n.node()) ns.push_back(c.n.node().root());
				else if (c.n.attribute()) ns.push_back(c.n.parent().root());

				return ns;
			}

			default:
				assert(!"Wrong expression for return type node set");
				return xpath_node_set();
			}
		}
		
		bool is_posinv()
		{
			switch (_type)
			{
			case ast_func_position:
				return false;

			case ast_string_constant:
			case ast_number_constant:
			// $$ case ast_variable:
				return true;

			case ast_step:
			case ast_step_root:
				return true;

			case ast_predicate:
			case ast_filter:
			case ast_filter_posinv:
				return true;

			default:
				if (_left && !_left->is_posinv()) return false;
				
				for (xpath_ast_node* n = _right; n; n = n->_next)
					if (!n->is_posinv()) return false;
					
				return true;
			}
		}

		xpath_value_type rettype() const
		{
			return static_cast<xpath_value_type>(_rettype);
		}
	};

	struct xpath_parser
	{
	    xpath_allocator& _alloc;
	    xpath_lexer _lexer;
		const char_t* _query;
		xpath_parse_result* _result;
		jmp_buf _error_handler;

		xpath_parser(const xpath_parser&);
		xpath_parser& operator=(const xpath_parser&);

		void throw_error(const char* message)
		{
			_result->error = message;
			_result->offset = _lexer.current_pos() - _query;

			longjmp(_error_handler, 1);
		}

		void* alloc_node()
		{
			void* result = _alloc.allocate(sizeof(xpath_ast_node));

			if (!result) throw_error("Out of memory");

			return result;
		}

		const char_t* alloc_string(const xpath_lexer_string& value)
		{
			if (value.begin)
			{
				size_t length = static_cast<size_t>(value.end - value.begin);

				char_t* c = static_cast<char_t*>(_alloc.allocate((length + 1) * sizeof(char_t)));
				if (!c) throw_error("Out of memory");

				memcpy(c, value.begin, length * sizeof(char_t));
				c[length] = 0;

				return c;
			}
			else return 0;
		}

		xpath_ast_node* parse_function_helper(ast_type_t type0, ast_type_t type1, size_t argc, xpath_ast_node* args[2])
		{
			assert(argc <= 1);

			if (argc == 1 && args[0]->rettype() != xpath_type_node_set) throw_error("Function has to be applied to node set");

			return new (alloc_node()) xpath_ast_node(argc == 0 ? type0 : type1, xpath_type_string, args[0]);
		}

		xpath_ast_node* parse_function(const xpath_lexer_string& name, size_t argc, xpath_ast_node* args[2])
		{
			switch (name.begin[0])
			{
			case 'b':
				if (name == PUGIXML_TEXT("boolean") && argc == 1)
					return new (alloc_node()) xpath_ast_node(ast_func_boolean, xpath_type_boolean, args[0]);
					
				break;
			
			case 'c':
				if (name == PUGIXML_TEXT("count") && argc == 1)
				{
					if (args[0]->rettype() != xpath_type_node_set) throw_error("count() has to be applied to node set");
					return new (alloc_node()) xpath_ast_node(ast_func_count, xpath_type_number, args[0]);
				}
				else if (name == PUGIXML_TEXT("contains") && argc == 2)
					return new (alloc_node()) xpath_ast_node(ast_func_contains, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("concat") && argc >= 2)
					return new (alloc_node()) xpath_ast_node(ast_func_concat, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("ceiling") && argc == 1)
					return new (alloc_node()) xpath_ast_node(ast_func_ceiling, xpath_type_number, args[0]);
					
				break;
			
			case 'f':
				if (name == PUGIXML_TEXT("false") && argc == 0)
					return new (alloc_node()) xpath_ast_node(ast_func_false, xpath_type_boolean);
				else if (name == PUGIXML_TEXT("floor") && argc == 1)
					return new (alloc_node()) xpath_ast_node(ast_func_floor, xpath_type_number, args[0]);
					
				break;
			
			case 'i':
				if (name == PUGIXML_TEXT("id") && argc == 1)
					return new (alloc_node()) xpath_ast_node(ast_func_id, xpath_type_node_set, args[0]);
					
				break;
			
			case 'l':
				if (name == PUGIXML_TEXT("last") && argc == 0)
					return new (alloc_node()) xpath_ast_node(ast_func_last, xpath_type_number);
				else if (name == PUGIXML_TEXT("lang") && argc == 1)
					return new (alloc_node()) xpath_ast_node(ast_func_lang, xpath_type_boolean, args[0]);
				else if (name == PUGIXML_TEXT("local-name") && argc <= 1)
					return parse_function_helper(ast_func_local_name_0, ast_func_local_name_1, argc, args);
			
				break;
			
			case 'n':
				if (name == PUGIXML_TEXT("name") && argc <= 1)
					return parse_function_helper(ast_func_name_0, ast_func_name_1, argc, args);
				else if (name == PUGIXML_TEXT("namespace-uri") && argc <= 1)
					return parse_function_helper(ast_func_namespace_uri_0, ast_func_namespace_uri_1, argc, args);
				else if (name == PUGIXML_TEXT("normalize-space") && argc <= 1)
					return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_normalize_space_0 : ast_func_normalize_space_1, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("not") && argc == 1)
					return new (alloc_node()) xpath_ast_node(ast_func_not, xpath_type_boolean, args[0]);
				else if (name == PUGIXML_TEXT("number") && argc <= 1)
					return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_number_0 : ast_func_number_1, xpath_type_number, args[0]);
			
				break;
			
			case 'p':
				if (name == PUGIXML_TEXT("position") && argc == 0)
					return new (alloc_node()) xpath_ast_node(ast_func_position, xpath_type_number);
				
				break;
			
			case 'r':
				if (name == PUGIXML_TEXT("round") && argc == 1)
					return new (alloc_node()) xpath_ast_node(ast_func_round, xpath_type_number, args[0]);

				break;
			
			case 's':
				if (name == PUGIXML_TEXT("string") && argc <= 1)
					return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_string_0 : ast_func_string_1, xpath_type_string, args[0]);
				else if (name == PUGIXML_TEXT("string-length") && argc <= 1)
					return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_string_length_0 : ast_func_string_length_1, xpath_type_string, args[0]);
				else if (name == PUGIXML_TEXT("starts-with") && argc == 2)
					return new (alloc_node()) xpath_ast_node(ast_func_starts_with, xpath_type_boolean, args[0], args[1]);
				else if (name == PUGIXML_TEXT("substring-before") && argc == 2)
					return new (alloc_node()) xpath_ast_node(ast_func_substring_before, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("substring-after") && argc == 2)
					return new (alloc_node()) xpath_ast_node(ast_func_substring_after, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("substring") && (argc == 2 || argc == 3))
					return new (alloc_node()) xpath_ast_node(argc == 2 ? ast_func_substring_2 : ast_func_substring_3, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("sum") && argc == 1)
				{
					if (args[0]->rettype() != xpath_type_node_set) throw_error("sum() has to be applied to node set");
					return new (alloc_node()) xpath_ast_node(ast_func_sum, xpath_type_number, args[0]);
				}

				break;
			
			case 't':
				if (name == PUGIXML_TEXT("translate") && argc == 3)
					return new (alloc_node()) xpath_ast_node(ast_func_translate, xpath_type_string, args[0], args[1]);
				else if (name == PUGIXML_TEXT("true") && argc == 0)
					return new (alloc_node()) xpath_ast_node(ast_func_true, xpath_type_boolean);
					
				break;
			}

			throw_error("Unrecognized function or wrong parameter count");

			return 0;
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
	    	switch (_lexer.current())
	    	{
	    	case lex_var_ref:
	    	{
				throw_error("Variables are not supported");

	    		return 0;
			}

			case lex_open_brace:
			{
				_lexer.next();

				xpath_ast_node* n = parse_expression();

				if (_lexer.current() != lex_close_brace)
					throw_error("Unmatched braces");

				_lexer.next();

				return n;
			}

			case lex_quoted_string:
			{
				const char_t* value = alloc_string(_lexer.contents());

				xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_string_constant, xpath_type_string, value);
				_lexer.next();

				return n;
			}

			case lex_number:
			{
				double value = 0;

				if (!convert_string_to_number(_lexer.contents().begin, _lexer.contents().end, &value))
					throw_error("Out of memory");

				xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_number_constant, xpath_type_number, value);
				_lexer.next();

				return n;
			}

			case lex_string:
			{
				xpath_ast_node* args[2] = {0};
				size_t argc = 0;
				
				xpath_lexer_string function = _lexer.contents();
				_lexer.next();
				
				xpath_ast_node* last_arg = 0;
				
				if (_lexer.current() != lex_open_brace)
					throw_error("Unrecognized function call");
				_lexer.next();

				if (_lexer.current() != lex_close_brace)
					args[argc++] = parse_expression();

				while (_lexer.current() != lex_close_brace)
				{
					if (_lexer.current() != lex_comma)
						throw_error("No comma between function arguments");
					_lexer.next();
					
					xpath_ast_node* n = parse_expression();
					
					if (argc < 2) args[argc] = n;
					else last_arg->set_next(n);

					argc++;
					last_arg = n;
				}
				
				_lexer.next();

				return parse_function(function, argc, args);
			}

	    	default:
	    		throw_error("Unrecognizable primary expression");

	    		return 0;
	    	}
	    }
	    
	    // FilterExpr ::= PrimaryExpr | FilterExpr Predicate
	    // Predicate ::= '[' PredicateExpr ']'
	    // PredicateExpr ::= Expr
	    xpath_ast_node* parse_filter_expression()
	    {
	    	xpath_ast_node* n = parse_primary_expression();

	    	while (_lexer.current() == lex_open_square_brace)
	    	{
	    		_lexer.next();

				xpath_ast_node* expr = parse_expression();

				if (n->rettype() != xpath_type_node_set) throw_error("Predicate has to be applied to node set");

				bool posinv = expr->rettype() != xpath_type_number && expr->is_posinv();

	    		n = new (alloc_node()) xpath_ast_node(posinv ? ast_filter_posinv : ast_filter, xpath_type_node_set, n, expr);

	    		if (_lexer.current() != lex_close_square_brace)
	    			throw_error("Unmatched square brace");
	    	
	    		_lexer.next();
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
				throw_error("Step has to be applied to node set");

			bool axis_specified = false;
			axis_t axis = axis_child; // implied child axis

			if (_lexer.current() == lex_axis_attribute)
			{
				axis = axis_attribute;
				axis_specified = true;
				
				_lexer.next();
			}
			else if (_lexer.current() == lex_dot)
			{
				_lexer.next();
				
				return new (alloc_node()) xpath_ast_node(ast_step, set, axis_self, nodetest_type_node, 0);
			}
			else if (_lexer.current() == lex_double_dot)
			{
				_lexer.next();
				
				return new (alloc_node()) xpath_ast_node(ast_step, set, axis_parent, nodetest_type_node, 0);
			}
	    
			nodetest_t nt_type = nodetest_none;
			xpath_lexer_string nt_name;
			
			if (_lexer.current() == lex_string)
			{
				// node name test
				nt_name = _lexer.contents();
				_lexer.next();

				// was it an axis name?
				if (_lexer.current() == lex_double_colon)
				{
					// parse axis name
					if (axis_specified) throw_error("Two axis specifiers in one step");

					axis = parse_axis_name(nt_name, axis_specified);

					if (!axis_specified) throw_error("Unknown axis");

					// read actual node test
					_lexer.next();

					if (_lexer.current() == lex_multiply)
					{
						nt_type = nodetest_all;
						nt_name = xpath_lexer_string();
						_lexer.next();
					}
					else if (_lexer.current() == lex_string)
					{
						nt_name = _lexer.contents();
						_lexer.next();
					}
					else throw_error("Unrecognized node test");
				}
				
				if (nt_type == nodetest_none)
				{
					// node type test or processing-instruction
					if (_lexer.current() == lex_open_brace)
					{
						_lexer.next();
						
						if (_lexer.current() == lex_close_brace)
						{
							_lexer.next();

							nt_type = parse_node_test_type(nt_name);

							if (nt_type == nodetest_none) throw_error("Unrecognized node type");
							
							nt_name = xpath_lexer_string();
						}
						else if (nt_name == PUGIXML_TEXT("processing-instruction"))
						{
							if (_lexer.current() != lex_quoted_string)
								throw_error("Only literals are allowed as arguments to processing-instruction()");
						
							nt_type = nodetest_pi;
							nt_name = _lexer.contents();
							_lexer.next();
							
							if (_lexer.current() != lex_close_brace)
								throw_error("Unmatched brace near processing-instruction()");
							_lexer.next();
						}
						else
							throw_error("Unmatched brace near node type test");

					}
					// QName or NCName:*
					else
					{
						const char_t* colon_pos = pstd::find(nt_name.begin, nt_name.end, ':');

						if (colon_pos + 2 == nt_name.end && colon_pos[1] == '*') // NCName:*
						{
							nt_name.end--; // erase *
							
							nt_type = nodetest_all_in_namespace;
						}
						else nt_type = nodetest_name;
					}
				}
			}
			else if (_lexer.current() == lex_multiply)
			{
				nt_type = nodetest_all;
				_lexer.next();
			}
			else throw_error("Unrecognized node test");
			
			xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_step, set, axis, nt_type, alloc_string(nt_name));
			
			xpath_ast_node* last = 0;
			
			while (_lexer.current() == lex_open_square_brace)
			{
				_lexer.next();
				
				xpath_ast_node* expr = parse_expression();

				xpath_ast_node* pred = new (alloc_node()) xpath_ast_node(ast_predicate, xpath_type_node_set, expr);
				
				if (_lexer.current() != lex_close_square_brace)
	    			throw_error("Unmatched square brace");
				_lexer.next();
				
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
			
			while (_lexer.current() == lex_slash || _lexer.current() == lex_double_slash)
			{
				lexeme_t l = _lexer.current();
				_lexer.next();

				if (l == lex_double_slash)
					n = new (alloc_node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0);
				
				n = parse_step(n);
			}
			
			return n;
	    }
	    
	    // LocationPath ::= RelativeLocationPath | AbsoluteLocationPath
	    // AbsoluteLocationPath ::= '/' RelativeLocationPath? | '//' RelativeLocationPath
	    xpath_ast_node* parse_location_path()
	    {
			if (_lexer.current() == lex_slash)
			{
				_lexer.next();
				
				xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_step_root, xpath_type_node_set);

				// relative location path can start from axis_attribute, dot, double_dot, multiply and string lexemes; any other lexeme means standalone root path
				lexeme_t l = _lexer.current();

				if (l == lex_string || l == lex_axis_attribute || l == lex_dot || l == lex_double_dot || l == lex_multiply)
					return parse_relative_location_path(n);
				else
					return n;
			}
			else if (_lexer.current() == lex_double_slash)
			{
				_lexer.next();
				
				xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_step_root, xpath_type_node_set);
				n = new (alloc_node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0);
				
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

			if (_lexer.current() == lex_var_ref || _lexer.current() == lex_open_brace || 
				_lexer.current() == lex_quoted_string || _lexer.current() == lex_number ||
				_lexer.current() == lex_string)
	    	{
	    		if (_lexer.current() == lex_string)
	    		{
	    			// This is either a function call, or not - if not, we shall proceed with location path
	    			const char_t* state = _lexer.state();
	    			
					while (IS_CHARTYPEX(*state, ctx_space)) ++state;
	    			
	    			if (*state != '(') return parse_location_path();

					// This looks like a function call; however this still can be a node-test. Check it.
					if (parse_node_test_type(_lexer.contents()) != nodetest_none) return parse_location_path();
	    		}
	    		
	    		xpath_ast_node* n = parse_filter_expression();

	    		if (_lexer.current() == lex_slash || _lexer.current() == lex_double_slash)
	    		{
					lexeme_t l = _lexer.current();
	    			_lexer.next();
	    			
					if (l == lex_double_slash)
					{
						if (n->rettype() != xpath_type_node_set) throw_error("Step has to be applied to node set");

						n = new (alloc_node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0);
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

	    	while (_lexer.current() == lex_union)
	    	{
	    		_lexer.next();

				xpath_ast_node* expr = parse_union_expression();

				if (n->rettype() != xpath_type_node_set || expr->rettype() != xpath_type_node_set)
					throw_error("Union operator has to be applied to node sets");

	    		n = new (alloc_node()) xpath_ast_node(ast_op_union, xpath_type_node_set, n, expr);
	    	}

	    	return n;
	    }

	    // UnaryExpr ::= UnionExpr | '-' UnaryExpr
	    xpath_ast_node* parse_unary_expression()
	    {
	    	if (_lexer.current() == lex_minus)
	    	{
	    		_lexer.next();

				xpath_ast_node* expr = parse_unary_expression();

	    		return new (alloc_node()) xpath_ast_node(ast_op_negate, xpath_type_number, expr);
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

	    	while (_lexer.current() == lex_multiply || (_lexer.current() == lex_string &&
	    		   (_lexer.contents() == PUGIXML_TEXT("mod") || _lexer.contents() == PUGIXML_TEXT("div"))))
	    	{
	    		ast_type_t op = _lexer.current() == lex_multiply ? ast_op_multiply :
	    			_lexer.contents().begin[0] == 'd' ? ast_op_divide : ast_op_mod;
	    		_lexer.next();

				xpath_ast_node* expr = parse_unary_expression();

	    		n = new (alloc_node()) xpath_ast_node(op, xpath_type_number, n, expr);
	    	}

	    	return n;
	    }

	    // AdditiveExpr ::= MultiplicativeExpr
	    //					| AdditiveExpr '+' MultiplicativeExpr
	    //					| AdditiveExpr '-' MultiplicativeExpr
	    xpath_ast_node* parse_additive_expression()
	    {
	    	xpath_ast_node* n = parse_multiplicative_expression();

	    	while (_lexer.current() == lex_plus || _lexer.current() == lex_minus)
	    	{
	    		lexeme_t l = _lexer.current();

	    		_lexer.next();

				xpath_ast_node* expr = parse_multiplicative_expression();

	    		n = new (alloc_node()) xpath_ast_node(l == lex_plus ? ast_op_add : ast_op_subtract, xpath_type_number, n, expr);
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

	    	while (_lexer.current() == lex_less || _lexer.current() == lex_less_or_equal || 
	    		   _lexer.current() == lex_greater || _lexer.current() == lex_greater_or_equal)
	    	{
	    		lexeme_t l = _lexer.current();
	    		_lexer.next();

				xpath_ast_node* expr = parse_additive_expression();

	    		n = new (alloc_node()) xpath_ast_node(l == lex_less ? ast_op_less : l == lex_greater ? ast_op_greater :
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

	    	while (_lexer.current() == lex_equal || _lexer.current() == lex_not_equal)
	    	{
	    		lexeme_t l = _lexer.current();

	    		_lexer.next();

				xpath_ast_node* expr = parse_relational_expression();

	    		n = new (alloc_node()) xpath_ast_node(l == lex_equal ? ast_op_equal : ast_op_not_equal, xpath_type_boolean, n, expr);
	    	}

	    	return n;
	    }
	    
	    // AndExpr ::= EqualityExpr | AndExpr 'and' EqualityExpr
	    xpath_ast_node* parse_and_expression()
	    {
	    	xpath_ast_node* n = parse_equality_expression();

	    	while (_lexer.current() == lex_string && _lexer.contents() == PUGIXML_TEXT("and"))
	    	{
	    		_lexer.next();

				xpath_ast_node* expr = parse_equality_expression();

	    		n = new (alloc_node()) xpath_ast_node(ast_op_and, xpath_type_boolean, n, expr);
	    	}

	    	return n;
	    }

	    // OrExpr ::= AndExpr | OrExpr 'or' AndExpr
	    xpath_ast_node* parse_or_expression()
	    {
	    	xpath_ast_node* n = parse_and_expression();

	    	while (_lexer.current() == lex_string && _lexer.contents() == PUGIXML_TEXT("or"))
	    	{
	    		_lexer.next();

				xpath_ast_node* expr = parse_and_expression();

	    		n = new (alloc_node()) xpath_ast_node(ast_op_or, xpath_type_boolean, n, expr);
	    	}

	    	return n;
	    }
		
		// Expr ::= OrExpr
		xpath_ast_node* parse_expression()
		{
			return parse_or_expression();
		}

		xpath_parser(const char_t* query, xpath_allocator& alloc, xpath_parse_result* result): _alloc(alloc), _lexer(query), _query(query), _result(result)
		{
		}

		xpath_ast_node* parse()
		{
			xpath_ast_node* result = parse_expression();
			
			if (_lexer.current() != lex_eof)
			{
				// there are still unparsed tokens left, error
				throw_error("Incorrect query");
			}
			
			return result;
		}

		static xpath_ast_node* parse(const char_t* query, xpath_allocator& alloc, xpath_parse_result* result)
		{
			xpath_parser parser(query, alloc, result);

			int error = setjmp(parser._error_handler);

			return (error == 0) ? parser.parse() : 0;
		}
	};

	const char* xpath_parse_result::description() const
	{
		return error ? error : "No error";
	}

	xpath_query::xpath_query(const char_t* query): _alloc(0), _root(0)
	{
		_result.error = 0;
		_result.offset = 0;

		_alloc = xpath_allocator::create();

		if (!_alloc)
		{
			_result.error = "Out of memory";
		}
		else
		{
			_root = xpath_parser::parse(query, *_alloc, &_result);
		}

		if (!_root)
		{
			xpath_allocator::destroy(_alloc);
			_alloc = 0;

		#ifndef PUGIXML_NO_EXCEPTIONS
			throw xpath_exception(_result);
		#endif
		}
	}

	xpath_query::~xpath_query()
	{
		xpath_allocator::destroy(_alloc);
	}

	xpath_value_type xpath_query::return_type() const
	{
		if (!_root) return xpath_type_none;

		return _root->rettype();
	}

	bool xpath_query::evaluate_boolean(const xml_node& n) const
	{
		if (!_root) return false;
		
		xpath_context c(n, 1, 1);
		
		return _root->eval_boolean(c);
	}
	
	double xpath_query::evaluate_number(const xml_node& n) const
	{
		if (!_root) return gen_nan();
		
		xpath_context c(n, 1, 1);
		
		return _root->eval_number(c);
	}
	
	string_t xpath_query::evaluate_string(const xml_node& n) const
	{
		if (!_root) return string_t();
		
		xpath_context c(n, 1, 1);
		
		return _root->eval_string(c).c_str();
	}
	
	xpath_node_set xpath_query::evaluate_node_set(const xml_node& n) const
	{
		if (!_root) return xpath_node_set();

		if (_root->rettype() != xpath_type_node_set)
		{
		#ifdef PUGIXML_NO_EXCEPTIONS
			return xpath_node_set();
		#else
			xpath_parse_result result = {"Expression does not evaluate to node set", 0};
			throw xpath_exception(result);
		#endif
		}
		
		xpath_context c(n, 1, 1);
		
		return _root->eval_node_set(c);
	}

	const xpath_parse_result& xpath_query::result() const
	{
		return _result;
	}

	xpath_query::operator unspecified_bool_type() const
	{
		return _root ? &xpath_query::_root : 0;
	}

	bool xpath_query::operator!() const
	{
		return !_root;
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
