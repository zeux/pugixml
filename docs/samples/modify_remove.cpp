#include "pugixml.hpp"

#include <iostream>

int main()
{
	pugi::xml_document doc;
	if (!doc.load("<node><description>Simple node</description><param name='id' value='123'/></node>")) return -1;

    //[code_modify_remove
	// remove description node with the whole subtree
	pugi::xml_node node = doc.child("node");
	node.remove_child("description");

	// remove id attribute
	pugi::xml_node param = node.child("param");
	param.remove_attribute("value");

	// we can also remove nodes/attributes by handles
	pugi::xml_attribute id = param.attribute("name");
	param.remove_attribute(id);
	//]

	doc.print(std::cout);
}
