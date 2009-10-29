#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

TEST_XML(xpath_document_order, "<node><child1 attr1='value1' attr2='value2'/><child2 attr1='value1'>test</child2></node>")
{
	CHECK(xml_node().document_order() == 0);
	CHECK(doc.child("node").document_order() == 0);
	CHECK(doc.document_order() == 0);

	doc.precompute_document_order();

	CHECK(doc.document_order() == 1);
	CHECK(doc.child("node").document_order() == 2);
	CHECK(doc.child("node").child("child1").document_order() == 3);
	CHECK(doc.child("node").child("child1").attribute("attr1").document_order() == 4);
	CHECK(doc.child("node").child("child1").attribute("attr2").document_order() == 5);
	CHECK(doc.child("node").child("child2").document_order() == 6);
	CHECK(doc.child("node").child("child2").attribute("attr1").document_order() == 7);
	CHECK(doc.child("node").child("child2").first_child().document_order() == 8);
}

#endif
