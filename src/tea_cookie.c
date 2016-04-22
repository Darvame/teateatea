#include "lua.h"
#include "lauxlib.h"

#include "tea_tcursor.h"
#include "tea_cookie.h"
#include "tea_trim.h"

int tea_cookie(lua_State *l, const char *str, size_t len)
{
	struct tea_tcursor tab;

	tea_tcursor_init(&tab);



	tea_tcursor_dump(l, &tab);
	return 1;
}
