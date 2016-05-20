#include "lua.h"
#include "lauxlib.h"

#include "tea_tcursor.h"
#include "tea_pack.h"
#include "tea_trim.h"

static const char pack_eq_default[] = TEA_PACK_EQ_DEFAULT;
static const char pack_sp_default[] = TEA_PACK_SP_DEFAULT;

//
// INIT MULTI DICT
//

static inline void init_multi_dict(unsigned char *dict, unsigned const char *ustr, size_t len)
{
	size_t i;

	for (i = 0; i < len; ++i) {
		dict[ustr[i]] = 1;
	}
}

//
// SEEK
//

#define seek_multi_word_value_end seek_word_value_end

static inline size_t seek_word_key_end(size_t *ip, unsigned char *eq_override, const char *str, size_t len, const char *eq, size_t eql, const char *sp, size_t spl)
{
	size_t match = 0;
	size_t i = *ip;

	while(i < len) {
		if (str[i] == eq[0]) {
			++i;

			while(i < len && ++match < eql && str[i] == eq[match]) ++i;

			if (match >= eql) break;

			i-= match;
			match = 0;
	  	}

		if (str[i] == sp[0]) {
			++i;

			while(i < len && ++match < spl && str[i] == sp[match]) ++i;

			if (match >= spl) {
				*eq_override = 1;
				break;
			}

			match = 0;
		} else {
			++i;
		}
	}

	*ip = i;

	return i - match;
}

static inline size_t seek_word_value_end(size_t *ip, const char *str, size_t len, const char *sp, size_t spl)
{
	size_t match = 0;
	size_t i = *ip;

	while(i < len) {
		if (str[i++] == sp[0]) {
			while(++match < spl && i < len && str[i] == sp[match]) ++i;

			if (match >= spl) {
				*ip = i;
				return i - match;
			}

			match = 0;
		}
	}

	*ip = len + 1;

	return len;
}

static inline size_t seek_key_end(size_t *ip, const char *str, size_t len, const char eq, const char sp)
{
	size_t i = *ip;

	while((i < len) && (str[i] != eq) && (str[i] != sp)) ++i;

	*ip = i + !(str[i] != eq);

	return i;
}

static inline size_t seek_value_end(size_t *ip, const char *str, size_t len, const char sp)
{
	size_t i = *ip;

	while((i < len) && (str[i] != sp)) ++i;

	*ip = i + 1;

	return i;
}

static inline size_t seek_multi_keyvalue_end(size_t *ip, unsigned const char *ustr, size_t len, unsigned char *eqd, unsigned char *spd)
{
	size_t i = *ip;

	while((i < len) && !eqd[ustr[i]] && !spd[ustr[i]]) ++i;

	*ip = i + eqd[ustr[i]];

	return i;
}

static inline size_t seek_multi_value_end(size_t *ip, unsigned const char *ustr, size_t len, unsigned char *spd)
{
	size_t i = *ip;

	while((i < len) && !spd[ustr[i]]) ++i;

	*ip = i + 1;

	return i;
}

static inline size_t seek_multi_word_key_end(size_t *ip, const char *str, size_t len, const char *eq, size_t eql, unsigned char *spd)
{
	size_t match = 0;
	size_t i = *ip;

	while(i < len) {
		if (str[i] == eq[0]) {
			++i;

			while(i < len && ++match < eql && str[i] == eq[match]) ++i;

			if (match >= eql) {
				*ip = i;
				return i - match;
			}

			i-= match;
			match = 0;
		}

		if (spd[(unsigned const char)str[i]]) break;

		++i;
	}

	*ip = i;

	return i;
}

static inline size_t seek_multi_word_key_end_reverse(size_t *ip, unsigned char *eq_override, const char *str, size_t len, const char *sp, size_t spl, unsigned char *eqd)
{
	size_t match = 0;
	size_t i = *ip;

	while(i < len) {
		if (str[i] == sp[0]) {
			++i;

			while(i < len && ++match < spl && str[i] == sp[match]) ++i;

			if (match >= spl) {
				*eq_override = 1;
				break;
			}

			i-= match;
			match = 0;
		}

		if (eqd[(unsigned const char) str[i]]) {
			*ip = i + 1;
			return i;
		}

		++i;
	}

	*ip = i;

	return i - match;
}

//
// ADD TCURSOR
//

static inline int add_tcursor(struct tea_tcursor *tab, const char *str, size_t value_begin, size_t value_end, unsigned char empty)
{
	value_end -= value_begin;

	if (!value_end && !empty) {
		return 0;
	}

	return tea_tcursor_add(tab, &str[value_begin], value_end);
}

static inline int add_tcursor_kv(struct tea_tcursor_kv *tab, const char *str, size_t key_begin, size_t key_end, size_t value_begin, size_t value_end, unsigned char empty, unsigned char swap)
{
	value_end -= value_begin;

	if (!value_end && !empty) {
		return 0;
	}

	key_end -= key_begin;

	if (!key_end && swap) {
		return empty ? tea_tcursor_kv_add(tab, &str[value_begin], value_end, &str[key_begin], key_end) : 0;
	}

	return tea_tcursor_kv_add(tab, &str[key_begin], key_end, &str[value_begin], value_end);
}

//
// INIT FLAG
//

static inline void init_flag(unsigned char flag, unsigned char *empty, unsigned char *trim)
{
	*empty = !(flag & TEA_PACK_FLAG_IGNORE_EMPTY);
	*trim = flag & TEA_PACK_FLAG_SPACE_TRIM_VALUE;
}

static inline void init_flag_kv(unsigned char flag, unsigned char *empty, unsigned char *swap_empty, unsigned char *trim_key, unsigned char *trim_value)
{
	*empty = !(flag & TEA_PACK_FLAG_IGNORE_EMPTY);
	*swap_empty = flag & TEA_PACK_FLAG_SWAP_EMPTY_KEYVALUE;
	*trim_key = flag & TEA_PACK_FLAG_SPACE_TRIM_KEY;
	*trim_value = flag & TEA_PACK_FLAG_SPACE_TRIM_VALUE;
}

//
// KV PACK
//

static inline int pack_kv_char(struct tea_tcursor_kv *tab, unsigned char flag, const char *str, size_t len, const char eq, const char sp)
{
	size_t key_begin;
	size_t key_end;
	size_t value_begin;
	size_t value_end;
	size_t i;

	unsigned char empty;
	unsigned char swap_empty;
	unsigned char trim_key;
	unsigned char trim_value;

	init_flag_kv(flag, &empty, &swap_empty, &trim_key, &trim_value);

	for(i = 0; i < len;) {
		// key: begin
		key_begin = i;

		// key: end
		key_end = seek_key_end(&i, str, len, eq, sp);

		// value: begin
		value_begin = i;

		// value: end
		value_end = seek_value_end(&i, str, len, sp);

		// capture
		if (trim_key) {
			TEA_PACK_SPACE_TRIM_WORD(str, key_begin, key_end);
		}

		if (trim_value) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (add_tcursor_kv(tab, str, key_begin, key_end, value_begin, value_end, empty, swap_empty)) {
			return -1;
		}
	}

	return 0;
}

static inline int pack_kv_word(struct tea_tcursor_kv *tab, unsigned char flag, const char *str, size_t len, const char *eq, size_t eql, const char *sp, size_t spl)
{
	size_t key_begin;
	size_t key_end;
	size_t value_begin;
	size_t value_end;
	size_t i;

	unsigned char empty;
	unsigned char swap_empty;
	unsigned char trim_key;
	unsigned char trim_value;

	unsigned char eq_override = 0;

	init_flag_kv(flag, &empty, &swap_empty, &trim_key, &trim_value);

	for(i = 0; i < len;) {
		// key: begin
		key_begin = i;

		// key: end
		key_end = seek_word_key_end(&i, &eq_override, str, len, eq, eql, sp, spl);

		// value: begin
		value_begin = i;

		// value: end
		if (eq_override) { // eq token override?
			value_end = i;
			eq_override = 0;
		} else {
			value_end = seek_word_value_end(&i, str, len, sp, spl);
		}

		// capture
		if (trim_key) {
			TEA_PACK_SPACE_TRIM_WORD(str, key_begin, key_end);
		}

		if (trim_value) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (add_tcursor_kv(tab, str, key_begin, key_end, value_begin, value_end, empty, swap_empty)) {
			return -1;
		}
	}

	return 0;
}

static inline int pack_kv_multi(struct tea_tcursor_kv *tab, unsigned char flag, const char *str, size_t len, const char *eq, size_t eql, const char *sp, size_t spl)
{
	unsigned char sp_dict[TEA_PACK_MULTI_DICT_SIZE] = {};
	unsigned char eq_dict[TEA_PACK_MULTI_DICT_SIZE] = {};

	size_t key_begin;
	size_t key_end;
	size_t value_begin;
	size_t value_end;
	size_t i;

	unsigned char empty;
	unsigned char swap_empty;
	unsigned char trim_key;
	unsigned char trim_value;

	init_flag_kv(flag, &empty, &swap_empty, &trim_key, &trim_value);
	init_multi_dict(sp_dict, (unsigned const char *) sp, spl);
	init_multi_dict(eq_dict, (unsigned const char *) eq, eql);

	for(i = 0; i < len;) {
		// key: begin
		key_begin = i;

		// key: end
		key_end = seek_multi_keyvalue_end(&i, (unsigned const char *) str, len, eq_dict, sp_dict);

		// value: begin
		value_begin = i;

		// value: end
		value_end = seek_multi_value_end(&i, (unsigned const char *) str, len, sp_dict);

		// capture
		if (trim_key) {
			TEA_PACK_SPACE_TRIM_WORD(str, key_begin, key_end);
		}

		if (trim_value) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (add_tcursor_kv(tab, str, key_begin, key_end, value_begin, value_end, empty, swap_empty)) {
			return -1;
		}
	}

	return 0;
}

static inline int pack_kv_multi_key(struct tea_tcursor_kv *tab, unsigned char flag, const char *str, size_t len, const char *eq, size_t eql, const char *sp, size_t spl)
{
	unsigned char eq_dict[TEA_PACK_MULTI_DICT_SIZE] = {};

	size_t key_begin;
	size_t key_end;
	size_t value_begin;
	size_t value_end;
	size_t i;

	unsigned char empty;
	unsigned char swap_empty;
	unsigned char trim_key;
	unsigned char trim_value;

	unsigned char eq_override = 0;

	init_flag_kv(flag, &empty, &swap_empty, &trim_key, &trim_value);
	init_multi_dict(eq_dict, (unsigned const char *) eq, eql);

	for(i = 0; i < len;) {
		// key: begin
		key_begin = i;

		// key: end
		key_end = seek_multi_word_key_end_reverse(&i, &eq_override, str, len, sp, spl, eq_dict);

		// value: begin
		value_begin = i;

		// value: end
		if (eq_override) { // eq token override?
			value_end = i;
			eq_override = 0;
		} else {
			value_end = seek_multi_word_value_end(&i, str, len, sp, spl);
		}

		// capture
		if (trim_key) {
			TEA_PACK_SPACE_TRIM_WORD(str, key_begin, key_end);
		}

		if (trim_value) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (add_tcursor_kv(tab, str, key_begin, key_end, value_begin, value_end, empty, swap_empty)) {
			return -1;
		}
	}

	return 0;
}

static inline int pack_kv_multi_value(struct tea_tcursor_kv *tab, unsigned char flag, const char *str, size_t len, const char *eq, size_t eql, const char *sp, size_t spl)
{
	unsigned char sp_dict[TEA_PACK_MULTI_DICT_SIZE] = {};

	size_t key_begin;
	size_t key_end;
	size_t value_begin;
	size_t value_end;
	size_t i;

	unsigned char empty;
	unsigned char swap_empty;
	unsigned char trim_key;
	unsigned char trim_value;

	init_flag_kv(flag, &empty, &swap_empty, &trim_key, &trim_value);
	init_multi_dict(sp_dict, (unsigned const char *) sp, spl);

	for(i = 0; i < len;) {
		// key: begin
		key_begin = i;

		// key: end
		key_end = seek_multi_word_key_end(&i, str, len, eq, eql, sp_dict);

		// value: begin
		value_begin = i;

		// value: end
		value_end = seek_multi_value_end(&i, (unsigned const char *) str, len, sp_dict);

		// capture
		if (trim_key) {
			TEA_PACK_SPACE_TRIM_WORD(str, key_begin, key_end);
		}

		if (trim_value) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (add_tcursor_kv(tab, str, key_begin, key_end, value_begin, value_end, empty, swap_empty)) {
			return -1;
		}
	}

	return 0;
}

//
// PACK
//

static inline int pack_char(struct tea_tcursor *tab, unsigned char flag, const char *str, size_t len, const char sp)
{
	size_t value_begin;
	size_t value_end;
	size_t i;

	unsigned char empty;
	unsigned char trim;

	init_flag(flag, &empty, &trim);

	for (i = 0; i <= len;) {
		// value: begin
		value_begin = i;

		// value: end
		value_end = seek_value_end(&i, str, len, sp);

		// capture
		if (trim) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (add_tcursor(tab, str, value_begin, value_end, empty)) {
			return -1;
		}
	}

	return 0;
}

static inline int pack_word(struct tea_tcursor *tab, unsigned char flag, const char *str, size_t len, const char *sp, size_t spl)
{
	size_t value_begin;
	size_t value_end;
	size_t i;

	unsigned char empty;
	unsigned char trim;

	init_flag(flag, &empty, &trim);

	for(i = 0; i <= len;) {
		// value: begin
		value_begin = i;

		// value: end
		value_end = seek_word_value_end(&i, str, len, sp, spl);

		// capture
		if (trim) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (add_tcursor(tab, str, value_begin, value_end, empty)) {
			return -1;
		}
	}

	return 0;
}

static inline int pack_multi(struct tea_tcursor *tab, unsigned char flag, const char *str, size_t len, const char *sp, size_t spl)
{
	unsigned char sp_dict[TEA_PACK_MULTI_DICT_SIZE] = {};

	size_t value_begin;
	size_t value_end;
	size_t i;

	unsigned char empty;
	unsigned char trim;

	init_flag(flag, &empty, &trim);
	init_multi_dict(sp_dict, (unsigned const char *) sp, spl);

	for(i = 0; i <= len;) {
		// value: begin
		value_begin = i;

		// value: end
		value_end = seek_multi_value_end(&i, (unsigned const char *) str, len, sp_dict);

		// capture
		if (trim) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (add_tcursor(tab, str, value_begin, value_end, empty)) {
			return -1;
		}
	}

	return 0;
}

//
// EXT
//

int tea_pack(lua_State *l, unsigned char flag, const char *str, size_t len, const char *sp, size_t spl)
{
	struct tea_tcursor tab;

	tea_tcursor_init(&tab);

	int stat;

	if (!sp) {
		sp = pack_sp_default;
		spl = TEA_PACK_SP_DEFAULT_LEN;
	}

	if(spl < 2) {
		stat = pack_char(&tab, flag, str, len, *sp);
	} else {
		if (flag & TEA_PACK_FLAG_MULTI_VALUE) {
			stat = pack_multi(&tab, flag, str, len, sp, spl);
		} else {
			stat = pack_word(&tab, flag, str, len, sp, spl);
		}
	}

	tea_tcursor_dump(l, &tab);

	if (stat) {
		luaL_error(l, "unable to perform a %s pack (code: %d)", "value", stat);
	}

	return 1;
}

int tea_pack_kv(lua_State *l, unsigned char flag, const char *str, size_t len, const char *eq, size_t eql, const char *sp, size_t spl)
{
	struct tea_tcursor_kv tab;

	tea_tcursor_kv_init(&tab);

	int stat;

	if (!eq) {
		eq = pack_eq_default;
		eql = TEA_PACK_EQ_DEFAULT_LEN;
	}

	if (!sp) {
		sp = pack_sp_default;
		spl = TEA_PACK_SP_DEFAULT_LEN;
	}

	if(eql < 2 && spl < 2) { // single key and value seps
		stat = pack_kv_char(&tab, flag, str, len, *eq, *sp);
	} else {
		switch(flag & TEA_PACK_FLAG_MULTI_KEYVALUE) {
			case TEA_PACK_FLAG_MULTI_KEY:
				if (spl > 1)
				 	if (eql > 1) stat = pack_kv_multi_key(&tab, flag, str, len, eq, eql, sp, spl);
					else stat = pack_kv_word(&tab, flag, str, len, eq, eql, sp, spl);
				else stat = pack_kv_multi(&tab, flag, str, len, eq, eql, sp, spl);
				break;
			case TEA_PACK_FLAG_MULTI_VALUE:
				if (eql > 1)
					if (spl > 1) stat = pack_kv_multi_value(&tab, flag, str, len, eq, eql, sp, spl);
					else stat = pack_kv_word(&tab, flag, str, len, eq, eql, sp, spl);
				else stat = pack_kv_multi(&tab, flag, str, len, eq, eql, sp, spl);
				break;
			case TEA_PACK_FLAG_MULTI_KEYVALUE:
				stat = pack_kv_multi(&tab, flag, str, len, eq, eql, sp, spl); break;
			case 0: default:
				stat = pack_kv_word(&tab, flag, str, len, eq, eql, sp, spl); break;
		}
	}

	tea_tcursor_kv_dump(l, &tab);

	if (stat) {
		luaL_error(l, "unable to perform a %s pack (code: %d)", "key-value", stat);
	}

	return 1;
}
