include $(MAKE_INCLUDE_GEN)

.PHONY: all clean
TARGET_LIB = lib_btaudiosync

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
BT_DIR = $(BASEDIR)/component/bluetooth
BT_AUDIO_DIR = $(BT_DIR)/bt_audio
AUDIO_HAL_DIR = $(BASEDIR)/component/audio

vpath %.c $(BT_AUDIO_DIR)/audio_sync

#vpath %.cpp $(BT_CODEC_AAC) $(shell find $(BT_CODEC_AAC) -type d)

MODULE_IFLAGS += \
	-I$(BT_AUDIO_DIR)/include \
	-I$(BT_AUDIO_DIR)/audio_sync \
	-I$(AUDIO_HAL_DIR)/interfaces \
	-I$(AUDIO_HAL_DIR)/base/osal/osal_c/interfaces \

#SBC CODEC
CSRC += \
	$(BT_AUDIO_DIR)/audio_sync/bt_audio_sync.c

OBJS += $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                          RULES TO GENERATE TARGETS                          #
#*****************************************************************************#
# LIBBLUETOOTH_OBJS_PATH = $(shell pwd)/objs
# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
	# mkdir -p $(LIBBLUETOOTH_OBJS_PATH)
	# for i in $(OBJS); do \
	# 	 $(COPY)  $$i $(LIBBLUETOOTH_OBJS_PATH) -f;\
	# 	done
	# $(REMOVE) *.i
	# $(REMOVE) *.s
	# $(REMOVE) *.d

#*****************************************************************************#
#                            GENERATE OBJECT FILE                             #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                           RULES TO CLEAN TARGETS                            #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d
	$(REMOVE) *.ii

-include $(DEPS)
