set(DIR ${BASEDIR}/component/audio/media_codec/codec-simple)
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/audio_utils/include
    ${BASEDIR}/component/audio/configs/include
    ${DIR}/include
)

set(MODULE_C_DEFINES __RTOS__ MEDIA_LITE_PLAYER)
set(MODULE_C_OPTIONS -Wno-multichar -Wno-unused-parameter)

set(CSRC ${DIR}/sw_media_codec_factory.c)

# rawpcm codec
if(CONFIG_MEDIA_LITE_CODEC_PCM)
    list(APPEND CSRC ${DIR}/codecs/rawdec/sw_raw_dec_lite.c)
    list(APPEND MODULE_C_DEFINES MEDIA_LITE_CODEC_PCM)
endif()

# haac codec
if(CONFIG_MEDIA_LITE_CODEC_HAAC)
    list(APPEND CSRC ${DIR}/codecs/helixaacdec/sw_helix_aac_dec_lite.c)
    list(APPEND MODULE_IFLAGS ${BASEDIR}/component/audio/third_party/haac)
    list(APPEND MODULE_C_DEFINES MEDIA_LITE_CODEC_HAAC)
endif()

# mp3 codec
if(CONFIG_MEDIA_LITE_CODEC_MP3)
    list(APPEND CSRC ${DIR}/codecs/mp3dec/sw_mp3_dec_lite.c)
    list(APPEND MODULE_IFLAGS
        ${DIR}/codecs/mp3dec
        ${BASEDIR}/component/audio/media_codec/common/pvmp3/include
        ${BASEDIR}/component/audio/media_codec/common/pvmp3/src
    )
    list(APPEND MODULE_C_DEFINES MEDIA_LITE_CODEC_MP3)
endif()

set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")
set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
