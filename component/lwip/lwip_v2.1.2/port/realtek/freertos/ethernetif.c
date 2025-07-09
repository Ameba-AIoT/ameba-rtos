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

#include "lwip_netconf.h"
#include "wifi_api.h"

#if CONFIG_WLAN
#include <wifi_intf_drv_to_lwip.h>
#endif

#if defined(CONFIG_INIC_HOST) && CONFIG_INIC_HOST
#include "freertos/inic_intf.h"
#endif

#if defined(CONFIG_WHC_HOST)
#if defined(CONFIG_WHC_INTF_SPI)
#include "whc_spi_host_trx.h"
#elif defined(CONFIG_WHC_INTF_SDIO)
#include "whc_sdio_host_trx.h"
#elif defined(CONFIG_WHC_INTF_IPC)
#include "whc_ipc_host_trx.h"
#endif
#endif

extern struct netif xnetif[NET_IF_NUM];
extern struct netif eth_netif;
extern int rltk_mii_send(struct pbuf * p);

#if defined(CONFIG_BRIDGE) && CONFIG_BRIDGE
extern unsigned char get_bridge_portnum(void);
#endif

static const char *const TAG = "ETHERNET";
#define ETHERNET_DEBUG		(0)
#define RTK_LOG_ETHERNET(format, ...) do {               \
        if ( ETHERNET_DEBUG ) DiagPrintf(format, ##__VA_ARGS__); \
    } while(0);

#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
#define MAX_BUFFER_SIZE		(1536)
#define DST_MAC_LEN			(6)
#define SRC_MAC_LEN			(6)
#define PROTO_TYPE_LEN		(2)  // protocol type
#define IP_LEN_OFFSET		(2)  // offset of total length field in IP packet

static rtos_mutex_t mii_tx_mutex;
static u8 TX_BUFFER[MAX_BUFFER_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 RX_BUFFER[MAX_BUFFER_SIZE];

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
SRAM_WLAN_CRITICAL_CODE_SECTION
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	/* Refer to eCos lwip eth_drv_send() */
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	int sg_len = 0;
	struct pbuf *q;
	struct eth_hdr *ethhdr = NULL;
	u8 is_special_pkt = 0;
	u8 *addr = (u8 *)p->payload;
#if defined(CONFIG_WHC_HOST)
	int ret = 0;
#endif

#if CONFIG_WLAN
#ifndef CONFIG_WHC_HOST
	if (!wifi_is_running(netif_get_idx(netif))) {
		return ERR_IF;
	}
#endif
#endif

	if (p->len >= ETH_HLEN + 24) {
		ethhdr = (struct eth_hdr *)p->payload;
		if (ETHTYPE_IP == _htons(ethhdr->type)) {
			addr += ETH_HLEN;
			if (((addr[21] == 68) && (addr[23] == 67)) ||
				((addr[21] == 67) && (addr[23] == 68))) {
				// DHCP packet, 68 : UDP BOOTP client, 67 : UDP BOOTP server
				is_special_pkt = 1;
			}
		}
	}

	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
		sg_list[sg_len].buf = (unsigned int) q->payload;
		sg_list[sg_len++].len = q->len;
	}

	if (sg_len) {
#if CONFIG_WLAN
#if defined(CONFIG_WHC_HOST)
		ret = whc_host_send(netif_get_idx(netif), sg_list, sg_len, p->tot_len, NULL, is_special_pkt);
		if (ret == ERR_IF) {
			return ret;
		}
		if (ret == 0)
#else
		if (rltk_wlan_send(netif_get_idx(netif), sg_list, sg_len, p->tot_len, is_special_pkt) == 0)
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

#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)

#if defined(CONFIG_ETHERNET_RMII) && CONFIG_ETHERNET_RMII
	(void) TX_BUFFER;
	if (p->tot_len) {
		if (rltk_mii_send(p) != 0) {
			return ERR_BUF;
		}
	}
#else
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
#endif
	return ERR_OK;
}

void rltk_mii_init(void)
{
#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
	if(mii_tx_mutex == NULL) {
		rtos_mutex_create(&mii_tx_mutex);
	}
#endif
}

void rltk_mii_deinit(void)
{
#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
	if(mii_tx_mutex) {
		rtos_mutex_delete(mii_tx_mutex);
		mii_tx_mutex = NULL;
	}
#endif
}

void rltk_mii_recv(struct eth_drv_sg *sg_list, int sg_len)
{
	UNUSED(sg_list);
	UNUSED(sg_len);
#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
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

u8 rltk_mii_recv_data_check(u8 *mac)
{
	UNUSED(mac);
	u8 check_res = TRUE;
#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
#if defined(CONFIG_LWIP_USB_ETHERNET_BRIDGE) && CONFIG_LWIP_USB_ETHERNET_BRIDGE
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

struct pbuf * ethernetif_rmii_buf_copy(u32 frame_len, u8 *src_buf)
{
#if (defined(CONFIG_ETHERNET_RMII) && CONFIG_ETHERNET_RMII)
	struct pbuf *p, *q;
	int sg_len = 0;

	if (frame_len > MAX_BUFFER_SIZE) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "recv data len error, len=%d\n", frame_len);
		return NULL;
	}

	// Allocate buffer to store received packet
	p = pbuf_alloc(PBUF_RAW, frame_len, PBUF_POOL);
	if (p == NULL) {
		RTK_LOGW(TAG, "\n\r[%s]Cannot allocate pbuf to receive packet(%d)\n", __func__, frame_len);
		return NULL;
	}

	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
		memcpy((void *)(q->payload), src_buf, q->len);
		src_buf += q->len;
		sg_len++;
	}

	return p;
#else
	(void) frame_len;
	(void) src_buf;
	return NULL;
#endif
}

void ethernetif_rmii_netif_recv(struct pbuf *p)
{
#if (defined(CONFIG_ETHERNET_RMII) && CONFIG_ETHERNET_RMII)
	struct netif *netif = &eth_netif;

	if (p == NULL) {
		return;
	}
	// Pass received packet to the interface
	if (ERR_OK != netif->input(p, netif)) {
		pbuf_free(p);
	}
#else
	(void) p;
#endif
}

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

void ethernetif_mii_recv(u8 *buf, u32 frame_len)
{
	(void) buf;
	(void) frame_len;
#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	struct pbuf *p, *q;
	int sg_len = 0;
	u32 total_len = 0;

	struct netif *netif = &eth_netif;
	u8 *macstr = (u8 *)(netif->hwaddr);

	if (frame_len > MAX_BUFFER_SIZE) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "recv data len is %d\n", frame_len);
		return;
	}

	if(0 == frame_len) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "recv data len is 0\n");
		return;
	}
	total_len = frame_len;
	memcpy((u8*)RX_BUFFER, buf, frame_len);

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