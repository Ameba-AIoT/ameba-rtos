include $(MAKE_INCLUDE_GEN)
include ../libs/config.mk

.PHONY: all lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/wifi-audio/pal/amebadplus/audio_hal

vpath %.c $(shell find $(DIR)/ -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__
GLOBAL_CFLAGS += -DNDEBUG -UDEBUG -Wno-multichar -Werror

ifeq ($(CONFIG_WIFI_AUDIO),y)
GLOBAL_CFLAGS += -DCONFIG_WL_AUDIO
endif

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
MODULE_IFLAGS += -I$(DIR)
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/audio_hal/amebadplus
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaDplus_KM4/non_secure
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaDplus_KM4/secure

CSRC = $(DIR)/ameba_wfa_audio_stream_utils.c
CSRC += $(DIR)/ameba_wfa_audio_stream_render.c
CSRC += $(DIR)/ameba_wfa_audio_stream_capture.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Dependency	                                  #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS

all: COPY_RAM_OBJS

lib: COPY_RAM_OBJS
	$(COPY) $(OBJS) ../audio_hal_xip/
#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
