#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE 0

#include <string.h> // because Borland's STL is braindead, we have to include <string.h> _before_ <string> in order to get memcpy

#include "common.hpp"

#include "writer_string.hpp"

#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <sstream>

#include <string>

TEST(document_create_empty)
{
	pugi::xml_document doc;
	CHECK_NODE(doc, STR(""));
}

TEST(document_create)
{
	pugi::xml_document doc;
	doc.append_child().set_name(STR("node"));
	CHECK_NODE(doc, STR("<node />"));
}

#ifndef PUGIXML_NO_STL
TEST(document_load_stream)
{
	pugi::xml_document doc;

	std::istringstream iss("<node/>");
	CHECK(doc.load(iss));
	CHECK_NODE(doc, STR("<node />"));
}

TEST(document_load_stream_offset)
{
	pugi::xml_document doc;

	std::istringstream iss("<foobar> <node/>");

	std::string s;
	iss >> s;

	CHECK(doc.load(iss));
	CHECK_NODE(doc, STR("<node />"));
}

TEST(document_load_stream_text)
{
	pugi::xml_document doc;

	std::ifstream iss("tests/data/multiline.xml");
	CHECK(doc.load(iss));
	CHECK_NODE(doc, STR("<node1 /><node2 /><node3 />"));
}

TEST(document_load_stream_error)
{
	pugi::xml_document doc;

	std::ifstream fs1("filedoesnotexist");
	CHECK(doc.load(fs1).status == status_io_error);
	
#ifndef __DMC__ // Digital Mars CRT does not like 'con' pseudo-file
	std::ifstream fs2("con");
	CHECK(doc.load(fs2).status == status_io_error);
#endif

	test_runner::_memory_fail_threshold = 1;
	std::istringstream iss("<node/>");
	CHECK(doc.load(iss).status == status_out_of_memory);
}

TEST(document_load_stream_empty)
{
	std::istringstream iss;

	pugi::xml_document doc;
	doc.load(iss); // parse result depends on STL implementation
	CHECK(!doc.first_child());
}

TEST(document_load_stream_wide)
{
	pugi::xml_document doc;

	std::basic_istringstream<wchar_t> iss(L"<node/>");
	CHECK(doc.load(iss));
	CHECK_NODE(doc, STR("<node />"));
}
#endif

TEST(document_load_string)
{
	pugi::xml_document doc;

	CHECK(doc.load(STR("<node/>")));
	CHECK_NODE(doc, STR("<node />"));
}

TEST(document_load_file)
{
	pugi::xml_document doc;

	CHECK(doc.load_file("tests/data/small.xml"));
	CHECK_NODE(doc, STR("<node />"));
}

TEST(document_load_file_empty)
{
	pugi::xml_document doc;

	CHECK(doc.load_file("tests/data/empty.xml"));
	CHECK(!doc.first_child());
}

TEST(document_load_file_large)
{
	pugi::xml_document doc;

	CHECK(doc.load_file("tests/data/large.xml"));

	std::basic_string<pugi::char_t> str;
	str += STR("<node>");
	for (int i = 0; i < 10000; ++i) str += STR("<node />");
	str += STR("</node>");

	CHECK_NODE(doc, str.c_str());
}

TEST(document_load_file_error)
{
	pugi::xml_document doc;

	CHECK(doc.load_file("filedoesnotexist").status == status_file_not_found);

#ifdef _WIN32
#ifndef __DMC__ // Digital Mars CRT does not like 'con' pseudo-file
	CHECK(doc.load_file("con").status == status_io_error);
#endif
#endif

	test_runner::_memory_fail_threshold = 1;
	CHECK(doc.load_file("tests/data/small.xml").status == status_out_of_memory);
}

TEST_XML(document_save, "<node/>")
{
	xml_writer_string writer;

	doc.save(writer, STR(""), pugi::format_no_declaration | pugi::format_raw, get_native_encoding());

	CHECK(writer.as_string() == STR("<node />"));
}

#ifndef PUGIXML_NO_STL
TEST_XML(document_save_stream, "<node/>")
{
	std::ostringstream oss;

	doc.save(oss, STR(""), pugi::format_no_declaration | pugi::format_raw);

	CHECK(oss.str() == "<node />");
}

TEST_XML(document_save_stream_wide, "<node/>")
{
	std::basic_ostringstream<wchar_t> oss;

	doc.save(oss, STR(""), pugi::format_no_declaration | pugi::format_raw);

	CHECK(oss.str() == L"<node />");
}
#endif

TEST_XML(document_save_bom, "<n/>")
{
	unsigned int flags = format_no_declaration | format_raw | format_write_bom;

	// specific encodings
	CHECK(test_save_narrow(doc, flags, encoding_utf8, "\xef\xbb\xbf<n />", 8));
	CHECK(test_save_narrow(doc, flags, encoding_utf16_be, "\xfe\xff\x00<\x00n\x00 \x00/\x00>", 12));
	CHECK(test_save_narrow(doc, flags, encoding_utf16_le, "\xff\xfe<\x00n\x00 \x00/\x00>\x00", 12));
	CHECK(test_save_narrow(doc, flags, encoding_utf32_be, "\x00\x00\xfe\xff\x00\x00\x00<\x00\x00\x00n\x00\x00\x00 \x00\x00\x00/\x00\x00\x00>", 24));
	CHECK(test_save_narrow(doc, flags, encoding_utf32_le, "\xff\xfe\x00\x00<\x00\x00\x00n\x00\x00\x00 \x00\x00\x00/\x00\x00\x00>\x00\x00\x00", 24));

	// encodings synonyms
	CHECK(save_narrow(doc, flags, encoding_utf16) == save_narrow(doc, flags, (is_little_endian() ? encoding_utf16_le : encoding_utf16_be)));
	CHECK(save_narrow(doc, flags, encoding_utf32) == save_narrow(doc, flags, (is_little_endian() ? encoding_utf32_le : encoding_utf32_be)));

	size_t wcharsize = sizeof(wchar_t);
	CHECK(save_narrow(doc, flags, encoding_wchar) == save_narrow(doc, flags, (wcharsize == 2 ? encoding_utf16 : encoding_utf32)));
}

TEST_XML(document_save_declaration, "<node/>")
{
	xml_writer_string writer;

	doc.save(writer, STR(""), pugi::format_default, get_native_encoding());

	CHECK(writer.as_string() == STR("<?xml version=\"1.0\"?>\n<node />\n"));
}

TEST_XML(document_save_file, "<node/>")
{
	CHECK(doc.save_file("tests/data/output.xml"));

	CHECK(doc.load_file("tests/data/output.xml", pugi::parse_default | pugi::parse_declaration));
	CHECK_NODE(doc, STR("<?xml version=\"1.0\"?><node />"));

	unlink("tests/data/output.xml");
}

TEST(document_load_buffer)
{
	const pugi::char_t text[] = STR("<?xml?><node/>");

	pugi::xml_document doc;

	CHECK(doc.load_buffer(text, sizeof(text)));
	CHECK_NODE(doc, STR("<node />"));
}

TEST(document_load_buffer_inplace)
{
	pugi::char_t text[] = STR("<?xml?><node/>");

	pugi::xml_document doc;

	CHECK(doc.load_buffer_inplace(text, sizeof(text)));
	CHECK_NODE(doc, STR("<node />"));
}

TEST(document_load_buffer_inplace_own)
{
	allocation_function alloc = get_memory_allocation_function();

	size_t size = strlen("<?xml?><node/>") * sizeof(pugi::char_t);

	pugi::char_t* text = static_cast<pugi::char_t*>(alloc(size));
	CHECK(text);

	memcpy(text, STR("<?xml?><node/>"), size);

	pugi::xml_document doc;

	CHECK(doc.load_buffer_inplace_own(text, size));
	CHECK_NODE(doc, STR("<node />"));
}

TEST(document_parse_result_bool)
{
	xml_parse_result result;

	result.status = status_ok;
	CHECK(result);
	CHECK(!!result);
	CHECK(result == true);

	for (int i = 1; i < 20; ++i)
	{
		result.status = (xml_parse_status)i;
		CHECK(!result);
		CHECK(result == false);
	}
}

TEST(document_parse_result_description)
{
	xml_parse_result result;

	for (int i = 0; i < 20; ++i)
	{
		result.status = (xml_parse_status)i;

		CHECK(result.description() != 0);
		CHECK(result.description()[0] != 0);
	}
}

TEST(document_load_fail)
{
	xml_document doc;
	CHECK(!doc.load(STR("<foo><bar/>")));
	CHECK(doc.child(STR("foo")).child(STR("bar")));
}

inline void check_utftest_document(const xml_document& doc)
{
	// ascii text
	CHECK_STRING(doc.last_child().first_child().name(), STR("English"));

	// check that we have parsed some non-ascii text
	CHECK((unsigned)doc.last_child().last_child().name()[0] >= 0x80);

	// check magic string
	const pugi::char_t* v = doc.last_child().child(STR("Heavy")).previous_sibling().child_value();

#ifdef PUGIXML_WCHAR_MODE
	CHECK(v[0] == 0x4e16 && v[1] == 0x754c && v[2] == 0x6709 && v[3] == 0x5f88 && v[4] == 0x591a && v[5] == 0x8bed && v[6] == 0x8a00);

	// last character is a surrogate pair
	unsigned int v7 = v[7];
	size_t wcharsize = sizeof(wchar_t);

	CHECK(wcharsize == 2 ? (v[7] == 0xd852 && v[8] == 0xdf62) : (v7 == 0x24b62));
#else
	// unicode string
	CHECK_STRING(v, "\xe4\xb8\x96\xe7\x95\x8c\xe6\x9c\x89\xe5\xbe\x88\xe5\xa4\x9a\xe8\xaf\xad\xe8\xa8\x80\xf0\xa4\xad\xa2");
#endif
}

TEST(document_load_file_convert_auto)
{
	const char* files[] =
	{
		"tests/data/utftest_utf16_be.xml",
		"tests/data/utftest_utf16_be_bom.xml",
		"tests/data/utftest_utf16_be_nodecl.xml",
		"tests/data/utftest_utf16_le.xml",
		"tests/data/utftest_utf16_le_bom.xml",
		"tests/data/utftest_utf16_le_nodecl.xml",
		"tests/data/utftest_utf32_be.xml",
		"tests/data/utftest_utf32_be_bom.xml",
		"tests/data/utftest_utf32_be_nodecl.xml",
		"tests/data/utftest_utf32_le.xml",
		"tests/data/utftest_utf32_le_bom.xml",
		"tests/data/utftest_utf32_le_nodecl.xml",
		"tests/data/utftest_utf8.xml",
		"tests/data/utftest_utf8_bom.xml",
		"tests/data/utftest_utf8_nodecl.xml"
	};

	encoding_t encodings[] =
	{
		encoding_utf16_be, encoding_utf16_be, encoding_utf16_be,
		encoding_utf16_le, encoding_utf16_le, encoding_utf16_le,
		encoding_utf32_be, encoding_utf32_be, encoding_utf32_be,
		encoding_utf32_le, encoding_utf32_le, encoding_utf32_le,
		encoding_utf8, encoding_utf8, encoding_utf8
	};

	for (unsigned int i = 0; i < sizeof(files) / sizeof(files[0]); ++i)
	{
		xml_document doc;
		xml_parse_result res = doc.load_file(files[i]);

		CHECK(res);
		CHECK(res.encoding == encodings[i]);
		check_utftest_document(doc);
	}
}

TEST(document_load_file_convert_specific)
{
	const char* files[] =
	{
		"tests/data/utftest_utf16_be.xml",
		"tests/data/utftest_utf16_be_bom.xml",
		"tests/data/utftest_utf16_be_nodecl.xml",
		"tests/data/utftest_utf16_le.xml",
		"tests/data/utftest_utf16_le_bom.xml",
		"tests/data/utftest_utf16_le_nodecl.xml",
		"tests/data/utftest_utf32_be.xml",
		"tests/data/utftest_utf32_be_bom.xml",
		"tests/data/utftest_utf32_be_nodecl.xml",
		"tests/data/utftest_utf32_le.xml",
		"tests/data/utftest_utf32_le_bom.xml",
		"tests/data/utftest_utf32_le_nodecl.xml",
		"tests/data/utftest_utf8.xml",
		"tests/data/utftest_utf8_bom.xml",
		"tests/data/utftest_utf8_nodecl.xml"
	};

	encoding_t encodings[] =
	{
		encoding_utf16_be, encoding_utf16_be, encoding_utf16_be,
		encoding_utf16_le, encoding_utf16_le, encoding_utf16_le,
		encoding_utf32_be, encoding_utf32_be, encoding_utf32_be,
		encoding_utf32_le, encoding_utf32_le, encoding_utf32_le,
		encoding_utf8, encoding_utf8, encoding_utf8
	};

	for (unsigned int i = 0; i < sizeof(files) / sizeof(files[0]); ++i)
	{
		for (unsigned int j = 0; j < sizeof(files) / sizeof(files[0]); ++j)
		{
			encoding_t encoding = encodings[j];

			xml_document doc;
			xml_parse_result res = doc.load_file(files[i], parse_default, encoding);

			if (encoding == encodings[i])
			{
				CHECK(res);
				CHECK(res.encoding == encoding);
				check_utftest_document(doc);
			}
			else
			{
				// should not get past first tag
				CHECK(!doc.first_child());
			}
		}
	}
}

TEST(document_load_file_convert_native_endianness)
{
	const char* files[2][6] =
	{
		{
			"tests/data/utftest_utf16_be.xml",
			"tests/data/utftest_utf16_be_bom.xml",
			"tests/data/utftest_utf16_be_nodecl.xml",
			"tests/data/utftest_utf32_be.xml",
			"tests/data/utftest_utf32_be_bom.xml",
			"tests/data/utftest_utf32_be_nodecl.xml",
		},
		{
			"tests/data/utftest_utf16_le.xml",
			"tests/data/utftest_utf16_le_bom.xml",
			"tests/data/utftest_utf16_le_nodecl.xml",
			"tests/data/utftest_utf32_le.xml",
			"tests/data/utftest_utf32_le_bom.xml",
			"tests/data/utftest_utf32_le_nodecl.xml",
		}
	};

	encoding_t encodings[] =
	{
		encoding_utf16, encoding_utf16, encoding_utf16,
		encoding_utf32, encoding_utf32, encoding_utf32
	};

	for (unsigned int i = 0; i < sizeof(files[0]) / sizeof(files[0][0]); ++i)
	{
		const char* right_file = files[is_little_endian()][i];
		const char* wrong_file = files[!is_little_endian()][i];

		for (unsigned int j = 0; j < sizeof(encodings) / sizeof(encodings[0]); ++j)
		{
			encoding_t encoding = encodings[j];

			// check file with right endianness
			{
				xml_document doc;
				xml_parse_result res = doc.load_file(right_file, parse_default, encoding);

				if (encoding == encodings[i])
				{
					CHECK(res);
					check_utftest_document(doc);
				}
				else
				{
					// should not get past first tag
					CHECK(!doc.first_child());
				}
			}

			// check file with wrong endianness
			{
				xml_document doc;
				doc.load_file(wrong_file, parse_default, encoding);
				CHECK(!doc.first_child());
			}
		}
	}
}

static bool load_file_in_memory(const char* path, char*& data, size_t& size)
{
	FILE* file = fopen(path, "rb");
	if (!file) return false;

	fseek(file, 0, SEEK_END);
	size = (size_t)ftell(file);
	fseek(file, 0, SEEK_SET);

	data = new char[size];

	CHECK(fread(data, 1, size, file) == size);
	fclose(file);

	return true;
}

TEST(document_contents_preserve)
{
	struct file_t
	{
		const char* path;
		encoding_t encoding;

		char* data;
		size_t size;
	};

	file_t files[] =
	{
		{"tests/data/utftest_utf16_be_clean.xml", encoding_utf16_be, 0, 0},
		{"tests/data/utftest_utf16_le_clean.xml", encoding_utf16_le, 0, 0},
		{"tests/data/utftest_utf32_be_clean.xml", encoding_utf32_be, 0, 0},
		{"tests/data/utftest_utf32_le_clean.xml", encoding_utf32_le, 0, 0},
		{"tests/data/utftest_utf8_clean.xml", encoding_utf8, 0, 0}
	};

	// load files in memory
	for (unsigned int i = 0; i < sizeof(files) / sizeof(files[0]); ++i)
	{
		CHECK(load_file_in_memory(files[i].path, files[i].data, files[i].size));
	}

	// convert each file to each format and compare bitwise
	for (unsigned int src = 0; src < sizeof(files) / sizeof(files[0]); ++src)
	{
		for (unsigned int dst = 0; dst < sizeof(files) / sizeof(files[0]); ++dst)
		{
			// parse into document (preserve comments, declaration and whitespace pcdata)
			xml_document doc;
			CHECK(doc.load_buffer(files[src].data, files[src].size, parse_default | parse_ws_pcdata | parse_declaration | parse_comments));

			// compare saved document with the original (raw formatting, without extra declaration, write bom if it was in original file)
			CHECK(test_save_narrow(doc, format_raw | format_no_declaration | format_write_bom, files[dst].encoding, files[dst].data, files[dst].size));
		}
	}

	// cleanup
	for (unsigned int j = 0; j < sizeof(files) / sizeof(files[0]); ++j)
	{
		delete[] files[j].data;
	}
}

static bool test_parse_fail(const void* buffer, size_t size, encoding_t encoding = encoding_utf8)
{
	// copy buffer to heap (to enable out-of-bounds checks)
	void* temp = malloc(size);
	memcpy(temp, buffer, size);

	// check that this parses without buffer overflows (yielding an error)
	xml_document doc;
	bool result = doc.load_buffer_inplace(temp, size, parse_default, encoding);

	free(temp);

	return !result;
}

TEST(document_convert_invalid_utf8)
{
	// invalid 1-byte input
	CHECK(test_parse_fail("<\xb0", 2));

	// invalid 2-byte input
	CHECK(test_parse_fail("<\xc0", 2));
	CHECK(test_parse_fail("<\xd0", 2));

	// invalid 3-byte input
	CHECK(test_parse_fail("<\xe2\x80", 3));
	CHECK(test_parse_fail("<\xe2", 2));

	// invalid 4-byte input
	CHECK(test_parse_fail("<\xf2\x97\x98", 4));
	CHECK(test_parse_fail("<\xf2\x97", 3));
	CHECK(test_parse_fail("<\xf2", 2));

	// invalid 5-byte input
	CHECK(test_parse_fail("<\xf8", 2));
}

TEST(document_convert_invalid_utf16)
{
	// check non-terminated degenerate handling
	CHECK(test_parse_fail("\x00<\xda\x1d", 4, encoding_utf16_be));
	CHECK(test_parse_fail("<\x00\x1d\xda", 4, encoding_utf16_le));

	// check incorrect leading code
	CHECK(test_parse_fail("\x00<\xde\x24", 4, encoding_utf16_be));
	CHECK(test_parse_fail("<\x00\x24\xde", 4, encoding_utf16_le));
}

TEST(document_load_buffer_empty)
{
	encoding_t encodings[] =
	{
		encoding_auto,
		encoding_utf8,
		encoding_utf16_le,
		encoding_utf16_be,
		encoding_utf16,
		encoding_utf32_le,
		encoding_utf32_be,
		encoding_utf32,
		encoding_wchar
	};

	char buffer[1];

	for (unsigned int i = 0; i < sizeof(encodings) / sizeof(encodings[0]); ++i)
	{
		encoding_t encoding = encodings[i];

		xml_document doc;
		CHECK(doc.load_buffer(buffer, 0, parse_default, encoding) && !doc.first_child());
		CHECK(doc.load_buffer(0, 0, parse_default, encoding) && !doc.first_child());

		CHECK(doc.load_buffer_inplace(buffer, 0, parse_default, encoding) && !doc.first_child());
		CHECK(doc.load_buffer_inplace(0, 0, parse_default, encoding) && !doc.first_child());

		void* own_buffer = pugi::get_memory_allocation_function()(1);

		CHECK(doc.load_buffer_inplace_own(own_buffer, 0, parse_default, encoding) && !doc.first_child());
		CHECK(doc.load_buffer_inplace_own(0, 0, parse_default, encoding) && !doc.first_child());
	}
}
