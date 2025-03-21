set(DIR ${TARGETDIR}/verification)

set(MODULE_IFLAGS ${DIR})

set(CSRC)

if(CONFIG_SIMULATION)
    list(
        APPEND CSRC
        ${DIR}/simulation/ameba_simulation.c
    )
endif()

if(CONFIG_ADC_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/adc/ameba_adc_test.c
    )
endif()

if(CONFIG_EFUSE_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/efuse/ameba_efuse_test.c
    )
endif()

if(CONFIG_GDMA_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/gdma/ameba_gdma_test.c
        ${DIR}/gdma/ameba_gdma_llp_test.c
    )
endif()

if(CONFIG_GPIO_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/gpio/ameba_gpio_test.c
    )
endif()

if(CONFIG_I2C_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/i2c/rtl8195a_i2c_test_sal.c
        ${DIR}/i2c/hal_i2c_test.c
    )
endif()

if(CONFIG_PMC_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/pmc/pmc_test_km0.c
    )
endif()

if(CONFIG_SPI_COM_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/spi_communication/rtl8721d_ssi_test.c
        ${DIR}/spi_communication/hal_ssi_test.c
    )
endif()

if(CONFIG_SPIC_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/spi_flash/rtl8721d_spi_flash_test.c
        ${DIR}/spi_flash/flash_runcode_test.c
        ${DIR}/spi_flash/rtl8721d_flash_N25Q00AA_patch.c
    )
endif()

if(CONFIG_TIMER_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/timer/ameba_timer_test.c
        ${DIR}/timer/ameba_rtc_test.c
        ${DIR}/timer/ameba_pwm_test.c
        ${DIR}/timer/ameba_timer_capture_test.c
    )
endif()

if(CONFIG_UART_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/uart/ameba_uart_test.c
        ${DIR}/uart/hal_uart_test.c
        ${DIR}/uart/ameba_uart_cyclic_dma_test.c
    )
endif()

if(CONFIG_WDG_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/wdg/ameba_wdg_test.c
    )
endif()

if(CONFIG_CAPTOUCH_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/captouch/ameba_captouch_test.c
    )
endif()

if(CONFIG_QDEC_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/qdecoder/rtl8721d_qdec_test.c
    )
endif()

if(CONFIG_IPC_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/ipc/ipc_test.c
    )
endif()

if(CONFIG_MPU_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/mpu/mpu_test.c
    )
endif()

if(CONFIG_CACHE_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/cache/rtl8721d_cache_test.c
    )
endif()

if(CONFIG_RSIP_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/rsip/rsip_test.c
    )
endif()

if(CONFIG_DHRYSTONE_TEST)
    list(
        APPEND CSRC
        ${DIR}/dhrystone/dhrystone_test.c
    )
endif()

if(CONFIG_PERFORMANCE_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/platform/ameba_performance_test.c
    )
endif()

if(CONFIG_SBOOT_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/sboot/sboot_test.c
    )
endif()

if(CONFIG_WFE_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/wfe/wfe_test.c
    )
endif()

if(CONFIG_THERMAL_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/thermal/ameba_thermal_test.c
    )
endif()

if(CONFIG_CHIP_RST_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/chip_reset/ameba_reset_test.c
    )
endif()

if(CONFIG_DBG_TIMER_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/timer/ameba_dbg_timer_test.c
    )
endif()

if(CONFIG_SPINAND_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/spi_nand/ameba_spinand_test.c
    )
endif()

if(CONFIG_LOGUART_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/loguart/ameba_loguart_test.c
        ${DIR}/loguart/hal_loguart_test.c
    )
endif()

if(CONFIG_KEYSCAN_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/keyscan/ameba_keyscan_test.c
    )
endif()

if(CONFIG_FLASH_AVL)
    list(
        APPEND CSRC
        ${DIR}/mbedtls/km0_sync.c
    )
endif()

if(CONFIG_DIAG_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/../../common/Diagnose/ameba_diag.c
		${DIR}/../../common/Diagnose/ameba_diag_test.c
    )
endif()

if(CSRC)
    ameba_internal_library(test_verification)
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
endif()