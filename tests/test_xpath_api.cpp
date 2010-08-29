#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

#include "helpers.hpp"

#include <string.h>
#include <string>

TEST_XML(xpath_api_select_nodes, "<node><head/><foo/><foo/><tail/></node>")
{
	xpath_node_set ns1 = doc.select_nodes(STR("node/foo"));

	xpath_query q(STR("node/foo"));
	xpath_node_set ns2 = doc.select_nodes(q);

	xpath_node_set_tester(ns1, "ns1") % 4 % 5;
	xpath_node_set_tester(ns2, "ns2") % 4 % 5;
}

TEST_XML(xpath_api_select_single_node, "<node><head/><foo id='1'/><foo/><tail/></node>")
{
	xpath_node n1 = doc.select_single_node(STR("node/foo"));

	xpath_query q(STR("node/foo"));
	xpath_node n2 = doc.select_single_node(q);

	CHECK(n1.node().attribute(STR("id")).as_int() == 1);
	CHECK(n2.node().attribute(STR("id")).as_int() == 1);

	xpath_node n3 = doc.select_single_node(STR("node/bar"));
	
	CHECK(!n3);

	xpath_node n4 = doc.select_single_node(STR("node/head/following-sibling::foo"));
	xpath_node n5 = doc.select_single_node(STR("node/tail/preceding-sibling::foo"));
	
	CHECK(n4.node().attribute(STR("id")).as_int() == 1);
	CHECK(n5.node().attribute(STR("id")).as_int() == 1);
}

TEST_XML(xpath_api_node_bool_ops, "<node attr='value'/>")
{
	generic_bool_ops_test(doc.select_single_node(STR("node")));
	generic_bool_ops_test(doc.select_single_node(STR("node/@attr")));
}

TEST_XML(xpath_api_node_eq_ops, "<node attr='value'/>")
{
	generic_eq_ops_test(doc.select_single_node(STR("node")), doc.select_single_node(STR("node/@attr")));
}

TEST_XML(xpath_api_node_accessors, "<node attr='value'/>")
{
	xpath_node null;
	xpath_node node = doc.select_single_node(STR("node"));
	xpath_node attr = doc.select_single_node(STR("node/@attr"));

	CHECK(!null.node());
	CHECK(!null.attribute());
	CHECK(!null.parent());

	CHECK(node.node() == doc.child(STR("node")));
	CHECK(!node.attribute());
	CHECK(node.parent() == doc);

	CHECK(!attr.node());
	CHECK(attr.attribute() == doc.child(STR("node")).attribute(STR("attr")));
	CHECK(attr.parent() == doc.child(STR("node")));
}

inline void xpath_api_node_accessors_helper(const xpath_node_set& set)
{
	CHECK(set.size() == 2);
	CHECK(set.type() == xpath_node_set::type_sorted);
	CHECK(!set.empty());
	CHECK_STRING(set[0].node().name(), STR("foo"));
	CHECK_STRING(set[1].node().name(), STR("foo"));
	CHECK(set.first() == set[0]);
	CHECK(set.begin() + 2 == set.end());
	CHECK(set.begin()[0] == set[0] && set.begin()[1] == set[1]);
}

TEST_XML(xpath_api_nodeset_accessors, "<node><foo/><foo/></node>")
{
	xpath_node_set null;
	CHECK(null.size() == 0);
	CHECK(null.type() == xpath_node_set::type_unsorted);
	CHECK(null.empty());
	CHECK(!null.first());
	CHECK(null.begin() == null.end());

	xpath_node_set set = doc.select_nodes(STR("node/foo"));
	xpath_api_node_accessors_helper(set);

	xpath_node_set copy = set;
	xpath_api_node_accessors_helper(copy);

	xpath_node_set assigned;
	assigned = set;
	xpath_api_node_accessors_helper(assigned);

	xpath_node_set nullcopy = null;
}

TEST_XML(xpath_api_nodeset_copy, "<node><foo/><foo/></node>")
{
	xpath_node_set set = doc.select_nodes(STR("node/foo"));

	xpath_node_set copy1 = set;
	CHECK(copy1.size() == 2);
	CHECK_STRING(copy1[0].node().name(), STR("foo"));

	xpath_node_set copy2;
	copy2 = set;
	CHECK(copy2.size() == 2);
	CHECK_STRING(copy2[0].node().name(), STR("foo"));

	xpath_node_set copy3;
	copy3 = set;
	copy3 = copy3;
	CHECK(copy3.size() == 2);
	CHECK_STRING(copy3[0].node().name(), STR("foo"));

	xpath_node_set copy4;
	copy4 = set;
	copy4 = copy1;
	CHECK(copy4.size() == 2);
	CHECK_STRING(copy4[0].node().name(), STR("foo"));
}

TEST_XML(xpath_api_evaluate, "<node attr='3'/>")
{
	xpath_query q(STR("node/@attr"));

	CHECK(q.evaluate_boolean(doc));
	CHECK(q.evaluate_number(doc) == 3);
	CHECK(q.evaluate_string(doc) == STR("3"));

	xpath_node_set ns = q.evaluate_node_set(doc);
	CHECK(ns.size() == 1 && ns[0].attribute() == doc.child(STR("node")).attribute(STR("attr")));
}

#ifdef PUGIXML_NO_EXCEPTIONS
TEST_XML(xpath_api_evaluate_fail, "<node attr='3'/>")
{
	CHECK_XPATH_BOOLEAN(doc, STR(""), false);
	CHECK_XPATH_NUMBER_NAN(doc, STR(""));
	CHECK_XPATH_STRING(doc, STR(""), STR(""));
	CHECK_XPATH_NODESET(doc, STR(""));
}
#endif

TEST(xpath_api_evaluate_node_set_fail)
{
#ifdef PUGIXML_NO_EXCEPTIONS
	CHECK_XPATH_NODESET(xml_node(), STR("1"));
#else
	try
	{
		xpath_query q(STR("1"));

		q.evaluate_node_set(xml_node());

		CHECK_FORCE_FAIL("Expected exception");
	}
	catch (const xpath_exception&)
	{
	}
#endif
}

TEST(xpath_api_return_type)
{
#ifdef PUGIXML_NO_EXCEPTIONS
	CHECK(xpath_query(STR("")).return_type() == xpath_type_none);
#endif

	CHECK(xpath_query(STR("node")).return_type() == xpath_type_node_set);
	CHECK(xpath_query(STR("1")).return_type() == xpath_type_number);
	CHECK(xpath_query(STR("'s'")).return_type() == xpath_type_string);
	CHECK(xpath_query(STR("true()")).return_type() == xpath_type_boolean);
}

TEST(xpath_api_query_bool)
{
	xpath_query q(STR("node"));
	
	CHECK(q);
	CHECK((!q) == false);
}

#ifdef PUGIXML_NO_EXCEPTIONS
TEST(xpath_api_query_bool_fail)
{
	xpath_query q(STR(""));
	
	CHECK((q ? true : false) == false);
	CHECK((!q) == true);
}
#endif

TEST(xpath_api_query_result)
{
	xpath_query q(STR("node"));

	CHECK(q.result());
	CHECK(q.result().error == 0);
	CHECK(q.result().offset == 0);
	CHECK(strcmp(q.result().description(), "No error") == 0);
}

TEST(xpath_api_query_result_fail)
{
#ifndef PUGIXML_NO_EXCEPTIONS
	try
	{
#endif
		xpath_query q(STR("//foo/child::/bar"));

#ifndef PUGIXML_NO_EXCEPTIONS
		CHECK_FORCE_FAIL("Expected exception");
	}
	catch (const xpath_exception& q)
	{
#endif
		xpath_parse_result result = q.result();

		CHECK(!result);
		CHECK(result.error != 0 && result.error[0] != 0);
		CHECK(result.description() == result.error);
		CHECK(result.offset == 13);

#ifndef PUGIXML_NO_EXCEPTIONS
	}
#endif
}

#ifndef PUGIXML_NO_EXCEPTIONS
TEST(xpath_api_exception_what)
{
	try
	{
		xpath_query q(STR(""));

		CHECK_FORCE_FAIL("Expected exception");
	}
	catch (const xpath_exception& e)
	{
		CHECK(e.what()[0] != 0);
	}
}
#endif

// $$$
// out of memory during parsing/execution (?)
#endif
