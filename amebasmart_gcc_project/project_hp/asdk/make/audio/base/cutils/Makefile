
include $(MAKE_INCLUDE_GEN)

.PHONY: lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/base/cutils
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/cutils/src/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces

vpath %.c $(shell find $(DIR)/src/ -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
CFLAGS += -D__RTOS__ -Werror -Wno-multichar

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC  = $(DIR)/src/property.c
#CSRC += $(DIR)/src/rtlist.c
#CSRC += $(DIR)/src/rtvector.c
CSRC += $(DIR)/src/rtrbtree.c
CSRC += $(DIR)/src/rtintmap.c
#CSRC += $(DIR)/src/rtrbmap.c
#CSRC += $(DIR)/src/rtringbuffer.c
#CSRC += $(DIR)/src/rtlooper.c
#CSRC += $(DIR)/src/rtmessage.c
#CSRC += $(DIR)/src/rtmessage_handler.c
#CSRC += $(DIR)/src/rtlooper_manager.c
#CSRC += $(DIR)/src/rtstate_machine.c
#CSRC += $(DIR)/src/rtdatabuffer.c
#CSRC += $(DIR)/src/rtobject_manager.c

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
	$(COPY) $(OBJS) ../xip/

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
