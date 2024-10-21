
set(DIR ${BASEDIR}/component/audio/media_demux)

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
    ${BASEDIR}/component/audio/media_source/include
    ${BASEDIR}/component/audio/configs/include
    ${DIR}/include
)


set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-unused-parameter -Wno-unused-value -Wno-unused-variable -Wno-unused-but-set-variable)


set(CPPSRC ${DIR}/media_extractor.cpp ${DIR}/media_extractor_factory.cpp)

if(CONFIG_MEDIA_PLAYER)
    list(APPEND MODULE_CPP_DEFINES MEDIA_PLAYER)
endif()

if(CONFIG_MEDIA_LITE_PLAYER)
    list(APPEND MODULE_CPP_DEFINES MEDIA_LITE_PLAYER)
endif()

#HELIX AAC
if(CONFIG_MEDIA_CODEC_HAAC OR CONFIG_MEDIA_LITE_CODEC_HAAC)
    list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_AAC_DELIVER_ADTS_HEADER)
endif()

# AAC demux
if(CONFIG_MEDIA_DEMUX_AAC OR CONFIG_MEDIA_LITE_DEMUX_AAC)
    list(APPEND CPPSRC ${DIR}/extractors/aac/aac_extractor.cpp)
    list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_AAC)
endif()

# WAV demux
if(CONFIG_MEDIA_DEMUX_WAV OR CONFIG_MEDIA_LITE_DEMUX_WAV)
    list(APPEND CPPSRC ${DIR}/extractors/wav/wav_extractor.cpp)
    list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_WAV)
endif()

# MP3 demux
if(CONFIG_MEDIA_DEMUX_MP3 OR CONFIG_MEDIA_LITE_DEMUX_MP3)
    list(
        APPEND CPPSRC
        ${DIR}/extractors/mp3/mp3_extractor.cpp
        ${DIR}/extractors/mp3/mp3_vbri_seeker.cpp
        ${DIR}/extractors/mp3/mpr_xing_seeker.cpp
        ${DIR}/extractors/mp3/mp3_id3.cpp
    )
    list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_MP3)
endif()

# AMR demux
if(CONFIG_MEDIA_DEMUX_AMR OR CONFIG_MEDIA_LITE_DEMUX_AMR)
    list(APPEND CPPSRC ${DIR}/extractors/amr/amr_extractor.cpp)
    list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_AMR)
endif()

# OGG demux
if(CONFIG_MEDIA_DEMUX_OGG OR CONFIG_MEDIA_LITE_DEMUX_OGG)
    list(APPEND CPPSRC ${DIR}/extractors/ogg/ogg_extractor.cpp)
    list(APPEND MODULE_IFLAGS ${BASEDIR}/component/audio/third_party/tremolo)
    list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_OGG)
endif()

# FLAC demux
if(CONFIG_MEDIA_DEMUX_FLAC OR CONFIG_MEDIA_LITE_DEMUX_FLAC)
    list(APPEND CPPSRC ${DIR}/extractors/flac/flac_extractor.cpp)
    list(APPEND MODULE_IFLAGS ${BASEDIR}/component/audio/third_party/flac/include)
    list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_FLAC)
endif()

# MP4 demux
if(CONFIG_MEDIA_DEMUX_MP4 OR CONFIG_MEDIA_LITE_DEMUX_MP4)
    list(
        APPEND CPPSRC
        ${DIR}/extractors/mp4/mpeg4_extractor.cpp
        ${DIR}/extractors/mp4/sample_table.cpp
        ${DIR}/extractors/mp4/sample_iterator.cpp
    )

    list(APPEND MODULE_CPP_DEFINES MEDIA_DEMUX_MP4)
endif()

set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
