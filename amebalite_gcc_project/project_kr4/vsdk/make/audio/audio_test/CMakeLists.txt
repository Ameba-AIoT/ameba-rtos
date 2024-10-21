set(
    MDOULE_IFLAGS 
    ${BASEDIR}/component/audio/base/audio_utils/include
    ${BASEDIR}/component/audio/base/cutils/interfaces
    ${BASEDIR}/component/audio/base/log/interfaces
    ${BASEDIR}/component/audio/base/libs/src/include
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_cxx/interfaces
    ${BASEDIR}/component/audio/audio_algorithm/sonic
    ${BASEDIR}/component/audio/interfaces
)

set(
    AUDIO_TEST_DIR
    ${BASEDIR}/component/audio/audio_test
)

set(
    MODULE_DEFINES 
    __RTOS__
)

set(
    MODULE_OPTIONS 
    -Wno-multichar -Wno-unused-parameter -Wno-narrowing
)

set(
    CSRC 
    ${AUDIO_TEST_DIR}/audio_test.c
    ${AUDIO_TEST_DIR}/audio_track_test.c
    ${AUDIO_TEST_DIR}/audio_record_test.c
    ${AUDIO_TEST_DIR}/audio_control_test.c
    ${AUDIO_TEST_DIR}/audio_test_utils.c
    ${TRACK_TEST_DIR}/audio_wav_test_thread.c
)

set(
    CPPSRC
    ${TRACK_TEST_DIR}/audio_wav_test.cpp
)

ameba_internal_library(audio_test)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE ${MODULE_OPTIONS})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CPPSRC})
