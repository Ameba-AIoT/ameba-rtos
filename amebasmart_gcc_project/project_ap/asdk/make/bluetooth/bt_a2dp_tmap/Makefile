include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
BT_DIR = $(BASEDIR)/component/bluetooth
AUDIO_HAL_DIR = $(BASEDIR)/component/audio
BT_AUDIO_DIR = $(BT_DIR)/bt_audio
BT_CODEC_SBC = $(BT_AUDIO_DIR)/bt_codec/sbc
A2DP_TMAP_DIR= $(BT_DIR)/example/bt_a2dp_tmap
BLE_AUDIO_COMMON_DIR = $(BT_DIR)/example/ble_audio_common
BT_APP_AUDIO_DATA = $(BT_DIR)/example/bt_app_audio_data

vpath %.c $(A2DP_TMAP_DIR) \
		$(BLE_AUDIO_COMMON_DIR)

MODULE_IFLAGS += \
	-I$(BT_APP_AUDIO_DATA) \
	-I$(BT_AUDIO_DIR)/include \
	-I$(BT_AUDIO_DIR)/bt_codec \
	-I$(BT_CODEC_SBC)/decoder/include \
	-I$(BT_CODEC_SBC)/encoder/include \
	-I$(BLE_AUDIO_COMMON_DIR)

CSRC += \
	$(A2DP_TMAP_DIR)/a2dp_tmap.c \
	$(A2DP_TMAP_DIR)/tmap_a2dp_source.c

OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                          RULES TO GENERATE TARGETS                          #
#*****************************************************************************#
# LIBBLUETOOTH_OBJS_PATH = $(shell pwd)/objs
# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
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
	$(call CLEAN_FUNC)

-include $(DEPS)
