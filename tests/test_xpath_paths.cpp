#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

TEST_XML(xpath_paths_axes_child, "<node attr='value'><child attr='value'><subchild/></child><another/><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "child:: node()");

	CHECK_XPATH_NODESET(n, "child:: node()") % 4 % 7 % 8; // child, another, last
	CHECK_XPATH_NODESET(n, "another/child:: node()");
}

TEST_XML(xpath_paths_axes_descendant, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "descendant:: node()");

	CHECK_XPATH_NODESET(n, "descendant:: node()") % 4 % 6 % 7 % 8 % 9; // child, subchild, another, subchild, last
	CHECK_XPATH_NODESET(doc, "descendant:: node()") % 2 % 4 % 6 % 7 % 8 % 9; // node, child, subchild, another, subchild, last
	CHECK_XPATH_NODESET(n, "another/descendant:: node()") % 8; // subchild
	CHECK_XPATH_NODESET(n, "last/descendant:: node()");
}

TEST_XML(xpath_paths_axes_parent, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "parent:: node()");

	CHECK_XPATH_NODESET(n.child("child"), "parent:: node()") % 2; // node
	CHECK_XPATH_NODESET(n, "child/subchild/parent:: node()") % 4; // child
	CHECK_XPATH_NODESET(n, "@attr/parent:: node()") % 2; // node
	CHECK_XPATH_NODESET(n, "parent:: node()") % 1; // root
	CHECK_XPATH_NODESET(doc, "parent:: node()");
}

TEST_XML(xpath_paths_axes_ancestor, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "ancestor:: node()");

	CHECK_XPATH_NODESET(n.child("child"), "ancestor:: node()") % 2 % 1; // node, root
	CHECK_XPATH_NODESET(n, "child/subchild/ancestor:: node()") % 4 % 2 % 1; // child, node, root
	CHECK_XPATH_NODESET(n, "child/@attr/ancestor:: node()") % 4 % 2 % 1; // child, node, root
	CHECK_XPATH_NODESET(n, "ancestor:: node()") % 1; // root
	CHECK_XPATH_NODESET(doc, "ancestor:: node()");
}

TEST_XML(xpath_paths_axes_following_sibling, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "following-sibling:: node()");

	CHECK_XPATH_NODESET(n.child("child"), "following-sibling:: node()") % 8 % 10; // another, last
	CHECK_XPATH_NODESET(n.child("last"), "following-sibling:: node()");
	CHECK_XPATH_NODESET(n, "@attr1/following-sibling:: node()"); // attributes are not siblings
}

TEST_XML(xpath_paths_axes_preceding_sibling, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "preceding-sibling:: node()");

	CHECK_XPATH_NODESET(n.child("child"), "preceding-sibling:: node()");
	CHECK_XPATH_NODESET(n.child("last"), "preceding-sibling:: node()") % 8 % 5; // another, child
	CHECK_XPATH_NODESET(n, "@attr2/following-sibling:: node()"); // attributes are not siblings
}

TEST_XML(xpath_paths_axes_following, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "following:: node()");

	CHECK_XPATH_NODESET(n, "following:: node()"); // no descendants
	CHECK_XPATH_NODESET(n.child("child"), "following:: node()") % 8 % 9 % 10; // another, subchild, last
	CHECK_XPATH_NODESET(n.child("child").child("subchild"), "following:: node()") % 8 % 9 % 10; // another, subchild, last
	CHECK_XPATH_NODESET(n.child("last"), "following:: node()");
}

TEST_XML(xpath_paths_axes_preceding, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "preceding:: node()");

	CHECK_XPATH_NODESET(n.child("child"), "preceding:: node()"); // no ancestors
	CHECK_XPATH_NODESET(n.child("last"), "preceding:: node()") % 9 % 8 % 7 % 5; // subchild, another, subchild, child
	CHECK_XPATH_NODESET(n.child("another").child("subchild"), "preceding:: node()") % 7 % 5; // subchild, child
	CHECK_XPATH_NODESET(n, "preceding:: node()");
}

TEST_XML(xpath_paths_axes_attribute, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another xmlns:foo='bar'><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "attribute:: node()");

	CHECK_XPATH_NODESET(n.child("child"), "attribute:: node()") % 6; // child/@attr
	CHECK_XPATH_NODESET(n.child("last"), "attribute:: node()");
	CHECK_XPATH_NODESET(n, "attribute:: node()") % 3 % 4; // node/@attr1 node/@attr2
	CHECK_XPATH_NODESET(doc, "descendant-or-self:: node()/attribute:: node()") % 3 % 4 % 6; // all attributes
	CHECK_XPATH_NODESET(n.child("another"), "attribute:: node()"); // namespace nodes are not attributes
}

TEST_XML(xpath_paths_axes_namespace, "<node xmlns:foo='bar'/>")
{
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(n, "namespace:: node()"); // namespace nodes are not supported
}

TEST_XML(xpath_paths_axes_self, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "self:: node()");

	CHECK_XPATH_NODESET(n.child("child"), "self:: node()") % 4; // child
	CHECK_XPATH_NODESET(n, "self:: node()") % 2; // node
	CHECK_XPATH_NODESET(n, "child/@attr/self:: node()") % 5; // @attr
	CHECK_XPATH_NODESET(doc, "self:: node()") % 1; // root
}

TEST_XML(xpath_paths_axes_descendant_or_self, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "descendant-or-self:: node()");

	CHECK_XPATH_NODESET(n, "descendant-or-self:: node()") % 2 % 4 % 6 % 7 % 8 % 9; // node, child, subchild, another, subchild, last
	CHECK_XPATH_NODESET(doc, "descendant-or-self:: node()") % 1 % 2 % 4 % 6 % 7 % 8 % 9; // root, node, child, subchild, another, subchild, last
	CHECK_XPATH_NODESET(n, "another/descendant-or-self:: node()") % 7 % 8; // another, subchild
	CHECK_XPATH_NODESET(n, "last/descendant-or-self:: node()") % 9; // last
}

TEST_XML(xpath_paths_axes_ancestor_or_self, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "ancestor-or-self:: node()");

	CHECK_XPATH_NODESET(n.child("child"), "ancestor-or-self:: node()") % 4 % 2 % 1; // child, node, root
	CHECK_XPATH_NODESET(n, "child/subchild/ancestor-or-self:: node()") % 6 % 4 % 2 % 1; // subchild, child, node, root
	CHECK_XPATH_NODESET(n, "child/@attr/ancestor-or-self:: node()") % 5 % 4 % 2 % 1; // @attr, child, node, root
	CHECK_XPATH_NODESET(n, "ancestor-or-self:: node()") % 2 % 1; // root, node
	CHECK_XPATH_NODESET(doc, "ancestor-or-self:: node()") % 1; // root
}

#endif
