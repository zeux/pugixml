#include "test.hpp"

TEST_XML(control_chars_skip, "<a>\f\t\n\x0F\x19</a>") {
	CHECK_NODE_EX(doc.first_child(), STR("<a>\t\n</a>\n"), STR(""), pugi::format_default | pugi::format_skip_control_chars);
}

TEST_XML(control_chars_keep, "<a>\f\t\n\x0F\x19</a>") {
	CHECK_NODE_EX(doc.first_child(), STR("<a>&#12;\t\n&#15;&#25;</a>\n"), STR(""), pugi::format_default);
}
