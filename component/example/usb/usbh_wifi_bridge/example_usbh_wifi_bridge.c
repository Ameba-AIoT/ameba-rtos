/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "lwip/prot/tcp.h"
#include "lwip_netconf.h"
#include "os_wrapper.h"
#include "os_wrapper_time.h"
#include "usbh.h"
#include "usbh_cdc_ecm.h"
#include "wifi_api.h"

/* Private defines -----------------------------------------------------------*/
extern void rltk_usb_eth_init(void);

#define ENABLE_USBH_CDC_ECM_HOT_PLUG            1     /* Hot plug */

#define ECMBDEBUG  0

#define BOOTP_PORT		(68)
#define ETH_ILEN		(6)

struct eth_addr host_mac = {{0x00}}; // mac of device connected to usb

static const char *const TAG = "ECMB";

/* Private types -------------------------------------------------------------*/
#define USBH_ECM_MAIN_THREAD_PRIORITY           5
#define USBH_ECM_HOTPLUG_THREAD_PRIORITY        6
#define USBH_ECM_ISR_PRIORITY                   INT_PRI_MIDDLE

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
int dhcp_done = 0;

extern struct netif *pnetif_usb_eth;

static usb_os_sema_t cdc_ecm_detach_sema;
#if ENABLE_USBH_CDC_ECM_HOT_PLUG
rtos_task_t 	            hotplug_task;
#endif

/* Private function prototypes -----------------------------------------------*/
static int cdc_ecm_cb_init(void);
static int cdc_ecm_cb_deinit(void);
static int cdc_ecm_cb_attach(void);
static int cdc_ecm_cb_detach(void);
static int cdc_ecm_cb_setup(void);
static int cdc_ecm_cb_process(usb_host_t *host, u8 msg);
static int cdc_ecm_cb_bulk_receive(u8 *pbuf, u32 Len);
static int cdc_ecm_cb_device_check(usb_host_t *host, u8 cfg_max);

static usbh_config_t usbh_ecm_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = 0, //USBH_SOF_INTR
	.isr_priority = USBH_ECM_ISR_PRIORITY,
	.main_task_priority = USBH_ECM_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
	.hub_support = 1U,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_cdc_ecm_state_cb_t cdc_ecm_usb_cb = {
	.init   = cdc_ecm_cb_init,
	.deinit = cdc_ecm_cb_deinit,
	.attach = cdc_ecm_cb_attach,
	.detach = cdc_ecm_cb_detach,
	.setup  = cdc_ecm_cb_setup,
	.bulk_received = cdc_ecm_cb_bulk_receive,
};

static usbh_user_cb_t usbh_ecm_usr_cb = {
	.process = cdc_ecm_cb_process,
	.validate = cdc_ecm_cb_device_check,
};

static int cdc_ecm_cb_device_check(usb_host_t *host, u8 cfg_max)
{
	UNUSED(cfg_max);
	return usbh_cdc_ecm_check_config_desc(host);
}

static int cdc_ecm_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return HAL_OK;
}

static int cdc_ecm_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	return HAL_OK;
}

static int cdc_ecm_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	return HAL_OK;
}

static int cdc_ecm_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if ENABLE_USBH_CDC_ECM_HOT_PLUG
	usb_os_sema_give(cdc_ecm_detach_sema);
#endif
	return HAL_OK;
}

static int cdc_ecm_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	return HAL_OK;
}

static int cdc_ecm_cb_bulk_receive(u8 *buf, u32 length)
{
	if (length > 0) {
		ethernetif_usb_eth_recv(buf, length);
	}

	return HAL_OK;
}

static int cdc_ecm_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);
	switch (msg) {
	case USBH_MSG_USER_SET_CONFIG:
		usbh_cdc_ecm_choose_config(host);
		break;
	case USBH_MSG_DISCONNECTED:
		// usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

static int cdc_ecm_do_init(void)
{
	int ret = 0;

	ret = usbh_init(&usbh_ecm_cfg, &usbh_ecm_usr_cb);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
		return 0;
	}

	ret = usbh_cdc_ecm_init(&cdc_ecm_usb_cb, NULL);
	if (ret < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ECM fail\n");
		usbh_deinit();
		return 0;
	}

	do {
		if (usbh_cdc_ecm_usb_is_ready()) {
			break;
		}
		rtos_time_delay_ms(1000);
	} while (1); //wait usb init success

	return 1;
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

	if (protocol == lwip_htons(ETHTYPE_IP)) {
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
	} else if (protocol == lwip_htons(ETHTYPE_ARP)) {
		arph = (struct etharp_hdr *)((u8 *)p->payload + ETH_HLEN);
		src_ip = (u8 *) & (arph->sipaddr);
		dst_ip = (u8 *) & (arph->dipaddr);
		//RTK_LOGS(TAG, RTK_LOG_INFO, "%s(%d), dstip:%d,%d,%d,%d\n", __func__, __LINE__, (uint8_t) * (dst_ip + 0), (uint8_t) * (dst_ip + 1), (uint8_t) * (dst_ip + 2),
		//			(uint8_t) * (dst_ip + 3));
	}

	//RTK_LOGS(TAG, RTK_LOG_INFO, "%s(%d)flags=%x, smac:%02x:%02x:%02x:%02x:%02x:%02x, dmac:%02x:%02x:%02x:%02x:%02x:%02x, p->len = %d\n", __func__, __LINE__, flags,
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
	if (pattrib->protocol == lwip_htons(ETHTYPE_ARP)) {
		memcpy(&host_mac, (u8 *)p->payload + ETH_ALEN, ETH_ALEN);
	}

	memcpy((u8 *)p->payload + ETH_ALEN, pnetif_sta->hwaddr, ETH_ALEN);
#if ECMBDEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "%s(%d) src_port = %d\n", __func__, __LINE__, pattrib->src_port);

	for (int i = 0; i < p->len; i++) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", *((u8 *)p->payload + i));
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
#endif

	/* send to etharp_output */
	if (pattrib->protocol == lwip_htons(ETHTYPE_IP)) {
	} else if (pattrib->protocol == lwip_htons(ETHTYPE_ARP)) {
		memcpy((u8 *)p->payload + ETH_ALEN + 16, pnetif_sta->hwaddr, ETH_ALEN);
	}

	pnetif_sta->linkoutput(pnetif_sta, p);

	return 0;
}

static u32_t send_to_usb(pkt_attrib_t *pattrib, struct pbuf *p)
{
	UNUSED(pattrib);

	pnetif_usb_eth->linkoutput(pnetif_usb_eth, p);

	return 0;
}


static err_t usb_in_wifi_out(struct pbuf *p, struct netif *netif)
{
	pkt_attrib_t *pattrib;

	if (p == NULL || netif == NULL) {
		return ERR_VAL;
	}

	pattrib = (pkt_attrib_t *)malloc(sizeof(pkt_attrib_t));
	get_packet_attrib(p, pattrib);

	//RTK_LOGS(TAG, RTK_LOG_INFO, "%s(%d) portnum=%d, protocol=0x%x\n", __FUNCTION__, __LINE__, netif->num, lwip_ntohs(pattrib->protocol));

	if (pattrib->protocol == lwip_htons(ETHTYPE_IPV6)) {
		pbuf_free(p);
		free(pattrib);
		return ERR_OK;
	}
	pattrib->port_idx = netif->num;

	//RTK_LOGS(TAG, RTK_LOG_INFO, "%s(%d): port_num:%d, protocol:%x, dst:%02x:%02x:%02x:%02x:%02x:%02x, src:%02x:%02x:%02x:%02x:%02x:%02x\n",
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
		return ERR_VAL;
	}

	pattrib = (pkt_attrib_t *)malloc(sizeof(pkt_attrib_t));
	get_packet_attrib(p, pattrib);
	//RTK_LOGS(TAG, RTK_LOG_INFO, "%s(%d) portnum=%d, protocol=0x%x\n", __FUNCTION__, __LINE__, netif->num, lwip_ntohs(pattrib->protocol));

	if (pattrib->protocol == lwip_htons(ETHTYPE_IPV6)) {
		pbuf_free(p);
		free(pattrib);
		return ERR_OK;
	}
	pattrib->port_idx = netif->num;

#if ECMBDEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "%s(%d): port_num:%d, protocol:%x, dst:%02x:%02x:%02x:%02x:%02x:%02x, src:%02x:%02x:%02x:%02x:%02x:%02x\n",
			 __func__, __LINE__, pattrib->port_idx, pattrib->protocol, pattrib->dst_mac.addr[0], pattrib->dst_mac.addr[1], pattrib->dst_mac.addr[2],
			 pattrib->dst_mac.addr[3], pattrib->dst_mac.addr[4], pattrib->dst_mac.addr[5], pattrib->src_mac.addr[0], pattrib->src_mac.addr[1],
			 pattrib->src_mac.addr[2], pattrib->src_mac.addr[3], pattrib->src_mac.addr[4], pattrib->src_mac.addr[5]);

	for (int i = 0; i < p->len; i++) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", *((u8 *)p->payload + i));
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
#endif

	if (pattrib->protocol == lwip_htons(ETHTYPE_ARP)) {
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
	if (cdc_ecm_do_init() == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB init fail\n");

		rtos_task_delete(NULL);
	}

	while (1) {
		link_is_up = usbh_cdc_ecm_get_connect_status();

		if (1 == link_is_up && (ethernet_unplug < ETH_STATUS_INIT)) {	// unlink -> link
			ethernet_unplug = ETH_STATUS_INIT;
			netif_set_link_up(pnetif_usb_eth);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Swicth to link !!\n");
		} else if (0 == link_is_up && (ethernet_unplug >= ETH_STATUS_INIT)) {	// link -> unlink
			ethernet_unplug = ETH_STATUS_DEINIT;
			netif_set_default(pnetif_sta);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Swicth to unlink !!\n");
		} else {
			rtos_time_delay_ms(1000);
		}
	}
}

static void ecm_example_bridge_thread(void *param)
{
	UNUSED(param);

	RTK_LOGS(TAG, RTK_LOG_INFO, "Bridge example \n");

	while (LwIP_Check_Connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Wait for WIFI connection ...\n");
		RTK_LOGS(TAG, RTK_LOG_INFO, "Please use AT+WLCONN=ssid,xx,pw,yy to connect AP first time\n");
		rtos_time_delay_ms(2000);
	}

	pnetif_sta->input = wifi_in_usb_out;                    // station netif
	pnetif_usb_eth->input = usb_in_wifi_out;                    // ethernet netif

	rtos_task_delete(NULL);
}

#if ENABLE_USBH_CDC_ECM_HOT_PLUG
static void ecm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		usb_os_sema_take(cdc_ecm_detach_sema, USB_OS_SEMA_TIMEOUT);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Hot plug\n");
		//stop isr
		usbh_cdc_ecm_deinit();
		usbh_deinit();

		rtos_time_delay_ms(10);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap size: 0x%08x\n", usb_os_get_free_heap_size());

		ret = usbh_init(&usbh_ecm_cfg, &usbh_ecm_usr_cb);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
			break;
		}

		ret = usbh_cdc_ecm_init(&cdc_ecm_usb_cb, NULL);
		if (ret < 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init CDC ECM fail\n");
			usbh_deinit();
			break;
		}
	}
}
#endif

/* Exported functions --------------------------------------------------------*/
void example_usbh_wifi_bridge(void)
{
	int status;
	rtos_task_t monitor_task;
	rtos_task_t bridge_task;

	usb_os_sema_create(&cdc_ecm_detach_sema);
	rltk_usb_eth_init();

	RTK_LOGS(TAG, RTK_LOG_INFO, "USB host usbh_wifi_bridge demo started\n");

#if ENABLE_USBH_CDC_ECM_HOT_PLUG
	status = rtos_task_create(&hotplug_task, "usbh_ecm_hotplug_thread", ecm_hotplug_thread, NULL, 1024U, USBH_ECM_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug check task fail\n");
	}
#endif

	status = rtos_task_create(&monitor_task, "ecm_example_link_change_thread", ecm_example_monitor_link_change_thread, NULL, 1024U * 2, 3U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create USB host monitor_link_change thread: %d\n", status);
	}

	status = rtos_task_create(&bridge_task, "cdc_ecm_bridge_task", ecm_example_bridge_thread, NULL, 1024U * 2, 2U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create USBH cdc_ecm_bridge_task thread\n");
	}
}
