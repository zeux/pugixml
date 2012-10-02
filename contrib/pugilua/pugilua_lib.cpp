#include "pugilua_lib.h"

#include <pugixml.hpp>
#include <luabridge.h>
#include <RefCountedPtr.h>
#include <string>

namespace pugi {
	namespace lua {

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
		.endClass()

		.beginClass<lxml_document>("xml_document")
		.addConstructor<void (*)()>()
		.addProperty("valid",&lxml_document::valid)
		.addFunction("load_file",&lxml_document::load_file)
		.addFunction("child",&lxml_document::child)
		.endClass()

		.endNamespace()
		;
}