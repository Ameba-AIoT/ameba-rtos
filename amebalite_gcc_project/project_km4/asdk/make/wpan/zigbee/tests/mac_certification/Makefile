include $(ZB_STACK_DIR)/tests/mac_certification/testbin/srcs_list

MAC_CERT = $(ZB_STACK_DIR)/tests/mac_certification/

vpath %.c $(shell find $(MAC_CERT) -type d)

MODULE_IFLAGS += -I$(ZB_STACK_DIR)/tests/mac_certification/testbin

GLOBAL_CFLAGS += -DZB_MULTI_TEST

OBJS += $(notdir $(SRCS_MAC_TESTS:.c=.o))

CSRC += \
		$(ZB_STACK_DIR)/tests/mac_certification/testbin/zb_multi_mac_test_main.c \
		$(ZB_STACK_DIR)/tests/mac_certification/testbin/zb_console_monitor.c \
		$(ZB_STACK_DIR)/tests/mac_certification/testbin/zb_command_parser.c \
		$(ZB_STACK_DIR)/tests/mac_certification/common/zb_cert_test_globals.c \


