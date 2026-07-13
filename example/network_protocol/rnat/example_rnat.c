/**
 * @file example_rnat.c
 * @brief R-NAT Example
 */

#include "example_rnat.h"
#include "rnat_netif.h"
#include "lwip_ipnat.h"
#include "dns_proxy.h"

#ifndef TAG
#define TAG "R-NAT-APP"
#endif

/* ======================================================================== */
/*                      User Netif Status Callback                          */
/* ======================================================================== */

/**
 * @brief User netif status callback
 * @param netif The netif object whose status changed
 * @param is_active true if interface link up, false if link down
 * @param user_data User-provided data (not used)
 */
static void user_netif_status_cb(rnat_netif_t *netif, bool is_active, void *user_data)
{
	(void)user_data;

	if (!netif) {
		return;
	}

	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "[USER-CB] %s Link %s\n",
			 netif->if_desc, is_active ? "UP" : "DOWN");
}

/* ======================================================================== */
/*                     Global Configuration Variables                       */
/* ======================================================================== */

/**
 * @brief WiFi configuration for STA
 */
static const rnat_wifi_config_t g_sta_wifi_config = {
	.ssid = "AmebaWiFi",
	.password = "12345678",
	.channel = 0
};

/**
 * @brief WiFi configuration for AP
 */
static const rnat_wifi_config_t g_ap_wifi_config = {
	.ssid = "AmebaRouter",
	.password = "12345678",
	.channel = 0
};

/**
 * @brief Custom IP configuration for AP (192.168.39.1/24)
 */
static const rnat_ip_info_t g_ap_custom_ip = {
	.ip = { .addr = PP_HTONL(LWIP_MAKEU32(192, 168, 39, 1)) },
	.gw = { .addr = PP_HTONL(LWIP_MAKEU32(192, 168, 39, 1)) },
	.netmask = { .addr = PP_HTONL(LWIP_MAKEU32(255, 255, 255, 0)) }
};

/**
 * @brief RNAT STA netif configuration (WAN - DHCP Client)
 */
const rnat_netif_config_t g_rnat_sta_netif_config = {
	.role = RNAT_ROLE_WAN,
	.ip_method = RNAT_IP_METHOD_DHCP_CLIENT,
	.priority = RNAT_ROUTE_PRIO_WAN_STA,
	.ip_info = NULL,
	.if_desc = "STA",
	.status_callback = user_netif_status_cb,
	.callback_user_data = NULL
};

/**
 * @brief RNAT AP netif configuration (LAN - DHCP Server)
 */
const rnat_netif_config_t g_rnat_ap_netif_config = {
	.role = RNAT_ROLE_LAN,
	.ip_method = RNAT_IP_METHOD_DHCP_SERVER,
	.priority = RNAT_ROUTE_PRIO_LAN,
	.ip_info = &g_ap_custom_ip,  /* Use custom static IP */
	.if_desc = "AP",
	.status_callback = user_netif_status_cb,
	.callback_user_data = NULL
};

#if defined(CONFIG_LWIP_ETHERNET)
/**
 * @brief RNAT ETH netif configuration
 */
const rnat_netif_config_t g_rnat_eth_netif_config = {
	.role = RNAT_ROLE_LAN,
	.ip_method = RNAT_IP_METHOD_DHCP_SERVER,  /* LAN - DHCP Server */
	.priority = RNAT_ROUTE_PRIO_LAN,
	.ip_info = NULL,
	.if_desc = "ETH",
	.status_callback = user_netif_status_cb,
	.callback_user_data = NULL
};
#endif

#if defined(CONFIG_LWIP_USB_ETHERNET)
/**
 * @brief RNAT USB netif configuration
 */
const rnat_netif_config_t g_rnat_usb_eth_netif_config = {
	.role = RNAT_ROLE_LAN,
	.ip_method = RNAT_IP_METHOD_DHCP_SERVER,  /* LAN - DHCP Server */
	.priority = RNAT_ROUTE_PRIO_LAN,
	.ip_info = NULL,
	.if_desc = "USB",
	.status_callback = user_netif_status_cb,
	.callback_user_data = NULL
};
#endif

/**
 * @brief R-NAT main task function
 */
static void rnat_example_thread(void *param)
{
	UNUSED(param);

	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "\n");
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "========================================\n");
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "            R-NAT Example              \n");
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "========================================\n\n");

	/* Initialize R-NAT system */
	rnat_init();

	/* Register and start STA interface (WAN) */
	rnat_netif_t *sta_netif = rnat_netif_create(NETIF_WLAN_STA_INDEX, &g_rnat_sta_netif_config);
	rnat_netif_start(sta_netif, (void *)&g_sta_wifi_config);

	/* Register and start AP interface (LAN) */
	rnat_netif_t *ap_netif = rnat_netif_create(NETIF_WLAN_AP_INDEX, &g_rnat_ap_netif_config);
	rnat_netif_start(ap_netif, (void *)&g_ap_wifi_config);

#if defined(CONFIG_LWIP_ETHERNET)
	/* Register and start Ethernet interface */
	rnat_netif_t *eth_netif = rnat_netif_create(NETIF_ETH_INDEX, &g_rnat_eth_netif_config);
	rnat_netif_start(eth_netif, NULL);
#endif

#if defined(CONFIG_LWIP_USB_ETHERNET)
	/* Register and start USB ETH interface */
	rnat_netif_t *usb_netif = rnat_netif_create(NETIF_USB_ETH_INDEX, &g_rnat_usb_eth_netif_config);
	rnat_netif_start(usb_netif, NULL);
#endif

	rtos_time_delay_ms(500);

	/* Print initial status */
	rnat_print_status();

	rtos_task_delete(NULL);
}

/**
 * @brief R-NAT Example Main Function
 */
void example_rnat(void)
{
	if (rtos_task_create(NULL, ((const char *)"rnat_example"), rnat_example_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create R-NAT example task failed\n");
		return;
	}

	/* Initialize and start DNS Proxy */
	dns_proxy_init(DNS_PROXY_CACHE_MAX_ENTRIES, DNS_PROXY_TX_MAX_PENDING);
	dns_proxy_start(DNS_PROXY_LISTEN_PORT);
}

/* ======================================================================== */
/*                         Shell Debug Commands                             */
/* ======================================================================== */

static u32 CmdRnat(u16 argc, u8 *argv[])
{
	const char *sub = (argc > 0) ? (const char *)argv[0] : "";

	if (argc == 0 || _strcmp(sub, "all") == 0) {
		rnat_print_status();
		ip_nat_dump();
		dns_proxy_print_status();
		dns_proxy_dump_cache();
		dns_proxy_dump_transactions();
	} else if (_strcmp(sub, "status") == 0) {
		rnat_print_status();
	} else if (_strcmp(sub, "nat") == 0) {
		ip_nat_dump();
	} else if (_strcmp(sub, "dns") == 0) {
		dns_proxy_print_status();
		dns_proxy_dump_cache();
		dns_proxy_dump_transactions();
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Usage: rnat [status|nat|dns|all]\n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "  status : R-NAT interface status\n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "  nat    : NAT table dump\n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "  dns    : DNS proxy status and cache\n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "  all    : all of the above (default)\n");
	}
	return TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE rnat_cmd_table[] = {
	{"rnat", CmdRnat},
};
