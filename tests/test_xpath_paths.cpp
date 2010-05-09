#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

TEST_XML(xpath_paths_axes_child, "<node attr='value'><child attr='value'><subchild/></child><another/><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("child:: node()"));

	CHECK_XPATH_NODESET(n, STR("child:: node()")) % 4 % 7 % 8; // child, another, last
	CHECK_XPATH_NODESET(n, STR("another/child:: node()"));
}

TEST_XML(xpath_paths_axes_descendant, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("descendant:: node()"));

	CHECK_XPATH_NODESET(n, STR("descendant:: node()")) % 4 % 6 % 7 % 8 % 9; // child, subchild, another, subchild, last
	CHECK_XPATH_NODESET(doc, STR("descendant:: node()")) % 2 % 4 % 6 % 7 % 8 % 9; // node, child, subchild, another, subchild, last
	CHECK_XPATH_NODESET(n, STR("another/descendant:: node()")) % 8; // subchild
	CHECK_XPATH_NODESET(n, STR("last/descendant:: node()"));
}

TEST_XML(xpath_paths_axes_parent, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("parent:: node()"));

	CHECK_XPATH_NODESET(n.child(STR("child")), STR("parent:: node()")) % 2; // node
	CHECK_XPATH_NODESET(n, STR("child/subchild/parent:: node()")) % 4; // child
	CHECK_XPATH_NODESET(n, STR("@attr/parent:: node()")) % 2; // node
	CHECK_XPATH_NODESET(n, STR("parent:: node()")) % 1; // root
	CHECK_XPATH_NODESET(doc, STR("parent:: node()"));
}

TEST_XML(xpath_paths_axes_ancestor, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("ancestor:: node()"));

	CHECK_XPATH_NODESET(n.child(STR("child")), STR("ancestor:: node()")) % 2 % 1; // node, root
	CHECK_XPATH_NODESET(n, STR("child/subchild/ancestor:: node()")) % 4 % 2 % 1; // child, node, root
	CHECK_XPATH_NODESET(n, STR("child/@attr/ancestor:: node()")) % 4 % 2 % 1; // child, node, root
	CHECK_XPATH_NODESET(n, STR("ancestor:: node()")) % 1; // root
	CHECK_XPATH_NODESET(doc, STR("ancestor:: node()"));
}

TEST_XML(xpath_paths_axes_following_sibling, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("following-sibling:: node()"));

	CHECK_XPATH_NODESET(n.child(STR("child")), STR("following-sibling:: node()")) % 8 % 10; // another, last
	CHECK_XPATH_NODESET(n.child(STR("last")), STR("following-sibling:: node()"));
	CHECK_XPATH_NODESET(n, STR("@attr1/following-sibling:: node()")); // attributes are not siblings
}

TEST_XML(xpath_paths_axes_preceding_sibling, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("preceding-sibling:: node()"));

	CHECK_XPATH_NODESET(n.child(STR("child")), STR("preceding-sibling:: node()"));
	CHECK_XPATH_NODESET(n.child(STR("last")), STR("preceding-sibling:: node()")) % 8 % 5; // another, child
	CHECK_XPATH_NODESET(n, STR("@attr2/following-sibling:: node()")); // attributes are not siblings
}

TEST_XML(xpath_paths_axes_following, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another><subchild/></another><almost/><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("following:: node()"));

	CHECK_XPATH_NODESET(n, STR("following:: node()")); // no descendants
	CHECK_XPATH_NODESET(n.child(STR("child")), STR("following:: node()")) % 8 % 9 % 10 % 11; // another, subchild, almost, last
	CHECK_XPATH_NODESET(n.child(STR("child")).child(STR("subchild")), STR("following:: node()")) % 8 % 9 % 10 % 11; // another, subchild, almost, last
	CHECK_XPATH_NODESET(n.child(STR("last")), STR("following:: node()"));
}

TEST_XML(xpath_paths_axes_preceding, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another><subchild/></another><almost/><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("preceding:: node()"));

	CHECK_XPATH_NODESET(n.child(STR("child")), STR("preceding:: node()")); // no ancestors
	CHECK_XPATH_NODESET(n.child(STR("last")), STR("preceding:: node()")) % 10 % 9 % 8 % 7 % 5; // almost, subchild, another, subchild, child
	CHECK_XPATH_NODESET(n.child(STR("another")).child(STR("subchild")), STR("preceding:: node()")) % 7 % 5; // subchild, child
	CHECK_XPATH_NODESET(n, STR("preceding:: node()"));
}

TEST_XML(xpath_paths_axes_attribute, "<node attr1='value' attr2='value'><child attr='value'><subchild/></child><another xmlns:foo='bar'><subchild/></another><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("attribute:: node()"));

	CHECK_XPATH_NODESET(n.child(STR("child")), STR("attribute:: node()")) % 6; // child/@attr
	CHECK_XPATH_NODESET(n.child(STR("last")), STR("attribute:: node()"));
	CHECK_XPATH_NODESET(n, STR("attribute:: node()")) % 3 % 4; // node/@attr1 node/@attr2
	CHECK_XPATH_NODESET(doc, STR("descendant-or-self:: node()/attribute:: node()")) % 3 % 4 % 6; // all attributes
	CHECK_XPATH_NODESET(n.child(STR("another")), STR("attribute:: node()")); // namespace nodes are not attributes
}

TEST_XML(xpath_paths_axes_namespace, "<node xmlns:foo='bar'/>")
{
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(n, STR("namespace:: node()")); // namespace nodes are not supported
}

TEST_XML(xpath_paths_axes_self, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("self:: node()"));

	CHECK_XPATH_NODESET(n.child(STR("child")), STR("self:: node()")) % 4; // child
	CHECK_XPATH_NODESET(n, STR("self:: node()")) % 2; // node
	CHECK_XPATH_NODESET(n, STR("child/self:: node()")) % 4; // child
	CHECK_XPATH_NODESET(n, STR("child/@attr/self:: node()")) % 5; // @attr
	CHECK_XPATH_NODESET(doc, STR("self:: node()")) % 1; // root
}

TEST_XML(xpath_paths_axes_descendant_or_self, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("descendant-or-self:: node()"));

	CHECK_XPATH_NODESET(n, STR("descendant-or-self:: node()")) % 2 % 4 % 6 % 7 % 8 % 9; // node, child, subchild, another, subchild, last
	CHECK_XPATH_NODESET(doc, STR("descendant-or-self:: node()")) % 1 % 2 % 4 % 6 % 7 % 8 % 9; // root, node, child, subchild, another, subchild, last
	CHECK_XPATH_NODESET(n, STR("another/descendant-or-self:: node()")) % 7 % 8; // another, subchild
	CHECK_XPATH_NODESET(n, STR("last/descendant-or-self:: node()")) % 9; // last
}

TEST_XML(xpath_paths_axes_ancestor_or_self, "<node attr='value'><child attr='value'><subchild/></child><another><subchild/></another><last/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("ancestor-or-self:: node()"));

	CHECK_XPATH_NODESET(n.child(STR("child")), STR("ancestor-or-self:: node()")) % 4 % 2 % 1; // child, node, root
	CHECK_XPATH_NODESET(n, STR("child/subchild/ancestor-or-self:: node()")) % 6 % 4 % 2 % 1; // subchild, child, node, root
	CHECK_XPATH_NODESET(n, STR("child/@attr/ancestor-or-self:: node()")) % 5 % 4 % 2 % 1; // @attr, child, node, root
	CHECK_XPATH_NODESET(n, STR("ancestor-or-self:: node()")) % 2 % 1; // root, node
	CHECK_XPATH_NODESET(doc, STR("ancestor-or-self:: node()")) % 1; // root
}

TEST_XML(xpath_paths_axes_abbrev, "<node attr='value'><foo/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	// @ axis
	CHECK_XPATH_NODESET(c, STR("@attr"));
	CHECK_XPATH_NODESET(n, STR("@attr")) % 3;

	// no axis - child implied
	CHECK_XPATH_NODESET(c, STR("foo"));
	CHECK_XPATH_NODESET(n, STR("foo")) % 4;
	CHECK_XPATH_NODESET(doc, STR("node()")) % 2;

	// @ axis should disable all other axis specifiers
	CHECK_XPATH_FAIL(STR("@child::foo"));
	CHECK_XPATH_FAIL(STR("@attribute::foo"));
}

TEST_XML(xpath_paths_nodetest_all, "<node a1='v1' x:a2='v2'><c1/><x:c2/><c3/><x:c4/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("*"));
	CHECK_XPATH_NODESET(c, STR("child::*"));

	CHECK_XPATH_NODESET(n, STR("*")) % 5 % 6 % 7 % 8;
	CHECK_XPATH_NODESET(n, STR("child::*")) % 5 % 6 % 7 % 8;
	CHECK_XPATH_NODESET(n, STR("attribute::*")) % 3 % 4;
}

TEST_XML_FLAGS(xpath_paths_nodetest_name, "<node a1='v1' x:a2='v2'><c1/><x:c2/><c3/><x:c4/><?c1?></node>", parse_default | parse_pi)
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("c1"));
	CHECK_XPATH_NODESET(c, STR("child::c1"));

	CHECK_XPATH_NODESET(n, STR("c1")) % 5;
	CHECK_XPATH_NODESET(n, STR("x:c2")) % 6;

	CHECK_XPATH_NODESET(n, STR("child::c1")) % 5;
	CHECK_XPATH_NODESET(n, STR("child::x:c2")) % 6;

	CHECK_XPATH_NODESET(n, STR("attribute::a1")) % 3;
	CHECK_XPATH_NODESET(n, STR("attribute::x:a2")) % 4;
	CHECK_XPATH_NODESET(n, STR("@x:a2")) % 4;
}

TEST_XML(xpath_paths_nodetest_all_in_namespace, "<node a1='v1' x:a2='v2'><c1/><x:c2/><c3/><x:c4/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("x:*"));
	CHECK_XPATH_NODESET(c, STR("child::x:*"));

	CHECK_XPATH_NODESET(n, STR("x:*")) % 6 % 8;
	CHECK_XPATH_NODESET(n, STR("child::x:*")) % 6 % 8;

	CHECK_XPATH_NODESET(n, STR("attribute::x:*")) % 4;
	CHECK_XPATH_NODESET(n, STR("@x:*")) % 4;

	CHECK_XPATH_FAIL(STR(":*"));
	CHECK_XPATH_FAIL(STR("@:*"));
}

TEST_XML_FLAGS(xpath_paths_nodetest_type, "<node attr='value'>pcdata<child/><?pi1 value?><?pi2 value?><!--comment--><![CDATA[cdata]]></node>", parse_default | parse_pi | parse_comments)
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	// check on empty nodes
	CHECK_XPATH_NODESET(c, STR("node()"));
	CHECK_XPATH_NODESET(c, STR("text()"));
	CHECK_XPATH_NODESET(c, STR("comment()"));
	CHECK_XPATH_NODESET(c, STR("processing-instruction()"));
	CHECK_XPATH_NODESET(c, STR("processing-instruction('foobar')"));

	// child axis
	CHECK_XPATH_NODESET(n, STR("node()")) % 4 % 5 % 6 % 7 % 8 % 9;
	CHECK_XPATH_NODESET(n, STR("text()")) % 4 % 9;
	CHECK_XPATH_NODESET(n, STR("comment()")) % 8;
	CHECK_XPATH_NODESET(n, STR("processing-instruction()")) % 6 % 7;
	CHECK_XPATH_NODESET(n, STR("processing-instruction('pi2')")) % 7;

	// attribute axis
	CHECK_XPATH_NODESET(n, STR("@node()")) % 3;
	CHECK_XPATH_NODESET(n, STR("@text()"));
	CHECK_XPATH_NODESET(n, STR("@comment()"));
	CHECK_XPATH_NODESET(n, STR("@processing-instruction()"));
	CHECK_XPATH_NODESET(n, STR("@processing-instruction('pi2')"));

	// incorrect 'argument' number
	CHECK_XPATH_FAIL(STR("node('')"));
	CHECK_XPATH_FAIL(STR("text('')"));
	CHECK_XPATH_FAIL(STR("comment('')"));
	CHECK_XPATH_FAIL(STR("processing-instruction(1)"));
	CHECK_XPATH_FAIL(STR("processing-instruction('', '')"));
}

TEST_XML(xpath_paths_absolute, "<node><foo><foo/><foo/></foo></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("/foo"));
	CHECK_XPATH_NODESET(n, STR("/foo"));
	CHECK_XPATH_NODESET(n, STR("/node/foo")) % 3;
	CHECK_XPATH_NODESET(n.child(STR("foo")), STR("/node/foo")) % 3;

	CHECK_XPATH_NODESET(c, STR("/"));
	CHECK_XPATH_NODESET(n, STR("/")) % 1;
	CHECK_XPATH_NODESET(n.child(STR("foo")), STR("/")) % 1;
}

TEST_XML(xpath_paths_step_abbrev, "<node><foo/></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("."));
	CHECK_XPATH_NODESET(c, STR(".."));

	CHECK_XPATH_NODESET(n, STR(".")) % 2;
	CHECK_XPATH_NODESET(n, STR("..")) % 1;
	CHECK_XPATH_NODESET(n, STR("../node")) % 2;
	CHECK_XPATH_NODESET(n.child(STR("foo")), STR("..")) % 2;

	CHECK_XPATH_FAIL(STR(".node"));
	CHECK_XPATH_FAIL(STR("..node"));
}

TEST_XML(xpath_paths_relative_abbrev, "<node><foo><foo/><foo/></foo></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("foo//bar"));

	CHECK_XPATH_NODESET(n, STR("foo/foo")) % 4 % 5;
	CHECK_XPATH_NODESET(n, STR("foo//foo")) % 4 % 5;
	CHECK_XPATH_NODESET(n, STR(".//foo")) % 3 % 4 % 5;
}

TEST_XML(xpath_paths_absolute_abbrev, "<node><foo><foo/><foo/></foo></node>")
{
	xml_node c;
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(c, STR("//bar"));

	CHECK_XPATH_NODESET(n, STR("//foo")) % 3 % 4 % 5;
	CHECK_XPATH_NODESET(n.child(STR("foo")), STR("//foo")) % 3 % 4 % 5;
	CHECK_XPATH_NODESET(doc, STR("//foo")) % 3 % 4 % 5;
}

TEST_XML(xpath_paths_predicate_boolean, "<node><chapter/><chapter/><chapter/><chapter/><chapter/></node>")
{
	xml_node n = doc.child(STR("node")).child(STR("chapter")).next_sibling().next_sibling();

	CHECK_XPATH_NODESET(n, STR("following-sibling::chapter[position()=1]")) % 6;
	CHECK_XPATH_NODESET(n, STR("following-sibling::chapter[position()=2]")) % 7;
	CHECK_XPATH_NODESET(n, STR("preceding-sibling::chapter[position()=1]")) % 4;
	CHECK_XPATH_NODESET(n, STR("preceding-sibling::chapter[position()=2]")) % 3;
}

TEST_XML(xpath_paths_predicate_number, "<node><chapter/><chapter/><chapter/><chapter/><chapter/></node>")
{
	xml_node n = doc.child(STR("node")).child(STR("chapter")).next_sibling().next_sibling();

	CHECK_XPATH_NODESET(n, STR("following-sibling::chapter[1]")) % 6;
	CHECK_XPATH_NODESET(n, STR("following-sibling::chapter[2]")) % 7;
	CHECK_XPATH_NODESET(n, STR("preceding-sibling::chapter[1]")) % 4;
	CHECK_XPATH_NODESET(n, STR("preceding-sibling::chapter[2]")) % 3;
}

TEST_XML(xpath_paths_predicate_several, "<node><employee/><employee secretary=''/><employee assistant=''/><employee secretary='' assistant=''/><employee assistant='' secretary=''/></node>")
{
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(n, STR("employee")) % 3 % 4 % 6 % 8 % 11;
	CHECK_XPATH_NODESET(n, STR("employee[@secretary]")) % 4 % 8 % 11;
	CHECK_XPATH_NODESET(n, STR("employee[@assistant]")) % 6 % 8 % 11;
	CHECK_XPATH_NODESET(n, STR("employee[@secretary][@assistant]")) % 8 % 11;
	CHECK_XPATH_NODESET(n, STR("employee[@assistant][@secretary]")) % 8 % 11;
	CHECK_XPATH_NODESET(n, STR("employee[@secretary and @assistant]")) % 8 % 11;
}

TEST_XML(xpath_paths_predicate_filter_boolean, "<node><chapter/><chapter/><chapter/><chapter/><chapter/></node>")
{
	xml_node n = doc.child(STR("node")).child(STR("chapter")).next_sibling().next_sibling();

	CHECK_XPATH_NODESET(n, STR("(following-sibling::chapter)[position()=1]")) % 6;
	CHECK_XPATH_NODESET(n, STR("(following-sibling::chapter)[position()=2]")) % 7;
	CHECK_XPATH_NODESET(n, STR("(preceding-sibling::chapter)[position()=1]")) % 3;
	CHECK_XPATH_NODESET(n, STR("(preceding-sibling::chapter)[position()=2]")) % 4;
}

TEST_XML(xpath_paths_predicate_filter_number, "<node><chapter/><chapter/><chapter/><chapter/><chapter/></node>")
{
	xml_node n = doc.child(STR("node")).child(STR("chapter")).next_sibling().next_sibling();

	CHECK_XPATH_NODESET(n, STR("(following-sibling::chapter)[1]")) % 6;
	CHECK_XPATH_NODESET(n, STR("(following-sibling::chapter)[2]")) % 7;
	CHECK_XPATH_NODESET(n, STR("(preceding-sibling::chapter)[1]")) % 3;
	CHECK_XPATH_NODESET(n, STR("(preceding-sibling::chapter)[2]")) % 4;
}

TEST_XML(xpath_paths_predicate_filter_posinv, "<node><employee/><employee secretary=''/><employee assistant=''/><employee secretary='' assistant=''/><employee assistant='' secretary=''/></node>")
{
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(n, STR("employee")) % 3 % 4 % 6 % 8 % 11;
	CHECK_XPATH_NODESET(n, STR("(employee[@secretary])[@assistant]")) % 8 % 11;
	CHECK_XPATH_NODESET(n, STR("((employee)[@assistant])[@secretary]")) % 8 % 11;
}

TEST_XML(xpath_paths_step_compose, "<node><foo><foo/><foo/></foo><foo/></node>")
{
	xml_node n = doc.child(STR("node"));

	CHECK_XPATH_NODESET(n, STR("(.)/foo")) % 3 % 6;
	CHECK_XPATH_NODESET(n, STR("(.)//foo")) % 3 % 4 % 5 % 6;
	CHECK_XPATH_NODESET(n, STR("(./..)//*")) % 2 % 3 % 4 % 5 % 6;

	CHECK_XPATH_FAIL(STR("(1)/foo"));
	CHECK_XPATH_FAIL(STR("(1)//foo"));
}

TEST_XML(xpath_paths_descendant_double_slash_w3c, "<node><para><para/><para/><para><para/></para></para><para/></node>")
{
	CHECK_XPATH_NODESET(doc, STR("//para")) % 3 % 4 % 5 % 6 % 7 % 8;
	CHECK_XPATH_NODESET(doc, STR("/descendant::para")) % 3 % 4 % 5 % 6 % 7 % 8;
	CHECK_XPATH_NODESET(doc, STR("//para[1]")) % 3 % 4 % 7;
	CHECK_XPATH_NODESET(doc, STR("/descendant::para[1]")) % 3;
}

TEST_XML(xpath_paths_needs_sorting, "<node><child/><child/><child><subchild/><subchild/></child></node>")
{
    CHECK_XPATH_NODESET(doc, STR("(node/child/subchild)[2]")) % 7;
}

#endif
