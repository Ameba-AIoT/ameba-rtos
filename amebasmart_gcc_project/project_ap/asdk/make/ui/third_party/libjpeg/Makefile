
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/ui/third_party/libjpeg-turbo/libjpeg-turbo

MODULE_IFLAGS +=  \
	-I$(BASEDIR)/component/ui/third_party/libjpeg-turbo/interfaces/include
TARGET_LIB = lib_jpg

GLOBAL_CFLAGS := $(filter-out -Werror, $(GLOBAL_CFLAGS))
CFLAGS += -O3 -fstrict-aliasing -Wno-sign-compare -Wno-unused-parameter

vpath %.c $(DIR) $(shell find $(DIR) -type d)
vpath %.S $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC :=  \
	$(DIR)/jcapimin.c    \
	$(DIR)/jcapistd.c    \
	$(DIR)/jaricom.c    \
	$(DIR)/jcarith.c    \
	$(DIR)/jccoefct.c    \
	$(DIR)/jccolor.c    \
	$(DIR)/jcdctmgr.c    \
	$(DIR)/jchuff.c    \
	$(DIR)/jcinit.c    \
	$(DIR)/jcmainct.c    \
	$(DIR)/jcmarker.c    \
	$(DIR)/jcmaster.c    \
	$(DIR)/jcomapi.c    \
	$(DIR)/jcparam.c    \
	$(DIR)/jcphuff.c    \
	$(DIR)/jcprepct.c    \
	$(DIR)/jcsample.c    \
	$(DIR)/jctrans.c    \
	$(DIR)/jdapimin.c    \
	$(DIR)/jdapistd.c    \
	$(DIR)/jdarith.c    \
	$(DIR)/jdatadst.c    \
	$(DIR)/jdatasrc.c    \
	$(DIR)/jdcoefct.c    \
	$(DIR)/jdcolor.c    \
	$(DIR)/jddctmgr.c    \
	$(DIR)/jdhuff.c    \
	$(DIR)/jdinput.c    \
	$(DIR)/jdmainct.c    \
	$(DIR)/jdmarker.c    \
	$(DIR)/jdmaster.c    \
	$(DIR)/jdmerge.c    \
	$(DIR)/jdphuff.c    \
	$(DIR)/jdpostct.c    \
	$(DIR)/jdsample.c    \
	$(DIR)/jdtrans.c    \
	$(DIR)/jerror.c    \
	$(DIR)/jfdctflt.c    \
	$(DIR)/jfdctfst.c    \
	$(DIR)/jfdctint.c    \
	$(DIR)/jidctflt.c    \
	$(DIR)/jidctfst.c    \
	$(DIR)/jidctint.c    \
	$(DIR)/jidctred.c    \
	$(DIR)/jmemmgr.c    \
	$(DIR)/jmemnobs.c    \
	$(DIR)/jquant1.c    \
	$(DIR)/jquant2.c    \
	$(DIR)/jutils.c    \
	$(DIR)/simd/jsimd_arm.c

ASMSRC += \
	$(DIR)/simd/jsimd_arm_neon.S

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
ASM_OBJS = $(notdir $(ASMSRC:.S=.o))
STATIC_LIB = lib_jpg.a

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

