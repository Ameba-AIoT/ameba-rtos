ameba_internal_library(example_coap)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_coap.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${BASEDIR}/component/network/coap/include
)
