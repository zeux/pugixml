// This file includes all tests for deprecated functionality; this is going away in the next release!

#ifdef _MSC_VER
#	pragma warning(disable: 4996)
#endif

#ifdef __GNUC__
#	if __GNUC__ >= 4 && __GNUC_MINOR__ >= 2
#		pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#	else
#		define PUGIXML_DEPRECATED
#	endif
#endif


#include <string.h>

#include "common.hpp"

#include "writer_string.hpp"

#include <vector>
#include <iterator>

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

// wildcard functions
TEST_XML(dom_node_child_w, "<node><child1/><child2/></node>")
{
	CHECK(doc.child_w(STR("n?de")) == doc.child(STR("node")));
	CHECK(doc.child_w(STR("n[az]de")) == xml_node());
	CHECK(doc.child_w(STR("n[aoz]de")) == doc.child(STR("node")));
	CHECK(doc.child_w(STR("*e")) == doc.child(STR("node")));
	CHECK(doc.child(STR("node")).child_w(STR("*l?[23456789]*")) == doc.child(STR("node")).child(STR("child2")));
}

TEST_XML(dom_node_attribute_w, "<node attr1='0' attr2='1'/>")
{
	xml_node node = doc.child(STR("node"));

	CHECK(node.attribute_w(STR("*tt?[23456789]*")) == node.attribute(STR("attr2")));
	CHECK(node.attribute_w(STR("?")) == xml_attribute());
}

TEST_XML(dom_node_next_previous_sibling_w, "<node><child1/><child2/><child3/></node>")
{
	CHECK(xml_node().next_sibling_w(STR("n")) == xml_node());
	CHECK(xml_node().previous_sibling_w(STR("n")) == xml_node());

	xml_node child1 = doc.child(STR("node")).child(STR("child1"));
	xml_node child3 = doc.child(STR("node")).child(STR("child3"));

	CHECK(child1.next_sibling_w(STR("*[3456789]")) == child3);
	CHECK(child1.next_sibling_w(STR("?")) == xml_node());
	CHECK(child3.previous_sibling_w(STR("*[3456789]")) == xml_node());
	CHECK(child3.previous_sibling_w(STR("?")) == xml_node());
	CHECK(child3.previous_sibling_w(STR("*1")) == child1);
}

TEST_XML(dom_node_child_value_w, "<node><novalue/><child1>value1</child1><child2>value2<n/></child2><child3><![CDATA[value3]]></child3>value4</node>")
{
	CHECK_STRING(xml_node().child_value_w(STR("n")), STR(""));

	xml_node node = doc.child(STR("node"));

	CHECK_STRING(node.child_value_w(STR("c*[23456789]")), STR("value2"));
	CHECK_STRING(node.child_value_w(STR("*")), STR("")); // child_value(name) and child_value_w(pattern) do not continue the search if a node w/out value is found first
	CHECK_STRING(node.child_value_w(STR("nothing*here")), STR(""));
}

TEST_XML(dom_node_find_child_by_attribute_w, "<node><child1 attr='value1'/><child2 attr='value2'/><child2 attr='value3'/></node>")
{
	CHECK(xml_node().find_child_by_attribute_w(STR("name"), STR("attr"), STR("value")) == xml_node());
	CHECK(xml_node().find_child_by_attribute_w(STR("attr"), STR("value")) == xml_node());

	xml_node node = doc.child(STR("node"));

	CHECK(node.find_child_by_attribute_w(STR("*"), STR("att?"), STR("val*[0123456789]")) == node.child(STR("child1")));
	CHECK(node.find_child_by_attribute_w(STR("*"), STR("attr3"), STR("val*[0123456789]")) == xml_node());
	CHECK(node.find_child_by_attribute_w(STR("att?"), STR("val*[0123456789]")) == node.child(STR("child1")));
	CHECK(node.find_child_by_attribute_w(STR("attr3"), STR("val*[0123456789]")) == xml_node());
}

TEST_XML(dom_node_all_elements_by_name_w, "<node><child><child/><child/></child></node>")
{
	std::vector<xml_node> v;

	v.clear();
	xml_node().all_elements_by_name_w(STR("*"), std::back_inserter(v));
	CHECK(v.empty());

	v.clear();
	doc.all_elements_by_name_w(STR("*"), std::back_inserter(v));
	CHECK(v.size() == 4);
	CHECK(v[0] == doc.child(STR("node")));
	CHECK(v[1] == doc.child(STR("node")).child(STR("child")));
	CHECK(v[2] == doc.child(STR("node")).child(STR("child")).first_child());
	CHECK(v[3] == doc.child(STR("node")).child(STR("child")).last_child());
}

TEST_XML(dom_node_wildcard_cset, "<node c='1'/>")
{
	xml_node node = doc.child(STR("node"));

	CHECK(node.attribute_w(STR("[A-Z]")).as_int() == 0);
	CHECK(node.attribute_w(STR("[a-z]")).as_int() == 1);
	CHECK(node.attribute_w(STR("[A-z]")).as_int() == 1);
	CHECK(node.attribute_w(STR("[z-a]")).as_int() == 0);
	CHECK(node.attribute_w(STR("[a-zA-Z]")).as_int() == 1);
	CHECK(node.attribute_w(STR("[!A-Z]")).as_int() == 1);
	CHECK(node.attribute_w(STR("[!A-Za-z]")).as_int() == 0);
}

TEST_XML(dom_node_wildcard_star, "<node cd='1'/>")
{
	xml_node node = doc.child(STR("node"));

	CHECK(node.attribute_w(STR("*")).as_int() == 1);
	CHECK(node.attribute_w(STR("?d*")).as_int() == 1);
	CHECK(node.attribute_w(STR("?c*")).as_int() == 0);
	CHECK(node.attribute_w(STR("*?*c*")).as_int() == 0);
	CHECK(node.attribute_w(STR("*?*d*")).as_int() == 1);
}

// parse_wnorm_attribute flag
TEST(parse_attribute_wnorm)
{
	xml_document doc;

	for (int eol = 0; eol < 2; ++eol)
		for (int wconv = 0; wconv < 2; ++wconv)
		{
			unsigned int flags = parse_minimal | parse_wnorm_attribute | (eol ? parse_eol : 0) | (wconv ? parse_wconv_attribute : 0);
			CHECK(doc.load(STR("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>"), flags));
			CHECK_STRING(doc.child(STR("node")).attribute(STR("id")).value(), STR("val1 val2 val3 val4"));
		}
}

TEST(parse_attribute_variations_wnorm)
{
	xml_document doc;

	for (int wnorm = 0; wnorm < 2; ++wnorm)
		for (int eol = 0; eol < 2; ++eol)
			for (int wconv = 0; wconv < 2; ++wconv)
				for (int escapes = 0; escapes < 2; ++escapes)
				{
					unsigned int flags = parse_minimal;
					
					 flags |= (wnorm ? parse_wnorm_attribute : 0);
					 flags |= (eol ? parse_eol : 0);
					 flags |= (wconv ? parse_wconv_attribute : 0);
					 flags |= (escapes ? parse_escapes : 0);

					CHECK(doc.load(STR("<node id='1'/>"), flags));
					CHECK_STRING(doc.child(STR("node")).attribute(STR("id")).value(), STR("1"));
				}
}

// document order
TEST_XML(document_order_coverage, "<node id='1'/>")
{
	xml_document doc;
	doc.precompute_document_order();

	CHECK(doc.child(STR("node")).document_order() == 0);
	CHECK(doc.child(STR("node")).attribute(STR("id")).document_order() == 0);
}
