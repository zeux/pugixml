#include "pugilua_lib.h"

#include <pugixml.hpp>
#include <luabridge.h>
#include <RefCountedPtr.h>
#include <string>
#include <iostream>

namespace pugi {
	namespace lua {

		class lxpath_node;
		class lxpath_node_set;

		////////////////////
		class lxml_attribute {
		public:
			lxml_attribute(pugi::xml_attribute const& a):att(a) {}
			lxml_attribute() {}

		public:
			bool valid() const { return (bool)att; }
			bool empty() const { return att.empty(); }
			std::string name() const { return att.name(); }
			std::string value() const { return att.value(); }
			double number() const { return att.as_double(); }
			bool as_bool() const { return att.as_bool(); }

			void set_name(char const* n) { att.set_name(n); }
			void set_value(char const* v) { att.set_value(v); }
			
			RefCountedPtr<lxml_attribute> next_attribute() const {
				return RefCountedPtr<lxml_attribute>(new lxml_attribute(att.next_attribute()));
			}

			RefCountedPtr<lxml_attribute> previous_attribute() const {
				return RefCountedPtr<lxml_attribute>(new lxml_attribute(att.previous_attribute()));
			}

			size_t hash_value() const {
				return att.hash_value();
			}

		private:
			pugi::xml_attribute att;
		};

		///////////////////////
		class lxml_parse_result {
		public:
			lxml_parse_result(pugi::xml_parse_result& r);
			lxml_parse_result();

		public:
			std::string description() const;

			bool valid() const;

			int status() const { //todo: define constants
				return res.status;
			}

			int encoding() const { //todo: define constants
				return res.encoding;
			}

			ptrdiff_t offset() const {
				return res.offset;
			}

		private:
			pugi::xml_parse_result res;
		};

		///////////////
		class lxml_node {
		public:
			lxml_node(pugi::xml_node const& n);
			lxml_node();

		public:
			bool valid() const;

			std::string name() const;
			std::string value() const;

			RefCountedPtr<lxpath_node_set> select_nodes(char const* query);

			bool empty() const;

			int type() const; //todo: define constants

			RefCountedPtr<lxml_attribute> first_attribute() const;
			RefCountedPtr<lxml_attribute> last_attribute() const;

			RefCountedPtr<lxml_node> first_child() const;
			RefCountedPtr<lxml_node> last_child() const;

			RefCountedPtr<lxml_node> parent() const;
			RefCountedPtr<lxml_node> root() const;

			RefCountedPtr<lxml_node> child(char const* name) const;
			RefCountedPtr<lxml_attribute> attribute(char const* name) const;

			RefCountedPtr<lxml_node> next() const;
			RefCountedPtr<lxml_node> previous() const;

			RefCountedPtr<lxml_node> next_sibling(char const* name) const;
			RefCountedPtr<lxml_node> previous_sibling(char const* name) const;

			//todo: text()



		private:
			pugi::xml_node node;
		};

		///////////////////
		class lxml_document {
		public:
			RefCountedPtr<lxml_parse_result> load_file(char const* path);

			RefCountedPtr<lxml_node> lxml_document::root() const;

			bool valid() const;

		private:
			pugi::xml_document doc;
		};

		/////////////////
		class lxpath_node {
		public:
			lxpath_node(pugi::xpath_node const& n);
			lxpath_node();

			bool valid() const;

			RefCountedPtr<lxml_node> node() const;
			RefCountedPtr<lxml_attribute> attribute() const;

		private:
			pugi::xpath_node _node;
		};

		/////////////////////
		class lxpath_node_set {
		public:
			lxpath_node_set(pugi::xpath_node_set& s);
			lxpath_node_set();
		public:

			int type() const;

			size_t size() const;

			// todo: think if 1..size is a better convention for lua
			RefCountedPtr<lxpath_node> get(size_t i);

			void sort(bool reverse);

		public:
			// enums
			static int type_unsorted () { return pugi::xpath_node_set::type_unsorted; }
			static int type_sorted () { return pugi::xpath_node_set::type_sorted; }
			static int type_sorted_reverse () { return pugi::xpath_node_set::type_sorted_reverse; }
		private:
			pugi::xpath_node_set node_set;
		};

	}
}

namespace pugi {
	namespace lua {

		///////////////////////
		lxml_parse_result::lxml_parse_result(pugi::xml_parse_result& r):res(r) { }
		lxml_parse_result::lxml_parse_result() { }

		std::string lxml_parse_result::description() const {
			return res.description();
		}

		bool lxml_parse_result::valid() const {
			return (bool)res;
		}


		///////////////
		lxml_node::lxml_node(pugi::xml_node const& n):node(n){}
		lxml_node::lxml_node() { }

		bool lxml_node::valid() const {
			return (bool)node;
		}

		RefCountedPtr<lxml_node> lxml_node::child(char const* name) const {
			return RefCountedPtr<lxml_node>(new lxml_node(node.child(name)));
		}

		std::string lxml_node::name() const {
			return node.name();
		}

		std::string lxml_node::value() const {
			return node.value();
		}

		RefCountedPtr<lxpath_node_set> lxml_node::select_nodes(char const* query) {
			try {
				return RefCountedPtr<lxpath_node_set>(new lxpath_node_set(node.select_nodes(query)));
			} catch (pugi::xpath_exception const& e) {
				std::cerr<<"Error: "<<e.what()<<std::endl;
				return RefCountedPtr<lxpath_node_set>(new lxpath_node_set());
			}
		}

		bool lxml_node::empty() const {
			return node.empty();
		}

		int lxml_node::type() const {
			return node.type();
		}

		RefCountedPtr<lxml_attribute> lxml_node::first_attribute() const {
			return RefCountedPtr<lxml_attribute>(new lxml_attribute(node.first_attribute()));
		}

		RefCountedPtr<lxml_attribute> lxml_node::last_attribute() const {
			return RefCountedPtr<lxml_attribute>(new lxml_attribute(node.last_attribute()));
		}

		RefCountedPtr<lxml_node> lxml_node::first_child() const {
			return RefCountedPtr<lxml_node>(new lxml_node(node.first_child()));
		}

		RefCountedPtr<lxml_node> lxml_node::last_child() const {
			return RefCountedPtr<lxml_node>(new lxml_node(node.last_child()));
		}

		RefCountedPtr<lxml_node> lxml_node::parent() const {
			return RefCountedPtr<lxml_node>(new lxml_node(node.parent()));
		}

		RefCountedPtr<lxml_node> lxml_node::root() const {
			return RefCountedPtr<lxml_node>(new lxml_node(node.root()));
		}

		RefCountedPtr<lxml_attribute> lxml_node::attribute(char const* name) const {
			return RefCountedPtr<lxml_attribute>(new lxml_attribute(node.attribute(name)));
		}

		RefCountedPtr<lxml_node> lxml_node::next() const {
			return RefCountedPtr<lxml_node>(new lxml_node(node.next_sibling()));
		}

		RefCountedPtr<lxml_node> lxml_node::previous() const {
			return RefCountedPtr<lxml_node>(new lxml_node(node.previous_sibling()));
		}

		RefCountedPtr<lxml_node> lxml_node::next_sibling(char const* name) const {
			return RefCountedPtr<lxml_node>(new lxml_node(node.next_sibling(name)));
		}

		RefCountedPtr<lxml_node> lxml_node::previous_sibling(char const* name) const {
			return RefCountedPtr<lxml_node>(new lxml_node(node.previous_sibling(name)));
		}

		///////////////////
		RefCountedPtr<lxml_parse_result> lxml_document::load_file(char const* path) {
			return RefCountedPtr<lxml_parse_result>(new lxml_parse_result(doc.load_file(path)));
		}

		RefCountedPtr<lxml_node> lxml_document::root() const {
			return RefCountedPtr<lxml_node>(new lxml_node(pugi::xml_node(doc)));
		}

		bool lxml_document::valid() const {
			return (bool)doc;
		}

		
		//////////////////////
		lxpath_node::lxpath_node(pugi::xpath_node const& n):_node(n){}
		lxpath_node::lxpath_node() { }

		bool lxpath_node::valid() const {
			return (bool)_node;
		}

		RefCountedPtr<lxml_node> lxpath_node::node() const {
			return RefCountedPtr<lxml_node>(new lxml_node(_node.node()));
		}

		RefCountedPtr<lxml_attribute> lxpath_node::attribute() const {
			return RefCountedPtr<lxml_attribute>(new lxml_attribute(_node.attribute()));
		}


		/////////////////////
		lxpath_node_set::lxpath_node_set(pugi::xpath_node_set& s):node_set(s) { }
		lxpath_node_set::lxpath_node_set() { }

		int lxpath_node_set::type() const {
			return node_set.type();
		}

		size_t lxpath_node_set::size() const {
			return node_set.size();
		}

		RefCountedPtr<lxpath_node> lxpath_node_set::get(size_t i) {
			return RefCountedPtr<lxpath_node>(new lxpath_node(node_set[i]));
		}

		void lxpath_node_set::sort(bool reverse) {
			node_set.sort(reverse);
		}
	}
}

void register_pugilua (lua_State* L) {
	using namespace pugi::lua;
	luabridge::getGlobalNamespace(L)
		.beginNamespace("pugi")

		.beginClass<lxml_attribute>("xml_attribute")
		.addConstructor<void (*)()>()
		.addProperty("valid",&lxml_attribute::valid)
		.addProperty("empty",&lxml_attribute::empty)
		.addProperty("name",&lxml_attribute::name)
		.addProperty("value",&lxml_attribute::value)
		.addProperty("number",&lxml_attribute::number)
		.addProperty("bool",&lxml_attribute::as_bool)
		.addProperty("hash_value",&lxml_attribute::hash_value)
		.addFunction("set_name",&lxml_attribute::set_name)
		.addFunction("set_value",&lxml_attribute::set_value)
		.addFunction("next_attribute",&lxml_attribute::next_attribute)
		.addFunction("previous_attribute",&lxml_attribute::previous_attribute)
		.endClass()

		.beginClass<lxml_parse_result>("xml_parse_result")
		.addConstructor<void (*)()>()
		.addProperty("description",&lxml_parse_result::description)
		.addProperty("valid",&lxml_parse_result::valid)
		.addProperty("status",&lxml_parse_result::status)
		.addProperty("encoding",&lxml_parse_result::encoding)
		.addProperty("offset",&lxml_parse_result::offset)
		.endClass()

		.beginClass<lxml_node>("xml_node")
		.addConstructor<void (*)()>()
		.addProperty("valid",&lxml_node::valid)
		.addProperty("name",&lxml_node::name)
		.addProperty("value",&lxml_node::value)
		.addProperty("type",&lxml_node::type)
		.addFunction("child",&lxml_node::child)
		.addFunction("select_nodes",&lxml_node::select_nodes)
		.addFunction("first_attribute",&lxml_node::first_attribute)
		.addFunction("last_attribute",&lxml_node::last_attribute)
		.addFunction("first_child",&lxml_node::first_child)
		.addFunction("last_child",&lxml_node::last_child)
		.addFunction("parent",&lxml_node::parent)
		.addFunction("root",&lxml_node::root)
		.addFunction("attribute",&lxml_node::attribute)
		.addFunction("next",&lxml_node::next)
		.addFunction("previous",&lxml_node::previous)
		.addFunction("next_sibling",&lxml_node::next_sibling)
		.addFunction("previous_sibling",&lxml_node::previous_sibling)
		.endClass()

		.beginClass<lxml_document>("xml_document")
		.addConstructor<void (*)()>()
		.addProperty("valid",&lxml_document::valid)
		.addFunction("load_file",&lxml_document::load_file)
		.addFunction("root",&lxml_document::root)
		.endClass()

		.beginClass<lxpath_node>("xpath_node")
		.addConstructor<void (*)()>()
		.addProperty("valid",&lxpath_node::valid)
		.addFunction("node",&lxpath_node::node)
		.addFunction("attribute",&lxpath_node::attribute)
		.endClass()

		.beginClass<lxpath_node_set>("xpath_node_set")
		.addConstructor<void (*)()>()
		.addProperty("type",&lxpath_node_set::type)
		.addProperty("size",&lxpath_node_set::size)
		.addStaticProperty("type_unsorted",&lxpath_node_set::type_unsorted)
		.addStaticProperty("type_sorted",&lxpath_node_set::type_sorted)
		.addStaticProperty("type_sorted_reverse",&lxpath_node_set::type_sorted_reverse)
		.addFunction("get",&lxpath_node_set::get)
		.addFunction("sort",&lxpath_node_set::sort)
		.endClass()

		.endNamespace()
		;
}