include $(MAKE_INCLUDE_GEN)

.PHONY: lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/media_demux

MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/libs/src/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/audio_utils/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/media_utils/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/media_common/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/media_source/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/configs/include
MODULE_IFLAGS += -I$(DIR)/include

vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-unused-value -Wno-unused-variable -Wno-unused-but-set-variable

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CPPSRC  = $(DIR)/media_extractor.cpp
CPPSRC += $(DIR)/media_extractor_factory.cpp

#CONFIG_MEDIA_DEMUX_AAC:=y  CONFIG_MEDIA_LITE_DEMUX_AAC:=y
#CONFIG_MEDIA_DEMUX_WAV:=y  CONFIG_MEDIA_LITE_DEMUX_WAV:=y
#CONFIG_MEDIA_DEMUX_MP3:=y  CONFIG_MEDIA_LITE_DEMUX_MP3:=y
#CONFIG_MEDIA_DEMUX_MP4:=y  CONFIG_MEDIA_LITE_DEMUX_MP4:=y

ifeq ($(CONFIG_MEDIA_PLAYER),y)
GLOBAL_CFLAGS += -DMEDIA_PLAYER
endif

ifeq ($(CONFIG_MEDIA_LITE_PLAYER),y)
GLOBAL_CFLAGS += -DMEDIA_LITE_PLAYER
endif

#HELIX AAC
ifneq ($(CONFIG_MEDIA_CODEC_HAAC)$(CONFIG_MEDIA_LITE_CODEC_HAAC),)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_AAC_DELIVER_ADTS_HEADER
endif

# AAC demux
ifneq ($(CONFIG_MEDIA_DEMUX_AAC)$(CONFIG_MEDIA_LITE_DEMUX_AAC),)
LIBAAC_SRC_FILES := \
    $(DIR)/extractors/aac/aac_extractor.cpp

CPPSRC += $(LIBAAC_SRC_FILES)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_AAC
endif

# WAV demux
ifneq ($(CONFIG_MEDIA_DEMUX_WAV)$(CONFIG_MEDIA_LITE_DEMUX_WAV),)
LIBWAV_SRC_FILES := \
    $(DIR)/extractors/wav/wav_extractor.cpp

CPPSRC += $(LIBWAV_SRC_FILES)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_WAV
endif

# MP3 demux
ifneq ($(CONFIG_MEDIA_DEMUX_MP3)$(CONFIG_MEDIA_LITE_DEMUX_MP3),)
LIBMP3_SRC_FILES := \
    $(DIR)/extractors/mp3/mp3_extractor.cpp \
    $(DIR)/extractors/mp3/mp3_vbri_seeker.cpp \
    $(DIR)/extractors/mp3/mpr_xing_seeker.cpp \
    $(DIR)/extractors/mp3/mp3_id3.cpp

CPPSRC += $(LIBMP3_SRC_FILES)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_MP3
endif

# AMR demux
ifneq ($(CONFIG_MEDIA_DEMUX_AMR)$(CONFIG_MEDIA_LITE_DEMUX_AMR),)
LIBAMR_SRC_FILES := \
    $(DIR)/extractors/amr/amr_extractor.cpp

CPPSRC += $(LIBAMR_SRC_FILES)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_AMR
endif

# OGG demux
ifneq ($(CONFIG_MEDIA_DEMUX_OGG)$(CONFIG_MEDIA_LITE_DEMUX_OGG),)
CPPSRC += $(DIR)/extractors/ogg/ogg_extractor.cpp
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/tremolo
GLOBAL_CFLAGS += -DMEDIA_DEMUX_OGG
endif

# FLAC demux
ifneq ($(CONFIG_MEDIA_DEMUX_FLAC)$(CONFIG_MEDIA_LITE_DEMUX_FLAC),)
CPPSRC += $(DIR)/extractors/flac/flac_extractor.cpp
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/flac/include
GLOBAL_CFLAGS += -DMEDIA_DEMUX_FLAC
endif

# MP4 demux
ifneq ($(CONFIG_MEDIA_DEMUX_MP4)$(CONFIG_MEDIA_LITE_DEMUX_MP4),)
LIBMP4_SRC_FILES := \
    $(DIR)/extractors/mp4/mpeg4_extractor.cpp \
    $(DIR)/extractors/mp4/sample_table.cpp \
    $(DIR)/extractors/mp4/sample_iterator.cpp

CPPSRC += $(LIBMP4_SRC_FILES)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_MP4
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
lib: COPY_RAM_OBJS
	cp *.o  ../media_xip

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
