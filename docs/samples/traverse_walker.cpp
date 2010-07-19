#include "pugixml.hpp"

#include <iostream>

const char* node_types[] =
{
    "null", "document", "element", "pcdata", "cdata", "comment", "pi", "declaration"
};

//[code_traverse_walker_impl
struct simple_walker: pugi::xml_tree_walker
{
    virtual bool for_each(pugi::xml_node& node)
    {
        for (int i = 0; i < depth(); ++i) std::cout << "  "; // indentation

        std::cout << node_types[node.type()] << ": name='" << node.name() << "', value='" << node.value() << "'\n";

        return true; // continue traversal
    }
};
//]

int main()
{
    pugi::xml_document doc;
    if (!doc.load_file("tree.xml")) return -1;

    //[code_traverse_walker_traverse
    simple_walker walker;
    doc.traverse(walker);
    //]
}

// vim:et
