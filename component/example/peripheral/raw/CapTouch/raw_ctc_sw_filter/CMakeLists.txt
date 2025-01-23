ameba_internal_library(example_raw_ctc_sw_filter)

if(CONFIG_AMEBASMART)
	set(DIR AmebaSmart)
elseif(CONFIG_AMEBALITE)
	set(DIR AmebaLite)
elseif(CONFIG_AMEBADPLUS)
	set(DIR AmebaDplus)
endif()

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
	example_raw_ctc_sw_filter.c
	${DIR}/main.c
)
