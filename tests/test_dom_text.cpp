#include "common.hpp"

#include "helpers.hpp"

TEST_XML_FLAGS(dom_text_empty, "<node><a>foo</a><b><![CDATA[bar]]></b><c><?pi value?></c><d/></node>", parse_default | parse_pi)
{
    xml_node node = doc.child(STR("node"));

    CHECK(node.child(STR("a")).text());
    CHECK(node.child(STR("b")).text());
    CHECK(!node.child(STR("c")).text());
    CHECK(!node.child(STR("d")).text());
    CHECK(!xml_node().text());
    CHECK(!xml_text());

    generic_empty_test(node.child(STR("a")).text());
}

TEST_XML(dom_text_bool_ops, "<node>foo</node>")
{
    generic_bool_ops_test(doc.child(STR("node")).text());
}

TEST_XML_FLAGS(dom_text_get, "<node><a>foo</a><b><![CDATA[bar]]></b><c><?pi value?></c><d/></node>", parse_default | parse_pi)
{
    xml_node node = doc.child(STR("node"));

    CHECK_STRING(node.child(STR("a")).text().get(), STR("foo"));
    CHECK_STRING(node.child(STR("a")).first_child().text().get(), STR("foo"));

    CHECK_STRING(node.child(STR("b")).text().get(), STR("bar"));
    CHECK_STRING(node.child(STR("b")).first_child().text().get(), STR("bar"));

    CHECK_STRING(node.child(STR("c")).text().get(), STR(""));
    CHECK_STRING(node.child(STR("c")).first_child().text().get(), STR(""));

    CHECK_STRING(node.child(STR("d")).text().get(), STR(""));

    CHECK_STRING(xml_node().text().get(), STR(""));
}

TEST_XML(dom_text_as_int, "<node><text1>1</text1><text2>-1</text2><text3>-2147483648</text3><text4>2147483647</text4></node>")
{
	xml_node node = doc.child(STR("node"));

	CHECK(xml_text().as_int() == 0);
	CHECK(node.child(STR("text1")).text().as_int() == 1);
	CHECK(node.child(STR("text2")).text().as_int() == -1);
	CHECK(node.child(STR("text3")).text().as_int() == -2147483647 - 1);
	CHECK(node.child(STR("text4")).text().as_int() == 2147483647);
}

TEST_XML(dom_text_as_uint, "<node><text1>0</text1><text2>1</text2><text3>2147483647</text3><text4>4294967295</text4></node>")
{
	xml_node node = doc.child(STR("node"));

	CHECK(xml_text().as_uint() == 0);
	CHECK(node.child(STR("text1")).text().as_uint() == 0);
	CHECK(node.child(STR("text2")).text().as_uint() == 1);
	CHECK(node.child(STR("text3")).text().as_uint() == 2147483647);
	CHECK(node.child(STR("text4")).text().as_uint() == 4294967295u);
}

TEST_XML(dom_text_as_float, "<node><text1>0</text1><text2>1</text2><text3>0.12</text3><text4>-5.1</text4><text5>3e-4</text5><text6>3.14159265358979323846</text6></node>")
{
	xml_node node = doc.child(STR("node"));

	CHECK(xml_text().as_float() == 0);
	CHECK_DOUBLE(node.child(STR("text1")).text().as_float(), 0);
	CHECK_DOUBLE(node.child(STR("text2")).text().as_float(), 1);
	CHECK_DOUBLE(node.child(STR("text3")).text().as_float(), 0.12);
	CHECK_DOUBLE(node.child(STR("text4")).text().as_float(), -5.1);
	CHECK_DOUBLE(node.child(STR("text5")).text().as_float(), 3e-4);
	CHECK_DOUBLE(node.child(STR("text6")).text().as_float(), 3.14159265358979323846);
}

TEST_XML(dom_text_as_double, "<node><text1>0</text1><text2>1</text2><text3>0.12</text3><text4>-5.1</text4><text5>3e-4</text5><text6>3.14159265358979323846</text6></node>")
{
	xml_node node = doc.child(STR("node"));

	CHECK(xml_text().as_double() == 0);
	CHECK_DOUBLE(node.child(STR("text1")).text().as_double(), 0);
	CHECK_DOUBLE(node.child(STR("text2")).text().as_double(), 1);
	CHECK_DOUBLE(node.child(STR("text3")).text().as_double(), 0.12);
	CHECK_DOUBLE(node.child(STR("text4")).text().as_double(), -5.1);
	CHECK_DOUBLE(node.child(STR("text5")).text().as_double(), 3e-4);
	CHECK_DOUBLE(node.child(STR("text6")).text().as_double(), 3.14159265358979323846);
}

TEST_XML(dom_text_as_bool, "<node><text1>0</text1><text2>1</text2><text3>true</text3><text4>True</text4><text5>Yes</text5><text6>yes</text6><text7>false</text7></node>")
{
	xml_node node = doc.child(STR("node"));

	CHECK(!xml_text().as_bool());
	CHECK(!node.child(STR("text1")).text().as_bool());
	CHECK(node.child(STR("text2")).text().as_bool());
	CHECK(node.child(STR("text3")).text().as_bool());
	CHECK(node.child(STR("text4")).text().as_bool());
	CHECK(node.child(STR("text5")).text().as_bool());
	CHECK(node.child(STR("text6")).text().as_bool());
	CHECK(!node.child(STR("text7")).text().as_bool());
}
