/**
 * traverse_tree.cpp
 * 
 * Traverses and prints to standard output a standard XML file using 
 * the pugixml library.
 * 
 * Copyright (c) 2020 Karen Heart
 * 
 * This software is based on pugixml library (http://pugixml.org). 
 * pugixml is Copyright (C) 2006-2018 Arseny Kapoulkine.
 * 
 * MIT License:
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "pugixml.hpp"

#include <iostream>

// add statement that we're using the pugi namespace
using namespace pugi;
using std::cout;
using std::endl;

void printNode( xml_node cur_node, int tabs ); // forward declaration

void printAttributes( xml_node node ){
	xml_attribute cur_attribute= node.first_attribute();
	while ( cur_attribute ){
		cout <<  "; ";
		cout << "Attribute-- name: " << cur_attribute.name() << ", content: " << cur_attribute.value();
		cur_attribute= cur_attribute.next_attribute();
	}
}

void printChildNodes( xml_node parent_node, int tabs ){
	xml_node cur_node= parent_node.first_child();
	while ( cur_node ){
		printNode( cur_node, tabs );
		cur_node= cur_node.next_sibling();
	}
}

void printElementNode( xml_node node ){	
	cout << "Element node-- name: " << node.name();
}

void printPCDataNode( xml_node node ){	
	cout << "PCData node-- content: " << node.value() << endl;
}

void printCDATANode( xml_node node ){	
	cout << "CData node-- content: " << node.value() << endl;
}

void printCommentNode( xml_node node ){	
	cout << "Comment node-- content: " << node.value() << endl;
}

void printPINode( xml_node node ){	
	cout << "PI node-- name: " << node.name() << "; content: " << node.value() << endl;
}

void printDeclarationNode( xml_node node ){	
	cout << "Declaration node-- name: " << node.name();
}

void printDocTypeNode( xml_node node ){	
	cout << "DocType node-- content: " << node.value() << endl;
}

void printNode( xml_node cur_node, int tabs ){
	if ( cur_node.empty() )
		return;
	for( int i= 0; i < tabs; i++ )
		cout << "\t";
	xml_node_type node_type= cur_node.type();
	switch( node_type ){
	case node_element:
		printElementNode( cur_node );
		printAttributes( cur_node );
		cout << endl;
		printChildNodes( cur_node, tabs + 1 );
		break;
	case node_pcdata:
		printPCDataNode( cur_node );
		break;
	case node_cdata:
		printCDATANode( cur_node );
		break;
	case node_comment:
		printCommentNode( cur_node );
		break;
	case node_pi:
		printPINode( cur_node );
		break;
	case node_declaration:
		printDeclarationNode( cur_node );
		printAttributes( cur_node );
		cout << endl;
		break;
	case node_doctype:
		printDocTypeNode( cur_node );
		break;
	}	
}

int main( int argc, char * argv[] ){
	if ( argc < 2 ){
		cout << "Usage: " << argv[0] << " <XML file name>" << endl;
		return 1;
	}
	
    xml_document doc;
    if ( !doc.load_file(argv[1], parse_full & ~parse_escapes ) ){
		cout << "Error occurred parsing file!" << endl; 
		return -1;
	}
    
    xml_node cur_node= doc.first_child();
	while( cur_node ){
		printNode( cur_node, 0 );
		cur_node= cur_node.next_sibling();
	}	
}
