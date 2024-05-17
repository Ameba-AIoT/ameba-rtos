include $(MAKE_INCLUDE_GEN)

.PHONY: test clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/media_test

MODULE_IFLAGS += \
	-I$(BASEDIR)/component/file_system/fatfs \
	-I$(BASEDIR)/component/file_system/fatfs/r0.14b/include \
	-I$(BASEDIR)/component/audio/base/libs/src/include \
	-I$(BASEDIR)/component/audio/base/log/interfaces \
	-I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces \
	-I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces \
	-I$(BASEDIR)/component/audio/base/cutils/interfaces \
	-I$(BASEDIR)/component/audio/interfaces

vpath %.c $(shell find $(DIR) -type d)
vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-incompatible-pointer-types -Wno-int-conversion -Wno-pointer-sign

ifeq ($(CONFIG_MEDIA_PLAYER),y)
    GLOBAL_CFLAGS += -DMEDIA_PLAYER
endif

ifeq ($(CONFIG_MEDIA_LITE_PLAYER),y)
    GLOBAL_CFLAGS += -DMEDIA_LITE_PLAYER
endif

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/app_example.c
CPPSRC =

#CSRC += $(DIR)/threads_test.c
CSRC += $(DIR)/socket_test.c
#CPPSRC = $(DIR)/test_thread.cpp
#CPPSRC += $(DIR)/handler_looper_test.cpp

ifeq ($(CONFIG_MEDIA_PLAYER),y)
CSRC += \
    $(DIR)/stardard/player_test.c \
    $(DIR)/stardard/player_looper_test.c \
    $(DIR)/stardard/mydata_source/mydata_source.c \
    $(DIR)/stardard/mydata_source/mydata_source_test.c \
    $(DIR)/stardard/meidi/mcrc_player_adapter.c \
    $(DIR)/stardard/meidi/mcrc_player_looper_test.c
endif

ifeq ($(CONFIG_MEDIA_LITE_PLAYER),y)
    CSRC += $(DIR)/lite/player_test_lite.c
endif

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
test: COPY_RAM_OBJS
#	cp *.o  ../media_xip

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
