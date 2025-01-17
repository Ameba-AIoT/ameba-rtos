ameba_internal_library(example_raw_i2c_int_slave)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_i2c_int_slave.c
    app_example.c
)
