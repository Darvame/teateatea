# TEATEATEA [lua-lib]
Easy way to doing some stuff with strings:
- pack separated values into table
- pack separated keys and values into key-value table
- trim whitespaces

<h3>Pack String Into Table, aka Split String</h3>
<h5>Single Values (array-table)</h5>
- pack <b>single</b> vales, separated by <b>single char</b> or <b>word</b>, into table
```lua
  teateatea.pack(string_to_pack[, separator]);
```
```lua
local packedTable = teateatea.pack("value1;value2;value3;value4");
local packedTable = teateatea.pack("value1,value2,value3,value4", ",");
local packedTable = teateatea.pack("value1!iamsep!value2!iamsep!value3!iamsep!value4", "!iamsep!");
-- packedTable = {"value1", "value2", "value3", "value4");
```
<b>/!\\</b> the default value of separator (if not given, or invalid) is '<b>;</b>'
- pack <b>single</b> vales, separated by <b>single char</b> or <b>word</b>, into table, but <b>trim whitespaces</b> before inserting
```lua
  teateatea.tpack(string_to_pack[, separator]);
```
```lua
local packedTable = teateatea.tpack("value1;   value2;    value3 ;  value4");
local packedTable = teateatea.tpack("   value1 ,  ,value2,value3  ,value4  ", ",");
local packedTable = teateatea.tpack("value1    !iamsep!    value2 !iamsep!value3  !iamsep!  value4", "!iamsep!");
-- packedTable = {"value1", "value2", "value3", "value4");
```
<b>/!\\</b> zero lengh values will be ignored
- pack <b>single</b> vales, separated by <b>single</b> char, into table, but with multiple variations of separators
```lua
  teateatea.mpack(string_to_pack[, separators]);
```
```lua
local packedTable = teateatea.mpack("value1;value2;value3;value4");
local packedTable = teateatea.mpack("value1,value2,value3,value4", ",");
local packedTable = teateatea.mpack("value1,value2!value3?value4;", ",;!?");
-- packedTable = {"value1", "value2", "value3", "value4");
```
<b>/!\\</b> its okay to end string with separator
- pack <b>single</b> vales, separated by <b>single</b> char, into table, but with multiple variations of separators and <b>trim whitespaces</b> before inserting
```lua
  teateatea.mtpack(string_to_pack[, separators]);
```
```lua
local packedTable = teateatea.mtpack("value1;   value2;  value3;   value4");
local packedTable = teateatea.mtpack("value1, value2,value3,  value4", ",");
local packedTable = teateatea.mtpack("value1,   value2!  value3? value4;", ",;!?");
-- packedTable = {"value1", "value2", "value3", "value4");
```
<b>/!\\</b> its okay to end string with separator

<h5>[Key] = Value Table</h5>
- pack <b>single</b> key-value pair, separated by <b>single char</b> or <b>word</b>, into key-value table
```lua
  teateatea.kvpack(string_to_pack[, key_separator[, value_separator]]);
```
```lua
local packedTable = teateatea.kvpack("key1=value1;key1=value1;key1=value1;key1=value1");
local packedTable = teateatea.kvpack("key1$value1,key1$value1,key1$value1,key1$value1", "$", ",");
local packedTable = teateatea.kvpack("key1!iseql!value1!iamsep!key1!iseql!value1!iamsep!key1!iseql!value1!iamsep!key1!iseql!value1", "!iseql!", "!iamsep!");
-- packedTable = {["key1"] = "value1", ["key2"] = "value2", ["key3"] = "value3", ["key4"] = "value4");
```
<b>/!\\</b> the default value of value-separator (if not given, or invalid) is '<b>;</b>' and key-separator is '<b>=</b>'
- pack <b>single</b> key-value pair, separated by <b>single char</b> or <b>word</b>, into table, but <b>trim whitespaces</b> before inserting
```lua
  teateatea.tkvpack(string_to_pack[, key_separator[, value_separator]]);
```
```lua
local packedTable = teateatea.tkvpack("key1=value1;   key1=value1;key1    =value1;   key1=   value1");
local packedTable = teateatea.tkvpack("key1$value1,key1$value1   ,    key1$value1 ,  key1$    value1", "$", ",");
local packedTable = teateatea.tkvpack("key1   !iseql!value1    !iamsep!   key1!iseql!value1   !iamsep!   key1!iseql!value1!iamsep!key1!iseql    !value1", "!iseql!", "!iamsep!");
-- packedTable = {["key1"] = "value1", ["key2"] = "value2", ["key3"] = "value3", ["key4"] = "value4");
```
<b>/!\\</b> tab is also whitespace
- pack <b>single</b> key-value pair, separated by <b>single</b> char, into table, but with multiple variations of separators
```lua
  teateatea.mkvpack(string_to_pack[, key_separators[, value_separators]]);
```
```lua
local packedTable = teateatea.mkvpack("key1=value1;key1=value1;key1=value1;key1=value1");
local packedTable = teateatea.mkvpack("key1$value1??key1!value1,key1$value1?key1$value1?", "$!", ",?");
-- packedTable = {["key1"] = "value1", ["key2"] = "value2", ["key3"] = "value3", ["key4"] = "value4");
```
- pack <b>single</b> key-value pair, separated by <b>single</b> char, into table, but with multiple variations of separators and <b>trim whitespaces</b> value before inserting
```lua
  teateatea.mtkvpack(string_to_pack[, key_separators[, value_separators]]);
```
```lua
local packedTable = teateatea.mtkvpack("key1=value1;   key1=value1;key1    =value1;   key1=   value1", "=");
local packedTable = teateatea.mtkvpack("key1$value1??key1!value1   ,    key1$value1 ?  key1$    value1?", "$!", ",?");
-- packedTable = {["key1"] = "value1", ["key2"] = "value2", ["key3"] = "value3", ["key4"] = "value4");
```
<b>/!\\</b> zero lengh key or value also will be ignored

<h5>Pack Overall</h5>
```lua
  teateatea.mtkvpack(string_to_pack [, first_serator[, second_separator]]);
            ^^^^
            ||||
            ||||-- key-value pack
            ||---- trim whitespaces
            |----- multiple single-char (not utf-8) separators
```
<b>/!\\</b> string_to_pack is also may be table/userdata with <b>__tostring</b> metametohd

<h3>Trim Whitespaces</h3>
single function to trim whitespaces
```lua
  teateatea.trim(string_to_trim);
```
```lua
  local trimmed = teateatea.trim("      AAA! TRIM ME!    ");
  -- trimmed = "AAA! TRIM ME!";
```

<h3>Build and Install</h3>
using luajit-2.0 defaults
```bash
make & make install
```
<h5>Overriding Defaults</h5>
- prefix
```bash
export PREFIX="/perfix"
```
<b>/!\\</b> default: <b>"/usr/local"</b>
- path to lua headers
```bash
export LUAINC="/perfix/include/luajit-2.0"
```
<b>/!\\</b> default: <b>"/usr/local/include/luajit-2.0"</b>
- path to lua libs
```bash
export LUALIBDIR="/perfix/lib/lua/5.1"
```
<b>/!\\</b> default: <b>"/usr/local/lib/lua/5.1"</b>
- path to lua runtime
```bash
export LUA="/perfix/bin/luajit"
```
<b>/!\\</b> default: <b>"/usr/local/bin/luajit"</b>
<h5>If CLANG</h5>
if you are using <b>clang</b>, you may need to set CC var
```bash
export CC="clang"
```

<h3>Including</h3>
```lua
local teateatea = require "teateatea";
```
<b>/!\\</b> teateatea does not create global reference

also we may extend string metatable
```lua
for k, v in next, (require("teateatea")) do
  string[k] = v;
end
```
and later:
```lua
local t = ("v1,v2,v3"):pack(",");
-- t = {"v1", "v2", "v3"};
```
