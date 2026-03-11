ameba_internal_library(example_cJSON)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_cJSON.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${BASEDIR}/component/network/cJSON
)
