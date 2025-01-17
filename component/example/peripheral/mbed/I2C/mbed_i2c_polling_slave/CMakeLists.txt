ameba_internal_library(example_mbed_i2c_polling_slave)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_mbed_i2c_polling_slave.c
    app_example.c
)
