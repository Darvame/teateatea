#ifndef TEA_TCURSOR_H
#define TEA_TCURSOR_H

#include "lua.h"

#define TEA_TCURSOR_WORKLOAD 128
#define TEA_TCURSOR_INITIAL TEA_TCURSOR_WORKLOAD

#define TEA_TCURSOR_KV_WORKLOAD 64
#define TEA_TCURSOR_KV_INITIAL TEA_TCURSOR_KV_WORKLOAD


struct tea_tcursor_part {
	size_t used;
	size_t vlen[TEA_TCURSOR_WORKLOAD];
	const char *value[TEA_TCURSOR_WORKLOAD];
	struct tea_tcursor_part *next;
};

struct tea_tcursor {
	size_t size;
	size_t parts;
	size_t vlen[TEA_TCURSOR_INITIAL];
	const char *value[TEA_TCURSOR_INITIAL];
	struct tea_tcursor_part *first;
	struct tea_tcursor_part *current;
};

int tea_tcursor_add(struct tea_tcursor *, const char *, size_t);
void tea_tcursor_dump(lua_State *, struct tea_tcursor *);

struct tea_tcursor_kv_part {
	size_t used;
	size_t klen[TEA_TCURSOR_KV_WORKLOAD];
	size_t vlen[TEA_TCURSOR_KV_WORKLOAD];
	const char *key[TEA_TCURSOR_KV_WORKLOAD];
	const char *value[TEA_TCURSOR_KV_WORKLOAD];
	struct tea_tcursor_kv_part *next;
};

struct tea_tcursor_kv {
	size_t size;
	size_t parts;
	size_t klen[TEA_TCURSOR_KV_INITIAL];
	size_t vlen[TEA_TCURSOR_KV_INITIAL];
	const char *key[TEA_TCURSOR_KV_INITIAL];
	const char *value[TEA_TCURSOR_KV_INITIAL];
	struct tea_tcursor_kv_part *first;
	struct tea_tcursor_kv_part *current;
};

int tea_tcursor_kv_add(struct tea_tcursor_kv *, const char *, size_t, const char *, size_t);
void tea_tcursor_kv_dump(lua_State *, struct tea_tcursor_kv *);

static inline void tea_tcursor_init(struct tea_tcursor *c)
{
	(c)->size = 0;
	(c)->parts = 0;
}

static inline void tea_tcursor_kv_init(struct tea_tcursor_kv *c)
{
	(c)->size = 0;
	(c)->parts = 0;
}

#endif
