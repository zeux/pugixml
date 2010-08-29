#ifndef PUGIXML_NO_XPATH

#include "common.hpp"

TEST(xpath_variables_type_none)
{
	xpath_variable_set set;

	xpath_variable* var = set.add(STR("target"), xpath_type_none);
	CHECK(!var);
}

TEST(xpath_variables_type_boolean)
{
	xpath_variable_set set;

	xpath_variable* var = set.add(STR("target"), xpath_type_boolean);
	CHECK(var);

	CHECK(var->type() == xpath_type_boolean);
	CHECK_STRING(var->name(), STR("target"));

	CHECK(var->get_boolean() == false);
	CHECK_DOUBLE_NAN(var->get_number());
	CHECK_STRING(var->get_string(), STR(""));
	CHECK(var->get_node_set().empty());

	CHECK(var->set(true));
	CHECK(!var->set(1.0));
	CHECK(!var->set(STR("abc")));
	CHECK(!var->set(xpath_node_set()));

	CHECK(var->get_boolean() == true);
	CHECK_DOUBLE_NAN(var->get_number());
	CHECK_STRING(var->get_string(), STR(""));
	CHECK(var->get_node_set().empty());
}

TEST(xpath_variables_type_number)
{
	xpath_variable_set set;

	xpath_variable* var = set.add(STR("target"), xpath_type_number);
	CHECK(var);

	CHECK(var->type() == xpath_type_number);
	CHECK_STRING(var->name(), STR("target"));

	CHECK(var->get_boolean() == false);
	CHECK_DOUBLE(var->get_number(), 0);
	CHECK_STRING(var->get_string(), STR(""));
	CHECK(var->get_node_set().empty());

	CHECK(!var->set(true));
	CHECK(var->set(1.0));
	CHECK(!var->set(STR("abc")));
	CHECK(!var->set(xpath_node_set()));

	CHECK(var->get_boolean() == false);
	CHECK_DOUBLE(var->get_number(), 1);
	CHECK_STRING(var->get_string(), STR(""));
	CHECK(var->get_node_set().empty());
}

TEST(xpath_variables_type_string)
{
	xpath_variable_set set;

	xpath_variable* var = set.add(STR("target"), xpath_type_string);
	CHECK(var);

	CHECK(var->type() == xpath_type_string);
	CHECK_STRING(var->name(), STR("target"));

	CHECK(var->get_boolean() == false);
	CHECK_DOUBLE_NAN(var->get_number());
	CHECK_STRING(var->get_string(), STR(""));
	CHECK(var->get_node_set().empty());

	CHECK(!var->set(true));
	CHECK(!var->set(1.0));
	CHECK(var->set(STR("abc")));
	CHECK(!var->set(xpath_node_set()));

	CHECK(var->get_boolean() == false);
	CHECK_DOUBLE_NAN(var->get_number());
	CHECK_STRING(var->get_string(), STR("abc"));
	CHECK(var->get_node_set().empty());
}

TEST_XML(xpath_variables_type_node_set, "<node/>")
{
	xpath_variable_set set;

	xpath_variable* var = set.add(STR("target"), xpath_type_node_set);
	CHECK(var);

	CHECK(var->type() == xpath_type_node_set);
	CHECK_STRING(var->name(), STR("target"));

	CHECK(var->get_boolean() == false);
	CHECK_DOUBLE_NAN(var->get_number());
	CHECK_STRING(var->get_string(), STR(""));
	CHECK(var->get_node_set().empty());

	CHECK(!var->set(true));
	CHECK(!var->set(1.0));
	CHECK(!var->set(STR("abc")));
	CHECK(var->set(doc.select_nodes(STR("*"))));

	CHECK(var->get_boolean() == false);
	CHECK_DOUBLE_NAN(var->get_number());
	CHECK_STRING(var->get_string(), STR(""));
	CHECK(var->get_node_set().size() == 1 && var->get_node_set()[0] == doc.first_child());
}

TEST(xpath_variables_set_operations)
{
	xpath_variable_set set;

	xpath_variable* v1 = set.add(STR("var1"), xpath_type_number);
	CHECK(v1);

	xpath_variable* v2 = set.add(STR("var2"), xpath_type_string);
	CHECK(v2);

	CHECK(v1 != v2);

	CHECK(set.add(STR("var1"), xpath_type_number) == v1);
	CHECK(set.add(STR("var2"), xpath_type_string) == v2);
	CHECK(set.add(STR("var2"), xpath_type_node_set) == 0);

	CHECK(set.get(STR("var1")) == v1);
	CHECK(set.get(STR("var2")) == v2);
	CHECK(set.get(STR("var")) == 0);
	CHECK(set.get(STR("var11")) == 0);

	CHECK(static_cast<const xpath_variable_set&>(set).get(STR("var1")) == v1);
	CHECK(static_cast<const xpath_variable_set&>(set).get(STR("var3")) == 0);
}

TEST_XML(xpath_variables_set_operations_set, "<node/>")
{
	xpath_variable_set set;

	xpath_variable* v1 = set.add(STR("var1"), xpath_type_number);
	CHECK(v1);

	xpath_variable* v2 = set.add(STR("var2"), xpath_type_string);
	CHECK(v2);

	CHECK(set.set(STR("var1"), 1.0));
	CHECK_DOUBLE(v1->get_number(), 1.0);

	CHECK(set.set(STR("var2"), STR("value")));
	CHECK_STRING(v2->get_string(), STR("value"));

	CHECK(!set.set(STR("var1"), true));

	CHECK(set.set(STR("var3"), doc.select_nodes(STR("*"))));

	xpath_variable* v3 = set.get(STR("var3"));

	CHECK(v3);
	CHECK(v3->type() == xpath_type_node_set);
	CHECK(v3->get_node_set().size() == 1);
}

TEST(xpath_variables_set_out_of_memory)
{
	test_runner::_memory_fail_threshold = 1;

	xpath_variable_set set;

	xpath_variable* var = set.add(STR("target"), xpath_type_number);
	CHECK(!var);
}

TEST(xpath_variable_out_of_memory)
{
	test_runner::_memory_fail_threshold = 64;

	xpath_variable_set set;

	xpath_variable* var = set.add(STR("target"), xpath_type_string);
	CHECK(var);

	CHECK(!var->set(STR("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")));
}

#endif
