#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

TEST_XML(xpath_paths_abbrev_w3c_1, "<node><para/><foo/><para/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "para");
	CHECK_XPATH_NODESET(n, "para") % 3 % 5;
}

TEST_XML(xpath_paths_abbrev_w3c_2, "<node><para/><foo/><para/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "*");
	CHECK_XPATH_NODESET(n, "*") % 3 % 4 % 5;
}

TEST_XML(xpath_paths_abbrev_w3c_3, "<node>pcdata<child/><![CDATA[cdata]]></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "text()");
	CHECK_XPATH_NODESET(n, "text()") % 3 % 5;
}

TEST_XML(xpath_paths_abbrev_w3c_4, "<node name='value' foo='bar' />")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "@name");
	CHECK_XPATH_NODESET(n, "@name") % 3;
}

TEST_XML(xpath_paths_abbrev_w3c_5, "<node name='value' foo='bar' />")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "@*");
	CHECK_XPATH_NODESET(n, "@*") % 3 % 4;
}

TEST_XML(xpath_paths_abbrev_w3c_6, "<node><para/><para/><para/><para/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "para[1]");
	CHECK_XPATH_NODESET(n, "para[1]") % 3;
}

TEST_XML(xpath_paths_abbrev_w3c_7, "<node><para/><para/><para/><para/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "para[last()]");
	CHECK_XPATH_NODESET(n, "para[last()]") % 6;
}

TEST_XML(xpath_paths_abbrev_w3c_8, "<node><para><para/><para/><foo><para/></foo></para><foo/><para/></node>")
{
	doc.precompute_document_order();

	xml_node c;

	CHECK_XPATH_NODESET(c, "*/para");
	CHECK_XPATH_NODESET(doc, "*/para") % 3 % 9;
}

TEST_XML(xpath_paths_abbrev_w3c_9, "<doc><chapter/><chapter/><chapter/><chapter/><chapter><section/><section/><section/></chapter><chapter/></doc>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("doc").child("chapter");

	CHECK_XPATH_NODESET(c, "/doc/chapter[5]/section[2]");
	CHECK_XPATH_NODESET(n, "/doc/chapter[5]/section[2]") % 9;
	CHECK_XPATH_NODESET(doc, "/doc/chapter[5]/section[2]") % 9;
}

TEST_XML(xpath_paths_abbrev_w3c_10, "<chapter><para><para/><para/><foo><para/></foo></para><foo/><para/></chapter>")
{
	doc.precompute_document_order();

	xml_node c;

	CHECK_XPATH_NODESET(c, "chapter//para");
	CHECK_XPATH_NODESET(doc, "chapter//para") % 3 % 4 % 5 % 7 % 9;
}

TEST_XML(xpath_paths_abbrev_w3c_11, "<node><para><para/><para/><foo><para/></foo></para><foo/><para/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "//para");
	CHECK_XPATH_NODESET(n, "//para") % 3 % 4 % 5 % 7 % 9;
	CHECK_XPATH_NODESET(n.child("para"), "//para") % 3 % 4 % 5 % 7 % 9;
}

TEST_XML(xpath_paths_abbrev_w3c_12, "<node><olist><item/></olist><item/><olist><olist><item/><item/></olist></olist></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "//olist/item");
	CHECK_XPATH_NODESET(n, "//olist/item") % 4 % 8 % 9;
	CHECK_XPATH_NODESET(n.child("olist"), "//olist/item") % 4 % 8 % 9;
}

TEST_XML(xpath_paths_abbrev_w3c_13, "<node><child/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, ".");
	CHECK_XPATH_NODESET(n, ".") % 2;
	CHECK_XPATH_NODESET(n.child("child"), ".") % 3;
}

TEST_XML(xpath_paths_abbrev_w3c_14, "<node><para><para/><para/><foo><para/></foo></para><foo/><para/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, ".//para");
	CHECK_XPATH_NODESET(n, ".//para") % 3 % 4 % 5 % 7 % 9;
	CHECK_XPATH_NODESET(n.child("para"), ".//para") % 4 % 5 % 7;
}

TEST_XML(xpath_paths_abbrev_w3c_15, "<node lang='en'><child/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "..");
	CHECK_XPATH_NODESET(n, "..") % 1;
	CHECK_XPATH_NODESET(n.child("child"), "..") % 2;
}

TEST_XML(xpath_paths_abbrev_w3c_16, "<node lang='en'><child/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "../@lang");
	CHECK_XPATH_NODESET(n, "../@lang");
	CHECK_XPATH_NODESET(n.child("child"), "../@lang") % 3;
}

TEST_XML(xpath_paths_abbrev_w3c_17, "<node><para/><para type='warning'/><para type='warning'/><para/><para type='error'/><para type='warning'/><para type='warning'/><para type='warning'/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "para[@type=\"warning\"]");
	CHECK_XPATH_NODESET(n, "para[@type=\"warning\"]") % 4 % 6 % 11 % 13 % 15;
}

TEST_XML(xpath_paths_abbrev_w3c_18, "<node><para/><para type='warning'/><para type='warning'/><para/><para type='error'/><para type='warning'/><para type='warning'/><para type='warning'/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "para[@type=\"warning\"][5]");
	CHECK_XPATH_NODESET(n, "para[@type=\"warning\"][5]") % 15;
}

TEST_XML(xpath_paths_abbrev_w3c_19a, "<node><para/><para type='warning'/><para type='warning'/><para/><para type='error'/><para type='warning'/><para type='warning'/><para type='warning'/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "para[5][@type=\"warning\"]");
	CHECK_XPATH_NODESET(n, "para[5][@type=\"warning\"]");
}

TEST_XML(xpath_paths_abbrev_w3c_19b, "<node><para/><para type='warning'/><para type='warning'/><para/><para type='warning'/><para type='warning'/><para type='warning'/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "para[5][@type=\"warning\"]");
	CHECK_XPATH_NODESET(n, "para[5][@type=\"warning\"]") % 9;
}

TEST_XML(xpath_paths_abbrev_w3c_20, "<node><chapter><title>foo</title></chapter><chapter><title>Introduction</title></chapter><chapter><title>introduction</title></chapter><chapter/><chapter><title>Introduction</title><title>foo</title></chapter></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "chapter[title=\"Introduction\"]");
	CHECK_XPATH_NODESET(n, "chapter[title=\"Introduction\"]") % 6 % 13;
}

TEST_XML(xpath_paths_abbrev_w3c_21, "<node><chapter><title>foo</title></chapter><chapter><title>Introduction</title></chapter><chapter><title>introduction</title></chapter><chapter/><chapter><title>Introduction</title><title>foo</title></chapter></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "chapter[title]");
	CHECK_XPATH_NODESET(n, "chapter[title]") % 3 % 6 % 9 % 13;
}

TEST_XML(xpath_paths_abbrev_w3c_22, "<node><employee/><employee secretary=''/><employee assistant=''/><employee secretary='' assistant=''/><employee assistant='' secretary=''/></node>")
{
	doc.precompute_document_order();

	xml_node c;
	xml_node n = doc.child("node");

	CHECK_XPATH_NODESET(c, "employee[@secretary and @assistant]");
	CHECK_XPATH_NODESET(n, "employee[@secretary and @assistant]") % 8 % 11;
}

#endif
