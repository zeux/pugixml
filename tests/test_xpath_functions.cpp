#include "common.hpp"

TEST_XML(xpath_number_number, "<node>123</node>")
{
	xml_node c;
	xml_node n = doc.child("node").first_child();
	
	// number with 0 arguments
	CHECK_XPATH_NUMBER_NAN(c, "number()");
	CHECK_XPATH_NUMBER(n, "number()", 123);

	// number with 1 string argument
	CHECK_XPATH_NUMBER(c, "number(' -123.456 ')", -123.456);
	CHECK_XPATH_NUMBER(c, "number(' -123.')", -123);
	CHECK_XPATH_NUMBER(c, "number('123.')", 123);
	CHECK_XPATH_NUMBER(c, "number('.56')", 0.56);
	CHECK_XPATH_NUMBER_NAN(c, "number('foobar')");
	CHECK_XPATH_NUMBER_NAN(c, "number('f1')");
	CHECK_XPATH_NUMBER_NAN(c, "number('1f')");
	CHECK_XPATH_NUMBER_NAN(c, "number('1.f')");
	CHECK_XPATH_NUMBER_NAN(c, "number('1.0f')");

	// number with 1 bool argument
	CHECK_XPATH_NUMBER(c, "number(true())", 1);
	CHECK_XPATH_NUMBER(c, "number(false())", 0);

	// number with 1 node set argument
	CHECK_XPATH_NUMBER(n, "number(.)", 123);

	// number with 1 number argument
	CHECK_XPATH_NUMBER(c, "number(1)", 1);
	
	// number with 2 arguments
	CHECK_XPATH_FAIL("number(1, 2)");
}

TEST_XML(xpath_number_sum, "<node>123<child>789</child></node><node/>")
{
	xml_node c;
	xml_node n = doc.child("node");
	
	// sum with 0 arguments
	CHECK_XPATH_FAIL("sum()");

	// sum with 1 argument
	CHECK_XPATH_NUMBER(c, "sum(.)", 0);
	CHECK_XPATH_NUMBER(n, "sum(.)", 123789); // 123 .. 789
	
	CHECK_XPATH_NUMBER(n, "sum(./descendant-or-self::node())", 125490); // node + 123 + child + 789 = 123789 + 123 + 789 + 789 = 125490
	CHECK_XPATH_NUMBER(n, "sum(.//node())", 1701); // 123 + child + 789 = 123 + 789 + 789
	CHECK_XPATH_NUMBER_NAN(doc.last_child(), "sum(.)");

	// sum with 2 arguments
	CHECK_XPATH_FAIL("sum(1, 2)");
	
	// sum with 1 non-node-set argument
	CHECK_XPATH_FAIL("sum(1)");
}

TEST(xpath_number_floor)
{
	xml_node c;

	// floor with 0 arguments
	CHECK_XPATH_FAIL("floor()");

	// floor with 1 argument
	CHECK_XPATH_NUMBER(c, "floor(1.2)", 1);
	CHECK_XPATH_NUMBER(c, "floor(1)", 1);
	CHECK_XPATH_NUMBER(c, "floor(-1.2)", -2);
	CHECK_XPATH_NUMBER_NAN(c, "floor(string('nan'))");
	CHECK_XPATH_STRING(c, "string(floor(1 div 0))", "Infinity");
	CHECK_XPATH_STRING(c, "string(floor(-1 div 0))", "-Infinity");

	// floor with 2 arguments
	CHECK_XPATH_FAIL("floor(1, 2)");
}

TEST(xpath_number_ceiling)
{
	xml_node c;

	// ceiling with 0 arguments
	CHECK_XPATH_FAIL("ceiling()");

	// ceiling with 1 argument
	CHECK_XPATH_NUMBER(c, "ceiling(1.2)", 2);
	CHECK_XPATH_NUMBER(c, "ceiling(1)", 1);
	CHECK_XPATH_NUMBER(c, "ceiling(-1.2)", -1);
	CHECK_XPATH_NUMBER_NAN(c, "ceiling(string('nan'))");
	CHECK_XPATH_STRING(c, "string(ceiling(1 div 0))", "Infinity");
	CHECK_XPATH_STRING(c, "string(ceiling(-1 div 0))", "-Infinity");

	// ceiling with 2 arguments
	CHECK_XPATH_FAIL("ceiling(1, 2)");
}

TEST(xpath_number_round)
{
	xml_node c;

	// round with 0 arguments
	CHECK_XPATH_FAIL("round()");

	// round with 1 argument
	CHECK_XPATH_NUMBER(c, "round(1.2)", 1);
	CHECK_XPATH_NUMBER(c, "round(1.5)", 2);
	CHECK_XPATH_NUMBER(c, "round(1.8)", 2);
	CHECK_XPATH_NUMBER(c, "round(1)", 1);
	CHECK_XPATH_NUMBER(c, "round(-1.2)", -1);
	CHECK_XPATH_NUMBER(c, "round(-1.5)", -1);
	CHECK_XPATH_NUMBER(c, "round(-1.6)", -2);
	CHECK_XPATH_NUMBER_NAN(c, "round(string('nan'))");
	CHECK_XPATH_STRING(c, "string(round(1 div 0))", "Infinity");
	CHECK_XPATH_STRING(c, "string(round(-1 div 0))", "-Infinity");

	// round with 2 arguments
	CHECK_XPATH_FAIL("round(1, 2)");

	// round with negative zero results
	// $$ CHECK_XPATH_NUMBER(c, "round(-0.3)", -0)
	// $$ CHECK_XPATH_NUMBER(c, "round(-0)", -0)
}

TEST_XML(xpath_boolean_boolean, "<node />")
{
	xml_node c;
	
	// boolean with 0 arguments
	CHECK_XPATH_FAIL("boolean()");

	// boolean with 1 number argument
	CHECK_XPATH_BOOLEAN(c, "boolean(0)", false);
	CHECK_XPATH_BOOLEAN(c, "boolean(1)", true);
	CHECK_XPATH_BOOLEAN(c, "boolean(-1)", true);
	CHECK_XPATH_BOOLEAN(c, "boolean(0.1)", true);
	CHECK_XPATH_BOOLEAN(c, "boolean(number('nan'))", false);

	// boolean with 1 string argument
	CHECK_XPATH_BOOLEAN(c, "boolean('x')", true);
	CHECK_XPATH_BOOLEAN(c, "boolean('')", false);

	// boolean with 1 node set argument
	CHECK_XPATH_BOOLEAN(c, "boolean(.)", false);
	CHECK_XPATH_BOOLEAN(doc, "boolean(.)", true);
	CHECK_XPATH_BOOLEAN(doc, "boolean(foo)", false);

	// boolean with 2 arguments
	CHECK_XPATH_FAIL("boolean(1, 2)");
}

TEST(xpath_boolean_not)
{
	xml_node c;
	
	// not with 0 arguments
	CHECK_XPATH_FAIL("not()");

	// not with 1 argument
	CHECK_XPATH_BOOLEAN(c, "not(true())", false);
	CHECK_XPATH_BOOLEAN(c, "not(false())", true);
	
	// boolean with 2 arguments
	CHECK_XPATH_FAIL("not(1, 2)");
}

TEST(xpath_boolean_true)
{
	xml_node c;
	
	// true with 0 arguments
	CHECK_XPATH_BOOLEAN(c, "true()", true);

	// true with 1 argument
	CHECK_XPATH_FAIL("true(1)");
}

TEST(xpath_boolean_false)
{
	xml_node c;
	
	// false with 0 arguments
	CHECK_XPATH_BOOLEAN(c, "false()", false);

	// false with 1 argument
	CHECK_XPATH_FAIL("false(1)");
}

TEST_XML(xpath_boolean_lang, "<node xml:lang='en'><child xml:lang='ru-UK'><subchild/></child></node><foo><bar/></foo>")
{
	xml_node c;
	
	// lang with 0 arguments
	CHECK_XPATH_FAIL("lang()");

	// lang with 1 argument, no language
	CHECK_XPATH_BOOLEAN(c, "lang('en')", false);
	CHECK_XPATH_BOOLEAN(doc.child("foo"), "lang('en')", false);
	CHECK_XPATH_BOOLEAN(doc.child("foo"), "lang('')", false);
	CHECK_XPATH_BOOLEAN(doc.child("foo").child("bar"), "lang('en')", false);
	
	// lang with 1 argument, same language/prefix
	CHECK_XPATH_BOOLEAN(doc.child("node"), "lang('en')", true);
	CHECK_XPATH_BOOLEAN(doc.child("node").child("child"), "lang('ru-uk')", true);
	CHECK_XPATH_BOOLEAN(doc.child("node").child("child"), "lang('ru')", true);
	CHECK_XPATH_BOOLEAN(doc.child("node").child("child").child("subchild"), "lang('ru')", true);
	CHECK_XPATH_BOOLEAN(doc.child("node").child("child").child("subchild"), "lang('RU')", true);

	// lang with 1 argument, different language/prefix
	CHECK_XPATH_BOOLEAN(doc.child("node"), "lang('')", false);
	CHECK_XPATH_BOOLEAN(doc.child("node"), "lang('e')", false);
	CHECK_XPATH_BOOLEAN(doc.child("node").child("child"), "lang('en')", false);
	CHECK_XPATH_BOOLEAN(doc.child("node").child("child"), "lang('ru-gb')", false);
	CHECK_XPATH_BOOLEAN(doc.child("node").child("child"), "lang('r')", false);
	CHECK_XPATH_BOOLEAN(doc.child("node").child("child").child("subchild"), "lang('en')", false);

	// lang with 2 arguments
	CHECK_XPATH_FAIL("lang(1, 2)");
}

TEST(xpath_string_concat)
{
	xml_node c;

	// concat with 0 arguments
	CHECK_XPATH_FAIL("concat()");

	// concat with 1 argument
	CHECK_XPATH_FAIL("concat('')");

	// concat with exactly 2 arguments
	CHECK_XPATH_STRING(c, "concat('prev','next')", "prevnext");
	CHECK_XPATH_STRING(c, "concat('','next')", "next");
	CHECK_XPATH_STRING(c, "concat('prev','')", "prev");

	// concat with 3 or more arguments
	CHECK_XPATH_STRING(c, "concat('a', 'b', 'c')", "abc");
	CHECK_XPATH_STRING(c, "concat('a', 'b', 'c', 'd')", "abcd");
	CHECK_XPATH_STRING(c, "concat('a', 'b', 'c', 'd', 'e')", "abcde");
	CHECK_XPATH_STRING(c, "concat('a', 'b', 'c', 'd', 'e', 'f')", "abcdef");
	CHECK_XPATH_STRING(c, "concat('a', 'b', 'c', 'd', 'e', 'f', 'g')", "abcdefg");
}

// $$$: string value of <node>123<child>789</child>100</node> should be 123789100 (?)
