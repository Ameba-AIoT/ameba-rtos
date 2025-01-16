ameba_internal_library(example_httpd_vfs)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_httpd_vfs.c
    app_example.c
)
