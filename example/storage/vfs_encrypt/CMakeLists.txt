ameba_internal_library(example_vfs_encrypt)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_vfs_encrypt.c
    app_example.c
)
