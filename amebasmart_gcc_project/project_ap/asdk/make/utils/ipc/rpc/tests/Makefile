include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/libs/src/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/include/xdr
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/src/aidl
MODULE_IFLAGS               += -I$(BASEDIR)/component/utils/ipc/rpc/tests/aidl

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/utils/ipc/rpc/tests

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC = \
	$(DIR)/rpc_test.c \
	$(DIR)/test_rpc_ap.c \
	$(DIR)/test_rpc_np.c \
	$(DIR)/test_rpc_np_client1.c \
	$(DIR)/test_rpc_np_client2.c

CSRC += \
	$(DIR)/aidl/Test1RPC_Agent_svc.c \
	$(DIR)/aidl/Test1RPC_Agent_xdr.c \
	$(DIR)/aidl/Test1RPCBaseDS_xdr.c \
	$(DIR)/aidl/Test1RPC_System_clnt.c \
	$(DIR)/aidl/Test1RPC_System_svc.c \
	$(DIR)/aidl/Test1RPC_System_xdr.c

CSRC += \
	$(DIR)/aidl/Test2RPC_Agent_svc.c \
	$(DIR)/aidl/Test2RPC_Agent_xdr.c \
	$(DIR)/aidl/Test2RPCBaseDS_xdr.c \
	$(DIR)/aidl/Test2RPC_System_clnt.c \
	$(DIR)/aidl/Test2RPC_System_svc.c \
	$(DIR)/aidl/Test2RPC_System_xdr.c

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
