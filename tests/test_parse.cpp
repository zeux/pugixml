#include "common.hpp"

TEST(parse_pi_skip)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_declaration};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load(STR("<?pi?><?pi value?>"), flags));
		CHECK(!doc.first_child());

		CHECK(doc.load(STR("<?pi <tag/> value?>"), flags));
		CHECK(!doc.first_child());
	}
}

TEST(parse_pi_parse)
{
	xml_document doc;
	CHECK(doc.load(STR("<?pi1?><?pi2 value?>"), parse_minimal | parse_pi));

	xml_node pi1 = doc.first_child();
	xml_node pi2 = doc.last_child();

	CHECK(pi1 != pi2);
	CHECK(pi1.type() == node_pi);
	CHECK_STRING(pi1.name(), STR("pi1"));
	CHECK_STRING(pi1.value(), STR(""));
	CHECK(pi2.type() == node_pi);
	CHECK_STRING(pi2.name(), STR("pi2"));
	CHECK_STRING(pi2.value(), STR("value"));
}

TEST(parse_pi_error)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_pi};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load(STR("<?"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<??"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?>"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?#?>"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name>"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name ?"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name?"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name? "), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name?  "), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name "), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name  "), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name   "), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name value"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name value "), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name value  "), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name value  ?"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name value  ? "), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name value  ? >"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name value  ? > "), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name&"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?name&?"), flags).status == status_bad_pi);
	}
	
	CHECK(doc.load(STR("<?xx#?>"), parse_minimal | parse_pi).status == status_bad_pi);
	CHECK(doc.load(STR("<?name&?>"), parse_minimal | parse_pi).status == status_bad_pi);
	CHECK(doc.load(STR("<?name& x?>"), parse_minimal | parse_pi).status == status_bad_pi);
}

TEST(parse_comments_skip)
{
	xml_document doc;
	CHECK(doc.load(STR("<!----><!--value-->"), parse_minimal));
	CHECK(!doc.first_child());
}

TEST(parse_comments_parse)
{
	xml_document doc;
	CHECK(doc.load(STR("<!----><!--value-->"), parse_minimal | parse_comments));

	xml_node c1 = doc.first_child();
	xml_node c2 = doc.last_child();

	CHECK(c1 != c2);
	CHECK(c1.type() == node_comment);
	CHECK_STRING(c1.name(), STR(""));
	CHECK_STRING(c1.value(), STR(""));
	CHECK(c2.type() == node_comment);
	CHECK_STRING(c2.name(), STR(""));
	CHECK_STRING(c2.value(), STR("value"));
}

TEST(parse_comments_parse_no_eol)
{
	xml_document doc;
	CHECK(doc.load(STR("<!--\r\rval1\rval2\r\nval3\nval4\r\r-->"), parse_minimal | parse_comments));

	xml_node c = doc.first_child();
	CHECK(c.type() == node_comment);
	CHECK_STRING(c.value(), STR("\r\rval1\rval2\r\nval3\nval4\r\r"));
}

TEST(parse_comments_parse_eol)
{
	xml_document doc;
	CHECK(doc.load(STR("<!--\r\rval1\rval2\r\nval3\nval4\r\r-->"), parse_minimal | parse_comments | parse_eol));

	xml_node c = doc.first_child();
	CHECK(c.type() == node_comment);
	CHECK_STRING(c.value(), STR("\n\nval1\nval2\nval3\nval4\n\n"));
}

TEST(parse_comments_error)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_comments, parse_minimal | parse_comments | parse_eol};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load(STR("<!-"), flags).status == status_bad_comment);
		CHECK(doc.load(STR("<!--"), flags).status == status_bad_comment);
		CHECK(doc.load(STR("<!--v"), flags).status == status_bad_comment);
		CHECK(doc.load(STR("<!-->"), flags).status == status_bad_comment);
		CHECK(doc.load(STR("<!--->"), flags).status == status_bad_comment);
		CHECK(doc.load(STR("<!-- <!-- --><!- -->"), flags).status == status_bad_comment);
	}
}

TEST(parse_cdata_skip)
{
	xml_document doc;
	CHECK(doc.load(STR("<![CDATA[]]><![CDATA[value]]>"), parse_minimal));
	CHECK(!doc.first_child());
}

TEST(parse_cdata_parse)
{
	xml_document doc;
	CHECK(doc.load(STR("<![CDATA[]]><![CDATA[value]]>"), parse_minimal | parse_cdata));

	xml_node c1 = doc.first_child();
	xml_node c2 = doc.last_child();

	CHECK(c1 != c2);
	CHECK(c1.type() == node_cdata);
	CHECK_STRING(c1.name(), STR(""));
	CHECK_STRING(c1.value(), STR(""));
	CHECK(c2.type() == node_cdata);
	CHECK_STRING(c2.name(), STR(""));
	CHECK_STRING(c2.value(), STR("value"));
}

TEST(parse_cdata_parse_no_eol)
{
	xml_document doc;
	CHECK(doc.load(STR("<![CDATA[\r\rval1\rval2\r\nval3\nval4\r\r]]>"), parse_minimal | parse_cdata));

	xml_node c = doc.first_child();
	CHECK(c.type() == node_cdata);
	CHECK_STRING(c.value(), STR("\r\rval1\rval2\r\nval3\nval4\r\r"));
}

TEST(parse_cdata_parse_eol)
{
	xml_document doc;
	CHECK(doc.load(STR("<![CDATA[\r\rval1\rval2\r\nval3\nval4\r\r]]>"), parse_minimal | parse_cdata | parse_eol));

	xml_node c = doc.first_child();
	CHECK(c.type() == node_cdata);
	CHECK_STRING(c.value(), STR("\n\nval1\nval2\nval3\nval4\n\n"));
}

TEST(parse_cdata_error)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_cdata, parse_minimal | parse_cdata | parse_eol};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load(STR("<!["), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![C"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CD"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDA"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDAT"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDATA"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDATA["), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDATA[]"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDATA[data"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDATA[data]"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDATA[data]]"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDATA[>"), flags).status == status_bad_cdata);
		CHECK(doc.load(STR("<![CDATA[ <![CDATA[]]><![CDATA ]]>"), flags).status == status_bad_cdata);
	}
}

TEST(parse_ws_pcdata_skip)
{
	xml_document doc;
	CHECK(doc.load(STR("  "), parse_minimal));
	CHECK(!doc.first_child());

	CHECK(doc.load(STR("<root>  <node>  </node>  </root>"), parse_minimal));
	
	xml_node root = doc.child(STR("root"));
	
	CHECK(root.first_child() == root.last_child());
	CHECK(!root.first_child().first_child());
}

TEST(parse_ws_pcdata_parse)
{
	xml_document doc;
	CHECK(doc.load(STR("<root>  <node>  </node>  </root>"), parse_minimal | parse_ws_pcdata));

	xml_node root = doc.child(STR("root"));

	xml_node c1 = root.first_child();
	xml_node c2 = c1.next_sibling();
	xml_node c3 = c2.next_sibling();

	CHECK(c3 == root.last_child());

	CHECK(c1.type() == node_pcdata);
	CHECK_STRING(c1.value(), STR("  "));
	CHECK(c3.type() == node_pcdata);
	CHECK_STRING(c3.value(), STR("  "));

	CHECK(c2.first_child() == c2.last_child());
	CHECK(c2.first_child().type() == node_pcdata);
	CHECK_STRING(c2.first_child().value(), STR("  "));
}

TEST(parse_pcdata_no_eol)
{
	xml_document doc;
	CHECK(doc.load(STR("<root>\r\rval1\rval2\r\nval3\nval4\r\r</root>"), parse_minimal));

	CHECK_STRING(doc.child_value(STR("root")), STR("\r\rval1\rval2\r\nval3\nval4\r\r"));
}

TEST(parse_pcdata_eol)
{
	xml_document doc;
	CHECK(doc.load(STR("<root>\r\rval1\rval2\r\nval3\nval4\r\r</root>"), parse_minimal | parse_eol));

	CHECK_STRING(doc.child_value(STR("root")), STR("\n\nval1\nval2\nval3\nval4\n\n"));
}

TEST(parse_pcdata_skip_ext)
{
	xml_document doc;
	CHECK(doc.load(STR("pre<root/>post"), parse_minimal));
	CHECK(doc.first_child() == doc.last_child());
	CHECK(doc.first_child().type() == node_element);
}

TEST(parse_pcdata_error)
{
	xml_document doc;
	CHECK(doc.load(STR("<root>pcdata"), parse_minimal).status == status_end_element_mismatch);
}

TEST(parse_escapes_skip)
{
	xml_document doc;
	CHECK(doc.load(STR("<node id='&lt;&gt;&amp;&apos;&quot;'>&lt;&gt;&amp;&apos;&quot;</node>"), parse_minimal));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id")).value(), STR("&lt;&gt;&amp;&apos;&quot;"));
}

TEST(parse_escapes_parse)
{
	xml_document doc;
	CHECK(doc.load(STR("<node id='&lt;&gt;&amp;&apos;&quot;'>&lt;&gt;&amp;&apos;&quot;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("<>&'\""));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id")).value(), STR("<>&'\""));
}

TEST(parse_escapes_code)
{
	xml_document doc;
	CHECK(doc.load(STR("<node>&#1;&#32;&#x20;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("\01  "));
}

TEST(parse_escapes_code_exhaustive_dec)
{
	xml_document doc;
	CHECK(doc.load(STR("<node>&#/;&#01;&#2;&#3;&#4;&#5;&#6;&#7;&#8;&#9;&#:;&#a;&#A;&#XA;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&#/;\x1\x2\x3\x4\x5\x6\x7\x8\x9&#:;&#a;&#A;&#XA;"));
}

TEST(parse_escapes_code_exhaustive_hex)
{
	xml_document doc;
	CHECK(doc.load(STR("<node>&#x/;&#x01;&#x2;&#x3;&#x4;&#x5;&#x6;&#x7;&#x8;&#x9;&#x:;&#x@;&#xA;&#xB;&#xC;&#xD;&#xE;&#xF;&#xG;&#x`;&#xa;&#xb;&#xc;&#xd;&#xe;&#xf;&#xg;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&#x/;\x1\x2\x3\x4\x5\x6\x7\x8\x9&#x:;&#x@;\xa\xb\xc\xd\xe\xf&#xG;&#x`;\xa\xb\xc\xd\xe\xf&#xg;"));
}

TEST(parse_escapes_code_restore)
{
	xml_document doc;
	CHECK(doc.load(STR("<node>&#1&#32;&#x1&#32;&#1-&#32;&#x1-&#32;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&#1 &#x1 &#1- &#x1- "));
}

TEST(parse_escapes_char_restore)
{
	xml_document doc;

	CHECK(doc.load(STR("<node>&q&#32;&qu&#32;&quo&#32;&quot&#32;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&q &qu &quo &quot "));

	CHECK(doc.load(STR("<node>&a&#32;&ap&#32;&apo&#32;&apos&#32;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&a &ap &apo &apos "));

	CHECK(doc.load(STR("<node>&a&#32;&am&#32;&amp&#32;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&a &am &amp "));

	CHECK(doc.load(STR("<node>&l&#32;&lt&#32;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&l &lt "));

	CHECK(doc.load(STR("<node>&g&#32;&gt&#32;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&g &gt "));
}

TEST(parse_escapes_unicode)
{
	xml_document doc;
	CHECK(doc.load(STR("<node>&#x03B3;&#x03b3;&#x24B62;</node>"), parse_minimal | parse_escapes));

#ifdef PUGIXML_WCHAR_MODE
	const pugi::char_t* v = doc.child_value(STR("node"));

	unsigned int v2 = v[2];
	size_t wcharsize = sizeof(wchar_t);

	CHECK(v[0] == 0x3b3 && v[1] == 0x3b3 && (wcharsize == 2 ? v[2] == 0xd852 && v[3] == 0xdf62 : v2 == 0x24b62));
#else
	CHECK_STRING(doc.child_value(STR("node")), "\xce\xb3\xce\xb3\xf0\xa4\xad\xa2");
#endif
}

TEST(parse_escapes_error)
{
	xml_document doc;
	CHECK(doc.load(STR("<node>&#x03g;&#ab;&quot</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&#x03g;&#ab;&quot"));

	CHECK(!doc.load(STR("<node id='&#x12")));
	CHECK(!doc.load(STR("<node id='&g")));
	CHECK(!doc.load(STR("<node id='&gt")));
	CHECK(!doc.load(STR("<node id='&l")));
	CHECK(!doc.load(STR("<node id='&lt")));
	CHECK(!doc.load(STR("<node id='&a")));
	CHECK(!doc.load(STR("<node id='&amp")));
	CHECK(!doc.load(STR("<node id='&apos")));
}

TEST(parse_escapes_code_invalid)
{
	xml_document doc;
	CHECK(doc.load(STR("<node>&#;&#x;&;&#x-;&#-;</node>"), parse_minimal | parse_escapes));
	CHECK_STRING(doc.child_value(STR("node")), STR("&#;&#x;&;&#x-;&#-;"));
}

TEST(parse_attribute_spaces)
{
	xml_document doc;
	CHECK(doc.load(STR("<node id1='v1' id2 ='v2' id3= 'v3' id4 = 'v4' id5 \n\r\t = \r\t\n 'v5' />"), parse_minimal));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id1")).value(), STR("v1"));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id2")).value(), STR("v2"));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id3")).value(), STR("v3"));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id4")).value(), STR("v4"));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id5")).value(), STR("v5"));
}

TEST(parse_attribute_quot)
{
	xml_document doc;
	CHECK(doc.load(STR("<node id1='v1' id2=\"v2\"/>"), parse_minimal));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id1")).value(), STR("v1"));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id2")).value(), STR("v2"));
}

TEST(parse_attribute_no_eol_no_wconv)
{
	xml_document doc;
	CHECK(doc.load(STR("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>"), parse_minimal));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id")).value(), STR(" \t\r\rval1  \rval2\r\nval3\nval4\r\r"));
}

TEST(parse_attribute_eol_no_wconv)
{
	xml_document doc;
	CHECK(doc.load(STR("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>"), parse_minimal | parse_eol));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id")).value(), STR(" \t\n\nval1  \nval2\nval3\nval4\n\n"));
}

TEST(parse_attribute_no_eol_wconv)
{
	xml_document doc;
	CHECK(doc.load(STR("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>"), parse_minimal | parse_wconv_attribute));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id")).value(), STR("    val1   val2 val3 val4  "));
}

TEST(parse_attribute_eol_wconv)
{
	xml_document doc;
	CHECK(doc.load(STR("<node id=' \t\r\rval1  \rval2\r\nval3\nval4\r\r'/>"), parse_minimal | parse_eol | parse_wconv_attribute));
	CHECK_STRING(doc.child(STR("node")).attribute(STR("id")).value(), STR("    val1   val2 val3 val4  "));
}

TEST(parse_attribute_variations)
{
	xml_document doc;

	for (int eol = 0; eol < 2; ++eol)
		for (int wconv = 0; wconv < 2; ++wconv)
			for (int escapes = 0; escapes < 2; ++escapes)
			{
				unsigned int flags = parse_minimal;
				
				 flags |= (eol ? parse_eol : 0);
				 flags |= (wconv ? parse_wconv_attribute : 0);
				 flags |= (escapes ? parse_escapes : 0);

				CHECK(doc.load(STR("<node id='1'/>"), flags));
				CHECK_STRING(doc.child(STR("node")).attribute(STR("id")).value(), STR("1"));
			}
}

TEST(parse_attribute_error)
{
	xml_document doc;
	CHECK(doc.load(STR("<node id"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id "), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id  "), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id   "), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id/"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id?/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id=/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id='/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id=\"/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id=\"'/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id='\"/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id='\"/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node #/>"), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("<node#/>"), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("<node id1='1'id2='2'/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node id&='1'/>"), parse_minimal).status == status_bad_attribute);
	CHECK(doc.load(STR("<node &='1'/>"), parse_minimal).status == status_bad_start_element);
}

TEST(parse_tag_single)
{
	xml_document doc;
	CHECK(doc.load(STR("<node/><node /><node\n/>"), parse_minimal));
	CHECK_NODE(doc, STR("<node /><node /><node />"));
}

TEST(parse_tag_hierarchy)
{
	xml_document doc;
	CHECK(doc.load(STR("<node><n1><n2/></n1><n3><n4><n5></n5></n4></n3 \r\n></node>"), parse_minimal));
	CHECK_NODE(doc, STR("<node><n1><n2 /></n1><n3><n4><n5 /></n4></n3></node>"));
}

TEST(parse_tag_error)
{
	xml_document doc;
	CHECK(doc.load(STR("<"), parse_minimal).status == status_unrecognized_tag);
	CHECK(doc.load(STR("<!"), parse_minimal).status == status_unrecognized_tag);
	CHECK(doc.load(STR("<!D"), parse_minimal).status == status_unrecognized_tag);
	CHECK(doc.load(STR("<#"), parse_minimal).status == status_unrecognized_tag);
	CHECK(doc.load(STR("<node#"), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("<node"), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("<node/"), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("<node /"), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("<node / "), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("<node / >"), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("<node/ >"), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("</ node>"), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("</node"), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("</node "), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("<node></ node>"), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("<node></node"), parse_minimal).status == status_bad_end_element);
	CHECK(doc.load(STR("<node></node "), parse_minimal).status == status_bad_end_element);
	CHECK(doc.load(STR("<node></nodes>"), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("<node>"), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("<node/><"), parse_minimal).status == status_unrecognized_tag);
	CHECK(doc.load(STR("<node attr='value'>"), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("</></node>"), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("</node>"), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("</>"), parse_minimal).status == status_end_element_mismatch);
	CHECK(doc.load(STR("<node></node v>"), parse_minimal).status == status_bad_end_element);
	CHECK(doc.load(STR("<node&/>"), parse_minimal).status == status_bad_start_element);
	CHECK(doc.load(STR("<node& v='1'/>"), parse_minimal).status == status_bad_start_element);
}

TEST(parse_declaration_cases)
{
	xml_document doc;
	CHECK(doc.load(STR("<?xml?><?xmL?><?xMl?><?xML?><?Xml?><?XmL?><?XMl?><?XML?>"), parse_minimal | parse_pi));
	CHECK(!doc.first_child());
}

TEST(parse_declaration_attr_cases)
{
	xml_document doc;
	CHECK(doc.load(STR("<?xml ?><?xmL ?><?xMl ?><?xML ?><?Xml ?><?XmL ?><?XMl ?><?XML ?>"), parse_minimal | parse_pi));
	CHECK(!doc.first_child());
}

TEST(parse_declaration_skip)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_pi};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load(STR("<?xml?><?xml version='1.0'?>"), flags));
		CHECK(!doc.first_child());

		CHECK(doc.load(STR("<?xml <tag/> ?>"), flags));
		CHECK(!doc.first_child());
	}
}

TEST(parse_declaration_parse)
{
	xml_document doc;
	CHECK(doc.load(STR("<?xml?><?xml version='1.0'?>"), parse_minimal | parse_declaration));

	xml_node d1 = doc.first_child();
	xml_node d2 = doc.last_child();

	CHECK(d1 != d2);
	CHECK(d1.type() == node_declaration);
	CHECK_STRING(d1.name(), STR("xml"));
	CHECK(d2.type() == node_declaration);
	CHECK_STRING(d2.name(), STR("xml"));
	CHECK_STRING(d2.attribute(STR("version")).value(), STR("1.0"));
}

TEST(parse_declaration_error)
{
	xml_document doc;

	unsigned int flag_sets[] = {parse_minimal, parse_minimal | parse_declaration};

	for (unsigned int i = 0; i < sizeof(flag_sets) / sizeof(flag_sets[0]); ++i)
	{
		unsigned int flags = flag_sets[i];

		CHECK(doc.load(STR("<?xml"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?xml?"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?xml>"), flags).status == status_bad_pi);
		CHECK(doc.load(STR("<?xml version='1>"), flags).status == status_bad_pi);
	}
	
	CHECK(doc.load(STR("<?xml version='1?>"), parse_minimal | parse_declaration).status == status_bad_attribute);
	CHECK(doc.load(STR("<foo><?xml version='1'?></foo>"), parse_minimal | parse_declaration).status == status_bad_pi);
}

TEST(parse_empty)
{
	xml_document doc;
	CHECK(doc.load(STR("")) && !doc.first_child());
}
