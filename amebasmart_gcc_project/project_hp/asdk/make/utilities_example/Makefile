
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/cJSON
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/coap/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/mqtt/MQTTClient
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/httplite
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/xml
MODULE_IFLAGS               += -I$(BASEDIR)/component/bluetooth
MODULE_IFLAGS               += -I$(BASEDIR)/component/sdio/sd_host/inc
MODULE_IFLAGS               += -I$(BASEDIR)/component/wifi/realmesh/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/bluetooth/example/bt_mesh_provisioner_rtk_demo
MODULE_IFLAGS               += -I$(BASEDIR)/component/bluetooth/example/bt_mesh_device_rtk_demo
MODULE_IFLAGS               += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/non_secure
MODULE_IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/secure
.PHONY: all clean

ifeq ($(CONFIG_USB_DEVICE_EN),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/common
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/core
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/cdc_acm
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/composite
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/hid
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/msc
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/vendor
endif
ifeq ($(CONFIG_USB_HOST_EN),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/common
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/core
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/cdc_acm
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/cdc_ecm
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/msc
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/vendor
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/uvc
MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ringbuffer
endif
ifeq ($(CONFIG_USB_DRD_EN),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/common
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/core
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/cdc_acm
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/composite
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/hid
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/msc
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/device/vendor
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/core
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/cdc_acm
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/cdc_ecm
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/msc
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/vendor
MODULE_IFLAGS += -I$(BASEDIR)/component/usb/host/uvc
MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ringbuffer
endif

ifeq ($(CONFIG_AUDIO_FWK),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/example/audio_track
MODULE_IFLAGS += -I$(BASEDIR)/component/example/audio_record
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
endif

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/example
RM_DIRS = 'example/audio_mp3|example/audio_amr|example/audio_helix_aac|example/audio_helix_mp3|example/audio_m4a_selfparse|example/audio_m4a|example/audio_pcm_upload|example/audio_hls'

ifneq ($(EXAMPLE),)
TARGET_DIR = $(shell find $(DIR) -maxdepth 2 -type d|grep -vE $(RM_DIRS) | grep -w $(EXAMPLE))
vpath %.c $(TARGET_DIR)
endif

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

# NOTE: use "make EXAMPLE=xxx" to build a example, xxx is the example folder name.
# For example, to build ssl download example, use "make EXAMPLE=ssl_download" to build.
# There is no need to add .c file to compile manually.

ifneq ($(EXAMPLE),)
CSRC = $(wildcard $(TARGET_DIR)/*.c)
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: clean COPY_RAM_OBJS

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
