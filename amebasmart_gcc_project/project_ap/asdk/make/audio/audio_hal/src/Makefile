include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS               += -I$(FREERTOSDIR)/include
MODULE_IFLAGS               += -I$(FREERTOSDIR)/portable/GCC/ARM_CA7
.PHONY: all lib clean
TARGET_LIB = lib_audio_hal

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/audio_hal/amebasmart

vpath %.c $(shell find $(DIR)/ -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__
GLOBAL_CFLAGS += -DNDEBUG -UDEBUG -Wno-multichar -Werror

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/ameba_audio_stream_buffer.c
CSRC += $(DIR)/ameba_audio_stream.c
CSRC += $(DIR)/ameba_audio_stream_control.c
CSRC += $(DIR)/ameba_audio_stream_utils.c
CSRC += $(DIR)/ameba_audio_stream_render.c
CSRC += $(DIR)/ameba_audio_stream_capture.c
CSRC += $(DIR)/primary_audio_hw_stream_out.c
CSRC += $(DIR)/primary_audio_hw_card.c
CSRC += $(DIR)/primary_audio_hw_stream_in.c
CSRC += $(DIR)/audio_hw_params_handle.c
CSRC += $(DIR)/audio_hw_manager.c
CSRC += $(DIR)/audio_hw_control.c

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
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))

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
