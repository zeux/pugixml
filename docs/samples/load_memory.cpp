#include "pugixml.hpp"

#include <iostream>

int main()
{
//[code_load_memory_decl
	const char source[] = "<mesh name='sphere'><bounds>0 0 1 1</bounds></mesh>";
	size_t size = sizeof(source);
//]

	pugi::xml_document doc;

	{
	//[code_load_memory_buffer
		// You can use load_buffer to load document from immutable memory block:
		pugi::xml_parse_result result = doc.load_buffer(source, size);
	//]

		std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("mesh").attribute("name").value() << std::endl;
	}

	{
	//[code_load_memory_buffer_inplace
		// You can use load_buffer_inplace to load document from mutable memory block; memory blocks lifetime must exceed that of document
		char* buffer = new char[size];
		memcpy(buffer, source, size);

		// The block can be allocated by any method; the block is modified during parsing
		pugi::xml_parse_result result = doc.load_buffer_inplace(buffer, size);

	//<-
		std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("mesh").attribute("name").value() << std::endl;
	//->
		// You have to destroy the block yourself after the document is no longer used
		delete[] buffer;
	//]
	}

	{
	//[code_load_memory_buffer_inplace_own
		// You can use load_buffer_inplace_own to load document from mutable memory block and to pass the ownership of this block
		// The block has to be allocated via pugixml allocation function - using i.e. operator new here is incorrect
		char* buffer = static_cast<char*>(pugi::get_memory_allocation_function()(size));
		memcpy(buffer, source, size);

		// The block will be deleted by the document
		pugi::xml_parse_result result = doc.load_buffer_inplace_own(buffer, size);
	//]

		std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("mesh").attribute("name").value() << std::endl;
	}

	{
	//[code_load_memory_string
		// You can use load to load document from null-terminated strings, for example literals:
		pugi::xml_parse_result result = doc.load("<mesh name='sphere'><bounds>0 0 1 1</bounds></mesh>");
	//]

		std::cout << "Load result: " << result.description() << ", mesh name: " << doc.child("mesh").attribute("name").value() << std::endl;
	}
}
