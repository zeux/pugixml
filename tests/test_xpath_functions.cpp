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

TEST_XML(xpath_string_string, "<node>123<child id='1'>789</child><child><![CDATA[200]]></child>100</node>")
{
	xml_node c;
	xml_node n = doc.child("node");

	// string with 0 arguments
	CHECK_XPATH_STRING(c, "string()", "");
	CHECK_XPATH_STRING(n.child("child"), "string()", "789");

	// string with 1 node-set argument
	CHECK_XPATH_STRING(n, "string(child)", "789");
	CHECK_XPATH_STRING(n, "string(child/@id)", "1");
	CHECK_XPATH_STRING(n, "string(.)", "123789200100");

	// string with 1 number argument
	CHECK_XPATH_STRING(c, "string(0 div 0)", "NaN");
	CHECK_XPATH_STRING(c, "string(0)", "0");
	CHECK_XPATH_STRING(c, "string(-0)", "0");
	CHECK_XPATH_STRING(c, "string(1 div 0)", "Infinity");
	CHECK_XPATH_STRING(c, "string(-1 div 0)", "-Infinity");
	CHECK_XPATH_STRING(c, "string(1234567)", "1234567");
	CHECK_XPATH_STRING(c, "string(-1234567)", "-1234567");
	CHECK_XPATH_STRING(c, "string(1234.5678)", "1234.5678");
	CHECK_XPATH_STRING(c, "string(-1234.5678)", "-1234.5678");
	CHECK_XPATH_STRING(c, "string(0.5678)", "0.5678");
	CHECK_XPATH_STRING(c, "string(-0.5678)", "-0.5678");
	CHECK_XPATH_STRING(c, "string(0.0)", "0");
	CHECK_XPATH_STRING(c, "string(-0.0)", "0");

	// string with 1 boolean argument
	CHECK_XPATH_STRING(c, "string(true())", "true");
	CHECK_XPATH_STRING(c, "string(false())", "false");

	// string with 1 string argument
	CHECK_XPATH_STRING(c, "string('abc')", "abc");

	// string with 2 arguments
	CHECK_XPATH_FAIL("string(1, 2)");
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

TEST(xpath_string_starts_with)
{
	xml_node c;

	// starts-with with 0 arguments
	CHECK_XPATH_FAIL("starts-with()");

	// starts-with with 1 argument
	CHECK_XPATH_FAIL("starts-with('a')");

	// starts-with with 2 arguments
	CHECK_XPATH_BOOLEAN(c, "starts-with('abc', '')", true);
	CHECK_XPATH_BOOLEAN(c, "starts-with('abc', 'a')", true);
	CHECK_XPATH_BOOLEAN(c, "starts-with('abc', 'abc')", true);
	CHECK_XPATH_BOOLEAN(c, "starts-with('abc', 'abcd')", false);
	CHECK_XPATH_BOOLEAN(c, "starts-with('bc', 'c')", false);
	CHECK_XPATH_BOOLEAN(c, "starts-with('', 'c')", false);
	CHECK_XPATH_BOOLEAN(c, "starts-with('', '')", true);

	// starts-with with 3 arguments
	CHECK_XPATH_FAIL("starts-with('a', 'b', 'c')");
}

TEST(xpath_string_contains)
{
	xml_node c;

	// contains with 0 arguments
	CHECK_XPATH_FAIL("contains()");

	// contains with 1 argument
	CHECK_XPATH_FAIL("contains('a')");

	// contains with 2 arguments
	CHECK_XPATH_BOOLEAN(c, "contains('abc', '')", true);
	CHECK_XPATH_BOOLEAN(c, "contains('abc', 'a')", true);
	CHECK_XPATH_BOOLEAN(c, "contains('abc', 'abc')", true);
	CHECK_XPATH_BOOLEAN(c, "contains('abcd', 'bc')", true);
	CHECK_XPATH_BOOLEAN(c, "contains('abc', 'abcd')", false);
	CHECK_XPATH_BOOLEAN(c, "contains('b', 'bc')", false);
	CHECK_XPATH_BOOLEAN(c, "contains('', 'c')", false);
	CHECK_XPATH_BOOLEAN(c, "contains('', '')", true);

	// contains with 3 arguments
	CHECK_XPATH_FAIL("contains('a', 'b', 'c')");
}

TEST(xpath_string_substring_before)
{
	xml_node c;

	// substring-before with 0 arguments
	CHECK_XPATH_FAIL("substring-before()");

	// substring-before with 1 argument
	CHECK_XPATH_FAIL("substring-before('a')");
	
	// substring-before with 2 arguments
	CHECK_XPATH_STRING(c, "substring-before('abc', 'abc')", "");
	CHECK_XPATH_STRING(c, "substring-before('abc', 'a')", "");
	CHECK_XPATH_STRING(c, "substring-before('abc', 'cd')", "");
	CHECK_XPATH_STRING(c, "substring-before('abc', 'b')", "a");
	CHECK_XPATH_STRING(c, "substring-before('abc', 'c')", "ab");
	CHECK_XPATH_STRING(c, "substring-before('', '')", "");
	
	// substring-before with 2 arguments, from W3C standard
	CHECK_XPATH_STRING(c, "substring-before(\"1999/04/01\",\"/\")", "1999");

	// substring-before with 3 arguments
	CHECK_XPATH_FAIL("substring-before('a', 'b', 'c')");
}

TEST(xpath_string_substring_after)
{
	xml_node c;

	// substring-after with 0 arguments
	CHECK_XPATH_FAIL("substring-after()");

	// substring-after with 1 argument
	CHECK_XPATH_FAIL("substring-after('a')");
	
	// substring-after with 2 arguments
	CHECK_XPATH_STRING(c, "substring-after('abc', 'abc')", "");
	CHECK_XPATH_STRING(c, "substring-after('abc', 'a')", "bc");
	CHECK_XPATH_STRING(c, "substring-after('abc', 'cd')", "");
	CHECK_XPATH_STRING(c, "substring-after('abc', 'b')", "c");
	CHECK_XPATH_STRING(c, "substring-after('abc', 'c')", "");
	CHECK_XPATH_STRING(c, "substring-after('', '')", "");

	// substring-before with 2 arguments, from W3C standard
	CHECK_XPATH_STRING(c, "substring-after(\"1999/04/01\",\"/\")", "04/01");
	CHECK_XPATH_STRING(c, "substring-after(\"1999/04/01\",\"19\")", "99/04/01");

	// substring-after with 3 arguments
	CHECK_XPATH_FAIL("substring-after('a', 'b', 'c')");
}

TEST(xpath_string_substring)
{
	xml_node c;

	// substring with 0 arguments
	CHECK_XPATH_FAIL("substring()");
	
	// substring with 1 argument
	CHECK_XPATH_FAIL("substring('')");
	
	// substring with 2 arguments
	CHECK_XPATH_STRING(c, "substring('abcd', 2)", "bcd");
	CHECK_XPATH_STRING(c, "substring('abcd', 1)", "abcd");
	CHECK_XPATH_STRING(c, "substring('abcd', 1.1)", "abcd");
	CHECK_XPATH_STRING(c, "substring('abcd', 1.5)", "bcd");
	CHECK_XPATH_STRING(c, "substring('abcd', 1.8)", "bcd");
	CHECK_XPATH_STRING(c, "substring('abcd', 10)", "");
	CHECK_XPATH_STRING(c, "substring('abcd', 0)", "abcd");
	CHECK_XPATH_STRING(c, "substring('abcd', -100)", "abcd");
	CHECK_XPATH_STRING(c, "substring('abcd', -1 div 0)", "abcd");
	CHECK_XPATH_STRING(c, "substring('abcd', 1 div 0)", "");
	CHECK_XPATH_STRING(c, "substring('abcd', 0 div 0)", "");
	CHECK_XPATH_STRING(c, "substring('', 1)", "");
	CHECK_XPATH_STRING(c, "substring('', 0)", "");

	// substring with 3 arguments
	CHECK_XPATH_STRING(c, "substring('abcd', 2, 1)", "b");
	CHECK_XPATH_STRING(c, "substring('abcd', 2, 2)", "bc");
	CHECK_XPATH_STRING(c, "substring('abcd', 1, 0)", "");
	CHECK_XPATH_STRING(c, "substring('abcd', 1, 0.4)", "");
	CHECK_XPATH_STRING(c, "substring('abcd', 1, 0.5)", "a");
	CHECK_XPATH_STRING(c, "substring('abcd', 10, -5)", "");
	CHECK_XPATH_STRING(c, "substring('abcd', 0, -1)", "");
	CHECK_XPATH_STRING(c, "substring('abcd', -100, 100)", "abcd");
	CHECK_XPATH_STRING(c, "substring('abcd', -1 div 0, 4)", "");
	CHECK_XPATH_STRING(c, "substring('abcd', 1 div 0, 0 div 0)", "");
	CHECK_XPATH_STRING(c, "substring('abcd', 0 div 0, 1)", "");
	CHECK_XPATH_STRING(c, "substring('', 1, 2)", "");
	CHECK_XPATH_STRING(c, "substring('', 0, 0)", "");

	// substring with 3 arguments, from W3C standard
	CHECK_XPATH_STRING(c, "substring('12345', 1.5, 2.6)", "234");
	CHECK_XPATH_STRING(c, "substring('12345', 0, 3)", "12");
	CHECK_XPATH_STRING(c, "substring('12345', 0 div 0, 3)", "");
	CHECK_XPATH_STRING(c, "substring('12345', 1, 0 div 0)", "");
	CHECK_XPATH_STRING(c, "substring('12345', -42, 1 div 0)", "12345");
	CHECK_XPATH_STRING(c, "substring('12345', -1 div 0, 1 div 0)", "");

	// substring with 4 arguments
	CHECK_XPATH_FAIL("substring('', 1, 2, 3)");
}

TEST_XML(xpath_string_string_length, "<node>123</node>")
{
	xml_node c;
	xml_node n = doc.child("node");

	// string-length with 0 arguments
	CHECK_XPATH_NUMBER(c, "string-length()", 0);
	CHECK_XPATH_NUMBER(n, "string-length()", 3);

	// string-length with 1 argument
	CHECK_XPATH_NUMBER(c, "string-length('')", 0);
	CHECK_XPATH_NUMBER(c, "string-length('a')", 1);
	CHECK_XPATH_NUMBER(c, "string-length('abcdef')", 6);

	// string-length with 2 arguments
	CHECK_XPATH_FAIL("string-length(1, 2)");
}

TEST_XML_FLAGS(xpath_string_normalize_space, "<node> \t\r\rval1  \rval2\r\nval3\nval4\r\r</node>", parse_minimal)
{
	xml_node c;
	xml_node n = doc.child("node");

	// normalize-space with 0 arguments
	CHECK_XPATH_STRING(c, "normalize-space()", "");
	CHECK_XPATH_STRING(n, "normalize-space()", "val1 val2 val3 val4");
	
	// normalize-space with 1 argument
	CHECK_XPATH_STRING(c, "normalize-space('')", "");
	CHECK_XPATH_STRING(c, "normalize-space('abcd')", "abcd");
	CHECK_XPATH_STRING(c, "normalize-space(' \r\nabcd')", "abcd");
	CHECK_XPATH_STRING(c, "normalize-space('abcd \n\r')", "abcd");
	CHECK_XPATH_STRING(c, "normalize-space('ab\r\n\tcd')", "ab cd");
	CHECK_XPATH_STRING(c, "normalize-space('ab    cd')", "ab cd");
	
	// normalize-space with 2 arguments
	CHECK_XPATH_FAIL("normalize-space(1, 2)");
}

TEST(xpath_string_translate)
{
	xml_node c;

	// translate with 0 arguments
	CHECK_XPATH_FAIL("translate()");
	
	// translate with 1 argument
	CHECK_XPATH_FAIL("translate('a')");

	// translate with 2 arguments
	CHECK_XPATH_FAIL("translate('a', 'b')");
	
	// translate with 3 arguments
	CHECK_XPATH_STRING(c, "translate('abc', '', '')", "abc");
	CHECK_XPATH_STRING(c, "translate('abc', '', 'foo')", "abc");
	CHECK_XPATH_STRING(c, "translate('abc', 'ab', 'ba')", "bac");
	CHECK_XPATH_STRING(c, "translate('abc', 'ab', 'f')", "fc");
	CHECK_XPATH_STRING(c, "translate('abc', 'aabb', '1234')", "13c");
	CHECK_XPATH_STRING(c, "translate('', 'abc', 'bac')", "");

	// translate with 3 arguments, from W3C standard
	CHECK_XPATH_STRING(c, "translate('bar','abc','ABC')", "BAr");
	CHECK_XPATH_STRING(c, "translate('--aaa--','abc-','ABC')", "AAA");

	// translate with 4 arguments
	CHECK_XPATH_FAIL("translate('a', 'b', 'c', 'd')");
}
