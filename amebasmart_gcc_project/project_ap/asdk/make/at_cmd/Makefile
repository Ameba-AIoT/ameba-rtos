
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
GLOBAL_CFLAGS += -DSUPPORT_ATCMD=1
endif
endif

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
vpath %.c $(APP_DIR) $(shell find $(APP_DIR) -type d)

CSRC = $(APP_DIR)/ram/monitor_ap.c \
       $(APP_DIR)/ram/shell_ram.c

ifeq ($(CONFIG_LONGER_CMD),y)
CSRC += $(APP_DIR)/rom/shell_rom_patch.c
else
CSRC += $(APP_DIR)/rom/shell_rom.c
endif

ifeq ($(CONFIG_AS_INIC_AP),y)
# note: apply new atcmd, add your config here!!!
vpath %.c $(DIR)

CSRC += $(DIR)/atcmd_service.c \
		$(DIR)/atcmd_sys.c \
		$(DIR)/atcmd_bt_mp.c

ifeq ($(CONFIG_WLAN),y)
CSRC +=	$(DIR)/atcmd_wifi.c \
		$(DIR)/atcmd_mqtt.c \
		$(DIR)/atcmd_lwip.c 
endif

ifeq ($(CONFIG_BT), y)
CSRC += $(DIR)/atcmd_bt_ext.c
endif

ifeq ($(CONFIG_ATCMD_IO_UART), y)
CSRC += $(DIR)/at_intf_uart.c
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
				-I$(FREERTOSDIR)/portable/GCC/ARM_CA7/

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
