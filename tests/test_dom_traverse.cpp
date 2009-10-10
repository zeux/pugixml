#include "common.hpp"

TEST_XML(dom_attr_bool_ops, "<node attr='1'/>")
{
	xml_attribute attr1;
	xml_attribute attr2 = doc.child("node").attribute("attr");

	CHECK(!attr1);
	CHECK(attr2);
	CHECK(!!attr2);

	bool attr1b = attr1;
	bool attr2b = attr2;

	CHECK(!attr1b);
	CHECK(attr2b);
}

TEST_XML(dom_attr_empty, "<node attr='1'/>")
{
	xml_attribute attr1;
	xml_attribute attr2 = doc.child("node").attribute("attr");

	CHECK(attr1.empty());
	CHECK(!attr2.empty());
}

TEST_XML(dom_node_bool_ops, "<node/>")
{
	xml_node node1;
	xml_node node2 = doc.child("node");

	CHECK(!node1);
	CHECK(node2);
	CHECK(!!node2);
	
	bool node1b = node1;
	bool node2b = node2;

	CHECK(!node1b);
	CHECK(node2b);
}

TEST_XML(dom_node_empty, "<node/>")
{
	xml_node node1;
	xml_node node2 = doc.child("node");

	CHECK(node1.empty());
	CHECK(!node2.empty());
}
