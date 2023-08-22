# $@ 目标文件 $^ 所有的依赖文件 $< 第一个依赖文件
# -Wl,-E 

PLAT ?= linux

# 在Makefile中，.PHONY后面的target表示的也是一个伪造的target, 而不是真实存在的文件target；
# 注意Makefile的target默认是文件。 shell
.PHONY : clean all cleanall

LUA_CLIB_PATH ?= luaclib
LUA_CLIB_SRC ?= lualib-src
LUA_CLIB = tbl uv reg1 reg2 ud
LUA_INC ?= ./lua/src

SHARED := -fPIC --shared
EXPORT := -Wl,-E
ifeq ($(PLAT), macosx)
	SHARED := -fPIC -dynamiclib -Wl,-undefined,dynamic_lookup
endif

CFLAGS = -g -O2 -Wall -I$(LUA_INC)
CC ?= gcc

LUA_STATICLIB := lua/src/liblua.a

all : \
	$(LUA_STATICLIB) \
	test-vm \
	$(foreach v, $(LUA_CLIB), $(LUA_CLIB_PATH)/$(v).so)

$(LUA_STATICLIB) :
	cd lua && $(MAKE) CC='$(CC) -std=gnu99' $(PLAT)

test-vm : test-vm.c
	$(CC) $(CFLAGS) $^ -o $@ -L$(LUA_INC) $(EXPORT) -llua -ldl -lm

$(LUA_CLIB_PATH) :
	mkdir -p $(LUA_CLIB_PATH)

$(LUA_CLIB_PATH)/tbl.so : $(LUA_CLIB_SRC)/lua-tbl.c | $(LUA_CLIB_PATH)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ 

$(LUA_CLIB_PATH)/uv.so : $(LUA_CLIB_SRC)/lua-uv.c | $(LUA_CLIB_PATH)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUA_CLIB_PATH)/reg1.so : $(LUA_CLIB_SRC)/lua-reg1.c | $(LUA_CLIB_PATH)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@ 

$(LUA_CLIB_PATH)/reg2.so : $(LUA_CLIB_SRC)/lua-reg2.c | $(LUA_CLIB_PATH)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

$(LUA_CLIB_PATH)/ud.so : $(LUA_CLIB_SRC)/lua-ud.c | $(LUA_CLIB_PATH)
	$(CC) $(CFLAGS) $(SHARED) $^ -o $@

clean:
	rm -f $(LUA_CLIB_PATH)/*.so
	rm -f test-vm

cleanall :
	rm -f $(LUA_CLIB_PATH)/*.so
	cd lua && $(MAKE) clean
	rm -f test-vm
