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

TEST_XML(dom_node_append_attribute, "<node><child/></node>")
{
	CHECK(xml_node().append_attribute("a") == xml_attribute());
	CHECK(doc.append_attribute("a") == xml_attribute());
	
	xml_attribute a1 = doc.child("node").append_attribute("a1");
	CHECK(a1);
	a1 = "v1";

	xml_attribute a2 = doc.child("node").append_attribute("a2");
	CHECK(a2 && a1 != a2);
	a2 = "v2";

	xml_attribute a3 = doc.child("node").child("child").append_attribute("a3");
	CHECK(a3 && a1 != a3 && a2 != a3);
	a3 = "v3";

	CHECK_NODE(doc, "<node a1=\"v1\" a2=\"v2\"><child a3=\"v3\" /></node>");
}

TEST_XML(dom_node_insert_attribute_after, "<node a1='v1'><child a2='v2'/></node>")
{
	CHECK(xml_node().insert_attribute_after("a", xml_attribute()) == xml_attribute());

	xml_node node = doc.child("node");
	xml_node child = node.child("child");

	xml_attribute a1 = node.attribute("a1");
	xml_attribute a2 = child.attribute("a2");

	CHECK(node.insert_attribute_after("a", xml_attribute()) == xml_attribute());
	CHECK(node.insert_attribute_after("a", a2) == xml_attribute());
	
	xml_attribute a3 = node.insert_attribute_after("a3", a1);
	CHECK(a3 && a3 != a2 && a3 != a1);
	a3 = "v3";

	xml_attribute a4 = node.insert_attribute_after("a4", a1);
	CHECK(a4 && a4 != a3 && a4 != a2 && a4 != a1);
	a4 = "v4";

	xml_attribute a5 = node.insert_attribute_after("a5", a3);
	CHECK(a5 && a5 != a4 && a5 != a3 && a5 != a2 && a5 != a1);
	a5 = "v5";

	CHECK(child.insert_attribute_after("a", a4) == xml_attribute());

	CHECK_NODE(doc, "<node a1=\"v1\" a4=\"v4\" a3=\"v3\" a5=\"v5\"><child a2=\"v2\" /></node>");
}

TEST_XML(dom_node_insert_attribute_before, "<node a1='v1'><child a2='v2'/></node>")
{
	CHECK(xml_node().insert_attribute_before("a", xml_attribute()) == xml_attribute());

	xml_node node = doc.child("node");
	xml_node child = node.child("child");

	xml_attribute a1 = node.attribute("a1");
	xml_attribute a2 = child.attribute("a2");

	CHECK(node.insert_attribute_before("a", xml_attribute()) == xml_attribute());
	CHECK(node.insert_attribute_before("a", a2) == xml_attribute());
	
	xml_attribute a3 = node.insert_attribute_before("a3", a1);
	CHECK(a3 && a3 != a2 && a3 != a1);
	a3 = "v3";

	xml_attribute a4 = node.insert_attribute_before("a4", a1);
	CHECK(a4 && a4 != a3 && a4 != a2 && a4 != a1);
	a4 = "v4";

	xml_attribute a5 = node.insert_attribute_before("a5", a3);
	CHECK(a5 && a5 != a4 && a5 != a3 && a5 != a2 && a5 != a1);
	a5 = "v5";

	CHECK(child.insert_attribute_before("a", a4) == xml_attribute());

	CHECK_NODE(doc, "<node a5=\"v5\" a3=\"v3\" a4=\"v4\" a1=\"v1\"><child a2=\"v2\" /></node>");
}

TEST_XML(dom_node_append_copy_attribute, "<node a1='v1'><child a2='v2'/><child/></node>")
{
	CHECK(xml_node().append_copy(xml_attribute()) == xml_attribute());
	CHECK(xml_node().append_copy(doc.child("node").attribute("a1")) == xml_attribute());
	CHECK(doc.append_copy(doc.child("node").attribute("a1")) == xml_attribute());
	
	xml_node node = doc.child("node");
	xml_node child = node.child("child");

	xml_attribute a1 = node.attribute("a1");
	xml_attribute a2 = child.attribute("a2");

	xml_attribute a3 = node.append_copy(a1);
	CHECK(a3 && a3 != a2 && a3 != a1);

	xml_attribute a4 = node.append_copy(a2);
	CHECK(a4 && a4 != a3 && a4 != a2 && a4 != a1);

	xml_attribute a5 = node.last_child().append_copy(a1);
	CHECK(a5 && a5 != a4 && a5 != a3 && a5 != a2 && a5 != a1);

	CHECK_NODE(doc, "<node a1=\"v1\" a1=\"v1\" a2=\"v2\"><child a2=\"v2\" /><child a1=\"v1\" /></node>");

	a3.set_name("a3");
	a3 = "v3";
	
	a4.set_name("a4");
	a4 = "v4";
	
	a5.set_name("a5");
	a5 = "v5";
	
	CHECK_NODE(doc, "<node a1=\"v1\" a3=\"v3\" a4=\"v4\"><child a2=\"v2\" /><child a5=\"v5\" /></node>");
}

TEST_XML(dom_node_insert_copy_after_attribute, "<node a1='v1'><child a2='v2'/></node>")
{
	CHECK(xml_node().insert_copy_after(xml_attribute(), xml_attribute()) == xml_attribute());

	xml_node node = doc.child("node");
	xml_node child = node.child("child");

	xml_attribute a1 = node.attribute("a1");
	xml_attribute a2 = child.attribute("a2");

	CHECK(node.insert_copy_after(a1, xml_attribute()) == xml_attribute());
	CHECK(node.insert_copy_after(xml_attribute(), a1) == xml_attribute());
	CHECK(node.insert_copy_after(a2, a2) == xml_attribute());
	
	xml_attribute a3 = node.insert_copy_after(a1, a1);
	CHECK(a3 && a3 != a2 && a3 != a1);

	xml_attribute a4 = node.insert_copy_after(a2, a1);
	CHECK(a4 && a4 != a3 && a4 != a2 && a4 != a1);

	xml_attribute a5 = node.insert_copy_after(a4, a1);
	CHECK(a5 && a5 != a4 && a5 != a3 && a5 != a2 && a5 != a1);

	CHECK(child.insert_copy_after(a4, a4) == xml_attribute());

	CHECK_NODE(doc, "<node a1=\"v1\" a2=\"v2\" a2=\"v2\" a1=\"v1\"><child a2=\"v2\" /></node>");

	a3.set_name("a3");
	a3 = "v3";
	
	a4.set_name("a4");
	a4 = "v4";
	
	a5.set_name("a5");
	a5 = "v5";
	
	CHECK_NODE(doc, "<node a1=\"v1\" a5=\"v5\" a4=\"v4\" a3=\"v3\"><child a2=\"v2\" /></node>");
}

TEST_XML(dom_node_insert_copy_before_attribute, "<node a1='v1'><child a2='v2'/></node>")
{
	CHECK(xml_node().insert_copy_before(xml_attribute(), xml_attribute()) == xml_attribute());

	xml_node node = doc.child("node");
	xml_node child = node.child("child");

	xml_attribute a1 = node.attribute("a1");
	xml_attribute a2 = child.attribute("a2");

	CHECK(node.insert_copy_before(a1, xml_attribute()) == xml_attribute());
	CHECK(node.insert_copy_before(xml_attribute(), a1) == xml_attribute());
	CHECK(node.insert_copy_before(a2, a2) == xml_attribute());
	
	xml_attribute a3 = node.insert_copy_before(a1, a1);
	CHECK(a3 && a3 != a2 && a3 != a1);

	xml_attribute a4 = node.insert_copy_before(a2, a1);
	CHECK(a4 && a4 != a3 && a4 != a2 && a4 != a1);

	xml_attribute a5 = node.insert_copy_before(a4, a1);
	CHECK(a5 && a5 != a4 && a5 != a3 && a5 != a2 && a5 != a1);

	CHECK(child.insert_copy_before(a4, a4) == xml_attribute());

	CHECK_NODE(doc, "<node a1=\"v1\" a2=\"v2\" a2=\"v2\" a1=\"v1\"><child a2=\"v2\" /></node>");

	a3.set_name("a3");
	a3 = "v3";
	
	a4.set_name("a4");
	a4 = "v4";
	
	a5.set_name("a5");
	a5 = "v5";
	
	CHECK_NODE(doc, "<node a3=\"v3\" a4=\"v4\" a5=\"v5\" a1=\"v1\"><child a2=\"v2\" /></node>");
}

TEST_XML(dom_node_remove_attribute, "<node a1='v1' a2='v2' a3='v3'><child a4='v4'/></node>")
{
	xml_node().remove_attribute("a");
	xml_node().remove_attribute(xml_attribute());
	
	xml_node node = doc.child("node");
	xml_node child = node.child("child");

	node.remove_attribute("a");
	node.remove_attribute(xml_attribute());
	node.remove_attribute(child.attribute("a4"));

	CHECK_NODE(doc, "<node a1=\"v1\" a2=\"v2\" a3=\"v3\"><child a4=\"v4\" /></node>");

	node.remove_attribute("a1");
	node.remove_attribute(node.attribute("a3"));
	child.remove_attribute("a4");

	CHECK_NODE(doc, "<node a2=\"v2\"><child /></node>");
}
