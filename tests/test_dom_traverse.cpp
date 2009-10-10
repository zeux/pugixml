#include "common.hpp"

#include <utility>

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

	CHECK_STRING(attr.name(), "attr");
	CHECK_STRING(attr.value(), "1");

	CHECK_STRING(xml_attribute().name(), "");
	CHECK_STRING(xml_attribute().value(), "");
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
