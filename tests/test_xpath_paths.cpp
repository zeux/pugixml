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

TEST_XML(xpath_paths_axes_abbrev, "<node attr='value'><foo/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	// @ axis
	CHECK_XPATH_NODESET(c, "@attr");
	CHECK_XPATH_NODESET(n, "@attr") % 3;

	// no axis - child implied
	CHECK_XPATH_NODESET(c, "foo");
	CHECK_XPATH_NODESET(n, "foo") % 4;
	CHECK_XPATH_NODESET(doc, "node()") % 2;

	// @ axis should disable all other axis specifiers
	CHECK_XPATH_FAIL("@child::foo");
	CHECK_XPATH_FAIL("@attribute::foo");
}

TEST_XML(xpath_paths_nodetest_all, "<node a1='v1' x:a2='v2'><c1/><x:c2/><c3/><x:c4/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "*");
	CHECK_XPATH_NODESET(c, "child::*");

	CHECK_XPATH_NODESET(n, "*") % 5 % 6 % 7 % 8;
	CHECK_XPATH_NODESET(n, "child::*") % 5 % 6 % 7 % 8;
	CHECK_XPATH_NODESET(n, "attribute::*") % 3 % 4;
}

TEST_XML_FLAGS(xpath_paths_nodetest_name, "<node a1='v1' x:a2='v2'><c1/><x:c2/><c3/><x:c4/><?c1?></node>", parse_default | parse_pi)
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "c1");
	CHECK_XPATH_NODESET(c, "child::c1");

	CHECK_XPATH_NODESET(n, "c1") % 5;
	CHECK_XPATH_NODESET(n, "x:c2") % 6;

	CHECK_XPATH_NODESET(n, "child::c1") % 5;
	CHECK_XPATH_NODESET(n, "child::x:c2") % 6;

	CHECK_XPATH_NODESET(n, "attribute::a1") % 3;
	CHECK_XPATH_NODESET(n, "attribute::x:a2") % 4;
	CHECK_XPATH_NODESET(n, "@x:a2") % 4;
}

TEST_XML(xpath_paths_nodetest_all_in_namespace, "<node a1='v1' x:a2='v2'><c1/><x:c2/><c3/><x:c4/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "x:*");
	CHECK_XPATH_NODESET(c, "child::x:*");

	CHECK_XPATH_NODESET(n, "x:*") % 6 % 8;
	CHECK_XPATH_NODESET(n, "child::x:*") % 6 % 8;

	CHECK_XPATH_NODESET(n, "attribute::x:*") % 4;
	CHECK_XPATH_NODESET(n, "@x:*") % 4;

	CHECK_XPATH_FAIL(":*");
	CHECK_XPATH_FAIL("@:*");
}

TEST_XML_FLAGS(xpath_paths_nodetest_type, "<node attr='value'>pcdata<child/><?pi1 value?><?pi2 value?><!--comment--><![CDATA[cdata]]></node>", parse_default | parse_pi | parse_comments)
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	// check on empty nodes
	CHECK_XPATH_NODESET(c, "node()");
	CHECK_XPATH_NODESET(c, "text()");
	CHECK_XPATH_NODESET(c, "comment()");
	CHECK_XPATH_NODESET(c, "processing-instruction()");
	CHECK_XPATH_NODESET(c, "processing-instruction('foobar')");

	// child axis
	CHECK_XPATH_NODESET(n, "node()") % 4 % 5 % 6 % 7 % 8 % 9;
	CHECK_XPATH_NODESET(n, "text()") % 4 % 9;
	CHECK_XPATH_NODESET(n, "comment()") % 8;
	CHECK_XPATH_NODESET(n, "processing-instruction()") % 6 % 7;
	CHECK_XPATH_NODESET(n, "processing-instruction('pi2')") % 7;

	// attribute axis
	CHECK_XPATH_NODESET(n, "@node()") % 3;
	CHECK_XPATH_NODESET(n, "@text()");
	CHECK_XPATH_NODESET(n, "@comment()");
	CHECK_XPATH_NODESET(n, "@processing-instruction()");
	CHECK_XPATH_NODESET(n, "@processing-instruction('pi2')");

	// incorrect 'argument' number
	CHECK_XPATH_FAIL("node('')");
	CHECK_XPATH_FAIL("text('')");
	CHECK_XPATH_FAIL("comment('')");
	CHECK_XPATH_FAIL("processing-instruction(1)");
	CHECK_XPATH_FAIL("processing-instruction('', '')");
}

TEST_XML(xpath_paths_absolute, "<node><foo><foo/><foo/></foo></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "/foo");
	CHECK_XPATH_NODESET(n, "/foo");
	CHECK_XPATH_NODESET(n, "/node/foo") % 3;
	CHECK_XPATH_NODESET(n.child("foo"), "/node/foo") % 3;

	CHECK_XPATH_NODESET(c, "/");
	CHECK_XPATH_NODESET(n, "/") % 1;
	CHECK_XPATH_NODESET(n.child("foo"), "/") % 1;
}

TEST_XML(xpath_paths_step_abbrev, "<node><foo/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, ".");
	CHECK_XPATH_NODESET(c, "..");

	CHECK_XPATH_NODESET(n, ".") % 2;
	CHECK_XPATH_NODESET(n, "..") % 1;
	CHECK_XPATH_NODESET(n, "../node") % 2;
	CHECK_XPATH_NODESET(n.child("foo"), "..") % 2;

	CHECK_XPATH_FAIL(".node");
	CHECK_XPATH_FAIL("..node");
}

TEST_XML(xpath_paths_relative_abbrev, "<node><foo><foo/><foo/></foo></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "foo//bar");

	CHECK_XPATH_NODESET(n, "foo/foo") % 4 % 5;
	CHECK_XPATH_NODESET(n, "foo//foo") % 4 % 5;
	CHECK_XPATH_NODESET(n, ".//foo") % 3 % 4 % 5;
}

TEST_XML(xpath_paths_absolute_abbrev, "<node><foo><foo/><foo/></foo></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "//bar");

	CHECK_XPATH_NODESET(n, "//foo") % 3 % 4 % 5;
	CHECK_XPATH_NODESET(n.child("foo"), "//foo") % 3 % 4 % 5;
	CHECK_XPATH_NODESET(doc, "//foo") % 3 % 4 % 5;
}

TEST_XML(xpath_paths_predicate_boolean, "<node><chapter/><chapter/><chapter/><chapter/><chapter/></node>")
{
	doc.precompute_document_order();

	xml_node n = doc.child("node").child("chapter").next_sibling().next_sibling();

	CHECK_XPATH_NODESET(n, "following-sibling::chapter[position()=1]") % 6;
	CHECK_XPATH_NODESET(n, "following-sibling::chapter[position()=2]") % 7;
	CHECK_XPATH_NODESET(n, "preceding-sibling::chapter[position()=1]") % 4;
	CHECK_XPATH_NODESET(n, "preceding-sibling::chapter[position()=2]") % 3;
}

TEST_XML(xpath_paths_predicate_number, "<node><chapter/><chapter/><chapter/><chapter/><chapter/></node>")
{
	doc.precompute_document_order();

	xml_node n = doc.child("node").child("chapter").next_sibling().next_sibling();

	CHECK_XPATH_NODESET(n, "following-sibling::chapter[1]") % 6;
	CHECK_XPATH_NODESET(n, "following-sibling::chapter[2]") % 7;
	CHECK_XPATH_NODESET(n, "preceding-sibling::chapter[1]") % 4;
	CHECK_XPATH_NODESET(n, "preceding-sibling::chapter[2]") % 3;
}

TEST_XML(xpath_paths_predicate_several, "<node><employee/><employee secretary=''/><employee assistant=''/><employee secretary='' assistant=''/><employee assistant='' secretary=''/></node>")
{
	doc.precompute_document_order();

	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(n, "employee") % 3 % 4 % 6 % 8 % 11;
	CHECK_XPATH_NODESET(n, "employee[@secretary]") % 4 % 8 % 11;
	CHECK_XPATH_NODESET(n, "employee[@assistant]") % 6 % 8 % 11;
	CHECK_XPATH_NODESET(n, "employee[@secretary][@assistant]") % 8 % 11;
	CHECK_XPATH_NODESET(n, "employee[@assistant][@secretary]") % 8 % 11;
	CHECK_XPATH_NODESET(n, "employee[@secretary and @assistant]") % 8 % 11;
}

TEST_XML(xpath_paths_predicate_filter_boolean, "<node><chapter/><chapter/><chapter/><chapter/><chapter/></node>")
{
	doc.precompute_document_order();

	xml_node n = doc.child("node").child("chapter").next_sibling().next_sibling();

	CHECK_XPATH_NODESET(n, "(following-sibling::chapter)[position()=1]") % 6;
	CHECK_XPATH_NODESET(n, "(following-sibling::chapter)[position()=2]") % 7;
	CHECK_XPATH_NODESET(n, "(preceding-sibling::chapter)[position()=1]") % 3;
	CHECK_XPATH_NODESET(n, "(preceding-sibling::chapter)[position()=2]") % 4;
}

TEST_XML(xpath_paths_predicate_filter_number, "<node><chapter/><chapter/><chapter/><chapter/><chapter/></node>")
{
	doc.precompute_document_order();

	xml_node n = doc.child("node").child("chapter").next_sibling().next_sibling();

	CHECK_XPATH_NODESET(n, "(following-sibling::chapter)[1]") % 6;
	CHECK_XPATH_NODESET(n, "(following-sibling::chapter)[2]") % 7;
	CHECK_XPATH_NODESET(n, "(preceding-sibling::chapter)[1]") % 3;
	CHECK_XPATH_NODESET(n, "(preceding-sibling::chapter)[2]") % 4;
}

TEST_XML(xpath_paths_predicate_filter_posinv, "<node><employee/><employee secretary=''/><employee assistant=''/><employee secretary='' assistant=''/><employee assistant='' secretary=''/></node>")
{
	doc.precompute_document_order();

	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(n, "employee") % 3 % 4 % 6 % 8 % 11;
	CHECK_XPATH_NODESET(n, "(employee[@secretary])[@assistant]") % 8 % 11;
	CHECK_XPATH_NODESET(n, "((employee)[@assistant])[@secretary]") % 8 % 11;
}

TEST_XML(xpath_paths_step_compose, "<node><foo><foo/><foo/></foo><foo/></node>")
{
	doc.precompute_document_order();

	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(n, "(.)/foo") % 3 % 6;
	CHECK_XPATH_NODESET(n, "(.)//foo") % 3 % 4 % 5 % 6;
	CHECK_XPATH_NODESET(n, "(./..)//*") % 2 % 3 % 4 % 5 % 6;

	CHECK_XPATH_FAIL("(1)/foo");
	CHECK_XPATH_FAIL("(1)//foo");
}

TEST_XML(xpath_paths_descendant_double_slash_w3c, "<node><para><para/><para/><para><para/></para></para><para/></node>")
{
	doc.precompute_document_order();

	CHECK_XPATH_NODESET(doc, "//para") % 3 % 4 % 5 % 6 % 7 % 8;
	CHECK_XPATH_NODESET(doc, "/descendant::para") % 3 % 4 % 5 % 6 % 7 % 8;
	CHECK_XPATH_NODESET(doc, "//para[1]") % 3 % 4 % 7;
	CHECK_XPATH_NODESET(doc, "/descendant::para[1]") % 3;
}

TEST_XML(xpath_paths_needs_sorting, "<node><child/><child/><child><subchild/><subchild/></child></node>")
{
    doc.precompute_document_order();

    CHECK_XPATH_NODESET(doc, "(node/child/subchild)[2]") % 7;
}

#endif
