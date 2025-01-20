ameba_internal_library(example_raw_ir_tx_nec_interrupt)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_ir_tx_nec_interrupt.c
    app_example.c
	ir_nec_protocol.c
)