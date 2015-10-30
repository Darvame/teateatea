#include "lua.h"

#define TEA_PACK_FLAG_SPACE_TRIM 0x1
#define TEA_PACK_FLAG_MULTI 0x2
#define TEA_PACK_FLAG_IGNORE_EMPTY 0x4

#define TEA_PACK_MULTI_DICT_SIZE 128

#define TEA_PACK_EQ_DEFAULT '='
#define TEA_PACK_SP_DEFAULT ';'

#define TEA_PACK_SAFE_INC(i, len)\
	if (++i >= len) return 0;


#define TEA_PACK_SEEK_WORD_KEY_END(k, i, m, s, l, eq, eql, sp, spl)\
	m = 0;\
	while(m < eql) {\
		if (s[i] == eq[0]) {\
			TEA_PACK_SAFE_INC(i, l);\
			while(++m < eql && s[i] == eq[m]) TEA_PACK_SAFE_INC(i, l);\
			if (m >= eql) continue;\
			i-= m; m = 0;\
		}\
		if (s[i] == sp[0]) {\
			TEA_PACK_SAFE_INC(i, l);\
			while(++m < spl && s[i] == sp[m]) TEA_PACK_SAFE_INC(i, l);\
			m = m == spl ? eql + 1 : 0;\
		} else {\
			TEA_PACK_SAFE_INC(i, l);\
		}\
	}\
	if(m > eql) continue;\
	k = i - eql;

#define TEA_PACK_SEEK_WORD_VALUE_END(v, i, m, s, l, sp, spl)\
	m = 0;\
	while(m < spl && i < l) {\
		if (s[i++] == sp[0]) {\
			m = 0;\
			while(++m < spl && i < l && s[i] == sp[m]) ++i;\
		}\
	}\
	v = i < l ? (i - spl) : (i - m);


#define TEA_PACK_SEEK_KEY_END(k, i, s, l, eq, sp)\
	while(s[i] != eq && s[i] != sp) TEA_PACK_SAFE_INC(i, l);\
	if(s[i] != eq) {++i; continue;}\
	k = i;\
	TEA_PACK_SAFE_INC(i, l);

#define TEA_PACK_SEEK_VALUE_END(v, i, s, l, sp)\
	while((s[i] != sp) && (++i < l));\
	v = i++;


#define TEA_PACK_UCCHAR(c) ((unsigned const char) c)

#define TEA_PACK_MULTI_DICT_INIT(d, i, s, l)\
	for (i = 0; i < l; ++i)\
		if (TEA_PACK_UCCHAR(s[i]) < TEA_PACK_MULTI_DICT_SIZE)\
			d[TEA_PACK_UCCHAR(s[i])] = 1;

#define TEA_PACK_MULTI_INDICT(d, c)\
	(TEA_PACK_UCCHAR(c) < TEA_PACK_MULTI_DICT_SIZE && d[TEA_PACK_UCCHAR(c)])

#define TEA_PACK_SEEK_MULTI_KEY_END(k, i, s, l, eqd, spd)\
	while(!TEA_PACK_MULTI_INDICT(eqd, s[i]) && !TEA_PACK_MULTI_INDICT(spd, s[i]))\
		TEA_PACK_SAFE_INC(i, l);\
	if(!TEA_PACK_MULTI_INDICT(eqd, s[i]))\
		{++i; continue;}\
	k = i;\
	TEA_PACK_SAFE_INC(i, l);

#define TEA_PACK_SEEK_MULTI_VALUE_END(v, i, s, l, spd)\
	while(!TEA_PACK_MULTI_INDICT(spd, s[i]) && (++i < l));\
	v = i++;


#define TEA_PACK_WHILE_ISSPACE(w, c, modify)\
	while(w) { switch(c) {\
		case 0x9: case 0xA: case 0xB:\
		case 0xC: case 0xD: case 0x20:\
		modify; continue;\
	} break; }

#define TEA_PACK_SPACE_TRIM_WORD(s, b, e)\
	TEA_PACK_WHILE_ISSPACE(e > b, s[b], ++b);\
	TEA_PACK_WHILE_ISSPACE(e > b, s[e-1], --e);


int tea_pack_kv(lua_State *, char, const char *, size_t, const char *, size_t, const char *, size_t);
int tea_pack(lua_State *, char, const char *, size_t, const char *, size_t);
