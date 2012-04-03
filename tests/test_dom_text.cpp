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

TEST_XML_FLAGS(dom_text_get, "<node><a>foo</a><b><node/><![CDATA[bar]]></b><c><?pi value?></c><d/></node>", parse_default | parse_pi)
{
    xml_node node = doc.child(STR("node"));

    CHECK_STRING(node.child(STR("a")).text().get(), STR("foo"));
    CHECK_STRING(node.child(STR("a")).first_child().text().get(), STR("foo"));

    CHECK_STRING(node.child(STR("b")).text().get(), STR("bar"));
    CHECK_STRING(node.child(STR("b")).last_child().text().get(), STR("bar"));

    CHECK_STRING(node.child(STR("c")).text().get(), STR(""));
    CHECK_STRING(node.child(STR("c")).first_child().text().get(), STR(""));

    CHECK_STRING(node.child(STR("d")).text().get(), STR(""));

    CHECK_STRING(xml_node().text().get(), STR(""));
}

TEST_XML_FLAGS(dom_text_as_string, "<node><a>foo</a><b><node/><![CDATA[bar]]></b><c><?pi value?></c><d/></node>", parse_default | parse_pi)
{
    xml_node node = doc.child(STR("node"));

    CHECK_STRING(node.child(STR("a")).text().as_string(), STR("foo"));
    CHECK_STRING(node.child(STR("a")).first_child().text().as_string(), STR("foo"));

    CHECK_STRING(node.child(STR("b")).text().as_string(), STR("bar"));
    CHECK_STRING(node.child(STR("b")).last_child().text().as_string(), STR("bar"));

    CHECK_STRING(node.child(STR("c")).text().as_string(), STR(""));
    CHECK_STRING(node.child(STR("c")).first_child().text().as_string(), STR(""));

    CHECK_STRING(node.child(STR("d")).text().as_string(), STR(""));

    CHECK_STRING(xml_node().text().as_string(), STR(""));
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

TEST_XML(dom_text_get_no_state, "<node/>")
{
    xml_node node = doc.child(STR("node"));
    xml_text t = node.text();

    CHECK(!t);
    CHECK(t.get() && *t.get() == 0);
    CHECK(!node.first_child());

    node.append_child(node_pcdata);

    CHECK(t);
    CHECK_STRING(t.get(), STR(""));

    node.first_child().set_value(STR("test"));

    CHECK(t);
    CHECK_STRING(t.get(), STR("test"));
}

TEST_XML(dom_text_set, "<node/>")
{
    xml_node node = doc.child(STR("node"));
    xml_text t = node.text();

    t.set(STR(""));
    CHECK(node.first_child().type() == node_pcdata);
    CHECK_NODE(node, STR("<node></node>"));

    t.set(STR("boo"));
    CHECK(node.first_child().type() == node_pcdata);
    CHECK(node.first_child() == node.last_child());
    CHECK_NODE(node, STR("<node>boo</node>"));

    t.set(STR("foobarfoobar"));
    CHECK(node.first_child().type() == node_pcdata);
    CHECK(node.first_child() == node.last_child());
    CHECK_NODE(node, STR("<node>foobarfoobar</node>"));
}

TEST_XML(dom_text_assign, "<node/>")
{
	xml_node node = doc.child(STR("node"));

	node.append_child(STR("text1")).text() = STR("v1");
	xml_text() = STR("v1");

	node.append_child(STR("text2")).text() = -2147483647;
	node.append_child(STR("text3")).text() = -2147483647 - 1;
	xml_text() = -2147483647 - 1;

	node.append_child(STR("text4")).text() = 4294967295u;
	node.append_child(STR("text5")).text() = 4294967294u;
	xml_text() = 2147483647;

	node.append_child(STR("text6")).text() = 0.5;
	xml_text() = 0.5;

	node.append_child(STR("text7")).text() = true;
	xml_text() = true;

	CHECK_NODE(node, STR("<node><text1>v1</text1><text2>-2147483647</text2><text3>-2147483648</text3><text4>4294967295</text4><text5>4294967294</text5><text6>0.5</text6><text7>true</text7></node>"));
}

TEST_XML(dom_text_set_value, "<node/>")
{
	xml_node node = doc.child(STR("node"));

	CHECK(node.append_child(STR("text1")).text().set(STR("v1")));
	CHECK(!xml_text().set(STR("v1")));

	CHECK(node.append_child(STR("text2")).text().set(-2147483647));
	CHECK(node.append_child(STR("text3")).text().set(-2147483647 - 1));
	CHECK(!xml_text().set(-2147483647 - 1));

	CHECK(node.append_child(STR("text4")).text().set(4294967295u));
	CHECK(node.append_child(STR("text5")).text().set(4294967294u));
	CHECK(!xml_text().set(2147483647));

	CHECK(node.append_child(STR("text6")).text().set(0.5));
	CHECK(!xml_text().set(0.5));

	CHECK(node.append_child(STR("text7")).text().set(true));
	CHECK(!xml_text().set(true));

	CHECK_NODE(node, STR("<node><text1>v1</text1><text2>-2147483647</text2><text3>-2147483648</text3><text4>4294967295</text4><text5>4294967294</text5><text6>0.5</text6><text7>true</text7></node>"));
}

TEST_XML(dom_text_middle, "<node><c1>notthisone</c1>text<c2/></node>")
{
    xml_node node = doc.child(STR("node"));
    xml_text t = node.text();

    CHECK_STRING(t.get(), STR("text"));
    t.set(STR("notext"));

    CHECK_NODE(node, STR("<node><c1>notthisone</c1>notext<c2 /></node>"));
    CHECK(node.remove_child(t.data()));

    CHECK(!t);
    CHECK_NODE(node, STR("<node><c1>notthisone</c1><c2 /></node>"));

    t.set(STR("yestext"));

    CHECK(t);
    CHECK_NODE(node, STR("<node><c1>notthisone</c1><c2 />yestext</node>"));
    CHECK(t.data() == node.last_child());
}

TEST_XML_FLAGS(dom_text_data, "<node><a>foo</a><b><![CDATA[bar]]></b><c><?pi value?></c><d/></node>", parse_default | parse_pi)
{
    xml_node node = doc.child(STR("node"));

    CHECK(node.child(STR("a")).text().data() == node.child(STR("a")).first_child());
    CHECK(node.child(STR("b")).text().data() == node.child(STR("b")).first_child());
    CHECK(!node.child(STR("c")).text().data());
    CHECK(!node.child(STR("d")).text().data());
    CHECK(!xml_text().data());
}

TEST(dom_text_defaults)
{
    xml_text text;

    CHECK_STRING(text.as_string(STR("foo")), STR("foo"));
    CHECK(text.as_int(42) == 42);
    CHECK(text.as_uint(42) == 42);
    CHECK(text.as_double(42) == 42);
    CHECK(text.as_float(42) == 42);
    CHECK(text.as_bool(true) == true);
}
