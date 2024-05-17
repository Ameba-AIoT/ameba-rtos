include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/sw_algorithm/Eq/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/sw_algorithm/Common/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/sw_algorithm/Common/src
.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/audio_effect/sw_algorithm/Eq/src

vpath %.c $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__
GLOBAL_CFLAGS += -DNDEBUG -UDEBUG -Werror

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/LVEQNB_CalcCoef.c
CSRC += $(DIR)/LVEQNB_Control.c
CSRC += $(DIR)/LVEQNB_Tables.c
CSRC += $(DIR)/LVEQNB_Init.c
CSRC += $(DIR)/LVEQNB_Process.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

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

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
