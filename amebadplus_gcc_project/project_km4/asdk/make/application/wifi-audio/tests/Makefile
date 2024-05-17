include $(MAKE_INCLUDE_GEN)

.PHONY: test clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/wifi-audio/tests

MODULE_IFLAGS += \
    -I$(BASEDIR)/component/audio/interfaces \
    -I$(BASEDIR)/component/application/wifi-audio/base/log/include \
    -I$(BASEDIR)/component/application/wifi-audio/base/cutils/include \
    -I$(BASEDIR)/component/application/wifi-audio/base/utils/include \
    -I$(BASEDIR)/component/application/wifi-audio/foundation/include \
    -I$(BASEDIR)/component/application/wifi-audio/common \
    -I$(BASEDIR)/component/application/wifi-audio/include

vpath %.c $(shell find $(DIR) -type d)
vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-incompatible-pointer-types -Wno-int-conversion -Wno-pointer-sign

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = \
	$(DIR)/app_example.c \
	$(DIR)/wfa_timer_test.c

CPPSRC = \
	$(DIR)/thread_test.cpp \
	$(DIR)/rlooper_test.cpp \
	$(DIR)/rmessage_test.cpp \
	$(DIR)/playback_source_test.cpp \
	$(DIR)/wifi_audio_imitate_test.cpp \
	$(DIR)/tunnel_renderer_test.cpp \
	$(DIR)/playback_session_test.cpp \
	$(DIR)/rtcodec_test.cpp \
	$(DIR)/flac_enc_test.cpp \
	$(DIR)/flac_dec_test.cpp \
	$(DIR)/apacket_source_test.cpp \
	$(DIR)/apacket_ringbuffer_test.cpp \
	$(DIR)/fiplayer_render_test.cpp

#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
OBJS += $(notdir $(CPPSRC:.cpp=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
test: COPY_RAM_OBJS

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
