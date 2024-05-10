include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/audio_utils/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/libs/src/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_algorithm/sonic
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_stream/core
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_stream/services
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/core
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/sw_algorithm/wrapper/Bundle
.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/audio_effect/passthrough

vpath %.cpp $(shell find $(DIR) -type d)
vpath %.c $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-narrowing
CPPFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-narrowing

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CPPSRC = $(DIR)/audio_equalizer.cpp
CSRC = $(DIR)/effect_module.c
CPPSRC += $(DIR)/effect_base.cpp
CPPSRC += $(DIR)/equalizer_client.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
CPPOBJS = $(notdir $(CPPSRC:.cpp=.o))
COBJS = $(notdir $(CSRC:.c=.o))

OBJS = $(CPPOBJS)
OBJS += $(COBJS)

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
	$(COPY) $(OBJS) ../../audio_xip/passthrough/

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
