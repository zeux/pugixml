#include "common.hpp"

// letters taken from http://www.utf8-chartable.de/

TEST(as_utf16)
{
	// valid 1-byte, 2-byte and 3-byte inputs
	CHECK(as_utf16("?\xd0\x80\xe2\x80\xbd") == L"?\x0400\x203D");
	
	// invalid 1-byte input
	CHECK(as_utf16("\xb0") == L" ");
	
	// valid 4-byte input
	std::wstring b4 = as_utf16("\xf2\x97\x98\xa4 \xf4\x80\x8f\xbf");
	CHECK(b4.size() == 3 && b4[0] == (wchar_t)0x97624 && b4[1] == L' ' && b4[2] == (wchar_t)0x1003ff);

	// invalid 5-byte input
	std::wstring b5 = as_utf16("\xf8\nbcd");
	CHECK(b5 == L" \nbcd");
}

TEST(as_utf8)
{
	// valid 1-byte, 2-byte and 3-byte outputs
	CHECK(as_utf8(L"?\x0400\x203D") == "?\xd0\x80\xe2\x80\xbd");
	
	// valid 4-byte output
#if 0
	// requires 4-byte wchar_t :(
	CHECK(as_utf8(L"\x97624 \x1003ff") == "\xf2\x97\x98\xa4 \xf4\x80\x8f\xbf");
#endif
}

TEST_XML(parse_bom_utf8, "\xef\xbb\xbf<node/>")
{
	CHECK_NODE(doc, "<node />");
}
