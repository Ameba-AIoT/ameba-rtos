/******************************************************************************
 *
 * Copyright(c) 2016 - 2026 Realtek Corporation. All rights reserved.
 *
 ******************************************************************************/

#include "rtw_wifi_common.h"
#include "os_wrapper.h"
#include "wifi_intf_drv_to_upper.h"
#include "wifi_intf_drv_to_lwip.h"
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
#include <lwip_netconf.h>
#include <dhcp/dhcps.h>
#endif
#if defined(CONFIG_WHC_DEV)
#if !defined(CONFIG_WHC_INTF_IPC)
#include "whc_dev.h"
#else
#include "whc_ipc_dev_trx.h"
#include "whc_ipc.h"
#endif
#endif

#ifdef CONFIG_WHC_DEV
/* ip/gw pushed by the host before suspend, used in wowlan mode */
u8 whc_ipc_ip_addr[4] = {0};
u8 whc_ipc_gateway[4] = {0};
#endif

/**
 *      wifi_netif_set_info - set netif hw address and register dev pointer to netif device
 *      @idx: netif index
 *			    0 for STA only or SoftAP only or STA in STA+SoftAP concurrent mode,
 *			    1 for SoftAP in STA+SoftAP concurrent mode
 *      @dev: register netdev pointer to LWIP. Reserved.
 *      @dev_addr: set netif hw address
 *
 *      Return Value: None
 */
void wifi_netif_set_info(int idx, void *dev, unsigned char *dev_addr)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	lwip_wlan_set_netif_info(idx, dev, dev_addr);
#if defined(CONFIG_WHC_WIFI_API_PATH)
	/* for dual tcpip in fullmac */
	whc_dev_set_netif_info(idx, dev_addr);
#endif
#else
#if defined(CONFIG_WHC_DEV) && defined(CONFIG_WHC_WIFI_API_PATH)
	whc_dev_set_netif_info(idx, dev_addr);
#endif
#endif
	/* Cache IPv6 enabled state at boot to avoid IPC during suspend */
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	wifi_if_set_wowlan_ipv6_wake(lwip_get_ipv6_enabled());
#elif defined(CONFIG_WHC_DEV)
	wifi_if_set_wowlan_ipv6_wake((u8)whc_dev_get_lwip_info(WHC_WLAN_GET_IPV6_ENABLED, NULL, 0));
#else
	wifi_if_set_wowlan_ipv6_wake(0);
#endif
	(void)idx;
	(void)dev;
	(void)dev_addr;
}

#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
static int netif_is_valid_ip(int idx, unsigned char *ip_dest)
{
#if defined(CONFIG_RNAT) && CONFIG_RNAT
	UNUSED(idx);
	UNUSED(ip_dest);
	return 1;
#else
	return lwip_is_valid_ip(idx, ip_dest);
#endif
}
#endif

/**
 *      wifi_netif_rx - indicate the pending rx skb of @idx to the network stack
 *      Return Value: 0 if consumed, -1 if dropped (caller frees the skb)
 */
int wifi_netif_rx(int idx, unsigned int len)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	struct sk_buff *skb = wifi_if_get_recv_skb(idx);

	/* IPv4 only (14B eth hdr + 20B ip hdr): drop unicast not destined to this netif */
	if (len >= 34 && skb->data[12] == 0x08 && skb->data[13] == 0x00 && !netif_is_valid_ip(idx, skb->data + 30)) {
		return -1;
	}
#endif

#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER && (defined(CONFIG_WHC_INTF_IPC) || defined(CONFIG_WHC_NONE)) && !defined(CONFIG_WHC_DEV_TCPIP_KEEPALIVE)
	/* for single core ipc case */
	netif_adapter_wifi_recv(idx, len);
#endif

#if defined(CONFIG_WHC_DEV)
	whc_dev_netif_rx(idx);
#endif
	(void)len;
	(void)idx;
	return 0;
}

void wifi_netif_set_up(u8 idx)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	lwip_netif_set_up(idx);
#else
	(void)idx;
#endif
}

void wifi_netif_set_down(u8 idx)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	lwip_netif_set_down(idx);
#else
	(void)idx;
#endif
}

void wifi_netif_link_up(u8 idx)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	lwip_netif_set_link_up(idx);
#else
	(void)idx;
#endif
}

void wifi_netif_link_down(u8 idx)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	lwip_netif_set_link_down(idx);
#else
	(void)idx;
#endif
}

/**
 *      wifi_netif_dhcps_ip_check - check whether gate.ip is a softap dhcp client
 *      Return Value: 1 if valid (or cannot be checked on this core), 0 otherwise
 */
int wifi_netif_dhcps_ip_check(u8 gate, u8 ip)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	return dhcps_ip_in_table_check(pnetif_ap, gate, ip);
#elif defined(CONFIG_WHC_DEV)
	return whc_dev_ip_in_table_indicate(gate, ip);
#else
	(void)gate;
	(void)ip;
	return 1;
#endif
}

/**
 *      wifi_netif_get_ip_gw - copy STA ip (and gw if @gw != NULL), 4 bytes each
 */
void wifi_netif_get_ip_gw(u8 wowlan_mode, u8 *ip, u8 *gw)
{
#if defined(CONFIG_WHC_DEV) && !(defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER)
	if (wowlan_mode) {
		memcpy(ip, whc_ipc_ip_addr, 4);
		if (gw != NULL) {
			memcpy(gw, whc_ipc_gateway, 4);
		}
	} else {
		memcpy(ip, wifi_netif_get_ip(0), 4);
		if (gw != NULL) {
			memcpy(gw, wifi_netif_get_gw(0), 4);
		}
	}
#else
	(void)wowlan_mode;
	memcpy(ip, wifi_netif_get_ip(0), 4);
	if (gw != NULL) {
		memcpy(gw, wifi_netif_get_gw(0), 4);
	}
#endif
}

unsigned char *wifi_netif_get_ip(int idx)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	return lwip_get_ip(idx);
#elif defined(CONFIG_WHC_DEV)
	return (unsigned char *)whc_dev_get_lwip_info(WHC_WLAN_GET_IP, NULL, idx);
#else
	UNUSED(idx);
	return NULL;
#endif
}

unsigned char *wifi_netif_get_gw(int idx)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	return lwip_get_gw(idx);
#elif defined(CONFIG_WHC_DEV)
	return (unsigned char *)whc_dev_get_lwip_info(WHC_WLAN_GET_GW, NULL, idx);
#else
	UNUSED(idx);
	return NULL;
#endif
}

unsigned char *wifi_netif_get_mask(int idx)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	return lwip_get_mask(idx);
#elif defined(CONFIG_WHC_DEV)
	return (unsigned char *)whc_dev_get_lwip_info(WHC_WLAN_GET_GWMSK, NULL, idx);
#else
	UNUSED(idx);
	return NULL;
#endif
}

unsigned char *wifi_netif_get_hwaddr(int idx)
{
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	return lwip_get_mac(idx);
#elif defined(CONFIG_WHC_DEV)
	return (unsigned char *)whc_dev_get_lwip_info(WHC_WLAN_GET_HW_ADDR, NULL, idx);
#else
	UNUSED(idx);
	return NULL;
#endif
}

#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
/**
 *      wifi_netif_send - send IP packets to WLAN. Called by low_level_output().
 *      @idx: netif index
 *      @sg_list: data buffer list
 *      @sg_len: size of each data buffer
 *      @total_len: total data len
 *
 *      Return Value: 0 on success, -1 on failure
 */
int wifi_netif_send(int idx, struct eth_drv_sg *sg_list, int sg_len, int total_len, u8 is_special_pkt)
{
	struct eth_drv_sg *last_sg;
	struct sk_buff *skb = NULL;
	int ret = 0;
	u8 special_times = 0;

	if (idx == -1) {
		RTK_LOGE(TAG_WLAN_DRV, "wlan_send idx err\n");
		return -1;
	}

	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	if (wifi_if_is_netdev_enable(idx)) {
		wifi_if_tx_inc(idx);
	} else {
		RTK_LOGE(TAG_WLAN_DRV, "wlan_send net is down\n");
		rtos_critical_exit(RTOS_CRITICAL_WIFI);
		return -1;
	}
	rtos_critical_exit(RTOS_CRITICAL_WIFI);

RETRY:
	skb = wifi_if_alloc_skb(total_len);
	if (skb == NULL) {
		/*JIRA: https://jira.realtek.com/browse/RSWLANDIOT-8584 */
		if (is_special_pkt && (special_times < 9)) {
			special_times++;
			rtos_time_delay_ms(1);
			goto RETRY;
		}
		ret = -1;
		goto exit;
	}
	for (last_sg = &sg_list[sg_len]; sg_list < last_sg; ++sg_list) {
		memcpy(skb->tail, (void *)(sg_list->buf), sg_list->len);
		skb_put(skb,  sg_list->len);
	}
	wifi_if_send_skb(idx, skb);

exit:
	rtos_critical_enter(RTOS_CRITICAL_WIFI);
	wifi_if_tx_dec(idx);
	rtos_critical_exit(RTOS_CRITICAL_WIFI);
	return ret;
}

/**
 *      wifi_netif_recv - indicate packets to LWIP. Called by netif_adapter_wifi_recv().
 *      @idx: netif index
 *      @sg_list: data buffer list
 *      @sg_len: size of each data buffer
 *
 *      Return Value: None
 */
void wifi_netif_recv(int idx, struct eth_drv_sg *sg_list, int sg_len)
{
	struct eth_drv_sg *last_sg;
	struct sk_buff *skb;

	if (idx == -1) {
		RTK_LOGE(TAG_WLAN_DRV, "wlanrecv skb NULL\n");
		return;
	}
	skb = wifi_if_get_recv_skb(idx);
	if (skb == NULL) {
		RTK_LOGE(TAG_WLAN_DRV, "wlanrecv skb null\n");
		return;
	}

	for (last_sg = &sg_list[sg_len]; sg_list < last_sg; ++sg_list) {
		if (sg_list->buf != 0) {
			memcpy((void *)(sg_list->buf), skb->data, sg_list->len);
			skb_pull(skb, sg_list->len);
		}
	}
}
#endif
