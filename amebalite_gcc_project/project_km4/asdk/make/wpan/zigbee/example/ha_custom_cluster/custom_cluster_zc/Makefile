
ZB_EXAMPLE_DIR	= $(BASEDIR)/component/wpan/zigbee/example/ha_custom_cluster/custom_cluster_zc

vpath %.c $(ZB_EXAMPLE_DIR)/
vpath %.c $(shell find $(ZB_EXAMPLE_COMMON_DIR) -type d)

MODULE_IFLAGS += -I$(ZB_EXAMPLE_DIR)/

CSRC += \
		$(ZB_EXAMPLE_DIR)/example.c \


