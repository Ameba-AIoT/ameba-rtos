##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

## Differentiated configuration based on SoC types
if (CONFIG_MEDIA_PLAYER)
    ameba_list_append(public_libraries
        ${c_SDK_LIB_APPLICATION_DIR}/lib_fdk_aac.a
    )
endif()

list(REMOVE_DUPLICATES public_libraries)
# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries} p_NO_WHOLE_ARCHIVE) #NOTE: no_whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources
    libAACdec/src/aacdec_drc.cpp
    libAACdec/src/aacdec_hcr_bit.cpp
    libAACdec/src/aacdec_hcr.cpp
    libAACdec/src/aacdec_hcrs.cpp
    libAACdec/src/aacdecoder.cpp
    libAACdec/src/aacdecoder_lib.cpp
    libAACdec/src/aacdec_pns.cpp
    libAACdec/src/aacdec_tns.cpp
    libAACdec/src/aac_ram.cpp
    libAACdec/src/aac_rom.cpp
    libAACdec/src/block.cpp
    libAACdec/src/channel.cpp
    libAACdec/src/channelinfo.cpp
    libAACdec/src/conceal.cpp
    libAACdec/src/FDK_delay.cpp
    libAACdec/src/ldfiltbank.cpp
    libAACdec/src/pulsedata.cpp
    libAACdec/src/rvlcbit.cpp
    libAACdec/src/rvlcconceal.cpp
    libAACdec/src/rvlc.cpp
    libAACdec/src/stereo.cpp
    libAACdec/src/usacdec_ace_d4t64.cpp
    libAACdec/src/usacdec_acelp.cpp
    libAACdec/src/usacdec_ace_ltp.cpp
    libAACdec/src/usacdec_fac.cpp
    libAACdec/src/usacdec_lpc.cpp
    libAACdec/src/usacdec_lpd.cpp
    libAACdec/src/usacdec_rom.cpp

    libPCMutils/src/limiter.cpp
    libPCMutils/src/pcm_utils.cpp
    libPCMutils/src/pcmdmx_lib.cpp

    libFDK/src/FDK_lpc.cpp
    libFDK/src/fixpoint_math.cpp
    libFDK/src/FDK_matrixCalloc.cpp
    libFDK/src/qmf.cpp
    libFDK/src/FDK_tools_rom.cpp
    libFDK/src/FDK_qmf_domain.cpp
    libFDK/src/FDK_decorrelate.cpp
    libFDK/src/mdct.cpp
    libFDK/src/autocorr2nd.cpp
    libFDK/src/FDK_bitbuffer.cpp
    libFDK/src/fft_rad2.cpp
    libFDK/src/nlc_dec.cpp
    libFDK/src/FDK_crc.cpp
    libFDK/src/fft.cpp
    libFDK/src/FDK_trigFcts.cpp
    libFDK/src/scale.cpp
    libFDK/src/FDK_hybrid.cpp
    libFDK/src/huff_nodes.cpp
    libFDK/src/FDK_core.cpp
    libFDK/src/dct.cpp

    libSYS/src/syslib_channelMapDescr.cpp
    libSYS/src/genericStds.cpp

    libMpegTPDec/src/tpdec_adts.cpp
    libMpegTPDec/src/tpdec_lib.cpp
    libMpegTPDec/src/tpdec_adif.cpp
    libMpegTPDec/src/tpdec_drm.cpp
    libMpegTPDec/src/tpdec_latm.cpp
    libMpegTPDec/src/tpdec_asc.cpp

    libArithCoding/src/ac_arith_coder.cpp

    libDRCdec/src/drcDec_tools.cpp
    libDRCdec/src/drcGainDec_init.cpp
    libDRCdec/src/drcDec_reader.cpp
    libDRCdec/src/drcDec_rom.cpp
    libDRCdec/src/drcDec_selectionProcess.cpp
    libDRCdec/src/drcGainDec_process.cpp
    libDRCdec/src/FDK_drcDecLib.cpp
    libDRCdec/src/drcDec_gainDecoder.cpp
    libDRCdec/src/drcGainDec_preprocess.cpp


    libSBRdec/src/pvc_dec.cpp
    libSBRdec/src/psbitdec.cpp
    libSBRdec/src/sbr_ram.cpp
    libSBRdec/src/env_extr.cpp
    libSBRdec/src/sbrdec_drc.cpp
    libSBRdec/src/huff_dec.cpp
    libSBRdec/src/sbrdecoder.cpp
    libSBRdec/src/env_calc.cpp
    libSBRdec/src/env_dec.cpp
    libSBRdec/src/lpp_tran.cpp
    libSBRdec/src/psdec_drm.cpp
    libSBRdec/src/sbr_dec.cpp
    libSBRdec/src/sbr_crc.cpp
    libSBRdec/src/HFgen_preFlat.cpp
    libSBRdec/src/psdecrom_drm.cpp
    libSBRdec/src/sbr_deb.cpp
    libSBRdec/src/hbe.cpp
    libSBRdec/src/psdec.cpp
    libSBRdec/src/sbrdec_freq_sca.cpp
    libSBRdec/src/sbr_rom.cpp

    libSACdec/src/sac_tsd.cpp
    libSACdec/src/sac_process.cpp
    libSACdec/src/sac_calcM1andM2.cpp
    libSACdec/src/sac_dec_conceal.cpp
    libSACdec/src/sac_stp.cpp
    libSACdec/src/sac_dec.cpp
    libSACdec/src/sac_qmf.cpp
    libSACdec/src/sac_rom.cpp
    libSACdec/src/sac_bitdec.cpp
    libSACdec/src/sac_reshapeBBEnv.cpp
    libSACdec/src/sac_smoothing.cpp
    libSACdec/src/sac_dec_lib.cpp
)
ameba_list_append(private_includes
    libAACdec/include
    libPCMutils/include
    libFDK/include
    libSYS/include
    libMpegTPDec/include
    libSBRdec/include
    libArithCoding/include
    libDRCdec/include
    libSACdec/include
)
ameba_list_append(private_definitions
    __RTOS__
)
ameba_list_append(private_compile_options
    -Wno-error
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(fdk_aac
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################