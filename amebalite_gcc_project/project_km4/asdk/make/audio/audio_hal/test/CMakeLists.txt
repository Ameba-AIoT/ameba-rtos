set(RENDER_TEST_DIR ${BASEDIR}/component/audio/audio_hal/test/render)
set(CAPTURE_TEST_DIR ${BASEDIR}/component/audio/audio_hal/test/capture)

set(MODULE_IFLAGS ${BASEDIR}/component/audio/audio_hal/interfaces)

set(MODULE_C_DEFINES __RTOS__)
set(MODULE_CPP_OPTIONS -Wno-unused-parameter)

set(CSRC ${RENDER_TEST_DIR}/render_test.c ${CAPTURE_TEST_DIR}/capture_test.c)

ameba_internal_library(audio_hal_test)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
