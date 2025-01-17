
if(CONFIG_AMEBALITE OR CONFIG_AMEBASMART)
	ameba_internal_library(example_raw_ecdsa_test)
endif()

if(CONFIG_AMEBALITE OR CONFIG_AMEBASMART)
target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_ecdsa_test.c
    app_example.c
)
endif()
