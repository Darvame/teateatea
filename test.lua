local tea = require "teateatea";
local crunning;

local tosmeta = {
	__tostring = function(self)
		return self[1];
	end
};

local check = function(this, tothis, opt, safe)
	if not (this == nil or this ~= tothis) then
		return true;
	end

	print(crunning or "Test", "OPT::" .. (opt or ""),"FAILED");
	print("1>" .. (tostring(this) or "nil") .. "<");
	print("2>" .. (tostring(tothis) or "nil") .. "<");

	assert(safe, 'test failed');
	return false;
end

local ok_cc = 0;
local ok = function()
	print("[OK] [" .. ok_cc .. "] " .. crunning);
	ok_cc = ok_cc + 1;
end

local eql_tables = function(t1, t2, opt)
	if not check(#t1, #t2, "table size", true) then
		print "THAT TABLE::";

		for i = 1, #t2 do
			print(">" .. (tostring(t1[i]) or "nil") .. "<", ">" .. (tostring(t2[i]) or "nil") .. "<");
		end

		assert(nil);
	end

	for i = 1, #t1 do
		check(t1[i], t2[i], opt);
	end
end

local eql_kvtables = function(t1, t2, opt)
	local er;

	for k, v in next, t1 do
		if not check(v, t2[k], opt, true) then er = true; end
	end
	-- and backward
	for k, v in next, t2 do
		if not check(v, t1[k], opt, true) then er = true; end
	end

	if er then
		print("TABLE::");
		for k, v in next, t1 do
			print(k.."="..v);
		end
	end

	assert(not er);
end

-- .pack
crunning = "pack";

do
	local str = "";

	for i = 1, 1000 do
		str = str .. i .. ";";
	end

	local t = {};

	for i = 1, 1000 do
		t[i] = tostring(i);
	end

	t[#t+1] = '';

	eql_tables(tea.pack(str, ";"), t);
end
ok(); -- 0

eql_tables(tea.pack("abc;i; ;;;*&;123;  -  ;last", ";", true), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "';'");
ok(); -- 1

eql_tables(tea.pack("abc!i! !!!*&!123!  -  !last", "!", true), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "'!'");
ok(); -- 2

eql_tables(tea.pack("abc!i! !!!*&!123!  -  !last", "!", false), {
	"abc", "i", " ", "", "", "*&", "123", "  -  ", "last"
}, "'!'");
ok(); -- 3

eql_tables(tea.pack("abc #mysep!i #mysep!  #mysep! #mysep! #mysep!*& #mysep!123 #mysep! #mysep!  -   #mysep!last", " #mysep!"), {
	"abc", "i", " ", "", "", "*&", "123", "", "  -  ", "last"
}, "' #mysep!'");
ok(); -- 4

eql_tables(tea.pack("a b c o p c ! the_end", " "), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space");
ok(); -- 5

-- .pack_t
crunning = "tpack";

eql_tables(tea.pack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      ", ";", nil, true), {
	"a b c", "i", "", "", "", "*&", "123", "-", "last", ""
}, ";");
ok(); -- 6

eql_tables(tea.pack(setmetatable({}, { __tostring = function() return "a b c; i; ; ;   ;*&; 123;  -  ;          last ;      " end }), ";", true, true), {
	"a b c", "i", "*&", "123", "-", "last"
}, "; meta");
ok(); -- 7

eql_tables(tea.pack("ab c ! i; ! ! !*&! 123 !  -  !last          ", "!", true, true), {
	"ab c", "i;", "*&", "123", "-", "last"
}, "!");
ok(); -- 8

eql_tables(tea.pack("a b c #mysep! i  #mysep!   #mysep!  a#mysep!   #mysep!*&    #mysep!123 #mysep!  -   #mysep!last", " #mysep!", true, true), {
	"a b c", "i", "a#mysep!", "*&", "123", "-", "last"
}, " #mysep!");
ok(); -- 9

eql_tables(tea.pack("a  b  c  o  p  c       !    the_end  ", " ", true, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space");
ok(); -- 10

eql_tables(tea.pack("a b c o p c ! the_end", " ", nil, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*1space");
ok(); -- 11

-- .pack_m
crunning = "mpack";

eql_tables(tea.pack("abc;i; ;?;*&;123;  -  ?last", ";?", true, nil, true, 1, 1, 1), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "';?'");
ok(); -- 12

eql_tables(tea.pack("abc;i; ;?;*&;123;  -  ?last", ";?", nil, nil, true), {
	"abc", "i", " ", "", "", "*&", "123", "  -  ", "last"
}, "';?'");
ok(); -- 13

eql_tables(tea.pack("a b1c o4p c?! the_end?", " ?1234567890", true, nil, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space, dig");
ok(); -- 14

-- .pack_mt
crunning = "mtpack";

eql_tables(tea.pack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      ", ";", true, true, true), {
	"a b c", "i", "*&", "123", "-", "last"
}, ";");
ok(); -- 15

eql_tables(tea.pack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      ", ";?", true, true, true), {
	"a b c", "i", "*&", "123", "-", "last"
}, ";?");
ok(); -- 16

eql_tables(tea.pack("a b c o p c?! the_end?", " ?1234567890", true, true, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space, dig");
ok(); -- 17

eql_tables(tea.pack("a  b  c  o  p  c       !    the_end  ", " ABCDEFG", true, true, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space");
ok(); -- 18

-- .pack_kv
crunning = "kvpack";

eql_kvtables(tea.kvpack("a=b;c=d;x= y ;empty;12345=12345", "=", ";"), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["12345"]="12345";
	["empty"] = "";
}, "';'");
ok(); -- 19

eql_kvtables(tea.kvpack("a b c d x  y  notbroken 12345 12345", " ", " ", true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 20

eql_kvtables(tea.kvpack("key1=value1&key2=value2&key3=value3&key4=value4&", "=", "&"), {
	["key1"] = "value1";
	["key2"] = "value2";
	["key3"] = "value3";
	["key4"] = "value4";
}, "&qry");
ok(); -- 21

eql_kvtables(tea.kvpack("key1endsvalue1endlkey2endsvalue2endlemptyendlendlkey3endsvalue3endlkey4endsvalue4endl", "ends", "endl"), {
	["key1"] = "value1";
	["key2"] = "value2";
	["key3"] = "value3";
	["key4"] = "value4";
	["empty"] = "";
}, "*almost same");
ok(); -- 22

-- .pack_tkv
crunning = "tkvpack";

eql_kvtables(tea.kvpack("a=b;c=d;x= y ;empty     ;   ;1234 5 =12345   ;", "=", ";", nil, true, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
	["empty"] = "";
}, "';'");
ok(); -- 23

eql_kvtables(tea.kvpack(setmetatable({}, { __tostring = function() return "a=b;c=d;x= y ;empty     ;  ;1234 5 =12345   ;" end }), "=", ";", nil, true, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
	["empty"] = "";
}, "';' meta");
ok(); -- 24

eql_kvtables(tea.kvpack("a b c d x  y  notbroken 12345 12345", " ", " ", true, true, true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 25

eql_kvtables(tea.kvpack("key1=value1&    key2=value2&key3=value3&key4=value4&&&&&&&&&&&&&&&&", "=", "&", nil, true, true), {
	["key1"] = "value1";
	["key2"] = "value2";
	["key3"] = "value3";
	["key4"] = "value4";
}, "&qry");
ok(); -- 26

eql_kvtables(tea.kvpack("abc k2l1 123 2l yolo k2l garb", "k2l1", "2l", nil, true, true), {
	["abc"] = "123";
	["yolo k"] = "";
	["garb"] = "";
}, "*key inside");
ok(); -- 27

-- .pack_mkv
crunning = "mkvpack";

eql_kvtables(tea.kvpack("a b c d x  y  notbroken 12345 12345", " !", " ?", true, nil, nil, true, true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 28

eql_kvtables(tea.kvpack("a=b;c=d,x= y ;empty=!empty2!!12345=12345", "=", ";,!", nil, nil, nil, true, true, 1, 1, 1, "NULL"), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["empty"] = "";
	["empty2"] = "";
	["12345"]="12345";
}, "';'");
ok(); -- 29

-- .pack_mtkv
crunning = "mtkvpack";

eql_kvtables(tea.kvpack("a=b;c=d,x= y ;empty!     ! 1234 5 =12345   ?", "=", "!;,?", nil, true, true, true, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
	["empty"] = "";
}, "!;,?");
ok(); -- 30

eql_kvtables(tea.kvpack("a=b;c=d,x- y ;empty-        !broken     ! 1234 5 =12345   ?", "=-", "!;,?", nil, true, true, true, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["empty"] = "";
	["broken"] = "";
	["1234 5"]="12345";
}, "!;,?|=-");
ok(); -- 31

eql_kvtables(tea.kvpack(setmetatable({}, { __tostring = function() return "a=b;c=d;x= y ;empty     ; 1234 5 =12345   ;" end }), "=", ';', nil, true, true, true, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
	["empty"] = "";
}, "';' meta");
ok(); -- 32

-- .trim
crunning = "trim";

check(tea.trim("      A   o   a   o   a   1!!!      "), "A   o   a   o   a   1!!!"); ok(); -- 33
check(tea.trim("      Aoaoa1!!!      ", 1, 1, 1), "Aoaoa1!!!"); ok(); -- 34
check(tea.trim(setmetatable({}, { __tostring = function() return "      Aoaoa1!!!      " end })), "Aoaoa1!!!"); ok(); -- 35
check(tea.trim("Aoaoa1!!!	 "), "Aoaoa1!!!"); ok(); -- 36
check(tea.trim("	 Aoaoa1!!!     		 "), "Aoaoa1!!!"); ok(); -- 37
check(tea.trim("Aoaoa1!!	!", 1 , 1, 1), "Aoaoa1!!	!"); ok(); -- 38
check(tea.trim({}, 1, 1) == nil, true); ok(); -- 39

-- .pack_mkv_value
crunning = "mkvpack_value";

eql_kvtables(tea.kvpack("a b c d x  y  notbroken 12345 12345", " ", " ?", true, nil, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 40

eql_kvtables(tea.kvpack("a=EQL!b;c=EQL!d,x=EQL! y ;empty=EQL!!empty2!!empty3=12345=EQL!12345", "=EQL!", ";,=!", nil, nil, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["empty"] = "";
	["empty2"] = "";
	["empty3"] = "";
	["12345"]="12345";
}, "';'");
ok(); -- 41

eql_kvtables(tea.kvpack("aEQLb;cEQLd,xEQL y ;emptyEQL!empty2!!empty3!12345EQL12345", "EQL", ";,!=", nil, nil, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["empty"] = "";
	["empty2"] = "";
	["empty3"] = "";
	["12345"]="12345";
}, "';'");
ok(); -- 42

-- .pack_mkv_key
crunning = "mkvpack_key";
eql_kvtables(tea.kvpack("a b c d x  y  notbroken!12345 12345", " !", " ", true, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 43

eql_kvtables(tea.kvpack("a=b=EQL+c+d=EQL+x- y =EQL+empty==EQL+empty2=EQL+=EQL+empty3=EQL+12345=12345", "=-+", "=EQL+", nil, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["empty"] = "";
	["empty2"] = "";
	["empty3"] = "";
	["12345"]="12345";
}, "';'");
ok(); -- 44

eql_kvtables(tea.kvpack("a=bEQLc+dEQLx- y EQLempty=EQLempty2EQLEQLempty3EQL12345=12345", "=-+", "EQL", nil, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["empty"] = "";
	["empty2"] = "";
	["empty3"] = "";
	["12345"]="12345";
}, "';'");
ok(); -- 45

crunning = "empty";
eql_kvtables(tea.pack("1=2;3=4;"), {
	"1=2;3=4;"
}, "empty"); ok(); -- 46

eql_kvtables(tea.kvpack("1=2;3=4;"), {
	["1=2;3=4;"] = "";
}, "emptykv"); ok(); -- 47

eql_kvtables(tea.kvpack("1=2;3=4;", nil, nil, true), {

}, "drop emptykv"); ok(); -- 48

crunning = "meta other";
eql_tables(tea.pack("1;2;3;4;", setmetatable({";"}, tosmeta)), {
	'1', '2', '3', '4', '',
}, "meta sep"); ok(); -- 49

eql_kvtables(tea.kvpack("1=2;3=4;", setmetatable({"="}, tosmeta),
setmetatable({";"}, tosmeta)), {
	['1'] = '2'; ['3'] = '4';
}, "meta eq sep"); ok(); -- 50

crunning = "diff trim ws";
eql_kvtables(tea.kvpack(" 1 = 2 ; 3 = 4 ;", "=", ";", nil, true), {
	['1'] = ' 2 ',
	['3'] = ' 4 ',
}, "tws key"); ok(); -- 51

eql_kvtables(tea.kvpack(" 1 = 2 ; 3 = 4 ;", "=", ";", nil, nil, true), {
	[' 1 '] = '2',
	[' 3 '] = '4',
}, "tws value"); ok(); -- 52

crunning = "pack single end";
eql_tables(tea.pack("qwerty88qwerty", "8888"), {
	"qwerty88qwerty"
}, "one long");
ok(); -- 53

eql_kvtables(tea.kvpack("1=qwerty88qwerty", "=", "8888"), {
	['1'] = "qwerty88qwerty"
}, "one long");
ok(); -- 54

crunning = "mask"

eql_kvtables(tea.kvpack_mask("a=EQL!b;c=EQL!d,x=EQL! y ;empty=EQL!!empty2!!empty3=12345=EQL!12345", "=EQL!", ";,=!", tea.mask.kvpack(nil, nil, nil, nil, true)), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["empty"] = "";
	["empty2"] = "";
	["empty3"] = "";
	["12345"]="12345";
}, "kvmask");
ok(); -- 55

eql_kvtables(tea.kvpack_mask("a=b;c=d;x= y ;empty     ;   ;1234 5 =12345   ;", "=", ";", tea.mask.kvpack(nil, true, true)), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
	["empty"] = "";
}, "kvmask");
ok(); -- 56

eql_tables(tea.pack_mask("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      ", ";", tea.mask.pack(nil, true)), {
	"a b c", "i", "", "", "", "*&", "123", "-", "last", ""
}, "mask");
ok(); -- 57

crunning = "no empty swap";

eql_kvtables(tea.kvpack("1=2;3=4;5=6;7=;8;=9;==10;100=100", "=", ";"), {
	['1'] = "2";
	['3'] = "4";
	['5'] = "6";
	['7'] = "";
	['8'] = "";
	['100'] = '100';
}, "do not swap");
ok(); -- 58

eql_kvtables(tea.kvpack("1=2;3=4;5=6;7=;8;=9;==10;100=100", "=", ";", true), {
	['1'] = "2";
	['3'] = "4";
	['5'] = "6";
	['100'] = '100';
}, "do not swap, drop empty");
ok(); -- 59

crunning = "empty swap";

eql_kvtables(tea.kvpack("1=2;3=4;5=6;7=;8;=9;==10;100=100", "=", ";", false, false, false, false, false, true), {
	['1'] = "2";
	['3'] = "4";
	['5'] = "6";
	['7'] = "";
	['8'] = "";
	['9'] = "";
	['=10'] = "";
	['100'] = '100';
}, "do swap");
ok(); -- 60

eql_kvtables(tea.kvpack("1=2;3=4;5=6;7=;8;=9;==10;100=100", "=", ";", true, false, false, false, false, true), {
	['1'] = "2";
	['3'] = "4";
	['5'] = "6";
	['100'] = '100';
}, "do swap, drop empty");
ok(); -- 61

crunning = "begins";

check(tea.begins("1234", 1234), true); ok(); -- 62
check(tea.begins(nil, 1234), false); ok(); -- 63
check(tea.begins("1", 12), false); ok(); -- 64
check(tea.begins("  1", setmetatable({" "}, tosmeta)), true); ok(); -- 65


crunning = "ends";

check(tea.ends("1234", 1234), true); ok(); -- 66
check(tea.ends(nil, 1234), false); ok(); -- 67
check(tea.ends("1", 12), false); ok(); -- 68
check(tea.ends("  1", setmetatable({"1"}, tosmeta)), true); ok(); -- 69

crunning = "begins_multiple";

eql_tables({tea.begins_multiple("0$0", "0$0", "12", false, "0$0   1")},{
	true, false, false, true
}); ok(); -- 70

crunning = "ends_multiple";

eql_tables({tea.ends_multiple("0$0", "0$0", "12", false, "1     0$0")},{
	true, false, false, true
}); ok(); -- 71

crunning = "pack_mask_multiple";
do
	local a, b, c, n = tea.pack_mask_multiple(tea.mask.pack(true, true), ";", "1;2;3;4", setmetatable({"1 ; 2; 3; 4 "}, tosmeta), "1;;2;;3;;4", false);
	local t = {'1', '2', '3', '4'};

	eql_tables(a, t);
	eql_tables(b, t);
	eql_tables(c, t);
	eql_tables(n, {}); ok(); -- 72
end

crunning = "kvpack_mask_multiple";
do
	local a, b, c, n = tea.kvpack_mask_multiple(tea.mask.kvpack(true, true, true), "=", ";",
		"1=a;2=bb;3=ccc;4=dddd", setmetatable({"1 = a ; 2   = bb; 3 = ccc; 4=dddd  "}, tosmeta), "1=a;;2=bb;;3=ccc;;4=dddd;;;", false);

	local t = {['1'] = 'a', ['2'] = 'bb', ['3'] = 'ccc', ['4'] = 'dddd'};

	eql_kvtables(a, t);
	eql_kvtables(b, t);
	eql_kvtables(c, t);
	eql_kvtables(n, {}); ok(); -- 73
end

crunning = "kvpack_10000";
do
	local c = tea.kvpack(("x=1;"):rep(1000), "=", ";");
	ok(); -- 74
end

crunning = "pack_10000";
do
	local c = tea.pack(("1,"):rep(1000), ",");
	ok(); -- 75
end
