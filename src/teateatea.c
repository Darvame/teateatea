#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "limits.h"

#include "tea_tcursor.h"
#include "tea_cookie.h"
#include "tea_pack.h"
#include "tea_trim.h"
#include "tea_laux.h"

#define TEA_VERSION 103

static int pack_kv(lua_State *l)
{
	int argc = lua_gettop(l);

	size_t len = 0;
	size_t eql = 0;
	size_t spl = 0;

	const char *eq = NULL;
	const char *sp = NULL;
	const char *str = NULL;

	unsigned char flag = 0;

	switch (argc < 9 ? argc : 9) {
		case 9: lua_pop(l, argc - 8);
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

static int pack_mkv(lua_State *l)
{
	int argc = lua_gettop(l);

	size_t len = 0;
	size_t eql = 0;
	size_t spl = 0;

	const char *eq = NULL;
	const char *sp = NULL;
	const char *str = NULL;

	long long int mflag = 0;

	switch (argc < 5 ? argc : 5) {
		case 5: lua_pop(l, argc - 4);
		case 4: mflag = lua_tonumber(l, 4);
		case 3: sp = tea_tolstring(l, 3, &spl);
		case 2: eq = tea_tolstring(l, 2, &eql);
		case 1: str = tea_tolstring(l, 1, &len);
	}

	return tea_pack_kv(l, (mflag >= 0 && mflag <= UCHAR_MAX) ? (unsigned char) mflag : 0, str, len, eq, eql, sp, spl);
}

static int mask_pack_kv(lua_State *l)
{
	int argc = lua_gettop(l);
	unsigned char flag = 0;

	switch (argc < 5 ? argc : 5) {
		case 5: if (lua_toboolean(l, 5)) flag|= TEA_PACK_FLAG_VALUE_MULTI;
		case 4: if (lua_toboolean(l, 4)) flag|= TEA_PACK_FLAG_KEY_MULTI;
		case 3: if (lua_toboolean(l, 3)) flag|= TEA_PACK_FLAG_SPACE_TRIM_VALUE;
		case 2: if (lua_toboolean(l, 2)) flag|= TEA_PACK_FLAG_SPACE_TRIM_KEY;
		case 1: if (lua_toboolean(l, 1)) flag|= TEA_PACK_FLAG_IGNORE_EMPTY;
	}

	lua_pushnumber(l, flag);
	return 1;
}

static int pack(lua_State *l)
{
	int argc = lua_gettop(l);

	size_t len = 0;
	size_t spl = 0;

	const char *str = NULL;
	const char *sp = NULL;

	unsigned char flag = 0;

	switch (argc < 6 ? argc : 6) {
		case 6: lua_pop(l, argc - 5);
		case 5: if (lua_toboolean(l, 5)) flag|= TEA_PACK_FLAG_VALUE_MULTI;
		case 4: if (lua_toboolean(l, 4)) flag|= TEA_PACK_FLAG_SPACE_TRIM_VALUE;
		case 3: if (lua_toboolean(l, 3)) flag|= TEA_PACK_FLAG_IGNORE_EMPTY;
		case 2: sp = tea_tolstring(l, 2, &spl);
		case 1: str = tea_tolstring(l, 1, &len);
	}

	return tea_pack(l, flag, str, len, sp, spl);
}

static int pack_m(lua_State *l)
{
	int argc = lua_gettop(l);

	size_t len = 0;
	size_t spl = 0;

	const char *str = NULL;
	const char *sp = NULL;

	long long int mflag = 0;

	switch (argc < 4 ? argc : 4) {
		case 4: lua_pop(l, argc - 3);
		case 3: mflag = lua_tonumber(l, 3);
		case 2: sp = tea_tolstring(l, 2, &spl);
		case 1: str = tea_tolstring(l, 1, &len);
	}

	return tea_pack(l, (mflag >= 0 && mflag <= UCHAR_MAX) ? (unsigned char) mflag : 0, str, len, sp, spl);
}

static int mask_pack(lua_State *l)
{
	int argc = lua_gettop(l);
	unsigned char flag = 0;

	switch (argc < 3 ? argc : 3) {
		case 3: if (lua_toboolean(l, 3)) flag|= TEA_PACK_FLAG_VALUE_MULTI;
		case 2: if (lua_toboolean(l, 2)) flag|= TEA_PACK_FLAG_SPACE_TRIM_VALUE;
		case 1: if (lua_toboolean(l, 1)) flag|= TEA_PACK_FLAG_IGNORE_EMPTY;
	}

	lua_pushnumber(l, flag);
	return 1;
}

static int cookie(lua_State *l)
{

	return tea_cookie(l, NULL, 0);
}

static int trim(lua_State *l)
{
	int argc = lua_gettop(l);
	int i;

	size_t len;
	const char *str;

	size_t begin;
	size_t end;

	//if (argc > 1) {
		// check the stack?
	//}

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
	{"mkvpack", pack_mkv},
	{"pack", pack},
	{"mpack", pack_m},
	{"trim", trim},
	{"cookie", cookie},

	{NULL, NULL}
};

static const luaL_Reg mask_list[] = {

	{"kvpack", mask_pack_kv},
	{"pack", mask_pack},

	{NULL, NULL}
};

int luaopen_teateatea(lua_State *l)
{
	const luaL_Reg *api = api_list;

	lua_createtable(l, 0, sizeof(api_list)/sizeof(luaL_Reg) - 1);

	for (; api->name; api++) {
		lua_pushstring(l, api->name);
		lua_pushcfunction(l, api->func);
		lua_rawset(l, -3);
	}

	// meta
	lua_createtable(l, 0, 1);

	// meta __index
	lua_createtable(l, 0, 2);

	// meta __index _VERSION
	lua_pushnumber(l, TEA_VERSION);
	lua_setfield(l, -2, "_VERSION");

	// meta __index mask
	lua_createtable(l, 0, sizeof(mask_list)/sizeof(luaL_Reg) - 1);

	api = mask_list;

	for (; api->name; api++) {
		lua_pushstring(l, api->name);
		lua_pushcfunction(l, api->func);
		lua_rawset(l, -3);
	}

	lua_setfield(l, -2, "mask");

	// meta __index
	lua_setfield(l, -2, "__index");

	lua_setmetatable(l, -2);

	return 1;
}
