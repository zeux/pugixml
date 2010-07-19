#define _SCL_SECURE_NO_WARNINGS

#include "test.hpp"

#include "writer_string.hpp"

#include <math.h>
#include <float.h>

#include <algorithm>
#include <vector>

#ifndef PUGIXML_NO_XPATH
static void build_document_order(std::vector<pugi::xpath_node>& result, pugi::xml_node root)
{
	result.push_back(pugi::xpath_node());

	pugi::xml_node cur = root;

	for (;;)
	{
		result.push_back(cur);

		for (pugi::xml_attribute a = cur.first_attribute(); a; a = a.next_attribute())
			result.push_back(pugi::xpath_node(a, cur));

		if (cur.first_child())
			cur = cur.first_child();
		else if (cur.next_sibling())
			cur = cur.next_sibling();
		else
		{
			while (cur && !cur.next_sibling()) cur = cur.parent();
			cur = cur.next_sibling();

			if (!cur) break;
		}
	}
}
#endif

bool test_string_equal(const pugi::char_t* lhs, const pugi::char_t* rhs)
{
	return (!lhs || !rhs) ? lhs == rhs : pugi::impl::strequal(lhs, rhs);
}

bool test_node(const pugi::xml_node& node, const pugi::char_t* contents, const pugi::char_t* indent, unsigned int flags)
{
	xml_writer_string writer;

	node.print(writer, indent, flags, get_native_encoding());

	return writer.as_string() == contents;
}

#ifndef PUGIXML_NO_XPATH
bool test_xpath_string(const pugi::xml_node& node, const pugi::char_t* query, const pugi::char_t* expected)
{
	pugi::xpath_query q(query);

	return q.evaluate_string(node) == expected;
}

bool test_xpath_boolean(const pugi::xml_node& node, const pugi::char_t* query, bool expected)
{
	pugi::xpath_query q(query);

	return q.evaluate_boolean(node) == expected;
}

#include <stdio.h>

bool test_xpath_number(const pugi::xml_node& node, const pugi::char_t* query, double expected)
{
	pugi::xpath_query q(query);

	double value = q.evaluate_number(node);
	double absolute_error = fabs(value - expected);

	const double tolerance = 1e-15f;
	return absolute_error < tolerance || absolute_error < fabs(expected) * tolerance;
}

bool test_xpath_number_nan(const pugi::xml_node& node, const pugi::char_t* query)
{
	pugi::xpath_query q(query);

	double r = q.evaluate_number(node);

#if defined(_MSC_VER) || defined(__BORLANDC__)
	return _isnan(r) != 0;
#else
	return r != r;
#endif
}

bool test_xpath_fail_compile(const pugi::char_t* query)
{
	try
	{
		pugi::xpath_query q(query);
		return false;
	}
	catch (const pugi::xpath_exception&)
	{
		return true;
	}
}

void xpath_node_set_tester::check(bool condition)
{
	if (!condition)
	{
		test_runner::_failure_message = message;
		longjmp(test_runner::_failure_buffer, 1);
	}
}

xpath_node_set_tester::xpath_node_set_tester(const pugi::xpath_node_set& set, const char* message): last(0), message(message)
{
	result = set;

	// only sort unsorted sets so that we're able to verify reverse order for some axes
	if (result.type() == pugi::xpath_node_set::type_unsorted) result.sort();

	if (result.empty())
	{
		document_order = 0;
		document_size = 0;
	}
	else
	{
		std::vector<pugi::xpath_node> order;
		build_document_order(order, (result[0].attribute() ? result[0].parent() : result[0].node()).root());

		document_order = new pugi::xpath_node[order.size()];
		std::copy(order.begin(), order.end(), document_order);

		document_size = order.size();
	}
}

xpath_node_set_tester::~xpath_node_set_tester()
{
	// check that we processed everything
	check(last == result.size());

	delete[] document_order;
}

xpath_node_set_tester& xpath_node_set_tester::operator%(unsigned int expected)
{
	// check element count
	check(last < result.size());

	// check document order
	check(expected < document_size);
	check(result.begin()[last] == document_order[expected]);

	// continue to the next element
	last++;

	return *this;
}

#endif

bool is_little_endian()
{
	unsigned int ui = 1;
	return *reinterpret_cast<char*>(&ui) == 1;
}

pugi::xml_encoding get_native_encoding()
{
#ifdef PUGIXML_WCHAR_MODE
	return pugi::encoding_wchar;
#else
	return pugi::encoding_utf8;
#endif
}
