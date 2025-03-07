if (__INCLUDED_BOOTLOADER_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_BOOTLOADER_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_bootloader INTERFACE)

#--------------------------#
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    if("${d_MCU_PROJECT_NAME}" STREQUAL "hp")
		if(CONFIG_AMEBASMART)
			ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bootloader p_SCOPE interface
				${d_SOC_PLATFORM_DIR}/swlib/log.c
                ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_fault_handle.c
			)
		endif()
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bootloader p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/bootloader/bootloader_hp.c
            ${d_SOC_PLATFORM_DIR}/bootloader/boot_trustzone_hp.c
            ${d_SOC_PLATFORM_DIR}/bootloader/boot_security_hp.c
            ${d_SOC_PLATFORM_DIR}/bootloader/boot_ota_hp.c
            ${d_SOC_PLATFORM_DIR}/usrcfg/ameba_boot_trustzonecfg.c
            ${d_SOC_PLATFORM_DIR}/usrcfg/ameba_bootcfg.c
            ${d_SOC_PLATFORM_DIR}/usrcfg/ameba_flashcfg.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_flashclk.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_nandflash.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_ddrphy.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_rxi316_dram_init.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_hp/ameba_psram.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_clk.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_swr.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_otp.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_swr_calib.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_pll.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_reset.c
            ${d_SOC_PLATFORM_DIR}/fwlib/ram_common/ameba_ipc_api.c
        )
        ameba_target_sources_if(CONFIG_COMPRESS_OTA_IMG src_${d_MCU_PROJECT_NAME}_bootloader p_SCOPE interface
            ${c_COMPONENT_DIR}/soc/common/lzma/ameba_boot_lzma.c
            ${c_COMPONENT_DIR}/soc/common/lzma/LzmaDec.c
        )

        ameba_target_include(src_${d_MCU_PROJECT_NAME}_bootloader p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/swlib
            ${d_SOC_PLATFORM_DIR}/lib/bootloader
            ${c_COMPONENT_DIR}/soc/common/crashdump/include
            ${c_COMPONENT_DIR}/soc/common/lzma
        )
    elseif("${d_MCU_PROJECT_NAME}" STREQUAL "lp")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_bootloader p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/bootloader/bootloader_lp.c
        )
    endif()
endif()

#--------------------------#
