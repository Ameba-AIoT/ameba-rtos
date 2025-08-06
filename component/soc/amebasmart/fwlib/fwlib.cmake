##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)

if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
    ameba_list_append(public_includes
        include
        ${c_CMPT_SWLIB_DIR}
        ${c_COMPONENT_DIR}/soc/common/include
    )

    if("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
        ameba_list_append(public_includes
            ap_core
        )
    endif()
endif()

# Component public part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
ameba_global_library(${public_libraries}) #default: whole-achived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------------------------------------------#
# Component private part, user config begin(DO NOT remove this line)
if(${c_CURRENT_IMAGE_TYPE} STREQUAL "image1")
    ameba_list_append(private_sources
        ram_hp/ameba_flashclk.c
        ram_hp/ameba_nandflash.c
        ram_hp/ameba_ddrphy.c
        ram_hp/ameba_rxi316_dram_init.c
        ram_hp/ameba_psram.c
        ram_common/ameba_clk.c
        ram_common/ameba_swr.c
        ram_common/ameba_otp.c
        ram_common/ameba_swr_calib.c
        ram_common/ameba_pll.c
        ram_common/ameba_reset.c
        ram_common/ameba_ipc_api.c
        ram_common/ameba_fault_handle.c
    )
elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "image2")
    ameba_list_append(private_sources
        ram_common/ameba_adc.c
        ram_common/ameba_aontimer.c
        ram_common/ameba_comparator.c
        ram_common/ameba_gdma_ram.c
        ram_common/ameba_ipc_api.c
        ram_common/ameba_ipc_ram.c
        ram_common/ameba_ir.c
        ram_common/ameba_i2c.c
        ram_common/ameba_ledc.c
        ram_common/ameba_otp.c
        ram_common/ameba_pll.c
        ram_common/ameba_reset.c
        ram_common/ameba_rtc.c
        ram_common/ameba_thermal.c
        ram_common/ameba_tim.c
        ram_common/ameba_wakepin.c

        ${c_CMPT_SWLIB_DIR}/log.c
        ${c_CMPT_SWLIB_DIR}/sscanf_minimal.c
        ${c_COMPONENT_DIR}/soc/common/rtk_assert/rtk_assert.c
    )

    if(CONFIG_AMEBASMART OR CONFIG_AMEBASMARTPLUS)
        if("${c_MCU_PROJECT_NAME}" STREQUAL "hp")
            ameba_list_append(private_sources
                ram_hp/ameba_trustzone.c
                ram_hp/ameba_codec.c
                ram_hp/ameba_sport.c
                ram_hp/ameba_vad.c
                ram_hp/ameba_vad_bt.c
                ram_hp/ameba_app_start.c
                ram_hp/ameba_pmc.c
                ram_hp/ameba_system.c
                ram_hp/ameba_pinmap.c
                ram_hp/ameba_lcdc.c
                ram_hp/ameba_mipi.c
                ram_hp/ameba_sdio_host.c
                ram_hp/ameba_sd.c
                ram_hp/ameba_psram.c
                ram_hp/ameba_ddrphy.c
                ram_hp/ameba_rxi316_dram_init.c
                ram_hp/ameba_codec.c
                ram_hp/ameba_sport.c
                ram_hp/ameba_audio_clock.c
                ram_hp/ameba_ipc_toap.c
                ram_hp/ameba_vad.c
                ram_hp/ameba_vad_bt.c
                ram_hp/ameba_vad_handler.c

                ram_common/ameba_uart.c
                ram_common/ameba_spi.c
                ram_common/ameba_flash_ram.c
                ram_common/ameba_captouch.c
                ram_common/ameba_gdma_memcpy.c
                ram_common/ameba_bor.c
                ram_common/ameba_mpu_ram.c
                ram_common/ameba_thermal.c
                ram_common/ameba_clk.c
                ram_common/ameba_arch.c
                ram_common/ameba_fault_handle.c

                ${c_CMPT_SWLIB_DIR}/locks.c

            )
        elseif("${c_MCU_PROJECT_NAME}" STREQUAL "lp")
            ameba_list_append(private_sources
                ram_lp/ameba_app_start.c
                ram_lp/ameba_system.c
                ram_lp/ameba_pmc.c
                ram_lp/ameba_lpcnp.c
                ram_lp/ameba_lpcap.c
                ram_hp/ameba_ddrphy.c
                ram_hp/ameba_rxi316_dram_init.c
                # ram_hp/ameba_clk.c

                ram_common/ameba_thermal.c
                ram_common/ameba_mpu_ram.c
                ram_common/ameba_swr.c
                ram_common/ameba_swr_calib.c
                ram_common/ameba_arch.c
                ram_common/ameba_fault_handle.c

                ${c_CMPT_SWLIB_DIR}/locks.c
            )
        elseif("${c_MCU_PROJECT_NAME}" STREQUAL "ap")
            ameba_list_append(private_sources
                ap_core/arm_gic.c
                ap_core/cstub.c
                ap_core/system_sheipa.c
                ap_core/amebaap_vector.c
                ap_core/ameba_app_start.c
                ap_core/traps.c
                ap_peripheral/ameba_loguart.c
                ap_core/psci.c
                ap_core/smp.c
                ap_peripheral/ameba_pmc_ram.c
                ap_peripheral/ameba_delay.c
                ap_core/ameba_pmc.c

                ram_common/ameba_gdma_memcpy.c
                ram_common/ameba_captouch.c
                ram_common/ameba_thermal.c
                ram_common/ameba_flash_ram.c
                ram_common/ameba_bor.c
                ram_common/ameba_uart.c
                ram_common/ameba_spi.c

                ram_hp/ameba_mipi.c
                ram_hp/ameba_lcdc.c
                ram_hp/ameba_sd.c
                ram_hp/ameba_sdio_host.c
                ram_hp/ameba_codec.c
                ram_hp/ameba_sport.c
                ram_hp/ameba_audio_clock.c
                ram_hp/ameba_vad.c
                ram_hp/ameba_vad_bt.c
                ram_hp/ameba_system.c

                ${c_CMPT_MISC_DIR}/ameba_pmu.c
            )
            ameba_list_append_if(CONFIG_CORE_AS_AP private_sources
                ${c_CMPT_MISC_DIR}/ameba_ota.c
            )
            ameba_list_append(private_sources
                ap_core/startup.S
                ap_core/vectors_table.S
                ap_core/smccc_call.S
                ap_core/cache-v7.S
                # ap_core/translation_table.S
                ap_core/xlat_table/xlat_tables_context.c
                ap_core/xlat_table/xlat_tables_core.c
                ap_core/xlat_table/xlat_tables_utils.c
                ap_core/xlat_table/xlat_tables_arch.c
                ap_core/ameba_xlat_table.c
                ap_core/xlat_table/enable_mmu.S
                ap_core/xlat_table/cache_helpers.S
            )

            #FIXME: Here cannot add ap_core/xlat_table to private_includes for some header file conflict with toolchain
            set_source_files_properties(ap_core/ameba_xlat_table.c PROPERTIES INCLUDE_DIRECTORIES
                ${CMAKE_CURRENT_SOURCE_DIR}/ap_core/xlat_table
            )
        endif()
    endif()
endif()

# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For open-source component, always build from source
ameba_add_internal_library(fwlib
    p_SOURCES
        ${private_sources}
    p_INCLUDES
        ${private_includes}
    p_DEFINITIONS
        ${private_definitions}
    p_COMPILE_OPTIONS
        ${private_compile_options}
)
##########################################################################################