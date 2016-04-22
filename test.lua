local tea = require "teateatea";
local crunning;

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

local eqlTables = function(t1, t2, opt)
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

local eqlkvTables = function(t1, t2, opt)
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

	eqlTables(tea.pack(str, ";"), t);
end
ok(); -- 0

eqlTables(tea.pack("abc;i; ;;;*&;123;  -  ;last", ";", true), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "';'");
ok(); -- 1

eqlTables(tea.pack("abc!i! !!!*&!123!  -  !last", "!", true), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "'!'");
ok(); -- 2

eqlTables(tea.pack("abc!i! !!!*&!123!  -  !last", "!", false), {
	"abc", "i", " ", "", "", "*&", "123", "  -  ", "last"
}, "'!'");
ok(); -- 3

eqlTables(tea.pack("abc #mysep!i #mysep!  #mysep! #mysep! #mysep!*& #mysep!123 #mysep! #mysep!  -   #mysep!last", " #mysep!"), {
	"abc", "i", " ", "", "", "*&", "123", "", "  -  ", "last"
}, "' #mysep!'");
ok(); -- 4

eqlTables(tea.pack("a b c o p c ! the_end", " "), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space");
ok(); -- 5

-- .pack_t
crunning = "tpack";

eqlTables(tea.pack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      ", ";", nil, true), {
	"a b c", "i", "", "", "", "*&", "123", "-", "last", ""
}, ";");
ok(); -- 6

eqlTables(tea.pack(setmetatable({}, { __tostring = function() return "a b c; i; ; ;   ;*&; 123;  -  ;          last ;      " end }), ";", true, true), {
	"a b c", "i", "*&", "123", "-", "last"
}, "; meta");
ok(); -- 7

eqlTables(tea.pack("ab c ! i; ! ! !*&! 123 !  -  !last          ", "!", true, true), {
	"ab c", "i;", "*&", "123", "-", "last"
}, "!");
ok(); -- 8

eqlTables(tea.pack("a b c #mysep! i  #mysep!   #mysep!  a#mysep!   #mysep!*&    #mysep!123 #mysep!  -   #mysep!last", " #mysep!", true, true), {
	"a b c", "i", "a#mysep!", "*&", "123", "-", "last"
}, " #mysep!");
ok(); -- 9

eqlTables(tea.pack("a  b  c  o  p  c       !    the_end  ", " ", true, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space");
ok(); -- 10

eqlTables(tea.pack("a b c o p c ! the_end", " ", nil, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*1space");
ok(); -- 11

-- .pack_m
crunning = "mpack";

eqlTables(tea.pack("abc;i; ;?;*&;123;  -  ?last", ";?", true, nil, true, 1, 1, 1), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "';?'");
ok(); -- 12

eqlTables(tea.pack("abc;i; ;?;*&;123;  -  ?last", ";?", nil, nil, true), {
	"abc", "i", " ", "", "", "*&", "123", "  -  ", "last"
}, "';?'");
ok(); -- 13

eqlTables(tea.pack("a b1c o4p c?! the_end?", " ?1234567890", true, nil, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space, dig");
ok(); -- 14

-- .pack_mt
crunning = "mtpack";

eqlTables(tea.pack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      ", ";", true, true, true), {
	"a b c", "i", "*&", "123", "-", "last"
}, ";");
ok(); -- 15

eqlTables(tea.pack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      ", ";?", true, true, true), {
	"a b c", "i", "*&", "123", "-", "last"
}, ";?");
ok(); -- 16

eqlTables(tea.pack("a b c o p c?! the_end?", " ?1234567890", true, true, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space, dig");
ok(); -- 17

eqlTables(tea.pack("a  b  c  o  p  c       !    the_end  ", " ABCDEFG", true, true, true), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space");
ok(); -- 18

-- .pack_kv
crunning = "kvpack";

eqlkvTables(tea.kvpack("a=b;c=d;x= y ;empty;12345=12345", "=", ";"), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["12345"]="12345";
	["empty"] = "";
}, "';'");
ok(); -- 19

eqlkvTables(tea.kvpack("a b c d x  y  notbroken 12345 12345", " ", " ", true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 20

eqlkvTables(tea.kvpack("key1=value1&key2=value2&key3=value3&key4=value4&", "=", "&"), {
	["key1"] = "value1";
	["key2"] = "value2";
	["key3"] = "value3";
	["key4"] = "value4";
}, "&qry");
ok(); -- 21

eqlkvTables(tea.kvpack("key1endsvalue1endlkey2endsvalue2endlemptyendlendlkey3endsvalue3endlkey4endsvalue4endl", "ends", "endl"), {
	["key1"] = "value1";
	["key2"] = "value2";
	["key3"] = "value3";
	["key4"] = "value4";
	["empty"] = "";
}, "*almost same");
ok(); -- 22

-- .pack_tkv
crunning = "tkvpack";

eqlkvTables(tea.kvpack("a=b;c=d;x= y ;empty     ;   ;1234 5 =12345   ;", "=", ";", nil, true, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
	["empty"] = "";
}, "';'");
ok(); -- 23

eqlkvTables(tea.kvpack(setmetatable({}, { __tostring = function() return "a=b;c=d;x= y ;empty     ;  ;1234 5 =12345   ;" end }), "=", ";", nil, true, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
	["empty"] = "";
}, "';' meta");
ok(); -- 24

eqlkvTables(tea.kvpack("a b c d x  y  notbroken 12345 12345", " ", " ", true, true, true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 25

eqlkvTables(tea.kvpack("key1=value1&    key2=value2&key3=value3&key4=value4&&&&&&&&&&&&&&&&", "=", "&", nil, true, true), {
	["key1"] = "value1";
	["key2"] = "value2";
	["key3"] = "value3";
	["key4"] = "value4";
}, "&qry");
ok(); -- 26

eqlkvTables(tea.kvpack("abc k2l1 123 2l yolo k2l garb", "k2l1", "2l", nil, true, true), {
	["abc"] = "123";
	["yolo k"] = "";
	["garb"] = "";
}, "*key inside");
ok(); -- 27

-- .pack_mkv
crunning = "mkvpack";

eqlkvTables(tea.kvpack("a b c d x  y  notbroken 12345 12345", " !", " ?", true, nil, nil, true, true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 28

eqlkvTables(tea.kvpack("a=b;c=d,x= y ;empty=!empty2!!12345=12345", "=", ";,!", nil, nil, nil, true, true, 1, 1, 1, "NULL"), {
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

eqlkvTables(tea.kvpack("a=b;c=d,x= y ;empty!     ! 1234 5 =12345   ?", "=", "!;,?", nil, true, true, true, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
	["empty"] = "";
}, "!;,?");
ok(); -- 30

eqlkvTables(tea.kvpack("a=b;c=d,x- y ;empty-        !broken     ! 1234 5 =12345   ?", "=-", "!;,?", nil, true, true, true, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["empty"] = "";
	["broken"] = "";
	["1234 5"]="12345";
}, "!;,?|=-");
ok(); -- 31

eqlkvTables(tea.kvpack(setmetatable({}, { __tostring = function() return "a=b;c=d;x= y ;empty     ; 1234 5 =12345   ;" end }), "=", ';', nil, true, true, true, true), {
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

eqlkvTables(tea.kvpack("a b c d x  y  notbroken 12345 12345", " ", " ?", true, nil, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 40

eqlkvTables(tea.kvpack("a=EQL!b;c=EQL!d,x=EQL! y ;empty=EQL!!empty2!!empty3=12345=EQL!12345", "=EQL!", ";,=!", nil, nil, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["empty"] = "";
	["empty2"] = "";
	["empty3"] = "";
	["12345"]="12345";
}, "';'");
ok(); -- 41

eqlkvTables(tea.kvpack("aEQLb;cEQLd,xEQL y ;emptyEQL!empty2!!empty3!12345EQL12345", "EQL", ";,!=", nil, nil, nil, nil, true), {
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
eqlkvTables(tea.kvpack("a b c d x  y  notbroken!12345 12345", " !", " ", true, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok(); -- 43

eqlkvTables(tea.kvpack("a=b=EQL+c+d=EQL+x- y =EQL+empty==EQL+empty2=EQL+=EQL+empty3=EQL+12345=12345", "=-+", "=EQL+", nil, nil, nil, true), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["empty"] = "";
	["empty2"] = "";
	["empty3"] = "";
	["12345"]="12345";
}, "';'");
ok(); -- 44

eqlkvTables(tea.kvpack("a=bEQLc+dEQLx- y EQLempty=EQLempty2EQLEQLempty3EQL12345=12345", "=-+", "EQL", nil, nil, nil, true), {
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
eqlkvTables(tea.pack("1=2;3=4;"), {
	"1=2;3=4;"
}, "empty"); ok(); -- 46

eqlkvTables(tea.kvpack("1=2;3=4;"), {
	["1=2;3=4;"] = "";
}, "emptykv"); ok(); -- 47

eqlkvTables(tea.kvpack("1=2;3=4;", nil, nil, true), {

}, "drop emptykv"); ok(); -- 48

crunning = "meta other";
eqlkvTables(tea.pack("1;2;3;4;", setmetatable({";"}, {__tostring = function(self) return self[1]; end})), {
	'1', '2', '3', '4', '',
}, "meta sep"); ok(); -- 49

eqlkvTables(tea.kvpack("1=2;3=4;", setmetatable({"="}, {__tostring = function(self) return self[1]; end}),
setmetatable({";"}, {__tostring = function(self) return self[1]; end})), {
	['1'] = '2'; ['3'] = '4';
}, "meta eq sep"); ok(); -- 50

crunning = "diff trim ws";
eqlkvTables(tea.kvpack(" 1 = 2 ; 3 = 4 ;", "=", ";", nil, true), {
	['1'] = ' 2 ',
	['3'] = ' 4 ',
}, "tws key"); ok(); -- 51

eqlkvTables(tea.kvpack(" 1 = 2 ; 3 = 4 ;", "=", ";", nil, nil, true), {
	[' 1 '] = '2',
	[' 3 '] = '4',
}, "tws value"); ok(); -- 52

crunning = "pack single end";
eqlTables(tea.pack("qwerty88qwerty", "8888"), {
	"qwerty88qwerty"
}, "one long");
ok(); -- 53

eqlTables(tea.kvpack("1=qwerty88qwerty", "=", "8888"), {
	['1'] = "qwerty88qwerty"
}, "one long");
ok(); -- 54

crunning = "mask"

eqlkvTables(tea.mkvpack("a=EQL!b;c=EQL!d,x=EQL! y ;empty=EQL!!empty2!!empty3=12345=EQL!12345", "=EQL!", ";,=!", tea.mask.kvpack(nil, nil, nil, nil, true)), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["empty"] = "";
	["empty2"] = "";
	["empty3"] = "";
	["12345"]="12345";
}, "kvmask");
ok(); -- 55

eqlkvTables(tea.mkvpack("a=b;c=d;x= y ;empty     ;   ;1234 5 =12345   ;", "=", ";", tea.mask.kvpack(nil, true, true)), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
	["empty"] = "";
}, "kvmask");
ok(); -- 56

eqlTables(tea.mpack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      ", ";", tea.mask.pack(nil, true)), {
	"a b c", "i", "", "", "", "*&", "123", "-", "last", ""
}, "mask");
ok(); -- 57
