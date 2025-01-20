ameba_internal_library(example_mbed_sdio_loopback_device)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_sdio_loopback_device.c
    app_example.c
)
