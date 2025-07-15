/**
  ******************************************************************************
  * @file    rtw_event.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include "rtw_inic_common.h"
#if !defined (CONFIG_FULLMAC) && !(defined(ZEPHYR_WIFI) && defined(CONFIG_WHC_HOST))
#include "wifi_api.h"
#include "platform_stdlib.h"
#if !defined(CONFIG_WHC_DEV)|| defined(CONFIG_WPA_LOCATION_DEV) || defined(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
#include "atcmd_service.h"
#include "wpa_lite_intf.h"
#include <wifi_auto_reconnect.h>
#include "../wpa_supplicant/wps_protocol_handler.h"
#include "../wpa_supplicant/eap_protocol_handler.h"
#endif
#if defined (CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#endif
#endif
#include "ameba_diagnose.h"
/**********************************************************************************************
 *                                          Globals
 *********************************************************************************************/
#if !(defined CONFIG_WHC_DEV) || defined(CONFIG_WPA_LOCATION_DEV) || defined(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)

extern int (*p_store_fast_connect_info)(unsigned int data1, unsigned int data2);
extern u8 rtw_join_status;
extern int join_fail_reason;
extern struct internal_block_param *join_block_param;
extern void (*p_wifi_join_info_free)(u8 iface_type);
extern void eap_disconnected_hdl(void);
#if !defined(CONFIG_MP_SHRINK) && defined (CONFIG_WHC_HOST)
extern u8 wifi_cast_get_initialized(void);
extern void wifi_cast_wifi_join_status_ev_hdl(u8 *buf, s32 buf_len, s32 flags);
#endif

/**********************************************************************************************
 *                                          External events
 *********************************************************************************************/

/**
 * @brief external event handle
 */
__weak struct rtw_event_hdl_func_t event_external_hdl[] = {
	{RTW_EVENT_MAX,			NULL},
};
__weak u16 array_len_of_event_external_hdl = sizeof(event_external_hdl) / sizeof(struct rtw_event_hdl_func_t);

void wifi_event_handle_external(u32 event_cmd, u8 *buf, s32 buf_len, s32 flags)
{
	if (!array_len_of_event_external_hdl) {
		return;
	}

	for (u32 i = 0; i < array_len_of_event_external_hdl; i++) {
		if (event_external_hdl[i].evt_id == event_cmd) {
			if (event_external_hdl[i].handler == NULL) {
				continue;
			}
			event_external_hdl[i].handler(buf, buf_len, flags);
		}
	}
}

/**********************************************************************************************
 *                                          Internal events
 *********************************************************************************************/

void wifi_event_join_status_internal_hdl(u8 *buf, s32 buf_len, s32 flags)
{
	UNUSED(buf_len);
#if (!defined(CONFIG_MP_SHRINK) && !defined(CONFIG_WHC_DEV)) || defined(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
	struct deauth_info  *deauth_data, *deauth_data_pre;
	u8 zero_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	u8 join_status = (u8)flags;
	struct rtw_event_join_status_info *evt_info = (struct rtw_event_join_status_info *)buf;
	struct rtw_event_join_fail *join_fail;
	struct rtw_event_disconnect *disconnect;
	struct diag_evt_wifi_disconn diag_disconn;
	struct diag_evt_wifi_join_fail diag_join_fail;

	rtw_join_status = join_status;

	/* step 1: internal process for different status*/
	if (join_status == RTW_JOINSTATUS_SUCCESS) {
		at_printf_indicate("wifi connected\r\n");
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
		LwIP_netif_set_link_up(0);
#endif

		/* if not use fast dhcp, store fast connect info to flash when connect successfully*/
#if !(defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP)
		if (p_store_fast_connect_info) {
			p_store_fast_connect_info(0, 0);
		}
		if (p_wifi_join_info_free) {
			/* free key here after join success */
			p_wifi_join_info_free(IFACE_PORT0);
		}
#endif

		/* if Synchronous connection, up sema when connect success*/
		if (join_block_param) {
			rtos_sema_give(join_block_param->sema);
		}
	}

	if (join_status == RTW_JOINSTATUS_FAIL) {
		join_fail = &evt_info->private.fail;
		/* if synchronous connection, up sema when connect fail*/
		if (join_block_param) {
			join_fail_reason = join_fail->fail_reason;
			rtos_sema_give(join_block_param->sema);
		}
		diag_join_fail.reason = -join_fail->fail_reason;
		diag_join_fail.reason_code = join_fail->reason_or_status_code;
		rtk_diag_event_add(RTK_EVENT_LEVEL_INFO, DIAG_EVT_WIFI_JOIN_FAIL, (u8 *)&diag_join_fail, sizeof(struct diag_evt_wifi_join_fail));
		at_printf_indicate("wifi connect failed\r\n");
	}

	if (join_status == RTW_JOINSTATUS_DISCONNECT) {
		disconnect = &evt_info->private.disconnect;
		diag_disconn.reason = disconnect->disconn_reason;
		rtk_diag_event_add(RTK_EVENT_LEVEL_INFO, DIAG_EVT_WIFI_DISCONN, (u8 *)&diag_disconn, sizeof(struct diag_evt_wifi_disconn));
		at_printf_indicate("wifi disconnected\r\n");
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
		LwIP_DHCP_stop(0);
		LwIP_netif_set_link_down(0);
#endif

#if (!defined(CONFIG_WHC_DEV) && !(defined(ZEPHYR_WIFI) && defined(CONFIG_WHC_HOST))) || defined(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
		deauth_data_pre = (struct deauth_info *)rtos_mem_zmalloc(sizeof(struct deauth_info));
		rtw_psk_deauth_info_flash((u8 *)deauth_data_pre, sizeof(struct deauth_info), FLASH_READ);
		if (memcmp(deauth_data_pre->bssid, zero_mac, 6) != 0) {
			deauth_data = (struct deauth_info *)rtos_mem_zmalloc(sizeof(struct deauth_info));
			rtw_psk_deauth_info_flash((u8 *)deauth_data, sizeof(struct deauth_info), FLASH_WRITE);
			rtos_mem_free((u8 *)deauth_data);
		}
		rtos_mem_free((u8 *)deauth_data_pre);
#endif

		eap_disconnected_hdl();
	}

	if ((join_status == RTW_JOINSTATUS_DISCONNECT) || (join_status == RTW_JOINSTATUS_FAIL)) {
		/*wpa lite disconnect hdl*/
		rtw_psk_disconnect_hdl(evt_info->bssid, IFACE_PORT0);
	}

#if !defined(CONFIG_WHC_DEV) && CONFIG_AUTO_RECONNECT
	rtw_reconn_join_status_hdl(buf, flags);
#endif

#ifdef CONFIG_WHC_HOST
	if (wifi_cast_get_initialized()) {
		wifi_cast_wifi_join_status_ev_hdl(buf, buf_len, flags);
	}
#endif
#else
	UNUSED(flags);
	UNUSED(buf);
#endif
}

void rtw_sta_assoc_hdl(u8 *buf, s32 buf_len, s32 flags)
{
	(void)buf_len;
	(void)flags;
	u8 *mac_addr = NULL;

	/* softap add sta */
	mac_addr = ((unsigned char *)((SIZE_PTR)(buf) + 10)); // GetAddr2Ptr
	at_printf_indicate("client connected:\""MAC_FMT"\"\r\n", MAC_ARG(mac_addr));
}

void rtw_sta_disassoc_hdl(u8 *buf, s32 buf_len, s32 flags)
{
	(void)flags;
	(void)buf_len;
	u8 *mac_addr = NULL;

	/* softap dis sta */
	mac_addr = buf;
	at_printf_indicate("client disconnected:\""MAC_FMT"\"\r\n", MAC_ARG(mac_addr));

	rtw_psk_disconnect_hdl(mac_addr, IFACE_PORT1);
}

void rtw_join_status_hdl(u8 *buf, s32 buf_len, s32 flags)
{
	(void)buf_len;

	wifi_event_join_status_internal_hdl(buf, buf_len, flags);
}

void rtw_eapol_start_hdl(u8 *buf, s32 buf_len, s32 flags)
{
#ifdef CONFIG_ENABLE_EAP
	if (get_eap_phase()) {
		eap_eapol_start_hdl(buf, buf_len, flags);
	}
#endif
}

void rtw_eapol_recvd_hdl(u8 *buf, s32 buf_len, s32 flags)
{
#ifdef CONFIG_ENABLE_EAP
	if (get_eap_phase()) {
		eap_eapol_recvd_hdl(buf, buf_len, flags);
	}
#endif

#if defined(CONFIG_ENABLE_WPS) && CONFIG_ENABLE_WPS
	if (get_wps_phase()) {
		wpas_wsc_eapol_recvd_hdl(buf, buf_len, flags);
	}
#endif
}

/**
 * @brief internal event handle
 */
const struct rtw_event_hdl_func_t event_internal_hdl[] = {
	{RTW_EVENT_STA_ASSOC,			rtw_sta_assoc_hdl},
	{RTW_EVENT_STA_DISASSOC,		rtw_sta_disassoc_hdl},
	{RTW_EVENT_JOIN_STATUS,			rtw_join_status_hdl},
#if defined(CONFIG_ENABLE_WPS) && CONFIG_ENABLE_WPS
	{RTW_EVENT_WPA_STA_WPS_START,	wpas_wsc_sta_wps_start_hdl},
	{RTW_EVENT_WPA_WPS_FINISH,		wpas_wsc_wps_finish_hdl},
#endif
	{RTW_EVENT_WPA_EAPOL_START,		rtw_eapol_start_hdl},
	{RTW_EVENT_WPA_EAPOL_RECVD,		rtw_eapol_recvd_hdl},
	{RTW_EVENT_RX_MGNT,				rtw_sae_sta_rx_auth},
	{RTW_EVENT_RX_MGNT_AP,			rtw_sae_ap_rx_auth},
	{RTW_EVENT_EXTERNAL_AUTH_REQ,	rtw_sae_sta_start},
	{RTW_EVENT_WPA_STA_4WAY_START,	rtw_psk_sta_start_4way},
	{RTW_EVENT_WPA_AP_4WAY_START,	rtw_psk_ap_start_4way},
	{RTW_EVENT_WPA_STA_4WAY_RECV,	rtw_psk_sta_recv_eapol},
	{RTW_EVENT_WPA_AP_4WAY_RECV,	rtw_psk_ap_recv_eapol},
	{RTW_EVENT_WPA_SET_PSK_INFO,	rtw_psk_set_psk_info_evt_hdl},
	{RTW_EVENT_OWE_START_CALC,		rtw_owe_start_calc},
	{RTW_EVENT_OWE_PEER_KEY_RECV,	rtw_owe_peer_key_recv},
#if defined(CONFIG_IEEE80211V) || defined(CONFIG_IEEE80211K) || defined(CONFIG_IEEE80211R)
	{RTW_EVENT_KVR_CAP_UPDATE,		rtw_roam_kvr_cap_update},
#if defined(CONFIG_IEEE80211V) || defined(CONFIG_IEEE80211K)
	{RTW_EVENT_NB_RESP_RECV,		rtw_roam_nb_rpt_elem_parsing},
#endif
#ifdef CONFIG_IEEE80211V
	{RTW_EVENT_BTM_REQ_RECV,		rtw_wnm_btm_req_process},
	{RTW_EVENT_BTM_DEBUG_CMD,		rtw_wnm_dbg_cmd},
#endif
#ifdef CONFIG_IEEE80211R
	{RTW_EVENT_FT_AUTH_START,		rtw_ft_auth_start},
	{RTW_EVENT_FT_RX_MGNT,			rtw_ft_rx_mgnt},
#endif
#endif
	{RTW_EVENT_DEAUTH_INFO_FLASH,	rtw_psk_deauth_info_flash_event_hdl},
};

void wifi_event_handle_internal(u32 event_cmd, u8 *buf, s32 buf_len, s32 flags)
{
#ifndef CONFIG_MP_SHRINK
#if !(defined(ZEPHYR_WIFI) && defined(CONFIG_WHC_HOST))
	/*internal & common events all need handle*/
#else
	/*only common events need handle*/
	if (event_cmd > RTW_EVENT_INTERNAL_BASE) {
		return;
	}
#endif

	for (u32 i = 0; i < sizeof(event_internal_hdl) / sizeof(struct rtw_event_hdl_func_t); i++) {
		if (event_internal_hdl[i].evt_id == event_cmd) {
			if (event_internal_hdl[i].handler == NULL) {
				continue;
			}
			event_internal_hdl[i].handler(buf, buf_len, flags);
		}
	}
#else
	UNUSED(event_cmd);
	UNUSED(buf);
	UNUSED(buf_len);
	UNUSED(flags);
#endif
}

/**********************************************************************************************
 *                                          Common events
 *********************************************************************************************/
int wifi_event_handle(u32 event_cmd, u8 *buf, s32 buf_len, s32 flags)
{
	if ((event_cmd >= RTW_EVENT_MAX && event_cmd <= RTW_EVENT_INTERNAL_BASE) || event_cmd > RTW_EVENT_INTERNAL_MAX) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "invalid evt: %d \n", event_cmd);
		return -RTK_ERR_BADARG;
	}

	wifi_event_handle_internal(event_cmd, buf, buf_len, flags);

	wifi_event_handle_external(event_cmd, buf, buf_len, flags);

	return RTK_SUCCESS;
}

void wifi_event_init(void)
{
	// Nothing TODO, global BSS variable default is 0
}
#endif

void wifi_indication(u32 event, u8 *buf, s32 buf_len, s32 flags)
{
#if defined(CONFIG_WHC_DEV) && !defined(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
	extern void whc_dev_wifi_event_indicate(u32 event_cmd, u8 * buf, s32 buf_len, s32 flags);
	whc_dev_wifi_event_indicate(event, buf, buf_len, flags);
#endif

#if !(defined CONFIG_WHC_DEV) || defined(CONFIG_WPA_LOCATION_DEV) || defined(CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
	wifi_event_handle(event, buf, buf_len, flags);
#endif
}

