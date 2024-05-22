include $(MAKE_INCLUDE_GEN)

.PHONY: all clean test lib

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/wifi-audio

MODULE_IFLAGS += \
    -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces \
    -I$(BASEDIR)/component/audio/interfaces \
    -I$(BASEDIR)/component/application/wifi-audio/base/log/include \
    -I$(BASEDIR)/component/application/wifi-audio/base/cutils/include \
    -I$(DIR)/interfaces


vpath %.cpp $(shell find $(DIR) -type d)
vpath %.c $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-unused-const-variable -Werror #-std=gnu++14

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = \
    $(DIR)/cmd/audiofi_example.c

CPPSRC  = \
    $(DIR)/cmd/audiofi_cmd.cpp \
    $(DIR)/cmd/audiofi_sink_cmd.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CPPSRC:.cpp=.o))
OBJS += $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

lib: COPY_RAM_OBJS
	$(COPY) $(OBJS) ../xip/

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
