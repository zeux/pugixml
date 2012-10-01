#include <lua.hpp>
#include <iostream>

#include "pugilua_lib.h"

extern "C" __declspec(dllexport) int luaopen_pugilua (lua_State* L) {
	register_pugilua(L);
	return 1;
}