set(DIR ${BASEDIR}/component/soc/amebadplus/verification)

set(MODULE_IFLAGS ${DIR})
set(MODULE_C_OPTIONS -O0)

if (CONFIG_SEC_VERIFY)
    if (CONFIG_AES_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/crypto/ameba_aes_ecb_test_secure.c
            ${DIR}/secure/crypto/ameba_aes_ecb_sim.c
        )
    endif()
    if(CONFIG_CRYPTO_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/crypto/ameba_crypto_test_secure.c
        )
    endif()
    if (CONFIG_GDMA_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/gdma/ameba_gdma_test_secure.c
        )
    endif()
    if(CONFIG_EFUSE_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/efuse/ameba_efuse_test_secure.c
        )
    endif()
    if(CONFIG_CACHE_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/cache/rtl8721d_cache_test_secure.c
        )
    endif()
    if(CONFIG_RDP_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/rdp/rdp_test.c
        )
    endif()
    if(CONFIG_WDG_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/wdg/ameba_wdg_test_secure.c
        )
    endif()
    if(CONFIG_TRNG_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/trng/ameba_trng_test_secure.c
        )
    endif()
    if(CONFIG_LEDC_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/ledc/ameba_ledc_test_secure.c
        )
    endif()
    if(CONFIG_IRDA_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/IR/protocol/ir_nec_protocol.c
            ${DIR}/secure/ir/ameba_ir_test_secure.c
        )
    endif()
    if(CONFIG_TIMER_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/timer/ameba_timer_test_secure.c
        )
    endif()
    if(CONFIG_DBGTIM_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/timer/ameba_dbg_timer_test_secure.c
        )
    endif()
    if(CONFIG_GPIO_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/gpio/ameba_gpio_test_secure.c
        )
    endif()
    if(CONFIG_RTC_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/rtc/ameba_rtc_test_secure.c
        )
    endif()
    if(CONFIG_ADC_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/adc/ameba_adc_test_secure.c
        )
    endif()
    if(CONFIG_THERMAL_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/thermal/ameba_thermal_test_secure.c
        )
    endif()
    if(CONFIG_LCDC_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/lcdc/ameba_lcdc_test_secure.c
        )
    endif()
    if(CONFIG_UART_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/uart/ameba_uart_secure_test.c
            ${DIR}/secure/uart/ameba_hal_uart_secure_test.c
        )
    endif()
    if(CONFIG_LOG_UART_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/uart/ameba_loguart_secure_test.c
        )
    endif()
    if(CONFIG_SPI_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/spi_communication/amebad2_spi_secure_test.c
            ${DIR}/secure/spi_communication/amebad2_hal_spi_secure_test.c
        )
    endif()
    if(CONFIG_I2C_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/i2c/amebad2_i2c_secure_test.c
            ${DIR}/secure/i2c/amebad2_hal_i2c_secure_test.c
        )
    endif()
    if(CONFIG_EDDSA_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/eddsa/ameba_eddsa_test_secure.c
        )
    endif()
    if(CONFIG_ECDSA_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/ecdsa/ameba_ecdsa_test_secure.c
        )
    endif()
    if(CONFIG_RSA_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/rsa/ameba_rsa_test_secure.c
        )
    endif()
    if(CONFIG_TZ_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/trustzone/ameba_trustzone_test_secure.c
        )
    endif()
    if(CONFIG_AUDIO_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/audio/ameba_audio_test_secure.c
        )
    endif()
    if(CONFIG_KEYSCAN_SEC_VERIFY)
        list(
            APPEND CSRC 
            ${DIR}/secure/keyscan/ameba_keyscan_secure_test.c
        )
    endif()
endif()

set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY ../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")

target_sources(target_img3 PRIVATE ${CSRC})