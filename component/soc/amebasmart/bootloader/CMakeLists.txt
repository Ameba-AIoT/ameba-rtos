##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin


if("${c_MCU_PROJECT_NAME}" STREQUAL "hp")
    ameba_list_append(private_sources
        bootloader_hp.c
        boot_trustzone_hp.c
        boot_security_hp.c
        boot_ota_hp.c
        ${c_CMPT_USRCFG_DIR}/ameba_boot_trustzonecfg.c
        ${c_CMPT_USRCFG_DIR}/ameba_bootcfg.c
        ${c_CMPT_USRCFG_DIR}/ameba_flashcfg.c
        ${c_CMPT_FWLIB_DIR}/ram_hp/ameba_flashclk.c
        ${c_CMPT_FWLIB_DIR}/ram_hp/ameba_nandflash.c
        ${c_CMPT_FWLIB_DIR}/ram_hp/ameba_ddrphy.c
        ${c_CMPT_FWLIB_DIR}/ram_hp/ameba_rxi316_dram_init.c
        ${c_CMPT_FWLIB_DIR}/ram_hp/ameba_psram.c
        ${c_CMPT_FWLIB_DIR}/ram_common/ameba_clk.c
        ${c_CMPT_FWLIB_DIR}/ram_common/ameba_swr.c
        ${c_CMPT_FWLIB_DIR}/ram_common/ameba_otp.c
        ${c_CMPT_FWLIB_DIR}/ram_common/ameba_swr_calib.c
        ${c_CMPT_FWLIB_DIR}/ram_common/ameba_pll.c
        ${c_CMPT_FWLIB_DIR}/ram_common/ameba_reset.c
        ${c_CMPT_FWLIB_DIR}/ram_common/ameba_ipc_api.c
        ${c_CMPT_FWLIB_DIR}/ram_common/ameba_fault_handle.c
        ${c_CMPT_SWLIB_DIR}/log.c
    )
    ameba_list_append_if(CONFIG_COMPRESS_OTA_IMG private_sources
        ${c_COMPONENT_DIR}/soc/common/lzma/ameba_boot_lzma.c
        ${c_COMPONENT_DIR}/soc/common/lzma/LzmaDec.c
    )

    ameba_list_append(private_includes
        ${c_CMPT_SWLIB_DIR}
        ${c_CMPT_SOC_DIR}/lib/bootloader
        ${c_COMPONENT_DIR}/soc/common/crashdump/include
        ${c_COMPONENT_DIR}/soc/common/lzma
    )
elseif("${c_MCU_PROJECT_NAME}" STREQUAL "lp")
    ameba_list_append(private_sources
        bootloader_lp.c
    )
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/soc
ameba_add_internal_library(bootloader_internal
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