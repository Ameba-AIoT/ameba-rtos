include $(MAKE_INCLUDE_GEN)
include config.mk

.PHONY: all clean test lib

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/wifi-audio
AUDIO_DIR = $(BASEDIR)/component/audio

vpath %.cpp $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -Wno-unused-parameter -Wno-multichar
CPPFLAGS += -std=gnu++14

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
MODULE_IFLAGS += \
    -I$(BASEDIR)/component/wifi/wifi_cast

MODULE_IFLAGS += \
    -I$(AUDIO_DIR)/base/libs/src/include \
    -I$(AUDIO_DIR)/base/cutils/interfaces \
    -I$(AUDIO_DIR)/base/osal/osal_c/interfaces \
    -I$(AUDIO_DIR)/base/audio_utils/include \
    -I$(AUDIO_DIR)/interfaces \
    -I$(AUDIO_DIR)/audio_hal/base \
    -I$(AUDIO_DIR)/third_party/flac/include/FLAC

MODULE_IFLAGS += \
    -I$(DIR)/base/log/include \
    -I$(DIR)/base/cutils/include \
    -I$(DIR)/base/utils/include \
    -I$(DIR)/foundation/include \
    -I$(DIR)/plc \
    -I$(DIR)/fec \
    -I$(DIR)/fec/fec \
    -I$(DIR)/aac_helper \
    -I$(DIR)/flac_helper \
    -I$(DIR)/common \
    -I$(DIR)/source \
    -I$(DIR)/sink \
    -I$(DIR)/pal \
    -I$(DIR)/pal/include \
    -I$(DIR)/interfaces

CPPSRC  = \
    $(DIR)/source/wave_table.cpp \
    $(DIR)/source/wave_source_base.cpp \
    $(DIR)/source/sine_source.cpp \
    $(DIR)/source/pulse_source.cpp \
    $(DIR)/source/triangle_source.cpp \
    $(DIR)/source/wave_source.cpp \
    $(DIR)/source/audio_source.cpp \
    $(DIR)/source/capture_source.cpp \
    $(DIR)/source/wav_file_source.cpp \
    $(DIR)/source/wave_file_source.cpp \
    $(DIR)/source/audioficast.cpp \
    $(DIR)/source/audiofi.cpp \
    $(DIR)/source/wifi_audio_source.cpp \
    $(DIR)/source/channel_monitor.cpp \
    $(DIR)/source/rtcp_sender.cpp \
    $(DIR)/source/network_sender.cpp \
    $(DIR)/source/source_assembler.cpp \
    $(DIR)/source/playback_session.cpp \
    $(DIR)/sink/audioficast_sink.cpp \
    $(DIR)/sink/audiofi_sink.cpp \
    $(DIR)/sink/wifi_audio_sink.cpp \
    $(DIR)/sink/clock_anchor.cpp \
    $(DIR)/sink/rtcp_receiver.cpp \
    $(DIR)/sink/network_atp_sink.cpp \
    $(DIR)/sink/sink_assembler.cpp \
    $(DIR)/sink/tunnel_renderer.cpp \
    $(DIR)/sink/fiplayer_pll_sync.cpp \
    $(DIR)/sink/fiplayer_renderer.cpp \
    $(DIR)/sink/fiplayer_audio_output.cpp

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
	make -C flac_helper lib
	make -C aac_helper lib
	make -C plc lib
	make -C fec lib
	make -C base lib
	make -C foundation lib
	make -C common lib
	make -C pal lib
	make -C libcoap lib
	$(COPY) $(OBJS) ../xip/

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	make -C flac_helper clean
	make -C aac_helper clean
	make -C plc clean
	make -C fec clean
	make -C base clean
	make -C foundation clean
	make -C common clean
	make -C pal clean
	make -C libcoap clean
	$(call CLEAN_FUNC)

-include $(DEPS)
