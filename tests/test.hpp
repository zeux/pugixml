#ifndef HEADER_TEST_HPP
#define HEADER_TEST_HPP

#include <string.h>

inline bool test_string_equal(const char* lhs, const char* rhs)
{
	return (!lhs || !rhs) ? lhs == rhs : strcmp(lhs, rhs) == 0;
}

struct test_runner
{
	test_runner(const char* name)
	{
		_name = name;
		_next = _tests;
		_tests = this;
	}

	virtual ~test_runner() {}

	virtual void run() = 0;

	const char* _name;
	test_runner* _next;

	static test_runner* _tests;
};

struct dummy_fixture {};

#define TEST_FIXTURE(name, fixture) \
	struct test_runner_helper_##name: fixture \
	{ \
		void run(); \
	}; \
	static struct test_runner_##name: test_runner \
	{ \
		test_runner_##name(): test_runner(#name) {} \
		\
		virtual void run() \
		{ \
			test_runner_helper_##name helper; \
			helper.run(); \
		} \
	} test_runner_instance_##name; \
	void test_runner_helper_##name::run()

#define TEST(name) TEST_FIXTURE(name, dummy_fixture)

#define TEST_XML(name, xml) \
	struct test_fixture_##name \
	{ \
		pugi::xml_document doc; \
		\
		test_fixture_##name() \
		{ \
			CHECK(doc.load(xml)); \
		} \
	}; \
	\
	TEST_FIXTURE(name, test_fixture_##name)

#define CHECK(condition) if (condition) ; else throw #condition " is false"
#define CHECK_STRING(value, expected) if (test_string_equal(value, expected)) ; else throw #value " is not equal to " #expected

#endif
