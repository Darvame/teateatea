#ifndef TEA_LAUX_H
#define TEA_LAUX_H

#include "lua.h"
#include "lauxlib.h"

// header -> inline only

static inline int meta_tostring(lua_State *l, int obj)
{
	if (luaL_callmeta(l, obj, "__tostring")) {
		lua_replace(l, obj < 0 ? (obj - 1) : obj);
		return 1;
	}

	return 0;
}

static inline const char *tea_tolstring(lua_State *l, int obj, size_t *len)
{
	const char *str = lua_tolstring(l, obj, len);

	if (!str && meta_tostring(l, obj)) {
		str = lua_tolstring(l, obj, len);
	}

	return str;
}

static inline const char *tea_checklstring(lua_State *l, int obj, size_t *len)
{
	const char *str = lua_tolstring(l, obj, len);

	if (str) {
		return str;
	}

	meta_tostring(l, obj);

	return luaL_checklstring(l, obj, len);
}

#endif
