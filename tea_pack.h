#include "lua.h"

#define TEA_PACK_FLAG_KEY_MULTI 0x1
#define TEA_PACK_FLAG_VALUE_MULTI 0x2
#define TEA_PACK_FLAG_KEYVALUE_MULTI (TEA_PACK_FLAG_VALUE_MULTI | TEA_PACK_FLAG_KEY_MULTI)

#define TEA_PACK_FLAG_IGNORE_EMPTY 0x4
#define TEA_PACK_FLAG_SPACE_TRIM 0x8
#define TEA_PACK_FLAG_MULTI 0x10

#define TEA_PACK_MULTI_DICT_SIZE 128

#define TEA_PACK_EQ_DEFAULT '='
#define TEA_PACK_SP_DEFAULT ';'


#define TEA_PACK_SEEK_WORD_KEY_END(k, i, m, s, l, eq, eql, sp, spl)\
	m = 0;\
	while(i < l) {\
		if (s[i] == eq[0]) {\
			++i;\
			while(i < l && ++m < eql && s[i] == eq[m]) ++i;\
			if (m >= eql) break;\
			i-= m;\
			m = 0;\
		}\
		if (s[i] == sp[0]) {\
			++i;\
			while(i < l && ++m < spl && s[i] == sp[m]) ++i;\
			if (m >= spl) { i-= m; m = 0; break; }\
			m = 0;\
		} else {\
			++i;\
		}\
	}\
	k = i - m;

#define TEA_PACK_SEEK_WORD_VALUE_END(v, i, m, s, l, sp, spl)\
	m = 0;\
	while(i < l && m < spl) {\
		if (s[i++] == sp[0]) {\
			m = 0;\
			while(++m < spl && i < l && s[i] == sp[m]) ++i;\
		}\
	}\
	v = i < l ? (i - spl) : (i - m);\
	if (!m) ++i;


#define TEA_PACK_SEEK_KEY_END(k, i, s, l, eq, sp)\
	while((i < l) && (s[i] != eq) && (s[i] != sp)) ++i;\
	if (s[i] != eq) k = i;\
	else k = i++;

#define TEA_PACK_SEEK_VALUE_END(v, i, s, l, sp)\
	while((i < l) && (s[i] != sp)) ++i;\
	v = i++;


#define TEA_PACK_UCCHAR(c) ((unsigned const char) c)

#define TEA_PACK_MULTI_DICT_INIT(d, i, s, l)\
	for (i = 0; i < l; ++i)\
		if (TEA_PACK_UCCHAR(s[i]) < TEA_PACK_MULTI_DICT_SIZE)\
			d[TEA_PACK_UCCHAR(s[i])] = 1;

#define TEA_PACK_MULTI_INDICT(d, c)\
	(TEA_PACK_UCCHAR(c) < TEA_PACK_MULTI_DICT_SIZE && d[TEA_PACK_UCCHAR(c)])

#define TEA_PACK_SEEK_MULTI_KEYVALUE_END(k, i, s, l, eqd, spd)\
	while((i < l) && !TEA_PACK_MULTI_INDICT(eqd, s[i]) && !TEA_PACK_MULTI_INDICT(spd, s[i])) ++i;\
	if (!TEA_PACK_MULTI_INDICT(eqd, s[i])) k = i;\
	else k = i++;\

#define TEA_PACK_SEEK_MULTI_VALUE_END(v, i, s, l, spd)\
	while((i < l) && !TEA_PACK_MULTI_INDICT(spd, s[i])) ++i;\
	v = i++;

#define TEA_PACK_SEEK_MULTI_KEY_END TEA_PACK_SEEK_MULTI_VALUE_END

#define TEA_PACK_SEEK_MULTI_WORD_KEY_END(k, i, m, s, l, eq, eql, spd)\
	m = 0;\
	while(i < l) {\
		if (s[i] == eq[0]) {\
			++i;\
			while(i < l && ++m < eql && s[i] == eq[m]) ++i;\
			if (m >= eql) break;\
			i-= m;\
			m = 0;\
		}\
		if (TEA_PACK_MULTI_INDICT(spd, s[i])) {\
			break;\
		}\
		++i;\
	}\
	k = i - m;

#define TEA_PACK_SEEK_MULTI_WORD_KEY_END_REVERSE(k, i, m, s, l, sp, spl, eqd)\
	m = 0;\
	while(i < l) {\
		if (s[i] == sp[0]) {\
			++i;\
			while(i < l && ++m < spl && s[i] == sp[m]) ++i;\
			if (m >= spl) break;\
			i-= m;\
			m = 0;\
		}\
		if (TEA_PACK_MULTI_INDICT(eqd, s[i])) {\
			++i; m = 1; break;\
		}\
		++i;\
	}\
	k = i - m;

#define TEA_PACK_SEEK_MULTI_WORD_VALUE_END TEA_PACK_SEEK_WORD_VALUE_END

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
