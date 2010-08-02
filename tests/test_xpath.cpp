#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

#include <float.h>
#include <string.h>
#include <wchar.h>

#include <string>

static void load_document_copy(xml_document& doc, const char_t* text)
{
	xml_document source;
	CHECK(source.load(text));

	doc.append_copy(source.first_child());
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

	xpath_node_set_tester(sorted, "sorted order failed") % 2 % 3 % 4 % 5 % 6 % 7 % 8;
	xpath_node_set_tester(reverse_sorted, "reverse sorted order failed") % 8 % 7 % 6 % 5 % 4 % 3 % 2;
}

TEST(xpath_sort_complex_copy) // copy the document so that document order optimization does not work
{
	xml_document doc;
	load_document_copy(doc, STR("<node><child1 attr1='value1' attr2='value2'/><child2 attr1='value1'>test</child2></node>"));

	// just some random union order, it should not matter probably?
	xpath_node_set ns = doc.child(STR("node")).select_nodes(STR("child1 | child2 | child1/@* | . | child2/@* | child2/text()"));

	ns.sort(false);
	xpath_node_set sorted = ns;

	ns.sort(true);
	xpath_node_set reverse_sorted = ns;

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

	xpath_node_set_tester(sorted, "sorted order failed") % 4 % 7;
	xpath_node_set_tester(reverse_sorted, "reverse sorted order failed") % 7 % 4;
}

TEST(xpath_sort_children_copy) // copy the document so that document order optimization does not work
{
	xml_document doc;
	load_document_copy(doc, STR("<node><child><subchild id='1'/></child><child><subchild id='2'/></child></node>"));

	xpath_node_set ns = doc.child(STR("node")).select_nodes(STR("child/subchild[@id=1] | child/subchild[@id=2]"));

	ns.sort(false);
	xpath_node_set sorted = ns;

	ns.sort(true);
	xpath_node_set reverse_sorted = ns;

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

	CHECK(test_xpath_string_prefix(c, str_flt_max, str_flt_max, 15));
	CHECK(test_xpath_string_prefix(c, str_flt_max_dec, str_flt_max, 15));

	CHECK(test_xpath_string_prefix(c, str_dbl_max, str_dbl_max, 15));
	CHECK(test_xpath_string_prefix(c, str_dbl_max_dec, str_dbl_max, 15));
}

#include <stdio.h>

TEST(xpath_denorm_numbers)
{
	pugi::string_t query;

	// 10^-318 - double denormal
	for (int i = 0; i < 106; ++i)
	{
		if (i != 0) query += STR(" * ");
		query += STR("0.001");
	}

	CHECK_XPATH_STRING(xml_node(), query.c_str(), STR("0.0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000009999987484955998"));
}

TEST_XML(xpath_rexml_1, "<a><b><c id='a'/></b><c id='b'/></a>")
{
	CHECK_XPATH_NODESET(doc, STR("//*[local-name()='c' and @id='b']")) % 6;
	CHECK_XPATH_NODESET(doc, STR("//*[ local-name()='c' and @id='b' ]")) % 6;
	CHECK_XPATH_NODESET(doc, STR("/a/c[@id]")) % 6;
	CHECK_XPATH_NODESET(doc, STR("/a/c[(@id)]")) % 6;
	CHECK_XPATH_NODESET(doc, STR("/a/c[ @id ]")) % 6;
	CHECK_XPATH_NODESET(doc, STR("/a/c[ (@id) ]")) % 6;
	CHECK_XPATH_NODESET(doc, STR("/a/c[( @id )]")) % 6;
	CHECK_XPATH_NODESET(doc, STR("/a/c[ ( @id ) ]")) % 6;
	CHECK_XPATH_NODESET(doc, STR("/a/c [ ( @id ) ] ")) % 6;
	CHECK_XPATH_NODESET(doc, STR(" / a / c [ ( @id ) ] ")) % 6;
}

TEST_XML(xpath_rexml_2, "<a:x xmlns:a='1'><a:y p='p' q='q'><a:z>zzz</a:z></a:y></a:x>")
{
	CHECK_XPATH_NODESET(doc, STR("a:x/a:y[@p='p' and @q='q']/a:z/text()")) % 8;
}

TEST_XML(xpath_rexml_3, "<article><section role='subdivision' id='1'><para>free flowing text.</para></section><section role='division'><section role='subdivision' id='2'><para>free flowing text.</para></section><section role='division'><para>free flowing text.</para></section></section></article>")
{
	CHECK_XPATH_NODESET(doc, STR("//section[../self::section[@role=\"division\"]]")) % 10 % 15;
    CHECK_XPATH_NODESET(doc, STR("//section[@role=\"subdivision\" and not(../self::section[@role=\"division\"])]")) % 3;
	CHECK_XPATH_NODESET(doc, STR("//section[@role=\"subdivision\"][not(../self::section[@role=\"division\"])]")) % 3;
}

TEST_XML_FLAGS(xpath_rexml_4, "<a><b number='1' str='abc'>TEXT1</b><c number='1'/><c number='2' str='def'><b number='3'/><d number='1' str='abc'>TEXT2</d><b number='2'><!--COMMENT--></b></c></a>", parse_default | parse_comments)
{
	CHECK_XPATH_NODESET(doc, STR("/descendant-or-self::node()[count(child::node()|following-sibling::node()|preceding-sibling::node())=0]")) % 6 % 17 % 20;
}

TEST_XML(xpath_rexml_5, "<a><b><c id='a'/></b><c id='b'/></a>")
{
	CHECK_XPATH_FAIL(STR(".//[@id]"));
	CHECK_XPATH_NODESET(doc, STR(".//self::*[@id]")) % 4 % 6;
	CHECK_XPATH_NODESET(doc, STR(".//node()[@id]")) % 4 % 6;
}

TEST_XML(xpath_rexml_6, "<div><span><strong>a</strong></span><em>b</em></div>")
{
	CHECK_XPATH_NODESET(doc, STR("//em|//strong")) % 4 % 6;
	CHECK_XPATH_NODESET(doc, STR("//*[self::em | self::strong]")) % 4 % 6;
	CHECK_XPATH_NODESET(doc, STR("//*[name()=\"em\" or name()=\"strong\"]")) % 4 % 6;
	CHECK_XPATH_NODESET(doc, STR("//*[self::em or self::strong]")) % 4 % 6;
}

TEST_XML(xpath_xsl_list_1, "<input><type>whatever</type></input><input><type>text</type></input><input><type>select</type></input><input><type>something</type></input>")
{
	// if I'm not last, and the next input/type isn't select
	CHECK_XPATH_NODESET(doc, STR("input[type[parent::input/following-sibling::input[1]/type != 'select']]")) % 2 % 8;
	CHECK_XPATH_NODESET(doc, STR("input[type[../following-sibling::input[1]/type != 'select']]")) % 2 % 8;

	CHECK_XPATH_NODESET(doc, STR("input[position()+1]"));
}

TEST_XML(xpath_xsl_list_2, "<TR><TD id='1'>text1</TD><TD id='2'>text2</TD><TD id='3'>text3</TD><TD id='4'>text4</TD></TR>")
{
	CHECK_XPATH_FAIL(STR(".[not(.=ancestor::TR/TD[15]/node())]"));

	CHECK_XPATH_NODESET(doc.child(STR("TR")).find_child_by_attribute(STR("TD"), STR("id"), STR("1")).first_child(), STR("self::node()[not(.=ancestor::TR/TD[3]/node())]")) % 5;
	CHECK_XPATH_NODESET(doc.child(STR("TR")).find_child_by_attribute(STR("TD"), STR("id"), STR("2")).first_child(), STR("self::node()[not(.=ancestor::TR/TD[3]/node())]")) % 8;
	CHECK_XPATH_NODESET(doc.child(STR("TR")).find_child_by_attribute(STR("TD"), STR("id"), STR("3")).first_child(), STR("self::node()[not(.=ancestor::TR/TD[3]/node())]"));
	CHECK_XPATH_NODESET(doc.child(STR("TR")).find_child_by_attribute(STR("TD"), STR("id"), STR("4")).first_child(), STR("self::node()[not(.=ancestor::TR/TD[3]/node())]")) % 14;

	CHECK_XPATH_NODESET(doc.child(STR("TR")).find_child_by_attribute(STR("TD"), STR("id"), STR("1")), STR("node()[not(.=ancestor::TR/TD[3]/node())]")) % 5;
	CHECK_XPATH_NODESET(doc.child(STR("TR")).find_child_by_attribute(STR("TD"), STR("id"), STR("2")), STR("node()[not(.=ancestor::TR/TD[3]/node())]")) % 8;
	CHECK_XPATH_NODESET(doc.child(STR("TR")).find_child_by_attribute(STR("TD"), STR("id"), STR("3")), STR("node()[not(.=ancestor::TR/TD[3]/node())]"));
	CHECK_XPATH_NODESET(doc.child(STR("TR")).find_child_by_attribute(STR("TD"), STR("id"), STR("4")), STR("node()[not(.=ancestor::TR/TD[3]/node())]")) % 14;
}

TEST_XML(xpath_star_token, "<node>0.5<section><child/><child/><child/><child/></section><section/></node>")
{
	CHECK_XPATH_NODESET(doc, STR("//*[/* * 4]")) % 6 % 9;
	CHECK_XPATH_NODESET(doc, STR("//*[/**4]")) % 6 % 9;
	CHECK_XPATH_FAIL(STR("//*[/***4]"));
}

TEST(xpath_miscellaneous)
{
	CHECK_XPATH_FAIL(STR("/root/child[a=3]/substring(child::text())"));
	CHECK_XPATH_NODESET(xml_node(), STR("foo/@FOO/@bar"));
}

TEST(xpath_document_order)
{
	CHECK(xml_attribute().document_order() == 0);
	CHECK(xml_node().document_order() == 0);
}
#endif
