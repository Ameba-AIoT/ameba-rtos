ameba_internal_library(example_httpd)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_httpd.c
    app_example.c
)
