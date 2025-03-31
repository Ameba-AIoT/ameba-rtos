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
#if !defined (CONFIG_FULLMAC) && !(defined(ZEPHYR_WIFI) && defined(CONFIG_AS_INIC_AP))
#include "wifi_api.h"
#include "platform_stdlib.h"
#if !defined(CONFIG_AS_INIC_NP) || defined CONFIG_ZEPHYR_SDK || defined(CONFIG_WPA_LOCATION_DEV)
#include "atcmd_service.h"
#include "wpa_lite_intf.h"
#include <wifi_auto_reconnect.h>
#endif
#if defined (CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#endif
#endif
/**********************************************************************************************
 *                                          Globals
 *********************************************************************************************/
#if !(defined CONFIG_AS_INIC_NP) || defined CONFIG_ZEPHYR_SDK || defined(CONFIG_WPA_LOCATION_DEV)
struct event_list_elem_t {
	void (*handler)(char *buf, int len, int flags, void *user_data);
	void	*handler_user_data;
};

#define WIFI_EVENT_MAX_ROW	2
static struct event_list_elem_t event_callback_list[WIFI_EVENT_MAX][WIFI_EVENT_MAX_ROW] = {0};
extern int (*p_store_fast_connect_info)(unsigned int data1, unsigned int data2);
extern u8 rtw_join_status;
extern int join_fail_reason;
extern struct internal_join_block_param *join_block_param;
extern void (*p_wifi_join_info_free)(u8 iface_type);
#ifdef CONFIG_ENABLE_EAP
extern void eap_disconnected_hdl(void);
#endif

/**********************************************************************************************
 *                                          Internal events
 *********************************************************************************************/

void wifi_event_join_status_internal_hdl(char *buf, int flags)
{
#if !defined(CONFIG_MP_SHRINK) && !defined(CONFIG_AS_INIC_NP)
	struct deauth_info  *deauth_data, *deauth_data_pre;
	u8 zero_mac[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	u8 join_status = (u8)flags;
	struct rtw_event_join_fail_info_t *fail_info = (struct rtw_event_join_fail_info_t *)buf;

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
		if (join_block_param && join_block_param->block) {
			rtos_sema_give(join_block_param->join_sema);
		}
	}

	if (join_status == RTW_JOINSTATUS_FAIL) {
		/* if synchronous connection, up sema when connect fail*/
		if (join_block_param && join_block_param->block) {
			join_fail_reason = fail_info->fail_reason;
			rtos_sema_give(join_block_param->join_sema);
		}

		at_printf_indicate("wifi connect failed\r\n");
	}

	if (join_status == RTW_JOINSTATUS_DISCONNECT) {
		at_printf_indicate("wifi disconnected\r\n");
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
		LwIP_DHCP_stop(0);
		LwIP_netif_set_link_down(0);
#endif

#if !defined(CONFIG_AS_INIC_NP) && !(defined(ZEPHYR_WIFI) && defined(CONFIG_AS_INIC_AP))
		deauth_data_pre = (struct deauth_info *)rtos_mem_zmalloc(sizeof(struct deauth_info));
		rtw_psk_deauth_info_flash((char *)deauth_data_pre, sizeof(struct deauth_info), FLASH_READ, NULL);
		if (memcmp(deauth_data_pre->bssid, zero_mac, 6) != 0) {
			deauth_data = (struct deauth_info *)rtos_mem_zmalloc(sizeof(struct deauth_info));
			rtw_psk_deauth_info_flash((char *)deauth_data, sizeof(struct deauth_info), FLASH_WRITE, NULL);
			rtos_mem_free((u8 *)deauth_data);
		}
		rtos_mem_free((u8 *)deauth_data_pre);
#endif

#ifdef CONFIG_ENABLE_EAP
		eap_disconnected_hdl();
#endif
	}

	if ((join_status == RTW_JOINSTATUS_DISCONNECT) || (join_status == RTW_JOINSTATUS_FAIL)) {
		/*wpa lite disconnect hdl*/
		u8 port = IFACE_PORT0;
		rtw_psk_disconnect_hdl(buf, 0, flags, &port);
	}

#if CONFIG_AUTO_RECONNECT
	rtw_reconn_join_status_hdl(buf, flags);
#endif

#else
	UNUSED(flags);
	UNUSED(buf);
#endif
}

/**
 * @brief internal event handle, must have same order as enum
 */
void (*const event_internal_hdl[])(char *buf, int len, int flags, void *user_data) = {
#if (!defined(CONFIG_AS_INIC_NP) && !(defined(ZEPHYR_WIFI) && defined(CONFIG_AS_INIC_AP))) || defined(CONFIG_ZEPHYR_SDK) || defined(CONFIG_WPA_LOCATION_DEV)
	rtw_sae_sta_rx_auth,				/*WIFI_EVENT_RX_MGNT*/
	rtw_sae_ap_rx_auth,					/*WIFI_EVENT_RX_MGNT_AP*/
	rtw_sae_sta_start,					/*WIFI_EVENT_EXTERNAL_AUTH_REQ*/
	rtw_psk_sta_start_4way,				/*WIFI_EVENT_WPA_STA_4WAY_START*/
	rtw_psk_ap_start_4way, 				/*WIFI_EVENT_WPA_AP_4WAY_START*/
	rtw_psk_sta_recv_eapol, 			/*WIFI_EVENT_WPA_STA_4WAY_RECV*/
	rtw_psk_ap_recv_eapol, 				/*WIFI_EVENT_WPA_AP_4WAY_RECV*/
	rtw_psk_set_psk_info_evt_hdl,		/*WIFI_EVENT_WPA_SET_PSK_INFO*/
	rtw_owe_start_calc,					/*WIFI_EVENT_OWE_START_CALC*/
	rtw_owe_peer_key_recv,				/*WIFI_EVENT_OWE_PEER_KEY_RECV*/
#if defined(CONFIG_IEEE80211V) || defined(CONFIG_IEEE80211K) || defined(CONFIG_IEEE80211R)
	rtw_roam_kvr_cap_update,			/*WIFI_EVENT_KVR_CAP_UPDATE*/
#if defined(CONFIG_IEEE80211V) || defined(CONFIG_IEEE80211K)
	rtw_roam_nb_rpt_elem_parsing,		/*WIFI_EVENT_NB_RESP_RECV*/
#endif
#ifdef CONFIG_IEEE80211V
	rtw_wnm_btm_req_process,			/*WIFI_EVENT_BTM_REQ_RECV*/
	rtw_wnm_dbg_cmd, 						/*WIFI_EVENT_BTM_DEBUG_CMD*/
#endif
#ifdef CONFIG_IEEE80211R
	rtw_ft_auth_start,					/*WIFI_EVENT_FT_AUTH_START*/
	rtw_ft_rx_mgnt,						/*WIFI_EVENT_FT_RX_MGNT*/
#endif
#endif
	rtw_psk_deauth_info_flash,			/*WIFI_EVENT_DEAUTH_INFO_FLASH*/
#endif
	/*WIFI_EVENT_INTERNAL_MAX*/
};

void wifi_event_handle_internal(unsigned int event_cmd, char *buf, int buf_len, int flags)
{
	u8 *mac_addr = NULL;
	/*internal only events*/
	if (event_cmd > WIFI_EVENT_INTERNAL_BASE) {
#if !defined(CONFIG_MP_SHRINK) && !(defined(ZEPHYR_WIFI) && defined(CONFIG_AS_INIC_AP))
		event_internal_hdl[event_cmd - WIFI_EVENT_INTERNAL_BASE - 1](buf, buf_len, flags, NULL);
#else
		UNUSED(buf_len);
#endif
		return;
	}

	/*some common events also need internal handle*/
	if (event_cmd == WIFI_EVENT_JOIN_STATUS) {
		wifi_event_join_status_internal_hdl(buf, flags);
	} else if (event_cmd == WIFI_EVENT_STA_ASSOC) {
		/* softap add sta */
		mac_addr = ((unsigned char *)((SIZE_PTR)(buf) + 10)); // GetAddr2Ptr
		at_printf_indicate("client connected:\""MAC_FMT"\"\r\n", MAC_ARG(mac_addr));
	} else if (event_cmd == WIFI_EVENT_STA_DISASSOC) {
		/* softap dis sta */
		mac_addr = (u8 *)buf;
		at_printf_indicate("client disconnected:\""MAC_FMT"\"\r\n", MAC_ARG(mac_addr));
	}
}

/**********************************************************************************************
 *                                          Common events
 *********************************************************************************************/
int wifi_event_handle(unsigned int event_cmd, char *buf, int buf_len, int flags)
{
	void (*handle)(char *buf, int len, int flags, void *user_data) = NULL;
	int i;

	if ((event_cmd >= WIFI_EVENT_MAX && event_cmd <= WIFI_EVENT_INTERNAL_BASE) || event_cmd > WIFI_EVENT_INTERNAL_MAX) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "invalid evt: %d \n", event_cmd);
		return RTW_BADARG;
	}

	wifi_event_handle_internal(event_cmd, buf, buf_len, flags);
	/*user callback handle*/
	if (event_cmd < WIFI_EVENT_MAX) {
		for (i = 0; i < WIFI_EVENT_MAX_ROW; i++) {
			handle = event_callback_list[event_cmd][i].handler;
			if (handle == NULL) {
				continue;
			}
			handle(buf, buf_len, flags, event_callback_list[event_cmd][i].handler_user_data);
		}
	}
	return RTK_SUCCESS;
}

void wifi_reg_event_handler(unsigned int event_cmds, void (*handler_func)(char *buf, int len, int flags, void *user_data), void *handler_user_data)
{
	int i = 0, j = 0;
	if (event_cmds < WIFI_EVENT_MAX) {
		for (i = 0; i < WIFI_EVENT_MAX_ROW; i++) {
			if (event_callback_list[event_cmds][i].handler == NULL) { //there exists an empty position for new handler
				for (j = 0; j < WIFI_EVENT_MAX_ROW; j++) {
					if (event_callback_list[event_cmds][j].handler == handler_func) { //the new handler already exists in the table
						return;
					}
				}
				event_callback_list[event_cmds][i].handler = handler_func;
				event_callback_list[event_cmds][i].handler_user_data = handler_user_data;
				return;
			}
		}
		//there is no empty position for new handler
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "WifiEvtReg fail: %d %d \n", event_cmds, WIFI_EVENT_MAX_ROW);
	}
}

void wifi_unreg_event_handler(unsigned int event_cmds, void (*handler_func)(char *buf, int len, int flags, void *user_data))
{
	int i;
	if (event_cmds < WIFI_EVENT_MAX) {
		for (i = 0; i < WIFI_EVENT_MAX_ROW; i++) {
			if (event_callback_list[event_cmds][i].handler == handler_func) {
				event_callback_list[event_cmds][i].handler = NULL;
				event_callback_list[event_cmds][i].handler_user_data = NULL;
				return;
			}
		}
	}
}

void wifi_event_init(void)
{
	// Nothing TODO, global BSS variable default is 0
	// memset(event_callback_list, 0, sizeof(event_callback_list));
}
#endif

void wifi_indication(unsigned int event, char *buf, int buf_len, int flags)
{
#if defined(CONFIG_AS_INIC_NP) || defined(CONFIG_WHC_BRIDGEB)
	extern void whc_dev_wifi_event_indicate(int event_cmd, char *buf, int buf_len, int flags);
	whc_dev_wifi_event_indicate(event, buf, buf_len, flags);
#endif

#if !(defined CONFIG_AS_INIC_NP) || defined CONFIG_ZEPHYR_SDK || defined(CONFIG_WPA_LOCATION_DEV)
	wifi_event_handle(event, buf, buf_len, flags);
#endif
}

