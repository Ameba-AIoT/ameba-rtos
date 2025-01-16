ameba_internal_library(example_http_client)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_http_client.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${BASEDIR}/component/network/httplite
)
