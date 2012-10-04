#include <lua.hpp>
#include <iostream>

#include "pugilua_lib.h"

#ifdef _MSC_VER
#define PUGILUA __declspec(dllexport)
#else
#define PUGILUA
#endif

extern "C" PUGILUA int luaopen_pugilua (lua_State* L) {
	register_pugilua(L);
	return 1;
}
