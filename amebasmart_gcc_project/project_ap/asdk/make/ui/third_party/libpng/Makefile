
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/ui/third_party/libpng/libpng

MODULE_IFLAGS +=  \
	-I$(BASEDIR)/component/ui/third_party/libpng/interfaces/include \
	-I$(BASEDIR)/component/ui/third_party/zlib/interfaces/include
TARGET_LIB = lib_png

GLOBAL_CFLAGS := $(filter-out -Werror, $(GLOBAL_CFLAGS))
CFLAGS += --std=gnu89 -Wall -Wno-unused-parameter

vpath %.c $(DIR) $(shell find $(DIR) -type d)
vpath %.S $(DIR) $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC :=  \
	$(DIR)/png.c \
	$(DIR)/pngerror.c \
	$(DIR)/pngget.c \
	$(DIR)/pngmem.c \
	$(DIR)/pngpread.c \
	$(DIR)/pngread.c \
	$(DIR)/pngrio.c \
	$(DIR)/pngrtran.c \
	$(DIR)/pngrutil.c \
	$(DIR)/pngset.c \
	$(DIR)/pngtrans.c \
	$(DIR)/pngwio.c \
	$(DIR)/pngwrite.c \
	$(DIR)/pngwtran.c \
	$(DIR)/pngwutil.c \
	$(DIR)/arm_init.c \
	$(DIR)/filter_neon_intrinsics.c

ASMSRC += \
	$(DIR)/arm/filter_neon.S

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
ASM_OBJS = $(notdir $(ASMSRC:.S=.o))
STATIC_LIB = lib_png.a

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: CLEAN_OBJS CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS) $(ASM_OBJS) $(STATIC_LIB)
$(STATIC_LIB):$(OBJS) $(ASM_OBJS)
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

