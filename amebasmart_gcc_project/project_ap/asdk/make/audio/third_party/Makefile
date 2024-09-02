include $(MAKE_INCLUDE_GEN)

.PHONY: all lib wfa clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets

all:lib

lib:
ifeq ($(CONFIG_MEDIA_CODEC_HAAC),y)
	make -C haac all
endif
ifeq ($(CONFIG_MEDIA_CODEC_AAC),y)
	make -C fdk-aac all
endif
ifeq ($(CONFIG_MEDIA_DEMUX_FLAC),y)
	make -C flac all
endif
ifeq ($(CONFIG_MEDIA_CODEC_VORBIS),y)
	make -C tremolo all
endif
ifneq ($(CONFIG_MEDIA_CODEC_OPUS)$(CONFIG_OPUS_LIB),)
	make -C opus all
endif
ifeq ($(CONFIG_SPEEX_LIB),y)
	make -C speexdsp all
endif

wfa:
	make -C flac all

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C haac clean
	make -C fdk-aac clean
	make -C flac clean
	make -C tremolo clean
	make -C opus clean
	make -C speexdsp clean

-include $(DEPS)
