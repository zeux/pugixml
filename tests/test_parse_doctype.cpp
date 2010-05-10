#include "common.hpp"

#include <string>

bool test_doctype_wf(const std::basic_string<char_t>& decl)
{
	xml_document doc;

	// standalone
	if (!doc.load(decl.c_str()) || doc.first_child()) return false;

	// pcdata pre/postfix
	if (!doc.load(("a" + decl).c_str()) || doc.first_child()) return false;
	if (!doc.load((decl + "b").c_str()) || doc.first_child()) return false;
	if (!doc.load(("a" + decl + "b").c_str()) || doc.first_child()) return false;

	// node pre/postfix
	if (!doc.load(("<nodea/>" + decl).c_str()) || !test_node(doc, STR("<nodea />"), STR(""), format_raw)) return false;
	if (!doc.load((decl + "<nodeb/>").c_str()) || !test_node(doc, STR("<nodeb />"), STR(""), format_raw)) return false;
	if (!doc.load(("<nodea/>" + decl + "<nodeb/>").c_str()) || !test_node(doc, STR("<nodea /><nodeb />"), STR(""), format_raw)) return false;

	return true;
}

bool test_doctype_nwf(const std::basic_string<char_t>& decl)
{
	xml_document doc;

	// standalone
	if (doc.load(decl.c_str()).status != status_bad_doctype) return false;

	// pcdata postfix
	if (doc.load((decl + "b").c_str()).status != status_bad_doctype) return false;

	// node postfix
	if (doc.load((decl + "<nodeb/>").c_str()).status != status_bad_doctype) return false;

	return true;
}

#define TEST_DOCTYPE_WF(contents) CHECK(test_doctype_wf(STR(contents)))
#define TEST_DOCTYPE_NWF(contents) CHECK(test_doctype_nwf(STR(contents)))

TEST(parse_doctype_skip)
{
	TEST_DOCTYPE_WF("<!DOCTYPE doc>");
	TEST_DOCTYPE_WF("<!DOCTYPE doc SYSTEM 'foo'>");
	TEST_DOCTYPE_WF("<!DOCTYPE doc SYSTEM \"foo\">");
	TEST_DOCTYPE_WF("<!DOCTYPE doc PUBLIC \"foo\" 'bar'>");
	TEST_DOCTYPE_WF("<!DOCTYPE doc PUBLIC \"foo'\">");
	TEST_DOCTYPE_WF("<!DOCTYPE doc SYSTEM 'foo' [<!ELEMENT foo 'ANY'>]>");
}

TEST(parse_doctype_error)
{
	TEST_DOCTYPE_NWF("<!DOCTYPE");
	TEST_DOCTYPE_NWF("<!DOCTYPE doc");
	TEST_DOCTYPE_NWF("<!DOCTYPE doc SYSTEM 'foo");
	TEST_DOCTYPE_NWF("<!DOCTYPE doc SYSTEM \"foo");
	TEST_DOCTYPE_NWF("<!DOCTYPE doc PUBLIC \"foo\" 'bar");
	TEST_DOCTYPE_NWF("<!DOCTYPE doc PUBLIC \"foo'\"");
	TEST_DOCTYPE_NWF("<!DOCTYPE doc SYSTEM 'foo' [<!ELEMENT foo 'ANY");
	TEST_DOCTYPE_NWF("<!DOCTYPE doc SYSTEM 'foo' [<!ELEMENT foo 'ANY'>");
	TEST_DOCTYPE_NWF("<!DOCTYPE doc SYSTEM 'foo' [<!ELEMENT foo 'ANY'>]");
	TEST_DOCTYPE_NWF("<!DOCTYPE doc SYSTEM 'foo' [<!ELEMENT foo 'ANY'>] ");
}

// Examples from W3C recommendations
TEST(parse_doctype_w3c_wf)
{
	TEST_DOCTYPE_WF("<!DOCTYPE greeting SYSTEM \"hello.dtd\">");
	TEST_DOCTYPE_WF("<!DOCTYPE greeting [ <!ELEMENT greeting (#PCDATA)> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE greeting [ <!ATTLIST list type    (bullets|ordered|glossary)  \"ordered\"> <!ATTLIST form method  CDATA   #FIXED \"POST\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE greeting [ <!ENTITY % draft 'INCLUDE' > <!ENTITY % final 'IGNORE' > <![%draft;[ <!ELEMENT book (comments*, title, body, supplements?)> ]]> <![%final;[ <!ELEMENT book (title, body, supplements?)> ]]>]>");
	TEST_DOCTYPE_WF("<!DOCTYPE greeting [ <!ENTITY open-hatch PUBLIC \"-//Textuality//TEXT Standard open-hatch boilerplate//EN\" \"http://www.textuality.com/boilerplate/OpenHatch.xml\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE greeting [ <!ENTITY EndAttr \"27'\" > ]>");
}

TEST(parse_doctype_w3c_nwf)
{
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting SYSTEM \"hello.dtd>");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting SYSTEM");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting [ <!ELEMENT greeting (#PCDATA)> ]");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting [ <!ELEMENT greeting (#PCDATA)>");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting [ <!ELEMENT greeting (#PCDATA");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting [ ");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting [ <!ATTLIST list type    (bullets|ordered|glossary)  \"ordered\"> ]");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting [ <!ATTLIST list type    (bullets|ordered|glossary)  \"ordered\">");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting [ <!ATTLIST list type    (bullets|ordered|glossary)  \"orde");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting [ <!ATTLIST list type    (bullets|ordered|glossary) ");
	TEST_DOCTYPE_NWF("<!DOCTYPE greeting [ <!ENTITY open-hatch PUBLIC \"-//Textuality//TEXT Standard open-hatch boilerplate//EN\" \"http://www.textuality.com/boilerplate/OpenHatch.x");
}
