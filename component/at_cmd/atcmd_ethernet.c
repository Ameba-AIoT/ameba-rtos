#include "platform_autoconf.h"
#ifdef CONFIG_ETHERNET

#include "kv.h"
#include "atcmd_service.h"
#include "wifi_api_types.h"
#include "ethernet_mii.h"
#ifdef CONFIG_LWIP_LAYER
#include "dhcp/dhcps.h"
#endif

extern struct netif *pnetif_eth;

static void at_ethip_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+ETHIP=<store_flag>,<ip>[,<gateway>,<netmask>]\r\n");
	RTK_LOGI(NOTAG, "\t<store_flag>:\t0: Apply Temporary Configuration, 1: Save Configuration to Flash, 2: Erase Configuration from Flash\r\n");
	RTK_LOGI(NOTAG, "\tThe <gateway> and <netmask> should be absent or present together\r\n");
}

void at_ethip(void *arg)
{
	char *argv[MAX_ARGC] = {0};
	int argc = 0;
	int error_no = 0;
	int store_flag = 0;
	unsigned int ip = 0;
	unsigned int netmask = 0xFFFFFF00;
	unsigned int gw = 0;

	if (arg == NULL) {
		RTK_LOGW(NOTAG, "[+ETHIP] The parameters can not be ignored\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc < 2 || argc > 5) {
		RTK_LOGW(NOTAG, "[+ETHIP] The parameters format ERROR\r\n");
		error_no = 1;
		goto end;
	}

	store_flag = atoi(argv[1]);
	if (store_flag < 0 || store_flag > 2) {
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

	ip = PP_HTONL(inet_addr(argv[2]));
	if (store_flag == 1) {
		rt_kv_set("eth_ip", &ip, 4);
	}
	gw = (ip & 0xFFFFFF00) | 0x01;

	if (argc >= 4) {
		gw = PP_HTONL(inet_addr(argv[3]));
		if (store_flag == 1) {
			rt_kv_set("eth_gw", &gw, 4);
		}
	}
	if (argc == 5) {
		netmask = PP_HTONL(inet_addr(argv[4]));
		if (store_flag == 1) {
			rt_kv_set("eth_netmask", &netmask, 4);
		}
	}

	if (netif_dhcp_data(pnetif_eth) != NULL) {
		LwIP_DHCP_stop(NETIF_ETH_INDEX);
	} else {
		LwIP_ReleaseIP(NETIF_ETH_INDEX);
	}

	LwIP_netif_set_down(NETIF_ETH_INDEX);
	LwIP_netif_set_up(NETIF_ETH_INDEX);
	LwIP_SetIP(NETIF_ETH_INDEX, ip, netmask, gw);

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_ethip_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_ethernet_items[ ] = {
	{"+ETHIP", at_ethip},
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