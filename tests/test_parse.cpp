#include "common.hpp"

TEST(parse_pi_skip)
{
	xml_document doc;
	CHECK(doc.load("<?pi?><?pi value?>", parse_minimal));
	CHECK(!doc.first_child());
}

TEST(parse_pi_parse)
{
	xml_document doc;
	CHECK(doc.load("<?pi1?><?pi2 value?>", parse_minimal | parse_pi));

	xml_node pi1 = doc.first_child();
	xml_node pi2 = doc.last_child();

	CHECK(pi1 != pi2);
	CHECK(pi1.type() == node_pi);
	CHECK_STRING(pi1.name(), "pi1");
	CHECK_STRING(pi1.value(), "");
	CHECK(pi2.type() == node_pi);
	CHECK_STRING(pi2.name(), "pi2");
	CHECK_STRING(pi2.value(), "value");
}

TEST(parse_pi_error)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_pi};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load("<?", flags).status == status_bad_pi);
		CHECK(doc.load("<?#?>", flags).status == status_bad_pi);
		CHECK(doc.load("<?name", flags).status == status_bad_pi);
		CHECK(doc.load("<?name>", flags).status == status_bad_pi);
		CHECK(doc.load("<?name ?", flags).status == status_bad_pi);
	}
}

TEST(parse_comments_skip)
{
	xml_document doc;
	CHECK(doc.load("<!----><!--value-->", parse_minimal));
	CHECK(!doc.first_child());
}

TEST(parse_comments_parse)
{
	xml_document doc;
	CHECK(doc.load("<!----><!--value-->", parse_minimal | parse_comments));

	xml_node c1 = doc.first_child();
	xml_node c2 = doc.last_child();

	CHECK(c1 != c2);
	CHECK(c1.type() == node_comment);
	CHECK_STRING(c1.name(), "");
	CHECK_STRING(c1.value(), "");
	CHECK(c2.type() == node_comment);
	CHECK_STRING(c2.name(), "");
	CHECK_STRING(c2.value(), "value");
}

TEST(parse_comments_parse_no_eol)
{
	xml_document doc;
	CHECK(doc.load("<!--\r\rval1\rval2\r\nval3\nval4\r\r-->", parse_minimal | parse_comments));

	xml_node c = doc.first_child();
	CHECK(c.type() == node_comment);
	CHECK_STRING(c.value(), "\r\rval1\rval2\r\nval3\nval4\r\r");
}

TEST(parse_comments_parse_eol)
{
	xml_document doc;
	CHECK(doc.load("<!--\r\rval1\rval2\r\nval3\nval4\r\r-->", parse_minimal | parse_comments | parse_eol));

	xml_node c = doc.first_child();
	CHECK(c.type() == node_comment);
	CHECK_STRING(c.value(), "\n\nval1\nval2\nval3\nval4\n\n");
}

TEST(parse_comments_error)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_comments, parse_minimal | parse_comments | parse_eol};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load("<!-", flags).status == status_bad_comment);
		CHECK(doc.load("<!--", flags).status == status_bad_comment);
		CHECK(doc.load("<!--v", flags).status == status_bad_comment);
		CHECK(doc.load("<!-->", flags).status == status_bad_comment);
		CHECK(doc.load("<!--->", flags).status == status_bad_comment);
		CHECK(doc.load("<!-- <!-- --><!- -->", flags).status == status_bad_comment);
	}
}

TEST(parse_cdata_skip)
{
	xml_document doc;
	CHECK(doc.load("<![CDATA[]]><![CDATA[value]]>", parse_minimal));
	CHECK(!doc.first_child());
}

TEST(parse_cdata_parse)
{
	xml_document doc;
	CHECK(doc.load("<![CDATA[]]><![CDATA[value]]>", parse_minimal | parse_cdata));

	xml_node c1 = doc.first_child();
	xml_node c2 = doc.last_child();

	CHECK(c1 != c2);
	CHECK(c1.type() == node_cdata);
	CHECK_STRING(c1.name(), "");
	CHECK_STRING(c1.value(), "");
	CHECK(c2.type() == node_cdata);
	CHECK_STRING(c2.name(), "");
	CHECK_STRING(c2.value(), "value");
}

TEST(parse_cdata_parse_no_eol)
{
	xml_document doc;
	CHECK(doc.load("<![CDATA[\r\rval1\rval2\r\nval3\nval4\r\r]]>", parse_minimal | parse_cdata));

	xml_node c = doc.first_child();
	CHECK(c.type() == node_cdata);
	CHECK_STRING(c.value(), "\r\rval1\rval2\r\nval3\nval4\r\r");
}

TEST(parse_cdata_parse_eol)
{
	xml_document doc;
	CHECK(doc.load("<![CDATA[\r\rval1\rval2\r\nval3\nval4\r\r]]>", parse_minimal | parse_cdata | parse_eol));

	xml_node c = doc.first_child();
	CHECK(c.type() == node_cdata);
	CHECK_STRING(c.value(), "\n\nval1\nval2\nval3\nval4\n\n");
}

TEST(parse_cdata_error)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_cdata, parse_minimal | parse_cdata | parse_eol};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load("<![", flags).status == status_bad_cdata);
		CHECK(doc.load("<![C", flags).status == status_bad_cdata);
		CHECK(doc.load("<![CD", flags).status == status_bad_cdata);
		CHECK(doc.load("<![CDA", flags).status == status_bad_cdata);
		CHECK(doc.load("<![CDAT", flags).status == status_bad_cdata);
		CHECK(doc.load("<![CDATA", flags).status == status_bad_cdata);
		CHECK(doc.load("<![CDATA[", flags).status == status_bad_cdata);
		CHECK(doc.load("<![CDATA[]", flags).status == status_bad_cdata);
		CHECK(doc.load("<![CDATA[>", flags).status == status_bad_cdata);
		CHECK(doc.load("<![CDATA[ <![CDATA[]]><![CDATA ]]>", flags).status == status_bad_cdata);
	}
}

TEST(parse_ws_pcdata_skip)
{
	xml_document doc;
	CHECK(doc.load("  ", parse_minimal));
	CHECK(!doc.first_child());

	CHECK(doc.load("<root>  <node>  </node>  </root>", parse_minimal));
	
	xml_node root = doc.child("root");
	
	CHECK(root.first_child() == root.last_child());
	CHECK(!root.first_child().first_child());
}

TEST(parse_ws_pcdata_parse)
{
	xml_document doc;
	CHECK(doc.load("<root>  <node>  </node>  </root>", parse_minimal | parse_ws_pcdata));

	xml_node root = doc.child("root");

	xml_node c1 = root.first_child();
	xml_node c2 = c1.next_sibling();
	xml_node c3 = c2.next_sibling();

	CHECK(c3 == root.last_child());

	CHECK(c1.type() == node_pcdata);
	CHECK_STRING(c1.value(), "  ");
	CHECK(c3.type() == node_pcdata);
	CHECK_STRING(c3.value(), "  ");

	CHECK(c2.first_child() == c2.last_child());
	CHECK(c2.first_child().type() == node_pcdata);
	CHECK_STRING(c2.first_child().value(), "  ");
}

TEST(parse_pcdata_no_eol)
{
	xml_document doc;
	CHECK(doc.load("<root>\r\rval1\rval2\r\nval3\nval4\r\r</root>", parse_minimal));

	CHECK_STRING(doc.child_value("root"), "\r\rval1\rval2\r\nval3\nval4\r\r");
}

TEST(parse_pcdata_eol)
{
	xml_document doc;
	CHECK(doc.load("<root>\r\rval1\rval2\r\nval3\nval4\r\r</root>", parse_minimal | parse_eol));

	CHECK_STRING(doc.child_value("root"), "\n\nval1\nval2\nval3\nval4\n\n");
}

TEST(parse_pcdata_skip_ext)
{
	xml_document doc;
	CHECK(doc.load("pre<root/>post", parse_minimal));
	CHECK(doc.first_child() == doc.last_child());
	CHECK(doc.first_child().type() == node_element);
}

TEST(parse_pcdata_error)
{
	xml_document doc;
	CHECK(doc.load("<root>pcdata", parse_minimal).status == status_end_element_mismatch);
}

TEST(parse_escapes_skip)
{
	xml_document doc;
	CHECK(doc.load("<node id='&lt;&gt;&amp;&apos;&quot;'>&lt;&gt;&amp;&apos;&quot;</node>", parse_minimal));
	CHECK_STRING(doc.child("node").attribute("id").value(), "&lt;&gt;&amp;&apos;&quot;");
}

TEST(parse_escapes_parse)
{
	xml_document doc;
	CHECK(doc.load("<node id='&lt;&gt;&amp;&apos;&quot;'>&lt;&gt;&amp;&apos;&quot;</node>", parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value("node"), "<>&'\"");
	CHECK_STRING(doc.child("node").attribute("id").value(), "<>&'\"");
}

TEST(parse_escapes_code)
{
	xml_document doc;
	CHECK(doc.load("<node>&#1;&#32;&#x20;</node>", parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value("node"), "\01  ");
}

TEST(parse_escapes_unicode)
{
	xml_document doc;
	CHECK(doc.load("<node>&#x03B3;&#x03b3;</node>", parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value("node"), "\xce\xb3\xce\xb3");
}

TEST(parse_escapes_error)
{
	xml_document doc;
	CHECK(doc.load("<node>&#x03g;&#ab;&quot</node>", parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value("node"), "&#x03g;&#ab;&quot");

	CHECK(!doc.load("<node id='&#x12"));
}

TEST(parse_attribute_spaces)
{
	xml_document doc;
	CHECK(doc.load("<node id1='v1' id2 ='v2' id3= 'v3' id4 = 'v4' id5 \n\r\t = \r\t\n 'v5' />", parse_minimal));
	CHECK_STRING(doc.child("node").attribute("id1").value(), "v1");
	CHECK_STRING(doc.child("node").attribute("id2").value(), "v2");
	CHECK_STRING(doc.child("node").attribute("id3").value(), "v3");
	CHECK_STRING(doc.child("node").attribute("id4").value(), "v4");
	CHECK_STRING(doc.child("node").attribute("id5").value(), "v5");
}

TEST(parse_attribute_quot)
{
	xml_document doc;
	CHECK(doc.load("<node id1='v1' id2=\"v2\"/>", parse_minimal));
	CHECK_STRING(doc.child("node").attribute("id1").value(), "v1");
	CHECK_STRING(doc.child("node").attribute("id2").value(), "v2");
}

TEST(parse_attribute_no_eol_no_wconv)
{
	xml_document doc;
	CHECK(doc.load("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>", parse_minimal));
	CHECK_STRING(doc.child("node").attribute("id").value(), " \t\r\rval1  \rval2\r\nval3\nval4\r\r");
}

TEST(parse_attribute_eol_no_wconv)
{
	xml_document doc;
	CHECK(doc.load("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>", parse_minimal | parse_eol));
	CHECK_STRING(doc.child("node").attribute("id").value(), " \t\n\nval1  \nval2\nval3\nval4\n\n");
}

TEST(parse_attribute_no_eol_wconv)
{
	xml_document doc;
	CHECK(doc.load("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>", parse_minimal | parse_wconv_attribute));
	CHECK_STRING(doc.child("node").attribute("id").value(), "    val1   val2  val3 val4  ");
}

TEST(parse_attribute_eol_wconv)
{
	xml_document doc;
	CHECK(doc.load("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>", parse_minimal | parse_eol | parse_wconv_attribute));
	CHECK_STRING(doc.child("node").attribute("id").value(), "    val1   val2 val3 val4  ");
}

TEST(parse_attribute_wnorm)
{
	xml_document doc;

	for (int eol = 0; eol < 2; ++eol)
		for (int wconv = 0; wconv < 2; ++wconv)
		{
			unsigned int flags = parse_minimal | parse_wnorm_attribute | (eol ? parse_eol : 0) | (wconv ? parse_wconv_attribute : 0);
			CHECK(doc.load("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>", flags));
			CHECK_STRING(doc.child("node").attribute("id").value(), "val1 val2 val3 val4");
		}
}

TEST(parse_attribute_variations)
{
	xml_document doc;

	for (int wnorm = 0; wnorm < 2; ++wnorm)
		for (int eol = 0; eol < 2; ++eol)
			for (int wconv = 0; wconv < 2; ++wconv)
				for (int escapes = 0; escapes < 2; ++escapes)
				{
					unsigned int flags = parse_minimal;
					
					 flags |= (wnorm ? parse_wnorm_attribute : 0);
					 flags |= (eol ? parse_eol : 0);
					 flags |= (wconv ? parse_wconv_attribute : 0);
					 flags |= (escapes ? parse_escapes : 0);

					CHECK(doc.load("<node id='1'/>", flags));
					CHECK_STRING(doc.child("node").attribute("id").value(), "1");
				}
}

TEST(parse_attribute_error)
{
	xml_document doc;
	CHECK(doc.load("<node id/>", parse_minimal).status == status_bad_attribute);
	CHECK(doc.load("<node id=/>", parse_minimal).status == status_bad_attribute);
	CHECK(doc.load("<node id='/>", parse_minimal).status == status_bad_attribute);
	CHECK(doc.load("<node id=\"/>", parse_minimal).status == status_bad_attribute);
	CHECK(doc.load("<node id=\"'/>", parse_minimal).status == status_bad_attribute);
	CHECK(doc.load("<node id='\"/>", parse_minimal).status == status_bad_attribute);
	CHECK(doc.load("<node id='\"/>", parse_minimal).status == status_bad_attribute);
	CHECK(doc.load("<node #/>", parse_minimal).status == status_bad_start_element);
	CHECK(doc.load("<node#/>", parse_minimal).status == status_bad_start_element);
}

TEST(parse_tag_single)
{
	xml_document doc;
	CHECK(doc.load("<node/><node /><node\n/>", parse_minimal));
	CHECK_NODE(doc, "<node /><node /><node />");
}

TEST(parse_tag_hierarchy)
{
	xml_document doc;
	CHECK(doc.load("<node><n1><n2/></n1><n3><n4><n5></n5></n4></n3 \r\n></node>", parse_minimal));
	CHECK_NODE(doc, "<node><n1><n2 /></n1><n3><n4><n5 /></n4></n3></node>");
}

TEST(parse_tag_error)
{
	xml_document doc;
	CHECK(doc.load("<", parse_minimal).status == status_unrecognized_tag);
	CHECK(doc.load("<!", parse_minimal).status == status_unrecognized_tag);
	CHECK(doc.load("<!D", parse_minimal).status == status_unrecognized_tag);
	CHECK(doc.load("<#", parse_minimal).status == status_unrecognized_tag);
	CHECK(doc.load("<node#", parse_minimal).status == status_bad_start_element);
	CHECK(doc.load("<node", parse_minimal).status == status_bad_start_element);
	CHECK(doc.load("<node/", parse_minimal).status == status_bad_start_element);
	CHECK(doc.load("<node/ >", parse_minimal).status == status_bad_start_element);
	CHECK(doc.load("</ node>", parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load("</node", parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load("</node ", parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load("<node></ node>", parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load("<node></node", parse_minimal).status == status_bad_end_element);
	CHECK(doc.load("<node></node ", parse_minimal).status == status_bad_end_element);
	CHECK(doc.load("<node></nodes>", parse_minimal).status == status_end_element_mismatch);
}

TEST(parse_declaration_skip)
{
	xml_document doc;
	CHECK(doc.load("<?xml?><?xml version='1.0'?>", parse_minimal));
	CHECK(!doc.first_child());
}

TEST(parse_declaration_parse)
{
	xml_document doc;
	CHECK(doc.load("<?xml?><?xml version='1.0'?>", parse_minimal | parse_declaration));

	xml_node d1 = doc.first_child();
	xml_node d2 = doc.last_child();

	CHECK(d1 != d2);
	CHECK(d1.type() == node_declaration);
	CHECK_STRING(d1.name(), "xml");
	CHECK(d2.type() == node_declaration);
	CHECK_STRING(d2.name(), "xml");
	CHECK_STRING(d2.attribute("version").value(), "1.0");
}

TEST(parse_declaration_error)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_declaration};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load("<?xml", flags).status == status_bad_pi);
		CHECK(doc.load("<?xml?", flags).status == status_bad_pi);
		CHECK(doc.load("<?xml>", flags).status == status_bad_pi);
		CHECK(doc.load("<?xml version='1>", flags).status == status_bad_pi);
	}
	
	CHECK(doc.load("<?xml version='1?>", parse_minimal | parse_declaration).status == status_bad_attribute);
}

TEST(parse_doctype_skip)
{
	xml_document doc;
	CHECK(doc.load("<!DOCTYPE doc>") && !doc.first_child());
	CHECK(doc.load("<!DOCTYPE doc SYSTEM 'foo'>") && !doc.first_child());
	CHECK(doc.load("<!DOCTYPE doc SYSTEM \"foo\">") && !doc.first_child());
	CHECK(doc.load("<!DOCTYPE doc PUBLIC \"foo\" 'bar'>") && !doc.first_child());
	CHECK(doc.load("<!DOCTYPE doc PUBLIC \"foo'\">") && !doc.first_child());
	CHECK(doc.load("<!DOCTYPE doc SYSTEM 'foo' [<!ELEMENT foo 'ANY'>]>") && !doc.first_child());

	CHECK(doc.load("<!DOCTYPE doc SYSTEM 'foo' [<!ELEMENT foo 'ANY'>]><node/>"));
	CHECK_NODE(doc, "<node />");
}

TEST(parse_doctype_error)
{
	xml_document doc;
	CHECK(doc.load("<!DOCTYPE").status == status_bad_doctype);
	CHECK(doc.load("<!DOCTYPE doc").status == status_bad_doctype);
	CHECK(doc.load("<!DOCTYPE doc SYSTEM 'foo").status == status_bad_doctype);
	CHECK(doc.load("<!DOCTYPE doc SYSTEM \"foo").status == status_bad_doctype);
	CHECK(doc.load("<!DOCTYPE doc PUBLIC \"foo\" 'bar").status == status_bad_doctype);
	CHECK(doc.load("<!DOCTYPE doc PUBLIC \"foo'\"").status == status_bad_doctype);
	CHECK(doc.load("<!DOCTYPE doc SYSTEM 'foo' [<!ELEMENT foo 'ANY").status == status_bad_doctype);
	CHECK(doc.load("<!DOCTYPE doc SYSTEM 'foo' [<!ELEMENT foo 'ANY'>").status == status_bad_doctype);
}
