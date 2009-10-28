#include "common.hpp"

#include <fstream>

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

TEST(document_create)
{
	pugi::xml_document doc;
	doc.append_child().set_name("node");
	CHECK_NODE(doc, "<node />");
}

TEST(document_load_stream)
{
	pugi::xml_document doc;

	std::istringstream iss("<node/>");
	CHECK(doc.load(iss));
	CHECK_NODE(doc, "<node />");
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

	std::ifstream fs3("nul");
	CHECK(doc.load(fs3).status == status_io_error);

	test_runner::_memory_fail_threshold = 1;
	std::istringstream iss("<node/>");
	CHECK(doc.load(iss).status == status_out_of_memory);
}

TEST(document_load_string)
{
	pugi::xml_document doc;

	CHECK(doc.load("<node/>"));
	CHECK_NODE(doc, "<node />");
}

TEST(document_load_file)
{
	pugi::xml_document doc;

	CHECK(doc.load_file("tests/data/small.xml"));
	CHECK_NODE(doc, "<node />");
}

TEST(document_load_file_large)
{
	pugi::xml_document doc;

	CHECK(doc.load_file("tests/data/large.xml"));

	std::ostringstream oss;
	oss << "<node>";
	for (int i = 0; i < 10000; ++i) oss << "<node />";
	oss << "</node>";

	CHECK_NODE(doc, oss.str().c_str());
}

TEST(document_load_file_error)
{
	pugi::xml_document doc;

	CHECK(doc.load_file("filedoesnotexist").status == status_file_not_found);

#ifndef __DMC__ // Digital Mars CRT does not like 'con' pseudo-file
	CHECK(doc.load_file("con").status == status_io_error);
#endif

	CHECK(doc.load_file("nul").status == status_io_error);

	test_runner::_memory_fail_threshold = 1;
	CHECK(doc.load_file("tests/data/small.xml").status == status_out_of_memory);
}

TEST_XML(document_save, "<node/>")
{
	std::ostringstream oss;
	xml_writer_stream writer(oss);

	doc.save(writer, "", pugi::format_no_declaration | pugi::format_raw);

	CHECK(oss.str() == "<node />");
}

TEST_XML(document_save_bom, "<node/>")
{
	std::ostringstream oss;
	xml_writer_stream writer(oss);

	doc.save(writer, "", pugi::format_no_declaration | pugi::format_raw | pugi::format_write_bom_utf8);

	CHECK(oss.str() == "\xef\xbb\xbf<node />");
}

TEST_XML(document_save_declaration, "<node/>")
{
	std::ostringstream oss;
	xml_writer_stream writer(oss);

	doc.save(writer);

	CHECK(oss.str() == "<?xml version=\"1.0\"?>\n<node />\n");
}

TEST_XML(document_save_file, "<node/>")
{
	CHECK(doc.save_file("tests/data/output.xml"));

	CHECK(doc.load_file("tests/data/output.xml", pugi::parse_default | pugi::parse_declaration));
	CHECK_NODE(doc, "<?xml version=\"1.0\"?><node />");

	unlink("tests/data/output.xml");
}

TEST(document_parse)
{
	char text[] = "<node/>";

	pugi::xml_document doc;

	CHECK(doc.parse(text));
	CHECK_NODE(doc, "<node />");
}

TEST(document_parse_transfer_ownership)
{
	allocation_function alloc = get_memory_allocation_function();

	char* text = static_cast<char*>(alloc(strlen("<node/>") + 1));
	CHECK(text);

	strcpy(text, "<node/>");

	pugi::xml_document doc;

	CHECK(doc.parse(transfer_ownership_tag(), text));
	CHECK_NODE(doc, "<node />");
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
