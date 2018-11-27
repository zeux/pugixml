#define _CRT_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE 0

#include <string.h> // because Borland's STL is braindead, we have to include <string.h> _before_ <string> in order to get memcpy

#include "test.hpp"

#include "writer_string.hpp"

#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <sstream>

#include <string>
#include <algorithm>

#ifndef PUGIXML_NO_EXCEPTIONS
#	include <stdexcept>
#endif

#ifdef __MINGW32__
#	include <io.h> // for unlink in C++0x mode
#endif

#if defined(__CELLOS_LV2__) || defined(ANDROID) || defined(_GLIBCXX_HAVE_UNISTD_H) || defined(__APPLE__)
#	include <unistd.h> // for unlink
#endif

using namespace pugi;

static bool load_file_in_memory(const char* path, char*& data, size_t& size)
{
	FILE* file = fopen(path, "rb");
	if (!file) return false;

	fseek(file, 0, SEEK_END);
	long length = ftell(file);
	fseek(file, 0, SEEK_SET);

	CHECK(length >= 0);
	size = static_cast<size_t>(length);
	data = new char[size];

	CHECK(fread(data, 1, size, file) == size);
	fclose(file);

	return true;
}

static bool test_file_contents(const char* path, const char* data, size_t size)
{
    char* fdata;
    size_t fsize;
    if (!load_file_in_memory(path, fdata, fsize)) return false;

    bool result = (size == fsize && memcmp(data, fdata, size) == 0);

    delete[] fdata;

    return result;
}

TEST(document_create_empty)
{
	xml_document doc;
	CHECK_NODE(doc, STR(""));
}

TEST(document_create)
{
	xml_document doc;
	doc.append_child().set_name(STR("node"));
	CHECK_NODE(doc, STR("<node/>"));
}

#ifndef PUGIXML_NO_STL
TEST(document_load_stream)
{
	xml_document doc;

	std::istringstream iss("<node/>");
	CHECK(doc.load(iss));
	CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_stream_offset)
{
	xml_document doc;

	std::istringstream iss("<foobar> <node/>");

	std::string s;
	iss >> s;

	CHECK(doc.load(iss));
	CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_stream_text)
{
	xml_document doc;

	std::ifstream iss("tests/data/multiline.xml");
	CHECK(doc.load(iss));
	CHECK_NODE(doc, STR("<node1/><node2/><node3/>"));
}

TEST(document_load_stream_error)
{
	xml_document doc;

	std::ifstream fs("filedoesnotexist");
	CHECK(doc.load(fs).status == status_io_error);
}

TEST(document_load_stream_out_of_memory)
{
	xml_document doc;

	std::istringstream iss("<node/>");
	test_runner::_memory_fail_threshold = 1;
	CHECK_ALLOC_FAIL(CHECK(doc.load(iss).status == status_out_of_memory));
}

TEST(document_load_stream_wide_out_of_memory)
{
	xml_document doc;

	std::basic_istringstream<wchar_t> iss(L"<node/>");
	test_runner::_memory_fail_threshold = 1;
	CHECK_ALLOC_FAIL(CHECK(doc.load(iss).status == status_out_of_memory));
}

TEST(document_load_stream_empty)
{
	std::istringstream iss;

	xml_document doc;
	doc.load(iss); // parse result depends on STL implementation
	CHECK(!doc.first_child());
}

TEST(document_load_stream_wide)
{
	xml_document doc;

	std::basic_istringstream<wchar_t> iss(L"<node/>");
	CHECK(doc.load(iss));
	CHECK_NODE(doc, STR("<node/>"));
}

#ifndef PUGIXML_NO_EXCEPTIONS
TEST(document_load_stream_exceptions)
{
	xml_document doc;

	// Windows has newline translation for text-mode files, so reading from this stream reaches eof and sets fail|eof bits.
	// This test does not cause stream to throw an exception on Linux - I have no idea how to get read() to fail except
	// newline translation.
	std::ifstream iss("tests/data/multiline.xml");
	iss.exceptions(std::ios::eofbit | std::ios::badbit | std::ios::failbit);

	try
	{
		doc.load(iss);

		CHECK(iss.good()); // if the exception was not thrown, stream reading should succeed without errors
	}
	catch (const std::ios_base::failure&)
	{
		CHECK(!doc.first_child());
	}
}
#endif

TEST(document_load_stream_error_previous)
{
	xml_document doc;
	CHECK(doc.load_string(STR("<node/>")));
	CHECK(doc.first_child());

	std::ifstream fs1("filedoesnotexist");
	CHECK(doc.load(fs1).status == status_io_error);
	CHECK(!doc.first_child());
}

TEST(document_load_stream_wide_error_previous)
{
	xml_document doc;
	CHECK(doc.load_string(STR("<node/>")));
	CHECK(doc.first_child());

	std::basic_ifstream<wchar_t> fs1("filedoesnotexist");
	CHECK(doc.load(fs1).status == status_io_error);
	CHECK(!doc.first_child());
}

template <typename T> class char_array_buffer: public std::basic_streambuf<T>
{
public:
    char_array_buffer(T* begin, T* end)
    {
        this->setg(begin, begin, end);
    }
};

TEST(document_load_stream_nonseekable)
{
    char contents[] = "<node />";
    char_array_buffer<char> buffer(contents, contents + sizeof(contents) / sizeof(contents[0]));
    std::istream in(&buffer);

    xml_document doc;
    CHECK(doc.load(in));
    CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_stream_wide_nonseekable)
{
    wchar_t contents[] = L"<node />";
    char_array_buffer<wchar_t> buffer(contents, contents + sizeof(contents) / sizeof(contents[0]));
    std::basic_istream<wchar_t> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in));
    CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_stream_nonseekable_large)
{
	std::basic_string<char_t> str;
	str += STR("<node>");
	for (int i = 0; i < 10000; ++i) str += STR("<node/>");
	str += STR("</node>");

    char_array_buffer<char_t> buffer(&str[0], &str[0] + str.length());
    std::basic_istream<char_t> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in));
    CHECK_NODE(doc, str.c_str());
}

TEST(document_load_stream_nonseekable_out_of_memory)
{
    char contents[] = "<node />";
    char_array_buffer<char> buffer(contents, contents + sizeof(contents) / sizeof(contents[0]));
    std::istream in(&buffer);

    test_runner::_memory_fail_threshold = 1;

    xml_document doc;
    CHECK_ALLOC_FAIL(CHECK(doc.load(in).status == status_out_of_memory));
}

TEST(document_load_stream_wide_nonseekable_out_of_memory)
{
    wchar_t contents[] = L"<node />";
    char_array_buffer<wchar_t> buffer(contents, contents + sizeof(contents) / sizeof(contents[0]));
    std::basic_istream<wchar_t> in(&buffer);

    test_runner::_memory_fail_threshold = 1;

    xml_document doc;
    CHECK_ALLOC_FAIL(CHECK(doc.load(in).status == status_out_of_memory));
}

TEST(document_load_stream_nonseekable_out_of_memory_large)
{
	std::basic_string<char> str;
	str += "<node>";
	for (int i = 0; i < 10000; ++i) str += "<node />";
	str += "</node>";

    char_array_buffer<char> buffer(&str[0], &str[0] + str.length());
    std::basic_istream<char> in(&buffer);

    test_runner::_memory_fail_threshold = 32768 * 3 + 4096;

    xml_document doc;
    CHECK_ALLOC_FAIL(CHECK(doc.load(in).status == status_out_of_memory));
}

TEST(document_load_stream_wide_nonseekable_out_of_memory_large)
{
	std::basic_string<wchar_t> str;
	str += L"<node>";
	for (int i = 0; i < 10000; ++i) str += L"<node />";
	str += L"</node>";

    char_array_buffer<wchar_t> buffer(&str[0], &str[0] + str.length());
    std::basic_istream<wchar_t> in(&buffer);

    test_runner::_memory_fail_threshold = 32768 * 3 * sizeof(wchar_t) + 4096;

    xml_document doc;
    CHECK_ALLOC_FAIL(CHECK(doc.load(in).status == status_out_of_memory));
}

template <typename T> class seek_fail_buffer: public std::basic_streambuf<T>
{
public:
	int seeks;

	seek_fail_buffer(): seeks(0)
	{
	}

	typename std::basic_streambuf<T>::pos_type seekoff(typename std::basic_streambuf<T>::off_type, std::ios_base::seekdir, std::ios_base::openmode) PUGIXML_OVERRIDE
	{
		seeks++;

		// pretend that our buffer is seekable (this is called by tellg)
		return seeks == 1 ? 0 : -1;
	}
};

TEST(document_load_stream_seekable_fail_seek)
{
    seek_fail_buffer<char> buffer;
    std::basic_istream<char> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in).status == status_io_error);
}

TEST(document_load_stream_wide_seekable_fail_seek)
{
    seek_fail_buffer<wchar_t> buffer;
    std::basic_istream<wchar_t> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in).status == status_io_error);
}

template <typename T> class tell_fail_buffer: public std::basic_streambuf<T>
{
public:
	int seeks;

	tell_fail_buffer(): seeks(0)
	{
	}

	typename std::basic_streambuf<T>::pos_type seekoff(typename std::basic_streambuf<T>::off_type, std::ios_base::seekdir dir, std::ios_base::openmode) PUGIXML_OVERRIDE
	{
		seeks++;

		return seeks > 1 && dir == std::ios_base::cur ? -1 : 0;
	}

	typename std::basic_streambuf<T>::pos_type seekpos(typename std::basic_streambuf<T>::pos_type, std::ios_base::openmode) PUGIXML_OVERRIDE
	{
		return 0;
	}
};

TEST(document_load_stream_seekable_fail_tell)
{
    tell_fail_buffer<char> buffer;
    std::basic_istream<char> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in).status == status_io_error);
}

TEST(document_load_stream_wide_seekable_fail_tell)
{
    tell_fail_buffer<wchar_t> buffer;
    std::basic_istream<wchar_t> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in).status == status_io_error);
}

#ifndef PUGIXML_NO_EXCEPTIONS
template <typename T> class read_fail_buffer: public std::basic_streambuf<T>
{
public:
    typename std::basic_streambuf<T>::int_type underflow() PUGIXML_OVERRIDE
	{
		throw std::runtime_error("underflow failed");

	#ifdef __DMC__
		return 0;
	#endif
	}
};

TEST(document_load_stream_nonseekable_fail_read)
{
    read_fail_buffer<char> buffer;
    std::basic_istream<char> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in).status == status_io_error);
}

TEST(document_load_stream_wide_nonseekable_fail_read)
{
    read_fail_buffer<wchar_t> buffer;
    std::basic_istream<wchar_t> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in).status == status_io_error);
}

template <typename T> class read_fail_seekable_buffer: public std::basic_streambuf<T>
{
public:
	typename std::basic_streambuf<T>::pos_type offset;

    read_fail_seekable_buffer(): offset(0)
    {
    }

    typename std::basic_streambuf<T>::int_type underflow() PUGIXML_OVERRIDE
	{
		throw std::runtime_error("underflow failed");

	#ifdef __DMC__
		return 0;
	#endif
	}

	typename std::basic_streambuf<T>::pos_type seekoff(typename std::basic_streambuf<T>::off_type off, std::ios_base::seekdir dir, std::ios_base::openmode) PUGIXML_OVERRIDE
	{
		switch (dir)
		{
		case std::ios_base::beg: offset = off; break;
		case std::ios_base::cur: offset += off; break;
		case std::ios_base::end: offset = 16 + off; break;
		default: ;
		}
		return offset;
	}

	typename std::basic_streambuf<T>::pos_type seekpos(typename std::basic_streambuf<T>::pos_type pos, std::ios_base::openmode) PUGIXML_OVERRIDE
	{
		offset = pos;
		return pos;
	}
};

TEST(document_load_stream_seekable_fail_read)
{
    read_fail_seekable_buffer<char> buffer;
    std::basic_istream<char> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in).status == status_io_error);
}

TEST(document_load_stream_wide_seekable_fail_read)
{
    read_fail_seekable_buffer<wchar_t> buffer;
    std::basic_istream<wchar_t> in(&buffer);

    xml_document doc;
    CHECK(doc.load(in).status == status_io_error);
}
#endif
#endif

TEST(document_load_string)
{
	xml_document doc;

	CHECK(doc.load_string(STR("<node/>")));
	CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_file)
{
	xml_document doc;

	CHECK(doc.load_file("tests/data/small.xml"));
	CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_file_empty)
{
	xml_document doc;

	CHECK(doc.load_file("tests/data/empty.xml").status == status_no_document_element);
	CHECK(!doc.first_child());
}

TEST(document_load_file_large)
{
	xml_document doc;

	CHECK(doc.load_file("tests/data/large.xml"));

	std::basic_string<char_t> str;
	str += STR("<node>");
	for (int i = 0; i < 10000; ++i) str += STR("<node/>");
	str += STR("</node>");

	CHECK_NODE(doc, str.c_str());
}

TEST(document_load_file_error)
{
	xml_document doc;

	CHECK(doc.load_file("filedoesnotexist").status == status_file_not_found);
}

TEST(document_load_file_out_of_memory)
{
	test_runner::_memory_fail_threshold = 1;

	xml_document doc;
	CHECK_ALLOC_FAIL(CHECK(doc.load_file("tests/data/small.xml").status == status_out_of_memory));
}

TEST(document_load_file_out_of_memory_file_leak)
{
	test_runner::_memory_fail_threshold = 1;

	xml_document doc;

	for (int i = 0; i < 256; ++i)
		CHECK_ALLOC_FAIL(CHECK(doc.load_file("tests/data/small.xml").status == status_out_of_memory));

	test_runner::_memory_fail_threshold = 0;

	CHECK(doc.load_file("tests/data/small.xml"));
	CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_file_wide_out_of_memory_file_leak)
{
	test_runner::_memory_fail_threshold = 256;

	xml_document doc;

	for (int i = 0; i < 256; ++i)
		CHECK_ALLOC_FAIL(CHECK(doc.load_file(L"tests/data/small.xml").status == status_out_of_memory));

	test_runner::_memory_fail_threshold = 0;

	CHECK(doc.load_file(L"tests/data/small.xml"));
	CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_file_error_previous)
{
	xml_document doc;
	CHECK(doc.load_string(STR("<node/>")));
	CHECK(doc.first_child());

	CHECK(doc.load_file("filedoesnotexist").status == status_file_not_found);
	CHECK(!doc.first_child());
}

TEST(document_load_file_wide_ascii)
{
	xml_document doc;

	CHECK(doc.load_file(L"tests/data/small.xml"));
	CHECK_NODE(doc, STR("<node/>"));
}

#if !defined(__DMC__) && !defined(__MWERKS__) && !(defined(__MINGW32__) && defined(__STRICT_ANSI__) && !defined(__MINGW64_VERSION_MAJOR)) && !defined(__BORLANDC__)
TEST(document_load_file_wide_unicode)
{
	xml_document doc;

	CHECK(doc.load_file(L"tests/data/\x0442\x0435\x0441\x0442.xml"));
	CHECK_NODE(doc, STR("<node/>"));
}
#endif

TEST(document_load_file_wide_out_of_memory)
{
	test_runner::_memory_fail_threshold = 1;

	xml_document doc;

	xml_parse_result result;
	result.status = status_out_of_memory;
	CHECK_ALLOC_FAIL(result = doc.load_file(L"tests/data/small.xml"));

	CHECK(result.status == status_out_of_memory || result.status == status_file_not_found);
}

#if defined(__APPLE__)
TEST(document_load_file_special_folder)
{
	xml_document doc;
	xml_parse_result result = doc.load_file(".");
	CHECK(result.status == status_io_error);
}
#endif

#if defined(__linux__)
TEST(document_load_file_special_device)
{
	xml_document doc;
	xml_parse_result result = doc.load_file("/dev/tty");
	CHECK(result.status == status_file_not_found || result.status == status_io_error);
}
#endif

TEST_XML(document_save, "<node/>")
{
	xml_writer_string writer;

	doc.save(writer, STR(""), format_no_declaration | format_raw, get_native_encoding());

	CHECK(writer.as_string() == STR("<node/>"));
}

#ifndef PUGIXML_NO_STL
TEST_XML(document_save_stream, "<node/>")
{
	std::ostringstream oss;

	doc.save(oss, STR(""), format_no_declaration | format_raw);

	CHECK(oss.str() == "<node/>");
}

TEST_XML(document_save_stream_wide, "<node/>")
{
	std::basic_ostringstream<wchar_t> oss;

	doc.save(oss, STR(""), format_no_declaration | format_raw);

	CHECK(oss.str() == L"<node/>");
}
#endif

TEST_XML(document_save_bom, "<n/>")
{
	unsigned int flags = format_no_declaration | format_raw | format_write_bom;

	// specific encodings
	CHECK(test_save_narrow(doc, flags, encoding_utf8, "\xef\xbb\xbf<n/>", 7));
	CHECK(test_save_narrow(doc, flags, encoding_utf16_be, "\xfe\xff\x00<\x00n\x00/\x00>", 10));
	CHECK(test_save_narrow(doc, flags, encoding_utf16_le, "\xff\xfe<\x00n\x00/\x00>\x00", 10));
	CHECK(test_save_narrow(doc, flags, encoding_utf32_be, "\x00\x00\xfe\xff\x00\x00\x00<\x00\x00\x00n\x00\x00\x00/\x00\x00\x00>", 20));
	CHECK(test_save_narrow(doc, flags, encoding_utf32_le, "\xff\xfe\x00\x00<\x00\x00\x00n\x00\x00\x00/\x00\x00\x00>\x00\x00\x00", 20));
	CHECK(test_save_narrow(doc, flags, encoding_latin1, "<n/>", 4));

	// encodings synonyms
	CHECK(save_narrow(doc, flags, encoding_utf16) == save_narrow(doc, flags, (is_little_endian() ? encoding_utf16_le : encoding_utf16_be)));
	CHECK(save_narrow(doc, flags, encoding_utf32) == save_narrow(doc, flags, (is_little_endian() ? encoding_utf32_le : encoding_utf32_be)));

	size_t wcharsize = sizeof(wchar_t);
	CHECK(save_narrow(doc, flags, encoding_wchar) == save_narrow(doc, flags, (wcharsize == 2 ? encoding_utf16 : encoding_utf32)));
}

TEST_XML(document_save_declaration, "<node/>")
{
	xml_writer_string writer;

	doc.save(writer, STR(""), format_default, get_native_encoding());

	CHECK(writer.as_string() == STR("<?xml version=\"1.0\"?>\n<node />\n"));
}

TEST(document_save_declaration_empty)
{
	xml_document doc;

	xml_writer_string writer;

	doc.save(writer, STR(""), format_default, get_native_encoding());

	CHECK(writer.as_string() == STR("<?xml version=\"1.0\"?>\n"));
}

TEST_XML(document_save_declaration_present_first, "<node/>")
{
	doc.insert_child_before(node_declaration, doc.first_child()).append_attribute(STR("encoding")) = STR("utf8");

	xml_writer_string writer;

	doc.save(writer, STR(""), format_default, get_native_encoding());

	CHECK(writer.as_string() == STR("<?xml encoding=\"utf8\"?>\n<node />\n"));
}

TEST_XML(document_save_declaration_present_second, "<node/>")
{
	doc.insert_child_before(node_declaration, doc.first_child()).append_attribute(STR("encoding")) = STR("utf8");
	doc.insert_child_before(node_comment, doc.first_child()).set_value(STR("text"));

	xml_writer_string writer;

	doc.save(writer, STR(""), format_default, get_native_encoding());

	CHECK(writer.as_string() == STR("<!--text-->\n<?xml encoding=\"utf8\"?>\n<node />\n"));
}

TEST_XML(document_save_declaration_present_last, "<node/>")
{
	doc.append_child(node_declaration).append_attribute(STR("encoding")) = STR("utf8");

	xml_writer_string writer;

	doc.save(writer, STR(""), format_default, get_native_encoding());

	// node writer only looks for declaration before the first element child
	CHECK(writer.as_string() == STR("<?xml version=\"1.0\"?>\n<node />\n<?xml encoding=\"utf8\"?>\n"));
}

TEST_XML(document_save_declaration_latin1, "<node/>")
{
	xml_writer_string writer;

	doc.save(writer, STR(""), format_default, encoding_latin1);

	CHECK(writer.as_narrow() == "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n<node />\n");
}

TEST_XML(document_save_declaration_raw, "<node/>")
{
	xml_writer_string writer;

	doc.save(writer, STR(""), format_raw, get_native_encoding());

	CHECK(writer.as_string() == STR("<?xml version=\"1.0\"?><node/>"));
}

struct temp_file
{
	char path[512];

	temp_file()
	{
		static int index = 0;
		sprintf(path, "%stempfile%d", test_runner::_temp_path, index++);
	}

	~temp_file()
	{
	#ifndef _WIN32_WCE
		CHECK(unlink(path) == 0);
	#endif
	}
};

TEST_XML(document_save_file, "<node/>")
{
	temp_file f;

	CHECK(doc.save_file(f.path));

	CHECK(doc.load_file(f.path, parse_default | parse_declaration));
	CHECK_NODE(doc, STR("<?xml version=\"1.0\"?><node/>"));
}

TEST_XML(document_save_file_wide, "<node/>")
{
	temp_file f;

	// widen the path
	wchar_t wpath[sizeof(f.path)];
	std::copy(f.path, f.path + strlen(f.path) + 1, wpath + 0);

	CHECK(doc.save_file(wpath));

	CHECK(doc.load_file(f.path, parse_default | parse_declaration));
	CHECK_NODE(doc, STR("<?xml version=\"1.0\"?><node/>"));
}

TEST_XML(document_save_file_error, "<node/>")
{
	CHECK(!doc.save_file("tests/data/unknown/output.xml"));
}

TEST_XML(document_save_file_text, "<node/>")
{
	temp_file f;

	CHECK(doc.save_file(f.path, STR(""), format_no_declaration | format_save_file_text));
    CHECK(test_file_contents(f.path, "<node />\n", 9) || test_file_contents(f.path, "<node />\r\n", 10));

	CHECK(doc.save_file(f.path, STR(""), format_no_declaration));
    CHECK(test_file_contents(f.path, "<node />\n", 9));
}

TEST_XML(document_save_file_wide_text, "<node/>")
{
	temp_file f;

	// widen the path
	wchar_t wpath[sizeof(f.path)];
	std::copy(f.path, f.path + strlen(f.path) + 1, wpath + 0);

	CHECK(doc.save_file(wpath, STR(""), format_no_declaration | format_save_file_text));
    CHECK(test_file_contents(f.path, "<node />\n", 9) || test_file_contents(f.path, "<node />\r\n", 10));

	CHECK(doc.save_file(wpath, STR(""), format_no_declaration));
    CHECK(test_file_contents(f.path, "<node />\n", 9));
}

TEST_XML(document_save_file_leak, "<node/>")
{
	temp_file f;

	for (int i = 0; i < 256; ++i)
		CHECK(doc.save_file(f.path));
}

TEST_XML(document_save_file_wide_leak, "<node/>")
{
	temp_file f;

	// widen the path
	wchar_t wpath[sizeof(f.path)];
	std::copy(f.path, f.path + strlen(f.path) + 1, wpath + 0);

	for (int i = 0; i < 256; ++i)
		CHECK(doc.save_file(wpath));
}

TEST(document_load_buffer)
{
	const char_t text[] = STR("<?xml?><node/>");

	xml_document doc;

	CHECK(doc.load_buffer(text, sizeof(text)));
	CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_buffer_inplace)
{
	char_t text[] = STR("<?xml?><node/>");

	xml_document doc;

	CHECK(doc.load_buffer_inplace(text, sizeof(text)));
	CHECK_NODE(doc, STR("<node/>"));
}

TEST(document_load_buffer_inplace_own)
{
	allocation_function alloc = get_memory_allocation_function();

	size_t size = strlen("<?xml?><node/>") * sizeof(char_t);

	char_t* text = static_cast<char_t*>(alloc(size));
	CHECK(text);

	memcpy(text, STR("<?xml?><node/>"), size);

	xml_document doc;

	CHECK(doc.load_buffer_inplace_own(text, size));
	CHECK_NODE(doc, STR("<node/>"));
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
		result.status = static_cast<xml_parse_status>(i);
		CHECK(!result);
		CHECK(result == false);
	}
}

TEST(document_parse_result_description)
{
	xml_parse_result result;

	for (int i = 0; i < 20; ++i)
	{
		result.status = static_cast<xml_parse_status>(i);

		CHECK(result.description() != 0);
		CHECK(result.description()[0] != 0);
	}
}

TEST(document_load_fail)
{
	xml_document doc;
	CHECK(!doc.load_string(STR("<foo><bar/>")));
	CHECK(doc.child(STR("foo")).child(STR("bar")));
}

inline void check_utftest_document(const xml_document& doc)
{
	// ascii text
	CHECK_STRING(doc.last_child().first_child().name(), STR("English"));

	// check that we have parsed some non-ascii text
	CHECK(static_cast<unsigned int>(doc.last_child().last_child().name()[0]) >= 0x80);

	// check magic string
	const char_t* v = doc.last_child().child(STR("Heavy")).previous_sibling().child_value();

#ifdef PUGIXML_WCHAR_MODE
	CHECK(v[0] == 0x4e16 && v[1] == 0x754c && v[2] == 0x6709 && v[3] == 0x5f88 && v[4] == 0x591a && v[5] == wchar_cast(0x8bed) && v[6] == wchar_cast(0x8a00));

	// last character is a surrogate pair
	size_t wcharsize = sizeof(wchar_t);

	CHECK(wcharsize == 2 ? (v[7] == wchar_cast(0xd852) && v[8] == wchar_cast(0xdf62)) : (v[7] == wchar_cast(0x24b62)));
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

	xml_encoding encodings[] =
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

	xml_encoding encodings[] =
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
			xml_encoding encoding = encodings[j];

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

	xml_encoding encodings[] =
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
			xml_encoding encoding = encodings[j];

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

struct file_data_t
{
    const char* path;
    xml_encoding encoding;

    char* data;
    size_t size;
};


TEST(document_contents_preserve)
{
	file_data_t files[] =
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

TEST(document_contents_preserve_latin1)
{
	file_data_t files[] =
	{
		{"tests/data/latintest_utf8.xml", encoding_utf8, 0, 0},
		{"tests/data/latintest_latin1.xml", encoding_latin1, 0, 0}
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

static bool test_parse_fail(const void* buffer, size_t size, xml_encoding encoding = encoding_utf8)
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
	xml_encoding encodings[] =
	{
		encoding_auto,
		encoding_utf8,
		encoding_utf16_le,
		encoding_utf16_be,
		encoding_utf16,
		encoding_utf32_le,
		encoding_utf32_be,
		encoding_utf32,
		encoding_wchar,
        encoding_latin1
	};

	char buffer[1];

	for (unsigned int i = 0; i < sizeof(encodings) / sizeof(encodings[0]); ++i)
	{
		xml_encoding encoding = encodings[i];

		xml_document doc;
		CHECK(doc.load_buffer(buffer, 0, parse_default, encoding).status == status_no_document_element && !doc.first_child());
		CHECK(doc.load_buffer(0, 0, parse_default, encoding).status == status_no_document_element && !doc.first_child());

		CHECK(doc.load_buffer_inplace(buffer, 0, parse_default, encoding).status == status_no_document_element && !doc.first_child());
		CHECK(doc.load_buffer_inplace(0, 0, parse_default, encoding).status == status_no_document_element && !doc.first_child());

		void* own_buffer = get_memory_allocation_function()(1);

		CHECK(doc.load_buffer_inplace_own(own_buffer, 0, parse_default, encoding).status == status_no_document_element && !doc.first_child());
		CHECK(doc.load_buffer_inplace_own(0, 0, parse_default, encoding).status == status_no_document_element && !doc.first_child());
	}
}

TEST(document_load_buffer_empty_fragment)
{
	xml_encoding encodings[] =
	{
		encoding_auto,
		encoding_utf8,
		encoding_utf16_le,
		encoding_utf16_be,
		encoding_utf16,
		encoding_utf32_le,
		encoding_utf32_be,
		encoding_utf32,
		encoding_wchar,
        encoding_latin1
	};

	char buffer[1];

	for (unsigned int i = 0; i < sizeof(encodings) / sizeof(encodings[0]); ++i)
	{
		xml_encoding encoding = encodings[i];

		xml_document doc;
		CHECK(doc.load_buffer(buffer, 0, parse_fragment, encoding) && !doc.first_child());
		CHECK(doc.load_buffer(0, 0, parse_fragment, encoding) && !doc.first_child());

		CHECK(doc.load_buffer_inplace(buffer, 0, parse_fragment, encoding) && !doc.first_child());
		CHECK(doc.load_buffer_inplace(0, 0, parse_fragment, encoding) && !doc.first_child());

		void* own_buffer = get_memory_allocation_function()(1);

		CHECK(doc.load_buffer_inplace_own(own_buffer, 0, parse_fragment, encoding) && !doc.first_child());
		CHECK(doc.load_buffer_inplace_own(0, 0, parse_fragment, encoding) && !doc.first_child());
	}
}

TEST(document_load_buffer_null)
{
	xml_document doc;

	CHECK(doc.load_buffer(0, 12).status == status_io_error && !doc.first_child());
	CHECK(doc.load_buffer(0, 12, parse_fragment).status == status_io_error && !doc.first_child());

	CHECK(doc.load_buffer_inplace(0, 12).status == status_io_error && !doc.first_child());
	CHECK(doc.load_buffer_inplace_own(0, 12).status == status_io_error && !doc.first_child());
}

TEST(document_progressive_truncation)
{
	char* original_data;
	size_t original_size;

	CHECK(load_file_in_memory("tests/data/truncation.xml", original_data, original_size));

	char* buffer = new char[original_size];

	for (size_t i = 1; i <= original_size; ++i)
	{
		char* truncated_data = buffer + original_size - i;

		// default flags
		{
			memcpy(truncated_data, original_data, i);

			xml_document doc;
			bool result = doc.load_buffer_inplace(truncated_data, i);

			// only eof is parseable
			CHECK((i == original_size) ? result : !result);
		}

		// fragment mode
		{
			memcpy(truncated_data, original_data, i);

			xml_document doc;
			bool result = doc.load_buffer_inplace(truncated_data, i, parse_default | parse_fragment);

			// some truncate locations are parseable - those that come after declaration, declaration + doctype, declaration + doctype + comment and eof
			CHECK(((i >= 21 && i <= 23) || (i >= 66 && i <= 68) || (i >= 95 && i <= 97) || i == original_size) ? result : !result);
		}
	}

	delete[] buffer;
	delete[] original_data;
}

TEST(document_load_buffer_short)
{
	char* data = new char[4];
	memcpy(data, "abcd", 4);

	xml_document doc;

	CHECK(doc.load_buffer(data, 4).status == status_no_document_element);
	CHECK(doc.load_buffer(data + 1, 3).status == status_no_document_element);
	CHECK(doc.load_buffer(data + 2, 2).status == status_no_document_element);
	CHECK(doc.load_buffer(data + 3, 1).status == status_no_document_element);
	CHECK(doc.load_buffer(data + 4, 0).status == status_no_document_element);
	CHECK(doc.load_buffer(0, 0).status == status_no_document_element);

	delete[] data;
}

TEST(document_load_buffer_short_fragment)
{
	char* data = new char[4];
	memcpy(data, "abcd", 4);

	xml_document doc;

	CHECK(doc.load_buffer(data, 4, parse_fragment) && test_string_equal(doc.text().get(), STR("abcd")));
	CHECK(doc.load_buffer(data + 1, 3, parse_fragment) && test_string_equal(doc.text().get(), STR("bcd")));
	CHECK(doc.load_buffer(data + 2, 2, parse_fragment) && test_string_equal(doc.text().get(), STR("cd")));
	CHECK(doc.load_buffer(data + 3, 1, parse_fragment) && test_string_equal(doc.text().get(), STR("d")));
	CHECK(doc.load_buffer(data + 4, 0, parse_fragment) && !doc.first_child());
	CHECK(doc.load_buffer(0, 0, parse_fragment) && !doc.first_child());

	delete[] data;
}

TEST(document_load_buffer_inplace_short)
{
	char* data = new char[4];
	memcpy(data, "abcd", 4);

	xml_document doc;

	CHECK(doc.load_buffer_inplace(data, 4).status == status_no_document_element);
	CHECK(doc.load_buffer_inplace(data + 1, 3).status == status_no_document_element);
	CHECK(doc.load_buffer_inplace(data + 2, 2).status == status_no_document_element);
	CHECK(doc.load_buffer_inplace(data + 3, 1).status == status_no_document_element);
	CHECK(doc.load_buffer_inplace(data + 4, 0).status == status_no_document_element);
	CHECK(doc.load_buffer_inplace(0, 0).status == status_no_document_element);

	delete[] data;
}

#ifndef PUGIXML_NO_EXCEPTIONS
TEST(document_load_exceptions)
{
    bool thrown = false;

    try
    {
        xml_document doc;
        if (!doc.load_string(STR("<node attribute='value"))) throw std::bad_alloc();

        CHECK_FORCE_FAIL("Expected parsing failure");
    }
    catch (const std::bad_alloc&)
    {
        thrown = true;
    }

    CHECK(thrown);
}
#endif

TEST_XML_FLAGS(document_element, "<?xml version='1.0'?><node><child/></node><!---->", parse_default | parse_declaration | parse_comments)
{
    CHECK(doc.document_element() == doc.child(STR("node")));
}

TEST_XML_FLAGS(document_element_absent, "<!---->", parse_comments | parse_fragment)
{
    CHECK(doc.document_element() == xml_node());
}

TEST_XML(document_reset, "<node><child/></node>")
{
    CHECK(doc.first_child());

    doc.reset();
    CHECK(!doc.first_child());
    CHECK_NODE(doc, STR(""));

    doc.reset();
    CHECK(!doc.first_child());
    CHECK_NODE(doc, STR(""));

    CHECK(doc.load_string(STR("<node/>")));
    CHECK(doc.first_child());
    CHECK_NODE(doc, STR("<node/>"));

    doc.reset();
    CHECK(!doc.first_child());
    CHECK_NODE(doc, STR(""));
}

TEST(document_reset_empty)
{
    xml_document doc;

    doc.reset();
    CHECK(!doc.first_child());
    CHECK_NODE(doc, STR(""));
}

TEST_XML(document_reset_copy, "<node><child/></node>")
{
    xml_document doc2;

    CHECK_NODE(doc2, STR(""));

    doc2.reset(doc);

    CHECK_NODE(doc2, STR("<node><child/></node>"));
    CHECK(doc.first_child() != doc2.first_child());

    doc.reset(doc2);

    CHECK_NODE(doc, STR("<node><child/></node>"));
    CHECK(doc.first_child() != doc2.first_child());

    CHECK(doc.first_child().offset_debug() == -1);
}

TEST_XML(document_reset_copy_self, "<node><child/></node>")
{
    CHECK_NODE(doc, STR("<node><child/></node>"));

    doc.reset(doc);

    CHECK(!doc.first_child());
    CHECK_NODE(doc, STR(""));
}

TEST(document_load_buffer_utf_truncated)
{
	const unsigned char utf8[] = {'<', 0xe2, 0x82, 0xac, '/', '>'};
	const unsigned char utf16_be[] = {0, '<', 0x20, 0xac, 0, '/', 0, '>'};
	const unsigned char utf16_le[] = {'<', 0, 0xac, 0x20, '/', 0, '>', 0};
	const unsigned char utf32_be[] = {0, 0, 0, '<', 0, 0, 0x20, 0xac, 0, 0, 0, '/', 0, 0, 0, '>'};
	const unsigned char utf32_le[] = {'<', 0, 0, 0, 0xac, 0x20, 0, 0, '/', 0, 0, 0, '>', 0, 0, 0};

	struct document_data_t
	{
	    xml_encoding encoding;

	    const unsigned char* data;
	    size_t size;
	};

	const document_data_t data[] =
	{
		{ encoding_utf8, utf8, sizeof(utf8) },
		{ encoding_utf16_be, utf16_be, sizeof(utf16_be) },
		{ encoding_utf16_le, utf16_le, sizeof(utf16_le) },
		{ encoding_utf32_be, utf32_be, sizeof(utf32_be) },
		{ encoding_utf32_le, utf32_le, sizeof(utf32_le) },
	};

	for (size_t i = 0; i < sizeof(data) / sizeof(data[0]); ++i)
	{
		const document_data_t& d = data[i];

		for (size_t j = 0; j <= d.size; ++j)
		{
			char* buffer = new char[j];
			memcpy(buffer, d.data, j);

			xml_document doc;
			xml_parse_result res = doc.load_buffer(buffer, j, parse_default, d.encoding);

			if (j == d.size)
			{
				CHECK(res);

				const char_t* name = doc.first_child().name();

			#ifdef PUGIXML_WCHAR_MODE
				CHECK(name[0] == 0x20ac && name[1] == 0);
			#else
				CHECK_STRING(name, "\xe2\x82\xac");
			#endif
			}
			else
			{
				CHECK(!res || !doc.first_child());
			}

			delete[] buffer;
		}
	}
}

#ifndef PUGIXML_NO_STL
TEST(document_load_stream_truncated)
{
	const unsigned char utf32_be[] = {0, 0, 0, '<', 0, 0, 0x20, 0xac, 0, 0, 0, '/', 0, 0, 0, '>'};

	for (size_t i = 0; i <= sizeof(utf32_be); ++i)
	{
		std::string prefix(reinterpret_cast<const char*>(utf32_be), i);
		std::istringstream iss(prefix);

		xml_document doc;
		xml_parse_result res = doc.load(iss);

		if (i == sizeof(utf32_be))
		{
			CHECK(res);
		}
		else
		{
			CHECK(!res || !doc.first_child());

			if (i < 8)
			{
				CHECK(!doc.first_child());
			}
			else
			{
				const char_t* name = doc.first_child().name();

			#ifdef PUGIXML_WCHAR_MODE
				CHECK(name[0] == 0x20ac && name[1] == 0);
			#else
				CHECK_STRING(name, "\xe2\x82\xac");
			#endif
			}
		}
	}
}
#endif

TEST(document_alignment)
{
	char buf[256 + sizeof(xml_document)];

	for (size_t offset = 0; offset < 256; offset += sizeof(void*))
	{
		xml_document* doc = new (buf + offset) xml_document;

		CHECK(doc->load_string(STR("<node />")));
		CHECK_NODE(*doc, STR("<node/>"));

		doc->~xml_document();
	}
}

TEST(document_convert_out_of_memory)
{
	file_data_t files[] =
	{
		{"tests/data/utftest_utf16_be_clean.xml", encoding_utf16_be, 0, 0},
		{"tests/data/utftest_utf16_le_clean.xml", encoding_utf16_le, 0, 0},
		{"tests/data/utftest_utf32_be_clean.xml", encoding_utf32_be, 0, 0},
		{"tests/data/utftest_utf32_le_clean.xml", encoding_utf32_le, 0, 0},
		{"tests/data/utftest_utf8_clean.xml", encoding_utf8, 0, 0},
		{"tests/data/latintest_latin1.xml", encoding_latin1, 0, 0}
	};

	// load files in memory
	for (unsigned int i = 0; i < sizeof(files) / sizeof(files[0]); ++i)
	{
		CHECK(load_file_in_memory(files[i].path, files[i].data, files[i].size));
	}

	// disallow allocations
	test_runner::_memory_fail_threshold = 1;

	for (unsigned int src = 0; src < sizeof(files) / sizeof(files[0]); ++src)
	{
		xml_document doc;
		CHECK_ALLOC_FAIL(CHECK(doc.load_buffer(files[src].data, files[src].size, parse_default, files[src].encoding).status == status_out_of_memory));
	}

	// cleanup
	for (unsigned int j = 0; j < sizeof(files) / sizeof(files[0]); ++j)
	{
		delete[] files[j].data;
	}
}

#ifdef PUGIXML_HAS_MOVE
TEST_XML(document_move_ctor, "<node1/><node2/>")
{
	xml_document other = std::move(doc);

	CHECK(doc.first_child().empty());

	CHECK_STRING(other.first_child().name(), STR("node1"));
	CHECK(other.first_child().parent() == other);

	CHECK_STRING(other.last_child().name(), STR("node2"));
	CHECK(other.last_child().parent() == other);
}

TEST_XML(document_move_assign, "<node1/><node2/>")
{
	xml_document other;
	CHECK(other.load_string(STR("<node3/>")));

	other = std::move(doc);

	CHECK(doc.first_child().empty());

	CHECK_STRING(other.first_child().name(), STR("node1"));
	CHECK(other.first_child().parent() == other);

	CHECK_STRING(other.last_child().name(), STR("node2"));
	CHECK(other.last_child().parent() == other);
}

TEST_XML(document_move_zero_alloc, "<node1/><node2/>")
{
	test_runner::_memory_fail_threshold = 1;

	xml_document other = std::move(doc);

	CHECK(doc.first_child().empty());

	CHECK_STRING(other.first_child().name(), STR("node1"));
	CHECK(other.first_child().parent() == other);

	CHECK_STRING(other.last_child().name(), STR("node2"));
	CHECK(other.last_child().parent() == other);
}

TEST(document_move_append_buffer)
{
	xml_document* doc = new xml_document();
	CHECK(doc->load_string(STR("<node1 attr1='value1'><node2/></node1>")));
	CHECK(doc->child(STR("node1")).append_buffer("<node3/>", 8));
	CHECK(doc->child(STR("node1")).append_buffer("<node4/>", 8));

	xml_document other = std::move(*doc);
	delete doc;

	CHECK(other.child(STR("node1")).append_buffer("<node5/>", 8));
	CHECK(other.child(STR("node1")).append_buffer("<node6/>", 8));

	CHECK_NODE(other, STR("<node1 attr1=\"value1\"><node2/><node3/><node4/><node5/><node6/></node1>"));
}

TEST(document_move_append_child)
{
	xml_document* doc = new xml_document();
	CHECK(doc->load_string(STR("<node1 attr1='value1'><node2/></node1>")));

	xml_document other = std::move(*doc);
	delete doc;

	for (int i = 0; i < 3000; ++i)
		other.child(STR("node1")).append_child(STR("node"));

	for (int i = 0; i < 3000; ++i)
		other.child(STR("node1")).remove_child(other.child(STR("node1")).last_child());

	CHECK_NODE(other, STR("<node1 attr1=\"value1\"><node2/></node1>"));

	other.remove_child(other.first_child());

	CHECK(!other.first_child());
}

TEST(document_move_empty)
{
	xml_document* doc = new xml_document();
	xml_document other = std::move(*doc);
	delete doc;
}

TEST(document_move_large)
{
	xml_document* doc = new xml_document();

	xml_node dn = doc->append_child(STR("node"));

	for (int i = 0; i < 3000; ++i)
		dn.append_child(STR("child"));

	xml_document other = std::move(*doc);
	delete doc;

	xml_node on = other.child(STR("node"));

	for (int i = 0; i < 3000; ++i)
		CHECK(on.remove_child(on.first_child()));

	CHECK(!on.first_child());
}

TEST_XML(document_move_buffer, "<node1/><node2/>")
{
	CHECK(doc.child(STR("node2")).offset_debug() == 9);

	xml_document other = std::move(doc);

	CHECK(other.child(STR("node2")).offset_debug() == 9);
}

TEST_XML(document_move_append_child_zero_alloc, "<node1/><node2/>")
{
	test_runner::_memory_fail_threshold = 1;

	xml_document other = std::move(doc);

	CHECK(other.append_child(STR("node3")));

	CHECK_NODE(other, STR("<node1/><node2/><node3/>"));
}

TEST(document_move_empty_zero_alloc)
{
	xml_document* docs = new xml_document[32];

	test_runner::_memory_fail_threshold = 1;

	for (int i = 1; i < 32; ++i)
		docs[i] = std::move(docs[i-1]);

	delete[] docs;
}

#ifndef PUGIXML_COMPACT
TEST(document_move_repeated_zero_alloc)
{
	xml_document docs[32];

	CHECK(docs[0].load_string(STR("<node><child/></node>")));

	test_runner::_memory_fail_threshold = 1;

	for (int i = 1; i < 32; ++i)
		docs[i] = std::move(docs[i-1]);

	for (int i = 0; i < 31; ++i)
		CHECK(!docs[i].first_child());

	CHECK_NODE(docs[31], STR("<node><child/></node>"));
}
#endif

#ifdef PUGIXML_COMPACT
TEST(document_move_compact_fail)
{
	xml_document docs[32];

	CHECK(docs[0].load_string(STR("<node><child/></node>")));

	test_runner::_memory_fail_threshold = 1;

	int safe_count = 21;

	for (int i = 1; i <= safe_count; ++i)
		docs[i] = std::move(docs[i-1]);

	CHECK_ALLOC_FAIL(docs[safe_count+1] = std::move(docs[safe_count]));

	for (int i = 0; i < safe_count; ++i)
		CHECK(!docs[i].first_child());

	CHECK_NODE(docs[safe_count], STR("<node><child/></node>"));
	CHECK(!docs[safe_count+1].first_child());
}
#endif
#endif
