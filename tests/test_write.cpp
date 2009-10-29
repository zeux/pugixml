#include "common.hpp"

#include <string>
#include <sstream>

TEST_XML(write_simple, "<node attr='1'><child>text</child></node>")
{
	CHECK_NODE_EX(doc, "<node attr=\"1\">\n<child>text</child>\n</node>\n", "", 0);
}

TEST_XML(write_raw, "<node attr='1'><child>text</child></node>")
{
	CHECK_NODE_EX(doc, "<node attr=\"1\"><child>text</child></node>", "", format_raw);
}

TEST_XML(write_indent, "<node attr='1'><child><sub>text</sub></child></node>")
{
	CHECK_NODE_EX(doc, "<node attr=\"1\">\n\t<child>\n\t\t<sub>text</sub>\n\t</child>\n</node>\n", "\t", format_indent);
}

TEST_XML(write_pcdata, "<node attr='1'><child><sub/>text</child></node>")
{
	CHECK_NODE_EX(doc, "<node attr=\"1\">\n\t<child>\n\t\t<sub />\n\t\ttext\n\t</child>\n</node>\n", "\t", format_indent);
}

TEST_XML(write_cdata, "<![CDATA[value]]>")
{
	CHECK_NODE(doc, "<![CDATA[value]]>");
	CHECK_NODE_EX(doc, "<![CDATA[value]]>\n", "", 0);
}

TEST_XML_FLAGS(write_comment, "<!--text-->", parse_default | parse_comments)
{
	CHECK_NODE(doc, "<!--text-->");
	CHECK_NODE_EX(doc, "<!--text-->\n", "", 0);
}

TEST_XML_FLAGS(write_pi, "<?name value?>", parse_default | parse_pi)
{
	CHECK_NODE(doc, "<?name value?>");
	CHECK_NODE_EX(doc, "<?name value?>\n", "", 0);
}

TEST_XML_FLAGS(write_declaration, "<?xml version='2.0'?>", parse_default | parse_declaration)
{
	CHECK_NODE(doc, "<?xml version=\"2.0\"?>");
	CHECK_NODE_EX(doc, "<?xml version=\"2.0\"?>\n", "", 0);
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

#ifndef PUGIXML_NO_STL
TEST_XML(write_print_stream, "<node/>")
{
	std::ostringstream oss;
	doc.print(oss);

	CHECK(oss.str() == "<node />\n");
}
#endif

TEST_XML(write_huge_chunk, "<node/>")
{
	std::string name(10000, 'n');
	doc.child("node").set_name(name.c_str());

	test_writer writer;
	doc.print(writer);

	CHECK(writer.contents == "<" + name + " />\n");
}
