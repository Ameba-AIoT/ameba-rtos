include $(MAKE_INCLUDE_GEN)

.PHONY: lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/wifi-audio/common

MODULE_IFLAGS += \
    -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces \
    -I$(BASEDIR)/component/audio/interfaces \
    -I$(BASEDIR)/component/wifi/wifi_cast \
    -I$(BASEDIR)/component/application/wifi-audio/base/log/include \
    -I$(BASEDIR)/component/application/wifi-audio/base/cutils/include \
    -I$(BASEDIR)/component/application/wifi-audio/base/utils/include \
    -I$(BASEDIR)/component/application/wifi-audio/foundation/include \
    -I$(BASEDIR)/component/application/wifi-audio/fec \
    -I$(BASEDIR)/component/application/wifi-audio/fec/fec \
    -I$(BASEDIR)/component/application/wifi-audio/interfaces

vpath %.c $(shell find $(DIR) -type d)
vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Werror
CPPFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-narrowing -Werror -std=c++14 -std=gnu++14

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CPPSRC  = \
    $(DIR)/apacket_source.cpp \
    $(DIR)/aringbuffer_packet_source.cpp \
    $(DIR)/network_session.cpp \
    $(DIR)/udp_session.cpp \
    $(DIR)/wifi_cast_session.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
OBJS += $(notdir $(CPPSRC:.cpp=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
lib: COPY_RAM_OBJS
	cp *.o  ../../xip

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
