
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DRV_DIR = $(BASEDIR)/component/usb
COM_DIR = $(DRV_DIR)/common
DEV_DIR = $(DRV_DIR)/device
VERCLASS_DIR = $(DEV_DIR)/verify
VER_DIR = $(DRV_DIR)/verification/device
MODULE_IFLAGS += -I$(COM_DIR)
MODULE_IFLAGS += -I$(DEV_DIR)/core
DIR =

ifeq ($(CONFIG_USBD_CDC_ACM),y)
CDC_DIR = $(DEV_DIR)/cdc_acm
DIR += $(CDC_DIR)
MODULE_IFLAGS += -I$(CDC_DIR)
endif

ifeq ($(CONFIG_USBD_COMPOSITE),y)
COMP_DIR = $(DEV_DIR)/composite
DIR += $(COMP_DIR)
MODULE_IFLAGS += -I$(COMP_DIR)
endif

ifeq ($(CONFIG_USBD_HID),y)
HID_DIR = $(DEV_DIR)/hid
DIR += $(HID_DIR)
MODULE_IFLAGS += -I$(HID_DIR)
endif

ifeq ($(CONFIG_USBD_MSC),y)
MSC_DIR = $(DEV_DIR)/msc
DIR += $(MSC_DIR)
MODULE_IFLAGS += -I$(MSC_DIR)
endif

ifeq ($(CONFIG_USBD_VENDOR),y)
VENDOR_DIR = $(DEV_DIR)/vendor
DIR += $(VENDOR_DIR)
MODULE_IFLAGS += -I$(VENDOR_DIR)
endif

ifeq ($(CONFIG_USBD_VERIFY),y)
MODULE_IFLAGS += -I$(VER_DIR) -I$(VERCLASS_DIR)
DIR += $(VER_DIR) $(VERCLASS_DIR)
endif

vpath %.c $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =

ifeq ($(CONFIG_USBD_CDC_ACM),y)
CSRC += $(CDC_DIR)/usbd_cdc_acm.c
endif

ifeq ($(CONFIG_USBD_COMPOSITE),y)
CSRC += $(COMP_DIR)/usbd_composite.c \
	$(COMP_DIR)/usbd_composite_cdc_acm.c \
	$(COMP_DIR)/usbd_composite_hid.c
endif

ifeq ($(CONFIG_USBD_HID),y)
CSRC += $(HID_DIR)/usbd_hid.c
endif

ifeq ($(CONFIG_USBD_MSC),y)
CSRC += $(MSC_DIR)/usbd_msc.c \
	$(MSC_DIR)/usbd_scsi.c
endif

ifeq ($(CONFIG_USBD_VENDOR),y)
CSRC += $(VENDOR_DIR)/usbd_vendor.c
endif

ifeq ($(CONFIG_USBD_VERIFY),y)
CSRC += $(VER_DIR)/cts.c \
	$(VER_DIR)/cdc_acm.c \
	$(VER_DIR)/usbd_cmd.c
CSRC += $(VERCLASS_DIR)/usbd_verify.c \
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
