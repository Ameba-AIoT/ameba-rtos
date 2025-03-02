if (__INCLUDED_TEST_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_TEST_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_test_apple INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_test_sboot INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_test_std_lib INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_test_verification INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_test_verification_secure INTERFACE)

#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_test_apple p_SCOPE interface
    ${c_COMPONENT_DIR}/application/apple/homekit/crypto/chacha
    ${c_COMPONENT_DIR}/application/apple/homekit/crypto/ed25519
    ${c_COMPONENT_DIR}/application/apple/homekit/crypto/ed25519/core
    ${c_COMPONENT_DIR}/application/apple/homekit/crypto/poly1305
    ${c_COMPONENT_DIR}/application/apple/homekit/crypto/sha512
    ${c_COMPONENT_DIR}/application/apple/WACServer/External/Curve25519
    ${c_COMPONENT_DIR}/application/apple/WACServer/External/GladmanAES
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_apple p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/swlib/apple/curve25519-donna-test.c
    ${d_SOC_PLATFORM_DIR}/verification/swlib/apple/ed25519_test.c
    ${d_SOC_PLATFORM_DIR}/verification/swlib/apple/sha512_hkdf_test.c
    ${d_SOC_PLATFORM_DIR}/verification/swlib/apple/aes_test.c
    ${d_SOC_PLATFORM_DIR}/verification/swlib/apple/chachatest.c
    ${d_SOC_PLATFORM_DIR}/verification/swlib/apple/poly1305test.c
)
#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_sboot p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/sboot/test/curve25519-donna-test.c
    ${d_SOC_PLATFORM_DIR}/sboot/test/ed25519_test.c
    ${d_SOC_PLATFORM_DIR}/sboot/test/sha512_hkdf_test.c
    ${d_SOC_PLATFORM_DIR}/sboot/test/ed25519_ge_fe_ram.c
    ${d_SOC_PLATFORM_DIR}/sboot/test/sboot_ed25519_keypair_ram.c
    ${d_SOC_PLATFORM_DIR}/sboot/test/sboot_ed25519_ram.c
    ${d_SOC_PLATFORM_DIR}/sboot/test/sboot_ed25519_sign_ram.c
)

#--------------------------#
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_std_lib p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/swlib/std_lib/rtl_test_lib.c
    ${d_SOC_PLATFORM_DIR}/verification/swlib/std_lib/rtl_test_string.c
    ${d_SOC_PLATFORM_DIR}/verification/swlib/std_lib/rtl_test_sscanf.c
    # ${d_SOC_PLATFORM_DIR}/verification/swlib/std_lib/rtl_test_math.c
    # ${d_SOC_PLATFORM_DIR}/verification/swlib/std_lib/rtl_test_aes.c
)

#--------------------------#
ameba_target_sources_if(CONFIG_ADC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/adc/ameba_adc_test.c
)
ameba_target_sources_if(CONFIG_CAPTOUCH_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/captouch/ameba_captouch_test.c
)
ameba_target_sources_if(CONFIG_QDEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/qdecoder/rtl8721d_qdec_test.c
)

if (CONFIG_FT)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/ft_test/adc
    )
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/ft_test/adc/rtl8195a_ft_adc_dac.c
        ${d_SOC_PLATFORM_DIR}/verification/ft_test/adc/rtl8195a_dac_sin_23p84.c
        ${d_SOC_PLATFORM_DIR}/verification/ft_test/adc/rtl8195a_i2c_spi_dac.c
        ${d_SOC_PLATFORM_DIR}/verification/ft_test/adc/rtl8195a_spidac_sin_5p722k.c
        ${d_SOC_PLATFORM_DIR}/verification/ft_test/adc/rtl8710b_ft_test.c
    )
endif()

if (CONFIG_EQC)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/eqc
    )
    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
                ${d_SOC_PLATFORM_DIR}/verification/eqc/ameba_eqc.c
                ${d_SOC_PLATFORM_DIR}/verification/eqc/ameba_eqc_ext.c
                ${d_SOC_PLATFORM_DIR}/verification/eqc/ameba_flashclk_eqc.c
                ${d_SOC_PLATFORM_DIR}/verification/eqc/ameba_ft_adc_dac.c
            )
        elseif("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
                ${d_SOC_PLATFORM_DIR}/verification/eqc/ameba_eqc.c
            )
        endif()
    endif()
endif()

if (CONFIG_OLT)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${c_COMPONENT_DIR}/verification/mbed/targets/hal/rtl8730
        ${d_SOC_PLATFORM_DIR}/verification/olt_test
        ${d_SOC_PLATFORM_DIR}/verification/wifi
    )
    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
                ${d_SOC_PLATFORM_DIR}/verification/olt_test/ameba_olt_hp.c
                ${d_SOC_PLATFORM_DIR}/verification/olt_test/ameba_olt_ext_hp.c
                # ${d_SOC_PLATFORM_DIR}/verification/olt_test/ameba_copytask.c
                ${d_SOC_PLATFORM_DIR}/verification/olt_test/ameba_olt_ft_adc_dac.c
                ${d_SOC_PLATFORM_DIR}/verification/wifi/amebad2_wifi_olt.c
            )
        elseif("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
            ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
                ${d_SOC_PLATFORM_DIR}/verification/olt_test/ameba_olt_lp.c
            )
        elseif("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
                # ${d_SOC_PLATFORM_DIR}/verification/olt_test/ameba_copytask.c
                ${d_SOC_PLATFORM_DIR}/verification/olt_test/ameba_olt_ap.c
                ${d_SOC_PLATFORM_DIR}/verification/olt_test/ameba_olt_ext_ap.c
            )
        endif()
    endif()
endif()

if (CONFIG_QA)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/qa_test
    )
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/qa_test/ameba_qa.c
        ${d_SOC_PLATFORM_DIR}/verification/qa_test/ameba_gpio_qa_test.c
        ${d_SOC_PLATFORM_DIR}/verification/qa_test/ameba_ft_adc_dac_qa_test.c
        ${d_SOC_PLATFORM_DIR}/verification/qa_test/ameba_memory_qa_test.c
        ${d_SOC_PLATFORM_DIR}/verification/qa_test/ameba_wifi_qa_test.c
        ${d_SOC_PLATFORM_DIR}/verification/qa_test/ameba_adc_key_detect_qa_test.c
    )
endif()

if (CONFIG_CORNER)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/corner_test
    )
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/corner_test/ameba_corner_test.c
        ${d_SOC_PLATFORM_DIR}/verification/corner_test/ameba_corner_test_ext.c
        ${d_SOC_PLATFORM_DIR}/verification/corner_test/ameba_ft_adc_dac_corner_test.c
    )
endif()

if (CONFIG_EMC)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/emc
    )
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/emc/ameba_emc.c
        ${d_SOC_PLATFORM_DIR}/verification/emc/ameba_ft_adc_dac_emc.c
        ${d_SOC_PLATFORM_DIR}/verification/emc/ameba_memory_test_emc.c
        ${d_SOC_PLATFORM_DIR}/verification/emc/ameba_mipi_show_emc.c
    )
endif()

ameba_target_sources_if(CONFIG_SIMULATION src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/simulation/ameba_simulation.c
)

ameba_target_sources_if(CONFIG_CRYPTO_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/crypto/rtl8721d_crypto_test.c
)

ameba_target_sources_if(CONFIG_EFUSE_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/efuse/ameba_efuse_test.c
)

ameba_target_sources_if(CONFIG_GDMA_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/gdma/rtl8195a_gdma_test.c
    ${d_SOC_PLATFORM_DIR}/verification/gdma/rtl8195a_gdma_llp_test.c
)

ameba_target_sources_if(CONFIG_GPIO_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/gpio/ameba_gpio_test.c
)

ameba_target_sources_if(CONFIG_I2C_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/i2c/rtl8195a_i2c_test_sal.c
    ${d_SOC_PLATFORM_DIR}/verification/i2c/hal_i2c_test.c
)
if(CONFIG_PSRAM_VERIFY)
    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
                ${d_SOC_PLATFORM_DIR}/verification/psram/psram_pressure_test.c
            )
        elseif("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
                ${d_SOC_PLATFORM_DIR}/verification/psram/rtl8721d_psram_test.c
            )
        endif()
    endif()
endif()

if(CONFIG_AUDIO_VERIFY)
    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/verification/audio/rtl8721d_audio_test.c
        )
        if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
                ${d_SOC_PLATFORM_DIR}/verification/vad/vad_handler_example.c
				${d_SOC_PLATFORM_DIR}/verification/vad/ameba_vad_test.c
				${d_SOC_PLATFORM_DIR}/verification/vad/ameba_bt_vad_test.c
            )
        endif()
    endif()
endif()

ameba_target_sources_if(CONFIG_SDIO_DEVICE_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/sdio_device/rtl8195a_sdio_device_test.c
)

ameba_target_sources_if(CONFIG_SPI_COM_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/spi_communication/rtl8721d_ssi_test.c
    ${d_SOC_PLATFORM_DIR}/verification/spi_communication/hal_ssi_test.c
)

ameba_target_sources_if(CONFIG_SPIC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/spi_flash/rtl8721d_spi_flash_test.c
)

ameba_target_sources_if(CONFIG_RF_Interference_Flash_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/rf_interference/rf_interference_flash_test.c
)

ameba_target_sources_if(CONFIG_TIMER_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/timer/ameba_timer_test.c
    ${d_SOC_PLATFORM_DIR}/verification/timer/ameba_rtc_test.c
    ${d_SOC_PLATFORM_DIR}/verification/timer/ameba_pwm_test.c
    ${d_SOC_PLATFORM_DIR}/verification/timer/ameba_timer_capture_test.c
)

ameba_target_sources_if(CONFIG_UART_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/uart/rtl8721d_uart_test.c
    ${d_SOC_PLATFORM_DIR}/verification/uart/hal_uart_test.c
    ${d_SOC_PLATFORM_DIR}/verification/uart/rtl8721d_uart_cyclic_dma_test.c
)

if (CONFIG_LEDC_VERIFY)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/ledc
    )
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/ledc/ameba_ledc_test.c
    )
endif()

if (CONFIG_LCDC_VERIFY)
    ameba_target_include(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/lcdc
        ${d_SOC_PLATFORM_DIR}/verification/demo_code
        ${c_COMPONENT_DIR}/ui/ucgui/Config
        ${c_COMPONENT_DIR}/ui/ucgui/GUI/Core
        ${c_COMPONENT_DIR}/ui/ucgui/GUI/JPEG
        ${c_COMPONENT_DIR}/ui/ucgui/GUI/MultiLayer
        ${c_COMPONENT_DIR}/ui/ucgui/GUI/Widget
        ${c_COMPONENT_DIR}/ui/ucgui/GUI/WM
    )
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
        ${d_SOC_PLATFORM_DIR}/verification/lcdc/ameba_lcdc_test.c
        ${d_SOC_PLATFORM_DIR}/verification/lcdc/ameba_mipi_test.c
    )
endif()

ameba_target_sources_if(CONFIG_WDG_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/wdg/ameba_wdg_test.c
)

ameba_target_sources_if(CONFIG_TRNG_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/trng/ameba_trng_test.c
)

ameba_target_sources_if(CONFIG_IPC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/ipc/ipc_test.c
)

ameba_target_sources_if(CONFIG_MPU_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/mpu/mpu_test.c
)

ameba_target_sources_if(CONFIG_RSIP_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/rsip/rsip_test.c
)

ameba_target_sources_if(CONFIG_DHRYSTONE_TEST src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/dhrystone/dhrystone_test.c
)

ameba_target_sources_if(CONFIG_SBOOT_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/sboot/sboot_test.c
)

ameba_target_sources_if(CONFIG_CACHE_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/cache/rtl8721d_cache_test.c
)

ameba_target_sources_if(CONFIG_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/secure_test_ns.c
    ${d_SOC_PLATFORM_DIR}/verification/secure/cryptoSimsecure.c
)

ameba_target_sources_if(CONFIG_WFE_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/wfe/wfe_test.c
)

if(CONFIG_IR_VERIFY)
    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/verification/IR/ameba_ir_test.c
            ${d_SOC_PLATFORM_DIR}/verification/IR/ameba_ir_learn.c
        )
        if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_target_sources(src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
                ${d_SOC_PLATFORM_DIR}/verification/IR/protocol/ir_nec_protocol.c
            )
        endif()
    endif()
endif()

ameba_target_sources_if(CONFIG_PMC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/pmc/pmc_test_hp.c
)

ameba_target_sources_if(CONFIG_FTL_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/ftl/rtl8721d_ftl_test.c
)

ameba_target_sources_if(CONFIG_SDIOH_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/sdio_host/rtl8721d_sdio_host_test.c
)

ameba_target_sources_if(CONFIG_DDR_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/ddr/iot_ddr_test.c
)

ameba_target_sources_if(CONFIG_RSA_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/rsa/rsa_test.c
)

ameba_target_sources_if(CONFIG_ECDSA_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/ecdsa/ecdsa_test.c
)

ameba_target_sources_if(CONFIG_ED25519_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/ed25519/ed25519_test.c
)

ameba_target_sources_if(CONFIG_THERMAL_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/thermal/ameba_thermal_test.c
)

ameba_target_sources_if(CONFIG_CHIP_RST_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/chip_reset/ameba_reset_test.c
)

ameba_target_sources_if(CONFIG_DBG_TIMER_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/timer/ameba_dbg_timer_test.c
)

ameba_target_sources_if(CONFIG_SPINAND_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/spi_nand/ameba_spinand_test.c
)

ameba_target_sources_if(CONFIG_LOGUART_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/loguart/ameba_loguart_test.c
    ${d_SOC_PLATFORM_DIR}/verification/loguart/hal_loguart_test.c
)

ameba_target_sources_if(CONFIG_WIFI_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/wifi/amebad2_wifi_test.c
    ${d_SOC_PLATFORM_DIR}/verification/wifi/amebad2_wifi_test_lbk.c
)

ameba_target_sources_if(CONFIG_ZB_INT_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/zb_interrupt/ameba_zb_interrupt_test.c
)

ameba_target_sources_if(CONFIG_DBGPORT_ENABLE src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/ameba_debugport.c
)

ameba_target_sources_if(CONFIG_FLASH_RW_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/ameba_flash_rw_test.c
)

ameba_target_sources_if(CONFIG_MULTI_INT_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/ameba_irq_send.c
)

ameba_target_sources_if(CONFIG_Flash_AVL src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/flash_avl/ameba_spi_flash_qc_test.c
    ${d_SOC_PLATFORM_DIR}/verification/flash_avl/flash_avl_ameba_flashclk.c
)

ameba_target_sources_if(CONFIG_RF_Interference_Flash src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/rf_interference/rf_interference_flash_test.c
)
if(CONFIG_AMEBASMARTPLUS)
	ameba_target_sources_if(CONFIG_DIAGPRINTF_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification p_SCOPE interface
		${d_SOC_PLATFORM_DIR}/verification/print/ameba_print_test.c
	)
endif()
#--------------------------#
ameba_target_sources_if(CONFIG_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/log/ameba_log_test_secure.c
)

ameba_target_sources_if(CONFIG_AES_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/crypto/ameba_aes_ecb_test_secure.c
    ${d_SOC_PLATFORM_DIR}/verification/secure/crypto/ameba_aes_ecb_sim.c
)

ameba_target_sources_if(CONFIG_CRYPTO_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/crypto/ameba_crypto_test_secure.c
    # ${d_SOC_PLATFORM_DIR}/verification/secure/crypto/ameba_crc_test_secure.c
)

ameba_target_sources_if(CONFIG_GDMA_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/gdma/ameba_gdma_test_secure.c
    # ${d_SOC_PLATFORM_DIR}/verification/secure/gdma/ameba_gdma_llp_test_secure.c
)

ameba_target_sources_if(CONFIG_EFUSE_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/efuse/ameba_efuse_test_secure.c
)

ameba_target_sources_if(CONFIG_CACHE_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/cache/rtl8721d_cache_test_secure.c
)

ameba_target_sources_if(CONFIG_RDP_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/rdp/rdp_test.c
)

ameba_target_sources_if(CONFIG_WDG_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/wdg/ameba_wdg_test_secure.c
)

ameba_target_sources_if(CONFIG_TRNG_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/trng/ameba_trng_test_secure.c
)

ameba_target_sources_if(CONFIG_LEDC_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/ledc/ameba_ledc_test_secure.c
)

ameba_target_sources_if(CONFIG_IRDA_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/IR/protocol/ir_nec_protocol.c
    ${d_SOC_PLATFORM_DIR}/verification/secure/ir/ameba_ir_test_secure.c
)

ameba_target_sources_if(CONFIG_TIMER_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/timer/ameba_timer_test_secure.c
)

ameba_target_sources_if(CONFIG_LCDC_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/lcdc/ameba_lcdc_test_secure.c
)

ameba_target_sources_if(CONFIG_UART_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/uart/amebad2_uart_secure_test.c
    ${d_SOC_PLATFORM_DIR}/verification/secure/uart/amebad2_hal_uart_secure_test.c
)

ameba_target_sources_if(CONFIG_SPI_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/spi_communication/amebad2_spi_secure_test.c
    ${d_SOC_PLATFORM_DIR}/verification/secure/spi_communication/amebad2_hal_spi_secure_test.c
)

ameba_target_sources_if(CONFIG_I2C_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/i2c/amebad2_i2c_secure_test.c
    ${d_SOC_PLATFORM_DIR}/verification/secure/i2c/amebad2_hal_i2c_secure_test.c
)

ameba_target_sources_if(CONFIG_EDDSA_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/eddsa/ameba_eddsa_test_secure.c
)

ameba_target_sources_if(CONFIG_ECDSA_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/ecdsa/ameba_ecdsa_test_secure.c
)

ameba_target_sources_if(CONFIG_RSA_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/rsa/ameba_rsa_test_secure.c
)

ameba_target_sources_if(CONFIG_TZ_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/trustzone/ameba_trustzone_test_secure.c
)

ameba_target_sources_if(CONFIG_AUDIO_SEC_VERIFY src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/verification/secure/audio/ameba_audio_test_secure.c
    ${d_SOC_PLATFORM_DIR}/verification/secure/vad/ameba_vad_test_secure.c
    ${d_SOC_PLATFORM_DIR}/verification/secure/vad/ameba_bt_vad_test_secure.c
)
ameba_target_compile_options(src_${d_MCU_PROJECT_NAME}_test_verification_secure p_SCOPE interface
    -O0
)
#--------------------------#
