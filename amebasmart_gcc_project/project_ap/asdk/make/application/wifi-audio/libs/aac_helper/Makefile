
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/libs/src/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libAACenc/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libAACdec/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libAACdec/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libPCMutils/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libFDK/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libSYS/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libMpegTPDec/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libSBRdec/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libArithCoding/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libDRCdec/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/aac/libSACdec/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/audio_utils/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces

.PHONY: all lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/wifi-audio/aac_helper

vpath %.c $(DIR) $(shell find $(DIR) -type d)
vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC += $(DIR)/aac_helper.c

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#

CPPOBJS = $(notdir $(CPPSRC:.cpp=.o))
COBJS = $(notdir $(CSRC:.c=.o))

OBJS = $(CPPOBJS)
OBJS += $(COBJS)

-include $(OBJS:.o=.d)
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS

lib: COPY_RAM_OBJS
	$(COPY) $(OBJS) ../../xip/
#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

%.o:%.cpp
	$(Q)$(CC) $(CPPFLAGS) $(MODULE_IFLAGS) $<  -o $@
%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean:
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
