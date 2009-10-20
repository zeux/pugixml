#include "common.hpp"

#include <fstream>

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

	std::ifstream fs1("");
	CHECK(doc.load(fs1).status == status_io_error);
	
	std::ifstream fs2("con");
	CHECK(doc.load(fs2).status == status_io_error);
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

	CHECK(doc.load_file("").status == status_file_not_found);
	CHECK(doc.load_file("con").status == status_io_error);
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
	char* text = static_cast<char*>(malloc(strlen("<node/>") + 1));
	CHECK(text);

	strcpy(text, "<node/>");

	pugi::xml_document doc;

	CHECK(doc.parse(transfer_ownership_tag(), text));
	CHECK_NODE(doc, "<node />");
}
