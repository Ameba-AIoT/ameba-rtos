ameba_internal_library(example_mydata_source_player)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mydata_source_player.c
    mydata_source.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${BASEDIR}/component/audio/interfaces
    ${BASEDIR}/component/audio/base/osal/osal_c/interfaces
)

