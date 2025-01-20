if (__INCLUDED_CHIPINFO_BUILD_CMAKE__)
    return()
else()
    set(__INCLUDED_CHIPINFO_BUILD_CMAKE__ TRUE)
endif()

add_library(src_${d_MCU_PROJECT_NAME}_chipinfo_lib INTERFACE)

#--------------------------#
if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_target_sources(src_${d_MCU_PROJECT_NAME}_chipinfo_lib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/lib/ram_common/ameba_chipinfo_lib.c
            ${d_SOC_PLATFORM_DIR}/lib/ram_common/ameba_rom_patch.c
    )
    if("${d_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_target_sources(src_${d_MCU_PROJECT_NAME}_chipinfo_lib p_SCOPE interface
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_hp
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_systimer_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_assert_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_syscfg_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_backup_reg.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_tim_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_clk_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_pinmux_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_rcc_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_uart_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_gpio_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_flash_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_common/ameba_wdg.c

            ${d_SOC_PLATFORM_DIR}/fwlib/rom_hp/ameba_aes.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_hp/ameba_crypto_rom.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_hp/ameba_md5.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_hp/ameba_rsa.c
            ${d_SOC_PLATFORM_DIR}/fwlib/rom_hp/ameba_sha.c
        )
    endif()
endif()
#--------------------------#
