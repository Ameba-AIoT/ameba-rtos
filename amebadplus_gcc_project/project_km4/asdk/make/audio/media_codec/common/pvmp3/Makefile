include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/media_codec/common/pvmp3

MODULE_IFLAGS += \
	-I$(DIR)/include \
	-I$(DIR)/src

vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Werror

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CPPSRC := \
	$(DIR)/src/pvmp3_normalize.cpp \
	$(DIR)/src/pvmp3_alias_reduction.cpp \
	$(DIR)/src/pvmp3_crc.cpp \
	$(DIR)/src/pvmp3_decode_header.cpp \
	$(DIR)/src/pvmp3_decode_huff_cw.cpp \
	$(DIR)/src/pvmp3_getbits.cpp \
	$(DIR)/src/pvmp3_dequantize_sample.cpp \
	$(DIR)/src/pvmp3_framedecoder.cpp \
	$(DIR)/src/pvmp3_get_main_data_size.cpp \
	$(DIR)/src/pvmp3_get_side_info.cpp \
	$(DIR)/src/pvmp3_get_scale_factors.cpp \
	$(DIR)/src/pvmp3_mpeg2_get_scale_data.cpp \
	$(DIR)/src/pvmp3_mpeg2_get_scale_factors.cpp \
	$(DIR)/src/pvmp3_mpeg2_stereo_proc.cpp \
	$(DIR)/src/pvmp3_huffman_decoding.cpp \
	$(DIR)/src/pvmp3_huffman_parsing.cpp \
	$(DIR)/src/pvmp3_tables.cpp \
	$(DIR)/src/pvmp3_imdct_synth.cpp \
	$(DIR)/src/pvmp3_mdct_6.cpp \
	$(DIR)/src/pvmp3_dct_6.cpp \
	$(DIR)/src/pvmp3_poly_phase_synthesis.cpp \
	$(DIR)/src/pvmp3_equalizer.cpp \
	$(DIR)/src/pvmp3_seek_synch.cpp \
	$(DIR)/src/pvmp3_stereo_proc.cpp \
	$(DIR)/src/pvmp3_reorder.cpp \
	$(DIR)/src/pvmp3_polyphase_filter_window.cpp \
	$(DIR)/src/pvmp3_mdct_18.cpp \
	$(DIR)/src/pvmp3_dct_9.cpp \
	$(DIR)/src/pvmp3_dct_16.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CPPSRC:.cpp=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	cp *.o  ../../../media_xip

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
