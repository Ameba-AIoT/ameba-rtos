ameba_internal_library(example_audio_manager)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_audio_manager.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${BASEDIR}/component/audio/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
)
