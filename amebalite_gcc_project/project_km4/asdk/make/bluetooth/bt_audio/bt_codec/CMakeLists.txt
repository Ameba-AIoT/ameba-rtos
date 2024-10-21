set(BT_DIR ${BASEDIR}/component/bluetooth)
set(BT_AUDIO_DIR ${BT_DIR}/bt_audio)
set(AUDIO_HAL_DIR ${BASEDIR}/component/audio)
set(BT_CODEC_SBC ${BT_AUDIO_DIR}/bt_codec/sbc)
set(BT_CODEC_LC3 ${BT_AUDIO_DIR}/bt_codec/lc3)
#set(BT_CODEC_AAC ${BT_AUDIO_DIR}/bt_codec/aac)


set(
    MODULE_IFLAGS
	${BT_AUDIO_DIR}/include
	${BT_AUDIO_DIR}/bt_codec
	${AUDIO_HAL_DIR}/interfaces
	${AUDIO_HAL_DIR}/base/osal/osal_c/interfaces
	${BT_CODEC_SBC}/decoder/include
	${BT_CODEC_SBC}/encoder/include
	${BT_CODEC_LC3}/include
	${BT_CODEC_LC3}/src
	#${BT_CODEC_AAC}/libAACdec/include
	#${BT_CODEC_AAC}/libAACenc/include
	#${BT_CODEC_AAC}/libPCMutils/include
	#${BT_CODEC_AAC}/libFDK/include
	#${BT_CODEC_AAC}/libSYS/include
	#${BT_CODEC_AAC}/libMpegTPDec/include
	#${BT_CODEC_AAC}/libMpegTPEnc/include
	#${BT_CODEC_AAC}/libSBRdec/include
	#${BT_CODEC_AAC}/libSBRenc/include
	#${BT_CODEC_AAC}/libArithCoding/include
	#${BT_CODEC_AAC}/libDRCdec/include
	#${BT_CODEC_AAC}/libSACdec/include
	#${BT_CODEC_AAC}/libSACenc/include
)

#SBC CODEC
set(
    CSRC
	${BT_CODEC_SBC}/decoder/srce/alloc.c
	${BT_CODEC_SBC}/decoder/srce/bitalloc.c
	${BT_CODEC_SBC}/decoder/srce/bitalloc-sbc.c
	${BT_CODEC_SBC}/decoder/srce/bitstream-decode.c
	${BT_CODEC_SBC}/decoder/srce/decoder-oina.c
	${BT_CODEC_SBC}/decoder/srce/decoder-private.c
	${BT_CODEC_SBC}/decoder/srce/decoder-sbc.c
	${BT_CODEC_SBC}/decoder/srce/dequant.c
	${BT_CODEC_SBC}/decoder/srce/framing.c
	${BT_CODEC_SBC}/decoder/srce/framing-sbc.c
	${BT_CODEC_SBC}/decoder/srce/oi_codec_version.c
	${BT_CODEC_SBC}/decoder/srce/synthesis-8-generated.c
	${BT_CODEC_SBC}/decoder/srce/synthesis-dct8.c
	${BT_CODEC_SBC}/decoder/srce/synthesis-sbc.c
	${BT_CODEC_SBC}/encoder/srce/sbc_analysis.c
	${BT_CODEC_SBC}/encoder/srce/sbc_dct.c
	${BT_CODEC_SBC}/encoder/srce/sbc_dct_coeffs.c
	${BT_CODEC_SBC}/encoder/srce/sbc_enc_bit_alloc_mono.c
	${BT_CODEC_SBC}/encoder/srce/sbc_enc_bit_alloc_ste.c
	${BT_CODEC_SBC}/encoder/srce/sbc_enc_coeffs.c
	${BT_CODEC_SBC}/encoder/srce/sbc_encoder.c
	${BT_CODEC_SBC}/encoder/srce/sbc_packing.c
	${BT_CODEC_LC3}/src/attdet.c
	${BT_CODEC_LC3}/src/bits.c
	${BT_CODEC_LC3}/src/bwdet.c
	${BT_CODEC_LC3}/src/energy.c
	${BT_CODEC_LC3}/src/lc3.c
	${BT_CODEC_LC3}/src/ltpf.c
	${BT_CODEC_LC3}/src/mdct.c
	${BT_CODEC_LC3}/src/plc.c
	${BT_CODEC_LC3}/src/sns.c
	${BT_CODEC_LC3}/src/spec.c
	${BT_CODEC_LC3}/src/tables.c
	${BT_CODEC_LC3}/src/tns.c
)

#AAC CODEC
# set(
#     CPPSRC
# 	${BT_CODEC_AAC}/libAACdec/src/aacdec_drc.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/aacdec_hcr_bit.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/aacdec_hcr.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/aacdec_hcrs.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/aacdecoder.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/aacdecoder_lib.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/aacdec_pns.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/aacdec_tns.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/aac_ram.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/aac_rom.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/block.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/channel.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/channelinfo.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/conceal.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/FDK_delay.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/ldfiltbank.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/pulsedata.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/rvlcbit.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/rvlcconceal.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/rvlc.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/stereo.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/usacdec_ace_d4t64.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/usacdec_acelp.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/usacdec_ace_ltp.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/usacdec_fac.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/usacdec_lpc.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/usacdec_lpd.cpp
# 	${BT_CODEC_AAC}/libAACdec/src/usacdec_rom.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/aacenc.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/aacenc_lib.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/aacenc_pns.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/aacEnc_ram.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/aacEnc_rom.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/aacenc_tns.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/adj_thr.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/band_nrg.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/bandwidth.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/bit_cnt.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/bitenc.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/block_switch.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/channel_map.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/chaosmeasure.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/dyn_bits.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/grp_data.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/intensity.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/line_pe.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/metadata_compressor.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/metadata_main.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/mps_main.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/ms_stereo.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/noisedet.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/pnsparam.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/pre_echo_control.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/psy_configuration.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/psy_main.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/qc_main.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/quantize.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/sf_estim.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/spreading.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/tonality.cpp
# 	${BT_CODEC_AAC}/libAACenc/src/transform.cpp
# 	${BT_CODEC_AAC}/libPCMutils/src/limiter.cpp
# 	${BT_CODEC_AAC}/libPCMutils/src/pcmdmx_lib.cpp
# 	${BT_CODEC_AAC}/libPCMutils/src/pcm_utils.cpp
# 	${BT_CODEC_AAC}/libFDK/src/autocorr2nd.cpp
# 	${BT_CODEC_AAC}/libFDK/src/dct.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_bitbuffer.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_core.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_crc.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_decorrelate.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_hybrid.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_lpc.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_matrixCalloc.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_qmf_domain.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_tools_rom.cpp
# 	${BT_CODEC_AAC}/libFDK/src/FDK_trigFcts.cpp
# 	${BT_CODEC_AAC}/libFDK/src/fft.cpp
# 	${BT_CODEC_AAC}/libFDK/src/fft_rad2.cpp
# 	${BT_CODEC_AAC}/libFDK/src/fixpoint_math.cpp
# 	${BT_CODEC_AAC}/libFDK/src/huff_nodes.cpp
# 	${BT_CODEC_AAC}/libFDK/src/mdct.cpp
# 	${BT_CODEC_AAC}/libFDK/src/nlc_dec.cpp
# 	${BT_CODEC_AAC}/libFDK/src/qmf.cpp
# 	${BT_CODEC_AAC}/libFDK/src/scale.cpp
# 	${BT_CODEC_AAC}/libSYS/src/genericStds.cpp
# 	${BT_CODEC_AAC}/libSYS/src/syslib_channelMapDescr.cpp
# 	${BT_CODEC_AAC}/libMpegTPDec/src/tpdec_adif.cpp
# 	${BT_CODEC_AAC}/libMpegTPDec/src/tpdec_adts.cpp
# 	${BT_CODEC_AAC}/libMpegTPDec/src/tpdec_asc.cpp
# 	${BT_CODEC_AAC}/libMpegTPDec/src/tpdec_drm.cpp
# 	${BT_CODEC_AAC}/libMpegTPDec/src/tpdec_latm.cpp
# 	${BT_CODEC_AAC}/libMpegTPDec/src/tpdec_lib.cpp
# 	${BT_CODEC_AAC}/libMpegTPEnc/src/tpenc_adif.cpp
# 	${BT_CODEC_AAC}/libMpegTPEnc/src/tpenc_adts.cpp
# 	${BT_CODEC_AAC}/libMpegTPEnc/src/tpenc_asc.cpp
# 	${BT_CODEC_AAC}/libMpegTPEnc/src/tpenc_latm.cpp
# 	${BT_CODEC_AAC}/libMpegTPEnc/src/tpenc_lib.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/env_calc.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/env_dec.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/env_extr.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/hbe.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/HFgen_preFlat.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/huff_dec.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/lpp_tran.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/psbitdec.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/psdec.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/psdec_drm.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/psdecrom_drm.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/pvc_dec.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/sbr_deb.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/sbr_dec.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/sbrdec_drc.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/sbrdec_freq_sca.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/sbrdecoder.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/sbr_ram.cpp
# 	${BT_CODEC_AAC}/libSBRdec/src/sbr_rom.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/bit_sbr.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/code_env.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/env_bit.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/env_est.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/fram_gen.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/invf_est.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/mh_det.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/nf_est.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/ps_bitenc.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/ps_encode.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/ps_main.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/resampler.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/sbrenc_freq_sca.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/sbr_encoder.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/sbrenc_ram.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/sbrenc_rom.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/sbr_misc.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/ton_corr.cpp
# 	${BT_CODEC_AAC}/libSBRenc/src/tran_det.cpp
# 	${BT_CODEC_AAC}/libDRCdec/src/drcDec_gainDecoder.cpp
# 	${BT_CODEC_AAC}/libDRCdec/src/drcDec_reader.cpp
# 	${BT_CODEC_AAC}/libDRCdec/src/drcDec_rom.cpp
# 	${BT_CODEC_AAC}/libDRCdec/src/drcDec_selectionProcess.cpp
# 	${BT_CODEC_AAC}/libDRCdec/src/drcDec_tools.cpp
# 	${BT_CODEC_AAC}/libDRCdec/src/drcGainDec_init.cpp
# 	${BT_CODEC_AAC}/libDRCdec/src/drcGainDec_preprocess.cpp
# 	${BT_CODEC_AAC}/libDRCdec/src/drcGainDec_process.cpp
# 	${BT_CODEC_AAC}/libDRCdec/src/FDK_drcDecLib.cpp
# 	${BT_CODEC_AAC}/libArithCoding/src/ac_arith_coder.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_bitdec.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_calcM1andM2.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_dec_conceal.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_dec.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_dec_lib.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_process.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_qmf.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_reshapeBBEnv.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_rom.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_smoothing.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_stp.cpp
# 	${BT_CODEC_AAC}/libSACdec/src/sac_tsd.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_bitstream.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_delay.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_dmx_tdom_enh.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_filter.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_framewindowing.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_huff_tab.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_lib.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_nlc_enc.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_onsetdetect.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_paramextract.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_staticgain.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_tree.cpp
# 	${BT_CODEC_AAC}/libSACenc/src/sacenc_vectorfunctions.cpp
# )

set(MODULE_C_DEFINES HAVE_ALLOCA_H DISABLE_FLOAT_API FIXED_POINT)

ameba_app_library(btcodec)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
