// This file includes all tests for deprecated functionality; this is going away in the next release!

#include <string.h>

#include "common.hpp"

#include "writer_string.hpp"

#ifdef _MSC_VER
#	pragma warning(disable: 4996)
#endif

// format_write_bom_utf8 - it's now format_write_bom!
TEST_XML(document_save_bom_utf8, "<node/>")
{
	xml_writer_string writer;

	CHECK(test_save_narrow(doc, pugi::format_no_declaration | pugi::format_raw | pugi::format_write_bom_utf8, encoding_utf8, "\xef\xbb\xbf<node />", 11));
}

// parse - it's now load_buffer_inplace
TEST(document_parse)
{
	char text[] = "<node/>";

	pugi::xml_document doc;

	CHECK(doc.parse(text));
	CHECK_NODE(doc, STR("<node />"));
}

// parse with transfer_ownership_tag attribute - it's now load_buffer_inplace_own
TEST(document_parse_transfer_ownership)
{
	allocation_function alloc = get_memory_allocation_function();

	char* text = static_cast<char*>(alloc(strlen("<node/>") + 1));
	CHECK(text);

	memcpy(text, "<node/>", strlen("<node/>") + 1);

	pugi::xml_document doc;

	CHECK(doc.parse(transfer_ownership_tag(), text));
	CHECK_NODE(doc, STR("<node />"));
}

#ifndef PUGIXML_NO_STL
// as_utf16 - it's now as_wide
TEST(as_utf16)
{
	CHECK(as_utf16("") == L"");

	// valid 1-byte, 2-byte and 3-byte inputs
#ifdef U_LITERALS
	CHECK(as_utf16("?\xd0\x80\xe2\x80\xbd") == L"?\u0400\u203D");
#else
	CHECK(as_utf16("?\xd0\x80\xe2\x80\xbd") == L"?\x0400\x203D");
#endif
}
#endif
