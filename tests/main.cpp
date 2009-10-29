#include "test.hpp"

#include <exception>
#include <stdio.h>

#include <malloc.h>

test_runner* test_runner::_tests = 0;
size_t test_runner::_memory_fail_threshold = 0;

static size_t g_memory_total_size = 0;

static void* custom_allocate(size_t size)
{
	if (test_runner::_memory_fail_threshold > 0 && test_runner::_memory_fail_threshold < size)
		return 0;
	else
	{
		void* ptr = malloc(size);

		g_memory_total_size += _msize(ptr);
		
		return ptr;
	}
}

static void custom_deallocate(void* ptr)
{
	if (ptr)
	{
		g_memory_total_size -= _msize(ptr);
		
		free(ptr);
	}
}

static void replace_memory_management()
{
	// create some document to touch original functions
	{
		pugi::xml_document doc;
		doc.append_child().set_name("node");
	}

	// replace functions
	pugi::set_memory_management_functions(custom_allocate, custom_deallocate);
}

static bool run_test(test_runner* test)
{
	try
	{
		g_memory_total_size = 0;
		test_runner::_memory_fail_threshold = 0;

		test->run();

		if (g_memory_total_size != 0) throw "Memory leaks found";

		return true;
	}
	catch (const std::exception& e)
	{
		printf("Test %s failed: exception %s\n", test->_name, e.what());
	}
	catch (const char* e)
	{
		printf("Test %s failed: %s\n", test->_name, e);
	}
	catch (...)
	{
		printf("Test %s failed for unknown reason\n", test->_name);
	}

	return false;
}

int main()
{
	replace_memory_management();

	unsigned int total = 0;
	unsigned int passed = 0;

	test_runner* test = 0; // gcc3 "variable might be used uninitialized in this function" bug workaround

	for (test = test_runner::_tests; test; test = test->_next)
	{
		total++;
		passed += run_test(test);
	}

	unsigned int failed = total - passed;

	if (failed != 0)
		printf("FAILURE: %u out of %u tests failed.\n", failed, total);
	else
		printf("Success: %u tests passed.\n", total);

	return failed;
}
