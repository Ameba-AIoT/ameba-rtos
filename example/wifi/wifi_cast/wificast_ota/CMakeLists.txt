ameba_internal_library(example_wificast_ota)

target_sources(${CURRENT_LIB_NAME} PRIVATE example_wificast_ota.c)

target_compile_options(
    ${CURRENT_LIB_NAME} PRIVATE
    -Wno-stringop-overflow
)
