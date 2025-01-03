set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/audio_utils/include
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/interfaces
)

set(DIR ${BASEDIR}/component/audio/audio_stream/passthrough)

set(MODULE_C_DEFINES __RTOS__ NDEBUG)
set(MODULE_C_OPTIONS -UDEBUG -Werror)


set(
    CSRC
    ${DIR}/audio_track.c
    ${DIR}/audio_record.c
    ${DIR}/audio_service.c
    ${DIR}/audio_manager.c
)



set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")
set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(audio_passthrough_${PROJECT_NAME} PRIVATE ${CSRC})
