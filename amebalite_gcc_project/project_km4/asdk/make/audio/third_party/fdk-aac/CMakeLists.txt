set(DIR ${BASEDIR}/component/audio/third_party/fdk-aac)
set(
    MODULE_IFLAGS
    ${DIR}/libAACdec/include
    ${DIR}/libPCMutils/include
    ${DIR}/libFDK/include
    ${DIR}/libSYS/include
    ${DIR}/libMpegTPDec/include
    ${DIR}/libSBRdec/include
    ${DIR}/libArithCoding/include
    ${DIR}/libDRCdec/include
    ${DIR}/libSACdec/include
)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_C_OPTIONS -Wno-error)

set(
    CPPSRC
    ${DIR}/libAACdec/src/aacdec_drc.cpp
    ${DIR}/libAACdec/src/aacdec_hcr_bit.cpp
    ${DIR}/libAACdec/src/aacdec_hcr.cpp
    ${DIR}/libAACdec/src/aacdec_hcrs.cpp
    ${DIR}/libAACdec/src/aacdecoder.cpp
    ${DIR}/libAACdec/src/aacdecoder_lib.cpp
    ${DIR}/libAACdec/src/aacdec_pns.cpp
    ${DIR}/libAACdec/src/aacdec_tns.cpp
    ${DIR}/libAACdec/src/aac_ram.cpp
    ${DIR}/libAACdec/src/aac_rom.cpp
    ${DIR}/libAACdec/src/block.cpp
    ${DIR}/libAACdec/src/channel.cpp
    ${DIR}/libAACdec/src/channelinfo.cpp
    ${DIR}/libAACdec/src/conceal.cpp
    ${DIR}/libAACdec/src/FDK_delay.cpp
    ${DIR}/libAACdec/src/ldfiltbank.cpp
    ${DIR}/libAACdec/src/pulsedata.cpp
    ${DIR}/libAACdec/src/rvlcbit.cpp
    ${DIR}/libAACdec/src/rvlcconceal.cpp
    ${DIR}/libAACdec/src/rvlc.cpp
    ${DIR}/libAACdec/src/stereo.cpp
    ${DIR}/libAACdec/src/usacdec_ace_d4t64.cpp
    ${DIR}/libAACdec/src/usacdec_acelp.cpp
    ${DIR}/libAACdec/src/usacdec_ace_ltp.cpp
    ${DIR}/libAACdec/src/usacdec_fac.cpp
    ${DIR}/libAACdec/src/usacdec_lpc.cpp
    ${DIR}/libAACdec/src/usacdec_lpd.cpp
    ${DIR}/libAACdec/src/usacdec_rom.cpp

    ${DIR}/libPCMutils/src/limiter.cpp
    ${DIR}/libPCMutils/src/pcm_utils.cpp
    ${DIR}/libPCMutils/src/pcmdmx_lib.cpp

    ${DIR}/libFDK/src/FDK_lpc.cpp
    ${DIR}/libFDK/src/fixpoint_math.cpp
    ${DIR}/libFDK/src/FDK_matrixCalloc.cpp
    ${DIR}/libFDK/src/qmf.cpp
    ${DIR}/libFDK/src/FDK_tools_rom.cpp
    ${DIR}/libFDK/src/FDK_qmf_domain.cpp
    ${DIR}/libFDK/src/FDK_decorrelate.cpp
    ${DIR}/libFDK/src/mdct.cpp
    ${DIR}/libFDK/src/autocorr2nd.cpp
    ${DIR}/libFDK/src/FDK_bitbuffer.cpp
    ${DIR}/libFDK/src/fft_rad2.cpp
    ${DIR}/libFDK/src/nlc_dec.cpp
    ${DIR}/libFDK/src/FDK_crc.cpp
    ${DIR}/libFDK/src/fft.cpp
    ${DIR}/libFDK/src/FDK_trigFcts.cpp
    ${DIR}/libFDK/src/scale.cpp
    ${DIR}/libFDK/src/FDK_hybrid.cpp
    ${DIR}/libFDK/src/huff_nodes.cpp
    ${DIR}/libFDK/src/FDK_core.cpp
    ${DIR}/libFDK/src/dct.cpp

    ${DIR}/libSYS/src/syslib_channelMapDescr.cpp
    ${DIR}/libSYS/src/genericStds.cpp

    ${DIR}/libMpegTPDec/src/tpdec_adts.cpp
    ${DIR}/libMpegTPDec/src/tpdec_lib.cpp
    ${DIR}/libMpegTPDec/src/tpdec_adif.cpp
    ${DIR}/libMpegTPDec/src/tpdec_drm.cpp
    ${DIR}/libMpegTPDec/src/tpdec_latm.cpp
    ${DIR}/libMpegTPDec/src/tpdec_asc.cpp

    ${DIR}/libArithCoding/src/ac_arith_coder.cpp

    ${DIR}/libDRCdec/src/drcDec_tools.cpp
    ${DIR}/libDRCdec/src/drcGainDec_init.cpp
    ${DIR}/libDRCdec/src/drcDec_reader.cpp
    ${DIR}/libDRCdec/src/drcDec_rom.cpp
    ${DIR}/libDRCdec/src/drcDec_selectionProcess.cpp
    ${DIR}/libDRCdec/src/drcGainDec_process.cpp
    ${DIR}/libDRCdec/src/FDK_drcDecLib.cpp
    ${DIR}/libDRCdec/src/drcDec_gainDecoder.cpp
    ${DIR}/libDRCdec/src/drcGainDec_preprocess.cpp

    ${DIR}/libSBRdec/src/pvc_dec.cpp
    ${DIR}/libSBRdec/src/psbitdec.cpp
    ${DIR}/libSBRdec/src/sbr_ram.cpp
    ${DIR}/libSBRdec/src/env_extr.cpp
    ${DIR}/libSBRdec/src/sbrdec_drc.cpp
    ${DIR}/libSBRdec/src/huff_dec.cpp
    ${DIR}/libSBRdec/src/sbrdecoder.cpp
    ${DIR}/libSBRdec/src/env_calc.cpp
    ${DIR}/libSBRdec/src/env_dec.cpp
    ${DIR}/libSBRdec/src/lpp_tran.cpp
    ${DIR}/libSBRdec/src/psdec_drm.cpp
    ${DIR}/libSBRdec/src/sbr_dec.cpp
    ${DIR}/libSBRdec/src/sbr_crc.cpp
    ${DIR}/libSBRdec/src/HFgen_preFlat.cpp
    ${DIR}/libSBRdec/src/psdecrom_drm.cpp
    ${DIR}/libSBRdec/src/sbr_deb.cpp
    ${DIR}/libSBRdec/src/hbe.cpp
    ${DIR}/libSBRdec/src/psdec.cpp
    ${DIR}/libSBRdec/src/sbrdec_freq_sca.cpp
    ${DIR}/libSBRdec/src/sbr_rom.cpp

    ${DIR}/libSACdec/src/sac_tsd.cpp
    ${DIR}/libSACdec/src/sac_process.cpp
    ${DIR}/libSACdec/src/sac_calcM1andM2.cpp
    ${DIR}/libSACdec/src/sac_dec_conceal.cpp
    ${DIR}/libSACdec/src/sac_stp.cpp
    ${DIR}/libSACdec/src/sac_dec.cpp
    ${DIR}/libSACdec/src/sac_qmf.cpp
    ${DIR}/libSACdec/src/sac_rom.cpp
    ${DIR}/libSACdec/src/sac_bitdec.cpp
    ${DIR}/libSACdec/src/sac_reshapeBBEnv.cpp
    ${DIR}/libSACdec/src/sac_smoothing.cpp
    ${DIR}/libSACdec/src/sac_dec_lib.cpp
)

ameba_app_library(fdkaac)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_OPTIONS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_CPP_DEFINES})
