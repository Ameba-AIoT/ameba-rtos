

include $(ZB_STACK_DIR)/tests/PRO_R22/certification/testbin/srcs_list

include tests/multitest_common/Makefile

TEST_STEP_CONTROL_SRCS = $(ZB_STACK_DIR)/tests/multitest_common/zb_test_step_control.c \
						 $(ZB_STACK_DIR)/tests/multitest_common/zb_test_step_storage.c

COMMON_SRCS_ZRZC = ../common/zb_cert_test_globals.c $(TEST_STEP_CONTROL_SRCS)
COMMON_SRCS_ZED  = ../common/zb_cert_test_globals.c $(TEST_STEP_CONTROL_SRCS)

MODULE_IFLAGS +=                               \
  -I$(ZB_STACK_DIR)/tests/multitest_common \
  -I$(ZB_STACK_DIR)/nwk                    \
  -I$(ZB_STACK_DIR)/tests/PRO_R22/certification/testbin \
  -I$(ZB_STACK_DIR)/tests/PRO_R22/certification/common \

PRO_R22_CERT = $(ZB_STACK_DIR)/tests/PRO_R22/certification/

vpath %.c $(shell find $(PRO_R22_CERT) -type d)

# add menuconfig if need
#ifeq ($(ZB_MEM_CONFIG),min)
#    GLOBAL_CFLAGS += -DZB_MEM_CONFIG_MIN
#else
#ifeq ($(ZB_MEM_CONFIG),med)
#    GLOBAL_CFLAGS += -DZB_MEM_CONFIG_MED
#else
#ifeq ($(ZB_MEM_CONFIG),max)
#    GLOBAL_CFLAGS += -DZB_MEM_CONFIG_MAX
#endif
#endif
#endif

GLOBAL_CFLAGS += -DZB_MULTI_TEST

ifeq ($(CONFIG_802154_ZIGBEE_ROLE_ZC_ZR), y)
OBJS_ZCZR = $(notdir $(SRCS_ZCZR:.c=.o))
SRCSC = $(MULTITEST_COMMON_SRCS) $(COMMON_SRCS_ZRZC) zb_multitest_r22.c 
OBJSC = $(notdir $(SRCSC:.c=.o))

OBJS += ${OBJS_ZCZR}
OBJS += ${OBJSC}
endif

ifeq ($(CONFIG_802154_ZIGBEE_ROLE_ZED), y)
OBJS_ZED = $(notdir $(SRCS_ZED:.c=.o))
SRCSE = $(MULTITEST_COMMON_SRCS) $(COMMON_SRCS_ZED) zb_multitest_r22.c 
OBJSE = $(notdir $(SRCSE:.c=.o))

OBJS += ${OBJS_ZED}
OBJS += ${OBJSE}
endif



