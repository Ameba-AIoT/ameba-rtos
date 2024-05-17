
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES                                     #
#*****************************************************************************#
RENDER_TEST_DIR = $(BASEDIR)/component/audio/audio_hal/test/render
CAPTURE_TEST_DIR = $(BASEDIR)/component/audio/audio_hal/test/capture
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/audio_hal/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces

vpath %.c $(shell find $(RENDER_TEST_DIR)/ -type d)
vpath %.c $(shell find $(CAPTURE_TEST_DIR)/ -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__
CPPFLAGS += -Wno-unused-parameter

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(RENDER_TEST_DIR)/render_test.c
CSRC += $(CAPTURE_TEST_DIR)/capture_test.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

##*****************************************************************************#
# GENERATE OBJECT FILE
##*****************************************************************************#

CORE_TARGETS:   $(OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
