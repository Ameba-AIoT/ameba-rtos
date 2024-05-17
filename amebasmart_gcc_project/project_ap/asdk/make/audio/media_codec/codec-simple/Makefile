include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/media_codec/codec-simple

MODULE_IFLAGS += \
    -I$(BASEDIR)/component/audio/base/libs/src/include \
    -I$(BASEDIR)/component/audio/base/log/interfaces \
    -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces \
    -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces \
    -I$(BASEDIR)/component/audio/base/cutils/interfaces \
    -I$(BASEDIR)/component/audio/base/audio_utils/include \
    -I$(BASEDIR)/component/audio/configs/include \
    -I$(DIR)/include

####MODULE_IFLAGS += -I$(BASEDIR)/component/stdlib

vpath %.c $(shell find $(DIR) -type d)
vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Werror

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
#CONFIG_MEDIA_PLAYER
#CONFIG_MEDIA_LITE_CODEC_PCM:=y
#CONFIG_MEDIA_LITE_CODEC_HAAC:=y
#CONFIG_MEDIA_LITE_CODEC_MP3:=y

CSRC = $(DIR)/sw_media_codec_factory.c

GLOBAL_CFLAGS += -DMEDIA_LITE_PLAYER

# rawpcm codec
ifeq ($(CONFIG_MEDIA_LITE_CODEC_PCM),y)
LIBRAW_SRC_FILES := $(DIR)/codecs/rawdec/sw_raw_dec_lite.c
CSRC += $(LIBRAW_SRC_FILES)
GLOBAL_CFLAGS += -DMEDIA_LITE_CODEC_PCM
endif

# haac codec
ifeq ($(CONFIG_MEDIA_LITE_CODEC_HAAC),y)
LIBHAAC_SRC_FILES := \
    $(DIR)/codecs/helixaacdec/sw_helix_aac_dec_lite.c

LIBHAAC_C_INCLUDES := \
    -I$(BASEDIR)/component/audio/third_party/haac

CSRC += $(LIBHAAC_SRC_FILES)
MODULE_IFLAGS += $(LIBHAAC_C_INCLUDES)
GLOBAL_CFLAGS += -DMEDIA_LITE_CODEC_HAAC
endif

# mp3 codec
ifeq ($(CONFIG_MEDIA_LITE_CODEC_MP3),y)
CSRC += $(DIR)/codecs/mp3dec/sw_mp3_dec_lite.c

LIBMP3_C_INCLUDES := \
	-I$(DIR)/codecs/mp3dec \
	-I$(BASEDIR)/component/audio/media_codec/common/pvmp3/include \
	-I$(BASEDIR)/component/audio/media_codec/common/pvmp3/src

MODULE_IFLAGS += $(LIBMP3_C_INCLUDES)
GLOBAL_CFLAGS += -DMEDIA_LITE_CODEC_MP3
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
	cp *.o  ../../media_xip

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
