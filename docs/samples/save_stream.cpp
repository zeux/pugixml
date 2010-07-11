#include "pugixml.hpp"

#include <iostream>

int main()
{
    // get a test document
    pugi::xml_document doc;
    doc.load("<foo bar='baz'><call>hey</call></foo>");

    //[code_save_stream
    // save document to standard output
    std::cout << "Document:\n";
    doc.save(std::cout);
    //]
}

// vim:et
