include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/audio_utils/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/libs/src/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_algorithm/sonic
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_algorithm/plc
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/interfaces
.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
AUDIO_TEST_DIR = $(BASEDIR)/component/audio/audio_test

vpath %.cpp $(shell find $(AUDIO_TEST_DIR) -type d)
vpath %.c $(shell find $(AUDIO_TEST_DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
 GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-narrowing
 CPPFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-narrowing

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(AUDIO_TEST_DIR)/audio_test.c
CSRC += $(AUDIO_TEST_DIR)/audio_buffer.c
CSRC += $(AUDIO_TEST_DIR)/audio_plc_test.c
CSRC += $(AUDIO_TEST_DIR)/audio_track_test.c
CSRC += $(AUDIO_TEST_DIR)/audio_record_test.c
CSRC += $(AUDIO_TEST_DIR)/audio_control_test.c
CSRC += $(AUDIO_TEST_DIR)/audio_test_utils.c
CPPSRC += $(AUDIO_TEST_DIR)/audio_wav_test.cpp
CSRC += $(AUDIO_TEST_DIR)/audio_wav_test_thread.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
CPPOBJS = $(notdir $(CPPSRC:.cpp=.o))
COBJS = $(notdir $(CSRC:.c=.o))

OBJS = $(CPPOBJS)
OBJS += $(COBJS)

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

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.cpp
	$(Q)$(CC) $(CPPFLAGS) $(MODULE_IFLAGS) $<  -o $@
%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
