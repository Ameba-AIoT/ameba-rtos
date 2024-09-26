
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean
TARGET_LIB = lib_speexdsp

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/third_party/speexdsp

vpath %.c $(DIR) $(shell find $(DIR) -type d)
#*****************************************************************************#
#                          Header FILE Directory LIST                         #
#*****************************************************************************#
MODULE_IFLAGS = -I$(DIR)/libspeexdsp/
MODULE_IFLAGS = -I$(DIR)/include/

GLOBAL_CFLAGS := $(filter-out -Werror, $(GLOBAL_CFLAGS))
GLOBAL_CFLAGS += -DHAVE_CONFIG_H
GLOBAL_CFLAGS += -DFIXED_POINT

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += $(DIR)/libspeexdsp/buffer.c
CSRC += $(DIR)/libspeexdsp/fftwrap.c
CSRC += $(DIR)/libspeexdsp/filterbank.c
CSRC += $(DIR)/libspeexdsp/jitter.c
CSRC += $(DIR)/libspeexdsp/kiss_fft.c
CSRC += $(DIR)/libspeexdsp/kiss_fftr.c
CSRC += $(DIR)/libspeexdsp/mdf.c
CSRC += $(DIR)/libspeexdsp/preprocess.c
CSRC += $(DIR)/libspeexdsp/resample.c
CSRC += $(DIR)/libspeexdsp/scal.c
CSRC += $(DIR)/libspeexdsp/smallft.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
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
