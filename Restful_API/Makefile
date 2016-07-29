#全局变量
PLAT := Linux
CC = gcc
AR = ar
CFLAGS = -MD -g -Wall -O2 $(INCLUDE) -lm -losAdaption
TOPDIR = $(shell pwd)

export CC
export AR
export PLAT
export TOPDIR

LIBDIR := $(TOPDIR)/Lib
LIB := $(LIBDIR)/libosAdaption.a
INCLUDE := -I./Inc -I./osAdaption/Inc 
TARGET := $(LIBDIR)/libRestFulLinux.a
SRC := $(wildcard Src/*.c)
OBJS := $(patsubst %.c, Objs/%.o,$(notdir $(SRC)))

all : 
	@if test ! -d $(LIBDIR);\
	then \
		mkdir $(LIBDIR);\
	fi
	$(MAKE) -C Src
	$(AR) -cr $(TARGET) $(OBJS)


.PHONY : clean

clean :
	rm -rf $(TARGET)
	$(MAKE) clean -C Src
	@if test -d $(LIBDIR);\
	then \
		rm -rf $(LIBDIR);\
	fi


