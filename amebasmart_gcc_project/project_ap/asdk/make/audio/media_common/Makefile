include $(MAKE_INCLUDE_GEN)

.PHONY: lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/media_common
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/libs/src/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/media_utils/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(DIR)/include

vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-deprecated-copy -Werror

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CPPSRC  = $(DIR)/media_track.cpp
CPPSRC += $(DIR)/media_buffer_base.cpp
CPPSRC += $(DIR)/media_buffer.cpp
CPPSRC += $(DIR)/media_buffer_group.cpp
CPPSRC += $(DIR)/codec_buffer_info.cpp
CPPSRC += $(DIR)/media_codec_buffer.cpp
CPPSRC += $(DIR)/media_defs.cpp
CPPSRC += $(DIR)/meta_data_base.cpp
CPPSRC += $(DIR)/meta_data.cpp
CPPSRC += $(DIR)/packet_pool.cpp
CPPSRC += $(DIR)/esds.cpp
CPPSRC += $(DIR)/utils.cpp
CPPSRC += $(DIR)/media_base64.cpp
CPPSRC += $(DIR)/media_bit_reader.cpp
CPPSRC += $(DIR)/clock_anchor.cpp

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
	cp *.o  ../media_xip

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
