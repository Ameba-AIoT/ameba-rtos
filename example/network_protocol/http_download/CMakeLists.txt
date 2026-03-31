ameba_internal_library(example_http_download)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_http_download.c
    app_example.c
)

target_include_directories(
    ${CURRENT_LIB_NAME} PRIVATE
    ${BASEDIR}/component/network/httplite
)
