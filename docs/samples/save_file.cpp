#include "pugixml.hpp"

#include <iostream>

int main()
{
    // get a test document
    pugi::xml_document doc;
    doc.load("<foo bar='baz'>hey</foo>");

    //[code_save_file
    // save document to file
    std::cout << "Saving result: " << doc.save_file("save_file_output.xml") << std::endl;
    //]
}

// vim:et
