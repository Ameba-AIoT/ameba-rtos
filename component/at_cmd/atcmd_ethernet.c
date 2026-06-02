#include "platform_autoconf.h"
#ifdef CONFIG_ETHERNET

#include "kv.h"
#include "atcmd_service.h"
#include "wifi_api_types.h"
#include "ethernet.h"
#ifdef CONFIG_LWIP_LAYER
#include "lwip_netconf.h"
#include "dhcp/dhcps.h"
#endif

extern struct netif *pnetif_eth;

enum eth_start_mode {
	ETH_MODE_DHCP_CLIENT = 0,
	ETH_MODE_DHCP_SERVER = 1,
	ETH_MODE_STATIC_IP   = 2,
};

static const char *const TAG_ETH = "AT_ETH";
static u8 g_eth_started = 0;
static u8 g_eth_mode = ETH_MODE_DHCP_CLIENT;

static void at_ethip_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+ETHIP=<store_flag>,<ip>[,<gateway>,<netmask>]\r\n");
	RTK_LOGI(NOTAG, "\t<store_flag>:\t1: Save Configuration to Flash, 2: Erase Configuration from Flash\r\n");
	RTK_LOGI(NOTAG, "\tThe <gateway> and <netmask> should be absent or present together\r\n");
	RTK_LOGI(NOTAG, "\tThis command only stores the static IP file; it does not configure netif/lwip.\r\n");
	RTK_LOGI(NOTAG, "\tUse AT+ETHSTART=2 to bring up Ethernet with the stored static IP.\r\n");
}

void at_ethip(u16 argc, char **argv)
{
	int error_no = 0;
	int store_flag = 0;
	unsigned int ip = 0;
	unsigned int netmask = 0xFFFFFF00;
	unsigned int gw = 0;

	if (argc == 1) {
		RTK_LOGW(NOTAG, "[+ETHIP] The parameters can not be ignored\r\n");
		error_no = 1;
		goto end;
	}

	if (argc < 2 || argc > 5) {
		RTK_LOGW(NOTAG, "[+ETHIP] The parameters format ERROR\r\n");
		error_no = 1;
		goto end;
	}

	store_flag = atoi(argv[1]);
	if (store_flag != 1 && store_flag != 2) {
		RTK_LOGW(NOTAG, "[+ETHIP] The parameters format ERROR\r\n");
		error_no = 1;
		goto end;
	}

	if (store_flag == 2) {
		rt_kv_delete("eth_ip");
		rt_kv_delete("eth_gw");
		rt_kv_delete("eth_netmask");
		goto end;
	}

	if (argc < 3) {
		RTK_LOGW(NOTAG, "[+ETHIP] <ip> is required when saving\r\n");
		error_no = 1;
		goto end;
	}

	ip = PP_HTONL(inet_addr(argv[2]));
	rt_kv_set("eth_ip", &ip, 4);

	gw = (ip & 0xFFFFFF00) | 0x01;
	if (argc >= 4) {
		gw = PP_HTONL(inet_addr(argv[3]));
	}
	rt_kv_set("eth_gw", &gw, 4);

	if (argc == 5) {
		netmask = PP_HTONL(inet_addr(argv[4]));
	}
	rt_kv_set("eth_netmask", &netmask, 4);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_ethip_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

#ifdef CONFIG_LWIP_LAYER
static int eth_load_static_ip(unsigned int *ip, unsigned int *netmask, unsigned int *gw)
{
	if (rt_kv_get("eth_ip", (uint8_t *)ip, 4) != 4) {
		return -1;
	}
	if (rt_kv_get("eth_netmask", (uint8_t *)netmask, 4) != 4) {
		*netmask = 0xFFFFFF00;
	}
	if (rt_kv_get("eth_gw", (uint8_t *)gw, 4) != 4) {
		*gw = (*ip & 0xFFFFFF00) | 0x01;
	}
	return 0;
}

static void eth_apply_static_ip(void)
{
	unsigned int ip = 0, netmask = 0xFFFFFF00, gw = 0;

	if (eth_load_static_ip(&ip, &netmask, &gw) != 0) {
		RTK_LOGE(TAG_ETH, "Static IP not configured. Run AT+ETHIP first.\r\n");
		return;
	}

	if (netif_dhcp_data(pnetif_eth) != NULL) {
		lwip_dhcp_stop(NETIF_ETH_INDEX);
	} else {
		lwip_clear_ip(NETIF_ETH_INDEX);
	}
	lwip_netif_set_down(NETIF_ETH_INDEX);
	lwip_netif_set_up(NETIF_ETH_INDEX);
	lwip_set_ip(NETIF_ETH_INDEX, ip, netmask, gw);
}

static void at_eth_link_callback(int link_up)
{
	if (link_up) {
		RTK_LOGI(TAG_ETH, "=== Ethernet Link UP (mode=%d) ===\r\n", g_eth_mode);

		if (!netif_is_up(pnetif_eth)) {
			netifapi_netif_set_up(pnetif_eth);
		}
		netifapi_netif_set_link_up(pnetif_eth);

		if (g_eth_mode == ETH_MODE_DHCP_CLIENT) {
			u32 dhcp_status = lwip_request_ip(NETIF_ETH_INDEX);
			if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
				netifapi_netif_set_default(pnetif_eth);
				uint8_t *ip = lwip_get_ip(NETIF_ETH_INDEX);
				RTK_LOGI(TAG_ETH, "Ethernet DHCP got IP: %d.%d.%d.%d\r\n",
						 ip[0], ip[1], ip[2], ip[3]);
			} else {
				RTK_LOGW(TAG_ETH, "Ethernet DHCP failed to get IP\r\n");
			}
		} else if (g_eth_mode == ETH_MODE_DHCP_SERVER) {
			eth_apply_static_ip();
			netifapi_netif_set_default(pnetif_eth);
			if (dhcps_init(pnetif_eth) == NULL) {
				RTK_LOGE(TAG_ETH, "dhcps_init failed (no memory?)\r\n");
			} else {
				dhcps_stop(pnetif_eth);
				if (dhcps_start(pnetif_eth) != ERR_OK) {
					RTK_LOGE(TAG_ETH, "DHCP server start failed\r\n");
				} else {
					uint8_t *ip = lwip_get_ip(NETIF_ETH_INDEX);
					RTK_LOGI(TAG_ETH, "Ethernet DHCP server started, IP: %d.%d.%d.%d\r\n",
							 ip[0], ip[1], ip[2], ip[3]);
				}
			}
		} else { /* ETH_MODE_STATIC_IP */
			eth_apply_static_ip();
			netifapi_netif_set_default(pnetif_eth);
			uint8_t *ip = lwip_get_ip(NETIF_ETH_INDEX);
			RTK_LOGI(TAG_ETH, "Ethernet static IP: %d.%d.%d.%d\r\n",
					 ip[0], ip[1], ip[2], ip[3]);
		}
	} else {
		RTK_LOGI(TAG_ETH, "=== Ethernet Link DOWN ===\r\n");

		if (g_eth_mode == ETH_MODE_DHCP_SERVER) {
			dhcps_stop(pnetif_eth);
		}
		netifapi_netif_set_link_down(pnetif_eth);
		lwip_clear_ip(NETIF_ETH_INDEX);
		RTK_LOGI(TAG_ETH, "Ethernet IP released\r\n");
	}
}
#endif /* CONFIG_LWIP_LAYER */

static void at_ethstart_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+ETHSTART=<mode>\r\n");
	RTK_LOGI(NOTAG, "\t<mode>:\t0: DHCP Client, 1: DHCP Server, 2: Static IP (from AT+ETHIP)\r\n");
}

void at_ethstart(u16 argc, char **argv)
{
	int error_no = 0;
	int mode = 0;

	if (argc != 2) {
		RTK_LOGW(NOTAG, "[+ETHSTART] The parameters format ERROR\r\n");
		error_no = 1;
		goto end;
	}

	mode = atoi(argv[1]);
	if (mode < ETH_MODE_DHCP_CLIENT || mode > ETH_MODE_STATIC_IP) {
		RTK_LOGW(NOTAG, "[+ETHSTART] Invalid <mode>\r\n");
		error_no = 1;
		goto end;
	}

#ifdef CONFIG_LWIP_LAYER
	if (mode == ETH_MODE_DHCP_SERVER || mode == ETH_MODE_STATIC_IP) {
		unsigned int ip = 0, netmask = 0, gw = 0;
		if (eth_load_static_ip(&ip, &netmask, &gw) != 0) {
			RTK_LOGW(NOTAG, "[+ETHSTART] Static IP not configured. Run AT+ETHIP=1,<ip>[,<gw>,<mask>] first.\r\n");
			error_no = 2;
			goto end;
		}
	}

	g_eth_mode = (u8)mode;

	if (!g_eth_started) {
		eth_register_link_cb(at_eth_link_callback);
		eth_init();
		g_eth_started = 1;
	}
	/* If already started, the new mode takes effect on the next link-up event. */
#else
	(void)mode;
	RTK_LOGW(NOTAG, "[+ETHSTART] CONFIG_LWIP_LAYER is not enabled\r\n");
	error_no = 3;
#endif

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_ethstart_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_ethernet_items[ ] = {
	{"+ETHIP", at_ethip},
	{"+ETHSTART", at_ethstart},
};

void print_ethernet_at(void)
{
	int index;
	int cmd_len = 0;

	cmd_len = sizeof(at_ethernet_items) / sizeof(at_ethernet_items[0]);
	for (index = 0; index < cmd_len; index++) {
		at_printf("AT%s\r\n", at_ethernet_items[index].log_cmd);
	}
}

#endif /* CONFIG_ETHERNET */
