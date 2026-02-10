
#include <lwip/netif.h>
#include <lwip/ip_addr.h>

int lwip_init_done = 0;
#define NET_IF_NUM 2
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

/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   0
#define GW_ADDR1   0
#define GW_ADDR2   0
#define GW_ADDR3   0

struct eth_drv_sg {
	unsigned int    buf;
	unsigned int     len;
};

#define MAX_ETH_DRV_SG    32
#define MAX_ETH_MSG    1540

#define ETH_ALEN	6		/* Octets in one ethernet addr	 */
#define ETH_HLEN	14		/* Total octets in header.	 */
#define ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */
#define ETH_TYPE_LEN		2
#define ETH_ARPHRD		1	/* ethernet hardware format */

#define ETH_P_IP	0x0800		/* Internet Protocol packet	*/


void LwIP_SetIP(uint8_t idx, u32_t addr, u32_t netmask_addr, u32_t gw_addr)
{
	struct netif *pnetif = &xnetif[idx];
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;

	ip_2_ip4(&ipaddr)->addr = PP_HTONL(addr);
	ip_2_ip4(&netmask)->addr = PP_HTONL(netmask_addr);
	ip_2_ip4(&gw)->addr = PP_HTONL(gw_addr);
	netifapi_netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
}

void LwIP_netif_set_link_up(uint8_t idx)
{
	struct netif *pnetif = &xnetif[idx];
	netifapi_netif_set_link_up(pnetif);
	if (idx == 0) {
		netifapi_netif_set_default(&xnetif[0]);
	} else if ((idx == 1) && (!(xnetif[0].flags & NETIF_FLAG_LINK_UP))) {
		/*default netif is on sta when sta and softap both up*/
		netifapi_netif_set_default(&xnetif[1]);
	}
}

void LwIP_wlan_set_netif_info(int idx_wlan, void *dev, unsigned char *dev_addr)
{
	printf("mac: %d %02x:%02x:%02x:%02x:%02x:%02x \r\n", idx_wlan, dev_addr[0], dev_addr[1], dev_addr[2], dev_addr[3], dev_addr[4], dev_addr[5]);
	memcpy(xnetif[idx_wlan].hwaddr, dev_addr, 6);
	xnetif[idx_wlan].state = dev;
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

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{

	/* Refer to eCos lwip eth_drv_send() */
	struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
	int sg_len = 0;
	struct pbuf *q;
	int ret = 0;
	struct eth_hdr *ethhdr = NULL;
	u8_t is_special_pkt = 0;
	u8_t *addr = (u8_t *)p->payload;

	for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
		sg_list[sg_len].buf = (unsigned int) q->payload;
		sg_list[sg_len++].len = q->len;
	}

	if (sg_len) {
		ret = whc_host_sdio_send(netif_get_idx(netif), sg_list, sg_len, p->tot_len, NULL, is_special_pkt);
		if (ret == ERR_IF) {
			return ret;
		}
		if (ret == 0) {
			return ERR_OK;
		} else {
			printf("%s[%d] fail\n", __func__, __LINE__);
			return ERR_BUF;    // return a non-fatal error
		}
	}

	return ERR_OK;
}

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

err_t ethernetif_init_rtk(struct netif *netif)
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

	return ERR_OK;
}

void LwIP_Init(void)
{
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;
	int8_t idx = 0;

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
	for (idx = 0; idx < NET_IF_NUM; idx++) {
		if (idx == 0) {
			IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
			IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
			IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
			xnetif[idx].name[0] = 'w';
			xnetif[idx].name[1] = '0';

		} else {
			IP4_ADDR(ip_2_ip4(&ipaddr), AP_IP_ADDR0, AP_IP_ADDR1, AP_IP_ADDR2, AP_IP_ADDR3);
			IP4_ADDR(ip_2_ip4(&netmask), AP_NETMASK_ADDR0, AP_NETMASK_ADDR1, AP_NETMASK_ADDR2, AP_NETMASK_ADDR3);
			IP4_ADDR(ip_2_ip4(&gw), AP_GW_ADDR0, AP_GW_ADDR1, AP_GW_ADDR2, AP_GW_ADDR3);
			xnetif[idx].name[0] = 'a';
			xnetif[idx].name[1] = 'p';
		}


		netifapi_netif_add(&xnetif[idx], ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw), NULL, &ethernetif_init_rtk, &tcpip_input);

		printf("interface %d is initialized\n", idx);

	}

	/*  Registers the default network interface. */
	netifapi_netif_set_default(&xnetif[0]);

	/*move these operations to wifi_on/wifi_off*/
#if 0
	/*  When the netif is fully configured this function must be called.*/
	for (idx = 0; idx < NET_IF_NUM; idx++) {
		netif_set_up(&xnetif[idx]);
	}
#endif
}

err_t LwIP_ethernetif_recv_inic(uint8_t wlan_idx, struct pbuf *p)
{
	struct netif *netif;
#if 1
	if (wlan_idx == 0) {
		netif = netif_find(wifi_get_ifname_sta());
	} else {
		netif = netif_find(wifi_get_ifname_ap());
	}
#else
	netif = &xnetif[wlan_idx];
#endif
	if (netif == NULL || p == NULL) {
		printf("error %s %d \r\n", __func__, __LINE__);
		pbuf_free(p);
		return ERR_IF;
	}

	if (netif->input(p, netif) != ERR_OK) {
		pbuf_free(p);
		return ERR_BUF;
	}
	return ERR_OK;
}
