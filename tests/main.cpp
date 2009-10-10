#include "../src/pugixml.hpp"

int main()
{
	pugi::xml_document doc;
	doc.load("<node/>");
	doc.select_single_node("node");

	// doc.select_single_node("//"); - fails? why? :)
}
