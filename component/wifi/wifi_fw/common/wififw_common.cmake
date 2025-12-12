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
    ${DIR_COMMON}/wififw_ps_dbg.c
    ${DIR_COMMON}/wififw_lpsofld_common.c
    ${DIR_COMMON}/wififw_keepalive_common.c
    ${DIR_COMMON}/wififw_disconchk_common.c
    ${DIR_COMMON}/wififw_media_common.c
    ${DIR_COMMON}/wififw_ra_ll.c
    ${DIR_COMMON}/wififw_txrpt_common.c
    ${DIR_COMMON}/wififw_bcnrxwindow_adjust_com.c
    ${DIR_COMMON}/wififw_wowlan_common.c
    ${DIR_COMMON}/wififw_mp.c
    ${DIR_COMMON}/wififw_gtimer_common.c
    ${DIR_COMMON}/wififw_btcoex_tdma_common.c
)

#From L2 and 7098, mac uses tx architecture. FW cannot use the old ICs' wififw_txpkt_common.c.
if(CONFIG_AMEBADPLUS OR CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAPRO3 OR CONFIG_AMEBALITE OR CONFIG_AMEBASMART OR CONFIG_AMEBAD)
    list(
        APPEND CSRC
        ${DIR_COMMON}/wififw_txpkt_common.c
        ${DIR_COMMON}/wififw_tunnel.c
    )
endif()

#From 7098, BT use cpu SDN design, and deleted the old ICs' HW interface between WL and BT. wififw_btcoex_mailbox_common.c only for old ICs.
if(CONFIG_AMEBADPLUS OR CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAPRO3 OR CONFIG_AMEBALITE OR CONFIG_AMEBASMART OR CONFIG_AMEBAD OR CONFIG_AMEBAL2)
    list(
        APPEND CSRC
        ${DIR_COMMON}/wififw_btcoex_mailbox_common.c
    )
endif()
