set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Eq/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Common/include
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Common/src
)

set(DIR ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Eq/src)

set(MODULE_C_DEFINES __RTOS__ NDEBUG)
set(MODULE_C_OPTIONS -UDEBUG -Werror)

set(
    CSRC
    ${DIR}/LVEQNB_CalcCoef.c
    ${DIR}/LVEQNB_Control.c
    ${DIR}/LVEQNB_Tables.c
    ${DIR}/LVEQNB_Init.c
    ${DIR}/LVEQNB_Process.c
)

set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")
set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(audio_mixer_${PROJECT_NAME} PRIVATE ${CSRC})
target_sources(audio_passthrough_${PROJECT_NAME} PRIVATE ${CSRC})
