##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_definitions})
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

ameba_list_append(private_sources
    ameba_codec.c
    ameba_sport.c
    ameba_vad.c
    ameba_vad_bt.c
    ameba_spi.c
    ameba_uart.c

    ameba_system.c
    ameba_pinmap.c
    ameba_lcdc.c
    ameba_mipi.c
    ameba_sdio_host.c
    ameba_sd.c
    ameba_audio_clock.c
    ameba_ipc_toap.c
    ameba_vad.c
    ameba_vad_bt.c
    ameba_vad_handler.c
    ameba_flashclk.c
    ameba_nandflash.c
)

ameba_list_append_ifnot(CONFIG_ARM_CORE_CA32 private_sources
    ameba_app_start.c
    ameba_trustzone.c
    ameba_pmc.c
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/soc
ameba_add_internal_library(fwlib_hp
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
