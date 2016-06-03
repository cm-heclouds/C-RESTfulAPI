ifeq ($(PLAT), Linux)
	SRCDIR := ./Linux
endif

CFLAGS := -MD -g -Wall -O2 
OUTPUTDIR := $(TOPDIR)/Objs/osAdaption
TARGET := $(TOPDIR)/Lib/libosAdaption.a
INCLUDE := $(TOPDIR)/Inc
SRC := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst %.c, $(OUTPUTDIR)/%.o,$(notdir $(SRC)))

all : $(OBJS)
	$(AR) -cr $(TARGET) $(OBJS)


$(OUTPUTDIR)/%.o : $(SRCDIR)/%.c 
	$(CC) $(CFLAGS) -I$(INCLUDE) -c $< -o $@

sinclude $(OBJS:.o=.d)

.PHONY : clean

clean:
	rm -rf $(TARGET) 
	rm -rf $(OBJS) $(OBJS:.o=.d)

