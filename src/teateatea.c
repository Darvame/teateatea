#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "tea_tcursor.h"
#include "tea_pack.h"

#define obj_from_top(abs, argc) ((abs - 1) - argc)

static int metatable_tostring(lua_State *l, int obj)
{
	if (luaL_callmeta(l, obj, "__tostring")) {
		lua_replace(l, obj - 1);
		return 1;
	}

	return 0;
}

static int pack_kv(lua_State *l)
{
	int argc = lua_gettop(l);

	size_t len = 0;
	size_t eql = 0;
	size_t spl = 0;

	const char *eq = NULL;
	const char *sp = NULL;
	const char *str = NULL;

	char flag = 0;

	switch (argc < 8 ? argc : 8) {
		case 8: lua_pop(l, argc - 7); argc = 7;
		case 7: if (lua_toboolean(l, 7)) flag|= TEA_PACK_FLAG_VALUE_MULTI;
		case 6: if (lua_toboolean(l, 6)) flag|= TEA_PACK_FLAG_KEY_MULTI;
		case 5: if (lua_toboolean(l, 5)) flag|= TEA_PACK_FLAG_SPACE_TRIM;
		case 4: if (lua_toboolean(l, 4)) flag|= TEA_PACK_FLAG_IGNORE_EMPTY;
		case 3:
			sp = lua_tolstring(l, 3, &spl);

			if (!sp && metatable_tostring(l, obj_from_top(3, argc))) {
				sp = lua_tolstring(l, 3, &spl);
			}
		case 2:
			eq = lua_tolstring(l, 2, &eql);

			if (!eq && metatable_tostring(l, obj_from_top(2, argc))) {
				eq = lua_tolstring(l, 2, &eql);
			}
		case 1:
			str = lua_tolstring(l, 1, &len);

			if (!str && metatable_tostring(l, obj_from_top(1, argc))) {
				str = lua_tolstring(l, 1, &len);
			}
	}

	return tea_pack_kv(l, flag, str, len, eq, eql, sp, spl);
}

static int pack(lua_State *l)
{
	int argc = lua_gettop(l);

	size_t len = 0;
	size_t spl = 0;

	const char *str = NULL;
	const char *sp = NULL;

	char flag = 0;

	switch (argc < 6 ? argc : 6) {
		case 6: lua_pop(l, argc - 5); argc = 5;
		case 5: if (lua_toboolean(l, 5)) flag|= TEA_PACK_FLAG_VALUE_MULTI;
		case 4: if (lua_toboolean(l, 4)) flag|= TEA_PACK_FLAG_SPACE_TRIM;
		case 3: if (lua_toboolean(l, 3)) flag|= TEA_PACK_FLAG_IGNORE_EMPTY;
		case 2:
			sp = lua_tolstring(l, 2, &spl);

			if (!sp && metatable_tostring(l, obj_from_top(2, argc))) {
				sp = lua_tolstring(l, 2, &spl);
			}
		case 1:
			str = lua_tolstring(l, 1, &len);

			if (!str && metatable_tostring(l, obj_from_top(1, argc))) {
				str = lua_tolstring(l, 1, &len);
			}
	}

	return tea_pack(l, flag, str, len, sp, spl);
}

static int trim(lua_State *l)
{
	if (lua_gettop(l) > 1) {
		lua_pop(l, lua_gettop(l) - 1);
	}

	size_t len;
	const char *str = lua_tolstring(l, 1, &len);

	if (!str && metatable_tostring(l, -1)) { // argc==1 always here
		str = lua_tolstring(l, 1, &len);
	}

	if(!str) {
		lua_pushnil(l);
		return 1;
	}

	size_t begin = 0;
	size_t end = len;

	TEA_PACK_SPACE_TRIM_WORD(str, begin, end);

	if (begin || end < len) {
		lua_pushlstring(l, &str[begin], end - begin);
	}

	return 1;
}

static const luaL_Reg api_list[] = {

	{"kvpack", pack_kv},
	{"pack", pack},
	{"trim", trim},

	{NULL, NULL}
};

int luaopen_teateatea(lua_State *l)
{
	const luaL_Reg *api = api_list;

	lua_newtable(l);

	for (; api->name; api++) {
		lua_pushstring(l, api->name);
		lua_pushcfunction(l, api->func);
		lua_rawset(l, -3);
	}

	lua_pushstring(l, "_VERSION");
	lua_pushnumber(l, 101);
	lua_rawset(l, -3);

	return 1;
}
