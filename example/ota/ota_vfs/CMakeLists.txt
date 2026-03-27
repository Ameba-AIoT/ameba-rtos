ameba_internal_library(example_ota_vfs)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_ota_vfs.c
    app_example.c
)
