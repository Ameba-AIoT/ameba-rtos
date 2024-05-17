include $(MAKE_INCLUDE_GEN)

.PHONY: config clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/configs

MODULE_IFLAGS += -I$(DIR)/include

vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -std=gnu++17 -std=c++17

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CPPSRC = $(DIR)/ameba_media_extractor_usrcfg.cpp
CPPSRC += $(DIR)/ameba_media_decoder_usrcfg.cpp

########## PLAYER CONFIGS ##########
ifeq ($(CONFIG_MEDIA_PLAYER),y)
GLOBAL_CFLAGS += -DMEDIA_PLAYER

#HELIC AAC
#ifeq ($(CONFIG_MEDIA_CODEC_HAAC),y)
#GLOBAL_CFLAGS += -DMEDIA_DEMUX_HELIC
#endif

# AAC demux
ifeq ($(CONFIG_MEDIA_DEMUX_AAC),y)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_AAC
endif

# WAV demux
ifeq ($(CONFIG_MEDIA_DEMUX_WAV),y)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_WAV
endif

# MP3 demux
ifeq ($(CONFIG_MEDIA_DEMUX_MP3),y)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_MP3
endif

# AMR demux
ifeq ($(CONFIG_MEDIA_DEMUX_AMR),y)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_AMR
endif

# OGG demux
ifeq ($(CONFIG_MEDIA_DEMUX_OGG),y)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_OGG
endif

# FLAC demux
ifeq ($(CONFIG_MEDIA_DEMUX_FLAC),y)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_FLAC
endif

# MP4 demux
ifeq ($(CONFIG_MEDIA_DEMUX_MP4),y)
GLOBAL_CFLAGS += -DMEDIA_DEMUX_MP4
endif

# rawpcm codec
ifeq ($(CONFIG_MEDIA_CODEC_PCM),y)
GLOBAL_CFLAGS += -DMEDIA_CODEC_PCM
endif

# mp3 codec
ifeq ($(CONFIG_MEDIA_CODEC_MP3),y)
GLOBAL_CFLAGS += -DMEDIA_CODEC_MP3
endif

# aac codec
ifeq ($(CONFIG_MEDIA_CODEC_AAC),y)
GLOBAL_CFLAGS += -DMEDIA_CODEC_AAC
endif

# haac codec
ifeq ($(CONFIG_MEDIA_CODEC_HAAC),y)
GLOBAL_CFLAGS += -DMEDIA_CODEC_HAAC
endif

# vorbis codec
ifeq ($(CONFIG_MEDIA_CODEC_VORBIS),y)
GLOBAL_CFLAGS += -DMEDIA_CODEC_VORBIS
endif

# opus codec
ifeq ($(CONFIG_MEDIA_CODEC_OPUS),y)
GLOBAL_CFLAGS += -DMEDIA_CODEC_OPUS
endif
endif


########## LITE PLAYER CONFIGS ##########
ifeq ($(CONFIG_MEDIA_LITE_PLAYER),y)
GLOBAL_CFLAGS += -DMEDIA_LITE_PLAYER

# AAC demux
ifeq ($(CONFIG_MEDIA_LITE_DEMUX_AAC),y)
GLOBAL_CFLAGS += -DMEDIA_LITE_DEMUX_AAC
endif

# WAV demux
ifeq ($(CONFIG_MEDIA_LITE_DEMUX_WAV),y)
GLOBAL_CFLAGS += -DMEDIA_LITE_DEMUX_WAV
endif

# MP3 demux
ifeq ($(CONFIG_MEDIA_LITE_DEMUX_MP3),y)
GLOBAL_CFLAGS += -DMEDIA_LITE_DEMUX_MP3
endif

# MP4 demux
ifeq ($(CONFIG_MEDIA_LITE_DEMUX_MP4),y)
GLOBAL_CFLAGS += -DMEDIA_LITE_DEMUX_MP4
endif

# rawpcm codec
ifeq ($(CONFIG_MEDIA_LITE_CODEC_PCM),y)
GLOBAL_CFLAGS += -DMEDIA_LITE_CODEC_PCM
endif

# mp3 codec
ifeq ($(CONFIG_MEDIA_LITE_CODEC_MP3),y)
GLOBAL_CFLAGS += -DMEDIA_LITE_CODEC_MP3
endif

# haac codec
ifeq ($(CONFIG_MEDIA_LITE_CODEC_HAAC),y)
GLOBAL_CFLAGS += -DMEDIA_LITE_CODEC_HAAC
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
config: COPY_RAM_OBJS

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
