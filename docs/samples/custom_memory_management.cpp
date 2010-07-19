#include "pugixml.hpp"

#include <new>

//[code_custom_memory_management_decl
void* custom_allocate(size_t size)
{
    return new (std::nothrow) char[size];
}

void custom_deallocate(void* ptr)
{
    delete[] static_cast<char*>(ptr);
}
//]

int main()
{
//[code_custom_memory_management_call
    pugi::set_memory_management_functions(custom_allocate, custom_deallocate);
//]

    pugi::xml_document doc;
    doc.load("<node/>");
}

// vim:et
