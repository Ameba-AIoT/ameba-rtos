set(DIR ${WIFIFWDIR}/amebalite/ram)
set(DIR_COMMON ${WIFIFWDIR}/common)
set(DIR_TEST ${WIFIFWDIR}/amebalite/test/src)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/wifi/wifi_fw/amebalite/include
    ${BASEDIR}/component/wifi/wifi_fw/common
)

if(CONFIG_WHC_DEV)
    set(
        CSRC
        ${DIR}/wififw_init.c
        ${DIR_COMMON}/wififw_init_common.c
        ${DIR_COMMON}/wififw_txpkt_common.c
        ${DIR}/wififw_bcnmode.c
        ${DIR}/wififw_pwrctrl.c
        ${DIR}/wififw_btcoex.c
        ${DIR_COMMON}/wififw_pwrctrl_common.c
        ${DIR}/wififw_csi.c
        ${DIR_COMMON}/wififw_csi_common.c
        ${DIR}/wififw_interrupt_handle.c
        ${DIR}/wififw_interrupt_ctrl.c
        ${DIR_COMMON}/wififw_ps_state_common.c
        ${DIR}/wififw_ps_bcnhdl.c
        ${DIR_COMMON}/wififw_ps_bcnhdl_common.c
        ${DIR_COMMON}/wififw_bcnrxwindow_adjust_com.c
        ${DIR_COMMON}/wififw_hioe_common.c
        ${DIR}/wififw_wowlan.c
        ${DIR}/wififw_lpsofld.c
        ${DIR_COMMON}/wififw_lpsofld_common.c
        ${DIR}/wififw_ra_new.c
        ${DIR}/wififw_phy_reg_init.c
        ${DIR_COMMON}/wififw_phy_common.c
        ${DIR}/wififw_periodcca.c
        ${DIR_COMMON}/wififw_ps_twt_common.c
        ${DIR}/wififw_ps_twt.c
        ${DIR}/wififw_h2c_c2h.c
        ${DIR_COMMON}/wififw_h2c_common.c
        ${DIR_COMMON}/wififw_c2h_common.c
        ${DIR}/wififw_gtimer.c
        ${DIR_COMMON}/wififw_gtimer_common.c
        ${DIR_COMMON}/wififw_btcoex_mailbox_common.c
        ${DIR_COMMON}/wififw_btcoex_tdma_common.c
        ${DIR_COMMON}/wififw_task_common.c
        ${DIR_COMMON}/wififw_ra_hl.c
        ${DIR_COMMON}/wififw_ra_ll.c
        ${DIR_COMMON}/wififw_interrupt_handle_com.c
        ${DIR_COMMON}/wififw_ra_debug.c
        ${DIR_COMMON}/wififw_wowlan_common.c
        ${DIR_COMMON}/wififw_keepalive_common.c
        ${DIR_COMMON}/wififw_disconchk_common.c
        ${DIR_COMMON}/wififw_media_common.c
        ${DIR}/wififw_txrpt.c
        ${DIR_COMMON}/wififw_txrpt_common.c
    )

    if(CONFIG_WIFI_FW_VERIFY)
        list(
            APPEND CSRC
            ${DIR_TEST}/wififw_serial_io_test.c
            ${DIR_TEST}/wififw_buf_test.c
            ${DIR_TEST}/wififw_ddma_test.c
            ${DIR_TEST}/wififw_efuse_test.c
            ${DIR_TEST}/wififw_int_test.c
            ${DIR_TEST}/wififw_iopath_test.c
            ${DIR_TEST}/wififw_reg_test.c
            ${DIR_TEST}/wififw_fwlbk_test.c
            ${DIR_TEST}/wififw_coex_test.c
            ${DIR_TEST}/wififw_test.c
            ${DIR_TEST}/wififw_loopback_test.c
            ${DIR_TEST}/wififw_util_test.c
            ${DIR_TEST}/wififw_phycfg_test.c
        )
    endif()
endif()
if(NOT CMAKE_REFACTOR) # For temporary compatibility.
ameba_app_library_with_gitver(wifi_fw)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
else() # For temporary compatibility.NOTE:  script below is new
##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

# You may use if-else condition to set or update predefined variable above

ameba_list_append(public_libraries
    ${c_SDK_LIB_APPLICATION_DIR}/lib_wifi_fw.a
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

ameba_list_append(private_sources ${CSRC})
ameba_list_append(private_includes ${MODULE_IFLAGS})

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
ameba_add_external_app_library(wifi_fw
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
endif()# For temporary compatibility.
