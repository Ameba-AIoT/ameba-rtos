/* Includes ------------------------------------------------------------------*/
#include "lwip_netconf.h"
#include "lwip/prot/dhcp.h"
#if LWIP_IPV6
#include "lwip/nd6.h"
#endif
#include "dhcp/dhcps.h"
#include "atcmd_service.h"
#include "wifi_intf_drv_to_upper.h"
#include "ameba_pmu.h"

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
#include "wifi_fast_connect.h"
#endif

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif

/* Private define ------------------------------------------------------------*/
#define MAX_DHCP_TRIES 5

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#ifdef CONFIG_WHC_HOST
#include "whc_ipc.h"
#endif

#if (defined(CONFIG_WHC_DEV) && defined(CONFIG_WHC_DEV_TCPIP_KEEPALIVE)) || defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE
struct static_ip_config user_static_ip;

unsigned char ap_ip[4] = {192, 168, 43, 1}, ap_netmask[4] = {255, 255, 255, 0}, ap_gw[4] = {192, 168, 43, 1};
#endif


extern void (*p_wifi_join_info_free)(u8 iface_type);
struct netif xnetif[NET_IF_NUM]; /* network interface structure */
struct netif *pnetif_sta = &xnetif[NETIF_WLAN_STA_INDEX];
struct netif *pnetif_ap = &xnetif[NETIF_WLAN_AP_INDEX];
#if defined(CONFIG_WIFI_NAN_ENABLE)
struct netif *pnetif_nan = &xnetif[NETIF_WLAN_NAN_INDEX];
#endif
#if defined(CONFIG_LWIP_ETHERNET)
struct netif *pnetif_eth = &xnetif[NETIF_ETH_INDEX];
#endif
#if defined(CONFIG_LWIP_USB_ETHERNET)
struct netif *pnetif_usb_eth = &xnetif[NETIF_USB_ETH_INDEX];
#endif
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the lwIP stack
  * @param  None
  * @retval None
  */

int lwip_init_done = 0;

void lwip_module_init(void)
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
	Give pointers to cleared ip_addr structures. The state pointer may be NULL.

	The init function pointer must point to a initialization function for
	your ethernet netif interface. The following code illustrates it's use.*/
	//RTK_LOGS(NOTAG, RTK_LOG_INFO, "NET_IF_NUM:%d\n\r",NET_IF_NUM);
	for (idx = 0; idx < NET_IF_NUM; idx++) {
		IP4_ADDR(ip_2_ip4(&ipaddr), IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
		IP4_ADDR(ip_2_ip4(&netmask), NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
		IP4_ADDR(ip_2_ip4(&gw), GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

		xnetif[idx].name[0] = 'r';
		xnetif[idx].name[1] = '0' + idx;

		netifapi_netif_add(&xnetif[idx], ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw), NULL, &netif_adapter_init, &tcpip_input);

		RTK_LOGS(NOTAG, RTK_LOG_INFO, "interface %d is initialized\n", idx);

	}

	/*  Registers the default network interface. */
	netifapi_netif_set_default(pnetif_sta);

	/*move these operations to wifi_on/wifi_off*/
#if 0
	/*  When the netif is fully configured this function must be called.*/
	for (idx = 0; idx < NET_IF_NUM; idx++) {
		netif_set_up(&xnetif[idx]);
	}
#endif

	lwip_init_done = 1;
#ifdef CONFIG_STANDARD_TICKLESS
	pmu_register_sleep_callback(PMU_LWIP_STACK, (PSM_HOOK_FUN)lwip_rm_unneeded_tmr, NULL, (PSM_HOOK_FUN)lwip_update_internal_counter, NULL);
#endif
	RTK_LOGS(TAG_WLAN_DRV, RTK_LOG_INFO, "LWIP consume heap %d\n", heap - rtos_mem_get_free_heap_size() - 4 * TCPIP_THREAD_STACKSIZE);
}

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
extern uint32_t offer_ip;
extern uint32_t server_ip;

#endif

void lwip_clear_ip(uint8_t idx)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return;
	}

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
uint8_t lwip_dhcp(uint8_t idx, uint8_t dhcp_state)
{
	uint32_t IPaddress;
	uint8_t iptab[4];
	uint8_t DHCP_state;
	struct dhcp *dhcp = NULL;
	uint8_t ret = 0;
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return 0;
	}

	DHCP_state = dhcp_state;

#if defined(CONFIG_LWIP_USB_ETHERNET_BRIDGE) && CONFIG_LWIP_USB_ETHERNET_BRIDGE
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "skip DHCP !!\n");
	return 0;
#endif

	extern struct static_ip_config user_static_ip;
	if (user_static_ip.use_static_ip) {
		lwip_set_ip(NETIF_WLAN_STA_INDEX, user_static_ip.addr, user_static_ip.netmask, user_static_ip.gw);
		iptab[3] = (uint8_t)(user_static_ip.addr >> 24);
		iptab[2] = (uint8_t)(user_static_ip.addr >> 16);
		iptab[1] = (uint8_t)(user_static_ip.addr >> 8);
		iptab[0] = (uint8_t)(user_static_ip.addr);
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rSet Interface %d static IP : %d.%d.%d.%d\n", idx, iptab[3], iptab[2], iptab[1], iptab[0]);
		return 0;
	}

	if (DHCP_state == 0) {
		ip_addr_set_zero(&pnetif->ip_addr);
		ip_addr_set_zero(&pnetif->netmask);
		ip_addr_set_zero(&pnetif->gw);
	}

	dhcp = ((struct dhcp *)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP));
	if (!netif_is_up(pnetif)) { // netif should be set up before doing dhcp request (in lwip v2.0.0)
		netifapi_netif_set_up(pnetif);
	}

#ifndef CONFIG_STANDARD_TICKLESS
	/*acqurie wakelock to guarantee dhcp*/
	pmu_acquire_wakelock(PMU_DHCP_PROCESS);
#endif
	for (;;) {
		//RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\r ========DHCP_state:%d============\n\r",DHCP_state);
		switch (DHCP_state) {
		case DHCP_START: {

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
			if (check_is_the_same_ap()) {
				if ((offer_ip != 0 && offer_ip != 0xFFFFFFFF) || (dhcp != NULL)) {
					if (dhcp == NULL) {
						dhcp = (struct dhcp *)mem_malloc(sizeof(struct dhcp));
						if (dhcp == NULL) {
							RTK_LOGS(NOTAG, RTK_LOG_ERROR, "dhcp_start(): could not allocate dhcp\n");
							ret = DHCP_STOP;
							goto exit;
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
				DHCP_state = DHCP_STOP;
				lwip_clear_ip(idx);
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rlwip_dhcp: dhcp stop.");
				wifi_indication(RTW_EVENT_DHCP_STATUS, &DHCP_state, sizeof(uint8_t));
				ret = DHCP_STOP;
				goto exit;
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
				wifi_dhcp_success_indicate();

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
				/* Detect and handle subnet conflict after DHCP success */
				lwip_manage_subnet_conflict(lwip_netif_get_idx(pnetif));

				wifi_indication(RTW_EVENT_DHCP_STATUS, &DHCP_state, sizeof(uint8_t));
				ret = DHCP_ADDRESS_ASSIGNED;
				goto exit;
			} else {
				/* DHCP timeout */
				if (dhcp->tries > MAX_DHCP_TRIES) {
					DHCP_state = DHCP_TIMEOUT;
					/* Stop DHCP */
					netifapi_dhcp_stop(pnetif);

					at_printf_indicate("wifi got ip timeout\r\n");

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
					if (p_store_fast_connect_info) {
						p_store_fast_connect_info((uint32_t)dhcp->offered_ip_addr.addr, (uint32_t)ip_addr_get_ip4_u32(&dhcp->server_ip_addr));
					}
					if (p_wifi_join_info_free) {
						/* free key here after join success */
						p_wifi_join_info_free(IFACE_PORT0);
					}
#endif
					wifi_indication(RTW_EVENT_DHCP_STATUS, &DHCP_state, sizeof(uint8_t));
					ret = DHCP_TIMEOUT;
					goto exit;
				}
			}
		}
		break;
		case DHCP_RELEASE_IP:
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rlwip_dhcp: Release ip");
			netifapi_dhcp_release(pnetif);
			ret = DHCP_RELEASE_IP;
			goto exit;
		case DHCP_STOP:
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rlwip_dhcp: dhcp stop.");
			lwip_dhcp_stop(idx);
			ret = DHCP_STOP;
			goto exit;
		default:
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\rlwip_dhcp: invalid dhcp state\n");
			ret = DHCP_STOP;
			goto exit;
		}
		/* wait 10 ms */
		rtos_time_delay_ms(10);
	}

exit:
#ifndef CONFIG_STANDARD_TICKLESS
	pmu_release_wakelock(PMU_DHCP_PROCESS);
#endif
	return ret;
}

void lwip_dhcp_stop(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return;
	}

	netifapi_dhcp_stop(pnetif);

	lwip_clear_ip(idx);
}

int lwip_netif_get_idx(struct netif *pnetif)
{
#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER == 1)
	int idx = pnetif - xnetif;
	return idx < NET_IF_NUM ? idx : -1;
#else
	UNUSED(pnetif);
	return -1;
#endif
}

struct netif *lwip_idx_get_netif(uint8_t idx)
{
#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER == 1)
	if (idx < NET_IF_NUM) {
		return &xnetif[idx];
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "idx is invalid !!\n");
		return NULL;
	}
#else
	UNUSED(idx);
	return NULL;
#endif
}

void lwip_netif_set_up(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return;
	}
	netifapi_netif_set_up(pnetif);
}

void lwip_netif_set_down(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return;
	}
	netifapi_netif_set_down(pnetif);
}

void lwip_netif_set_link_up(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return;
	}
	netifapi_netif_set_link_up(pnetif);
	if (idx == NETIF_WLAN_STA_INDEX) {
		netifapi_netif_set_default(pnetif_sta);
	} else if ((idx == NETIF_WLAN_AP_INDEX) && (!(xnetif[NETIF_WLAN_STA_INDEX].flags & NETIF_FLAG_LINK_UP))) {
		/*default netif is on sta when sta and softap both up*/
		netifapi_netif_set_default(pnetif_ap);
	}
}

void lwip_netif_set_link_down(uint8_t idx)
{
	struct netif *pnetif;
	if (idx < NET_IF_NUM) {
		pnetif = &xnetif[idx];
	} else {
		return;
	}
	netifapi_netif_set_link_down(pnetif);
	if (idx == NETIF_WLAN_AP_INDEX) {
		netifapi_netif_set_default(pnetif_sta);
	} else if (idx == NETIF_WLAN_STA_INDEX) {
		netifapi_netif_set_default(pnetif_ap);
	}
}

uint8_t *lwip_get_mac(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return NULL;
	}
	return (uint8_t *)(pnetif->hwaddr);
}

uint8_t *lwip_get_ip(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return NULL;
	}
	return (uint8_t *) & (pnetif->ip_addr);
}

uint8_t *lwip_get_gw(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return NULL;
	}
	return (uint8_t *) & (pnetif->gw);
}

uint8_t *lwip_get_mask(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return NULL;
	}
	return (uint8_t *) & (pnetif->netmask);
}

void lwip_wlan_set_netif_info(uint8_t idx, void *dev, unsigned char *dev_addr)
{
	struct netif *pnetif;
	if (idx < NET_IF_NUM) {
		pnetif = &xnetif[idx];
	} else {
		return;
	}
	memcpy(pnetif->hwaddr, dev_addr, 6);
	pnetif->state = dev;
}

int lwip_is_valid_ip(uint8_t idx, unsigned char *ip_dest)
{
#if defined(CONFIG_LWIP_LAYER) && (CONFIG_LWIP_LAYER == 1)
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return 0;
	}

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
void lwip_get_dns(struct ip_addr *dns)
{
	struct ip_addr *tmp = (struct ip_addr *)dns_getserver(0);
	*dns = *tmp;
}

void lwip_set_dns(struct ip_addr *dns)
{
	dns_setserver(0, dns);
}
#endif

void lwip_set_ip(uint8_t idx, u32_t addr, u32_t netmask_addr, u32_t gw_addr)
{
	struct ip_addr ipaddr;
	struct ip_addr netmask;
	struct ip_addr gw;
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return;
	}

#if CONFIG_WLAN || (defined(CONFIG_LWIP_USB_ETHERNET) && CONFIG_LWIP_USB_ETHERNET) || (defined(CONFIG_LWIP_ETHERNET) && CONFIG_LWIP_ETHERNET)
	ip_2_ip4(&ipaddr)->addr = PP_HTONL(addr);
	ip_2_ip4(&netmask)->addr = PP_HTONL(netmask_addr);
	ip_2_ip4(&gw)->addr = PP_HTONL(gw_addr);
	netifapi_netif_set_addr(pnetif, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask), ip_2_ip4(&gw));
#endif
}

#if LWIP_AUTOIP
#include <lwip/autoip.h>
#include <lwip/prot/autoip.h>

void lwip_autoip(uint8_t idx)
{
	uint8_t *ip = lwip_get_ip(idx);
	struct autoip *autoip = NULL;
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return;
	}

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

void lwip_autoip_stop(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return;
	}
	netifapi_autoip_stop(pnetif);
}
#endif
#if LWIP_IPV6
/* Get IPv6 address with lwip 1.5.0 */
void lwip_autoip_ipv6(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return;
	}
	netif_create_ip6_linklocal_address(pnetif, 1);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\nIPv6 link-local address: ");
	LwIP_DUMP_IPV6_ADDRESS(netif_ip6_addr(pnetif, 0)->addr);
}

/* Add a static IPv6 neighbor on netif idx: derive the peer link-local from its MAC
 * (EUI-64) and bind it to that MAC as a reachable neighbor-cache entry. */
void lwip_add_ipv6_neighbor(uint8_t idx, const uint8_t *peer_mac)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	ip6_addr_t ll;

	if (pnetif == NULL || peer_mac == NULL) {
		return;
	}
	ll.addr[0] = PP_HTONL(0xfe800000UL);
	ll.addr[1] = 0;
	ll.addr[2] = lwip_htonl(((u32_t)(peer_mac[0] ^ 0x02) << 24) | ((u32_t)peer_mac[1] << 16) |
							((u32_t)peer_mac[2] << 8) | 0xffUL);
	ll.addr[3] = lwip_htonl((0xfeUL << 24) | ((u32_t)peer_mac[3] << 16) |
							((u32_t)peer_mac[4] << 8) | (u32_t)peer_mac[5]);
	ip6_addr_assign_zone(&ll, IP6_UNICAST, pnetif);
	nd6_add_static_neighbor(pnetif, &ll, peer_mac);
}
#endif

//To check successful WiFi connection and obtain of an IP address
int lwip_check_connectivity(uint8_t idx)
{
	if (idx == NETIF_WLAN_STA_INDEX) {
		u8 join_status = RTW_JOINSTATUS_UNKNOWN;
		if (!((wifi_get_join_status(&join_status) == RTK_SUCCESS)
			  && (join_status == RTW_JOINSTATUS_SUCCESS)
#if defined(CONFIG_LWIP_USB_ETHERNET_BRIDGE) && CONFIG_LWIP_USB_ETHERNET_BRIDGE
			 )) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Wait for WiFi Connect Success...\n");
#else
			  &&(*(u32 *)lwip_get_ip(NETIF_WLAN_STA_INDEX) != IP_ADDR_INVALID))) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Wait for WiFi and DHCP Connect Success...\n");
#endif
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Please use AT+WLCONN to connect AP first time\n");
			return CONNECTION_INVALID;
		} else {
			return CONNECTION_VALID;
		}
	} else if (idx == NETIF_WLAN_AP_INDEX) {
		if (wifi_is_running(SOFTAP_WLAN_INDEX) == FALSE) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "SoftAP is not running...\n");
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Please use AT+WLSTARTAP to start AP first time\n");
			return CONNECTION_INVALID;
		} else {
			return CONNECTION_VALID;
		}
	} else {
		return CONNECTION_INVALID;
	}
}

/**
  * @brief  For sta get ipv4(dhcp) and ipv6 address
  * @param  None
  * @retval -1 for failed
  */

uint8_t lwip_request_ip(uint8_t idx)
{
	uint8_t ret = -1;
#if LWIP_IPV6
	lwip_autoip_ipv6(idx);
#endif
#if LWIP_IPV4
	ret = lwip_dhcp(idx, DHCP_START);
#endif
	return ret;
}

/**
  * @brief  Check if a subnet is already used by any netif except 'exclude'
  * @param  check_ip: The IP address to check
  * @param  exclude: Netif to skip during the check, NULL to check all netifs
  * @retval 1: subnet is used; 0: subnet is not used
  */
int lwip_subnet_is_used(struct ip_addr *check_ip, struct netif *exclude)
{
	if (check_ip == NULL || ip_addr_isany(check_ip)) {
		return 0;
	}

	for (int i = 0; i < NET_IF_NUM; i++) {
		struct netif *pnetif = lwip_idx_get_netif(i);
		if (pnetif == NULL || pnetif == exclude) {
			continue;
		}

		struct ip_addr *compared_ip = &pnetif->ip_addr;
		if (ip_addr_isany(compared_ip)) {
			continue;
		}

		struct ip_addr *mask = &pnetif->netmask;
		if (ip_addr_netcmp(check_ip, compared_ip, ip_2_ip4(mask))) {
			return 1;
		}
	}

	return 0;
}

/**
  * @brief  Allocate a non-conflicting IP address from IP pool for the netif
  * @param  idx: The index of network interface to allocate IP address
  * @retval 0: success; -1: failed
  */
int lwip_alloc_ip(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return -1;
	}

	struct ip_addr new_ip, new_mask, new_gw;

	/* Try from 192.168.NETWORK_SEGMENT_POOL_START.x to 192.168.NETWORK_SEGMENT_POOL_END.x */
	for (uint8_t subnet = NETWORK_SEGMENT_POOL_START; subnet <= NETWORK_SEGMENT_POOL_END; subnet++) {
		IP4_ADDR(ip_2_ip4(&new_ip), 192, 168, subnet, 1);
		IP4_ADDR(ip_2_ip4(&new_mask), 255, 255, 255, 0);
		IP4_ADDR(ip_2_ip4(&new_gw), 192, 168, subnet, 1);

		/* Check if this subnet is used by any other netif (exclude self) */
		if (!lwip_subnet_is_used(&new_ip, pnetif)) {
			netifapi_netif_set_addr(pnetif, ip_2_ip4(&new_ip), ip_2_ip4(&new_mask), ip_2_ip4(&new_gw));

			RTK_LOGS(NOTAG, RTK_LOG_INFO, "Netif %c alloc IP: %d.%d.%d.%d\n", pnetif->name[1],
					 ip4_addr1(ip_2_ip4(&new_ip)), ip4_addr2(ip_2_ip4(&new_ip)),
					 ip4_addr3(ip_2_ip4(&new_ip)), ip4_addr4(ip_2_ip4(&new_ip)));
			return 0;
		}
	}

	/* All subnets conflicted */
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Failed to alloc IP\n");
	return -1;
}

/**
  * @brief  Detect and handle subnet conflict after DHCP client gets IP
  * @param  idx: The index of DHCP client netif that got conflicting IP
  * @retval 0: success; -1: failed
  */
int lwip_manage_subnet_conflict(uint8_t idx)
{
	struct netif *pnetif = lwip_idx_get_netif(idx);
	if (pnetif == NULL) {
		return -1;
	}

	struct ip_addr *target_ip = &pnetif->ip_addr;
	if (ip_addr_isany(target_ip)) {
		return 0;
	}

	for (int i = 0; i < NET_IF_NUM; i++) {
		struct netif *existing_netif = lwip_idx_get_netif(i);
		if (existing_netif == NULL || existing_netif == pnetif || !netif_is_up(existing_netif)) {
			continue;
		}

		struct ip_addr *existing_ip = &existing_netif->ip_addr;
		struct ip_addr *mask = &existing_netif->netmask;
		if (ip_addr_isany(existing_ip)) {
			continue;
		}

		if (!ip_addr_netcmp(target_ip, existing_ip, ip_2_ip4(mask))) {
			continue;
		}

		dhcps_t *dhcps = dhcps_get_from_netif(existing_netif);
		bool is_dhcp_server = (dhcps != NULL && dhcps->dhcps_pcb != NULL);
		if (is_dhcp_server) {
			/* Case A: Client vs Server conflict - restart DHCP server */
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "CONFLICT: DHCPC(Netif %c) vs DHCPS(Netif %c) use same subnet\n",
					 pnetif->name[1], existing_netif->name[1]);

			dhcps_stop(existing_netif);

			if (lwip_alloc_ip(i) == 0) {
				/* Restart DHCP Server with new IP */
				dhcps_start(existing_netif);
			} else {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Netif %c failed to realloc IP\n", existing_netif->name[1]);
				return -1;
			}
		} else {
			/* Case B: Client vs Client conflict - warning only */
			RTK_LOGS(NOTAG, RTK_LOG_WARN, "Warn: DHCPC(Netif %c) vs DHCPC(Netif %c) use same subnet\n",
					 pnetif->name[1], existing_netif->name[1]);
		}
	}

	return 0;
}

/**
  * @brief  Get lwIP IPv6 enabled state
  * @param  None
  * @retval 1 if IPv6 is enabled, 0 otherwise
  */
uint8_t lwip_get_ipv6_enabled(void)
{
#if LWIP_IPV6
	return 1;
#else
	return 0;
#endif
}
