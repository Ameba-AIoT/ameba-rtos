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
if(CONFIG_AMEBADPLUS OR CONFIG_AMEBAGREEN2 OR CONFIG_AMEBAPRO3 OR CONFIG_AMEBALITE OR CONFIG_AMEBASMART OR CONFIG_AMEBAD)
    list(
        APPEND CSRC
        ${DIR_COMMON}/wififw_btcoex_mailbox_common.c
    )
endif()

# CONFIG_WIFI_TWT_ENABLE 未打开时，编译 TWT stub（置空函数）
# only AX IC 并且排除 SMART 平台，需要编译 disable stub
if(NOT CONFIG_AMEBASMART)
    if(NOT CONFIG_WIFI_TWT_ENABLE)
        list(
            APPEND CSRC
            ${DIR_COMMON}/wififw_ps_twt_disable_com.c
        )
    endif()
endif()

# CONFIG_WIFI_CSI_ENABLE 未打开时，编译 CSI stub（置空函数）
#For smart,code size is not considered under the non-shared-core architecture 
if(NOT CONFIG_AMEBASMART)
    if(NOT CONFIG_WIFI_CSI_ENABLE)
        list(
            APPEND CSRC
            ${DIR_COMMON}/wififw_csi_disable_com.c
        )
    endif()
endif()