include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/bluetooth

vpath %.c $(DIR)/driver/hci \
		$(DIR)/driver/hci/hci_process \
		$(DIR)/driver/hci/hci_transport \
		$(DIR)/driver/platform/amebalite/hci \
		$(DIR)/rtk_stack/platform/common \
		$(DIR)/osif

MODULE_IFLAGS += \
	-I$(BASEDIR)/component/wifi/driver \
	-I$(BASEDIR)/component/wifi/driver/phl \
	-I$(BASEDIR)/component/wifi/driver/phl_g6 \
	-I$(BASEDIR)/component/wifi/driver/phldm \
	-I$(BASEDIR)/component/wifi/driver/halmac \
	-I$(BASEDIR)/component/wifi/driver/halcommon \
	-I$(BASEDIR)/component/wifi/driver/halbtc \
	-I$(BASEDIR)/component/wifi/driver/rtl8720e \
	-I$(BASEDIR)/component/wifi/driver/rtl8720e_rom

CSRC += \
	$(DIR)/driver/hci/hci_process/hci_process.c \
	$(DIR)/driver/hci/hci_process/hci_common.c \
	$(DIR)/driver/hci/hci_process/hci_standalone.c \
	$(DIR)/driver/hci/hci_transport/hci_h4.c \
	$(DIR)/driver/platform/amebalite/hci/bt_normal_patch.c \
	$(DIR)/driver/platform/amebalite/hci/bt_mp_patch.c \
	$(DIR)/driver/platform/amebalite/hci/hci_platform.c \
	$(DIR)/driver/platform/amebalite/hci/hci_uart.c \
	$(DIR)/osif/bt_debug.c \
	$(DIR)/osif/osif.c

ifeq ($(CONFIG_BT_ZEPHYR), y)
CSRC += \
	$(DIR)/driver/hci/hci_if_zephyr.c
else
CSRC += \
	$(DIR)/driver/hci/hci_if_rtk.c \
	$(DIR)/rtk_stack/platform/common/platform_utils.c \
	$(DIR)/rtk_stack/platform/common/trace_uart.c
endif

OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                          RULES TO GENERATE TARGETS                          #
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
#                            GENERATE OBJECT FILE                             #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                           RULES TO CLEAN TARGETS                            #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
