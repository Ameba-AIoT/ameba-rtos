include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets

all:
ifneq ($(CONFIG_MEDIA_CODEC_MP3)$(CONFIG_MEDIA_LITE_CODEC_MP3),)
	make -C pvmp3 all
endif

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
ifneq ($(CONFIG_MEDIA_CODEC_MP3)$(CONFIG_MEDIA_LITE_CODEC_MP3),)
	make -C pvmp3 clean
endif

-include $(DEPS)