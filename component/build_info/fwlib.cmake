if (__INCLUDED_FWLIB_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_FWLIB_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_fwlib INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_fwlib_secure INTERFACE)
add_library(src_${d_MCU_PROJECT_NAME}_fwlib_xlat_tables INTERFACE)

#--------------------------#
if(CONFIG_AMEBASMARTPLUS)
	if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
		ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
			${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_sema_rom.c
		)
    else()
        ameba_target_include(src_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/rom/include
        )
	endif()
endif()

if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
		if(CONFIG_AMEBASMART)
			ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib_secure p_SCOPE interface
				${d_SOC_PLATFORM_DIR}/swlib/log.c
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_fault_handle.c
			)
		endif()
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib_secure p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_trustzone.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_codec.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_sport.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_vad.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_vad_bt.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_tim.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_otp.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_uart.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_spi.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_i2c.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_gdma_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ir.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ledc.c
        )

		if(CONFIG_AMEBASMART)
			ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
				${d_SOC_PLATFORM_DIR}/swlib/log.c
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_fault_handle.c
			)
		endif()
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_app_start.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_pmc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_system.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_pinmap.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_lcdc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_mipi.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_sdio_host.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_sd.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_psram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_ddrphy.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_rxi316_dram_init.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_codec.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_sport.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_audio_clock.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_ipc_toap.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_vad.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_vad_bt.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_vad_handler.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_spi.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_i2c.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_rtc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_uart.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_tim.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_adc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_comparator.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_flash_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_pll.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ir.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ipc_api.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_captouch.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_gdma_memcpy.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_bor.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_gdma_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ipc_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_mpu_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ledc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_reset.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_thermal.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_otp.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_clk.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_aontimer.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_wakepin.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_arch.c

            ${d_SOC_PLATFORM_DIR}/swlib/locks.c
            ${d_SOC_PLATFORM_DIR}/swlib/sscanf_minimal.c
            ${c_CMPT_USRCFG_DIR}/ameba_pinmapcfg.c
            ${c_CMPT_USRCFG_DIR}/ameba_flashcfg.c
            ${c_CMPT_USRCFG_DIR}/ameba_sleepcfg.c
            ${BASEDIR}/component/soc/common/rtk_assert/rtk_assert.c
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
		if(CONFIG_AMEBASMART)
			ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
				${d_SOC_PLATFORM_DIR}/swlib/log.c
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_fault_handle.c
			)
		endif()
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_lp/ameba_app_start.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_lp/ameba_system.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_lp/ameba_pmc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_lp/ameba_lpcnp.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_lp/ameba_lpcap.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_ddrphy.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_rxi316_dram_init.c
            # ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_clk.c

            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_pll.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ipc_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ipc_api.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_tim.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_rtc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_reset.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_thermal.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_mpu_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_i2c.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_otp.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_swr.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_swr_calib.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_adc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_comparator.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_aontimer.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_wakepin.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_arch.c

            ${c_CMPT_USRCFG_DIR}/ameba_sleepcfg.c
            ${d_SOC_PLATFORM_DIR}/swlib/locks.c
            ${d_SOC_PLATFORM_DIR}/swlib/sscanf_minimal.c
            ${BASEDIR}/component/soc/common/rtk_assert/rtk_assert.c
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/arm_gic.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/cstub.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/system_sheipa.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/amebaap_vector.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/ameba_app_start.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/traps.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_peripheral/ameba_loguart.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/psci.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/smp.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_peripheral/ameba_pmc_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_peripheral/ameba_delay.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/ameba_pmc.c

            ${d_SOC_PLATFORM_DIR}/misc/ameba_pmu.c

            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ipc_api.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ipc_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_gdma_memcpy.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_gdma_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_adc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_captouch.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_comparator.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_i2c.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ir.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ledc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_rtc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_spi.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_thermal.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_tim.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_uart.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_flash_ram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_pll.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_otp.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_reset.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_aontimer.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_wakepin.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_bor.c

            ${c_CMPT_USRCFG_DIR}/ameba_flashcfg.c
            ${c_CMPT_USRCFG_DIR}/ameba_sleepcfg.c

            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_mipi.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_lcdc.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_sd.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_sdio_host.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_codec.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_sport.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_audio_clock.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_vad.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_vad_bt.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_system.c

            ${d_SOC_PLATFORM_DIR}/swlib/log.c
            ${d_SOC_PLATFORM_DIR}/swlib/sscanf_minimal.c
            ${BASEDIR}/component/soc/common/rtk_assert/rtk_assert.c
        )
        ameba_target_sources_if(CONFIG_AS_INIC_AP src_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/misc/ameba_ota.c
        )
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/startup.S
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/vectors_table.S
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/smccc_call.S
            ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/cache-v7.S
            # ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/translation_table.S
        )
    endif()
else()

endif()
#--------------------------#
ameba_target_include(src_${d_MCU_PROJECT_NAME}_fwlib_xlat_tables p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/xlat_table
)
ameba_target_sources(src_${d_MCU_PROJECT_NAME}_fwlib_xlat_tables p_SCOPE interface
    ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/xlat_table/xlat_tables_context.c
    ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/xlat_table/xlat_tables_core.c
    ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/xlat_table/xlat_tables_utils.c
    ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/xlat_table/xlat_tables_arch.c
    ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/ameba_xlat_table.c
    ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/xlat_table/enable_mmu.S
    ${d_SOC_PLATFORM_DIR}/fwlib/ap_core/xlat_table/cache_helpers.S
)
#--------------------------#
