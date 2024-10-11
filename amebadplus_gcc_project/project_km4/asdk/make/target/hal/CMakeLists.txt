set(DIR ${TARGETDIR}/hal/src)

set(CSRC 
    ${DIR}/flash_api.c
    ${DIR}/pinmap_common.c
    ${DIR}/sys_api.c
    ${DIR}/us_ticker.c
    ${DIR}/timer_api.c
    ${DIR}/dma_api.c
    ${DIR}/gpio_api.c
    ${DIR}/gpio_irq_api.c
    ${DIR}/pinmap.c
    ${DIR}/port_api.c
    ${DIR}/spi_api.c
    ${DIR}/serial_api.c
    ${DIR}/i2c_api.c
    ${DIR}/pwmout_api.c
    ${DIR}/analogin_api.c
    ${DIR}/efuse_api.c
    ${DIR}/wdt_api.c
    ${DIR}/us_ticker_api.c
    ${DIR}/wait_api.c
    ${DIR}/rtc_api.c
    ${DIR}/captouch_api.c
    ${DIR}/spdio_api.c
)

ameba_internal_library(target_hal)
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
