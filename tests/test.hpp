#ifndef HEADER_TEST_HPP
#define HEADER_TEST_HPP

#include <string.h>
#include <math.h>
#include <sstream>

#include "../src/pugixml.hpp"

inline bool test_string_equal(const char* lhs, const char* rhs)
{
	return (!lhs || !rhs) ? lhs == rhs : strcmp(lhs, rhs) == 0;
}

template <typename Node> inline bool test_node_name_value(const Node& node, const char* name, const char* value)
{
	return test_string_equal(node.name(), name) && test_string_equal(node.value(), value);
}

inline bool test_node(const pugi::xml_node& node, const char* contents, const char* indent, unsigned int flags)
{
	std::ostringstream oss;
	node.print(oss, indent, flags);

	return oss.str() == contents;
}

inline bool test_xpath_string(const pugi::xml_node& node, const char* query, const char* expected)
{
	pugi::xpath_query q(query);

	return q.evaluate_string(node) == expected;
}

inline bool test_xpath_boolean(const pugi::xml_node& node, const char* query, bool expected)
{
	pugi::xpath_query q(query);

	return q.evaluate_boolean(node) == expected;
}

inline bool test_xpath_number(const pugi::xml_node& node, const char* query, double expected)
{
	pugi::xpath_query q(query);

	return fabs(q.evaluate_number(node) - expected) < 1e-8f;
}

inline bool test_xpath_number_nan(const pugi::xml_node& node, const char* query)
{
	pugi::xpath_query q(query);

	double r = q.evaluate_number(node);

	return r != r;
}

inline bool test_xpath_fail_compile(const char* query)
{
	try
	{
		pugi::xpath_query q(query);
		return false;
	}
	catch (const pugi::xpath_exception& e)
	{
		return true;
	}
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
	static size_t _memory_fail_threshold;
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

#define TEST_XML_FLAGS(name, xml, flags) \
	struct test_fixture_##name \
	{ \
		pugi::xml_document doc; \
		\
		test_fixture_##name() \
		{ \
			CHECK(doc.load(xml, flags)); \
		} \
	}; \
	\
	TEST_FIXTURE(name, test_fixture_##name)

#define TEST_XML(name, xml) TEST_XML_FLAGS(name, xml, pugi::parse_default)

#define CHECK_TEXT(condition, text) if (condition) ; else throw text

#define CHECK(condition) CHECK_TEXT(condition, #condition " is false")
#define CHECK_STRING(value, expected) CHECK_TEXT(test_string_equal(value, expected), #value " is not equal to " #expected)
#define CHECK_DOUBLE(value, expected) CHECK_TEXT(fabs(value - expected) < 1e-6, #value " is not equal to " #expected)
#define CHECK_NAME_VALUE(node, name, value) CHECK_TEXT(test_node_name_value(node, name, value), #node " name/value do not match " #name " and " #value)
#define CHECK_NODE_EX(node, expected, indent, flags) CHECK_TEXT(test_node(node, expected, indent, flags), #node " contents does not match " #expected)
#define CHECK_NODE(node, expected) CHECK_NODE_EX(node, expected, "", pugi::format_raw)

#define CHECK_XPATH_STRING(node, query, expected) CHECK_TEXT(test_xpath_string(node, query, expected), #query " does not evaluate to " #expected " in context " #node)
#define CHECK_XPATH_BOOLEAN(node, query, expected) CHECK_TEXT(test_xpath_boolean(node, query, expected), #query " does not evaluate to " #expected " in context " #node)
#define CHECK_XPATH_NUMBER(node, query, expected) CHECK_TEXT(test_xpath_number(node, query, expected), #query " does not evaluate to " #expected " in context " #node)
#define CHECK_XPATH_NUMBER_NAN(node, query) CHECK_TEXT(test_xpath_number_nan(node, query), #query " does not evaluate to NaN in context " #node)
#define CHECK_XPATH_FAIL(query) CHECK_TEXT(test_xpath_fail_compile(query), #query " should not compile")

#endif
