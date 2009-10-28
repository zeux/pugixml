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
