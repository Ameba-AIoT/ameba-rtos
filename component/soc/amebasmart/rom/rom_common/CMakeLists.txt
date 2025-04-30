##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#------------------------------------------------------------------#
# Component public part, user config begin(DO NOT remove this line)

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

ameba_list_append(private_includes
    ${c_MBEDTLS_DIR}/include
)
ameba_list_append(private_sources
    ameba_assert_rom.c
    ameba_backup_reg.c
    ameba_clk_rom.c
    ameba_delay_rom.c
    ameba_flash_rom.c
    ameba_gpio_rom.c
    ameba_loguart_rom.c
    ameba_pinmux_rom.c
    ameba_rcc_rom.c
    ameba_shell_rom.c
    ameba_syscfg_rom.c
    ameba_systimer_rom.c
    ameba_tim_rom.c
    ameba_uart_rom.c
    ameba_wdg.c
)

ameba_list_append_ifnot(CONFIG_ARM_CORE_CA32 private_sources
    ameba_monitor_rom.c
)

# Component private part, user config end(DO NOT remove this line)
#------------------------------------------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

if(${c_CURRENT_IMAGE_IS_ROM})
    ameba_add_internal_library(rom_common
        p_SOURCES
            ${private_sources}
        p_INCLUDES
            ${private_includes}
        p_DEFINITIONS
            ${private_definitions}
        p_COMPILE_OPTIONS
            ${private_compile_options}
    )
    ameba_library_section_rename(${c_CURRENT_TARGET_NAME} p_TO_ROM)
else()
    ameba_add_external_tmp_library(rom_common
        p_SOURCES
            ${private_sources}
        p_INCLUDES
            ${private_includes}
        p_DEFINITIONS
            ${private_definitions}
        p_COMPILE_OPTIONS
            ${private_compile_options}
    )
endif()
##########################################################################################
