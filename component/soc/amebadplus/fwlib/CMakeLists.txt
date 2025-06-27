##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)

ameba_list_append(public_includes
    include
    ${c_COMPONENT_DIR}/soc/common/include
)

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
        ram_km4/ameba_flashclk.c
        ram_common/ameba_psram.c
        ram_common/ameba_clk.c
        ram_common/ameba_ldo.c
        ram_common/ameba_reset.c
        ram_common/ameba_pll.c
        ram_common/ameba_fault_handle.c
    )
elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "image2")
    ameba_list_append(private_sources
        ${c_COMPONENT_DIR}/soc/common/rtk_assert/rtk_assert.c
        ram_${c_MCU_PROJECT_NAME}/ameba_app_start.c
        ram_common/ameba_adc.c
        ram_common/ameba_arch.c
        ram_common/ameba_captouch.c
        ram_common/ameba_comparator.c
        ram_common/ameba_flash_ram.c
        ram_common/ameba_fault_handle.c
        ram_common/ameba_gdma_ram.c
        ram_common/ameba_i2c.c
        ram_common/ameba_ipc_api.c
        ram_common/ameba_ipc_ram.c
        ram_common/ameba_keyscan.c
        ram_common/ameba_mpu_ram.c
        ram_common/ameba_pll.c
        ram_common/ameba_psram.c
        ram_common/ameba_qspi.c
        ram_common/ameba_reset.c
        ram_common/ameba_rtc.c
        ram_common/ameba_rtc_io.c
        ram_common/ameba_sdio.c
        ram_common/ameba_spi.c
        ram_common/ameba_tim.c
        ram_common/ameba_uart.c
        ram_common/ameba_ppe.c
        ram_common/ameba_wakepin.c
    )

    if("${c_MCU_PROJECT_NAME}" STREQUAL "km0")
        ameba_list_append(private_sources
            ram_${c_MCU_PROJECT_NAME}/ameba_npcap.c
            ram_${c_MCU_PROJECT_NAME}/ameba_pinmap.c
        )
    elseif("${c_MCU_PROJECT_NAME}" STREQUAL "km4")
        ameba_list_append(private_sources
            ram_${c_MCU_PROJECT_NAME}/ameba_data_flash.c
            ram_${c_MCU_PROJECT_NAME}/ameba_data_flashclk.c
            ram_common/ameba_audio_clock.c
            ram_common/ameba_bor.c
            ram_common/ameba_clk.c
            ram_common/ameba_codec.c
            ram_common/ameba_gdma_memcpy.c
            ram_common/ameba_ir.c
            ram_common/ameba_ledc.c
            ram_common/ameba_sport.c
            ram_common/ameba_ups.c
        )
    endif()
elseif(${c_CURRENT_IMAGE_TYPE} STREQUAL "image3")
    ameba_list_append(private_sources
        ram_km4/ameba_trustzone.c
        ram_common/ameba_gdma_ram.c
        ram_common/ameba_codec.c
        ram_common/ameba_sport.c
        ram_common/ameba_ledc.c
        ram_common/ameba_uart.c
        ram_common/ameba_spi.c
        ram_common/ameba_i2c.c
        ram_common/ameba_tim.c
        ram_common/ameba_rtc.c
        ram_common/ameba_adc.c
        ram_common/ameba_comparator.c
        ram_common/ameba_keyscan.c
    )
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