#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "tea_tcursor.h"
#include "tea_pack.h"
#include "tea_trim.h"
#include "tea_laux.h"

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

	switch (argc < 9 ? argc : 9) {
		case 9: lua_pop(l, argc - 8); argc = 8;
		case 8: if (lua_toboolean(l, 8)) flag|= TEA_PACK_FLAG_VALUE_MULTI;
		case 7: if (lua_toboolean(l, 7)) flag|= TEA_PACK_FLAG_KEY_MULTI;
		case 6: if (lua_toboolean(l, 6)) flag|= TEA_PACK_FLAG_SPACE_TRIM_VALUE;
		case 5: if (lua_toboolean(l, 5)) flag|= TEA_PACK_FLAG_SPACE_TRIM_KEY;
		case 4: if (lua_toboolean(l, 4)) flag|= TEA_PACK_FLAG_IGNORE_EMPTY;
		case 3: sp = tea_tolstring(l, 3, &spl);
		case 2: eq = tea_tolstring(l, 2, &eql);
		case 1: str = tea_tolstring(l, 1, &len);
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
		case 4: if (lua_toboolean(l, 4)) flag|= TEA_PACK_FLAG_SPACE_TRIM_VALUE;
		case 3: if (lua_toboolean(l, 3)) flag|= TEA_PACK_FLAG_IGNORE_EMPTY;
		case 2: sp = tea_tolstring(l, 2, &spl);
		case 1: str = tea_tolstring(l, 1, &len);
	}

	return tea_pack(l, flag, str, len, sp, spl);
}

static int trim(lua_State *l)
{
	int argc = lua_gettop(l);
	int i;

	size_t len;
	const char *str;

	size_t begin;
	size_t end;

	for (i = 1; i <= argc; ++i) {
		str = tea_tolstring(l, i, &len);

		if (str) {

			begin = 0;
			end = len;

			TEA_PACK_SPACE_TRIM_WORD(str, begin, end);

			if (begin || end < len) {
				lua_pushlstring(l, &str[begin], end - begin);
			} else {
				lua_pushvalue(l, i);
			}

		} else {
			lua_pushnil(l);
		}
	}

	return argc;
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
