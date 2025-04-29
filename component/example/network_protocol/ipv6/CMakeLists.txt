ameba_internal_library(example_ipv6)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ipv6.c
    app_example.c
)

target_compile_options(
    ${CURRENT_LIB_NAME} PRIVATE
    -Wno-unused-function
)
