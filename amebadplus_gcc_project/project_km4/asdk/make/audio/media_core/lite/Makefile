include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/media_core/lite

MODULE_IFLAGS += \
    -I$(BASEDIR)/component/audio/base/libs/src/include \
    -I$(BASEDIR)/component/audio/base/log/interfaces \
    -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces \
    -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces \
    -I$(BASEDIR)/component/audio/base/cutils/interfaces \
    -I$(BASEDIR)/component/audio/base/audio_utils/include \
    -I$(BASEDIR)/component/audio/interfaces \
    -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces/osal_c \
    -I$(BASEDIR)/component/audio/media_utils/include \
    -I$(BASEDIR)/component/audio/media_common/include \
    -I$(BASEDIR)/component/audio/media_source/include \
    -I$(BASEDIR)/component/audio/media_demux/include \
    -I$(BASEDIR)/component/audio/media_codec/codec-simple/include \
    -I$(DIR)/include


vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Werror #-std=gnu++14

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CPPSRC  = \
    $(DIR)/player_driver_lite.cpp \
    $(DIR)/player_core_lite.cpp \
    $(DIR)/player_core_decoder_lite.cpp \
    $(DIR)/player_core_renderer_lite.cpp \
    $(DIR)/generic_source_lite.cpp \
    $(DIR)/audio_output_lite.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CPPSRC:.cpp=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	cp *.o  ../../media_xip

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
