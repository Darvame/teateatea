#include "lua.h"
#include "lualib.h"
#include "stdlib.h"
#include "tea_tcursor.h"

int tea_tcursor_add(struct tea_tcursor *cursor, const char *value, size_t vlen)
{
	if (cursor->size < TEA_TCURSOR_INITIAL) {
		cursor->value[cursor->size] = value;
		cursor->vlen[cursor->size] = vlen;

		++cursor->size;

		return 0;
	}

	struct tea_tcursor_part *current = cursor->current;

	if (!cursor->parts || current->used >= TEA_TCURSOR_WORKLOAD) {
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

void tea_tcursor_dump(lua_State *l, struct tea_tcursor *cursor)
{
	struct tea_tcursor_part *current = cursor->first;
	struct tea_tcursor_part *next;

	lua_createtable(l, cursor->size, 0);

	size_t i, j, count;

	for (count = 0, i = cursor->size > TEA_TCURSOR_INITIAL ? TEA_TCURSOR_INITIAL : cursor->size; i > 0; --i) {
		lua_pushlstring(l, cursor->value[count], cursor->vlen[count]);
		lua_rawseti(l, -2, ++count);
	}

	for (i = 0; i < cursor->parts; ++i) {

		for (j = 0; j < current->used; ++j) {
			lua_pushlstring(l, current->value[j], current->vlen[j]);
			lua_rawseti(l, -2, ++count);
		}

		next = current->next;
		free(current);
		current = next;
	}

	cursor->size = 0;
	cursor->parts = 0;
}

int tea_tcursor_kv_add(struct tea_tcursor_kv *cursor, const char *key, size_t klen, const char *value, size_t vlen)
{
	if (!klen) return 0;

	if (cursor->size < TEA_TCURSOR_KV_INITIAL) {
		cursor->key[cursor->size] = key;
		cursor->klen[cursor->size] = klen;

		cursor->value[cursor->size] = value;
		cursor->vlen[cursor->size] = vlen;

		++cursor->size;

		return 0;
	}

	struct tea_tcursor_kv_part *current = cursor->current;

	if (!cursor->parts || current->used >= TEA_TCURSOR_KV_WORKLOAD) {
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

void tea_tcursor_kv_dump(lua_State *l, struct tea_tcursor_kv *cursor)
{
	struct tea_tcursor_kv_part *current = cursor->first;
	struct tea_tcursor_kv_part *next;

	lua_createtable(l, 0, cursor->size);

	size_t i, j;

	for (j = 0, i = cursor->size > TEA_TCURSOR_KV_INITIAL ? TEA_TCURSOR_KV_INITIAL : cursor->size; j < i; ++j) {
		lua_pushlstring(l, cursor->key[j], cursor->klen[j]);
		lua_pushlstring(l, cursor->value[j], cursor->vlen[j]);
		lua_rawset(l, -3);
	}

	for (i = 0; i < cursor->parts; ++i) {

		for (j = 0; j < current->used; ++j) {
			lua_pushlstring(l, current->key[j], current->klen[j]);
			lua_pushlstring(l, current->value[j], current->vlen[j]);
			lua_rawset(l, -3);
		}

		next = current->next;
		free(current);
		current = next;
	}

	cursor->size = 0;
	cursor->parts = 0;
}
