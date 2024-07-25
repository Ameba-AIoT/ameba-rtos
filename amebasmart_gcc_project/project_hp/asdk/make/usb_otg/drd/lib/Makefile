
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
HAL_DIR = $(TARGETDIR)/fwlib/ram_common
DRV_DIR = $(BASEDIR)/component/usb
COM_DIR = $(DRV_DIR)/common
USBD_CORE_DIR = $(DRV_DIR)/device/core
USBH_CORE_DIR = $(DRV_DIR)/host/core
MODULE_IFLAGS += -I$(COM_DIR)
MODULE_IFLAGS += -I$(USBD_CORE_DIR)
MODULE_IFLAGS += -I$(USBH_CORE_DIR)
TARGET_LIB = lib_usb

vpath %.c $(HAL_DIR) $(COM_DIR) $(USBD_CORE_DIR) $(USBH_CORE_DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += $(HAL_DIR)/ameba_usb.c \
	$(COM_DIR)/usb_hal.c \
	$(COM_DIR)/usb_os.c \
	$(USBH_CORE_DIR)/usbd.c \
	$(USBH_CORE_DIR)/usbd_core.c \
	$(USBH_CORE_DIR)/usbd_hal.c \
	$(USBH_CORE_DIR)/usbd_pcd.c \
	$(USBH_CORE_DIR)/usbh.c \
	$(USBH_CORE_DIR)/usbh_core.c \
	$(USBH_CORE_DIR)/usbh_hal.c \
	$(USBH_CORE_DIR)/usbh_hcd.c
	
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
	
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
# Define the Rules to build the core targets
all: CORE_TARGETS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
	
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
