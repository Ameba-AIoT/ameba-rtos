ameba_internal_library(example_wifi_tsf_sync)

ameba_global_define(CONFIG_WHC_DEV_TSF_SYNC)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE

    example_wifi_tsf_sync.c
    tsf_sync_dev.c
    app_example.c
)
