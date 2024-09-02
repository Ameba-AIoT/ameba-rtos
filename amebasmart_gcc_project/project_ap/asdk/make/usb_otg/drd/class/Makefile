
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DRV_DIR = $(BASEDIR)/component/usb
COM_DIR = $(DRV_DIR)/common
USBD_DIR = $(DRV_DIR)/device
USBH_DIR = $(DRV_DIR)/host
USBD_VER_APP_DIR = $(DRV_DIR)/verification/device
USBH_VER_APP_DIR = $(DRV_DIR)/verification/host
MODULE_IFLAGS += -I$(COM_DIR)
MODULE_IFLAGS += -I$(USBD_DIR)/core
MODULE_IFLAGS += -I$(USBH_DIR)/core
DIR =

ifeq ($(CONFIG_USBD_CDC_ACM),y)
USBD_CDC_ACM_DIR = $(USBD_DIR)/cdc_acm
DIR += $(USBD_CDC_ACM_DIR)
MODULE_IFLAGS += -I$(USBD_CDC_ACM_DIR)
endif

ifeq ($(CONFIG_USBD_COMPOSITE),y)
USBD_COMP_DIR = $(USBD_DIR)/composite
DIR += $(USBD_COMP_DIR)
MODULE_IFLAGS += -I$(USBD_COMP_DIR)
endif

ifeq ($(CONFIG_USBD_HID),y)
USBD_HID_DIR = $(USBD_DIR)/hid
DIR += $(USBD_HID_DIR)
MODULE_IFLAGS += -I$(USBD_HID_DIR)
endif

ifeq ($(CONFIG_USBD_MSC),y)
USBD_MSC_DIR = $(USBD_DIR)/msc
DIR += $(USBD_MSC_DIR)
MODULE_IFLAGS += -I$(USBD_MSC_DIR)
endif

ifeq ($(CONFIG_USBD_UAC),y)
USBD_UAC_DIR = $(DEV_DIR)/uac
DIR += $(USBD_UAC_DIR)
MODULE_IFLAGS += -I$(USBD_UAC_DIR)
endif

ifeq ($(CONFIG_USBD_VENDOR),y)
USBD_VEN_DIR = $(USBD_DIR)/vendor
DIR += $(USBD_VEN_DIR)
MODULE_IFLAGS += -I$(USBD_VEN_DIR)
endif

ifeq ($(CONFIG_USBD_VERIFY),y)
USBD_VER_DIR = $(USBD_DIR)/verify
MODULE_IFLAGS += -I$(USBD_VER_DIR) -I$(USBD_VER_APP_DIR)
DIR += $(USBD_VER_DIR) $(USBD_VER_APP_DIR)
endif

ifeq ($(CONFIG_USBH_CDC_ACM),y)
USBH_CDC_ACM_DIR = $(USBH_DIR)/cdc_acm
MODULE_IFLAGS += -I$(USBH_CDC_ACM_DIR)
DIR += $(USBH_CDC_ACM_DIR)
endif

ifeq ($(CONFIG_USBH_CDC_ECM),y)
USBH_CDC_ECM_DIR = $(USBH_DIR)/cdc_ecm
MODULE_IFLAGS += -I$(USBH_CDC_ECM_DIR)
DIR += $(USBH_CDC_ECM_DIR)
endif

ifeq ($(CONFIG_USBH_MSC),y)
USBH_MSC_DIR = $(USBH_DIR)/msc
MODULE_IFLAGS += -I$(USBH_MSC_DIR)
DIR += $(USBH_MSC_DIR)
endif

ifeq ($(CONFIG_USBH_UVC),y)
USBH_UVC_DIR = $(USBH_DIR)/uvc
MODULE_IFLAGS += -I$(USBH_UVC_DIR)
DIR += $(USBH_UVC_DIR)
endif

ifeq ($(CONFIG_USBH_VENDOR),y)
USBH_VEN_DIR = $(USBH_DIR)/vendor
MODULE_IFLAGS += -I$(USBH_VEN_DIR)
DIR += $(USBH_VEN_DIR)
endif

ifeq ($(CONFIG_USBH_VERIFY),y)
USBH_VER_DIR = $(USBH_DIR)/verify
MODULE_IFLAGS += -I$(USBH_VER_DIR) -I$(USBH_VER_APP_DIR)
DIR += $(USBH_VER_DIR) $(USBH_VER_APP_DIR)
endif

vpath %.c $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =

ifeq ($(CONFIG_USBD_CDC_ACM),y)
CSRC += $(USBD_CDC_ACM_DIR)/usbd_cdc_acm.c
endif

ifeq ($(CONFIG_USBD_COMPOSITE),y)
CSRC += $(USBD_COMP_DIR)/usbd_composite.c \
        $(USBD_COMP_DIR)/usbd_composite_cdc_acm.c \
        $(USBD_COMP_DIR)/usbd_composite_hid.c
endif

ifeq ($(CONFIG_USBD_HID),y)
CSRC += $(USBD_HID_DIR)/usbd_hid.c
endif

ifeq ($(CONFIG_USBD_MSC),y)
CSRC += $(USBD_MSC_DIR)/usbd_msc.c \
        $(USBD_MSC_DIR)/usbd_scsi.c
endif

ifeq ($(CONFIG_USBD_UAC),y)
CSRC += $(USBD_UAC_DIR)/usbd_uac.c
endif

ifeq ($(CONFIG_USBD_VENDOR),y)
CSRC += $(USBD_VEN_DIR)/usbd_vendor.c
endif

ifeq ($(CONFIG_USBD_VERIFY),y)
CSRC += $(USBD_VER_DIR)/usbd_verify.c \
	$(USBD_VER_APP_DIR)/usbd_cts.c \
	$(USBD_VER_APP_DIR)/usbd_cdc_acm_test.c \
	$(USBD_VER_APP_DIR)/usbd_cmd.c \
	$(USBD_VER_APP_DIR)/usbd_verify_test.c
endif

ifeq ($(CONFIG_USBH_CDC_ACM),y)
CSRC += $(USBH_CDC_ACM_DIR)/usbh_cdc_acm.c
endif

ifeq ($(CONFIG_USBH_CDC_ECM),y)
CSRC += $(USBH_CDC_ECM_DIR)/usbh_cdc_ecm.c \
	$(USBH_CDC_ECM_DIR)/usbh_cdc_ecm_hal.c
endif

ifeq ($(CONFIG_USBH_MSC),y)
CSRC += $(USBH_MSC_DIR)/usbh_msc.c \
	$(USBH_MSC_DIR)/usbh_msc_scsi.c \
	$(USBH_MSC_DIR)/usbh_msc_disk.c
endif

ifeq ($(CONFIG_USBH_UVC),y)
CSRC += $(USBH_UVC_DIR)/usbh_uvc_class.c \
	$(USBH_UVC_DIR)/usbh_uvc_intf.c \
	$(USBH_UVC_DIR)/usbh_uvc_parse.c \
	$(USBH_UVC_DIR)/usbh_uvc_stream.c
endif

ifeq ($(CONFIG_USBH_VENDOR),y)
CSRC += $(USBH_VEN_DIR)/usbh_vendor.c
endif

ifeq ($(CONFIG_USBH_VERIFY),y)
CSRC += $(USBH_VER_DIR)/usbh_verify.c \
	$(USBH_VER_APP_DIR)/usbh_cts.c \
	$(USBH_VER_APP_DIR)/usbh_cts_xfer_test.c \
	$(USBH_VER_APP_DIR)/usbh_emc.c \
	$(USBH_VER_APP_DIR)/usbh_cmd.c \
	$(USBH_VER_APP_DIR)/usbh_verify_test.c
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	
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
