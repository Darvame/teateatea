SRC=tea_tcursor.c tea_pack.c teateatea.c
TARGET=teateatea.so
TESTFILE=test.lua

ifdef LUAPKG
	PREFIX=$(shell pkg-config --variable=prefix $(LUAPKG))
	LUALIBDIR=$(shell pkg-config --variable=INSTALL_CMOD $(LUAPKG))
	LUAINC=$(shell pkg-config --cflags $(LUAPKG))
	LUA=$(LUAPKG)
else
	PREFIX?=/usr/local
	LUALIBDIR?=$(PREFIX)/lib/lua/5.1
	LUAINC?=-I$(PREFIX)/include/luajit-2.0
	LUA?=$(PREFIX)/bin/luajit
endif

CCOPT=-O3 -Wall -fPIC $(CFLAGS) $(LUAINC)
LDOPT=-shared $(LDFLAGS)
OBJ=$(SRC:.c=.o)
CHMOD=755

ifeq ($(CC), clang)
	LDOPT+= -undefined dynamic_lookup
endif

all: $(SRC) $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDOPT) $(OBJ) -o $@

.c.o:
	$(CC) -c $(CCOPT) $< -o $@

clean:
	rm -f $(TARGET) $(OBJ)

install: $(TARGET)
	mkdir -p $(LUALIBDIR)
	cp $(TARGET) $(LUALIBDIR)/$(TARGET)
	chmod $(CHMOD) $(LUALIBDIR)/$(TARGET)

test: $(TARGET)
	$(LUA) $(TESTFILE)

.PHONY: all clean install test
