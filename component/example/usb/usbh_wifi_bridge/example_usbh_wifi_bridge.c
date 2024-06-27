/**
  ******************************************************************************
  * @file    example_usbh_cdc_ecm.c
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2023-1-13
  * @brief   This file provides the demo code for USB CDC ECM host class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "lwip/prot/tcp.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "os_wrapper_time.h"
#include "usbh.h"
#include "usbh_cdc_ecm_hal.h"
#include "wifi_intf_drv_to_app_basic.h"

/* Private defines -----------------------------------------------------------*/
extern void rltk_mii_init(void);


#define ECMBDEBUG  0

#define BOOTP_PORT		(68)
#define ETH_ILEN		(6)

struct eth_addr host_mac = {{0x00}}; // mac of device connected to usb

static const char *TAG = "ECMB";

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
int dhcp_done = 0;

extern struct netif xnetif[NET_IF_NUM];
extern struct netif eth_netif;

u8 cdc_ecm_do_init(void)
{
	return usbh_cdc_ecm_do_init(ethernetif_mii_recv);
}


/* Ethernet State */
typedef enum {
	ETH_STATUS_IDLE = 0U,
	ETH_STATUS_DEINIT,
	ETH_STATUS_INIT,
	ETH_STATUS_MAX,
} eth_state_t;

typedef struct pkt_attrib_s {
	u16_t protocol;
	u16_t src_port;
	u16_t dst_port;
	u8_t src_ip[4];
	u8_t dst_ip[4];
	u8_t flags;
	u8_t type;
	struct eth_addr src_mac;
	struct eth_addr dst_mac;
	u8_t port_idx;
} pkt_attrib_t;

static void get_packet_attrib(struct pbuf *p, pkt_attrib_t *pattrib)
{
	u16_t protocol, src_port = 0, dst_port = 0;
	u8_t type = 0;
	u8_t flags = 0;
	struct ip_hdr  *iph = NULL;
	struct etharp_hdr *arph = NULL;
	struct tcp_hdr *tcph = NULL;
	struct udp_hdr *udph = NULL;
	u8_t *src_ip = NULL, *dst_ip = NULL;
	struct eth_addr *src_addr, *dst_addr;

	dst_addr = (struct eth_addr *)((u8_t *)p->payload);
	src_addr = (struct eth_addr *)(((u8_t *)p->payload) + sizeof(struct eth_addr));
	protocol = *((unsigned short *)((u8 *)p->payload + 2 * ETH_ALEN));

	if (protocol == lwip_htons(ETH_P_IP)) {
		/* update src ip/mac mapping */
		iph = (struct ip_hdr *)((u8 *)p->payload + ETH_HLEN);
		src_ip = (u8_t *) & (iph->src.addr);
		dst_ip = (u8_t *) & (iph->dest.addr);
		type = iph->_proto;

		switch (iph->_proto) {
		case IP_PROTO_TCP://TCP
			tcph = (struct tcp_hdr *)((u8 *)p->payload + ETH_HLEN + sizeof(struct ip_hdr));
			if (tcph != NULL) {
				src_port = PP_NTOHS(tcph->src);
				dst_port = PP_NTOHS(tcph->dest);
				flags = TCPH_FLAGS(tcph);
			}
			break;
		case IP_PROTO_UDP://UDP
			udph = (struct udp_hdr *)((u8 *)p->payload + ETH_HLEN + sizeof(struct ip_hdr));
			if (udph != NULL) {
				src_port = PP_NTOHS(udph->src);
				dst_port = PP_NTOHS(udph->dest);
			}
			break;
		default: //ICMP
			src_port = 0;
			dst_port = 0;
			break;
		}
	} else if (protocol == lwip_htons(ETH_P_ARP)) {
		arph = (struct etharp_hdr *)((u8 *)p->payload + ETH_HLEN);
		src_ip = (u8 *) & (arph->sipaddr);
		dst_ip = (u8 *) & (arph->dipaddr);
		//RTK_LOGS(TAG, "[ECMB] %s(%d), dstip:%d,%d,%d,%d\n", __func__, __LINE__, (uint8_t) * (dst_ip + 0), (uint8_t) * (dst_ip + 1), (uint8_t) * (dst_ip + 2),
		//			(uint8_t) * (dst_ip + 3));
	}

	//RTK_LOGS(TAG, "[ECMB] %s(%d)flags=%x, smac:%02x:%02x:%02x:%02x:%02x:%02x, dmac:%02x:%02x:%02x:%02x:%02x:%02x, p->len = %d\n", __func__, __LINE__, flags,
	//			src_addr->addr[0], src_addr->addr[1], src_addr->addr[2], src_addr->addr[3], src_addr->addr[4], src_addr->addr[5],
	//			dst_addr->addr[0], dst_addr->addr[1], dst_addr->addr[2], dst_addr->addr[3], dst_addr->addr[4], dst_addr->addr[5], p->len);

	pattrib->protocol = protocol;
	pattrib->src_port = src_port;
	pattrib->dst_port = dst_port;
	pattrib->type = type;
	pattrib->flags = flags;
	memcpy(&pattrib->src_mac, src_addr, sizeof(struct eth_addr));
	memcpy(&pattrib->dst_mac, dst_addr, sizeof(struct eth_addr));
	if (src_ip != NULL) {
		memcpy(pattrib->src_ip, src_ip, ETH_ILEN);
	}
	if (dst_ip != NULL) {
		memcpy(pattrib->dst_ip, dst_ip, ETH_ILEN);
	}
}

static u32_t send_to_wifi(pkt_attrib_t *pattrib, struct pbuf *p)
{
	//RTK_LOGS(TAG, "[ECMB] %s(%d) out>toap\n", __func__, __LINE__);

	if (pattrib->protocol == lwip_htons(ETH_P_ARP)) {
		memcpy(&host_mac, (u8 *)p->payload + ETH_ALEN, ETH_ALEN);
	}

	memcpy((u8 *)p->payload + ETH_ALEN, xnetif[0].hwaddr, ETH_ALEN);
#if ECMBDEBUG
	RTK_LOGS(TAG, "[ECMB] %s(%d) src_port = %d\n", __func__, __LINE__, pattrib->src_port);

	for (int i = 0; i < p->len; i++) {
		RTK_LOGS(TAG, "%02x ", *((u8 *)p->payload + i));
	}
	RTK_LOGS(TAG, "\n");
#endif

	/* send to etharp_output */
	if (pattrib->protocol == lwip_htons(ETH_P_IP)) {
	} else if (pattrib->protocol == lwip_htons(ETH_P_ARP)) {
		memcpy((u8 *)p->payload + ETH_ALEN + 16, xnetif[0].hwaddr, ETH_ALEN);
	}

	xnetif[0].linkoutput(&xnetif[0], p);

	//RTK_LOGS(TAG, "[ECMB] %s[%d]\n", __func__, __LINE__);

	return 0;
}

static u32_t send_to_usb(pkt_attrib_t *pattrib, struct pbuf *p)
{
	UNUSED(pattrib);

	eth_netif.linkoutput(&eth_netif, p);

	//RTK_LOGS(TAG, "[ECMB] %s(%d) out>tousb\n", __func__, __LINE__);

	return 0;
}


static err_t usb_in_wifi_out(struct pbuf *p, struct netif *netif)
{
	pkt_attrib_t *pattrib;

	if (p == NULL || netif == NULL) {
		RTK_LOGS(TAG, "[ECMB] %s(%d)\n", __func__, __LINE__);
		return ERR_VAL;
	}

	pattrib = (pkt_attrib_t *)malloc(sizeof(pkt_attrib_t));
	get_packet_attrib(p, pattrib);

	//RTK_LOGS(TAG, "[ECMB] %s(%d) portnum=%d, protocol=0x%x\n", __FUNCTION__, __LINE__, netif->num, lwip_ntohs(pattrib->protocol));

	if (pattrib->protocol == lwip_htons(ETH_P_IPV6)) {
		pbuf_free(p);
		free(pattrib);
		return ERR_OK;
	}
	pattrib->port_idx = netif->num;

	//RTK_LOGS(TAG, "[ECMB] %s(%d): port_num:%d, protocol:%x, dst:%02x:%02x:%02x:%02x:%02x:%02x, src:%02x:%02x:%02x:%02x:%02x:%02x\n",
	//			__func__, __LINE__, pattrib->port_idx, pattrib->protocol, pattrib->dst_mac.addr[0], pattrib->dst_mac.addr[1], pattrib->dst_mac.addr[2],
	//			pattrib->dst_mac.addr[3], pattrib->dst_mac.addr[4], pattrib->dst_mac.addr[5], pattrib->src_mac.addr[0], pattrib->src_mac.addr[1],
	//			pattrib->src_mac.addr[2], pattrib->src_mac.addr[3], pattrib->src_mac.addr[4], pattrib->src_mac.addr[5]);

	send_to_wifi(pattrib, p);

	pbuf_free(p);
	free(pattrib);

	return ERR_OK;
}

static err_t wifi_in_usb_out(struct pbuf *p, struct netif *netif)
{
	pkt_attrib_t *pattrib;

	if (p == NULL || netif == NULL) {
		RTK_LOGS(TAG, "[ECMB] %s(%d)\n", __func__, __LINE__);
		return ERR_VAL;
	}

	pattrib = (pkt_attrib_t *)malloc(sizeof(pkt_attrib_t));
	get_packet_attrib(p, pattrib);
	//RTK_LOGS(TAG, "[ECMB] %s(%d) portnum=%d, protocol=0x%x\n", __FUNCTION__, __LINE__, netif->num, lwip_ntohs(pattrib->protocol));

	if (pattrib->protocol == lwip_htons(ETH_P_IPV6)) {
		pbuf_free(p);
		free(pattrib);
		return ERR_OK;
	}
	pattrib->port_idx = netif->num;

#if ECMBDEBUG
	RTK_LOGS(TAG, "[ECMB] %s(%d): port_num:%d, protocol:%x, dst:%02x:%02x:%02x:%02x:%02x:%02x, src:%02x:%02x:%02x:%02x:%02x:%02x\n",
			 __func__, __LINE__, pattrib->port_idx, pattrib->protocol, pattrib->dst_mac.addr[0], pattrib->dst_mac.addr[1], pattrib->dst_mac.addr[2],
			 pattrib->dst_mac.addr[3], pattrib->dst_mac.addr[4], pattrib->dst_mac.addr[5], pattrib->src_mac.addr[0], pattrib->src_mac.addr[1],
			 pattrib->src_mac.addr[2], pattrib->src_mac.addr[3], pattrib->src_mac.addr[4], pattrib->src_mac.addr[5]);

	for (int i = 0; i < p->len; i++) {
		RTK_LOGS(TAG, "%02x ", *((u8 *)p->payload + i));
	}
	RTK_LOGS(TAG, "\n");
#endif

	if (pattrib->protocol == lwip_htons(ETH_P_ARP)) {
		memcpy((u8 *)p->payload + ETH_HLEN + 18, &host_mac, ETH_ALEN);
	}

	//dst mac
	memcpy(p->payload, &host_mac, ETH_ALEN);

	send_to_usb(pattrib, p);

	pbuf_free(p);
	free(pattrib);
	return ERR_OK;
}

static void ecm_example_monitor_link_change_thread(void *param)
{
	UNUSED(param);

	u8 link_is_up = 0;
	eth_state_t ethernet_unplug = ETH_STATUS_IDLE;

	//todo: The usb will not be initialized at the beginning(need the cooperation of hw design)
	cdc_ecm_do_init();

	rltk_mii_init();

	while (1) {
		link_is_up = usbh_cdc_ecm_get_connect_status();

		if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {	// unlink -> link
			ethernet_unplug = ETH_STATUS_INIT;
			netif_set_link_up(&eth_netif);

		} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {	// link -> unlink
			ethernet_unplug = ETH_STATUS_DEINIT;
			netif_set_default(&xnetif[0]);
			RTK_LOGS(TAG, "[ECMB] Swicth to unlink !!\n");
		} else {
			rtos_time_delay_ms(1000);
		}
	}
}

static void ecm_example_bridge_thread(void *param)
{
	UNUSED(param);

	RTK_LOGS(TAG, "[ECMB] Bridge example \n");

	while (!(wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS)) {
		RTK_LOGS(TAG, "[ECMB] Wait for WIFI connection ...\n");
		RTK_LOGS(TAG, "[ECMB] Please use ATW0=ssid, ATW1=password, ATWC or AT+WLCONN to connect AP first time\n");
		rtos_time_delay_ms(2000);
	}

	xnetif[0].input = wifi_in_usb_out;                    // station netif
	eth_netif.input = usb_in_wifi_out;                    // ethernet netif

	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/
void example_usbh_wifi_bridge(void)
{
	int status;
	rtos_task_t monitor_task;
	rtos_task_t bridge_task;

	RTK_LOGS(TAG, "[ECMB] USB host usbh_wifi_bridge demo started\n");

	status = rtos_task_create(&monitor_task, "ecm_example_link_change_thread", ecm_example_monitor_link_change_thread, NULL, 1024U * 2, 3U);
	if (status != SUCCESS) {
		RTK_LOGS(TAG, "[ECMB] Fail to create USB host monitor_link_change thread: %d\n", status);
	}

	status = rtos_task_create(&bridge_task, "cdc_ecm_bridge_task", ecm_example_bridge_thread, NULL, 1024U * 2, 2U);
	if (status != SUCCESS) {
		RTK_LOGS(TAG, "[ECMB] Fail to create USBH cdc_ecm_bridge_task thread\n");
	}
}
