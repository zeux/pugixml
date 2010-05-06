/**
 * pugixml parser - version 0.5
 * --------------------------------------------------------
 * Copyright (C) 2006-2009, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
 * Report bugs and download new versions at http://code.google.com/p/pugixml/
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 *
 * This work is based on the pugxml parser, which is:
 * Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
 */

#ifndef HEADER_PUGIUTF_HPP
#define HEADER_PUGIUTF_HPP

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

#endif

/**
 * Copyright (c) 2006-2009 Arseny Kapoulkine
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
