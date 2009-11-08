#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

#include "helpers.hpp"

TEST_XML(xpath_api_select_nodes, "<node><head/><foo/><foo/><tail/></node>")
{
	doc.precompute_document_order();

	xpath_node_set ns1 = doc.select_nodes("node/foo");

	xpath_query q("node/foo");
	xpath_node_set ns2 = doc.select_nodes(q);

	CHECK(ns1.size() == 2 && ns1[0].node().document_order() == 4 && ns1[1].node().document_order() == 5);
	CHECK(ns2.size() == 2 && ns2[0].node().document_order() == 4 && ns2[1].node().document_order() == 5);
}

TEST_XML(xpath_api_select_single_node, "<node><head/><foo/><foo/><tail/></node>")
{
	doc.precompute_document_order();

	xpath_node n1 = doc.select_single_node("node/foo");

	xpath_query q("node/foo");
	xpath_node n2 = doc.select_single_node(q);

	CHECK(n1.node().document_order() == 4);
	CHECK(n2.node().document_order() == 4);

	xpath_node n3 = doc.select_single_node("node/bar");
	
	CHECK(!n3);

	xpath_node n4 = doc.select_single_node("node/head/following-sibling::foo");
	xpath_node n5 = doc.select_single_node("node/tail/preceding-sibling::foo");
	
	CHECK(n4.node().document_order() == 4);
	CHECK(n5.node().document_order() == 4);
}

TEST(xpath_api_exception_what)
{
	try
	{
		xpath_query q("");
	}
	catch (const xpath_exception& e)
	{
		CHECK(e.what()[0] != 0);
	}
}

TEST_XML(xpath_api_node_bool_ops, "<node attr='value'/>")
{
	generic_bool_ops_test(doc.select_single_node("node"));
	generic_bool_ops_test(doc.select_single_node("node/@attr"));
}

TEST_XML(xpath_api_node_eq_ops, "<node attr='value'/>")
{
	generic_eq_ops_test(doc.select_single_node("node"), doc.select_single_node("node/@attr"));
}

#endif
