ameba_internal_library(example_raw_ir_tx_nec_polling)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_ir_tx_nec_polling.c
    app_example.c
	ir_nec_protocol.c
)
