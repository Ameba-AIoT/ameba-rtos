include $(MAKE_INCLUDE_GEN)
include ../config.mk

.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
WFA_DIR = $(BASEDIR)/component/application/wifi-audio
AUDIO_DIR = $(BASEDIR)/component/audio
DIR = $(WFA_DIR)/plc

vpath %.c $(shell find $(DIR)/ -type d)
vpath %.cpp $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
MODULE_IFLAGS += \
    -I$(AUDIO_DIR)/interfaces \
    -I$(AUDIO_DIR)/base/audio_utils/include

MODULE_IFLAGS += \
    -I$(WFA_DIR)/base/log/include \
    -I$(WFA_DIR)/base/cutils/include

CSRC = $(DIR)/rlist.c
CSRC += $(DIR)/vox_mem.c
CSRC += $(DIR)/vox_math.c
CSRC += $(DIR)/rt_plc_engine.c
CPPSRC += $(DIR)/rt_plc.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
COBJS = $(notdir $(CSRC:.c=.o))
CPPOBJS = $(notdir $(CPPSRC:.cpp=.o))

OBJS = $(COBJS)
OBJS += $(CPPOBJS)

#*****************************************************************************#
#                               Dependency	                                  #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS

lib: COPY_RAM_OBJS
	$(COPY) $(OBJS) ../../xip/

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.cpp
	$(Q)$(CC) $(CPPFLAGS) $(MODULE_IFLAGS) $<  -o $@
%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
