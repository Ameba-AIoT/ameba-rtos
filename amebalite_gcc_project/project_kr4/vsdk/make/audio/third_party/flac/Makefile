
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean
TARGET_LIB = lib_flac

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/third_party/flac

MODULE_IFLAGS += \
    -I$(DIR) \
    -I$(DIR)/include \
    -I$(DIR)/src/libFLAC/include

vpath %.c $(shell find $(DIR)/ -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS := $(filter-out -Werror, $(GLOBAL_CFLAGS))
GLOBAL_CFLAGS += \
    -D__RTOS__ \
    -DHAVE_CONFIG_H \
    -DFLAC__INTEGER_ONLY_LIBRARY \
    -DFLAC__HAS_OGG=0

# flags for build warnings
GLOBAL_CFLAGS += \
    -Wno-undef \
    -Wno-format \
    -Wno-implicit-fallthrough \
    -Wno-discarded-qualifiers \
    -Wno-incompatible-pointer-types

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC  = \
    $(DIR)/src/libFLAC/bitmath.c \
    $(DIR)/src/libFLAC/bitreader.c \
    $(DIR)/src/libFLAC/bitwriter.c \
    $(DIR)/src/libFLAC/cpu.c \
    $(DIR)/src/libFLAC/crc.c \
    $(DIR)/src/libFLAC/fixed.c \
    $(DIR)/src/libFLAC/float.c \
    $(DIR)/src/libFLAC/format.c \
    $(DIR)/src/libFLAC/lpc.c \
    $(DIR)/src/libFLAC/memory.c \
    $(DIR)/src/libFLAC/md5.c \
    $(DIR)/src/libFLAC/stream_decoder.c \
    $(DIR)/src/libFLAC/stream_encoder.c \
    $(DIR)/src/libFLAC/stream_encoder_framing.c \
    $(DIR)/src/libFLAC/window.c \
    $(DIR)/src/libFLAC/metadata_object.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


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
#              GENERATE GIT VERSION FILE                                      #
#*****************************************************************************#

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
