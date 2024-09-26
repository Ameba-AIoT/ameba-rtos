
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean
TARGET_LIB = lib_opus

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/third_party/libopus
MODULE_IFLAGS = -I$(DIR)

vpath %.c $(DIR)/celt $(shell find $(DIR)/celt -type d)
vpath %.c $(DIR)/silk $(shell find $(DIR)/silk -type d)
vpath %.c $(DIR)/silk/fixed $(shell find $(DIR)/silk/fixed -type d)
vpath %.c $(DIR)/src $(shell find $(DIR)/src -type d)


GLOBAL_CFLAGS := $(filter-out -Werror, $(GLOBAL_CFLAGS))
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
MODULE_IFLAGS += -I$(DIR)/celt
MODULE_IFLAGS += -I$(DIR)/silk
MODULE_IFLAGS += -I$(DIR)/silk/fixed
MODULE_IFLAGS += -I$(DIR)/src
MODULE_IFLAGS += -I$(DIR)/include

CSRC := \
    $(DIR)/celt/bands.c \
    $(DIR)/celt/celt.c \
    $(DIR)/celt/celt_encoder.c \
    $(DIR)/celt/celt_decoder.c \
    $(DIR)/celt/cwrs.c \
    $(DIR)/celt/entcode.c \
    $(DIR)/celt/entdec.c \
    $(DIR)/celt/entenc.c \
    $(DIR)/celt/kiss_fft.c \
    $(DIR)/celt/laplace.c \
    $(DIR)/celt/mathops.c \
    $(DIR)/celt/mdct.c \
    $(DIR)/celt/modes.c \
    $(DIR)/celt/pitch.c \
    $(DIR)/celt/celt_lpc.c \
    $(DIR)/celt/quant_bands.c \
    $(DIR)/celt/rate.c \
    $(DIR)/celt/vq.c

CSRC += \
    $(DIR)/silk/CNG.c \
    $(DIR)/silk/code_signs.c \
    $(DIR)/silk/init_decoder.c \
    $(DIR)/silk/decode_core.c \
    $(DIR)/silk/decode_frame.c \
    $(DIR)/silk/decode_parameters.c \
    $(DIR)/silk/decode_indices.c \
    $(DIR)/silk/decode_pulses.c \
    $(DIR)/silk/decoder_set_fs.c \
    $(DIR)/silk/dec_API.c \
    $(DIR)/silk/enc_API.c \
    $(DIR)/silk/encode_indices.c \
    $(DIR)/silk/encode_pulses.c \
    $(DIR)/silk/gain_quant.c \
    $(DIR)/silk/interpolate.c \
    $(DIR)/silk/LP_variable_cutoff.c \
    $(DIR)/silk/NLSF_decode.c \
    $(DIR)/silk/NSQ.c \
    $(DIR)/silk/NSQ_del_dec.c \
    $(DIR)/silk/PLC.c \
    $(DIR)/silk/shell_coder.c \
    $(DIR)/silk/tables_gain.c \
    $(DIR)/silk/tables_LTP.c \
    $(DIR)/silk/tables_NLSF_CB_NB_MB.c \
    $(DIR)/silk/tables_NLSF_CB_WB.c \
    $(DIR)/silk/tables_other.c \
    $(DIR)/silk/tables_pitch_lag.c \
    $(DIR)/silk/tables_pulses_per_block.c \
    $(DIR)/silk/VAD.c \
    $(DIR)/silk/control_audio_bandwidth.c \
    $(DIR)/silk/quant_LTP_gains.c \
    $(DIR)/silk/VQ_WMat_EC.c \
    $(DIR)/silk/HP_variable_cutoff.c \
    $(DIR)/silk/NLSF_encode.c \
    $(DIR)/silk/NLSF_VQ.c \
    $(DIR)/silk/NLSF_unpack.c \
    $(DIR)/silk/NLSF_del_dec_quant.c \
    $(DIR)/silk/process_NLSFs.c \
    $(DIR)/silk/stereo_LR_to_MS.c \
    $(DIR)/silk/stereo_MS_to_LR.c \
    $(DIR)/silk/check_control_input.c \
    $(DIR)/silk/control_SNR.c \
    $(DIR)/silk/init_encoder.c \
    $(DIR)/silk/control_codec.c \
    $(DIR)/silk/A2NLSF.c \
    $(DIR)/silk/ana_filt_bank_1.c \
    $(DIR)/silk/biquad_alt.c \
    $(DIR)/silk/bwexpander_32.c \
    $(DIR)/silk/bwexpander.c \
    $(DIR)/silk/debug.c \
    $(DIR)/silk/decode_pitch.c \
    $(DIR)/silk/inner_prod_aligned.c \
    $(DIR)/silk/lin2log.c \
    $(DIR)/silk/log2lin.c \
    $(DIR)/silk/LPC_analysis_filter.c \
    $(DIR)/silk/LPC_fit.c \
    $(DIR)/silk/LPC_inv_pred_gain.c \
    $(DIR)/silk/table_LSF_cos.c \
    $(DIR)/silk/NLSF2A.c \
    $(DIR)/silk/NLSF_stabilize.c \
    $(DIR)/silk/NLSF_VQ_weights_laroia.c \
    $(DIR)/silk/pitch_est_tables.c \
    $(DIR)/silk/resampler.c \
    $(DIR)/silk/resampler_down2_3.c \
    $(DIR)/silk/resampler_down2.c \
    $(DIR)/silk/resampler_private_AR2.c \
    $(DIR)/silk/resampler_private_down_FIR.c \
    $(DIR)/silk/resampler_private_IIR_FIR.c \
    $(DIR)/silk/resampler_private_up2_HQ.c \
    $(DIR)/silk/resampler_rom.c \
    $(DIR)/silk/sigm_Q15.c \
    $(DIR)/silk/sort.c \
    $(DIR)/silk/sum_sqr_shift.c \
    $(DIR)/silk/stereo_decode_pred.c \
    $(DIR)/silk/stereo_encode_pred.c \
    $(DIR)/silk/stereo_find_predictor.c \
    $(DIR)/silk/stereo_quant_pred.c

CSRC += \
    $(DIR)/silk/fixed/LTP_analysis_filter_FIX.c \
    $(DIR)/silk/fixed/LTP_scale_ctrl_FIX.c \
    $(DIR)/silk/fixed/corrMatrix_FIX.c \
    $(DIR)/silk/fixed/encode_frame_FIX.c \
    $(DIR)/silk/fixed/find_LPC_FIX.c \
    $(DIR)/silk/fixed/find_LTP_FIX.c \
    $(DIR)/silk/fixed/find_pitch_lags_FIX.c \
    $(DIR)/silk/fixed/find_pred_coefs_FIX.c \
    $(DIR)/silk/fixed/noise_shape_analysis_FIX.c \
    $(DIR)/silk/fixed/process_gains_FIX.c \
    $(DIR)/silk/fixed/regularize_correlations_FIX.c \
    $(DIR)/silk/fixed/residual_energy16_FIX.c \
    $(DIR)/silk/fixed/residual_energy_FIX.c \
    $(DIR)/silk/fixed/warped_autocorrelation_FIX.c \
    $(DIR)/silk/fixed/apply_sine_window_FIX.c \
    $(DIR)/silk/fixed/autocorr_FIX.c \
    $(DIR)/silk/fixed/burg_modified_FIX.c \
    $(DIR)/silk/fixed/k2a_FIX.c \
    $(DIR)/silk/fixed/k2a_Q16_FIX.c \
    $(DIR)/silk/fixed/pitch_analysis_core_FIX.c \
    $(DIR)/silk/fixed/vector_ops_FIX.c \
    $(DIR)/silk/fixed/schur64_FIX.c \
    $(DIR)/silk/fixed/schur_FIX.c

CSRC += \
    $(DIR)/src/mapping_matrix.c \
    $(DIR)/src/opus.c \
    $(DIR)/src/opus_decoder.c \
    $(DIR)/src/opus_encoder.c \
    $(DIR)/src/opus_multistream.c \
    $(DIR)/src/opus_multistream_encoder.c \
    $(DIR)/src/opus_multistream_decoder.c \
    $(DIR)/src/opus_projection_encoder.c \
    $(DIR)/src/opus_projection_decoder.c \
    $(DIR)/src/repacketizer.c

CSRC += \
    $(DIR)/src/analysis.c \
    $(DIR)/src/mlp.c \
    $(DIR)/src/mlp_data.c

#CSRC += \
    $(DIR)/celt/arm/armcpu.c \
    $(DIR)/celt/arm/arm_celt_map.c \

#CSRC += \
    $(DIR)/celt/arm/celt_neon_intr.c \
    $(DIR)/celt/arm/pitch_neon_intr.c \
    $(DIR)/silk/arm/arm_silk_map.c \
    $(DIR)/silk/arm/biquad_alt_neon_intr.c \
    $(DIR)/silk/arm/LPC_inv_pred_gain_neon_intr.c \
    $(DIR)/silk/arm/NSQ_del_dec_neon_intr.c \
    $(DIR)/silk/arm/NSQ_neon.c \
    $(DIR)/silk/fixed/arm/warped_autocorrelation_FIX_neon_intr.c

#ASMSRC += \
    $(DIR)/celt/arm/celt_pitch_xcorr_arm_gnu.s

#GLOBAL_CFLAGS += \
    -D__linux__ \
    -DOPUS_ARM_ASM \
    -DOPUS_ARM_INLINE_ASM \
    -DOPUS_ARM_MAY_HAVE_EDSP \
    -DOPUS_ARM_INLINE_EDSP \
    -DOPUS_ARM_MAY_HAVE_MEDIA \
    -DOPUS_ARM_INLINE_MEDIA \
    -DOPUS_ARM_MAY_HAVE_NEON \
    -DOPUS_HAVE_RTCD

#GLOBAL_CFLAGS += \
    -DOPUS_ARM_MAY_HAVE_NEON \
    -DOPUS_ARM_MAY_HAVE_NEON_INTR \
    -DOPUS_ARM_PRESUME_NEON \
    -DOPUS_ARM_INLINE_NEON

GLOBAL_CFLAGS += \
    -DNULL=0 \
    -DSOCKLEN_T=socklen_t \
    -DLOCALE_NOT_USED \
    -D_LARGEFILE_SOURCE=1 \
    -D_FILE_OFFSET_BITS=64 \
    -Drestrict= \
    -D__EMX__ \
    -DOPUS_BUILD \
    -DFIXED_POINT \
    -DUSE_ALLOCA \
    -DSIMD_EXTRA_ALLOC_BYTES=16 \
    -DHAVE_LRINT \
    -DHAVE_LRINTF \
    -DENABLE_HARDENING \
    -O2 \
    -fno-math-errno
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
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
