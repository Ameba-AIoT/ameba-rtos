#include "netif.h"
#include "wwd_network.h"
#include "wwd_management.h"
#include "wwd_wifi.h"
#include "platform_autoconf.h"
#include "opt.h"
#include "def.h"
#include "mem.h"
#include "pbuf.h"
#include "sys.h"
#include "tcpip.h"
#include "icmp.h"
#include "etharp.h"
#include "err.h"
#include "netifapi.h"

#include "rtw_whc_common.h"

#define ETHERNET_DEBUG		(0)
#define RTK_LOG_ETHERNET(format, ...) do {               \
				if ( ETHERNET_DEBUG ) printf(format, ##__VA_ARGS__); \
			} while(0);


unsigned char ap_ip[4] = {192, 168, 43, 1}, ap_netmask[4] = {255, 255, 255, 0}, ap_gw[4] = {192, 168, 43, 1};
struct netif xnetif[NET_IF_NUM]; /* network interface structure */

#define IP_ADDR0   0
#define IP_ADDR1   0
#define IP_ADDR2   0
#define IP_ADDR3   0

/*Static IP ADDRESS*/
#define STATIC_IP_ADDR0   192
#define STATIC_IP_ADDR1   168
#define STATIC_IP_ADDR2   1
#define STATIC_IP_ADDR3   80

/*Static IP ADDRESS*/
#define AP_IP_ADDR0   ap_ip[0]
#define AP_IP_ADDR1   ap_ip[1]
#define AP_IP_ADDR2   ap_ip[2]
#define AP_IP_ADDR3   ap_ip[3]

/*NETMASK*/
#define AP_NETMASK_ADDR0   ap_netmask[0]
#define AP_NETMASK_ADDR1   ap_netmask[1]
#define AP_NETMASK_ADDR2   ap_netmask[2]
#define AP_NETMASK_ADDR3   ap_netmask[3]

/*Gateway Address*/
#define AP_GW_ADDR0   ap_gw[0]
#define AP_GW_ADDR1   ap_gw[1]
#define AP_GW_ADDR2   ap_gw[2]
#define AP_GW_ADDR3   ap_gw[3]

int lwip_init_done = 0;

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

int netif_get_idx(struct netif *pnetif)
{
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

}

#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{

	/* Refer to eCos lwip eth_drv_send() */
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	int sg_len = 0;
	struct pbuf *q;
#if defined(CONFIG_WHC_HOST)
	int ret = 0;
	struct eth_hdr *ethhdr = NULL;
	u8 is_special_pkt = 0;
	u8 *addr = (u8 *)p->payload;
#endif

#if CONFIG_WLAN
#ifndef CONFIG_WHC_HOST
	if (!wifi_is_running(netif_get_idx(netif))) {
		return ERR_IF;
	}
#endif
#endif

#if defined(CONFIG_WHC_HOST)
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
#if defined(CONFIG_WHC_HOST)
		ret = whc_host_sdio_send(netif_get_idx(netif), sg_list, sg_len, p->tot_len, NULL, is_special_pkt);
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
		} else {
			RTK_LOG_ETHERNET("%s[%d] fail\n", __func__, __LINE__);
			return ERR_BUF;    // return a non-fatal error
		}
	}
	RTK_LOG_ETHERNET("%s[%d] ok\n", __func__, __LINE__);
	return ERR_OK;
}

err_t ethernetif_init_rtk(struct netif *netif)
{
	LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "ameba";
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

void LwIP_Init(void)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	int8_t idx = 0;

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

		netifapi_netif_add(&xnetif[idx], ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw), NULL, &ethernetif_init_rtk, &tcpip_input);

		printf("interface %d is initialized\n", idx);

	}

	/*  Registers the default network interface. */
	netifapi_netif_set_default(&xnetif[0]);


	lwip_init_done = 1;
}

void LwIP_ethernetif_recv_inic(uint8_t idx, struct pbuf *p_buf)
{
	err_enum_t error = ERR_OK;
	error = xnetif[idx].input(p_buf, &xnetif[idx]);
	if (error != ERR_OK) {
		printf("lwip input err (%d)\n", error);
		pbuf_free(p_buf);
	}
}

void LwIP_wlan_set_netif_info(int idx_wlan, void *dev, unsigned char *dev_addr)
{
	memcpy(xnetif[idx_wlan].hwaddr, dev_addr, 6);
	xnetif[idx_wlan].state = dev;
}

void LwIP_netif_set_up(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	netifapi_netif_set_up(pnetif);
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


#define MAX_DHCP_TRIES 5


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

	//extern struct static_ip_config user_static_ip;

	pnetif = &xnetif[idx];

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
			netifapi_dhcp_start(pnetif);
			dhcp = ((struct dhcp *)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP));
			IPaddress = 0;
			DHCP_state = DHCP_WAIT_ADDRESS;
		}
		break;

		case DHCP_WAIT_ADDRESS: {
			/* If DHCP stopped by wifi_disconn_hdl*/
#include "lwip/prot/dhcp.h"
			if ((dhcp_state_enum_t)dhcp->state == DHCP_STATE_OFF) {
				IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
				IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
				IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
				netifapi_netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
				printf("\n\rLwIP_DHCP: dhcp stop.");
				return DHCP_STOP;
			}

			/* Read the new IP address */
			IPaddress = ip_addr_get_ip4_u32(netif_ip_addr4(pnetif));

			if (IPaddress != 0) {
#if LWIP_RANDOMIZE_INITIAL_LOCAL_PORTS
				/* not in ver 2.0.3 */
				//tcp_randomize_local_port();
				//udp_randomize_local_port();
#endif
				DHCP_state = DHCP_ADDRESS_ASSIGNED;

				/* Stop DHCP */
				// dhcp_stop(pnetif);  /* can not stop, need to renew, Robbie*/

				iptab[0] = (uint8_t)(IPaddress >> 24);
				iptab[1] = (uint8_t)(IPaddress >> 16);
				iptab[2] = (uint8_t)(IPaddress >> 8);
				iptab[3] = (uint8_t)(IPaddress);
				printf("\n\rInterface %d IP address : %d.%d.%d.%d\n", idx, iptab[3], iptab[2], iptab[1], iptab[0]);

				return DHCP_ADDRESS_ASSIGNED;
			} else {
				/* DHCP timeout */
				if (dhcp->tries > MAX_DHCP_TRIES) {
					DHCP_state = DHCP_TIMEOUT;
					/* Stop DHCP */
					netifapi_dhcp_stop(pnetif);

					printf("\n\rInterface %d DHCP timeout", idx);

					return DHCP_TIMEOUT;
				}
			}
		}
		break;
		case DHCP_RELEASE_IP:
			printf("\n\rLwIP_DHCP: Release ip");
			netifapi_dhcp_release(pnetif);
			return DHCP_RELEASE_IP;
		case DHCP_STOP:
			printf("\n\rLwIP_DHCP: dhcp stop.");
			LwIP_DHCP_stop(idx);
			return DHCP_STOP;
		default:
			break;
		}
		/* wait 250 ms */
		//rtos_time_delay_ms(10);
		vTaskDelay(10);
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

