set(DIR ${WIFIFWDIR}/amebalite/ram)
set(DIR_COMMON ${WIFIFWDIR}/common)
set(DIR_TEST ${WIFIFWDIR}/amebalite/test/src)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/wifi/wifi_fw/amebalite/include
    ${BASEDIR}/component/wifi/wifi_fw/common
)

if(CONFIG_AS_INIC_NP)
    set(
        CSRC
        ${DIR}/wififw_init.c
        ${DIR_COMMON}/wififw_init_common.c
        ${DIR_COMMON}/wififw_txpkt_common.c
        ${DIR}/wififw_bcnmode.c
        ${DIR}/wififw_pwrctrl.c
        ${DIR_COMMON}/wififw_pwrctrl_common.c
        ${DIR}/wififw_csi.c
        ${DIR_COMMON}/wififw_csi_common.c
        ${DIR}/wififw_interrupt_handle.c
        ${DIR}/wififw_interrupt_ctrl.c
        ${DIR_COMMON}/wififw_ps_state_common.c
        ${DIR}/wififw_ps_bcnhdl.c
        ${DIR_COMMON}/wififw_ps_bcnhdl_common.c
        ${DIR_COMMON}/wififw_hioe_common.c
        ${DIR}/wififw_wowlan.c
        ${DIR}/wififw_lpsofld.c
        ${DIR}/wififw_ra_new.c
        ${DIR}/wififw_phy_reg_init.c
        ${DIR_COMMON}/wififw_phy_common.c
        ${DIR}/wififw_periodcca.c
        ${DIR_COMMON}/wififw_ps_twt.c
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
        ${CMAKE_CURRENT_SOURCE_DIR}/lib_wifi_fw_git_version.c
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

ameba_app_library_with_gitver(wifi_fw)

target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
