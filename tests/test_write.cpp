#include "common.hpp"

#include <string>

TEST_XML(write_simple, "<node attr='1'><child>text</child></node>")
{
	CHECK_NODE_EX(doc, "<node attr=\"1\">\n<child>text</child>\n</node>\n", "", 0);
}

TEST_XML(write_raw, "<node attr='1'><child>text</child></node>")
{
	CHECK_NODE_EX(doc, "<node attr=\"1\"><child>text</child></node>", "", pugi::format_raw);
}

TEST_XML(write_indent, "<node attr='1'><child><sub>text</sub></child></node>")
{
	CHECK_NODE_EX(doc, "<node attr=\"1\">\n\t<child>\n\t\t<sub>text</sub>\n\t</child>\n</node>\n", "\t", pugi::format_indent);
}

TEST_XML(write_pcdata, "<node attr='1'><child><sub/>text</child></node>")
{
	CHECK_NODE_EX(doc, "<node attr=\"1\">\n\t<child>\n\t\t<sub />\n\t\ttext\n\t</child>\n</node>\n", "\t", pugi::format_indent);
}

TEST_XML(write_cdata, "<![CDATA[value]]>")
{
	CHECK_NODE(doc, "<![CDATA[value]]>");
}

TEST_XML_FLAGS(write_comment, "<!--text-->", pugi::parse_default | pugi::parse_comments)
{
	CHECK_NODE(doc, "<!--text-->");
}

TEST_XML_FLAGS(write_pi, "<?name value?>", pugi::parse_default | pugi::parse_pi)
{
	CHECK_NODE(doc, "<?name value?>");
}

TEST_XML_FLAGS(write_declaration, "<?xml version='2.0'?>", pugi::parse_default | pugi::parse_declaration)
{
	CHECK_NODE(doc, "<?xml version=\"2.0\"?>");
}

TEST_XML(write_escape, "<node attr=''>text</node>")
{
	doc.child("node").attribute("attr") = "<>'\"&\x04\r\n\t";
	doc.child("node").first_child().set_value("<>'\"&\x04\r\n\t");

	CHECK_NODE(doc, "<node attr=\"&lt;&gt;'&quot;&amp;&#4;&#13;&#10;\t\">&lt;&gt;'\"&amp;&#4;\r\n\t</node>");
}

struct test_writer: xml_writer
{
	std::string contents;

	virtual void write(const void* data, size_t size)
	{
		contents += std::string(static_cast<const char*>(data), static_cast<const char*>(data) + size);
	}
};

TEST_XML(write_print_writer, "<node/>")
{
	test_writer writer;
	doc.print(writer);

	CHECK(writer.contents == "<node />\n");
}

TEST_XML(write_print_stream, "<node/>")
{
	std::ostringstream oss;
	doc.print(oss);

	CHECK(oss.str() == "<node />\n");
}
