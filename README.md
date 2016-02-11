# TEATEATEA

<b>Teh Lua Lib, that provides you an easy way to do some stuff with strings:</b>

- pack (split) separated values into a new array table
```lua
    local table = teateatea.pack(str[, separator, ignore_empty, trim_whitespaces, multi_separators])
```
- pack (split) separated keys and values into a new key-value table
```lua
    local table = teateatea.kvpack(str[, equal, separator, drop_empty, trim_whitespaces, multi_equal, multi_separators])
```
- trim whitespaces
```lua
    local trimed_str = teateatea.trim(str)
```

---

<b>checkout [example.lua](./example.lua) for examples and more info </b>

---

# BUILDING & INSTALLATION

<h3>Downloading:</h3>

```bash
git clone https://github.com/Darvame/teateatea.git
cd teateatea
```

<h3>Setting Variables:</h3>

```bash
#by default sudo does not pass exported variables
#you may need to run everything bellow (from this point) with root
#in order to use '$ make install'
su -
#or call make with specified variables
make CC=clang
```
- <b>with pgk-config</b>
```bash
export LUAPKG=lua5.2 #or any ('luajit', 'lua5.1', 'lua5.3')
```
- <b>without pgk-config</b>
```bash
# this values are defaults (from luajit), do not set if have the same
export PREFIX=/usr/local #prefix
export LUALIBDIR=$PREFIX/lib/lua/5.1 #path to lua libs, used only for '$ make install'
export LUAINC=-I$PREFIX/include/luajit-2.0 #path to lua headers
export LUA=$PREFIX/bin/luajit #lua executable, used only for '$ make test'
```
- <b>with clang</b>
```bash
export CC=clang #if build on osx
```

<h3>Building & Installing:</h3>

```bash
make && make install
```
