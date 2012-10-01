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

		private:
			pugi::xml_parse_result res;
		};

		class lxml_document {
		public:
			
			RefCountedPtr<lxml_parse_result> load_file(char const* path) {
				return RefCountedPtr<lxml_parse_result>(new lxml_parse_result(doc.load_file(path)));
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
			.endClass()

			.beginClass<lxml_document>("xml_document")
			.addConstructor<void (*)()>()
			.addFunction("load_file",&lxml_document::load_file)
			.endClass()

		.endNamespace()
		;
}