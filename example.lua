local tea = require "teateatea";

--[[
	local tab = tea.pack(str[, separator, drop_empty, trim_whitespaces, multi_separators]);
	local tab = tea.kvpack(str[, equal, separator, drop_empty, trim_whitespaces, multi_sep_equal]);
	local trimed_str = tea.trim(str);
]]


-- *** basic:

local str = "abc;bcd;qwety;;;lol";

local tab = tea.pack(str); -- default separator is ';';
local tab = tea.pack(str, ";"); -- doing the same

--[[
	tab = {
		[1] = 'abc',
		[2] = 'bcd',
		[3] = 'qwerty',
		[5] = '',
		[6] = '',
		[4] = 'lol',
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
		[5] = '',
		[6] = '',
		[4] = 'lol',
	}
]]

--- *** drop empty values:

local str = "///abc/bcd/qwerty///////lastvalue//";

local tab = tea.pack(str, "/", true); -- using custom separator, setting 3rd arg to 'true' (drop empty values)

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

local tab = tea.pack(str, ",", nil, true); -- 4rd arg is true -> remove whitespaces before inserting

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

local tab = tea.pack(str, ";! ", nil, true, true); -- 5th arg goes true (now using multi/single char separators)

--[[
	tab = {
		[1] = 'one',
		[2] = 'two',
		[3] = 'three',
		[5] = 'foh',
	}
]]

--- *** key-value result:

local str = "key1=value1;key2=value2;key3=value3";

local tab = tea.kvpack(str); -- default separator is ';', default equal char is '='
local tab = tea.kvpack(str, "=", ";"); -- doing the same

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
		["key3"] = "value3",
	}
]]

--- *** key-value big sep/eq:

local str = "key1 is equal value1 and key2 is equal value2 and key3 is equal value3";

local tab = tea.kvpack(str, "is equal", "and", false, true); -- use long sep/eq, trim whitespaces

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
		["key3"] = "value3",
	}
]]

--- *** multi sep/eq:

local str = "key1-value1:key2=value2;key3?value3!";

local tab = tea.kvpack(str, "=-?", ";:!", false, false, true); -- 6th arg goes true

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
		["key3"] = "value3",
	}
]]

--- *** empty keys are not supported!

local str = "key1=value1;ignore;=broken_value;   =trimed_key; key2 = value2";

local tab = tea.kvpack(str, "=", ";", true, true); -- alose trim whitespaces and ignore empty

--[[
	tab = {
		["key1"] = "value1",
		["key2"] = "value2",
	}
]]

--- *** metatable.__tostring support (only for tables and userdata):

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

local packed_cookie = tea.kvpack(cookie, "=", ";", true, true); -- ignore empty values and trim whitespaces

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
	index[k] = v;
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

-- or trim string

local str = ("   TRIMED       "):trim();

--[[
	str = "TRIMED"
]]
