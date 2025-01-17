if(CONFIG_AMEBADPLUS OR CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAD)
	ameba_internal_library(example_raw_i2c_dma_mode)
endif()

if(CONFIG_AMEBADPLUS OR CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAD)
	target_sources(
		${CURRENT_LIB_NAME} PRIVATE
		example_raw_i2c_dma_mode.c
		app_example.c
	)
endif()