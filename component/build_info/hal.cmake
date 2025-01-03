if (__INCLUDED_HAL_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_HAL_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_hal INTERFACE)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_hal p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/hal/src/flash_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/pinmap_common.c
    ${d_SOC_PLATFORM_DIR}/hal/src/sys_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/us_ticker.c
    ${d_SOC_PLATFORM_DIR}/hal/src/timer_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/dma_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/gpio_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/gpio_irq_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/pinmap.c
    ${d_SOC_PLATFORM_DIR}/hal/src/port_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/spi_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/serial_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/i2c_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/pwmout_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/analogin_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/efuse_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/wdt_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/us_ticker_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/wait_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/rtc_api.c
    ${d_SOC_PLATFORM_DIR}/hal/src/captouch_api.c
)
#--------------------------#