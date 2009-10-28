#include "common.hpp"

TEST_XML(xpath_operators_arithmetic, "<node><foo-bar>10</foo-bar><foo>2</foo><bar>3</bar></node>")
{
	xml_node c;
	xml_node n = doc.child("node");

	// incorrect unary operator
	CHECK_XPATH_FAIL("-");

	// correct unary operator
	CHECK_XPATH_NUMBER(c, "-1", -1);
	CHECK_XPATH_NUMBER(c, "--1", 1);
	CHECK_XPATH_NUMBER(c, "---1", -1);

	// incorrect binary operators
	CHECK_XPATH_FAIL("5+");
	CHECK_XPATH_FAIL("5-");
	CHECK_XPATH_FAIL("5*");
	CHECK_XPATH_FAIL("+5");
	CHECK_XPATH_FAIL("*5");
	CHECK_XPATH_FAIL("1div2");
	CHECK_XPATH_FAIL("1mod");
	CHECK_XPATH_FAIL("1div");

	// correct trivial binary operators
	CHECK_XPATH_NUMBER(c, "1 + 2", 3);
	CHECK_XPATH_NUMBER(c, "1+2", 3);
	CHECK_XPATH_NUMBER(c, "1 * 2", 2);
	CHECK_XPATH_NUMBER(c, "1*2", 2);
	CHECK_XPATH_NUMBER(c, "1 div 2", 0.5);

	// operator precedence
	CHECK_XPATH_NUMBER(c, "2 + 2 * 2 div 1 mod 3", 3);
	CHECK_XPATH_NUMBER(c, "2 + 2 * 2 div (1 mod 3)", 6);
	CHECK_XPATH_NUMBER(c, "(2 + 2) * 2 div (1 mod 3)", 8);
	CHECK_XPATH_NUMBER(c, "(2 + 2) * (2 div 1) mod 3", 2);
	CHECK_XPATH_NUMBER(c, "2 - -2", 4);
	CHECK_XPATH_NUMBER(c, "2--2", 4);
	CHECK_XPATH_NUMBER(c, "1-2-3", -4);

	// infinity/nan
	CHECK_XPATH_STRING(c, "1 div 0", "Infinity");
	CHECK_XPATH_STRING(c, "-1 div 0", "-Infinity");
	CHECK_XPATH_STRING(c, "-1 div 0 + 1 div 0", "NaN");
	CHECK_XPATH_STRING(c, "0 div 0", "NaN");
	CHECK_XPATH_STRING(c, "1 div 0 + 1 div 0", "Infinity");
	CHECK_XPATH_STRING(c, "-1 div 0 + -1 div 0", "-Infinity");
	CHECK_XPATH_STRING(c, "1 div 0 + 100", "Infinity");
	CHECK_XPATH_STRING(c, "-1 div 0 + 100", "-Infinity");
	CHECK_XPATH_STRING(c, "0 div 0 + 100", "NaN");

	// mod, from W3C standard
	CHECK_XPATH_NUMBER(c, "5 mod 2", 1);
	CHECK_XPATH_NUMBER(c, "5 mod -2", 1);
	CHECK_XPATH_NUMBER(c, "-5 mod 2", -1);
	CHECK_XPATH_NUMBER(c, "-5 mod -2", -1);

	// correct subtraction parsing, from W3C standard
	CHECK_XPATH_NUMBER(n, "foo-bar", 10);
	CHECK_XPATH_NUMBER(n, "foo -bar", -1);
	CHECK_XPATH_NUMBER(n, "foo - bar", -1);
	CHECK_XPATH_NUMBER(n, "-foo-bar", -10);
	CHECK_XPATH_NUMBER(n, "-foo -bar", -5);
}

TEST(xpath_operators_logical)
{
	xml_node c;

	// boolean arithmetic
	CHECK_XPATH_BOOLEAN(c, "true() or true()", true);
	CHECK_XPATH_BOOLEAN(c, "true() or false()", true);
	CHECK_XPATH_BOOLEAN(c, "false() or false()", false);
	CHECK_XPATH_BOOLEAN(c, "false() or true()", true);

	CHECK_XPATH_BOOLEAN(c, "true() and true()", true);
	CHECK_XPATH_BOOLEAN(c, "true() and false()", false);
	CHECK_XPATH_BOOLEAN(c, "false() and false()", false);
	CHECK_XPATH_BOOLEAN(c, "false() and true()", false);
	
	// boolean conversion
	CHECK_XPATH_BOOLEAN(c, "1 or ''", true);
	CHECK_XPATH_BOOLEAN(c, "1 and ''", false);
	CHECK_XPATH_BOOLEAN(c, "0 or ''", false);
	CHECK_XPATH_BOOLEAN(c, "0 or 'a'", true);
}

TEST(xpath_operators_equality_primitive_boolean)
{
	xml_node c;

	// boolean vs boolan
	CHECK_XPATH_BOOLEAN(c, "true() = true()", true);
	CHECK_XPATH_BOOLEAN(c, "false() = false()", true);
	CHECK_XPATH_BOOLEAN(c, "true() != false()", true);
	CHECK_XPATH_BOOLEAN(c, "false() != false()", false);

	// upcast to boolean
	CHECK_XPATH_BOOLEAN(c, "true() = 2", true);
	CHECK_XPATH_BOOLEAN(c, "true() != 2", false);
	CHECK_XPATH_BOOLEAN(c, "false() = 2", false);
	CHECK_XPATH_BOOLEAN(c, "false() != 2", true);
	CHECK_XPATH_BOOLEAN(c, "false() = 0", true);
	CHECK_XPATH_BOOLEAN(c, "false() != 0", false);

	CHECK_XPATH_BOOLEAN(c, "2 = true()", true);
	CHECK_XPATH_BOOLEAN(c, "2 != true()", false);
	CHECK_XPATH_BOOLEAN(c, "2 = false()", false);
	CHECK_XPATH_BOOLEAN(c, "2 != false()", true);
	CHECK_XPATH_BOOLEAN(c, "0 = false()", true);
	CHECK_XPATH_BOOLEAN(c, "0 != false()", false);
}

TEST(xpath_operators_equality_primitive_number)
{
	xml_node c;

	// number vs number
	CHECK_XPATH_BOOLEAN(c, "1 = 1", true);
	CHECK_XPATH_BOOLEAN(c, "0.5 = 0.5", true);
	CHECK_XPATH_BOOLEAN(c, "1 != 2", true);
	CHECK_XPATH_BOOLEAN(c, "1 = -1", false);

	// infinity/nan
	CHECK_XPATH_BOOLEAN(c, "1 div 0 = 2 div 0", true);
	CHECK_XPATH_BOOLEAN(c, "-1 div 0 != 2 div 0", true);
	CHECK_XPATH_BOOLEAN(c, "0 div 0 = 1", false);
	CHECK_XPATH_BOOLEAN(c, "0 div 0 != 1", true);
	CHECK_XPATH_BOOLEAN(c, "0 div 0 = 0 div 0", false);

	// upcast to number
	CHECK_XPATH_BOOLEAN(c, "2 = '2'", true);
	CHECK_XPATH_BOOLEAN(c, "2 != '2'", false);
	CHECK_XPATH_BOOLEAN(c, "'1' != 2", true);
	CHECK_XPATH_BOOLEAN(c, "'1' = 2", false);
}

TEST(xpath_operators_equality_primitive_string)
{
	xml_node c;

	// string vs string
	CHECK_XPATH_BOOLEAN(c, "'a' = 'a'", true);
	CHECK_XPATH_BOOLEAN(c, "'a' = 'b'", false);
	CHECK_XPATH_BOOLEAN(c, "'ab' != 'a'", true);
	CHECK_XPATH_BOOLEAN(c, "'' != 'a'", true);
	CHECK_XPATH_BOOLEAN(c, "'a' != ''", true);
	CHECK_XPATH_BOOLEAN(c, "'' != ''", false);
}

TEST_XML(xpath_operators_equality_node_set_node_set, "<node><c1><v>a</v><v>b</v></c1><c2><v>a</v><v>c</v></c2><c3><v>b</v></c3><c4><v>d</v></c4><c5><v>a</v><v>b</v></c5><c6><v>b</v></c6></node>")
{
	xml_node c;
	xml_node n = doc.child("node");

	// node set vs node set
	CHECK_XPATH_BOOLEAN(c, "x = x", false); // empty node set compares as false with any other object via any comparison operator, as per XPath spec
	CHECK_XPATH_BOOLEAN(c, "x != x", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v = c2/v", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v = c3/v", true);
	CHECK_XPATH_BOOLEAN(n, "c2/v = c3/v", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v = c4/v", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v = x", false);
	CHECK_XPATH_BOOLEAN(n, "x = c1", false);

	CHECK_XPATH_BOOLEAN(n, "c1/v != c2/v", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v != c3/v", true);
	CHECK_XPATH_BOOLEAN(n, "c2/v != c3/v", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v != c4/v", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v != c5/v", true); // (a, b) != (a, b), since a != b, as per XPath spec (comparison operators are so not intutive)
	CHECK_XPATH_BOOLEAN(n, "c3/v != c6/v", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v != x", false);
	CHECK_XPATH_BOOLEAN(n, "x != c1/v", false);
}

TEST_XML(xpath_operators_equality_node_set_primitive, "<node><c1><v>1</v><v>-1</v><v>100</v></c1><c2><v>1</v><v>nan</v></c2></node>")
{
	xml_node c;
	xml_node n = doc.child("node");

	// node set vs number
	CHECK_XPATH_BOOLEAN(c, "x = 1", false);
	CHECK_XPATH_BOOLEAN(c, "x != 1", false);
	CHECK_XPATH_BOOLEAN(c, "1 = x", false);
	CHECK_XPATH_BOOLEAN(c, "1 != x", false);

	CHECK_XPATH_BOOLEAN(n, "c1/v = 1", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v = -1", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v != 1", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v = 5", false);
	CHECK_XPATH_BOOLEAN(n, "c2/v = 1", true);
	CHECK_XPATH_BOOLEAN(n, "c2/v != 1", true);

	CHECK_XPATH_BOOLEAN(n, "1 = c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "-1 = c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "1 != c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "5 = c1/v", false);
	CHECK_XPATH_BOOLEAN(n, "1 = c2/v", true);
	CHECK_XPATH_BOOLEAN(n, "1 != c2/v", true);
	
	// node set vs string
	CHECK_XPATH_BOOLEAN(c, "x = '1'", false);
	CHECK_XPATH_BOOLEAN(c, "x != '1'", false);
	CHECK_XPATH_BOOLEAN(c, "'1' = x", false);
	CHECK_XPATH_BOOLEAN(c, "'1' != x", false);

	CHECK_XPATH_BOOLEAN(n, "c1/v = '1'", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v = '-1'", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v != '1'", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v = '5'", false);
	CHECK_XPATH_BOOLEAN(n, "c2/v = '1'", true);
	CHECK_XPATH_BOOLEAN(n, "c2/v != '1'", true);

	CHECK_XPATH_BOOLEAN(n, "'1' = c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "'-1' = c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "'1' != c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "'5' = c1/v", false);
	CHECK_XPATH_BOOLEAN(n, "'1' = c2/v", true);
	CHECK_XPATH_BOOLEAN(n, "'1' != c2/v", true);

	// node set vs almost-numeric string just in case
	CHECK_XPATH_BOOLEAN(n, "c1/v = '1.0'", false);

	// node set vs boolean - special rules! empty sets are equal to true()
	CHECK_XPATH_BOOLEAN(n, "x = true()", false);
	CHECK_XPATH_BOOLEAN(n, "x != true()", true);
	CHECK_XPATH_BOOLEAN(n, "x = false()", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v = true()", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v != true()", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v = false()", false);

	CHECK_XPATH_BOOLEAN(n, "true() = x", false);
	CHECK_XPATH_BOOLEAN(n, "true() != x", true);
	CHECK_XPATH_BOOLEAN(n, "false() = x", true);
	CHECK_XPATH_BOOLEAN(n, "true() = c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "true() != c1/v", false);
	CHECK_XPATH_BOOLEAN(n, "false() = c1/v", false);
}

TEST(xpath_operators_inequality_primitive)
{
	xml_node c;

	// number vs number
	CHECK_XPATH_BOOLEAN(c, "1 < 2", true);
	CHECK_XPATH_BOOLEAN(c, "1 <= 2", true);
	CHECK_XPATH_BOOLEAN(c, "1 > 2", false);
	CHECK_XPATH_BOOLEAN(c, "1 >= 2", false);

	CHECK_XPATH_BOOLEAN(c, "1 < 1", false);
	CHECK_XPATH_BOOLEAN(c, "1 <= 1", true);
	CHECK_XPATH_BOOLEAN(c, "1 > 1", false);
	CHECK_XPATH_BOOLEAN(c, "1 >= 1", true);

	// infinity/nan
	CHECK_XPATH_BOOLEAN(c, "1 div 0 <= 2 div 0", true);
	CHECK_XPATH_BOOLEAN(c, "1 div 0 < 2 div 0", false);
	CHECK_XPATH_BOOLEAN(c, "-1 div 0 < 2 div 0", true);
	CHECK_XPATH_BOOLEAN(c, "-1 div 0 > 2 div 0", false);

	CHECK_XPATH_BOOLEAN(c, "0 div 0 < 1", false);
	CHECK_XPATH_BOOLEAN(c, "0 div 0 <= 1", false);
	CHECK_XPATH_BOOLEAN(c, "0 div 0 > 1", false);
	CHECK_XPATH_BOOLEAN(c, "0 div 0 >= 1", false);

	// upcast to number
	CHECK_XPATH_BOOLEAN(c, "2 < '2'", false);
	CHECK_XPATH_BOOLEAN(c, "1 < '2'", true);
	CHECK_XPATH_BOOLEAN(c, "2 <= '2'", true);
	CHECK_XPATH_BOOLEAN(c, "3 <= '2'", false);
	CHECK_XPATH_BOOLEAN(c, "2 > '2'", false);
	CHECK_XPATH_BOOLEAN(c, "3 > '2'", true);
	CHECK_XPATH_BOOLEAN(c, "2 >= '2'", true);
	CHECK_XPATH_BOOLEAN(c, "3 >= '2'", true);
	CHECK_XPATH_BOOLEAN(c, "1 >= true()", true);
	CHECK_XPATH_BOOLEAN(c, "1 > true()", false);
}

TEST_XML(xpath_operators_inequality_node_set_node_set, "<node><c1><v>1</v><v>-1</v><v>-100</v></c1><c2><v>1</v><v>nan</v></c2></node>")
{
	xml_node c;
	xml_node n = doc.child("node");

	// node set vs node set
	CHECK_XPATH_BOOLEAN(c, "x < x", false);
	CHECK_XPATH_BOOLEAN(c, "x > x", false);
	CHECK_XPATH_BOOLEAN(c, "x <= x", false);
	CHECK_XPATH_BOOLEAN(c, "x >= x", false);

	CHECK_XPATH_BOOLEAN(n, "c1/v > x", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v < x", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v >= x", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v <= x", false);

	CHECK_XPATH_BOOLEAN(n, "x > c1/v", false);
	CHECK_XPATH_BOOLEAN(n, "x < c1/v", false);
	CHECK_XPATH_BOOLEAN(n, "x >= c1/v", false);
	CHECK_XPATH_BOOLEAN(n, "x <= c1/v", false);

	CHECK_XPATH_BOOLEAN(n, "c1/v > c2/v", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v >= c2/v", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v < c2/v", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v <= c2/v", true);
}

TEST_XML(xpath_operators_inequality_node_set_primitive, "<node><c1><v>1</v><v>-1</v><v>-100</v></c1><c2><v>1</v><v>nan</v></c2></node>")
{
	xml_node c;
	xml_node n = doc.child("node");

	// node set vs number
	CHECK_XPATH_BOOLEAN(c, "x < 0", false);
	CHECK_XPATH_BOOLEAN(c, "x > 0", false);
	CHECK_XPATH_BOOLEAN(c, "x <= 0", false);
	CHECK_XPATH_BOOLEAN(c, "x >= 0", false);

	CHECK_XPATH_BOOLEAN(c, "0 < x", false);
	CHECK_XPATH_BOOLEAN(c, "0 > x", false);
	CHECK_XPATH_BOOLEAN(c, "0 <= x", false);
	CHECK_XPATH_BOOLEAN(c, "0 >= x", false);

	CHECK_XPATH_BOOLEAN(n, "c1/v > 0", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v > 1", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v >= 0", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v < 0", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v <= 0", true);

	CHECK_XPATH_BOOLEAN(n, "0 < c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "1 < c1/v", false);
	CHECK_XPATH_BOOLEAN(n, "0 <= c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "0 > c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "0 >= c1/v", true);

	// node set vs string
	CHECK_XPATH_BOOLEAN(n, "c1/v > '0'", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v > '1'", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v >= '0'", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v < '0'", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v <= '0'", true);

	CHECK_XPATH_BOOLEAN(n, "'0' < c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "'1' < c1/v", false);
	CHECK_XPATH_BOOLEAN(n, "'0' <= c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "'0' > c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "'0' >= c1/v", true);

	// node set vs boolean
	CHECK_XPATH_BOOLEAN(n, "c1/v > false()", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v > true()", false);
	CHECK_XPATH_BOOLEAN(n, "c1/v >= false()", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v < false()", true);
	CHECK_XPATH_BOOLEAN(n, "c1/v <= false()", true);

	CHECK_XPATH_BOOLEAN(n, "false() < c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "true() < c1/v", false);
	CHECK_XPATH_BOOLEAN(n, "false() <= c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "false() > c1/v", true);
	CHECK_XPATH_BOOLEAN(n, "false() >= c1/v", true);
}

TEST(xpath_operators_boolean_precedence)
{
	xml_node c;

	CHECK_XPATH_BOOLEAN(c, "1 = 0 or 2 = 2", true);
	CHECK_XPATH_BOOLEAN(c, "1 = (0 or 2) = false()", false);
	CHECK_XPATH_BOOLEAN(c, "1 < 0 or 2 > 2", false);
	CHECK_XPATH_BOOLEAN(c, "2 < 1 = false()", true);
	CHECK_XPATH_BOOLEAN(c, "2 < (1 = false())", false);
	CHECK_XPATH_BOOLEAN(c, "3 > 2 > 1", false);
	CHECK_XPATH_BOOLEAN(c, "(3 > 2) > 1", false);
	CHECK_XPATH_BOOLEAN(c, "3 > (2 > 1)", true);
}
