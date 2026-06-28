ameba_add_subdirectory_if(CONFIG_SDN_HOST hci/host)
ameba_add_subdirectory_if(CONFIG_SDN_DEV hci/client)

if (NOT CONFIG_SDN)
    return()
endif()

ameba_add_subdirectory_if(CONFIG_BT_COEXIST coex)
ameba_add_subdirectory(platform/${c_SOC_TYPE})
# ameba_add_subdirectory(verify)

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
    ${CMAKE_CURRENT_SOURCE_DIR}/coex
    ${c_COMPONENT_DIR}/rtk_coex
)

ameba_list_append(public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_sdn.a
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

ameba_list_append(private_sources
    util/sdn_atcmd.c
    util/sdn_util.c
    util/sdn_debug.c
    util/sdn_watchdog.c
    scheduler/sdn_schedule.c
)

ameba_list_append(private_sources
    bt/hci/bt_hci_acl.c
    bt/hci/bt_hci_cmd_linkctrl.c
    bt/hci/bt_hci_cmd_baseband.c
    bt/hci/bt_hci_cmd_info.c
    bt/hci/bt_hci_cmd_status.c
    bt/hci/bt_hci_cmd_le.c
    bt/hci/bt_hci_cmd_vendor.c
    bt/hci/bt_hci_event.c
    bt/hci/bt_hci.c
    bt/hci/bt_hci_cmd_vendor_mp.c
    bt/le/ble_ll_dtm.c
    bt/le/ble_ll.c
    bt/le/ble_ll_addr.c
    bt/le/ble_ll_adv.c
    bt/le/ble_ll_ext_adv.c
    bt/le/ble_ll_periodic_adv.c
    bt/le/ble_ll_phy.c
    bt/le/ble_ll_conn.c
    bt/le/ble_ll_peripheral.c
    bt/le/ble_ll_chnl.c
    bt/le/ble_llcp.c
    bt/le/ble_llcp_remote.c
    bt/le/ble_llcp_local.c
    bt/le/ble_llcp_common.c
    bt/le/ble_llcp_chnlmap_update.c
    bt/le/ble_llcp_phy_update.c
    bt/le/ble_llcp_conn_update.c
    bt/le/ble_llcp_encryption.c
    bt/le/ble_ll_privacy.c
    bt/le/ble_ll_filter.c
    bt/le/ble_ll_scan.c
    bt/le/ble_ll_ext_scan.c
    bt/le/ble_ll_central.c
    bt/le/ble_ll_security.c
    bt/le/ble_psd.c
    bt/le/ble_ll_pa_sync.c
)

ameba_list_append(private_includes
    ../verification
    bt
    bt/hci
    bt/le
    util
    hci
    hci/client
    scheduler
    user_config
)

ameba_list_append(private_sources
    coex/sdn_coex_bt_api_weak.c
)

ameba_list_append(private_compile_options
    -Wno-address-of-packed-member
    -DCONFIG_SDN_ROM
)

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(sdn
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