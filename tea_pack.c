#include "lua.h"
#include "tea_tcursor.h"
#include "tea_pack.h"

static const char pack_eq_default[] = {TEA_PACK_EQ_DEFAULT};
static const char pack_sp_default[] = {TEA_PACK_SP_DEFAULT};

static int pack_kv_char(struct tea_tcursor_kv *tab, char flag, const char *str, size_t len, const char eq, const char sp)
{
	size_t key_begin;
	size_t key_end;
	size_t value_begin;
	size_t value_end;
	size_t i;

	char ignore = flag & TEA_PACK_FLAG_IGNORE_EMPTY;
	char trim = flag & TEA_PACK_FLAG_SPACE_TRIM;

	for(i = 0; i < len;) {
		// key: begin
		key_begin = i;

		// key: end
		TEA_PACK_SEEK_KEY_END(key_end, i, str, len, eq, sp)

		// value: begin
		value_begin = i;

		// value: end
		TEA_PACK_SEEK_VALUE_END(value_end, i, str, len, sp)

		// capture
		if (trim) {
			TEA_PACK_SPACE_TRIM_WORD(str, key_begin, key_end);
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (tea_tcursor_kv_add(tab, ignore, &str[key_begin], key_end - key_begin, &str[value_begin], value_end - value_begin)) {
			return -1;
		}
	}

	return 0;
}

static int pack_kv_key(struct tea_tcursor_kv *tab, char flag, const char *str, size_t len, const char *eq, size_t eql, const char *sp, size_t spl)
{
	size_t key_begin;
	size_t key_end;
	size_t value_begin;
	size_t value_end;
	size_t match;
	size_t i;

	char ignore = flag & TEA_PACK_FLAG_IGNORE_EMPTY;
	char trim = flag & TEA_PACK_FLAG_SPACE_TRIM;

	for(i = 0; i < len;) {
		// key: begin
		key_begin = i;

		// key: end
		TEA_PACK_SEEK_WORD_KEY_END(key_end, i, match, str, len, eq, eql, sp, spl);

		// value: begin
		value_begin = i;

		// value: end
		TEA_PACK_SEEK_WORD_VALUE_END(value_end, i, match, str, len, sp, spl);

		// capture
		if (trim) {
			TEA_PACK_SPACE_TRIM_WORD(str, key_begin, key_end);
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (tea_tcursor_kv_add(tab, ignore, &str[key_begin], key_end - key_begin, &str[value_begin], value_end - value_begin)) {
			return -1;
		}
	}

	return 0;
}

static int pack_kv_multi(struct tea_tcursor_kv *tab, char flag, const char *str, size_t len, const char *eq, size_t eql, const char *sp, size_t spl)
{
	char sp_dict[TEA_PACK_MULTI_DICT_SIZE] = {};
	char eq_dict[TEA_PACK_MULTI_DICT_SIZE] = {};

	size_t key_begin;
	size_t key_end;
	size_t value_begin;
	size_t value_end;
	size_t i;

	TEA_PACK_MULTI_DICT_INIT(sp_dict, i, sp, spl);
	TEA_PACK_MULTI_DICT_INIT(eq_dict, i, eq, eql);

	char ignore = flag & TEA_PACK_FLAG_IGNORE_EMPTY;
	char trim = flag & TEA_PACK_FLAG_SPACE_TRIM;

	for(i = 0; i < len;) {
		// key: begin
		key_begin = i;

		// key: end
		TEA_PACK_SEEK_MULTI_KEY_END(key_end, i, str, len, eq_dict, sp_dict);

		// value: begin
		value_begin = i;

		// value: end
		TEA_PACK_SEEK_MULTI_VALUE_END(value_end, i, str, len, sp_dict);

		// capture
		if (trim) {
			TEA_PACK_SPACE_TRIM_WORD(str, key_begin, key_end);
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (tea_tcursor_kv_add(tab, ignore, &str[key_begin], key_end - key_begin, &str[value_begin], value_end - value_begin)) {
			return -1;
		}
	}

	return 0;
}

int tea_pack_kv(lua_State *l, char flag, const char *str, size_t len, const char *eq, size_t eql, const char *sp, size_t spl)
{
	struct tea_tcursor_kv tab = {};
	int stat;

	if(eql < 2 && spl < 2) {
		stat = pack_kv_char(&tab, flag, str, len,
			eql > 0 ? eq[0] : TEA_PACK_EQ_DEFAULT,
			spl > 0 ? sp[0] : TEA_PACK_SP_DEFAULT);
	} else {
		if(!eql) { eq = pack_eq_default; eql = 1; }
		if(!spl) { sp = pack_sp_default; spl = 1; }

		if (flag & TEA_PACK_FLAG_MULTI) {
			stat = pack_kv_multi(&tab, flag, str, len, eq, eql, sp, spl);
		} else {
			stat = pack_kv_key(&tab, flag, str, len, eq, eql, sp, spl);
		}
	}

	if (stat) {
		lua_pushlstring(l, str, len);
		lua_error(l);
	}

	tea_tcursor_kv_dump(l, &tab);
	return 1;
}


static int pack_char(struct tea_tcursor *tab, char flag, const char *str, size_t len, const char sp)
{
	size_t value_begin;
	size_t value_end;
	size_t i;

	char ignore = flag & TEA_PACK_FLAG_IGNORE_EMPTY;
	char trim = flag & TEA_PACK_FLAG_SPACE_TRIM;

	for(i = 0; i < len;) {
		// value: begin
		value_begin = i;

		// value: end
		TEA_PACK_SEEK_VALUE_END(value_end, i, str, len, sp);

		// capture
		if (trim) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (tea_tcursor_add(tab, ignore, &str[value_begin], value_end - value_begin)) {
			return -1;
		}
	}

	return 0;
}

static int pack_key(struct tea_tcursor *tab, char flag, const char *str, size_t len, const char *sp, size_t spl)
{
	size_t value_begin;
	size_t value_end;
	size_t match;
	size_t i;

	char ignore = flag & TEA_PACK_FLAG_IGNORE_EMPTY;
	char trim = flag & TEA_PACK_FLAG_SPACE_TRIM;

	for(i = 0; i < len;) {
		// value: begin
		value_begin = i;

		// value: end
		TEA_PACK_SEEK_WORD_VALUE_END(value_end, i, match, str, len, sp, spl);

		// capture
		if (trim) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (tea_tcursor_add(tab, ignore, &str[value_begin], value_end - value_begin)) {
			return -1;
		}
	}

	return 0;
}

static int pack_multi(struct tea_tcursor *tab, char flag, const char *str, size_t len, const char *sp, size_t spl)
{
	char sp_dict[TEA_PACK_MULTI_DICT_SIZE] = {};

	size_t value_begin;
	size_t value_end;
	size_t i;

	TEA_PACK_MULTI_DICT_INIT(sp_dict, i, sp, spl);

	char ignore = flag & TEA_PACK_FLAG_IGNORE_EMPTY;
	char trim = flag & TEA_PACK_FLAG_SPACE_TRIM;

	for(i = 0; i < len;) {
		// value: begin
		value_begin = i;

		// value: end
		TEA_PACK_SEEK_MULTI_VALUE_END(value_end, i, str, len, sp_dict);

		// capture
		if (trim) {
			TEA_PACK_SPACE_TRIM_WORD(str, value_begin, value_end);
		}

		if (tea_tcursor_add(tab, ignore, &str[value_begin], value_end - value_begin)) {
			return -1;
		}
	}

	return 0;
}

int tea_pack(lua_State *l, char flag, const char *str, size_t len, const char *sp, size_t spl)
{
	struct tea_tcursor tab = {};
	int stat;

	if(spl < 2) {
		stat = pack_char(&tab, flag, str, len, spl > 0 ? sp[0] : TEA_PACK_SP_DEFAULT);
	} else {
		if(!spl) { sp = pack_sp_default; spl = 1; }

		if (flag & TEA_PACK_FLAG_MULTI) {
			stat = pack_multi(&tab, flag, str, len, sp, spl);
		} else {
			stat = pack_key(&tab, flag, str, len, sp, spl);
		}
	}

	if (stat) {
		lua_pushlstring(l, str, len);
		lua_error(l);
	}

	tea_tcursor_dump(l, &tab);
	return 1;
}
