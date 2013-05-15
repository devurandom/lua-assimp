ifneq ($(USE_LUAJIT),)
LUA_CPPFLAGS=-I/usr/include/luajit-2.0
LUA_LIBS=-lluajit-2.0
else
ifeq ($(LUA_VERSION),)
LUA_VERSION=5.2
endif
LUA_CPPFLAGS=-I/usr/include/lua$(LUA_VERSION)
LUA_LIBS=-llua$(LUA_VERSION)
endif

ifeq ($(LIBASSIMP),)
ASSIMP_CPPFLAGS=-Iassimp/include
ASSIMP_LDFLAGS=-Lassimp/build/code
LUA_ASSIMP_DEPENDS=assimp/build/code/libassimp.so
endif

ifneq ($(DEBUG),)
EXTRA_CFLAGS+= -g -O0
endif

CFLAGS=-Wall -Werror -pedantic -std=c99 -fPIC $(EXTRA_CFLAGS) $(LUA_CPPFLAGS) $(ASSIMP_CPPFLAGS)
LIBS=$(LUA_LIBS) $(ASSIMP_LDFLAGS) -lassimp

.PHONY: all
all: assimp.so

assimp.so: assimp.o lextlib/lextlib.o $(LUA_ASSIMP_DEPENDS)

assimp.o: assimp.c lextlib/lextlib.h lextlib/lextlib_global.h lextlib/lextlib_lua52.h

lextlib/lextlib.o:
	$(MAKE) $(MAKEFLAGS) -C lextlib

assimp/build:
	mkdir assimp/build

assimp/build/code/libassimp.so: assimp/build
	cd assimp/build && cmake .. && cmake --build .

.PHONY: clean
clean:
	$(RM) *.so *.o

.PHONY: distclean
distclean: clean
	$(RM) -r assimp/build

.SUFFIXES: .c .o .so

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

.o.so:
	$(CC) $(CFLAGS) $(LDFLAGS) -shared -o $@ $^ $(LIBS)
