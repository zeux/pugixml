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

// Examples from xmlsuite
TEST(parse_doctype_xmlconf_eduni_1)
{
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!NOTATION gif SYSTEM \"file:///usr/X11R6/bin/xv\"> <!ENTITY declared SYSTEM \"xyzzy\" NDATA gif> <!ATTLIST foo bar ENTITY \"7\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!NOTATION gif SYSTEM \"file:///usr/X11R6/bin/xv\"> <!ENTITY declared SYSTEM \"xyzzy\" NDATA gif> <!ATTLIST foo bar ENTITY \"undeclared\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY % e SYSTEM \"E60.ent\"> %e; ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY e \"an &unparsed; entity\"> <!NOTATION gif SYSTEM \"file:///usr/X11R6/bin/xv\"> <!ENTITY unparsed SYSTEM \"xyzzy\" NDATA gif> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo bar CDATA #IMPLIED> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo xml:lang CDATA #IMPLIED> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY e SYSTEM \"E38.ent\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo SYSTEM \"E36.dtd\">");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ELEMENT bar (foo|foo)> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!NOTATION one SYSTEM \"file:///usr/bin/awk\"> <!ATTLIST foo bar NOTATION (one|one) #IMPLIED> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo bar (one|one) #IMPLIED> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo xml:lang NMTOKEN #IMPLIED> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY gt \">\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo bar NMTOKENS #IMPLIED> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY % pe SYSTEM \"subdir1/E18-pe\"> %pe; %intpe; ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo (PCDATA|foo)*> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo (foo*)> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo (foo*)> <!ENTITY space \"&#38;#32;\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo (foo*)> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo (foo*)> <!ENTITY space \" \"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo EMPTY> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo EMPTY> <!ENTITY empty \"\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <![INCLUDE[<!ATTLIST foo bar CDATA #IMPLIED>]]> <![IGNORE[some junk]]> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY % pe \"hello\"> <!-- If forward were expanded when ent was declared, we were get an error, but it is bypassed and not expanded until ent is used in the instance --> <!ENTITY ent \"%pe; &#33; &forward;\"> <!ENTITY forward \"goodbye\"> ]>");
	TEST_DOCTYPE_NWF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY % e \"bar CDATA #IMPLIED>\"> <!ATTLIST foo %e;");
	TEST_DOCTYPE_NWF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY % e \"bar CDATA #IMPLIED>\"> <!ATTLIST foo %e; ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ENTITY ent SYSTEM 'E18-ent'> ]>");
}

TEST(parse_doctype_xmlconf_eduni_2)
{
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ENTITY % pe \"<!ENTITY ent1 'text'>\"> %pe; <!ELEMENT foo ANY> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ENTITY ent SYSTEM \"ent\"> <!ELEMENT foo ANY> <!ATTLIST foo a CDATA \"contains &ent; reference\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo id ID #IMPLIED> <!ATTLIST foo ref IDREF \"undef\"> <!ATTLIST foo ent ENTITY \"undef\"> <!-- can't test NOTATION attribute, because if it's undeclared then we'll get an error for one of the enumerated values being undeclared. --> <!ENTITY ent SYSTEM \"foo\" NDATA not> <!NOTATION not SYSTEM \"not\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo EMPTY> <!ATTLIST foo a (one|two|three) \"four\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo a NOTATION (not) \"not2\"> <!NOTATION not SYSTEM \"not\"> <!NOTATION not2 SYSTEM \"not2\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo EMPTY> <!ATTLIST foo a NMTOKENS \"34+\"> ]>");
}

TEST(parse_doctype_xmlconf_eduni_3)
{
	TEST_DOCTYPE_WF("<!DOCTYPE animal [ <!ELEMENT animal ANY> <?_\xd9\x9f an only legal per 5th edition extender #x65f in PITarget ?> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE root [ <!ELEMENT root ANY> <!ELEMENT \xc2\xb7_BadName EMPTY> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE doc [ <!ENTITY e \"<X&#xe5c;></X&#xe5c;>\"> ]>");
}

TEST(parse_doctype_xmlconf_eduni_4)
{
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY a:b \"bogus\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo xmlns:a CDATA #IMPLIED xmlns:b NMTOKEN #IMPLIED xmlns:c CDATA #IMPLIED> <!ELEMENT bar ANY> <!ATTLIST bar a:attr CDATA #IMPLIED b:attr CDATA #IMPLIED c:attr CDATA #IMPLIED> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo xmlns:a CDATA #IMPLIED xmlns:b CDATA #IMPLIED xmlns:c CDATA #IMPLIED> <!ELEMENT bar ANY> <!ATTLIST bar a:attr CDATA #IMPLIED b:attr CDATA #IMPLIED c:attr CDATA #IMPLIED> <!ENTITY tilde \"~\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT xmlns:foo EMPTY> ]>");
}

TEST(parse_doctype_xmlconf_eduni_5)
{
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ENTITY e \"&#x0c;\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo (foo*)> <!ENTITY e \"abc&#x85;def\"> ]>");
	TEST_DOCTYPE_WF("<!DOCTYPE foo [ <!ELEMENT foo ANY> <!ATTLIST foo bar  NMTOKENS #IMPLIED> <!ENTITY val \"abc&#x85;def\"> ]>");
}
