#ifndef TEA_PACK_H
#define TEA_PACK_H

#include "lua.h"
#include "limits.h"

#define TEA_PACK_FLAG_MULTI_KEY 0x01
#define TEA_PACK_FLAG_MULTI_VALUE 0x02
#define TEA_PACK_FLAG_MULTI_KEYVALUE (TEA_PACK_FLAG_MULTI_VALUE | TEA_PACK_FLAG_MULTI_KEY)

#define TEA_PACK_FLAG_IGNORE_EMPTY 0x04
#define TEA_PACK_FLAG_SPACE_TRIM_KEY 0x08
#define TEA_PACK_FLAG_SPACE_TRIM_VALUE 0x10
#define TEA_PACK_FLAG_SWAP_EMPTY_KEYVALUE 0x20

#define TEA_PACK_MULTI_DICT_SIZE (UCHAR_MAX + 1)

#define TEA_PACK_EQ_DEFAULT "" // anything you want
#define TEA_PACK_SP_DEFAULT "" // also modify the len bellow

#define TEA_PACK_EQ_DEFAULT_LEN 0 // an empty string
#define TEA_PACK_SP_DEFAULT_LEN 0 // an empty string


int tea_pack_kv(lua_State *, unsigned char, const char *, size_t, const char *, size_t, const char *, size_t);
int tea_pack(lua_State *, unsigned char, const char *, size_t, const char *, size_t);

#endif
