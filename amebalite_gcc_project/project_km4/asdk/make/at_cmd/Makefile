
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS += -I$(BASEDIR)/component/network/mqtt/MQTTClient
MODULE_IFLAGS += -I$(BASEDIR)/component/network/mqtt/MQTTPacket

.PHONY: all clean

#*****************************************************************************#
#                                 VARIABLES                                   #
#*****************************************************************************#
APP_DIR = $(TARGETDIR)/app/monitor
DIR = $(BASEDIR)/component/at_cmd

ifeq ($(CONFIG_WLAN),y)
ifeq ($(findstring y, $(CONFIG_AS_INIC_AP) $(CONFIG_SINGLE_CORE_WIFI)), y)
GLOBAL_CFLAGS += -DSUPPORT_LOG_SERVICE=1
endif
endif

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
vpath %.c $(APP_DIR) $(shell find $(APP_DIR) -type d)

CSRC = $(APP_DIR)/ram/monitor_km4.c \
       $(APP_DIR)/ram/shell_ram.c \
       $(APP_DIR)/ram/low_level_io.c

ifeq ($(CONFIG_LONGER_CMD),y)
CSRC += $(APP_DIR)/rom/shell_rom_patch.c
endif

ifeq ($(CONFIG_WLAN),y)
# apply old atcmd, which should be deleted after new version is ready
ifneq ($(CONFIG_NEW_ATCMD),y)
DIR_OLD = $(BASEDIR)/component/at_cmd/old

vpath %.c $(DIR_OLD) $(DIR)
MODULE_IFLAGS += -I $(DIR_OLD)

ifeq ($(findstring y, $(CONFIG_AS_INIC_AP) $(CONFIG_SINGLE_CORE_WIFI)), y)
CSRC +=	$(DIR)/log_service.c
CSRC +=	$(DIR_OLD)/atcmd_wifi.c \
		$(DIR_OLD)/atcmd_sys.c \
		$(DIR_OLD)/atcmd_bt_mp.c \
		$(DIR_OLD)/atcmd_uart_bridge_old.c
ifeq ($(CONFIG_BT), y)
CSRC += $(DIR_OLD)/atcmd_bt_ext.c
endif
# zigbee
ifeq ($(CONFIG_802154_ZIGBEE_EN), y)
CSRC += $(DIR_OLD)/atcmd_wpan.c
endif
endif

else # ifneq ($(CONFIG_NEW_ATCMD),y)
# note: apply new atcmd, add your config here!!!
vpath %.c $(DIR)

CSRC += $(DIR)/log_service.c \
		$(DIR)/atcmd_sys.c \
		$(DIR)/atcmd_wifi.c \
		$(DIR)/atcmd_mqtt.c \
		$(DIR)/atcmd_lwip.c \
		$(DIR)/atcmd_bt_mp.c
ifeq ($(CONFIG_BT), y)
CSRC += $(DIR)/atcmd_bt_ext.c
endif

ifeq ($(CONFIG_ATCMD_IO_UART), y)
CSRC += $(DIR)/at_intf_uart.c
endif
endif
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
MODULE_IFLAGS += -I$(DIR) \
				-I$(APP_DIR)/include \
				-I$(FREERTOSDIR)/include/ \
				-I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/non_secure \
				-I$(FREERTOSDIR)/portable/GCC/AmebaLite_KM4/secure

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
