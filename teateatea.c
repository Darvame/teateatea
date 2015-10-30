#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"

#include "tea_tcursor.h"
#include "tea_pack.h"

static int utable(lua_State *l, int obj)
{
	int type = lua_type(l, obj);
	return (type == LUA_TTABLE || type == LUA_TUSERDATA);
}

static void metatable_tostring(lua_State *l, int obj)
{
	if (luaL_callmeta(l, obj, "__tostring")) {
		if (lua_isstring(l, -1)) {
			lua_replace(l, obj - 1);
		} else {
			lua_pop(l, 1);
		}
	}
}

static int pack_kv_call(lua_State *l, char flag)
{
	int argc = lua_gettop(l);

	size_t len = 0;
	size_t eql = 0;
	size_t spl = 0;

	const char *eq = NULL;
	const char *sp = NULL;
	const char *str = NULL;

	char ignore = 0;

	switch (argc <= 4 ? argc : 4) {
		case 4: ignore = lua_toboolean(l, 4) ? TEA_PACK_FLAG_IGNORE_EMPTY : 0;
		case 3: sp = lua_tolstring(l, 3, &spl);
		case 2: eq = lua_tolstring(l, 2, &eql);
		case 1:
			if (utable(l, 1)) {
				metatable_tostring(l, -argc);
			}

			str = lua_tolstring(l, 1, &len);
	}

	return tea_pack_kv(l, flag | ignore, str, len, eq, eql, sp, spl);
}

static int pack_mtkv(lua_State *l)
{
	return pack_kv_call(l, TEA_PACK_FLAG_SPACE_TRIM | TEA_PACK_FLAG_MULTI);
}

static int pack_tkv(lua_State *l)
{
	return pack_kv_call(l, TEA_PACK_FLAG_SPACE_TRIM);
}

static int pack_mkv(lua_State *l)
{
	return pack_kv_call(l, TEA_PACK_FLAG_MULTI);
}

static int pack_kv(lua_State *l)
{
	return pack_kv_call(l, 0);
}


static int pack_call(lua_State *l, char flag)
{
	int argc = lua_gettop(l);

	size_t len = 0;
	size_t spl = 0;

	const char *str = NULL;
	const char *sp = NULL;

	char ignore = 0;

	switch (argc <= 3 ? argc : 3) {
		case 3: ignore = lua_toboolean(l, 3) ? TEA_PACK_FLAG_IGNORE_EMPTY : 0;
		case 2: sp = lua_tolstring(l, 2, &spl);
		case 1:
			if (utable(l, 1)) {
				metatable_tostring(l, -argc);
			}

			str = lua_tolstring(l, 1, &len);
	}

	return tea_pack(l, flag | ignore, str, len, sp, spl);
}

static int pack_mt(lua_State *l)
{
	return pack_call(l, TEA_PACK_FLAG_SPACE_TRIM | TEA_PACK_FLAG_MULTI);
}

static int pack_t(lua_State *l)
{
	return pack_call(l, TEA_PACK_FLAG_SPACE_TRIM);
}

static int pack_m(lua_State *l)
{
	return pack_call(l, TEA_PACK_FLAG_MULTI);
}

static int pack(lua_State *l)
{
	return pack_call(l, 0);
}


static int trim(lua_State *l)
{
	int argc = lua_gettop(l);
	size_t len;

	if (utable(l, 1)) {
		metatable_tostring(l, -argc);
	}

	const char *str = lua_tolstring(l, 1, &len);

	if(!str) {
		lua_pushnil(l);
		return 1;
	}

	size_t begin = 0;
	size_t end = len;

	TEA_PACK_SPACE_TRIM_WORD(str, begin, end);

	if (begin == 0 && end == len) {
		lua_pushvalue(l, 1);
	} else {
		lua_pushlstring(l, &str[begin], end - begin);
	}

	return 1;
}

static const luaL_Reg api_list[] = {
	{"mtkvpack", pack_mtkv},
	{"tkvpack", pack_tkv},
	{"mkvpack", pack_mkv},
	{"kvpack", pack_kv},
	{"mtpack", pack_mt},
	{"mpack", pack_m},
	{"tpack", pack_t},
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

	return 1;
}
