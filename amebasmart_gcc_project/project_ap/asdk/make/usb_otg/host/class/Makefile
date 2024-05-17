
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DRV_DIR = $(BASEDIR)/component/usb
COM_DIR = $(DRV_DIR)/common
HOST_DIR = $(DRV_DIR)/host
VERCLASS_DIR = $(HOST_DIR)/verify
VER_DIR = $(DRV_DIR)/verification/host
MODULE_IFLAGS += -I$(COM_DIR)
MODULE_IFLAGS += -I$(HOST_DIR)/core
DIR =

ifeq ($(CONFIG_USBH_CDC_ACM),y)
ACM_DIR = $(HOST_DIR)/cdc_acm
MODULE_IFLAGS += -I$(ACM_DIR)
DIR += $(ACM_DIR)
endif

ifeq ($(CONFIG_USBH_CDC_ECM),y)
ECM_DIR = $(HOST_DIR)/cdc_ecm
MODULE_IFLAGS += -I$(ECM_DIR)
DIR += $(ECM_DIR)
endif

ifeq ($(CONFIG_USBH_MSC),y)
MSC_DIR = $(HOST_DIR)/msc
MODULE_IFLAGS += -I$(MSC_DIR)
DIR += $(MSC_DIR)
endif

ifeq ($(CONFIG_USBH_UVC),y)
UVC_DIR = $(HOST_DIR)/uvc
MODULE_IFLAGS += -I$(UVC_DIR)
DIR += $(UVC_DIR)
endif

ifeq ($(CONFIG_USBH_VENDOR),y)
VENDOR_DIR = $(HOST_DIR)/vendor
MODULE_IFLAGS += -I$(VENDOR_DIR)
DIR += $(VENDOR_DIR)
endif

ifeq ($(CONFIG_USBH_VERIFY),y)
MODULE_IFLAGS += -I$(VER_DIR) -I$(VERCLASS_DIR)
DIR += $(VER_DIR) $(VERCLASS_DIR)
endif

vpath %.c $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =

ifeq ($(CONFIG_USBH_CDC_ACM),y)
CSRC += $(ACM_DIR)/usbh_cdc_acm.c
endif

ifeq ($(CONFIG_USBH_CDC_ECM),y)
CSRC += $(ECM_DIR)/usbh_cdc_ecm.c \
	$(ECM_DIR)/usbh_cdc_ecm_hal.c
endif

ifeq ($(CONFIG_USBH_MSC),y)
CSRC += $(MSC_DIR)/usbh_msc.c \
	$(MSC_DIR)/usbh_msc_scsi.c \
	$(MSC_DIR)/usbh_msc_disk.c
endif

ifeq ($(CONFIG_USBH_UVC),y)
CSRC += $(UVC_DIR)/usbh_uvc_class.c \
	$(UVC_DIR)/usbh_uvc_intf.c \
	$(UVC_DIR)/usbh_uvc_parse.c \
	$(UVC_DIR)/usbh_uvc_stream.c
endif

ifeq ($(CONFIG_USBH_VENDOR),y)
CSRC += $(VENDOR_DIR)/usbh_vendor.c
endif

ifeq ($(CONFIG_USBH_VERIFY),y)
CSRC += $(VER_DIR)/cts.c \
	$(VER_DIR)/cts_xfer_test.c \
	$(VER_DIR)/emc.c \
	$(VER_DIR)/usbh_cmd.c
CSRC += $(VERCLASS_DIR)/usbh_verify.c \
	$(VER_DIR)/verify.c
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
