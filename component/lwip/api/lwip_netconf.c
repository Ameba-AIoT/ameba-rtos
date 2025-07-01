/* Includes ------------------------------------------------------------------*/
#include "lwip/prot/dhcp.h"
#include "lwip_netconf.h"
#include "atcmd_service.h"
#include "wifi_api.h"

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
#include "wifi_fast_connect.h"
#endif

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif


/*Static IP ADDRESS FOR ETHERNET*/
#ifndef ETH_IP_ADDR0
#define ETH_IP_ADDR0 192
#define ETH_IP_ADDR1 168
#define ETH_IP_ADDR2 0
#define ETH_IP_ADDR3 80
#endif

/*NETMASK FOR ETHERNET*/
#ifndef ETH_NETMASK_ADDR0
#define ETH_NETMASK_ADDR0 255
#define ETH_NETMASK_ADDR1 255
#define ETH_NETMASK_ADDR2 255
#define ETH_NETMASK_ADDR3 0
#endif

/*Gateway address for ethernet*/
#ifndef ETH_GW_ADDR0
#define ETH_GW_ADDR0 192
#define ETH_GW_ADDR1 168
#define ETH_GW_ADDR2 0
#define ETH_GW_ADDR3 1
#endif

/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES 5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef CONFIG_AS_INIC_AP
#include "whc_ipc.h"
#endif

#if defined(CONFIG_AS_INIC_NP) && defined(CONFIG_WHC_DUAL_TCPIP)
unsigned char ap_ip[4] = {192, 168, 43, 1}, ap_netmask[4] = {255, 255, 255, 0}, ap_gw[4] = {192, 168, 43, 1};
#endif

#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)

struct netif eth_netif;
extern err_t ethernetif_mii_init(struct netif *netif);
#endif
extern void (*p_wifi_join_info_free)(u8 iface_type);
struct netif xnetif[NET_IF_NUM]; /* network interface structure */
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */

int lwip_init_done = 0;

void LwIP_Init(void)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	int8_t idx = 0;
	u32 heap = rtos_mem_get_free_heap_size();

	/* Create tcp_ip stack thread */
	tcpip_init(NULL, NULL);

#ifdef LWIP_HOOK_TCP_ISN
	tcp_isn_init();
#endif

	/* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
	        struct ip_addr *netmask, struct ip_addr *gw,
	        void *state, err_t (* init)(struct netif *netif),
	        err_t (* input)(struct pbuf *p, struct netif *netif))

	Adds your network interface to the netif_list. Allocate a struct
	netif and pass a pointer to this structure as the first argument.
	Give pointers to cleared ip_addr structures when using DHCP,
	or fill them with sane numbers otherwise. The state pointer may be NULL.

	The init function pointer must point to a initialization function for
	your ethernet netif interface. The following code illustrates it's use.*/
	//RTK_LOGS(NOTAG, RTK_LOG_INFO, "NET_IF_NUM:%d\n\r",NET_IF_NUM);
	for (idx = 0; idx < NET_IF_NUM; idx++) {
		if (idx == 0) {
			IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
			IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
			IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		} else {
			IP4_ADDR(ip_2_ip4(&ipaddr), AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
			IP4_ADDR(ip_2_ip4(&netmask), AP_NETMASK_ADDR0, AP_NETMASK_ADDR1, AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
			IP4_ADDR(ip_2_ip4(&gw), AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
		}

		xnetif[idx].name[0] = 'r';
		xnetif[idx].name[1] = '0' + idx;

		netifapi_netif_add(&xnetif[idx], ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw), NULL, &ethernetif_init, &tcpip_input);

		RTK_LOGS(NOTAG, RTK_LOG_INFO, "interface %d is initialized\n", idx);

	}

#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
	eth_netif.name[0] = 'r';
	eth_netif.name[1] = '2';
	IP4_ADDR(ip_2_ip4(&ipaddr), ETH_IP_ADDR0, ETH_IP_ADDR1, ETH_IP_ADDR2, ETH_IP_ADDR3);
	IP4_ADDR(ip_2_ip4(&netmask), ETH_NETMASK_ADDR0, ETH_NETMASK_ADDR1, ETH_NETMASK_ADDR2, ETH_NETMASK_ADDR3);
	IP4_ADDR(ip_2_ip4(&gw), ETH_GW_ADDR0, ETH_GW_ADDR1, ETH_GW_ADDR2, ETH_GW_ADDR3);

	netifapi_netif_add(&eth_netif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw), NULL, &ethernetif_mii_init, &tcpip_input);

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "interface 2 is initialized\n");
#endif

	/*  Registers the default network interface. */
	netifapi_netif_set_default(&xnetif[0]);

	/*move these operations to wifi_on/wifi_off*/
#if 0
	/*  When the netif is fully configured this function must be called.*/
	for (idx = 0; idx < NET_IF_NUM; idx++) {
		netif_set_up(&xnetif[idx]);
	}
#endif

	lwip_init_done = 1;
	RTK_LOGS(TAG_WLAN_DRV, RTK_LOG_INFO, "LWIP consume heap %d\n", heap - rtos_mem_get_free_heap_size() - 4 * TCPIP_THREAD_STACKSIZE);
}

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
extern uint32_t offer_ip;
extern uint32_t server_ip;

#endif
/**
  * @brief  LwIP_DHCP_Process_Handle
  * @param  None
  * @retval None
  */
uint8_t LwIP_DHCP(uint8_t idx, uint8_t dhcp_state)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	uint32_t IPaddress;
	uint8_t iptab[4];
	uint8_t DHCP_state;
	struct netif *pnetif = NULL;
	struct dhcp *dhcp = NULL;

	DHCP_state = dhcp_state;

#if defined(CONFIG_LWIP_USB_ETHERNET_BRIDGE) && CONFIG_LWIP_USB_ETHERNET_BRIDGE
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "skip DHCP !!\n");
	return 0;
#endif

#if !((defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET))
	if (idx > 1) {
		idx = 1;
	}
#endif

	extern struct static_ip_config user_static_ip;
	if (user_static_ip.use_static_ip) {
		LwIP_SetIP(0, user_static_ip.addr, user_static_ip.netmask, user_static_ip.gw);
		iptab[3] = (uint8_t)(user_static_ip.addr >> 24);
		iptab[2] = (uint8_t)(user_static_ip.addr >> 16);
		iptab[1] = (uint8_t)(user_static_ip.addr >> 8);
		iptab[0] = (uint8_t)(user_static_ip.addr);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rSet Interface %d static IP : %d.%d.%d.%d\n", idx, iptab[3], iptab[2], iptab[1], iptab[0]);
		return 0;
	}

	pnetif = &xnetif[idx];

#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
	if (idx > 1) {
		pnetif = &eth_netif;
	}
#endif

	if (DHCP_state == 0) {
		ip_addr_set_zero(&pnetif->ip_addr);
		ip_addr_set_zero(&pnetif->netmask);
		ip_addr_set_zero(&pnetif->gw);
	}

	dhcp = ((struct dhcp *)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP));
	if (!netif_is_up(pnetif)) { // netif should be set up before doing dhcp request (in lwip v2.0.0)
		netifapi_netif_set_up(pnetif);
	}

	for (;;) {
		//RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r ========DHCP_state:%d============\n\r",DHCP_state);
		switch (DHCP_state) {
		case DHCP_START: {
			/*acqurie wakelock to guarantee dhcp*/
#ifndef CONFIG_AS_INIC_AP
			rtw_wakelock_timeout(4 * 1000);
#endif

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
			if (check_is_the_same_ap()) {
				if ((offer_ip != 0 && offer_ip != 0xFFFFFFFF) || (dhcp != NULL)) {
					if (dhcp == NULL) {
						dhcp = (struct dhcp *)mem_malloc(sizeof(struct dhcp));
						if (dhcp == NULL) {
							RTK_LOGS(NOTAG, RTK_LOG_ERROR, "dhcp_start(): could not allocate dhcp\n");
							return -1;
						}
					}
					memset(dhcp, 0, sizeof(struct dhcp));
					dhcp->offered_ip_addr.addr = (u32_t)offer_ip;

					ip_addr_set_ip4_u32(&dhcp->server_ip_addr, (u32_t)server_ip);

					netif_set_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP, dhcp);
				}
			} else {
				if (dhcp != NULL) {
					memset(dhcp, 0, sizeof(struct dhcp));
				}
			}

#endif
			netifapi_dhcp_start(pnetif);
			dhcp = ((struct dhcp *)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP));
			IPaddress = 0;
			DHCP_state = DHCP_WAIT_ADDRESS;
		}
		break;

		case DHCP_WAIT_ADDRESS: {
			/* If DHCP stopped by wifi_disconn_hdl*/
			if ((dhcp_state_enum_t)dhcp->state == DHCP_STATE_OFF) {
				IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
				IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
				IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
				netifapi_netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rLwIP_DHCP: dhcp stop.");
				return DHCP_STOP;
			}

			/* Read the new IP address */
			IPaddress = ip_addr_get_ip4_u32(netif_ip_addr4(pnetif));

			if (IPaddress != 0) {
#if LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS
				tcp_randomize_local_port();
				udp_randomize_local_port();
#endif
				DHCP_state = DHCP_ADDRESS_ASSIGNED;

				/* Stop DHCP */
				// dhcp_stop(pnetif);  /* can not stop, need to renew, Robbie*/

				iptab[0] = (uint8_t)(IPaddress >> 24);
				iptab[1] = (uint8_t)(IPaddress >> 16);
				iptab[2] = (uint8_t)(IPaddress >> 8);
				iptab[3] = (uint8_t)(IPaddress);

				at_printf_indicate("wifi got ip:\"%d.%d.%d.%d\"\r\n", iptab[3], iptab[2], iptab[1], iptab[0]);

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
				dhcp = ((struct dhcp *)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP));

				if (p_store_fast_connect_info) {
					p_store_fast_connect_info((uint32_t)dhcp->offered_ip_addr.addr, (uint32_t)ip_addr_get_ip4_u32(&dhcp->server_ip_addr));
				}
				if (p_wifi_join_info_free) {
					/* free key here after join success */
					p_wifi_join_info_free(IFACE_PORT0);
				}
#endif
				return DHCP_ADDRESS_ASSIGNED;
			} else {
				/* DHCP timeout */
				if (dhcp->tries > MAX_DHCP_TRIES) {
					DHCP_state = DHCP_TIMEOUT;
					/* Stop DHCP */
					netifapi_dhcp_stop(pnetif);

					/* Static address used */

					IP4_ADDR(ip_2_ip4(&ipaddr), STATIC_IP_ADDR0, STATIC_IP_ADDR1, STATIC_IP_ADDR2, STATIC_IP_ADDR3);
					IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
					IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
					netifapi_netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
					iptab[0] = STATIC_IP_ADDR3;
					iptab[1] = STATIC_IP_ADDR2;
					iptab[2] = STATIC_IP_ADDR1;
					iptab[3] = STATIC_IP_ADDR0;
					at_printf_indicate("wifi got ip timeout\r\n");
					RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rStatic IP address : %d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
					if (p_store_fast_connect_info) {
						p_store_fast_connect_info((uint32_t)dhcp->offered_ip_addr.addr, (uint32_t)ip_addr_get_ip4_u32(&dhcp->server_ip_addr));
					}
					if (p_wifi_join_info_free) {
						/* free key here after join success */
						p_wifi_join_info_free(IFACE_PORT0);
					}
#endif

#if (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_ETHERNET) && CONFIG_ETHERNET)
					if (idx > 1) { // This is the ethernet interface, set it up for static ip address
						netifapi_netif_set_up(pnetif);
					}
#endif
					return DHCP_TIMEOUT;
				}
			}
		}
		break;
		case DHCP_RELEASE_IP:
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rLwIP_DHCP: Release ip");
			netifapi_dhcp_release(pnetif);
			return DHCP_RELEASE_IP;
		case DHCP_STOP:
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rLwIP_DHCP: dhcp stop.");
			LwIP_DHCP_stop(idx);
			return DHCP_STOP;
		default:
			break;
		}
		/* wait 250 ms */
		rtos_time_delay_ms(10);
	}
}

void LwIP_ReleaseIP(uint8_t idx)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	struct netif *pnetif = &xnetif[idx];
	IP4_ADDR(ip_2_ip4(&ipaddr), 0, 0, 0, 0);
	IP4_ADDR(ip_2_ip4(&netmask), 255, 255, 255, 0);
	IP4_ADDR(ip_2_ip4(&gw), 0, 0, 0, 0);
	netifapi_netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
}

void LwIP_DHCP_stop(uint8_t idx)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	struct netif *pnetif = &xnetif[idx];
	netifapi_dhcp_stop(pnetif);

	IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
	IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
	netifapi_netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
}

int netif_get_idx(struct netif *pnetif)
{
#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER == 1)
	int idx = pnetif - xnetif;

	switch (idx) {
	case 0:
		return 0;
	case 1:
		return 1;
	case 2:
		return 2;
	default:
		return -1;
	}
#else
	UNUSED(pnetif);
	return -1;
#endif
}

void LwIP_netif_set_up(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	netifapi_netif_set_up(pnetif);
}

void LwIP_netif_set_down(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	netifapi_netif_set_down(pnetif);
}

void LwIP_netif_set_link_up(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	netifapi_netif_set_link_up(pnetif);
	if (idx == STA_WLAN_INDEX) {
		netifapi_netif_set_default(&xnetif[STA_WLAN_INDEX]);
	} else if ((idx == SOFTAP_WLAN_INDEX) && (!(xnetif[STA_WLAN_INDEX].flags & NETIF_FLAG_LINK_UP))) {
		/*default netif is on sta when sta and softap both up*/
		netifapi_netif_set_default(&xnetif[SOFTAP_WLAN_INDEX]);
	}
}

void LwIP_netif_set_link_down(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	netifapi_netif_set_link_down(pnetif);
	if (idx == SOFTAP_WLAN_INDEX) {
		netifapi_netif_set_default(&xnetif[STA_WLAN_INDEX]);
	} else if (idx == STA_WLAN_INDEX) {
		netifapi_netif_set_default(&xnetif[SOFTAP_WLAN_INDEX]);
	}
}

uint8_t *LwIP_GetMAC(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	return (uint8_t *)(pnetif->hwaddr);
}

uint8_t *LwIP_GetIP(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	return (uint8_t *) & (pnetif->ip_addr);
}

uint8_t *LwIP_GetGW(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	return (uint8_t *) & (pnetif->gw);
}

uint8_t *LwIP_GetMASK(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	return (uint8_t *) & (pnetif->netmask);
}

void LwIP_wlan_set_netif_info(int idx_wlan, void *dev, unsigned char *dev_addr)
{
	memcpy(xnetif[idx_wlan].hwaddr, dev_addr, 6);
	xnetif[idx_wlan].state = dev;
}

void LwIP_ethernetif_recv(uint8_t idx, int total_len)
{
	ethernetif_recv(&xnetif[idx], total_len);
}

SRAM_WLAN_CRITICAL_CODE_SECTION
void LwIP_ethernetif_recv_inic(uint8_t idx, struct pbuf *p_buf)
{
	err_enum_t error = ERR_OK;
	error = xnetif[idx].input(p_buf, &xnetif[idx]);
	if (error != ERR_OK) {
		RTK_LOGS(TAG_WLAN_INIC, RTK_LOG_ERROR, "lwip input err (%d)\n", error);
		pbuf_free(p_buf);
	}
}

int LwIP_netif_is_valid_IP(int idx, unsigned char *ip_dest)
{
#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER == 1)
	struct netif *pnetif = &xnetif[idx];

	ip_addr_t addr = { 0 };
	u32_t ip_dest_addr = { 0 };

	memcpy(&ip_dest_addr, ip_dest, 4);

	ip_addr_set_ip4_u32(&addr, ip_dest_addr);

	if ((ip_addr_get_ip4_u32(netif_ip_addr4(pnetif))) == 0) {
		return 1;
	}

	if (ip_addr_ismulticast(&addr) || ip_addr_isbroadcast(&addr, pnetif)) {
		return 1;
	}

	//if(ip_addr_netcmp(&(pnetif->ip_addr), &addr, &(pnetif->netmask))) //addr&netmask
	//	return 1;

	if (ip_addr_cmp(&(pnetif->ip_addr), &addr)) {
		return 1;
	}

	//RTK_LOGS(NOTAG, RTK_LOG_INFO, "invalid IP: %d.%d.%d.%d ",ip_dest[0],ip_dest[1],ip_dest[2],ip_dest[3]);
#endif
	UNUSED(idx);
	UNUSED(ip_dest);
	return 0;
}

#if LWIP_DNS
void LwIP_GetDNS(struct ip_addr *dns)
{
	struct ip_addr *tmp = (struct ip_addr *)dns_getserver(0);
	*dns = *tmp;
}

void LwIP_SetDNS(struct ip_addr *dns)
{
	dns_setserver(0, dns);
}
#endif

void LwIP_SetIP(uint8_t idx, u32_t addr, u32_t netmask_addr, u32_t gw_addr)
{
	struct netif *pnetif = &xnetif[idx];
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;

#if CONFIG_WLAN
	ip_2_ip4(&ipaddr)->addr = PP_HTONL(addr);
	ip_2_ip4(&netmask)->addr = PP_HTONL(netmask_addr);
	ip_2_ip4(&gw)->addr = PP_HTONL(gw_addr);
	netifapi_netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
#endif
}

#if LWIP_AUTOIP
#include <lwip/autoip.h>
#include <lwip/prot/autoip.h>

void LwIP_AUTOIP(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	uint8_t *ip = LwIP_GetIP(idx);
	struct autoip *autoip = NULL;

	autoip = ((struct autoip *)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_AUTOIP));
	if (autoip && (autoip->tried_llipaddr >= MAX_CONFLICTS)) { // before autoip_start(), autoip may be NULL
		autoip->tried_llipaddr = 0;
	}

	netifapi_autoip_start(pnetif);

	autoip = ((struct autoip *)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_AUTOIP));

	while ((autoip->state == AUTOIP_STATE_PROBING) || (autoip->state == AUTOIP_STATE_ANNOUNCING)) {
		rtos_time_delay_ms(1000);
	}

	if (*((uint32_t *) ip) == 0) {
		struct ip_addr ipaddr;
		struct ip_addr netmask;
		struct ip_addr gw;

		RTK_LOGS(NOTAG, RTK_LOG_INFO, "AUTOIP timeout\n");

		/* Static address used */
		IP4_ADDR(ip_2_ip4(&ipaddr), STATIC_IP_ADDR0, STATIC_IP_ADDR1, STATIC_IP_ADDR2, STATIC_IP_ADDR3);
		IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
		netifapi_netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Static IP address : %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nLink-local address: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
	}
}

void LwIP_AUTOIP_STOP(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	netifapi_autoip_stop(pnetif);
}
#endif
#if LWIP_IPV6
/* Get IPv6 address with lwip 1.5.0 */
void LwIP_AUTOIP_IPv6(struct netif *pnetif)
{
	uint8_t *ipv6 = (uint8_t *) netif_ip6_addr(pnetif, 0)->addr;
	netif_create_ip6_linklocal_address(pnetif, 1);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nIPv6 link-local address: %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x\n",
			 ipv6[0], ipv6[1],  ipv6[2],  ipv6[3],  ipv6[4],  ipv6[5],  ipv6[6], ipv6[7],
			 ipv6[8], ipv6[9], ipv6[10], ipv6[11], ipv6[12], ipv6[13], ipv6[14], ipv6[15]);
}
#endif

//To check successful WiFi connection and obtain of an IP address
void LwIP_Check_Connectivity(void)
{
	u8 join_status = RTW_JOINSTATUS_UNKNOWN;
	rtos_time_delay_ms(2000);
	while (!((wifi_get_join_status(&join_status) == RTK_SUCCESS)
			 && (join_status == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != IP_ADDR_INVALID))) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Wait for WiFi and DHCP Connect Success...\n");
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Please use AT+WLCONN to connect AP first time\n");
		rtos_time_delay_ms(2000);
	}
}
