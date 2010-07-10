#include "pugixml.hpp"

#include <iostream>

int main()
{
	pugi::xml_document doc;
	if (!doc.load_file("xgconsole.xml")) return -1;

//[code_xpath_select
	pugi::xpath_node_set tools = doc.select_nodes("/Profile/Tools/Tool[@AllowRemote='true' and @DeriveCaptionFrom='lastparam']");

	std::cout << "Tools:";

	for (pugi::xpath_node_set::const_iterator it = tools.begin(); it != tools.end(); ++it)
	{
		pugi::xpath_node node = *it;
		std::cout << " " << node.node().attribute("Filename").value();
	}

	pugi::xpath_node build_tool = doc.select_single_node("//Tool[contains(Description, 'build system')]");

	std::cout << "\nBuild tool: " << build_tool.node().attribute("Filename").value() << "\n";
//]
}
