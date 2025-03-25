include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#
BT_DIR = $(BASEDIR)/component/bluetooth
MODULE_IFLAGS += -I$(BASEDIR)/component/at_cmd

vpath %.c $(BT_DIR)/example/ble_wifimate \
		  $(BT_DIR)/example/bt_atcmd \
		  $(BT_DIR)/example/gatt_service/client

MODULE_IFLAGS += \
	-I$(BT_DIR)/example/ble_wifimate

#*****************************************************************************#
#                        DEVICE APP FILES                                     #
#*****************************************************************************#

ifeq ($(CONFIG_BT_WIFIMATE_DEVICE), y)
CSRC += \
	$(BT_DIR)/example/ble_wifimate/ble_wifimate_device.c \
	$(BT_DIR)/example/ble_wifimate/ble_wifimate_server.c
endif

ifeq ($(CONFIG_BT_WIFIMATE_CONFIGURATOR), y)
	CSRC += \
	$(BT_DIR)/example/ble_wifimate/ble_wifimate_configurator.c \
	$(BT_DIR)/example/ble_wifimate/ble_wifimate_client.c \
	$(BT_DIR)/example/bt_atcmd/atcmd_bt_wifimate_configurator.c
endif

CSRC += \
	$(BT_DIR)/example/gatt_service/client/rtk_gcs_client.c

OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
# LIBBLUETOOTH_OBJS_PATH = $(shell pwd)/objs
# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
	# mkdir -p $(LIBBLUETOOTH_OBJS_PATH)
	# for i in $(OBJS); do \
	# 	 $(COPY)  $$i $(LIBBLUETOOTH_OBJS_PATH) -f;\
	# 	done
	# $(REMOVE) *.i
	# $(REMOVE) *.s
	# $(REMOVE) *.d

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)