#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

TEST_XML(xpath_xalan_match, "<root><x spot='a' num='1'/><x spot='b' num='2'/><x spot='c' num='3'/><x spot='d' num='4'/><x spot='e' num='5'/><x spot='f' num='6'/><x spot='g' num='7'/><x spot='h' num='8'/><x spot='i' num='9'/><x spot='j' num='10'/><x spot='k' num='11'/><x spot='l' num='12'/></root>")
{
	xml_node c = doc.child(STR("root"));

	CHECK_XPATH_NODESET(c, STR("x[(position() mod 2)=1][position() > 3]")) % 21 % 27 % 33;
	CHECK_XPATH_NODESET(c, STR("x[(position() mod 2)=1][position() > 3][position()=2]")) % 27;
	CHECK_XPATH_NODESET(c, STR("x[(position() mod 2) > 0][position() > 3][2]")) % 27;
	CHECK_XPATH_NODESET(c, STR("x[(position() mod 2)=1][position() > 3][last()]")) % 33;
	CHECK_XPATH_NODESET(c, STR("x[(position() mod 2)=1][@num > 5][last()]")) % 33;
	CHECK_XPATH_NODESET(c, STR("x[(@num mod 3)=2][position() > 2][last()]")) % 33;
	CHECK_XPATH_NODESET(c, STR("x[(position() mod 2)=1][2][@num < 10]")) % 9;
	CHECK_XPATH_NODESET(c, STR("x[(((((2*10)-4)+9) div 5) mod 3)]")) % 6;
}

TEST_XML(xpath_xalan_axes_1, "<far-north><north-north-west1/><north-north-west2/><north><near-north><far-west/><west/><near-west/><center center-attr-1='c1' center-attr-2='c2' center-attr-3='c3'><near-south-west/><near-south><south><far-south/></south></near-south><near-south-east/></center><near-east/><east/><far-east/></near-north></north><north-north-east1/><north-north-east2/></far-north>")
{
	xml_node center = doc.select_single_node(STR("//center")).node();

	CHECK_XPATH_NODESET(center, STR("self::*[near-south]")) % 10;
	CHECK_XPATH_NODESET(center, STR("self::*[@center-attr-2]")) % 10;
	CHECK_XPATH_NODESET(center, STR("preceding-sibling::*")) % 9 % 8 % 7;
	CHECK_XPATH_NODESET(center, STR("preceding-sibling::*/following-sibling::*")) % 8 % 9 % 10 % 19 % 20 % 21;
	CHECK_XPATH_NODESET(center, STR("preceding-sibling::*[2]/following-sibling::*")) % 9 % 10 % 19 % 20 % 21;
	CHECK_XPATH_NODESET(center, STR("preceding-sibling::*[2]/following-sibling::*[4]")) % 20;
	CHECK_XPATH_NODESET(center, STR("preceding-sibling::*[2]/following-sibling::*[4]/preceding-sibling::*[5]/following-sibling::*[4]/following-sibling::*[2]")) % 21;
	CHECK_XPATH_NODESET(center, STR("following-sibling::*")) % 19 % 20 % 21;
	CHECK_XPATH_NODESET(center, STR("following-sibling::*/preceding-sibling::*")) % 7 % 8 % 9 % 10 % 19 % 20;
	CHECK_XPATH_NODESET(center, STR("following-sibling::*[2]/preceding-sibling::*")) % 19 % 10 % 9 % 8 % 7;
	CHECK_XPATH_NODESET(center, STR("following-sibling::*[2]/preceding-sibling::*[4]")) % 8;
	CHECK_XPATH_NODESET(center, STR("following-sibling::*[2]/preceding-sibling::*[4]/following-sibling::*[5]/preceding-sibling::*[4]/preceding-sibling::*[2]")) % 7;
	CHECK_XPATH_NODESET(center, STR("following::*[4]/../*[2]")) % 4;
	CHECK_XPATH_NODESET(center, STR("preceding::*[2]/../following::*")) % 22 % 23;
	CHECK_XPATH_NODESET(center, STR("preceding::*[2]/../descendant::*[10]/following-sibling::east")) % 20;
	CHECK_XPATH_NODESET(center, STR("//*")) % 2 % 3 % 4 % 5 % 6 % 7 % 8 % 9 % 10 % 14 % 15 % 16 % 17 % 18 % 19 % 20 % 21 % 22 % 23;
	CHECK_XPATH_NODESET(center, STR("//ancestor::*")) % 2 % 5 % 6 % 10 % 15 % 16;
	CHECK_XPATH_NODESET(center, STR("//*[count(ancestor::*) >= 2]/../parent::*")) % 2 % 5 % 6 % 10 % 15;
	CHECK_XPATH_NODESET(center, STR("//*[count(./*/*) > 0]")) % 2 % 5 % 6 % 10 % 15;
	CHECK_XPATH_NODESET(center, STR("@*/ancestor::*")) % 2 % 5 % 6 % 10;
	CHECK_XPATH_NODESET(center, STR("@*/following::*")) % 14 % 15 % 16 % 17 % 18 % 19 % 20 % 21 % 22 % 23;
	CHECK_XPATH_NODESET(center, STR("@*/preceding::*")) % 3 % 4 % 7 % 8 % 9;
	CHECK_XPATH_NODESET(center, STR("preceding-sibling::*|following-sibling::*")) % 7 % 8 % 9 % 19 % 20 % 21;
	CHECK_XPATH_NODESET(center, STR("(preceding-sibling::*|following-sibling::*)/ancestor::*[last()]/*[last()]")) % 23;
	CHECK_XPATH_NODESET(center, STR(".//near-south/preceding-sibling::*|following-sibling::east/ancestor-or-self::*[2]")) % 6 % 14;
}

#endif
