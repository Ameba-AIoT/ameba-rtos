include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/media_codec/codec-omx

MODULE_IFLAGS += \
    -I$(BASEDIR)/component/audio/base/libs/src/include \
    -I$(BASEDIR)/component/audio/base/log/interfaces \
    -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces \
    -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces \
    -I$(BASEDIR)/component/audio/base/cutils/interfaces \
    -I$(BASEDIR)/component/audio/base/audio_utils/include \
    -I$(BASEDIR)/component/audio/media_utils/include \
    -I$(BASEDIR)/component/audio/media_common/include \
    -I$(BASEDIR)/component/audio/configs/include \
    -I$(DIR)/include \
    -I$(DIR)/include/media/codec/omxil

vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
#CONFIG_MEDIA_PLAYER
#CONFIG_MEDIA_CODEC_PCM:=y
#CONFIG_MEDIA_CODEC_AAC:=y
#CONFIG_MEDIA_CODEC_HAAC:=y
#CONFIG_MEDIA_CODEC_MP3:=y
#CONFIG_MEDIA_CODEC_VORBIS:=y
#CONFIG_MEDIA_CODEC_OPUS:=y

CPPSRC =

ifeq ($(CONFIG_MEDIA_PLAYER),y)
CPPSRC += \
    $(DIR)/media_codec.cpp \
    $(DIR)/codec.cpp \
    $(DIR)/codec_buffer_tunnel.cpp \
    $(DIR)/data_converter.cpp \
    $(DIR)/omx/omx_buffer.cpp \
    $(DIR)/omx/omx_utils.cpp \
    $(DIR)/omx/omx_node.cpp \
    $(DIR)/omx/omx.cpp \
    $(DIR)/omx/omx_core.cpp \
    $(DIR)/omx/sw_omx_component.cpp

GLOBAL_CFLAGS += -DMEDIA_PLAYER

# rawpcm codec
ifeq ($(CONFIG_MEDIA_CODEC_PCM),y)
LIBRAW_SRC_FILES := $(DIR)/codecs/rawdec/sw_raw_dec.cpp
CPPSRC += $(LIBRAW_SRC_FILES)
GLOBAL_CFLAGS += -DMEDIA_CODEC_PCM
endif

# aac codec
ifeq ($(CONFIG_MEDIA_CODEC_AAC),y)
LIBAAC_SRC_FILES := \
    $(DIR)/codecs/aacdec/drc_pres_mode_wrap.cpp \
    $(DIR)/codecs/aacdec/sw_aac2_dec.cpp

LIBAAC_C_INCLUDES := \
    -I$(BASEDIR)/component/audio/third_party/aac/libAACdec/include \
    -I$(BASEDIR)/component/audio/third_party/aac/libPCMutils/include \
    -I$(BASEDIR)/component/audio/third_party/aac/libFDK/include \
    -I$(BASEDIR)/component/audio/third_party/aac/libSYS/include \
    -I$(BASEDIR)/component/audio/third_party/aac/libMpegTPDec/include \
    -I$(BASEDIR)/component/audio/third_party/aac/libSBRdec/include \
    -I$(BASEDIR)/component/audio/third_party/aac/libArithCoding/include \
    -I$(BASEDIR)/component/audio/third_party/aac/libDRCdec/include \
    -I$(BASEDIR)/component/audio/third_party/aac/libSACdec/include

CPPSRC += $(LIBAAC_SRC_FILES)
MODULE_IFLAGS += $(LIBAAC_C_INCLUDES)
GLOBAL_CFLAGS += -DMEDIA_CODEC_AAC
endif

# haac codec
ifeq ($(CONFIG_MEDIA_CODEC_HAAC),y)
LIBHAAC_SRC_FILES := \
    $(DIR)/codecs/helixaacdec/sw_helix_aac_dec.cpp

LIBHAAC_C_INCLUDES := \
    -I$(BASEDIR)/component/audio/third_party/haac

CPPSRC += $(LIBHAAC_SRC_FILES)
MODULE_IFLAGS += $(LIBHAAC_C_INCLUDES)
GLOBAL_CFLAGS += -DMEDIA_CODEC_HAAC
endif

# mp3 codec
ifeq ($(CONFIG_MEDIA_CODEC_MP3),y)
LIBMP3_SRC_FILES := \
	$(DIR)/codecs/mp3dec/sw_mp3_dec.cpp

LIBMP3_C_INCLUDES := \
	-I$(DIR)/codecs/mp3dec/ \
	-I$(BASEDIR)/component/audio/media_codec/common/pvmp3/include \
	-I$(BASEDIR)/component/audio/media_codec/common/pvmp3/src

CPPSRC += $(LIBMP3_SRC_FILES)
MODULE_IFLAGS += $(LIBMP3_C_INCLUDES)
GLOBAL_CFLAGS += -DMEDIA_CODEC_MP3
endif

# opus codec
ifeq ($(CONFIG_MEDIA_CODEC_VORBIS),y)
CPPSRC += $(DIR)/codecs/vorbisdec/sw_vorbis_dec.cpp
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/tremolo
GLOBAL_CFLAGS += -DMEDIA_CODEC_VORBIS
endif

# opus codec
ifeq ($(CONFIG_MEDIA_CODEC_OPUS),y)
CPPSRC += $(DIR)/codecs/vorbisdec/sw_opus_dec.cpp
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/libopus/include
GLOBAL_CFLAGS += -DMEDIA_CODEC_OPUS
endif
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CPPSRC:.cpp=.o))

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
