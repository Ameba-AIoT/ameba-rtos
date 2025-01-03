set(DIR ${BASEDIR}/component/audio/media_codec/common/amrwb)
set(
    MODULE_IFLAGS
    ${DIR}/include
    ${DIR}/src
)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-error -Wno-multichar -Wno-unused-parameter)

set(
    CPPSRC
    ${DIR}/src/agc2_amr_wb.cpp
    ${DIR}/src/band_pass_6k_7k.cpp
    ${DIR}/src/dec_acelp_2p_in_64.cpp
    ${DIR}/src/dec_acelp_4p_in_64.cpp
    ${DIR}/src/dec_alg_codebook.cpp
    ${DIR}/src/dec_gain2_amr_wb.cpp
    ${DIR}/src/deemphasis_32.cpp
    ${DIR}/src/dtx_decoder_amr_wb.cpp
    ${DIR}/src/get_amr_wb_bits.cpp
    ${DIR}/src/highpass_400hz_at_12k8.cpp
    ${DIR}/src/highpass_50hz_at_12k8.cpp
    ${DIR}/src/homing_amr_wb_dec.cpp
    ${DIR}/src/interpolate_isp.cpp
    ${DIR}/src/isf_extrapolation.cpp
    ${DIR}/src/isp_az.cpp
    ${DIR}/src/isp_isf.cpp
    ${DIR}/src/lagconceal.cpp
    ${DIR}/src/low_pass_filt_7k.cpp
    ${DIR}/src/median5.cpp
    ${DIR}/src/mime_io.cpp
    ${DIR}/src/noise_gen_amrwb.cpp
    ${DIR}/src/normalize_amr_wb.cpp
    ${DIR}/src/oversamp_12k8_to_16k.cpp
    ${DIR}/src/phase_dispersion.cpp
    ${DIR}/src/pit_shrp.cpp
    ${DIR}/src/pred_lt4.cpp
    ${DIR}/src/preemph_amrwb_dec.cpp
    ${DIR}/src/pvamrwb_math_op.cpp
    ${DIR}/src/pvamrwbdecoder.cpp
    ${DIR}/src/q_gain2_tab.cpp
    ${DIR}/src/qisf_ns.cpp
    ${DIR}/src/qisf_ns_tab.cpp
    ${DIR}/src/qpisf_2s.cpp
    ${DIR}/src/qpisf_2s_tab.cpp
    ${DIR}/src/scale_signal.cpp
    ${DIR}/src/synthesis_amr_wb.cpp
    ${DIR}/src/voice_factor.cpp
    ${DIR}/src/wb_syn_filt.cpp
    ${DIR}/src/weight_amrwb_lpc.cpp
)

set_source_files_properties(${CPPSRC} DIRECTORY ../../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY ../../.. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY ../../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
