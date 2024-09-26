
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/ui/third_party/zlib

MODULE_IFLAGS +=  \
	-I$(DIR)/interfaces/include \
	-I$(DIR)/zlib/src
TARGET_LIB = lib_z

GLOBAL_CFLAGS := $(filter-out -Werror, $(GLOBAL_CFLAGS))
CFLAGS += -O3 -DUSE_MMAP

vpath %.c $(DIR) $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC :=  \
	$(DIR)/zlib/src/adler32.c  \
	$(DIR)/zlib/src/compress.c  \
	$(DIR)/zlib/src/crc32.c  \
	$(DIR)/zlib/src/deflate.c  \
	$(DIR)/zlib/src/gzclose.c  \
	$(DIR)/zlib/src/gzlib.c  \
	$(DIR)/zlib/src/gzread.c  \
	$(DIR)/zlib/src/gzwrite.c  \
	$(DIR)/zlib/src/infback.c  \
	$(DIR)/zlib/src/inflate.c  \
	$(DIR)/zlib/src/inftrees.c  \
	$(DIR)/zlib/src/inffast.c  \
	$(DIR)/zlib/src/trees.c  \
	$(DIR)/zlib/src/uncompr.c  \
	$(DIR)/zlib/src/zutil.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
STATIC_LIB = lib_z.a

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: CLEAN_OBJS CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS) $(STATIC_LIB)
$(STATIC_LIB):$(OBJS)
	$(AR) rvs $@ $^
	$(FROMELF) -g -D $@
	$(MOVE) -f $@ $(ROOTDIR)/lib/application

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)

