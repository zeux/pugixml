#include "common.hpp"

namespace
{
	char buffer[8];
	int allocate_count = 0;
	int deallocate_count = 0;

	void* allocate(size_t size)
	{
		CHECK(size == 8);
		++allocate_count;
		return buffer;
	}

	void deallocate(void* ptr)
	{
		CHECK(ptr == buffer);
		++deallocate_count;
	}
}

TEST(custom_memory_management)
{
	// remember old functions
	allocation_function old_allocate = get_memory_allocation_function();
	deallocation_function old_deallocate = get_memory_deallocation_function();

	// replace functions
	set_memory_management_functions(allocate, deallocate);

	{
		// parse document
		xml_document doc;
		CHECK(doc.load("<node/>"));
	
		CHECK(allocate_count == 1);
		CHECK(deallocate_count == 0);
		CHECK_STRING(buffer, "<node\0>");

		// modify document
		doc.child("node").set_name("foobars");

		CHECK(allocate_count == 2);
		CHECK(deallocate_count == 0);
		CHECK_STRING(buffer, "foobars");
	}

	CHECK(allocate_count == 2);
	CHECK(deallocate_count == 2);
	CHECK_STRING(buffer, "foobars");

	// restore old functions
	set_memory_management_functions(old_allocate, old_deallocate);
}
