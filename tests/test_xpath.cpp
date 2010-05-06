#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

#include <float.h>
#include <string.h>
#include <wchar.h>

#include <string>

TEST_XML(xpath_document_order, "<node><child1 attr1='value1' attr2='value2'/><child2 attr1='value1'>test</child2></node>")
{
	CHECK(xml_node().document_order() == 0);
	CHECK(doc.child(STR("node")).document_order() == 0);
	CHECK(doc.document_order() == 0);

	doc.precompute_document_order();

	CHECK(doc.document_order() == 1);
	CHECK(doc.child(STR("node")).document_order() == 2);
	CHECK(doc.child(STR("node")).child(STR("child1")).document_order() == 3);
	CHECK(doc.child(STR("node")).child(STR("child1")).attribute(STR("attr1")).document_order() == 4);
	CHECK(doc.child(STR("node")).child(STR("child1")).attribute(STR("attr2")).document_order() == 5);
	CHECK(doc.child(STR("node")).child(STR("child2")).document_order() == 6);
	CHECK(doc.child(STR("node")).child(STR("child2")).attribute(STR("attr1")).document_order() == 7);
	CHECK(doc.child(STR("node")).child(STR("child2")).first_child().document_order() == 8);
}

TEST(xpath_allocator_many_pages)
{
	pugi::string_t query = STR("0");

	for (int i = 0; i < 128; ++i) query += STR("+string-length('abcdefgh')");

	CHECK_XPATH_NUMBER(xml_node(), query.c_str(), 1024);
}

TEST(xpath_allocator_large_page)
{
	pugi::string_t query;

	for (int i = 0; i < 1024; ++i) query += STR("abcdefgh");

	CHECK_XPATH_NUMBER(xml_node(), (STR("string-length('") + query + STR("')")).c_str(), 8192);
}

TEST_XML(xpath_sort_complex, "<node><child1 attr1='value1' attr2='value2'/><child2 attr1='value1'>test</child2></node>")
{
	// just some random union order, it should not matter probably?
	xpath_node_set ns = doc.child(STR("node")).select_nodes(STR("child1 | child2 | child1/@* | . | child2/@* | child2/text()"));

	ns.sort(false);
	xpath_node_set sorted = ns;

	ns.sort(true);
	xpath_node_set reverse_sorted = ns;

	doc.precompute_document_order();

	xpath_node_set_tester(sorted, "sorted order failed") % 2 % 3 % 4 % 5 % 6 % 7 % 8;
	xpath_node_set_tester(reverse_sorted, "reverse sorted order failed") % 8 % 7 % 6 % 5 % 4 % 3 % 2;
}

TEST_XML(xpath_sort_children, "<node><child><subchild id='1'/></child><child><subchild id='2'/></child></node>")
{
	xpath_node_set ns = doc.child(STR("node")).select_nodes(STR("child/subchild[@id=1] | child/subchild[@id=2]"));

	ns.sort(false);
	xpath_node_set sorted = ns;

	ns.sort(true);
	xpath_node_set reverse_sorted = ns;

	doc.precompute_document_order();

	xpath_node_set_tester(sorted, "sorted order failed") % 4 % 7;
	xpath_node_set_tester(reverse_sorted, "reverse sorted order failed") % 7 % 4;
}

TEST_XML(xpath_sort_attributes, "<node/>")
{
	xml_node n = doc.child(STR("node"));

	// we need to insert attributes manually since unsorted node sets are (always?) sorted via pointers because of remove_duplicates,
	// so we need to have different document and pointer order to cover all comparator cases
	n.append_attribute(STR("attr2"));
	n.append_attribute(STR("attr3"));
	n.insert_attribute_before(STR("attr1"), n.attribute(STR("attr2")));

	xpath_node_set ns = n.select_nodes(STR("@*"));

	ns.sort(true);
	xpath_node_set reverse_sorted = ns;

	ns.sort(false);
	xpath_node_set sorted = ns;

	doc.precompute_document_order();

	xpath_node_set_tester(sorted, "sorted order failed") % 3 % 4 % 5;
	xpath_node_set_tester(reverse_sorted, "reverse sorted order failed") % 5 % 4 % 3;
}

TEST(xpath_long_numbers_parse)
{
	const pugi::char_t* str_flt_max = STR("340282346638528860000000000000000000000");
	const pugi::char_t* str_flt_max_dec = STR("340282346638528860000000000000000000000.000000");
	
	const pugi::char_t* str_dbl_max = STR("179769313486231570000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	const pugi::char_t* str_dbl_max_dec = STR("179769313486231570000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000");

	xml_node c;

	// check parsing
	CHECK_XPATH_NUMBER(c, str_flt_max, FLT_MAX);
	CHECK_XPATH_NUMBER(c, str_flt_max_dec, FLT_MAX);
	CHECK_XPATH_NUMBER(c, str_dbl_max, DBL_MAX);
	CHECK_XPATH_NUMBER(c, str_dbl_max_dec, DBL_MAX);
}

static bool test_xpath_string_prefix(const pugi::xml_node& node, const pugi::char_t* query, const pugi::char_t* expected, size_t match_length)
{
#ifdef PUGIXML_WCHAR_MODE
	size_t expected_length = wcslen(expected);
#else
	size_t expected_length = strlen(expected);
#endif

	pugi::xpath_query q(query);
	pugi::string_t value = q.evaluate_string(node);

	return value.length() == expected_length && value.compare(0, match_length, expected, match_length) == 0;
}

TEST(xpath_long_numbers_stringize)
{
	const pugi::char_t* str_flt_max = STR("340282346638528860000000000000000000000");
	const pugi::char_t* str_flt_max_dec = STR("340282346638528860000000000000000000000.000000");
	
	const pugi::char_t* str_dbl_max = STR("179769313486231570000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
	const pugi::char_t* str_dbl_max_dec = STR("179769313486231570000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.000000");

	xml_node c;

	CHECK(test_xpath_string_prefix(c, str_flt_max, str_flt_max, 16));
	CHECK(test_xpath_string_prefix(c, str_flt_max_dec, str_flt_max, 16));

#ifndef __BORLANDC__ // printf with %f format still results in 1.xxxe+308 form
	CHECK(test_xpath_string_prefix(c, str_dbl_max, str_dbl_max, 16));
	CHECK(test_xpath_string_prefix(c, str_dbl_max_dec, str_dbl_max, 16));
#endif
}

#endif
