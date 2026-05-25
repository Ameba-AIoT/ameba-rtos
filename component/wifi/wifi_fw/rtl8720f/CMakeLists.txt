##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

if(CONFIG_MP_INCLUDED AND CONFIG_MP_EXPAND)
    set(TARGET_LIB wifi_fw_mp)
else()
    set(TARGET_LIB wifi_fw)
endif()

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

if("${c_MCU_PROJECT_NAME}" STREQUAL "km4tz")
    if(NOT CONFIG_FW_DRIVER_COEXIST)
        return()
    endif()
endif()

ameba_list_append(public_libraries
    ${c_SDK_LIB_APP_DIR}/lib_${TARGET_LIB}.a
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

set(CSRC)
set(WIFIFWDIR ${c_CMPT_WIFI_DIR}/wifi_fw)
include(${c_CMPT_WIFI_DIR}/wifi_fw/common/wififw_common.cmake)

ameba_list_append(private_sources
    ${CSRC}
    ram/wififw_init.c
    ram/wififw_txpkt.c
    ram/wififw_pwrctrl.c
    ram/wififw_csi.c
    ram/wififw_interrupt_ctrl.c
    ram/wififw_interrupt_handle.c
    ram/wififw_ps_bcnhdl.c
    ram/wififw_wowlan.c
    ram/wififw_h2c_c2h.c
    ram/wififw_ra_new.c
    ram/wififw_phy_reg_init.c
    ram/wififw_ps_twt.c
    ram/wififw_periodcca.c
    ram/wififw_lpsofld.c
    ram/wififw_gtimer.c
    ram/wififw_btcoex.c
    ram/wififw_multi_rx_bcn.c
    ram/wififw_txrpt.c
    ram/wififw_mp_debug.c
)
ameba_list_append_if(CONFIG_WIFI_FW_VERIFY private_sources
    test/src/wififw_coex_test.c
    test/src/wififw_int_test.c
    test/src/wififw_mem_io_test.c
)
ameba_list_append(private_includes
    include
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(${TARGET_LIB}
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
