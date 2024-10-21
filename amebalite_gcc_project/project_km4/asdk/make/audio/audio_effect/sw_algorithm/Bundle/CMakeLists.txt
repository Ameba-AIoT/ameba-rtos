set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Bundle/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Common/src
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Common/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/StereoWidening/src
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/StereoWidening/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Eq/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Eq/src
)

set(DIR ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Bundle/src)

set(MODULE_C_DEFINES __RTOS__ NDEBUG)
set(MODULE_C_OPTIONS -UDEBUG -Werror)

set(
    CSRC
    ${DIR}/LVM_Buffers.c
    ${DIR}/LVM_Control.c
    ${DIR}/LVM_Init.c
    ${DIR}/LVM_Process.c
    ${DIR}/LVM_Tables.c
    ${DIR}/LVM_API_Specials.c
)

set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")
set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(audio_mixer_${PROJECT_NAME} PRIVATE ${CSRC})
target_sources(audio_passthrough_${PROJECT_NAME} PRIVATE ${CSRC})
