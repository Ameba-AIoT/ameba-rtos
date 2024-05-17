include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/sw_algorithm/Eq/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/audio_utils/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/sw_algorithm/Bundle/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/sw_algorithm/Common/include
.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/audio_effect/sw_algorithm/wrapper/Bundle

vpath %.cpp $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
CPPFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Werror

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/EffectBundle.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.cpp=.o))

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
	$(COPY) $(OBJS) ../../../audio_xip/local/
	$(COPY) $(OBJS) ../../../audio_xip/passthrough/

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.cpp
	$(Q)$(CC) $(CPPFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
