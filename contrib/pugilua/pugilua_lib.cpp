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

			int status() const {
				return res.status;
			}

			int encoding() const {
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

			RefCountedPtr<lxml_node> child(char const* name);

			std::string name() const;

			RefCountedPtr<lxpath_node_set> select_nodes(char const* query);

			bool empty () const;

		private:
			pugi::xml_node node;
		};

		///////////////////
		class lxml_document {
		public:
			RefCountedPtr<lxml_parse_result> load_file(char const* path);

			RefCountedPtr<lxml_node> child(char const* name);

			RefCountedPtr<lxpath_node_set> select_nodes(char const* query);

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

		RefCountedPtr<lxml_node> lxml_node::child(char const* name) {
			return RefCountedPtr<lxml_node>(new lxml_node(node.child(name)));
		}

		std::string lxml_node::name() const {
			return node.name();
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

		///////////////////
		RefCountedPtr<lxml_parse_result> lxml_document::load_file(char const* path) {
			return RefCountedPtr<lxml_parse_result>(new lxml_parse_result(doc.load_file(path)));
		}

		RefCountedPtr<lxml_node> lxml_document::child(char const* name) {
			return RefCountedPtr<lxml_node>(new lxml_node(doc.child(name)));
		}

		RefCountedPtr<lxpath_node_set> lxml_document::select_nodes(char const* query) {
			try {
				return RefCountedPtr<lxpath_node_set>(new lxpath_node_set(doc.select_nodes(query)));
			} catch (pugi::xpath_exception const& e) {
				std::cerr<<"Error: "<<e.what()<<std::endl;
				return RefCountedPtr<lxpath_node_set>(new lxpath_node_set());
			}
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