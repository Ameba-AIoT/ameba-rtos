include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/cutils/src/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces

.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/base/cutils/cmd

vpath %.c $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-pointer-sign

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC = $(DIR)/cmd_main.c

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
