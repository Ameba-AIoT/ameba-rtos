set(
    MODULE_IFLAGS
    ${BASEDIR}/component/audio/interfaces
    ${BASEDIR}/component/audio/base/audio_utils/include
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/libs/src/include
)

set(DIR ${BASEDIR}/component/audio/audio_algorithm/plc)

set(MODULE_C_OPTIONS -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Werror)
set(MODULE_CPP_OPTIONS  -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-narrowing -Werror)

set(
    CSRC 
    ${DIR}/vox_mem.c
    ${DIR}/vox_math.c
)

set(
    CPPSRC 
    ${DIR}/rt_plc.cpp
)

set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
set_source_files_properties(${CPPSRC} DIRECTORY ../../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CPPSRC} DIRECTORY ../../.. PROPERTIES COMPILE_OPTIONS "${MODULE_CPP_OPTIONS}")

target_sources(audio_mixer_${PROJECT_NAME} PRIVATE ${CSRC} ${CPPSRC})
