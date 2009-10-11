#include "common.hpp"

TEST_XML(dom_attr_assign, "<node attr1='' attr2='' attr3='' attr4='' attr5=''/>")
{
	xml_node node = doc.child("node");

	node.attribute("attr1") = "v1";
	xml_attribute() = "v1";

	node.attribute("attr2") = -2147483647 - 1;
	xml_attribute() = -2147483647 - 1;

	node.attribute("attr3") = 2147483647u;
	xml_attribute() = 2147483647;

	node.attribute("attr4") = 0.5;
	xml_attribute() = 0.5;

	node.attribute("attr5") = true;
	xml_attribute() = true;

	CHECK_NODE(node, "<node attr1=\"v1\" attr2=\"-2147483648\" attr3=\"2147483647\" attr4=\"0.5\" attr5=\"true\" />");
}

TEST_XML(dom_attr_set_value, "<node attr1='' attr2='' attr3='' attr4='' attr5=''/>")
{
	xml_node node = doc.child("node");

	CHECK(node.attribute("attr1").set_value("v1"));
	CHECK(!xml_attribute().set_value("v1"));

	CHECK(node.attribute("attr2").set_value(-2147483647 - 1));
	CHECK(!xml_attribute().set_value(-2147483647 - 1));

	CHECK(node.attribute("attr3").set_value(2147483647u));
	CHECK(!xml_attribute().set_value(2147483647));

	CHECK(node.attribute("attr4").set_value(0.5));
	CHECK(!xml_attribute().set_value(0.5));

	CHECK(node.attribute("attr5").set_value(true));
	CHECK(!xml_attribute().set_value(true));

	CHECK_NODE(node, "<node attr1=\"v1\" attr2=\"-2147483648\" attr3=\"2147483647\" attr4=\"0.5\" attr5=\"true\" />");
}

TEST_XML(dom_node_set_name, "<node>text</node>")
{
	CHECK(doc.child("node").set_name("n"));
	CHECK(!doc.child("node").first_child().set_name("n"));
	CHECK(!xml_node().set_name("n"));

	CHECK_NODE(doc, "<n>text</n>");
}

TEST_XML(dom_node_set_value, "<node>text</node>")
{
	CHECK(doc.child("node").first_child().set_value("no text"));
	CHECK(!doc.child("node").set_value("no text"));
	CHECK(!xml_node().set_value("no text"));

	CHECK_NODE(doc, "<node>no text</node>");
}
