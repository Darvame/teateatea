#include "lua.h"
#include "lualib.h"
#include "stdlib.h"
#include "tea_tcursor.h"

int tea_tcursor_add(struct tea_tcursor *cursor, char ignore, const char *value, size_t vlen)
{
	if (!vlen && ignore) return 0;

	struct tea_tcursor_part *current = cursor->current;

	if (!cursor->size || current->used >= TEA_TCURSOR_WORKLOAD) {
		current = (struct tea_tcursor_part*) malloc(sizeof(struct tea_tcursor_part));
		if (current == NULL) return -1;
		current->used = 0;

		if (++cursor->parts == 1) {
			cursor->first = current;
		} else {
			cursor->current->next = current;
		}

		cursor->current = current;
	}

	current->value[current->used] = value;
	current->vlen[current->used] = vlen;

	++current->used;
	++cursor->size;

	return 0;
}

void tea_tcursor_dump(lua_State *L, struct tea_tcursor *cursor)
{
	struct tea_tcursor_part *current = cursor->first;
	struct tea_tcursor_part *next;

	lua_createtable(L, cursor->size, 0);

	size_t i,j;
	size_t count = 0;

	for (i = 0; i < cursor->parts; ++i) {

		for (j = 0; j < current->used; ++j) {
			lua_pushlstring(L, current->value[j], current->vlen[j]);
			lua_rawseti(L, -2, ++count);
		}

		next = current->next;
		free(current);
		current = next;
	}

	cursor->size = 0;
	cursor->parts = 0;
}

int tea_tcursor_kv_add(struct tea_tcursor_kv *cursor, char ignore, const char *key, size_t klen, const char *value, size_t vlen)
{
	if (!klen || (!vlen && ignore)) return 0;

	struct tea_tcursor_kv_part *current = cursor->current;

	if (!cursor->size || current->used >= TEA_TCURSOR_KV_WORKLOAD) {
		current = (struct tea_tcursor_kv_part*) malloc(sizeof(struct tea_tcursor_kv_part));
		if (current == NULL) return -1;
		current->used = 0;

		if (++cursor->parts == 1) {
			cursor->first = current;
		} else {
			cursor->current->next = current;
		}

		cursor->current = current;
	}

	current->key[current->used] = key;
	current->klen[current->used] = klen;

	current->value[current->used] = value;
	current->vlen[current->used] = vlen;

	++current->used;
	++cursor->size;

	return 0;
}

void tea_tcursor_kv_dump(lua_State *L, struct tea_tcursor_kv *cursor)
{
	struct tea_tcursor_kv_part *current = cursor->first;
	struct tea_tcursor_kv_part *next;

	lua_createtable(L, 0, cursor->size);

	size_t i,j;

	for (i = 0; i < cursor->parts; ++i) {

		for (j = 0; j < current->used; ++j) {
			lua_pushlstring(L, current->key[j], current->klen[j]);
			lua_pushlstring(L, current->value[j], current->vlen[j]);
			lua_rawset(L, -3);
		}

		next = current->next;
		free(current);
		current = next;
	}

	cursor->size = 0;
	cursor->parts = 0;
}
