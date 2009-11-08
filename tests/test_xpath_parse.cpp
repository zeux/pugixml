#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

TEST(xpath_literal_parse)
{
	xml_node c;
	CHECK_XPATH_STRING(c, "'a\"b'", "a\"b");
	CHECK_XPATH_STRING(c, "\"a'b\"", "a'b");
	CHECK_XPATH_STRING(c, "\"\"", "");
	CHECK_XPATH_STRING(c, "\'\'", "");
}

TEST(xpath_literal_error)
{
	CHECK_XPATH_FAIL("\"");
	CHECK_XPATH_FAIL("\'");
}

TEST(xpath_number_parse)
{
	xml_node c;
	CHECK_XPATH_NUMBER(c, "0", 0);
	CHECK_XPATH_NUMBER(c, "123", 123);
	CHECK_XPATH_NUMBER(c, "123.456", 123.456);
	CHECK_XPATH_NUMBER(c, ".123", 0.123);
	CHECK_XPATH_NUMBER(c, "123.4567890123456789012345", 123.4567890123456789012345);
}

TEST(xpath_number_error)
{
	CHECK_XPATH_FAIL("123a");
	CHECK_XPATH_FAIL("123.a");
	CHECK_XPATH_FAIL(".123a");
}

TEST(xpath_variables)
{
	CHECK_XPATH_FAIL("$var"); // not implemented
	CHECK_XPATH_FAIL("$1");
}

TEST(xpath_empty_expression)
{
	CHECK_XPATH_FAIL("");
}

TEST(xpath_lexer_error)
{
	CHECK_XPATH_FAIL("!");
}

TEST(xpath_unmatched_braces)
{
	CHECK_XPATH_FAIL("node[");
	CHECK_XPATH_FAIL("node[1");
	CHECK_XPATH_FAIL("node[]]");
	CHECK_XPATH_FAIL("node(");
	CHECK_XPATH_FAIL("node(()");
	CHECK_XPATH_FAIL("(node)[1");
	CHECK_XPATH_FAIL("(1");
}

TEST(xpath_incorrect_step)
{
	CHECK_XPATH_FAIL("child::1");
	CHECK_XPATH_FAIL("something::*");
}

TEST(xpath_semantics_error)
{
	CHECK_XPATH_FAIL("1[1]");
	CHECK_XPATH_FAIL("1 | 1");
}

TEST(xpath_semantics_posinv) // coverage for contains()
{
	xpath_query("(node)[substring(1, 2, 3)]");
	xpath_query("(node)[concat(1, 2, 3, 4)]");
	xpath_query("(node)[count(foo)]");
	xpath_query("(node)[local-name()]");
	xpath_query("(node)[(node)[1]]");
}

#endif
