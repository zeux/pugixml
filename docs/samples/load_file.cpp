#include "pugixml.hpp"

#include <iostream>

int main()
{
//[code_load_file
    pugi::xml_document doc;

    pugi::xml_parse_result result = doc.load_file("tree.xml");

    std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("mesh").attribute("name").value() << std::endl;
//]
}

// vim:et
