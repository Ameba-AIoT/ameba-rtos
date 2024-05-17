include $(MAKE_INCLUDE_GEN)

MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/hal/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/ipc/amebasmart/include

.PHONY: all lib clean
TARGET_LIB = lib_rpc_hal

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#

DIR = $(BASEDIR)/component/utils/ipc/rpc
vpath %.c $(shell find $(DIR)/ -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__
GLOBAL_CFLAGS += -DNDEBUG -UDEBUG

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC += $(DIR)/rpc_hw_manager.c

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

all: COPY_RAM_OBJS

lib: COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))

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
	$(REMOVEDIR) ./src
	$(call CLEAN_FUNC)
