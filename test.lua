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

	eqlTables(tea.pack(str), t);
end
ok();

eqlTables(tea.pack("abc;i; ;;;*&;123;  -  ;last"), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "';'");
ok();

eqlTables(tea.pack("abc!i! !!!*&!123!  -  !last", "!"), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "'!'");
ok();

eqlTables(tea.pack("abc #mysep!i #mysep!  #mysep! #mysep! #mysep!*& #mysep!123 #mysep!  -   #mysep!last", " #mysep!"), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "' #mysep!'");
ok();

eqlTables(tea.pack("a b c o p c ! the_end", " "), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space");
ok();

-- .pack_t
crunning = "tpack";

eqlTables(tea.tpack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      "), {
	"a b c", "i", "*&", "123", "-", "last"
}, ";");
ok();

eqlTables(tea.tpack(setmetatable({}, { __tostring = function() return "a b c; i; ; ;   ;*&; 123;  -  ;          last ;      " end })), {
	"a b c", "i", "*&", "123", "-", "last"
}, "; meta");
ok();

eqlTables(tea.tpack("ab c ! i; ! ! !*&! 123 !  -  !last          ", "!"), {
	"ab c", "i;", "*&", "123", "-", "last"
}, "!");
ok();

eqlTables(tea.tpack("a b c #mysep! i  #mysep!   #mysep!  a#mysep!   #mysep!*&    #mysep!123 #mysep!  -   #mysep!last", " #mysep!"), {
	"a b c", "i", "a#mysep!", "*&", "123", "-", "last"
}, " #mysep!");
ok();

eqlTables(tea.tpack("a  b  c  o  p  c       !    the_end  ", " "), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space");
ok();

eqlTables(tea.tpack("a b c o p c ! the_end", " "), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*1space");
ok();

-- .pack_m
crunning = "mpack";

eqlTables(tea.mpack("abc;i; ;?;*&;123;  -  ?last", ";?"), {
	"abc", "i", " ", "*&", "123", "  -  ", "last"
}, "';?'");
ok();

eqlTables(tea.mpack("a b c o p c?! the_end?", " ?1234567890"), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space, dig");
ok();

-- .pack_mt
crunning = "mtpack";

eqlTables(tea.mtpack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      "), {
	"a b c", "i", "*&", "123", "-", "last"
}, ";");
ok();

eqlTables(tea.mtpack("a b c; i; ; ;   ;*&; 123;  -  ;          last ;      ", ";?"), {
	"a b c", "i", "*&", "123", "-", "last"
}, ";?");
ok();

eqlTables(tea.mtpack("a b c o p c?! the_end?", " ?1234567890"), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space, dig");
ok();

eqlTables(tea.mtpack("a  b  c  o  p  c       !    the_end  ", " ABCDEFG"), {
	"a", "b", "c", "o", "p", "c", "!", "the_end"
}, "*space");
ok();

-- .pack_kv
crunning = "kvpack";

eqlkvTables(tea.kvpack("a=b;c=d;x= y ;broken;12345=12345"), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["12345"]="12345";
}, "';'");
ok();

eqlkvTables(tea.kvpack("a b c d x  y  notbroken 12345 12345", " ", " "), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok();

eqlkvTables(tea.kvpack("key1=value1&key2=value2&key3=value3&key4=value4&", nil, "&"), {
	["key1"] = "value1";
	["key2"] = "value2";
	["key3"] = "value3";
	["key4"] = "value4";
}, "&qry");
ok();

eqlkvTables(tea.kvpack("key1endsvalue1endlkey2endsvalue2endlbrokenkeyendlendlkey3endsvalue3endlkey4endsvalue4endl", "ends", "endl"), {
	["key1"] = "value1";
	["key2"] = "value2";
	["key3"] = "value3";
	["key4"] = "value4";
}, "*almost same");
ok();

-- .pack_tkv
crunning = "tkvpack";

eqlkvTables(tea.tkvpack("a=b;c=d;x= y ;broken     ; 1234 5 =12345   ;"), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
}, "';'");
ok();

eqlkvTables(tea.tkvpack(setmetatable({}, { __tostring = function() return "a=b;c=d;x= y ;broken     ; 1234 5 =12345   ;" end })), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
}, "';' meta");
ok();

eqlkvTables(tea.tkvpack("a b c d x  y  notbroken 12345 12345", " ", " "), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok();

eqlkvTables(tea.tkvpack("key1=value1&    key2=value2&key3=value3&key4=value4&&&&&&&&&&&&&&&&", nil, "&"), {
	["key1"] = "value1";
	["key2"] = "value2";
	["key3"] = "value3";
	["key4"] = "value4";
}, "&qry");
ok();

eqlkvTables(tea.tkvpack("abc k2l1 123 2l yolo k2l garb", "k2l1", "2l"), {
	["abc"] = "123"
}, "*key inside");
ok();

-- .pack_mkv
crunning = "mkvpack";

eqlkvTables(tea.mkvpack("a b c d x  y  notbroken 12345 12345", " !", " ?"), {
	["a"] = "b";
	["c"] = "d";
	["notbroken"]="12345";
}, "only*space; same keys");
ok();

eqlkvTables(tea.mkvpack("a=b;c=d,x= y ;broken!12345=12345", "=", ";,!"), {
	["a"] = "b";
	["c"] = "d";
	["x"] = " y ";
	["12345"]="12345";
}, "';'");
ok();

-- .pack_mtkv
crunning = "mtkvpack";

eqlkvTables(tea.mtkvpack("a=b;c=d,x= y ;broken     ! 1234 5 =12345   ?", nil, "!;,?"), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
}, "!;,?");
ok();

eqlkvTables(tea.mtkvpack("a=b;c=d,x- y ;broken     ! 1234 5 =12345   ?", "=-", "!;,?"), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
}, "!;,?|=-");
ok();

eqlkvTables(tea.mtkvpack(setmetatable({}, { __tostring = function() return "a=b;c=d;x= y ;broken     ; 1234 5 =12345   ;" end })), {
	["a"] = "b";
	["c"] = "d";
	["x"] = "y";
	["1234 5"]="12345";
}, "';' meta");
ok();

-- .trim
crunning = "trim";

check(tea.trim("      A   o   a   o   a   1!!!      "), "A   o   a   o   a   1!!!"); ok();
check(tea.trim("      Aoaoa1!!!      "), "Aoaoa1!!!"); ok();
check(tea.trim(setmetatable({}, { __tostring = function() return "      Aoaoa1!!!      " end })), "Aoaoa1!!!"); ok();
check(tea.trim("Aoaoa1!!!	 "), "Aoaoa1!!!"); ok();
check(tea.trim("	 Aoaoa1!!!      "), "Aoaoa1!!!"); ok();
check(tea.trim("Aoaoa1!!	!"), "Aoaoa1!!	!"); ok();
