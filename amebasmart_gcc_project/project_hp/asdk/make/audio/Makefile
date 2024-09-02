
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
all: open_src test

open_src: CORE_TARGETS
ifeq ($(CONFIG_AUDIO_FWK),y)
	make -C audio_hal all
endif

ifeq ($(CONFIG_AUDIO_FWK),y)
	make -C audio_config config
endif

test: CORE_TARGETS
ifeq ($(CONFIG_AUDIO_FWK),y)
	#make -C audio_test all
	#make -C audio_hal/test
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
	make -C audio_config clean
	make -C audio_xip clean
	make -C base clean
	make -C third_party clean
-include $(DEPS)
