##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/rtk_coex_wl_chip/
    ${CMAKE_CURRENT_SOURCE_DIR}/rtk_coex_ext_chip/
)

set(need_external_lib OFF)

if(CONFIG_MP_INCLUDED)
    if(NOT CONFIG_MP_SHRINK)
        set(target_name coex_ext_mp)
    endif()
else()
    set(target_name coex_ext)
endif()

if (CONFIG_COEX_EXT_CHIP_SUPPORT AND NOT CONFIG_MP_SHRINK)
    set(need_external_lib ON)
endif()

## Differentiated configuration based on SoC types
ameba_list_append_if(need_external_lib public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_${target_name}.a
)

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

if(NOT CONFIG_MP_SHRINK)
    ameba_list_append(private_sources
        rtk_coex_ext_chip/rtw_coex_ext_chip.c
        rtw_coex_action_dual_chip.c
        rtw_coex_api_ext.c
        rtw_coex_api_wp.c
        rtk_coex_ext_chip/rtw_coex_ext_chip_bt.c
        rtk_coex_ext_chip/rtw_coex_ext_chip_wpan.c
        rtk_coex_ext_chip/rtw_coex_ext_chip_debug.c
        rtk_coex_ext_chip/rtw_coex_ext_chip_btwpan.c
    )

    if(CONFIG_AMEBADPLUS)
        ameba_list_append(private_sources
            rtk_coex_ext_chip/rtw_coex_ext_chip_rtl8721da.c
        )
    elseif(CONFIG_AMEBASMART)
        ameba_list_append(private_sources
            rtk_coex_ext_chip/rtw_coex_ext_chip_rtl8730e.c
        )
    elseif(CONFIG_AMEBALITE)
        ameba_list_append(private_sources
            rtk_coex_ext_chip/rtw_coex_ext_chip_rtl8720e.c
        )
    elseif(CONFIG_AMEBAGREEN2)
         ameba_list_append(private_sources
            rtk_coex_ext_chip/rtw_coex_ext_chip_rl7005.c
        )
    elseif(CONFIG_AMEBAD)
        ameba_list_append(private_sources
            rtk_coex_ext_chip/rtw_coex_ext_chip_rtl8721d.c
        )
    elseif(CONFIG_AMEBAPRO3)
        ameba_list_append(private_sources
            rtk_coex_ext_chip/rtw_coex_ext_chip_rl7013.c
        )
    elseif(CONFIG_RTL8720F)
        ameba_list_append(private_sources
            rtk_coex_ext_chip/rtw_coex_ext_chip_rtl8720f.c
        )
    else()

    endif()
endif()


ameba_list_append(private_includes
    rtk_coex_wl_chip
    rtk_coex_ext_chip
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(${target_name}
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
