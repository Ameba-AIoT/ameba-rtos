/**
 * @file
 * Ethernet Interface Skeleton
 *
 */

/*
 * Copyright (c) 2001-2004 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/*
 * This file is a skeleton for developing Ethernet network interface
 * drivers for lwIP. Add code to the low_level functions and do a
 * search-and-replace for the word "ethernetif" to replace it with
 * something that better describes your network interface.
 */

#include "platform_autoconf.h"
#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "lwip/icmp.h"
#include "netif/etharp.h"
#include "lwip/err.h"
#include "ethernetif.h"
#include "lwip_netconf.h"

#include "lwip/ethip6.h" //Add for ipv6

#include "wifi_conf.h"
#include "platform_stdlib.h"
#include "basic_types.h"

#if CONFIG_WLAN
#include <wifi_intf_drv_to_lwip.h>
#endif

#if defined(CONFIG_INIC_HOST) && CONFIG_INIC_HOST
#include "freertos/inic_intf.h"
#endif

#if defined(CONFIG_AS_INIC_AP)
#include "inic_ipc_host_trx.h"
#endif

#define netifMTU                                (1500)
#define netifINTERFACE_TASK_STACK_SIZE        ( 350 )
#define netifINTERFACE_TASK_PRIORITY        ( configMAX_PRIORITIES - 1 )
#define netifGUARD_BLOCK_TIME            ( 250 )
/* The time to block waiting for input. */
#define emacBLOCK_TIME_WAITING_FOR_INPUT    ( ( portTickType ) 100 )

#define IF2NAME0 'r'
#define IF2NAME1 '2'

extern struct netif xnetif[NET_IF_NUM];
extern struct netif eth_netif;
extern signed char rltk_mii_send(struct eth_drv_sg *sg_list, int sg_len, int total_len);

static void arp_timer(void *arg);
#if defined(CONFIG_BRIDGE) && CONFIG_BRIDGE
extern unsigned char get_bridge_portnum(void);
#endif

static const char *TAG = "ETHERNET";
#define ETHERNET_DEBUG		(0)
#define RTK_LOG_ETHERNET(format, ...) do {               \
        if ( ETHERNET_DEBUG ) DiagPrintf(format, ##__VA_ARGS__); \
    } while(0);

#if defined(CONFIG_ETHERNET) && CONFIG_ETHERNET
#define MAX_BUFFER_SIZE		(1536)
#define DST_MAC_LEN			(6)
#define SRC_MAC_LEN			(6)
#define PROTO_TYPE_LEN		(2)  // protocol type
#define IP_LEN_OFFSET		(2)  // offset of total length field in IP packet
#define ETHERNET_REASSEMBLE_PACKET	(0)

static rtos_mutex_t mii_tx_mutex;
static u8 TX_BUFFER[MAX_BUFFER_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));;
static u8 RX_BUFFER[MAX_BUFFER_SIZE];

#if defined(ETHERNET_REASSEMBLE_PACKET) && ETHERNET_REASSEMBLE_PACKET
static u32 pkt_total_len = 0;
static u32 rx_buffer_saved_data_len = 0;
static u16 eth_type = 0;
#endif

extern int usbh_cdc_ecm_send_data(u8 *buf, u32 len);
extern u16 usbh_cdc_ecm_get_receive_mps(void);
#endif

/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */

static void low_level_init(struct netif *netif)
{

	/* set netif MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set netif maximum transfer unit */
	netif->mtu = 1500;

	/* Accept broadcast address and ARP traffic */
	netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

#if LWIP_IGMP
	/* make LwIP_Init do igmp_start to add group 224.0.0.1 */
	netif->flags |= NETIF_FLAG_IGMP;
#endif
#if defined(CONFIG_BRIDGE) && CONFIG_BRIDGE
	netif->flags |= NETIF_FLAG_ETHERNET;
#endif


#if LWIP_IPV6
#if LWIP_IPV6_MLD
	netif->flags |= NETIF_FLAG_MLD6;
#endif
#endif
	/* Wlan interface is initialized later */
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{


	/* Refer to eCos lwip eth_drv_send() */
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	int sg_len = 0;
	struct pbuf *q;
#if defined(CONFIG_AS_INIC_AP)
	int ret = 0;
	struct eth_hdr *ethhdr = NULL;
	u8 is_special_pkt = 0;
	u8 *addr = (u8 *)p->payload;
#endif

#if CONFIG_WLAN
#ifndef CONFIG_AS_INIC_AP
	if (!wifi_is_running(netif_get_idx(netif))) {
		return ERR_IF;
	}
#endif
#endif

#if defined(CONFIG_AS_INIC_AP)
	if (p->len >= ETH_HLEN + 24) {
		ethhdr = (struct eth_hdr *)p->payload;
		if (ETH_P_IP == _htons(ethhdr->type)) {
			addr += ETH_HLEN;
			if (((addr[21] == 68) && (addr[23] == 67)) ||
				((addr[21] == 67) && (addr[23] == 68))) {
				// DHCP packet, 68 : UDP BOOTP client, 67 : UDP BOOTP server
				is_special_pkt = 1;
			}
		}
	}
#endif

	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
		sg_list[sg_len].buf = (unsigned int) q->payload;
		sg_list[sg_len++].len = q->len;
	}

	if (sg_len) {
#if CONFIG_WLAN
#if defined(CONFIG_AS_INIC_AP)
		ret = inic_host_send(netif_get_idx(netif), sg_list, sg_len, p->tot_len, NULL, is_special_pkt);
		if (ret == ERR_IF) {
			return ret;
		}
		if (ret == 0)
#else
		if (rltk_wlan_send(netif_get_idx(netif), sg_list, sg_len, p->tot_len) == 0)
#endif
#elif CONFIG_INIC_HOST
		if (rltk_inic_send(sg_list, sg_len, p->tot_len) == 0)
#else
		if (1)
#endif
		{
			RTK_LOG_ETHERNET("%s[%d] ok\n", __func__, __LINE__);
			return ERR_OK;
		}
		else {
			RTK_LOG_ETHERNET("%s[%d] fail\n", __func__, __LINE__);
			return ERR_BUF;    // return a non-fatal error
		}
	}
	RTK_LOG_ETHERNET("%s[%d] ok\n", __func__, __LINE__);
	return ERR_OK;
}

/*for ethernet mii interface*/
static err_t low_level_output_mii(struct netif *netif, struct pbuf *p)
{
	(void) netif;
	(void) p;
	RTK_LOG_ETHERNET("%s %d \n", __func__, __LINE__);

#if defined(CONFIG_ETHERNET) && CONFIG_ETHERNET
	struct pbuf *q;
	u8 *pdata = TX_BUFFER;
	u32 size = 0;
	int ret = 0;

	memset(TX_BUFFER, 0, MAX_BUFFER_SIZE);
	for (q = p; q != NULL; q = q->next) {
		memcpy((unsigned int *)pdata, (unsigned int *) q->payload, q->len);
		pdata += q->len;
		size += q->len;
	}

	rtos_mutex_take(mii_tx_mutex, MUTEX_WAIT_TIMEOUT);
	ret = usbh_cdc_ecm_send_data(TX_BUFFER, size);
	rtos_mutex_give(mii_tx_mutex);

	if (ret != 0) {
		RTK_LOG_ETHERNET("%s error = %d\n", __func__, ret);
		return ERR_BUF;    // return a non-fatal error
	}
#endif
	return ERR_OK;
}


/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
//static struct pbuf * low_level_input(struct netif *netif){}


/**
 * This function is the ethernetif_input task, it is processed when a packet
 * is ready to be read from the interface. It uses the function low_level_input()
 * that should handle the actual reception of bytes from the network
 * interface. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
//void ethernetif_input( void * pvParameters )


/* Refer to eCos eth_drv_recv to do similarly in ethernetif_input */
void ethernetif_recv(struct netif *netif, int total_len)
{
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	struct pbuf *p, *q;
	int sg_len = 0;
#if CONFIG_WLAN
	if (!wifi_is_running(netif_get_idx(netif))) {
		return;
	}
#endif
#if defined(CONFIG_BRIDGE) && CONFIG_BRIDGE
	if (get_bridge_portnum() != (NET_IF_NUM - 1)) {
		// return if bridge not ready
		return;
	}
#endif


	if ((total_len > MAX_ETH_MSG) || (total_len < 0)) {
		total_len = MAX_ETH_MSG;
	}

	// Allocate buffer to store received packet
	p = pbuf_alloc(PBUF_RAW, total_len, PBUF_POOL);
	if (p == NULL) {
		RTK_LOGW(TAG, "\n\r[%s]Cannot allocate pbuf to receive packet(%d)\n", __func__, total_len);
		return;
	}

	// Create scatter list
	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
		sg_list[sg_len].buf = (unsigned int) q->payload;
		sg_list[sg_len++].len = q->len;
	}
	if (p->if_idx == NETIF_NO_INDEX) {
		p->if_idx = netif_get_index(netif);
	}

	// Copy received packet to scatter list from wrapper rx skb
	//RTK_LOGW(TAG, "\n\rwlan:%c: Recv sg_len: %d, tot_len:%d", netif->name[1],sg_len, total_len);
#if CONFIG_WLAN
	rltk_wlan_recv(netif_get_idx(netif), sg_list, sg_len);
#elif CONFIG_INIC_HOST
	rltk_inic_recv(sg_list, sg_len);
#endif
	// Pass received packet to the interface
	if (ERR_OK != netif->input(p, netif)) {
		pbuf_free(p);
	}

}

void rltk_mii_init(void)
{
#if defined(CONFIG_ETHERNET) && CONFIG_ETHERNET
	rtos_mutex_create(&mii_tx_mutex);
#endif
}

void rltk_mii_recv(struct eth_drv_sg *sg_list, int sg_len)
{
	UNUSED(sg_list);
	UNUSED(sg_len);
#if defined(CONFIG_ETHERNET) && CONFIG_ETHERNET
	struct eth_drv_sg *last_sg;
	u8 *pbuf = RX_BUFFER;

	for (last_sg = &sg_list[sg_len]; sg_list < last_sg; ++sg_list) {
		if (sg_list->buf != 0) {
			memcpy((void *)(sg_list->buf), pbuf, sg_list->len);
			pbuf += sg_list->len;
		}
	}
#endif
}

#if defined(ETHERNET_REASSEMBLE_PACKET) && ETHERNET_REASSEMBLE_PACKET
u8 rltk_mii_recv_data(u8 *buf, u32 frame_length, u32 *total_len)
{
	UNUSED(buf);
	UNUSED(total_len);
	UNUSED(frame_length);

	RTK_LOG_ETHERNET("enter %s %d\n", __func__, __LINE__);

#if defined(CONFIG_ETHERNET) && CONFIG_ETHERNET
	u8 *pbuf;
	u32 pkt_len_index = DST_MAC_LEN + SRC_MAC_LEN + PROTO_TYPE_LEN;
	u16 usb_receive_mps = usbh_cdc_ecm_get_receive_mps();	//only 512 bytes is supported now.

	if (0 == pkt_total_len) { //first packet
		pbuf = RX_BUFFER;
		if (frame_length > 0) {
			memcpy((void *)pbuf, buf, frame_length);
		}
		if ((0 == frame_length) || (frame_length % usb_receive_mps != 0)) { //should finish
			*total_len = frame_length;
			rx_buffer_saved_data_len = 0;
			pkt_total_len = 0;
			return TRUE;
		} else { //get the total length
			rx_buffer_saved_data_len = frame_length;
			//should check the vlan header
			eth_type = buf[DST_MAC_LEN + SRC_MAC_LEN] * 256 + buf[DST_MAC_LEN + SRC_MAC_LEN + 1];

			if (eth_type == ETH_P_IP) {
				pkt_total_len =  buf[pkt_len_index + IP_LEN_OFFSET] * 256 + buf[pkt_len_index + IP_LEN_OFFSET + 1];
			}
		}
	} else {
		if (rx_buffer_saved_data_len + frame_length > MAX_BUFFER_SIZE) {
			RTK_LOGS(NOTAG, "frame_length(%d) and rx_buffer_saved_data_len(%d) is too long, MAX_BUFFER_SIZE = %d !\n", frame_length, rx_buffer_saved_data_len,
					 MAX_BUFFER_SIZE);
			//drop packet
			rx_buffer_saved_data_len = 0;
		}

		pbuf = RX_BUFFER + rx_buffer_saved_data_len;
		if (frame_length > 0) {
			memcpy((void *)pbuf, buf, frame_length);
		}
		rx_buffer_saved_data_len += frame_length;
		if ((0 == frame_length) || (frame_length % usb_receive_mps != 0)) {
			//should finish
			*total_len = rx_buffer_saved_data_len;
			rx_buffer_saved_data_len = 0;
			pkt_total_len = 0;
			return TRUE;
		}
	}
#endif
	return FALSE;
}
#endif

u8 rltk_mii_recv_data_check(u8 *mac)
{
	UNUSED(mac);
	u8 check_res = TRUE;
#if defined(CONFIG_ETHERNET) && CONFIG_ETHERNET
#if defined(CONFIG_ETHERNET_BRIDGE) && CONFIG_ETHERNET_BRIDGE
	return check_res;
#else
	u8 *pbuf = RX_BUFFER;
	u8 multi_mac[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

	if (memcmp(mac, pbuf, ETH_ALEN) == 0 || memcmp(multi_mac, pbuf, ETH_ALEN) == 0) {
		check_res = TRUE;
	} else {
		check_res = FALSE;
	}
#endif

#endif
	return check_res;
}

void ethernetif_mii_recv(u8 *buf, u32 frame_len)
{
	(void) buf;
	(void) frame_len;
#if defined(CONFIG_ETHERNET) && CONFIG_ETHERNET
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	struct pbuf *p, *q;
	int sg_len = 0;
	u32 total_len = 0;

	struct netif *netif = &eth_netif;
	u8 *macstr = (u8 *)(netif->hwaddr);

	if (frame_len > MAX_BUFFER_SIZE) {
		RTK_LOGS(NOTAG, "recv data len is %d\n", frame_len);
		return;
	}

#if defined(ETHERNET_REASSEMBLE_PACKET) && ETHERNET_REASSEMBLE_PACKET 
	RTK_LOG_ETHERNET("%s %d will rltk_mii_recv_data\n", __func__, __LINE__);
	if (FALSE == rltk_mii_recv_data(buf, frame_len, &total_len)) {
		return;
	}
#else
	if(0 == frame_len) {
		RTK_LOGS(NOTAG, "recv data len is 0\n");
		return;
	}
	total_len = frame_len;
	memcpy((u8*)RX_BUFFER, buf, frame_len);
#endif
	if (FALSE == rltk_mii_recv_data_check(macstr)) {
		RTK_LOG_ETHERNET("rltk_mii_recv_data_check fail\n");
		for(u32 i = 0; i < total_len; i++)
		{
			RTK_LOG_ETHERNET("%02x ", *((u8*)RX_BUFFER + i));
		}
		RTK_LOG_ETHERNET("\n");
		return;
	}


	RTK_LOG_ETHERNET("%s %d rltk_mii_recv_data_check ok\n", __func__, __LINE__);

	// Allocate buffer to store received packet
	p = pbuf_alloc(PBUF_RAW, total_len, PBUF_POOL);
	if (p == NULL) {
		RTK_LOGW(TAG, "\n\r[%s]Cannot allocate pbuf to receive packet(%d)\n", __func__, total_len);
		return;
	}

	// Create scatter list
	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
		sg_list[sg_len].buf = (unsigned int) q->payload;
		sg_list[sg_len++].len = q->len;
	}
	rltk_mii_recv(sg_list, sg_len);

	// Pass received packet to the interface
	if (ERR_OK != netif->input(p, netif)) {
		pbuf_free(p);
	}
#endif

}
/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
	LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	if (netif->name[1] == '0') {
		netif->hostname = "lwip0";
	} else if (netif->name[1] == '1') {
		netif->hostname = "lwip1";
	}
#endif /* LWIP_NETIF_HOSTNAME */

	netif->output = etharp_output;
#if LWIP_IPV6
	netif->output_ip6 = ethip6_output;
	netif->ip6_autoconfig_enabled = 1;
#endif
	netif->linkoutput = low_level_output;

	/* initialize the hardware */
	low_level_init(netif);

	etharp_init();

	return ERR_OK;
}

err_t ethernetif_mii_init(struct netif *netif)
{
	LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
	netif->hostname = "lwip2";
#endif /* LWIP_NETIF_HOSTNAME */

	netif->output = etharp_output;
#if LWIP_IPV6
	netif->output_ip6 = ethip6_output;
#endif
	netif->linkoutput = low_level_output_mii;

	/* initialize the hardware */
	low_level_init(netif);

	etharp_init();

	return ERR_OK;
}

static void arp_timer(void *arg)
{
	(void) arg;
	etharp_tmr();
	sys_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
}

/*
 * For FreeRTOS tickless
 */
int lwip_tickless_used = 0;

int arp_timeout_exist(void)
{
	struct sys_timeouts *timeouts;
	struct sys_timeo *t;

	timeouts = sys_arch_timeouts();

	for (t = timeouts->next; t != NULL; t = t->next)
		if (t->h == arp_timer) {
			return 1;
		}

	return 0;
}

//Called by rltk_wlan_PRE_SLEEP_PROCESSING()
void lwip_PRE_SLEEP_PROCESSING(void)
{
	if (arp_timeout_exist()) {
		tcpip_untimeout(arp_timer, NULL);
	}
	lwip_tickless_used = 1;
}

//Called in ips_leave() path, support tickless when wifi power wakeup due to ioctl or deinit
void lwip_POST_SLEEP_PROCESSING(void)
{
	if (lwip_tickless_used) {
		tcpip_timeout(ARP_TMR_INTERVAL, arp_timer, NULL);
	}
}

