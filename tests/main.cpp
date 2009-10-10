#include "test.hpp"

#include <exception>
#include <stdio.h>

test_runner* test_runner::_tests = 0;

int main()
{
	unsigned int total = 0;
	unsigned int passed = 0;

	for (test_runner* test = test_runner::_tests; test; test = test->_next)
	{
		try
		{
			total++;
			test->run();
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
