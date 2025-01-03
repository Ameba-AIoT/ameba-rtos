set(DIR ${TARGETDIR}/verification)
set(MODULE_IFLAGS ${DIR}./  ${BASEDIR}/component/application/ir)
set(CFGDIR ${TARGETDIR}/usrcfg)

set(CSRC)

if(CONFIG_ADC_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/adc/ameba_adc_test.c
    )
endif()

if(CONFIG_CAPTOUCH_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/captouch/ameba_captouch_test.c
    )
endif()

if(CONFIG_AF_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/af/af_test.c
    )
endif()

if(CONFIG_FT)
    list(
        APPEND MODULE_IFLAGS
        ${DIR}/ft_test/adc/
    )
    list(
        APPEND CSRC
        ${DIR}/ft_test/adc/rtl8195a_ft_adc_dac.c
        ${DIR}/ft_test/adc/rtl8195a_dac_sin_23p84.c
        ${DIR}/ft_test/adc/rtl8195a_i2c_spi_dac.c
        ${DIR}/ft_test/adc/rtl8195a_spidac_sin_5p722k.c
        ${DIR}/ft_test/adc/rtl8710b_ft_test.c
    )
endif()

if(CONFIG_EQC)
    list(
        APPEND MODULE_IFLAGS
        ${TARGETDIR}/fwlib/include
        ${DIR}/eqc
    )
    list(
        APPEND CSRC
        ${DIR}/eqc/ameba_eqc.c
        ${DIR}/eqc/ameba_eqc_ext.c
        ${DIR}/eqc/ameba_ft_adc_dac.c
    )

endif()

if(CONFIG_QA)
    list(
        APPEND MODULE_IFLAGS
        ${DIR}/qa_test
    )
    list(
        APPEND CSRC
        ${DIR}/qa_test/ameba_qa.c
        ${DIR}/qa_test/ameba_gpio_qa_test.c
        ${DIR}/qa_test/ameba_ft_adc_dac_qa_test.c
        ${DIR}/qa_test/ameba_ft_adc_dac_dmic_qa_test.c
        ${DIR}/qa_test/ameba_memory_qa_test.c
        ${DIR}/qa_test/ameba_wifi_qa_test.c
        ${DIR}/qa_test/ameba_adc_key_detect_qa_test.c
    )
endif()

if(CONFIG_OLT)
    list(
        APPEND MODULE_IFLAGS
        ${DIR}/olt_test
    )
    list(
        APPEND CSRC
        ${DIR}/olt_test/ameba_olt.c
        ${DIR}/olt_test/ameba_olt_ext.c
        ${DIR}/olt_test/ameba_olt_ft_adc_dac.c
    )
endif()

if(CONFIG_CORNER)
    list(
        APPEND MODULE_IFLAGS
        ${DIR}/corner_test
        ${TARGETDIR}/fwlib/include
    )
    list(
        APPEND CSRC
        ${DIR}/corner_test/ameba_corner_test.c
        ${DIR}/corner_test/ameba_corner_test_ext.c
        ${DIR}/corner_test/ameba_ft_adc_dac_corner_test.c
        ${DIR}/corner_test/ameba_pmc_corner_test.c
        ${DIR}/corner_test/ameba_thermal_corner_test.c
    )
endif()

if(CONFIG_CRYPTO_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/crypto/rtl8721d_crypto_test.c
        ${DIR}/crypto/cryptoSim.c
    )
endif()

if(CONFIG_SIMULATION)
    list(
        APPEND CSRC
        ${DIR}/simulation/ameba_simulation.c
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
        ${DIR}/gdma/rtl8195a_gdma_test.c
        ${DIR}/gdma/rtl8195a_gdma_llp_test.c
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

if(CONFIG_PSRAM_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/psram/psram_pressure_test.c
    )
endif()

if(CONFIG_I2S_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/i2s/rtl8195a_i2s_test.c
    )
endif()

if(CONFIG_AUDIO_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/audio/rtl8721d_audio_test.c
        ${DIR}/vad/ameba_vad_test.c
        ${DIR}/vad/vad_handler_example.c
        #${DIR}/audio/rtl8721d_audio_llp_test.c
        #${DIR}/audio/rtl8721d_audio_cyclic_dma_test.c
        #${DIR}/audio/rtl8721d_audio_opus_test.c
    )
endif()

if(CONFIG_SDIO_DEVICE_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/sdio_device/rtl8195a_sdio_device_test.c
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
        #${DIR}/spi_flash/rtl8721d_spi_flash_qc_test.c
        #${DIR}/spi_flash/flash_runcode_test.c
        #${DIR}/spi_flash/rtl8721d_flash_N25Q00AA_patch.c
    )
endif()

if(CONFIG_TIMER_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/timer/ameba_timer_test.c
        ${DIR}/timer/ameba_rtc_test.c
        ${DIR}/timer/ameba_pwm_test.c
    )
endif()

if(CONFIG_PRS_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/ups/ameba_ups_test.c
    )
endif()

if(CONFIG_UART_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/uart/rtl8721d_uart_test.c
        ${DIR}/uart/hal_uart_test.c
        ${DIR}/uart/rtl8721d_uart_cyclic_dma_test.c
    )
endif()

if(CONFIG_LEDC_VERIFY)
    list(
        APPEND MODULE_IFLAGS
        ${DIR}/ledc
    )
    list(
        APPEND CSRC
        ${DIR}/ledc/ameba_ledc_test.c
    )
endif()

if(CONFIG_LCDC_VERIFY)
    list(
        APPEND MODULE_IFLAGS
        ${DIR}/lcdc/
        ${DIR}/lcdc/demo_code
        ${BASEDIR}/component/ui/ucgui/Config/
        ${BASEDIR}/component/ui/ucgui/GUI/Core/
        ${BASEDIR}/component/ui/ucgui/GUI/JPEG/
        ${BASEDIR}/component/ui/ucgui/GUI/MultiLayer/
        ${BASEDIR}/component/ui/ucgui/GUI/Widget/
        ${BASEDIR}/component/ui/ucgui/GUI/WM/
    )
    list(
        APPEND CSRC
        ${DIR}/lcdc/ameba_lcdc_test.c
        ${DIR}/lcdc/ameba_mipi_test.c
        #${DIR}/lcdc/bsp_mcu_lcd.c
        #${DIR}/lcdc/bsp_rgb_lcd.c
        #${DIR}/lcdc/bsp_led_board.c
        #${DIR}/lcdc/bsp_lcdc_font.c
        #${DIR}/lcdc/gt9147.c
        #${DIR}/lcdc/ctiic.c
        #${DIR}/lcdc/touch.c

        #${DIR}/lcdc/demo_code/clock_menu.c
        #${DIR}/lcdc/demo_code/grad_menu.c
        #${DIR}/lcdc/demo_code/gui_cus_parts.c
        #${DIR}/lcdc/demo_code/light_menu.c
        #${DIR}/lcdc/demo_code/main_menu.c
        #${DIR}/lcdc/demo_code/realtek_demo.c
        #${DIR}/lcdc/demo_code/start_menu.c
        #${DIR}/lcdc/demo_code/tempre_menu.c
        #${DIR}/lcdc/demo_code/wave_menu.c
        #${DIR}/lcdc/demo_code/clock_pic.c
        #${DIR}/lcdc/demo_code/gradienter_pic.c
        #${DIR}/lcdc/demo_code/light_off_pic.c
        #${DIR}/lcdc/demo_code/light_on_pic.c
        #${DIR}/lcdc/demo_code/light_pic.c
        #${DIR}/lcdc/demo_code/realtek_logo.c
        #${DIR}/lcdc/demo_code/return_btn_pic.c
        #${DIR}/lcdc/demo_code/tempre_pic.c
        #${DIR}/lcdc/demo_code/wave_pic.c
    )
endif()

if(CONFIG_WDG_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/wdg/ameba_wdg_test.c
    )
endif()

if(CONFIG_TRNG_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/trng/ameba_trng_test.c
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

if(CONFIG_SBOOT_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/sboot/sboot_test.c
    )
endif()

if(CONFIG_CACHE_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/cache/rtl8721d_cache_test.c
    )
endif()

if(CONFIG_SEC_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/secure/secure_test_ns.c
        ${DIR}/secure/cryptoSimsecure.c
    )
endif()

if(CONFIG_WFE_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/wfe/wfe_test.c
    )
endif()

if(CONFIG_IR_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/IR/ameba_ir_test.c
        ${DIR}/IR/ameba_ir_learn.c
        ${DIR}/IR/protocol/ir_nec_protocol.c
    )
endif()

if(CONFIG_PMC_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/pmc/pmc_test_km4.c
    )
endif()

if(CONFIG_FTL_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/ftl/rtl8721d_ftl_test.c
    )
endif()

if(CONFIG_SDIOH_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/sdio_host/rtl8721d_sdio_host_test.c
    )
endif()

if(CONFIG_DDR_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/ddr/iot_ddr_test.c
    )
endif()

if(CONFIG_RSA_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/rsa/rsa_test.c
    )
endif()

if(CONFIG_ECDSA_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/ecdsa/ecdsa_test.c
    )
endif()

if(CONFIG_ED25519_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/ed25519/ed25519_test.c
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

if(CONFIG_LOGUART_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/loguart/ameba_loguart_test.c
        ${DIR}/loguart/hal_loguart_test.c
    )
endif()

if(CONFIG_WIFI_VERIFY)
    list(
        APPEND MODULE_IFLAGS
        ${BASEDIR}/component/wifi/driver
        ${BASEDIR}/component/wifi/driver/halmac
        ${BASEDIR}/component/wifi/driver/halcommon
        ${BASEDIR}/component/wifi/driver/halbtc
        ${BASEDIR}/component/wifi/driver/phl
        ${BASEDIR}/component/wifi/driver/rtl8720e
        ${BASEDIR}/component/wifi/driver/rtl8720e_rom
        ${BASEDIR}/component/wifi/wifi_fw/amebalite/include
        ${BASEDIR}/component/wifi/driver/phy_g6/bb
        ${BASEDIR}/component/wifi/driver/phy_g6/rf
    )
    list(
        APPEND CSRC
        ${DIR}/wifi/amebalite_wifi_test.c
        ${DIR}/wifi/amebalite_wifi_test_lbk.c
    )
endif()

if(CONFIG_ZB_INT_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/zb_interrupt/ameba_zb_interrupt_test.c
    )
endif()

if(CONFIG_DBGPORT_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/ameba_debugport.c
    )
endif()

if(CONFIG_MEMSHARE_VERIFY)
    list(
        APPEND CSRC
        ${DIR}/memory_share/ameba_memory_share_test.c
    )
endif()

if(CONFIG_FLASH_AVL)
    list(
        APPEND CSRC
        ${DIR}/flash_avl/ameba_spi_flash_qc_test.c
    )
endif()

if(CSRC)
    ameba_internal_library(test_verification)
    target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
    target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
endif()
