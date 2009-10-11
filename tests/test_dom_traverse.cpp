#include "common.hpp"

#include <utility>
#include <vector>
#include <iterator>

template <typename I> I move_iter(I base, int n)
{
	std::advance(base, n);
	return base;
}

template <typename T> void generic_bool_ops_test(const T& obj)
{
	T null;

	CHECK(!null);
	CHECK(obj);
	CHECK(!!obj);

	bool b1 = null, b2 = obj;

	CHECK(!b1);
	CHECK(b2);
}

template <typename T> void generic_rel_ops_test(T obj1, T obj2)
{
	T null = T();

	// obj1 < obj2 (we use operator<, but there is no other choice
	if (obj1 > obj2) std::swap(obj1, obj2);

	// operator==
	CHECK(null == null);
	CHECK(obj1 == obj1);
	CHECK(!(null == obj1));
	CHECK(!(null == obj2));
	CHECK(T(null) == null);
	CHECK(T(obj1) == obj1);

	// operator!=
	CHECK(!(null != null));
	CHECK(!(obj1 != obj1));
	CHECK(null != obj1);
	CHECK(null != obj2);
	CHECK(!(T(null) != null));
	CHECK(!(T(obj1) != obj1));

	// operator<
	CHECK(null < obj1);
	CHECK(null < obj2);
	CHECK(obj1 < obj2);
	CHECK(!(null < null));
	CHECK(!(obj1 < obj1));
	CHECK(!(obj1 < null));
	CHECK(!(obj2 < obj1));

	// operator<=
	CHECK(null <= obj1);
	CHECK(null <= obj2);
	CHECK(obj1 <= obj2);
	CHECK(null <= null);
	CHECK(obj1 <= obj1);
	CHECK(!(obj1 <= null));
	CHECK(!(obj2 <= obj1));

	// operator>
	CHECK(obj1 > null);
	CHECK(obj2 > null);
	CHECK(obj2 > obj1);
	CHECK(!(null > null));
	CHECK(!(obj1 > obj1));
	CHECK(!(null > obj1));
	CHECK(!(obj1 > obj2));

	// operator>=
	CHECK(obj1 >= null);
	CHECK(obj2 >= null);
	CHECK(obj2 >= obj1);
	CHECK(null >= null);
	CHECK(obj1 >= obj1);
	CHECK(!(null >= obj1));
	CHECK(!(obj1 >= obj2));
}

template <typename T> void generic_empty_test(const T& obj)
{
	T null;

	CHECK(null.empty());
	CHECK(!obj.empty());
}

TEST_XML(dom_attr_bool_ops, "<node attr='1'/>")
{
	generic_bool_ops_test(doc.child("node").attribute("attr"));
}

TEST_XML(dom_attr_rel_ops, "<node attr1='1' attr2='2'/>")
{
	generic_rel_ops_test(doc.child("node").attribute("attr1"), doc.child("node").attribute("attr2"));
}

TEST_XML(dom_attr_empty, "<node attr='1'/>")
{
	generic_empty_test(doc.child("node").attribute("attr"));
}

TEST_XML(dom_attr_next_previous_attribute, "<node attr1='1' attr2='2' />")
{
	xml_attribute attr1 = doc.child("node").attribute("attr1");
	xml_attribute attr2 = doc.child("node").attribute("attr2");

	CHECK(attr1.next_attribute() == attr2);
	CHECK(attr2.next_attribute() == xml_attribute());
	
	CHECK(attr1.previous_attribute() == xml_attribute());
	CHECK(attr2.previous_attribute() == attr1);
	
	CHECK(xml_attribute().next_attribute() == xml_attribute());
	CHECK(xml_attribute().previous_attribute() == xml_attribute());
}

TEST_XML(dom_attr_name_value, "<node attr='1'/>")
{
	xml_attribute attr = doc.child("node").attribute("attr");

	CHECK_NAME_VALUE(attr, "attr", "1");
	CHECK_NAME_VALUE(xml_attribute(), "", "");
}

TEST_XML(dom_attr_as_int, "<node attr1='1' attr2='-1' attr3='-2147483648' attr4='2147483647'/>")
{
	xml_node node = doc.child("node");

	CHECK(xml_attribute().as_int() == 0);
	CHECK(node.attribute("attr1").as_int() == 1);
	CHECK(node.attribute("attr2").as_int() == -1);
	CHECK(node.attribute("attr3").as_int() == -2147483647 - 1);
	CHECK(node.attribute("attr4").as_int() == 2147483647);
}

TEST_XML(dom_attr_as_uint, "<node attr1='0' attr2='1' attr3='2147483647'/>")
{
	xml_node node = doc.child("node");

	CHECK(xml_attribute().as_uint() == 0);
	CHECK(node.attribute("attr1").as_uint() == 0);
	CHECK(node.attribute("attr2").as_uint() == 1);
	CHECK(node.attribute("attr3").as_uint() == 2147483647);
}

TEST_XML(dom_attr_as_float, "<node attr1='0' attr2='1' attr3='0.12' attr4='-5.1' attr5='3e-4' attr6='3.14159265358979323846'/>")
{
	xml_node node = doc.child("node");

	CHECK(xml_attribute().as_float() == 0);
	CHECK_DOUBLE(node.attribute("attr1").as_float(), 0);
	CHECK_DOUBLE(node.attribute("attr2").as_float(), 1);
	CHECK_DOUBLE(node.attribute("attr3").as_float(), 0.12);
	CHECK_DOUBLE(node.attribute("attr4").as_float(), -5.1);
	CHECK_DOUBLE(node.attribute("attr5").as_float(), 3e-4);
	CHECK_DOUBLE(node.attribute("attr6").as_float(), 3.14159265358979323846);
}

TEST_XML(dom_attr_as_double, "<node attr1='0' attr2='1' attr3='0.12' attr4='-5.1' attr5='3e-4' attr6='3.14159265358979323846'/>")
{
	xml_node node = doc.child("node");

	CHECK(xml_attribute().as_double() == 0);
	CHECK_DOUBLE(node.attribute("attr1").as_double(), 0);
	CHECK_DOUBLE(node.attribute("attr2").as_double(), 1);
	CHECK_DOUBLE(node.attribute("attr3").as_double(), 0.12);
	CHECK_DOUBLE(node.attribute("attr4").as_double(), -5.1);
	CHECK_DOUBLE(node.attribute("attr5").as_double(), 3e-4);
	CHECK_DOUBLE(node.attribute("attr6").as_double(), 3.14159265358979323846);
}

TEST_XML(dom_attr_as_bool, "<node attr1='0' attr2='1' attr3='true' attr4='True' attr5='Yes' attr6='yes' attr7='false'/>")
{
	xml_node node = doc.child("node");

	CHECK(!xml_attribute().as_bool());
	CHECK(!node.attribute("attr1").as_bool());
	CHECK(node.attribute("attr2").as_bool());
	CHECK(node.attribute("attr3").as_bool());
	CHECK(node.attribute("attr4").as_bool());
	CHECK(node.attribute("attr5").as_bool());
	CHECK(node.attribute("attr6").as_bool());
	CHECK(!node.attribute("attr7").as_bool());
}

TEST_XML(dom_attr_iterator, "<node><node1 attr1='0'/><node2 attr1='0' attr2='1'/><node3/></node>")
{
	xml_node node1 = doc.child("node").child("node1");
	xml_node node2 = doc.child("node").child("node2");
	xml_node node3 = doc.child("node").child("node3");

	CHECK(xml_node().attributes_begin() == xml_attribute_iterator());
	CHECK(xml_node().attributes_end() == xml_attribute_iterator());

	CHECK(node1.attributes_begin() == xml_attribute_iterator(node1.attribute("attr1")));
	CHECK(move_iter(node1.attributes_begin(), 1) == node1.attributes_end());
	CHECK(move_iter(node1.attributes_end(), -1) == node1.attributes_begin());
	CHECK(*node1.attributes_begin() == node1.attribute("attr1"));
	CHECK_STRING(node1.attributes_begin()->name(), "attr1");

	CHECK(move_iter(node2.attributes_begin(), 2) == node2.attributes_end());
	CHECK(move_iter(node2.attributes_end(), -2) == node2.attributes_begin());

	CHECK(node3.attributes_begin() == xml_attribute_iterator());
	CHECK(node3.attributes_begin() == node3.attributes_end());

	xml_attribute_iterator it = node2.attribute("attr2");
	xml_attribute_iterator itt = it;

	CHECK(itt++ == it);
	CHECK(itt == node2.attributes_end());

	CHECK(itt-- == node2.attributes_end());
	CHECK(itt == it);

	CHECK(++itt == node2.attributes_end());
	CHECK(itt == node2.attributes_end());

	CHECK(--itt == it);
	CHECK(itt == it);

	CHECK(++itt != it);
}

TEST_XML(dom_node_bool_ops, "<node/>")
{
	generic_bool_ops_test(doc.child("node"));
}

TEST_XML(dom_node_rel_ops, "<node><node1/><node2/></node>")
{
	generic_rel_ops_test(doc.child("node").child("node1"), doc.child("node").child("node2"));
}

TEST_XML(dom_node_empty, "<node/>")
{
	generic_empty_test(doc.child("node"));
}

TEST_XML(dom_node_iterator, "<node><node1><child1/></node1><node2><child1/><child2/></node2><node3/></node>")
{
	xml_node node1 = doc.child("node").child("node1");
	xml_node node2 = doc.child("node").child("node2");
	xml_node node3 = doc.child("node").child("node3");

	CHECK(xml_node().begin() == xml_node_iterator());
	CHECK(xml_node().end() == xml_node_iterator());

	CHECK(node1.begin() == xml_node_iterator(node1.child("child1")));
	CHECK(move_iter(node1.begin(), 1) == node1.end());
	CHECK(move_iter(node1.end(), -1) == node1.begin());
	CHECK(*node1.begin() == node1.child("child1"));
	CHECK_STRING(node1.begin()->name(), "child1");

	CHECK(move_iter(node2.begin(), 2) == node2.end());
	CHECK(move_iter(node2.end(), -2) == node2.begin());

	CHECK(node3.begin() == xml_node_iterator());
	CHECK(node3.begin() == node3.end());

	xml_node_iterator it = node2.child("child2");
	xml_node_iterator itt = it;

	CHECK(itt++ == it);
	CHECK(itt == node2.end());

	CHECK(itt-- == node2.end());
	CHECK(itt == it);

	CHECK(++itt == node2.end());
	CHECK(itt == node2.end());

	CHECK(--itt == it);
	CHECK(itt == it);

	CHECK(++itt != it);
}

TEST_XML(dom_node_parent, "<node><child/></node>")
{
	CHECK(xml_node().parent() == xml_node());
	CHECK(doc.child("node").child("child").parent() == doc.child("node"));
	CHECK(doc.child("node").parent() == doc);
}

TEST_XML(dom_node_root, "<node><child/></node>")
{
	CHECK(xml_node().root() == xml_node());
	CHECK(doc.child("node").child("child").root() == doc);
	CHECK(doc.child("node").root() == doc);
}

TEST_XML_FLAGS(dom_node_type, "<?xml?><?pi?><!--comment--><node>pcdata<![CDATA[cdata]]></node>", parse_default | parse_pi | parse_comments | parse_declaration)
{
	CHECK(xml_node().type() == node_null);
	CHECK(doc.type() == node_document);

	xml_node_iterator it = doc.begin();

	CHECK((it++)->type() == node_declaration);
	CHECK((it++)->type() == node_pi);
	CHECK((it++)->type() == node_comment);
	CHECK((it++)->type() == node_element);

	xml_node_iterator cit = doc.child("node").begin();
	
	CHECK((cit++)->type() == node_pcdata);
	CHECK((cit++)->type() == node_cdata);
}

TEST_XML_FLAGS(dom_node_name_value, "<?xml?><?pi?><!--comment--><node>pcdata<![CDATA[cdata]]></node>", parse_default | parse_pi | parse_comments | parse_declaration)
{
	CHECK_NAME_VALUE(xml_node(), "", "");
	CHECK_NAME_VALUE(doc, "", "");

	xml_node_iterator it = doc.begin();

	CHECK_NAME_VALUE(*it++, "xml", "");
	CHECK_NAME_VALUE(*it++, "pi", "");
	CHECK_NAME_VALUE(*it++, "", "comment");
	CHECK_NAME_VALUE(*it++, "node", "");

	xml_node_iterator cit = doc.child("node").begin();
	
	CHECK_NAME_VALUE(*cit++, "", "pcdata");
	CHECK_NAME_VALUE(*cit++, "", "cdata");
}

TEST_XML(dom_node_child, "<node><child1/><child2/></node>")
{
	CHECK(xml_node().child("n") == xml_node());

	CHECK(doc.child("n") == xml_node());
	CHECK_NAME_VALUE(doc.child("node"), "node", "");
	CHECK(doc.child("node").child("child2") == doc.child("node").last_child());

	CHECK(doc.child_w("n?de") == doc.child("node"));
	CHECK(doc.child_w("n[az]de") == xml_node());
	CHECK(doc.child_w("n[aoz]de") == doc.child("node"));
	CHECK(doc.child_w("*e") == doc.child("node"));
	CHECK(doc.child("node").child_w("*l?[23456789]*") == doc.child("node").child("child2"));
}

TEST_XML(dom_node_attribute, "<node attr1='0' attr2='1'/>")
{
	CHECK(xml_node().attribute("a") == xml_attribute());

	xml_node node = doc.child("node");

	CHECK(node.attribute("n") == xml_attribute());
	CHECK_NAME_VALUE(node.attribute("attr1"), "attr1", "0");
	CHECK(node.attribute("attr2") == node.last_attribute());

	CHECK(node.attribute_w("*tt?[23456789]*") == node.attribute("attr2"));
	CHECK(node.attribute_w("?") == xml_attribute());
}

TEST_XML(dom_node_next_previous_sibling, "<node><child1/><child2/><child3/></node>")
{
	CHECK(xml_node().next_sibling() == xml_node());
	CHECK(xml_node().next_sibling("n") == xml_node());
	CHECK(xml_node().next_sibling_w("n") == xml_node());

	CHECK(xml_node().previous_sibling() == xml_node());
	CHECK(xml_node().previous_sibling("n") == xml_node());
	CHECK(xml_node().previous_sibling_w("n") == xml_node());

	xml_node child1 = doc.child("node").child("child1");
	xml_node child2 = doc.child("node").child("child2");
	xml_node child3 = doc.child("node").child("child3");

	CHECK(child1.next_sibling() == child2);
	CHECK(child3.next_sibling() == xml_node());
	
	CHECK(child1.previous_sibling() == xml_node());
	CHECK(child3.previous_sibling() == child2);
	
	CHECK(child1.next_sibling("child3") == child3);
	CHECK(child1.next_sibling("child") == xml_node());

	CHECK(child3.previous_sibling("child1") == child1);
	CHECK(child3.previous_sibling("child") == xml_node());

	CHECK(child1.next_sibling_w("*[3456789]") == child3);
	CHECK(child1.next_sibling_w("?") == xml_node());
	CHECK(child3.previous_sibling_w("*[3456789]") == xml_node());
	CHECK(child3.previous_sibling_w("?") == xml_node());
}

TEST_XML(dom_node_child_value, "<node><novalue/><child1>value1</child1><child2>value2<n/></child2><child3><![CDATA[value3]]></child3>value4</node>")
{
	CHECK_STRING(xml_node().child_value(), "");
	CHECK_STRING(xml_node().child_value("n"), "");
	CHECK_STRING(xml_node().child_value_w("n"), "");

	xml_node node = doc.child("node");

	CHECK_STRING(node.child_value(), "value4");
	CHECK_STRING(node.child("child1").child_value(), "value1");
	CHECK_STRING(node.child("child2").child_value(), "value2");
	CHECK_STRING(node.child("child3").child_value(), "value3");
	CHECK_STRING(node.child_value("child3"), "value3");
	CHECK_STRING(node.child_value_w("c*[23456789]"), "value2");
	CHECK_STRING(node.child_value_w("*"), ""); // child_value(name) and child_value_w(pattern) do not continue the search if a node w/out value is found first
}

TEST_XML(dom_node_first_last_attribute, "<node attr1='0' attr2='1'/>")
{
	xml_node node = doc.child("node");

	CHECK(node.first_attribute() == node.attribute("attr1"));
	CHECK(node.last_attribute() == node.attribute("attr2"));

	CHECK(xml_node().first_attribute() == xml_attribute());
	CHECK(xml_node().last_attribute() == xml_attribute());

	CHECK(doc.first_attribute() == xml_attribute());
	CHECK(doc.last_attribute() == xml_attribute());
}

TEST_XML(dom_node_first_last_child, "<node><child1/><child2/></node>")
{
	xml_node node = doc.child("node");

	CHECK(node.first_child() == node.child("child1"));
	CHECK(node.last_child() == node.child("child2"));

	CHECK(xml_node().first_child() == xml_node());
	CHECK(xml_node().last_child() == xml_node());

	CHECK(doc.first_child() == node);
	CHECK(doc.last_child() == node);
}

TEST_XML(dom_node_find_child_by_attribute, "<node><child1 attr='value1'/><child2 attr='value2'/><child2 attr='value3'/></node>")
{
	CHECK(xml_node().find_child_by_attribute("name", "attr", "value") == xml_node());
	CHECK(xml_node().find_child_by_attribute_w("name", "attr", "value") == xml_node());
	CHECK(xml_node().find_child_by_attribute("attr", "value") == xml_node());
	CHECK(xml_node().find_child_by_attribute_w("attr", "value") == xml_node());

	xml_node node = doc.child("node");

	CHECK(node.find_child_by_attribute("child2", "attr", "value3") == node.last_child());
	CHECK(node.find_child_by_attribute("child2", "attr3", "value3") == xml_node());
	CHECK(node.find_child_by_attribute("attr", "value2") == node.child("child2"));
	CHECK(node.find_child_by_attribute("attr3", "value") == xml_node());

	CHECK(node.find_child_by_attribute_w("*", "att?", "val*[0123456789]") == node.child("child1"));
	CHECK(node.find_child_by_attribute_w("*", "attr3", "val*[0123456789]") == xml_node());
	CHECK(node.find_child_by_attribute_w("att?", "val*[0123456789]") == node.child("child1"));
	CHECK(node.find_child_by_attribute_w("attr3", "val*[0123456789]") == xml_node());
}

TEST_XML(dom_node_all_elements_by_name, "<node><child><child/><child/></child></node>")
{
	std::vector<xml_node> v;

	v.clear();
	xml_node().all_elements_by_name("node", std::back_inserter(v));
	CHECK(v.empty());

	v.clear();
	xml_node().all_elements_by_name_w("*", std::back_inserter(v));
	CHECK(v.empty());

	v.clear();
	doc.all_elements_by_name("node", std::back_inserter(v));
	CHECK(v.size() == 1 && v[0] == doc.child("node"));

	v.clear();
	doc.all_elements_by_name("child", std::back_inserter(v));
	CHECK(v.size() == 3);
	CHECK(v[0] == doc.child("node").child("child"));
	CHECK(v[1] == doc.child("node").child("child").first_child());
	CHECK(v[2] == doc.child("node").child("child").last_child());

	v.clear();
	doc.all_elements_by_name_w("*", std::back_inserter(v));
	CHECK(v.size() == 4);
	CHECK(v[0] == doc.child("node"));
	CHECK(v[1] == doc.child("node").child("child"));
	CHECK(v[2] == doc.child("node").child("child").first_child());
	CHECK(v[3] == doc.child("node").child("child").last_child());
}

struct find_predicate_const
{
	bool result;

	find_predicate_const(bool result): result(result)
	{
	}

	template <typename T> bool operator()(const T&) const
	{
		return result;
	}
};

struct find_predicate_prefix
{
	const char* prefix;

	find_predicate_prefix(const char* prefix): prefix(prefix)
	{
	}

	template <typename T> bool operator()(const T& obj) const
	{
		return strncmp(obj.name(), prefix, strlen(prefix)) == 0;
	}
};

TEST_XML(dom_node_find_attribute, "<node attr1='0' attr2='1'/>")
{
	CHECK(xml_node().find_attribute(find_predicate_const(true)) == xml_attribute());

	xml_node node = doc.child("node");

	CHECK(doc.find_attribute(find_predicate_const(true)) == xml_attribute());
	CHECK(node.find_attribute(find_predicate_const(true)) == node.first_attribute());
	CHECK(node.find_attribute(find_predicate_const(false)) == xml_attribute());
	CHECK(node.find_attribute(find_predicate_prefix("attr2")) == node.last_attribute());
	CHECK(node.find_attribute(find_predicate_prefix("attr")) == node.first_attribute());
}

TEST_XML(dom_node_find_child, "<node><child1/><child2/></node>")
{
	CHECK(xml_node().find_child(find_predicate_const(true)) == xml_node());

	xml_node node = doc.child("node");

	CHECK(node.child("node").child("child1").find_child(find_predicate_const(true)) == xml_node());
	CHECK(node.find_child(find_predicate_const(true)) == node.first_child());
	CHECK(node.find_child(find_predicate_const(false)) == xml_node());
	CHECK(node.find_child(find_predicate_prefix("child2")) == node.last_child());
	CHECK(node.find_child(find_predicate_prefix("child")) == node.first_child());
}

TEST_XML(dom_node_find_node, "<node><child1/><child2/></node>")
{
	CHECK(xml_node().find_node(find_predicate_const(true)) == xml_node());

	xml_node node = doc.child("node");

	CHECK(node.child("node").child("child1").find_node(find_predicate_const(true)) == xml_node());
	CHECK(node.find_node(find_predicate_const(true)) == node.first_child());
	CHECK(node.find_node(find_predicate_const(false)) == xml_node());
	CHECK(node.find_node(find_predicate_prefix("child2")) == node.last_child());
	CHECK(node.find_node(find_predicate_prefix("child")) == node.first_child());
	CHECK(doc.find_node(find_predicate_prefix("child")) == node.first_child());
	CHECK(doc.find_node(find_predicate_prefix("child2")) == node.last_child());
	CHECK(doc.find_node(find_predicate_prefix("child3")) == xml_node());
}

TEST_XML(dom_node_path, "<node><child1>text<child2/></child1></node>")
{
	CHECK(xml_node().path() == "");
	
	CHECK(doc.path() == "");
	CHECK(doc.child("node").path() == "/node");
	CHECK(doc.child("node").child("child1").path() == "/node/child1");
	CHECK(doc.child("node").child("child1").child("child2").path() == "/node/child1/child2");
	CHECK(doc.child("node").child("child1").first_child().path() == "/node/child1/");
	
	CHECK(doc.child("node").child("child1").path('\\') == "\\node\\child1");
}

TEST_XML(dom_node_first_element_by_path, "<node><child1>text<child2/></child1></node>")
{
	CHECK(xml_node().first_element_by_path("/") == xml_node());
	
	CHECK(doc.first_element_by_path("") == doc);
	CHECK(doc.first_element_by_path("/") == doc);

	CHECK(doc.first_element_by_path("/node/") == doc.child("node"));
	CHECK(doc.first_element_by_path("node/") == doc.child("node"));
	CHECK(doc.first_element_by_path("node") == doc.child("node"));
	CHECK(doc.first_element_by_path("/node") == doc.child("node"));

	CHECK(doc.first_element_by_path("/node/child1/child2").path() == "/node/child1/child2");
	CHECK(doc.first_element_by_path("/node/child2") == xml_node());
	
	CHECK(doc.first_element_by_path("\\node\\child1", '\\') == doc.child("node").child("child1"));

	CHECK(doc.child("node").first_element_by_path("..") == doc);
	CHECK(doc.child("node").first_element_by_path(".") == doc.child("node"));
}

struct test_walker: xml_tree_walker
{
	std::string log;
	unsigned int call_count;
	unsigned int stop_count;

	test_walker(unsigned int stop_count = 0): call_count(0), stop_count(stop_count)
	{
	}

	virtual bool begin(xml_node& node)
	{
		char buffer[32];
		sprintf(buffer, "|%d <%s=%s", depth(), node.name(), node.value());

		log += buffer;

		return ++call_count != stop_count && xml_tree_walker::begin(node);
	}

	virtual bool for_each(xml_node& node)
	{
		char buffer[32];
		sprintf(buffer, "|%d !%s=%s", depth(), node.name(), node.value());
			
		log += buffer;

		return ++call_count != stop_count && xml_tree_walker::end(node);
	}

	virtual bool end(xml_node& node)
	{
		char buffer[32];
		sprintf(buffer, "|%d >%s=%s", depth(), node.name(), node.value());

		log += buffer;

		return ++call_count != stop_count;
	}
};

TEST_XML(dom_node_traverse, "<node><child>text</child></node>")
{
	test_walker walker;

	CHECK(doc.traverse(walker));

	CHECK(walker.call_count == 5);
	CHECK(walker.log == "|-1 <=|0 !node=|1 !child=|2 !=text|-1 >=");
}

TEST_XML(dom_node_traverse_siblings, "<node><child/><child>text</child><child/></node>")
{
	test_walker walker;

	CHECK(doc.traverse(walker));

	CHECK(walker.call_count == 7);
	CHECK(walker.log == "|-1 <=|0 !node=|1 !child=|1 !child=|2 !=text|1 !child=|-1 >=");
}

TEST(dom_node_traverse_empty)
{
	test_walker walker;

	CHECK(xml_node().traverse(walker));

	CHECK(walker.call_count == 2);
	CHECK(walker.log == "|-1 <=|-1 >=");
}

TEST_XML(dom_node_traverse_child, "<node><child>text</child></node>")
{
	test_walker walker;

	CHECK(doc.child("node").traverse(walker));

	CHECK(walker.call_count == 4);
	CHECK(walker.log == "|-1 <node=|0 !child=|1 !=text|-1 >node=");
}

TEST_XML(dom_node_traverse_stop_begin, "<node><child>text</child></node>")
{
	test_walker walker(1);

	CHECK(!doc.traverse(walker));

	CHECK(walker.call_count == 1);
	CHECK(walker.log == "|-1 <=");
}

TEST_XML(dom_node_traverse_stop_for_each, "<node><child>text</child></node>")
{
	test_walker walker(3);

	CHECK(!doc.traverse(walker));

	CHECK(walker.call_count == 3);
	CHECK(walker.log == "|-1 <=|0 !node=|1 !child=");
}

TEST_XML(dom_node_traverse_stop_end, "<node><child>text</child></node>")
{
	test_walker walker(5);

	CHECK(!doc.traverse(walker));

	CHECK(walker.call_count == 5);
	CHECK(walker.log == "|-1 <=|0 !node=|1 !child=|2 !=text|-1 >=");
}

TEST_XML_FLAGS(dom_offset_debug, "<?xml?><?pi?><!--comment--><node>pcdata<![CDATA[cdata]]></node>", parse_default | parse_pi | parse_comments | parse_declaration)
{
	CHECK(xml_node().offset_debug() == -1);
	CHECK(doc.offset_debug() == 0);

	xml_node_iterator it = doc.begin();

	CHECK((it++)->offset_debug() == 2);
	CHECK((it++)->offset_debug() == 9);
	CHECK((it++)->offset_debug() == 17);
	CHECK((it++)->offset_debug() == 28);

	xml_node_iterator cit = doc.child("node").begin();
	
	CHECK((cit++)->offset_debug() == 33);
	CHECK((cit++)->offset_debug() == 48);
}
