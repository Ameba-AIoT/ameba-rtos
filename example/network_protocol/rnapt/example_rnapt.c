/**
 * @file example_rnapt.c
 * @brief R-NAPT Example
 */

#include "example_rnapt.h"
#include "rnapt_netif.h"

#ifndef TAG
#define TAG "R-NAPT-APP"
#endif

extern void dns_relay_service_init(void);

/* ======================================================================== */
/*                      User Netif Status Callback                          */
/* ======================================================================== */

/**
 * @brief User netif status callback
 * @param netif The netif object whose status changed
 * @param is_active true if interface link up, false if link down
 * @param user_data User-provided data (not used)
 */
static void user_netif_status_cb(rnapt_netif_t *netif, bool is_active, void *user_data)
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
static const rnapt_wifi_config_t g_sta_wifi_config = {
	.ssid = "AmebaWiFi",
	.password = "12345678",
	.channel = 0
};

/**
 * @brief WiFi configuration for AP
 */
static const rnapt_wifi_config_t g_ap_wifi_config = {
	.ssid = "AmebaRouter",
	.password = "12345678",
	.channel = 0
};

/**
 * @brief Custom IP configuration for AP (192.168.39.1/24)
 */
static const rnapt_ip_info_t g_ap_custom_ip = {
	.ip = { .addr = PP_HTONL(LWIP_MAKEU32(192, 168, 39, 1)) },
	.gw = { .addr = PP_HTONL(LWIP_MAKEU32(192, 168, 39, 1)) },
	.netmask = { .addr = PP_HTONL(LWIP_MAKEU32(255, 255, 255, 0)) }
};

/**
 * @brief RNAPT STA netif configuration (WAN - DHCP Client)
 */
const rnapt_netif_config_t g_rnapt_sta_netif_config = {
	.role = RNAPT_ROLE_WAN,
	.ip_method = RNAPT_IP_METHOD_DHCP_CLIENT,
	.priority = RNAPT_ROUTE_PRIO_WAN_STA,
	.ip_info = NULL,
	.if_desc = "STA",
	.status_callback = user_netif_status_cb,
	.callback_user_data = NULL
};

/**
 * @brief RNAPT AP netif configuration (LAN - DHCP Server)
 */
const rnapt_netif_config_t g_rnapt_ap_netif_config = {
	.role = RNAPT_ROLE_LAN,
	.ip_method = RNAPT_IP_METHOD_DHCP_SERVER,
	.priority = RNAPT_ROUTE_PRIO_LAN,
	.ip_info = &g_ap_custom_ip,  /* Use custom static IP */
	.if_desc = "AP",
	.status_callback = user_netif_status_cb,
	.callback_user_data = NULL
};

#if defined(CONFIG_LWIP_ETHERNET)
/**
 * @brief RNAPT ETH netif configuration
 */
const rnapt_netif_config_t g_rnapt_eth_netif_config = {
	.role = RNAPT_ROLE_LAN,
	.ip_method = RNAPT_IP_METHOD_DHCP_SERVER,  /* LAN - DHCP Server */
	.priority = RNAPT_ROUTE_PRIO_LAN,
	.ip_info = NULL,
	.if_desc = "ETH",
	.status_callback = user_netif_status_cb,
	.callback_user_data = NULL
};
#endif

#if defined(CONFIG_LWIP_USB_ETHERNET)
/**
 * @brief RNAPT USB netif configuration
 */
const rnapt_netif_config_t g_rnapt_usb_eth_netif_config = {
	.role = RNAPT_ROLE_LAN,
	.ip_method = RNAPT_IP_METHOD_DHCP_SERVER,  /* LAN - DHCP Server */
	.priority = RNAPT_ROUTE_PRIO_LAN,
	.ip_info = NULL,
	.if_desc = "USB",
	.status_callback = user_netif_status_cb,
	.callback_user_data = NULL
};
#endif

/**
 * @brief R-NAPT main task function
 */
static void rnapt_example_thread(void *param)
{
	UNUSED(param);

	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "\n");
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "========================================\n");
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "            R-NAPT Example              \n");
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "========================================\n\n");

	/* Initialize R-NAPT system */
	rnapt_init();

	/* Register and start STA interface (WAN) */
	rnapt_netif_t *sta_netif = rnapt_netif_create(NETIF_WLAN_STA_INDEX, &g_rnapt_sta_netif_config);
	rnapt_netif_start(sta_netif, (void *)&g_sta_wifi_config);

	/* Register and start AP interface (LAN) */
	rnapt_netif_t *ap_netif = rnapt_netif_create(NETIF_WLAN_AP_INDEX, &g_rnapt_ap_netif_config);
	rnapt_netif_start(ap_netif, (void *)&g_ap_wifi_config);

#if defined(CONFIG_LWIP_ETHERNET)
	/* Register and start Ethernet interface */
	rnapt_netif_t *eth_netif = rnapt_netif_create(NETIF_ETH_INDEX, &g_rnapt_eth_netif_config);
	rnapt_netif_start(eth_netif, NULL);
#endif

#if defined(CONFIG_LWIP_USB_ETHERNET)
	/* Register and start USB ETH interface */
	rnapt_netif_t *usb_netif = rnapt_netif_create(NETIF_USB_ETH_INDEX, &g_rnapt_usb_eth_netif_config);
	rnapt_netif_start(usb_netif, NULL);
#endif

	rtos_time_delay_ms(500);

	/* Print initial status */
	rnapt_print_status();

	rtos_task_delete(NULL);
}

/**
 * @brief R-NAPT Example Main Function
 */
void example_rnapt(void)
{
	if (rtos_task_create(NULL, ((const char *)"rnapt_example"), rnapt_example_thread, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create R-NAPT example task failed\n");
		return;
	}

	/* Initialize DNS Relay service for LAN clients */
	dns_relay_service_init();
}
