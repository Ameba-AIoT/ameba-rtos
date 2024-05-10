include $(MAKE_INCLUDE_GEN)

MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/hal/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/xdr/include

.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/utils/ipc/rpc
vpath %.c $(DIR) $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wimplicit-function-declaration

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = \
	$(DIR)/src/rpc_client.c \
	$(DIR)/src/rpc_server.c \
	$(DIR)/src/rpc_common.c \
	$(DIR)/src/rpc_utils.c \
	$(DIR)/src/xdr/xdr_array.c \
	$(DIR)/src/xdr/xdr.c \
	$(DIR)/src/xdr/xdr_float.c \
	$(DIR)/src/xdr/xdr_mem.c \
	$(DIR)/src/xdr/xdr_process.c \
	$(DIR)/src/xdr/xdr_ref.c \
	$(DIR)/src/xdr/xdr_sizeof.c

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
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
