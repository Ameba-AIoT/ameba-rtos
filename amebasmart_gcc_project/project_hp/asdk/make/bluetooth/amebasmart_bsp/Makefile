include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/bluetooth

vpath %.c $(DIR)/driver/hci \
		$(DIR)/driver/hci/hci_process \
		$(DIR)/driver/hci/hci_transport \
		$(DIR)/driver/platform/amebasmart/hci \
		$(DIR)/rtk_stack/platform/common \
		$(DIR)/osif \
		$(DIR)/bt_mp

CSRC += \
	$(DIR)/driver/hci/hci_process/hci_process.c \
	$(DIR)/driver/hci/hci_process/hci_common.c \
	$(DIR)/driver/hci/hci_process/hci_standalone.c \
	$(DIR)/driver/hci/hci_transport/hci_h4.c \
	$(DIR)/driver/platform/amebasmart/hci/bt_normal_patch.c \
	$(DIR)/driver/platform/amebasmart/hci/bt_mp_patch.c \
	$(DIR)/driver/platform/amebasmart/hci/hci_platform.c \
	$(DIR)/driver/platform/amebasmart/hci/hci_uart.c \
	$(DIR)/osif/bt_debug.c \
	$(DIR)/osif/osif.c \
	$(DIR)/bt_mp/bt_mp_api.c

ifneq ($(CONFIG_BT_INIC), y)
ifeq ($(CONFIG_BT_ZEPHYR), y)
CSRC += \
	$(DIR)/driver/hci/hci_if_zephyr.c
else
CSRC += \
	$(DIR)/driver/hci/hci_if_rtk.c \
	$(DIR)/rtk_stack/platform/common/platform_utils.c \
	$(DIR)/rtk_stack/platform/common/trace_uart.c
endif
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
