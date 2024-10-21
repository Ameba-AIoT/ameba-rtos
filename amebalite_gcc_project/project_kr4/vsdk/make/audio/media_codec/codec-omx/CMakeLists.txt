set(DIR ${BASEDIR}/component/audio/media_codec/codec-omx)
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/audio_utils/include
    ${BASEDIR}/component/audio/media_utils/include
    ${BASEDIR}/component/audio/media_common/include
    ${BASEDIR}/component/audio/configs/include
    ${DIR}/include
    ${DIR}/include/media/codec/omxil
)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-unused-parameter)

set(CPPSRC)

if(CONFIG_MEDIA_PLAYER)
    list(
        APPEND CPPSRC
        ${DIR}/media_codec.cpp
        ${DIR}/codec.cpp
        ${DIR}/codec_buffer_tunnel.cpp
        ${DIR}/data_converter.cpp
        ${DIR}/omx/omx_buffer.cpp
        ${DIR}/omx/omx_utils.cpp
        ${DIR}/omx/omx_node.cpp
        ${DIR}/omx/omx.cpp
        ${DIR}/omx/omx_core.cpp
        ${DIR}/omx/sw_omx_component.cpp
    )
    list(APPEND MODULE_CPP_DEFINES MEDIA_PLAYER)

    # rawpcm codec
    if(CONFIG_MEDIA_CODEC_PCM)
        list(APPEND CPPSRC ${DIR}/codecs/rawdec/sw_raw_dec.cpp)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_PCM)
    endif()

    # aac codec
    if(CONFIG_MEDIA_CODEC_AAC)
        list(
            APPEND CPPSRC
            ${DIR}/codecs/aacdec/drc_pres_mode_wrap.cpp
            ${DIR}/codecs/aacdec/sw_aac2_dec.cpp
        )

        list(
            APPEND MODULE_IFLAGS
            ${BASEDIR}/component/audio/third_party/aac/libAACdec/include
            ${BASEDIR}/component/audio/third_party/aac/libPCMutils/include
            ${BASEDIR}/component/audio/third_party/aac/libFDK/include
            ${BASEDIR}/component/audio/third_party/aac/libSYS/include
            ${BASEDIR}/component/audio/third_party/aac/libMpegTPDec/include
            ${BASEDIR}/component/audio/third_party/aac/libSBRdec/include
            ${BASEDIR}/component/audio/third_party/aac/libArithCoding/include
            ${BASEDIR}/component/audio/third_party/aac/libDRCdec/include
            ${BASEDIR}/component/audio/third_party/aac/libSACdec/include
        )

        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_AAC)
    endif()

    # haac codec
    if(CONFIG_MEDIA_CODEC_HAAC)
        list(APPEND CPPSRC ${DIR}/codecs/helixaacdec/sw_helix_aac_dec.cpp)

        list(
            APPEND MODULE_IFLAGS
            ${BASEDIR}/component/audio/third_party/haac
        )
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_HAAC)
    endif()

    # mp3 codec
    if(CONFIG_MEDIA_CODEC_MP3)
        list(APPEND CPPSRC ${DIR}/codecs/mp3dec/sw_mp3_dec.cpp)

        list(
            APPEND MODULE_IFLAGS
            ${DIR}/codecs/mp3dec/
            ${BASEDIR}/component/audio/media_codec/common/pvmp3/include
            ${BASEDIR}/component/audio/media_codec/common/pvmp3/src
        )

        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_MP3)
    endif()

    # vorbis codec
    if(CONFIG_MEDIA_CODEC_VORBIS)
        list(APPEND CPPSRC ${DIR}/codecs/vorbisdec/sw_vorbis_dec.cpp)
        list(APPEND MODULE_IFLAGS ${BASEDIR}/component/audio/third_party/tremolo)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_VORBIS)
    endif()

    # opus codec
    if(CONFIG_MEDIA_CODEC_OPUS)
        list(APPEND CPPSRC ${DIR}/codecs/opusdec/sw_opus_dec.cpp)
        list(APPEND MODULE_IFLAGS ${BASEDIR}/component/audio/third_party/libopus/include)
        list(APPEND MODULE_CPP_DEFINES MEDIA_CODEC_OPUS)
    endif()
endif()


set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
