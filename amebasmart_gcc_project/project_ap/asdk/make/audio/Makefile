
include $(MAKE_INCLUDE_GEN)

.PHONY: all lib test clean


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: open_src config test cmd

open_src: CORE_TARGETS
ifeq ($(CONFIG_AUDIO_FWK),y)
	make -C audio_hal all
endif

cmd: CORE_TARGETS
ifeq ($(CONFIG_AUDIO_FWK),y)
	#make -C base all
endif

config: CORE_TARGETS
ifneq ($(CONFIG_MEDIA_PLAYER)$(CONFIG_MEDIA_LITE_PLAYER),)
	make -C media_config config
endif

ifeq ($(CONFIG_AUDIO_FWK),y)
	make -C audio_config config
endif

test: CORE_TARGETS
ifeq ($(CONFIG_AUDIO_FWK),y)
	#make -C audio_test all
	#make -C audio_hal/test test
endif
ifneq ($(CONFIG_MEDIA_PLAYER)$(CONFIG_MEDIA_LITE_PLAYER),)
	#make -C media_test test
endif

lib: CORE_TARGETS
ifeq ($(CONFIG_AUDIO_FWK),y)
	make -C audio_algorithm lib
	make -C audio_effect lib
	make -C audio_control lib
	make -C audio_stream lib
	make -C audio_xip lib
	make -C base lib
endif
ifneq ($(CONFIG_MEDIA_PLAYER)$(CONFIG_MEDIA_LITE_PLAYER),)
	make -C base lib
	make -C media_utils lib
	make -C media_common lib
	make -C media_source lib
endif
ifneq ($(CONFIG_MEDIA_PLAYER)$(CONFIG_MEDIA_LITE_PLAYER),)
	make -C media_demux lib
endif
ifeq ($(CONFIG_MEDIA_PLAYER),y)
	make -C media_codec codec-omx
	make -C media_core standard
	make -C media_libmedia standard
endif
ifeq ($(CONFIG_MEDIA_LITE_PLAYER),y)
	make -C media_codec codec-simple
	make -C media_core lite
	make -C media_libmedia lite
endif
ifneq ($(CONFIG_MEDIA_PLAYER)$(CONFIG_MEDIA_LITE_PLAYER),)
	make -C media_libmedia component
	make -C media_xip lib
endif
ifeq ($(CONFIG_WIFI_AUDIO),y)
	make -C third_party wfa
endif
	make -C third_party lib

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C audio_hal clean
	make -C audio_algorithm clean
	make -C audio_effect clean
	make -C audio_control clean
	make -C audio_stream clean
	make -C audio_test clean
	make -C audio_xip clean
	make -C audio_config clean
	make -C base clean
	make -C third_party clean
	make -C media_config clean
	make -C media_utils clean
	make -C media_common clean
	make -C media_source clean
	make -C media_demux clean
	make -C media_codec clean
	make -C media_core clean
	make -C media_libmedia clean
	make -C media_xip clean
	make -C media_test clean
-include $(DEPS)
