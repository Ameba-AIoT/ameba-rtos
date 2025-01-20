ameba_internal_library(example_raw_ir_rx_learning)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_ir_rx_learning.c
    app_example.c
	ir_nec_protocol.c
)