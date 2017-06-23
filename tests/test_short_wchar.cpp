#if defined(__GNUC__) && __WCHAR_MAX__ == 0xffff
#undef PUGIXML_WCHAR_MODE
#define PUGIXML_WCHAR_MODE

#undef PUGIXML_NO_STL
#define PUGIXML_NO_STL

#define PUGIXML_HEADER_ONLY
#define pugi pugisw

#include "test.hpp"

#include <string.h>

using namespace pugi;

TEST(short_wchar_parse_utf32_be)
{
	xml_document doc;
	CHECK(doc.load_buffer("\x00\x00\xfe\xff\x00\x00\x00t\x00\x00\x00o\x00\x00\x00s\x00\x00\x00t", 20, parse_fragment));

	const char_t* text = doc.text().get();

	CHECK(text[0] == 't');
	CHECK(text[1] == 'o');
	CHECK(text[2] == 's');
	CHECK(text[3] == 't');
	CHECK(text[4] == 0);
}

TEST(short_wchar_parse_utf32_le)
{
	xml_document doc;
	CHECK(doc.load_buffer("\xff\xfe\x00\x00t\x00\x00\x00o\x00\x00\x00s\x00\x00\x00t\x00\x00\x00", 20, parse_fragment));

	const char_t* text = doc.text().get();

	CHECK(text[0] == 't');
	CHECK(text[1] == 'o');
	CHECK(text[2] == 's');
	CHECK(text[3] == 't');
	CHECK(text[4] == 0);
}

struct buffer_writer: xml_writer
{
	char buffer[128];
	size_t offset;

	buffer_writer(): offset(0)
	{
	}

	virtual void write(const void* data, size_t size) PUGIXML_OVERRIDE
	{
		CHECK(offset + size <= sizeof(buffer));

		memcpy(buffer + offset, data, size);
		offset += size;
	}
};

TEST(short_wchar_output_utf32_be)
{
	xml_document doc;
	doc.append_child(node_doctype);

	buffer_writer writer;
	doc.print(writer, L"", format_raw, encoding_utf32_be);

	CHECK(memcmp(writer.buffer, "\0\0\0<\0\0\0!\0\0\0D\0\0\0O\0\0\0C\0\0\0T\0\0\0Y\0\0\0P\0\0\0E\0\0\0>", 40) == 0);
}

TEST(short_wchar_output_utf32_le)
{
	xml_document doc;
	doc.append_child(node_doctype);

	buffer_writer writer;
	doc.print(writer, L"", format_raw, encoding_utf32_le);

	CHECK(memcmp(writer.buffer, "<\0\0\0!\0\0\0D\0\0\0O\0\0\0C\0\0\0T\0\0\0Y\0\0\0P\0\0\0E\0\0\0>\0\0\0", 40) == 0);
}
#endif
