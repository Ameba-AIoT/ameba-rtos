include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
BT_DIR = $(BASEDIR)/component/bluetooth
BLE_AUDIO_COMMON_DIR = $(BT_DIR)/example/ble_audio_common
BT_APP_AUDIO_DATA = $(BT_DIR)/example/bt_app_audio_data

vpath %.c $(BT_DIR)/example/ble_audio

MODULE_IFLAGS += \
	-I$(BLE_AUDIO_COMMON_DIR) \
	-I$(BT_APP_AUDIO_DATA)

CSRC += \
	$(BT_DIR)/example/ble_audio/bap.c \
	$(BT_DIR)/example/ble_audio/cap.c \
	$(BT_DIR)/example/ble_audio/pbp.c \
	$(BT_DIR)/example/ble_audio/tmap.c \
	$(BT_DIR)/example/ble_audio/gmap.c

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
