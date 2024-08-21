
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/cJSON
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/coap/include
#MODULE_IFLAGS               += -I$(BASEDIR)/component/network/libcoap/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/mqtt/MQTTClient
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/http2/nghttp2-1.31.0/includes
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/httplite
MODULE_IFLAGS               += -I$(BASEDIR)/component/network/xml
MODULE_IFLAGS               += -I$(BASEDIR)/component/bluetooth
MODULE_IFLAGS               += -I$(BASEDIR)/component/sdio/sd_host/inc
MODULE_IFLAGS               += -I$(BASEDIR)/component/wifi/realmesh/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/bluetooth/example/bt_mesh_provisioner_rtk_demo
MODULE_IFLAGS               += -I$(BASEDIR)/component/bluetooth/example/bt_mesh_device_rtk_demo
MODULE_IFLAGS               += -I$(FREERTOSDIR)/include
MODULE_IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/AmebaLite_KR4
.PHONY: all clean

ifeq ($(CONFIG_MEDIA_PLAYER),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/example/player
MODULE_IFLAGS += -I$(BASEDIR)/component/example/audio/speexdsp
MODULE_IFLAGS += -I$(BASEDIR)/component/example/mydata_source_player
MODULE_IFLAGS += -I$(BASEDIR)/component/example/player_cache_test
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/libs/src/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/utils/interfaces
endif

ifeq ($(CONFIG_AUDIO_FWK),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/example/audio_track
MODULE_IFLAGS += -I$(BASEDIR)/component/example/audio_record
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
endif

ifeq ($(CONFIG_RPC_EN),y)
MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ipc/rpc/include
MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ipc/rpc/xdr/include
MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ipc/rpc/src/aidl
MODULE_IFLAGS += -I$(BASEDIR)/component/example/audio/voice
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

EXAMPLE_MF = $(shell find ./ -type d | grep -w $(EXAMPLE))
endif

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

# NOTE: use "make EXAMPLE=xxx" to build a example, xxx is the example folder name.
# For example, to build ssl download example, use "make EXAMPLE=ssl_download" to build.
# There is no need to add .c file to compile manually.

ifneq ($(EXAMPLE),)
ifeq ($(EXAMPLE_MF),)
CSRC = $(wildcard $(TARGET_DIR)/*.c)
else
include $(EXAMPLE_MF)/Makefile
endif
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
