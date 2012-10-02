#include "pugilua_lib.h"

#include <pugixml.hpp>
#include <luabridge.h>
#include <RefCountedPtr.h>
#include <string>
#include <iostream>

namespace pugi {
	namespace lua {

		class lxml_xpath_node_set {
		public:
			lxml_xpath_node_set(pugi::xpath_node_set& s):node_set(s) { }
			lxml_xpath_node_set() { }
		public:

			int _type() const {
				return node_set.type();
			}

		public:
			// enums
			static int type_unsorted () { return pugi::xpath_node_set::type_unsorted; }
			static int type_sorted () { return pugi::xpath_node_set::type_sorted; }
			static int type_sorted_reverse () { return pugi::xpath_node_set::type_sorted_reverse; }
		private:
			pugi::xpath_node_set node_set;
		};

		class lxml_parse_result {
		public:
			lxml_parse_result(pugi::xml_parse_result& r):res(r) { }
			lxml_parse_result() { }

		public:
			std::string description() const {
				return res.description();
			}

			bool valid() const {
				return (bool)res;
			}

		private:
			pugi::xml_parse_result res;
		};

		//int const lxml_xpath_node_set::type_unsorted=pugi::xpath_node_set::type_unsorted;
		//int const lxml_xpath_node_set::type_sorted=pugi::xpath_node_set::type_sorted;
		//int const lxml_xpath_node_set::type_sorted_reverse=pugi::xpath_node_set::type_sorted_reverse;

		class lxml_node {
		public:
			lxml_node(pugi::xml_node& n):node(n){}
			lxml_node() { }

		public:
			bool valid() const {
				return (bool)node;
			}

			RefCountedPtr<lxml_node> child(char const* name) {
				return RefCountedPtr<lxml_node>(new lxml_node(node.child(name)));
			}

			std::string name() const {
				return node.name();
			}

			RefCountedPtr<lxml_xpath_node_set> select_nodes(char const* query) {
				try {
					return RefCountedPtr<lxml_xpath_node_set>(new lxml_xpath_node_set(node.select_nodes(query)));
				} catch (pugi::xpath_exception const& e) {
					std::cerr<<"Error: "<<e.what()<<std::endl;
					return RefCountedPtr<lxml_xpath_node_set>(new lxml_xpath_node_set());
				}
			}

		private:
			pugi::xml_node node;
		};

		class lxml_document {
		public:
			RefCountedPtr<lxml_parse_result> load_file(char const* path) {
				return RefCountedPtr<lxml_parse_result>(new lxml_parse_result(doc.load_file(path)));
			}

			// redundant, but defined due to composition up to now
			RefCountedPtr<lxml_node> child(char const* name) {
				return RefCountedPtr<lxml_node>(new lxml_node(doc.child(name)));
			}

			RefCountedPtr<lxml_xpath_node_set> select_nodes(char const* query) {
				try {
					return RefCountedPtr<lxml_xpath_node_set>(new lxml_xpath_node_set(doc.select_nodes(query)));
				} catch (pugi::xpath_exception const& e) {
					std::cerr<<"Error: "<<e.what()<<std::endl;
					return RefCountedPtr<lxml_xpath_node_set>(new lxml_xpath_node_set());
				}
			}

			bool valid() const {
				return (bool)doc;
			}

		private:
			pugi::xml_document doc;
		};

	}
}

void register_pugilua (lua_State* L) {
	using namespace pugi::lua;
	luabridge::getGlobalNamespace(L)
		.beginNamespace("pugi")

		.beginClass<lxml_xpath_node_set>("xpath_node_set")
		.addConstructor<void (*)()>()
		.addProperty("type",&lxml_xpath_node_set::_type)
		.addStaticProperty("type_unsorted",&lxml_xpath_node_set::type_unsorted)
		.addStaticProperty("type_sorted",&lxml_xpath_node_set::type_sorted)
		.addStaticProperty("type_sorted_reverse",&lxml_xpath_node_set::type_sorted_reverse)
		.endClass()

		.beginClass<lxml_parse_result>("xml_parse_result")
		.addConstructor<void (*)()>()
		.addProperty("description",&lxml_parse_result::description)
		.addProperty("valid",&lxml_parse_result::valid)
		.endClass()

		.beginClass<lxml_node>("xml_node")
		.addConstructor<void (*)()>()
		.addProperty("valid",&lxml_node::valid)
		.addProperty("name",&lxml_node::name)
		.addFunction("child",&lxml_node::child)
		.addFunction("select_nodes",&lxml_node::select_nodes)
		.endClass()

		.beginClass<lxml_document>("xml_document")
		.addConstructor<void (*)()>()
		.addProperty("valid",&lxml_document::valid)
		.addFunction("load_file",&lxml_document::load_file)
		.addFunction("child",&lxml_document::child)
		.addFunction("select_nodes",&lxml_document::select_nodes)
		.endClass()

		.endNamespace()
		;
}