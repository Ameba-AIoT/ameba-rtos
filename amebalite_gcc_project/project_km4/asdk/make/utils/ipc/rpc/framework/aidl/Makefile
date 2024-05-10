include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/include/xdr
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/src/aidl

.PHONY: all lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/utils/ipc/rpc/src/aidl
RPCGEN = $(BASEDIR)/component/utils/ipc/rpc/tools/rpcgen/rpcgen
vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC = \
	$(DIR)/RPCBaseDS_xdr.c

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
RPC_GEN:
	chmod 775 $(RPCGEN)
	$(RPCGEN) $(DIR)/RPCBaseDS.idl

COPY_RAM_OBJS: CORE_TARGETS

all: COPY_RAM_OBJS

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
