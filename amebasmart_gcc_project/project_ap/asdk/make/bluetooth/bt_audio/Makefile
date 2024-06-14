include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
BT_DIR = $(BASEDIR)/component/bluetooth
BT_AUDIO_DIR = $(BT_DIR)/bt_audio
AUDIO_HAL_DIR = $(BASEDIR)/component/audio
BT_CODEC_SBC = $(BT_AUDIO_DIR)/bt_codec/sbc
BT_CODEC_LC3 = $(BT_AUDIO_DIR)/bt_codec/lc3
#BT_CODEC_AAC = $(BT_AUDIO_DIR)/bt_codec/aac

vpath %.c $(BT_AUDIO_DIR) \
		$(BT_AUDIO_DIR)/bt_codec

MODULE_IFLAGS += \
	-I$(BT_DIR)/rtk_stack/example/ble_audio \
	-I$(BT_DIR)/rtk_stack/inc/bluetooth/leaudio \
	-I$(BT_AUDIO_DIR)/include \
	-I$(BT_AUDIO_DIR)/bt_codec \
	-I$(BT_AUDIO_DIR)/audio_sync \
	-I$(AUDIO_HAL_DIR)/interfaces \
	-I$(AUDIO_HAL_DIR)/base/osal/osal_c/interfaces \
	-I$(BT_CODEC_SBC)/decoder/include \
	-I$(BT_CODEC_SBC)/encoder/include \
	-I$(BT_CODEC_LC3)/include \
	-I$(BT_CODEC_LC3)/src
	#-I$(BT_CODEC_AAC)/libAACdec/include \
	#-I$(BT_CODEC_AAC)/libAACenc/include \
	#-I$(BT_CODEC_AAC)/libPCMutils/include \
	#-I$(BT_CODEC_AAC)/libFDK/include \
	#-I$(BT_CODEC_AAC)/libSYS/include \
	#-I$(BT_CODEC_AAC)/libMpegTPDec/include \
	#-I$(BT_CODEC_AAC)/libMpegTPEnc/include \
	#-I$(BT_CODEC_AAC)/libSBRdec/include \
	#-I$(BT_CODEC_AAC)/libSBRenc/include \
	#-I$(BT_CODEC_AAC)/libArithCoding/include \
	#-I$(BT_CODEC_AAC)/libDRCdec/include \
	#-I$(BT_CODEC_AAC)/libSACdec/include \
	#-I$(BT_CODEC_AAC)/libSACenc/include \

CSRC += \
	$(BT_AUDIO_DIR)/bt_audio_intf.c \
	$(BT_AUDIO_DIR)/bt_audio_resample.c \
	$(BT_AUDIO_DIR)/bt_audio_track_api.c \
	$(BT_AUDIO_DIR)/bt_audio_record_api.c \
	$(BT_AUDIO_DIR)/bt_audio_codec_wrapper.c \
	$(BT_AUDIO_DIR)/bt_codec/sbc_codec_entity.c \
	$(BT_AUDIO_DIR)/bt_codec/lc3_codec_entity.c \
	$(BT_AUDIO_DIR)/bt_codec/cvsd_codec_entity.c

OBJS += $(notdir $(CSRC:.c=.o))

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
