ZB_EXAMPLE_DIR	= $(BASEDIR)/component/wpan/zigbee/example/ha_on_off_light_n_switch/on_off_light_switch
ZB_EXAMPLE_COMMON_DIR = $(BASEDIR)/component/wpan/zigbee/example/common

vpath %.c $(ZB_EXAMPLE_DIR)/
vpath %.c $(shell find $(ZB_EXAMPLE_COMMON_DIR) -type d)

MODULE_IFLAGS += -I$(ZB_EXAMPLE_DIR)/
MODULE_IFLAGS += -I$(ZB_EXAMPLE_COMMON_DIR)/light_driver/
MODULE_IFLAGS += -I$(ZB_EXAMPLE_COMMON_DIR)/switch_driver/

CSRC += \
		$(ZB_EXAMPLE_DIR)/example.c \
		$(ZB_EXAMPLE_COMMON_DIR)/light_driver/light_driver.c \
		$(ZB_EXAMPLE_COMMON_DIR)/switch_driver/switch_driver.c \
