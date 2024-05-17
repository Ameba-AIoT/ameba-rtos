include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
BT_DIR = $(BASEDIR)/component/bluetooth
BT_APP_AUDIO_DATA = $(BT_DIR)/example/bt_app_audio_data
BT_AUDIO_DIR = $(BT_DIR)/bt_audio
HFP_DIR = $(BT_DIR)/example/bt_hfp

vpath %.c $(HFP_DIR)

MODULE_IFLAGS += \
	-I$(HFP_DIR) \
	-I$(BT_APP_AUDIO_DATA) \
	-I$(BT_AUDIO_DIR)/include \
	-I$(BT_AUDIO_DIR)/bt_codec

CSRC += \
	$(HFP_DIR)/hfp.c

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
