set(DIR ${BASEDIR}/component/audio/media_common)
set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/media_utils/include
    ${DIR}/include
)

set(MODULE_CPP_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-multichar -Wno-deprecated-copy)

set(
    CPPSRC 
    ${DIR}/media_track.cpp
    ${DIR}/media_buffer_base.cpp
    ${DIR}/media_buffer.cpp
    ${DIR}/media_buffer_group.cpp
    ${DIR}/codec_buffer_info.cpp
    ${DIR}/media_defs.cpp
    ${DIR}/meta_data_base.cpp
    ${DIR}/meta_data.cpp
    ${DIR}/packet_pool.cpp
    ${DIR}/esds.cpp
    ${DIR}/utils.cpp
    ${DIR}/media_base64.cpp
    ${DIR}/media_bit_reader.cpp
)

set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES COMPILE_DEFINITIONS "${MODULE_CPP_DEFINES}")
set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY .. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
