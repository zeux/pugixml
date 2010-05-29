#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

TEST(xpath_literal_parse)
{
	xml_node c;
	CHECK_XPATH_STRING(c, STR("'a\"b'"), STR("a\"b"));
	CHECK_XPATH_STRING(c, STR("\"a'b\""), STR("a'b"));
	CHECK_XPATH_STRING(c, STR("\"\""), STR(""));
	CHECK_XPATH_STRING(c, STR("\'\'"), STR(""));
}

TEST(xpath_literal_error)
{
	CHECK_XPATH_FAIL(STR("\""));
	CHECK_XPATH_FAIL(STR("\"foo"));
	CHECK_XPATH_FAIL(STR("\'"));
	CHECK_XPATH_FAIL(STR("\'bar"));
}

TEST(xpath_number_parse)
{
	xml_node c;
	CHECK_XPATH_NUMBER(c, STR("0"), 0);
	CHECK_XPATH_NUMBER(c, STR("123"), 123);
	CHECK_XPATH_NUMBER(c, STR("123.456"), 123.456);
	CHECK_XPATH_NUMBER(c, STR(".123"), 0.123);
	CHECK_XPATH_NUMBER(c, STR("123.4567890123456789012345"), 123.4567890123456789012345);
}

TEST(xpath_number_error)
{
	CHECK_XPATH_FAIL(STR("123a"));
	CHECK_XPATH_FAIL(STR("123.a"));
	CHECK_XPATH_FAIL(STR(".123a"));
}

TEST(xpath_variables)
{
	CHECK_XPATH_FAIL(STR("$var")); // not implemented
	CHECK_XPATH_FAIL(STR("$1"));
}

TEST(xpath_empty_expression)
{
	CHECK_XPATH_FAIL(STR(""));
}

TEST(xpath_lexer_error)
{
	CHECK_XPATH_FAIL(STR("!"));
	CHECK_XPATH_FAIL(STR("&"));
}

TEST(xpath_unmatched_braces)
{
	CHECK_XPATH_FAIL(STR("node["));
	CHECK_XPATH_FAIL(STR("node[1"));
	CHECK_XPATH_FAIL(STR("node[]]"));
	CHECK_XPATH_FAIL(STR("node("));
	CHECK_XPATH_FAIL(STR("node(()"));
	CHECK_XPATH_FAIL(STR("(node)[1"));
	CHECK_XPATH_FAIL(STR("(1"));
}

TEST(xpath_incorrect_step)
{
	CHECK_XPATH_FAIL(STR("child::1"));
	CHECK_XPATH_FAIL(STR("something::*"));
	CHECK_XPATH_FAIL(STR("a::*"));
	CHECK_XPATH_FAIL(STR("c::*"));
	CHECK_XPATH_FAIL(STR("d::*"));
	CHECK_XPATH_FAIL(STR("f::*"));
	CHECK_XPATH_FAIL(STR("n::*"));
	CHECK_XPATH_FAIL(STR("p::*"));
}

TEST(xpath_semantics_error)
{
	CHECK_XPATH_FAIL(STR("1[1]"));
	CHECK_XPATH_FAIL(STR("1 | 1"));
}

TEST(xpath_semantics_posinv) // coverage for contains()
{
	xpath_query(STR("(node)[substring(1, 2, 3)]"));
	xpath_query(STR("(node)[concat(1, 2, 3, 4)]"));
	xpath_query(STR("(node)[count(foo)]"));
	xpath_query(STR("(node)[local-name()]"));
	xpath_query(STR("(node)[(node)[1]]"));
}

#endif
