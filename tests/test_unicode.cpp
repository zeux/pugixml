#include "common.hpp"

// letters taken from http://www.utf8-chartable.de/

#ifdef __DMC__
#define U_LITERALS // DMC does not understand \x01234 (it parses first three digits), but understands \u01234
#endif

inline wchar_t wchar_cast(unsigned int value)
{
	return static_cast<wchar_t>(value); // to avoid C4310 on MSVC
}

#ifndef PUGIXML_NO_STL
TEST(as_utf16)
{
	// valid 1-byte, 2-byte and 3-byte inputs
#ifdef U_LITERALS
	CHECK(as_utf16("?\xd0\x80\xe2\x80\xbd") == L"?\u0400\u203D");
#else
	CHECK(as_utf16("?\xd0\x80\xe2\x80\xbd") == L"?\x0400\x203D");
#endif

	// invalid 1-byte input
	CHECK(as_utf16("\xb0") == L" ");
	
	// valid 4-byte input
	std::wstring b4 = as_utf16("\xf2\x97\x98\xa4 \xf4\x80\x8f\xbf");
	CHECK(b4.size() == 3 && b4[0] == wchar_cast(0x97624) && b4[1] == L' ' && b4[2] == wchar_cast(0x1003ff));

	// invalid 5-byte input
	std::wstring b5 = as_utf16("\xf8\nbcd");
	CHECK(b5 == L" \nbcd");
}

TEST(as_utf8)
{
	// valid 1-byte, 2-byte and 3-byte outputs
#ifdef U_LITERALS
	CHECK(as_utf8(L"?\u0400\u203D") == "?\xd0\x80\xe2\x80\xbd");
#else
	CHECK(as_utf8(L"?\x0400\x203D") == "?\xd0\x80\xe2\x80\xbd");
#endif
	
	// valid 4-byte output
#if 0
	// requires 4-byte wchar_t :(
	CHECK(as_utf8(L"\x97624 \x1003ff") == "\xf2\x97\x98\xa4 \xf4\x80\x8f\xbf");
#endif
}
#endif

TEST_XML(parse_bom_utf8, "\xef\xbb\xbf<node/>")
{
	CHECK_NODE(doc, "<node />");
}
