include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ipc/ipc/amebasmart/include
MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ipc/ipc/amebasmart/include/include
MODULE_IFLAGS += -I$(BASEDIR)/component/utils/ipc/ipc/amebasmart/include/src

MODULE_IFLAGS += -I$(BASEDIR)/component/audio/frameworks/media/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/libs/src/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/utils/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/cutils/interfaces

MODULE_IFLAGS += -I$(BASEDIR)/component/audio/third_party/libopus/include

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/clintwood/audio/msft_client
vpath %.c $(DIR)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -DRPC_SERVER

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/app_example.c \
       $(DIR)/audio_opus_encode_decode.c \
       $(DIR)/audio_record_play.c \
       $(DIR)/example_opus_rpc.c \
       $(DIR)/opus_rpc.c

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
COPY_RAM_OBJS: CORE_TARGETS
all: clean COPY_RAM_OBJS

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
