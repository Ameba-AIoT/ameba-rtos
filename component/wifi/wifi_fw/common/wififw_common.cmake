set(DIR_COMMON ${WIFIFWDIR}/common)

list(
    APPEND CSRC
    ${DIR_COMMON}/wififw_init_common.c
    ${DIR_COMMON}/wififw_pwrctrl_common.c
    ${DIR_COMMON}/wififw_csi_common.c
    ${DIR_COMMON}/wififw_ps_state_common.c
    ${DIR_COMMON}/wififw_ps_bcnhdl_common.c
    ${DIR_COMMON}/wififw_hioe_common.c
    ${DIR_COMMON}/wififw_h2c_common.c
    ${DIR_COMMON}/wififw_phy_common.c
    ${DIR_COMMON}/wififw_ps_twt_common.c
    ${DIR_COMMON}/wififw_c2h_common.c
    ${DIR_COMMON}/wififw_task_common.c
    ${DIR_COMMON}/wififw_ra_hl.c
    ${DIR_COMMON}/wififw_interrupt_handle_com.c
    ${DIR_COMMON}/wififw_ra_debug.c
    ${DIR_COMMON}/wififw_lpsofld_common.c
    ${DIR_COMMON}/wififw_keepalive_common.c
    ${DIR_COMMON}/wififw_disconchk_common.c
    ${DIR_COMMON}/wififw_media_common.c
    ${DIR_COMMON}/wififw_ra_ll.c
    ${DIR_COMMON}/wififw_txrpt_common.c
)

#FIXME: These source are not add for amebaL2, add them will cause some compile error
if(NOT CONFIG_AMEBAL2)
    list(
        APPEND CSRC
        ${DIR_COMMON}/wififw_txpkt_common.c
        ${DIR_COMMON}/wififw_bcnrxwindow_adjust_com.c
        ${DIR_COMMON}/wififw_gtimer_common.c
        ${DIR_COMMON}/wififw_btcoex_mailbox_common.c
        ${DIR_COMMON}/wififw_btcoex_tdma_common.c
        ${DIR_COMMON}/wififw_wowlan_common.c
        ${DIR_COMMON}/wififw_tunnel.c
    )
endif()