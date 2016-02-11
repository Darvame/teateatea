SOURCES=tea_tcursor.c tea_pack.c teateatea.c
TARGET=teateatea.so
TESTFILE=test.lua

SRCDIR=src
OBJDIR=obj

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
CHMOD=755

SRC=$(addprefix $(SRCDIR)/, $(SOURCES))
OBJ=$(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))

ifeq ($(CC), clang)
	LDOPT+= -undefined dynamic_lookup
endif


all: $(TARGET)

obj:
	mkdir -p obj

$(TARGET): $(OBJ)
	$(CC) $(LDOPT) $(OBJ) -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.c obj
	$(CC) -c $(CCOPT) $< -o $@

clean:
	rm -rf $(OBJDIR)
	rm -f $(TARGET)

install: $(TARGET)
	mkdir -p $(LUALIBDIR)
	cp $(TARGET) $(LUALIBDIR)/$(TARGET)
	chmod $(CHMOD) $(LUALIBDIR)/$(TARGET)

test: $(TARGET)
	$(LUA) $(TESTFILE)

.PHONY: all clean install test
