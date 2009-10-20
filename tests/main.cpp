#include "test.hpp"

#include <exception>
#include <stdio.h>

#include <malloc.h>

test_runner* test_runner::_tests = 0;
size_t test_runner::_memory_fail_threshold = 0;

size_t g_memory_total_size = 0;

void* custom_allocate(size_t size)
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

void custom_deallocate(void* ptr)
{
	if (ptr)
	{
		g_memory_total_size -= _msize(ptr);
		
		free(ptr);
	}
}

void replace_memory_management()
{
	// create some document to touch original functions
	{
		pugi::xml_document doc;
		doc.append_child().set_name("node");
	}

	// replace functions
	pugi::set_memory_management_functions(custom_allocate, custom_deallocate);
}

int main()
{
	replace_memory_management();

	unsigned int total = 0;
	unsigned int passed = 0;

	for (test_runner* test = test_runner::_tests; test; test = test->_next)
	{
		try
		{
			total++;

			g_memory_total_size = 0;
			test_runner::_memory_fail_threshold = 0;

			test->run();

			if (g_memory_total_size != 0) throw "Memory leaks found";

			passed++;
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
	}

	unsigned int failed = total - passed;

	if (failed != 0)
		printf("FAILURE: %u out of %u tests failed.\n", failed, total);
	else
		printf("Success: %d tests passed.\n", total);

	return failed;
}
