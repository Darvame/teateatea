local tea = require "teateatea";

--[[
	local tab = tea.pack(str, separator[, drop_empty, trim_value_whitespaces, multi_separators]);
	local tab = tea.kvpack(str, equals, separator[, drop_empty, trim_key_whitespaces, trim_value_whitespaces, multi_equals, multi_separators]);
	local trimed_str = tea.trim(str);

	where:
	str = "string" or metatable.__tostring result (target string)
	separator = "string" or metatable.__tostring result (separation token)
	equals = "string" or metatable.__tostring result (equals token)

	drop_empty = boolean (don't push empty values)
	trim_key_whitespaces = boolean (trim whitespaces from key before pushing)
	trim_value_whitespaces = boolean (trim whitespaces from value before pushing)
	multi_separators = boolean (use the separator value as a collection of 1 byte separation tokens) UTF-8 (2 bytes and more) is unsupported
	multi_equals = boolean (use the equals value as a collection of 1 byte equals tokens) UTF-8 (2 bytes and more) is unsupported

]]

-- *** basic:

local str = "abc;bcd;qwety;;;lol";

local tab = tea.pack(str, ";"); -- split the str value into the array bellow

--[[
	tab = {
		[1] = 'abc',
		[2] = 'bcd',
		[3] = 'qwerty',
		[4] = '',
		[5] = '',
		[6] = 'lol',
	}
]]

--- *** key separator:

local str = "abcCUTbcdCUTqwetyCUTCUTCUTlol";

local tab = tea.pack(str, "CUT");

--[[
	tab = {
		[1] = 'abc',
		[2] = 'bcd',
		[3] = 'qwerty',
		[4] = '',
		[5] = '',
		[6] = 'lol',
	}
]]

--- *** drop empty values:

local str = "///abc/bcd/qwerty///////lastvalue//";

local tab = tea.pack(str, "/", true); -- using the custom separator '/', setting the 3rd arg to 'true' (drop empty values)

--[[
	tab = {
		[1] = 'abc',
		[2] = 'bcd',
		[3] = 'qwerty',
		[4] = 'lastvalue',
	}
]]

--- *** trim whitespaces:

local str = "one , two , three  , , foh  ";

local tab = tea.pack(str, ",", false, true); -- 4rd arg is true -> to remove any whitespaces before a value insertion

--[[
	tab = {
		[1] = 'one',
		[2] = 'two',
		[3] = 'three',
		[4] = '', -- to remove this, set 3rd to true
		[5] = 'foh',
	}
]]

--- *** multi separators, with trim

local str = "one; two! three foh";

local tab = tea.pack(str, ";! ", nil, true, true); -- 5th arg goes true (now using set of single char separators)

--[[
	tab = {
		[1] = 'one',
		[2] = '',
		[3] = 'two',
		[4] = 'three',
		[5] = '',
		[6] = 'foh',
	}
]]

--- *** key-value result:

local str = "key1=value1;key2=value2;key3=value3";

local tab = tea.kvpack(str, "=", ";"); -- split the str value into the key-value table bellow

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
		["key3"] = "value3",
	}
]]

--- *** key-value long sep/eq:

local str = "key1 is equal value1 and key2 is equal value2 and key3 is equal value3";

local tab = tea.kvpack(str, "is equal", "and", false, true, true); -- use long sep/eq, trim whitespaces

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
		["key3"] = "value3",
	}
]]

--- *** multi sep/eq:

local str = "key1-value1:key2=value2;key3?value3!";

local tab = tea.kvpack(str, "=-?", ";:!", false, false, false, true, true); -- 7th (for eq) and 8th (for sep) args go true

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
		["key3"] = "value3",
	}
]]

local str = "key1=value1:key2=value2;key3=value3!";

local tab = tea.kvpack(str, "=", ";:!", false, false, false, false, true); -- 8th goes true, if using a set of single char separators

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
		["key3"] = "value3",
	}
]]

--- *** empty keys are not supported!

local str = "key1=value1;;;;;=broken;   =broken_trimed_key; key2 = value2; key3=;key4";

local tab = tea.kvpack(str, "=", ";", false, true, true); -- trim whitespaces (but do not drop empty values)

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
		["key3"] = "",
		["key4"] = "",
	}
]]

--- *** trim only keys

local str = " key1   = value1 ;      key2 =       value2; key3   =    ; key4 =     ;";

local tab = tea.kvpack(str, "=", ";", false, true, false); -- trim whitespaces only from keys

--[[
	tab = {
		["key1"] = " value1 ",
		["key2"] = "       value2",
		["key3"] = "    ",
		["key4"] = "     ",
	}
]]


--- *** drop empty values

local str = "key1=value1;=broken;   =broken_trimed_key; key2 = value2; key3=;key4";

local tab = tea.kvpack(str, "=", ";", true, true, true); -- also ignore empty values and trim both (keys and values)

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
	}
]]

--- *** metatable.__tostring support:

local str = setmetatable({
	[1] = "key1=value1";
	[2] = "key2=value2";
	[3] = "key3=value3";
}, {
	__tostring = function(self)
		return table.concat(self, ";");
	end
});

local tab = tea.kvpack(str, "=", ";");

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
		["key3"] = "value3",
	}
]]

--- *** cookie pack example:

local cookie = "cookie1=value1; cookie2=value2; cookie3=value3";

local packed_cookie = tea.kvpack(cookie, "=", ";", true, true, true); -- ignore empty values and trim whitespaces

--[[
	packed_cookie = {
		["cookie1"] = "value1",
		["cookie2"] = "value2",
		["cookie3"] = "value3",
	}
]]

--- *** string metatable.__index integration:

local index = getmetatable("").__index;

for k, v in next, (require"teateatea") do
	if (type(v) == "function") then
		index[k] = v;
	end
end

-- and now

local tab = ("a|b|c|d"):pack("|");

--[[
	tab = {
		[1] = "a",
		[2] = "b",
		[3] = "c",
		[4] = "d",
	}
]]

-- or trim a string

local str = ("   TRIMED       "):trim();

--[[
	str = "TRIMED"
]]
