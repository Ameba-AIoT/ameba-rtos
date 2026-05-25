/**
 * @file rnapt_netif.c
 * @brief R-NAPT Netif Management
 */

#include "platform_autoconf.h"

#if defined(CONFIG_RNAPT) && (CONFIG_RNAPT == 1)

#include "rnapt_netif.h"
#include "ethernet.h"
#include "ameba_soc.h"
#include "dhcp/dhcps.h"
#include "wifi_api.h"
#include "lwip_ipnat.h"

#ifndef TAG
#define TAG "R-NAPT"
#endif

extern void ip_nat_reinitialize(void);
extern void ip_nat_sync_dns_serever_data(void);

/* ======================================================================== */
/*                           Global Variables                               */
/* ======================================================================== */

/**
 * @brief R-NAPT netif array
 */
static rnapt_netif_t *g_netif_array[NET_IF_NUM] = {NULL};

/**
 * @brief Current default gateway netif
 */
static rnapt_netif_t *g_default_gw = NULL;

/* ======================================================================== */
/*                           Helper Functions                               */
/* ======================================================================== */

/**
 * @brief  Validate role and IP method combination
 * @param  role Network role (WAN/LAN)
 * @param  ip_method IP configuration method
 * @param  ip_info IP configuration info (can be NULL)
 * @return true if valid, false if conflict detected
 */
static bool validate_role_and_ip_method(rnapt_role_t role, rnapt_ip_method_t ip_method, const rnapt_ip_info_t *ip_info)
{
	if (role == RNAPT_ROLE_WAN) {
		/* WAN interface cannot act as DHCP server */
		if (ip_method == RNAPT_IP_METHOD_DHCP_SERVER) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "WAN interface cannot use DHCP_SERVER mode\n");
			return false;
		}
	} else if (role == RNAPT_ROLE_LAN) {
		/* LAN interface cannot act as DHCP client */
		if (ip_method == RNAPT_IP_METHOD_DHCP_CLIENT) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "LAN interface cannot use DHCP_CLIENT mode\n");
			return false;
		}
	}

	if (ip_method == RNAPT_IP_METHOD_STATIC && ip_info == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "STATIC mode requires custom IP configuration\n");
		return false;
	}

	return true;
}

/**
 * @brief  Get IP method string by ip_method value
 * @param  ip_method IP configuration method
 * @return String representation of the IP method
 */
static const char *get_ip_method_str(rnapt_ip_method_t ip_method)
{
	switch (ip_method) {
	case RNAPT_IP_METHOD_DHCP_CLIENT:
		return "DHCP-Client";
	case RNAPT_IP_METHOD_DHCP_SERVER:
		return "DHCP-Server";
	case RNAPT_IP_METHOD_STATIC:
		return "Static";
	default:
		return "Unknown";
	}
}

/**
 * @brief Get interface description string by index
 * @param  idx Interface index
 * @return Pointer to description string
 */
static const char *get_if_desc_by_idx(uint8_t idx)
{
	switch (idx) {
	case NETIF_WLAN_STA_INDEX:
		return "STA";
	case NETIF_WLAN_AP_INDEX:
		return "AP";
#if defined(CONFIG_LWIP_ETHERNET)
	case NETIF_ETH_INDEX:
		return "ETH";
#endif
#if defined(CONFIG_LWIP_USB_ETHERNET)
	case NETIF_USB_ETH_INDEX:
		return "USB";
#endif
	default:
		return "UNKNOWN";
	}
}

/**
 * @brief  Find rnapt_netif from lwip netif pointer
 * @param  lwip_netif LWIP netif pointer
 * @return R-NAPT netif object or NULL if not found
 */
rnapt_netif_t *rnapt_netif_get(struct netif *lwip_netif)
{
	for (int i = 0; i < NET_IF_NUM; i++) {
		if (g_netif_array[i] && g_netif_array[i]->lwip_netif == lwip_netif) {
			return g_netif_array[i];
		}
	}
	return NULL;
}

/* ======================================================================== */
/*                          NETIF Extension Callback                        */
/* ======================================================================== */

#if LWIP_NETIF_EXT_STATUS_CALLBACK

/**
 * @brief LWIP netif extension callback
 */
static void rnapt_netif_ext_callback(struct netif *netif,
									 netif_nsc_reason_t reason,
									 const netif_ext_callback_args_t *args)
{
	(void)args;

	rnapt_netif_t *rnapt_netif = rnapt_netif_get(netif);
	if (!rnapt_netif) {
		return;
	}

	/* Update active flag based on link status */
	if (reason & LWIP_NSC_LINK_CHANGED) {
		bool link_up = netif_is_link_up(netif);

		if (link_up != rnapt_netif->is_active) {
			rnapt_netif->is_active = link_up;
			RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] LINK %s\n", rnapt_netif->if_desc,
					 link_up ? "UP - ACTIVE" : "DOWN - INACTIVE");

			/* Call user status callback if registered */
			if (rnapt_netif->status_callback) {
				rnapt_netif->status_callback(rnapt_netif, link_up, rnapt_netif->callback_user_data);
			}

			/* Update default gw on link status change */
			if (rnapt_netif->role == RNAPT_ROLE_LAN ||
				(rnapt_netif->role == RNAPT_ROLE_WAN && rnapt_netif->ip_method == RNAPT_IP_METHOD_STATIC)) {
				rnapt_update_default_gw();
			}
		}
	}

	/* Handle IPv4 address changes */
	if (reason & LWIP_NSC_IPV4_ADDRESS_CHANGED) {
		if (rnapt_netif->role == RNAPT_ROLE_WAN &&
			rnapt_netif->ip_method == RNAPT_IP_METHOD_DHCP_CLIENT) {
			/* Reinitialize NAPT and sync DNS when WAN gets new IP */
			RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] WAN got new IP, reinitializing NAPT\n",
					 rnapt_netif->if_desc);

			/* Reinitialize NAPT tables */
			ip_nat_reinitialize();

			/* Sync DNS server data from the new WAN interface */
			ip_nat_sync_dns_serever_data();

			/* Update default gateway */
			rnapt_update_default_gw();
		}
	}
}

NETIF_DECLARE_EXT_CALLBACK(rnapt_callback);

#endif /* LWIP_NETIF_EXT_STATUS_CALLBACK */

/* ======================================================================== */
/*                        Core Management API Implementation                 */
/* ======================================================================== */

/**
 * @brief Initialize R-NAPT system
 */
void rnapt_init(void)
{
	g_default_gw = NULL;
	memset(g_netif_array, 0, sizeof(g_netif_array));

	netif_add_ext_callback(&rnapt_callback, rnapt_netif_ext_callback);
}

/**
 * @brief Set status callback for a netif
 * @param netif R-NAPT netif object
 * @param callback Callback function (NULL to disable)
 * @param user_data User data passed to callback
 */
void rnapt_netif_set_status_callback(rnapt_netif_t *netif, rnapt_netif_status_cb_t callback, void *user_data)
{
	if (!netif) {
		return;
	}

	netif->status_callback = callback;
	netif->callback_user_data = user_data;
}

/**
 * @brief  Create and register a R-NAPT netif
 * @param  idx Interface index
 * @param  config Inherent configuration
 * @return rnapt_netif_t pointer on success, NULL on failure
 */
rnapt_netif_t *rnapt_netif_create(uint8_t idx, const rnapt_netif_config_t *config)
{
	if (!config || idx >= NET_IF_NUM) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid configuration or index\n");
		return NULL;
	}

	/* Validate role and IP method combination */
	if (!validate_role_and_ip_method(config->role, config->ip_method, config->ip_info)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid role and IP method combination\n");
		return NULL;
	}

	/* Check if already registered */
	if (g_netif_array[idx]) {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Netif[%d] already registered\n", idx);
		return g_netif_array[idx];
	}

	/* Allocate memory */
	rnapt_netif_t *netif = malloc(sizeof(rnapt_netif_t));
	if (!netif) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Failed to allocate memory for netif[%d]\n", idx);
		return NULL;
	}

	memset(netif, 0, sizeof(rnapt_netif_t));

	/* Initialize fields from config */
	netif->idx = idx;
	netif->role = config->role;
	netif->ip_method = config->ip_method;
	netif->priority = config->priority;
	netif->is_active = false;
	netif->lwip_netif = &xnetif[idx];
	netif->dhcps_instance = NULL;
	netif->status_callback = config->status_callback;
	netif->callback_user_data = config->callback_user_data;

	/* Set interface description */
	if (config->if_desc) {
		strncpy(netif->if_desc, config->if_desc, sizeof(netif->if_desc) - 1);
		netif->if_desc[sizeof(netif->if_desc) - 1] = '\0';
	} else {
		strncpy(netif->if_desc, get_if_desc_by_idx(idx), sizeof(netif->if_desc) - 1);
		netif->if_desc[sizeof(netif->if_desc) - 1] = '\0';
	}

	/* IP configuration */
	if (config->role == RNAPT_ROLE_LAN && config->ip_method == RNAPT_IP_METHOD_DHCP_SERVER) {
		/* For DHCP Server: Check subnet conflict and alloc non-conflicting IP */

		/* Stop and deinit existing DHCP Server */
		if (netif->dhcps_instance) {
			dhcps_stop(netif->lwip_netif);
			dhcps_deinit(netif->lwip_netif);
			netif->dhcps_instance = NULL;
		}

		struct ip_addr check_ip;
		bool use_custom_ip = false;

		if (config->ip_info != NULL) {
			ip_addr_copy_from_ip4(check_ip, config->ip_info->ip);
			RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Custom IP: %d.%d.%d.%d\n", netif->if_desc,
					 ip4_addr1(&config->ip_info->ip), ip4_addr2(&config->ip_info->ip),
					 ip4_addr3(&config->ip_info->ip), ip4_addr4(&config->ip_info->ip));

			/* Check if custom IP subnet conflicts */
			if (!lwip_subnet_is_used(&check_ip)) {
				/* No conflict, use custom IP */
				use_custom_ip = true;
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "[%s] Custom IP subnet conflicts, auto-allocating...\n",
						 netif->if_desc);
			}
		}

		/* Alloc IP: use custom IP if no conflict, otherwise auto-alloc */
		if (use_custom_ip) {
			netifapi_netif_set_addr(netif->lwip_netif, &config->ip_info->ip,
									&config->ip_info->netmask, &config->ip_info->gw);
		} else {
			/* Alloc non-conflicting IP from pool */
			if (lwip_alloc_ip(netif->idx) != 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "[%s] Failed to alloc IP\n", netif->if_desc);
				free(netif);
				return NULL;
			}
		}
	} else if (config->ip_method == RNAPT_IP_METHOD_STATIC && config->ip_info != NULL) {
		/* STATIC mode with Custom IP: set IP directly, no DHCP */
		RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Custom IP: %d.%d.%d.%d\n", netif->if_desc,
				 ip4_addr1(&config->ip_info->ip), ip4_addr2(&config->ip_info->ip),
				 ip4_addr3(&config->ip_info->ip), ip4_addr4(&config->ip_info->ip));

		netifapi_netif_set_addr(netif->lwip_netif, &config->ip_info->ip,
								&config->ip_info->netmask, &config->ip_info->gw);
	}

	g_netif_array[idx] = netif;

	RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Created (role=%s, ip_method=%d, prio=%d)\n",
			 netif->if_desc, config->role == RNAPT_ROLE_WAN ? "WAN" : "LAN",
			 config->ip_method, config->priority);

	return netif;
}

/**
 * @brief  Unregister and destroy a R-NAPT netif
 * @param  netif R-NAPT netif object to destroy
 * @return 0 on success, negative on failure
 */
int rnapt_netif_destroy(rnapt_netif_t *netif)
{
	if (!netif) {
		return -1;
	}

	/* Stop first if active */
	if (netif->is_active) {
		rnapt_netif_stop(netif);
	}

	/* Ensure DHCP Server is fully deinitialized */
	if (netif->dhcps_instance) {
		dhcps_stop(netif->lwip_netif);
		dhcps_deinit(netif->lwip_netif);
		netif->dhcps_instance = NULL;
	}

	/* Remove from array */
	for (int i = 0; i < NET_IF_NUM; i++) {
		if (g_netif_array[i] == netif) {
			g_netif_array[i] = NULL;
			break;
		}
	}

	free(netif);

	RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Destroyed\n", netif->if_desc);
	return 0;
}

/**
 * @brief  Start WiFi STA interface
 * @param  netif R-NAPT object to start
 * @param  arg WiFi configuration pointer
 * @return 0 on success, negative on failure
 */
static int rnapt_netif_start_sta(rnapt_netif_t *netif, void *arg)
{
	int ret = 0;
	struct rtw_network_info wifi_cfg = {0};
	char password_buf[65] = {0};

	/* Check if STA config is provided */
	if (!arg) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[%s] Config not provided\n", netif->if_desc);
		return -1;
	}

	const rnapt_wifi_config_t *sta_config = (const rnapt_wifi_config_t *)arg;

	RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Connecting to '%s'...\n", netif->if_desc, sta_config->ssid);

	/* Configure SSID */
	wifi_cfg.ssid.len = strlen(sta_config->ssid);
	strncpy((char *)wifi_cfg.ssid.val, sta_config->ssid, sizeof(wifi_cfg.ssid.val) - 1);

	/* Configure password and security type */
	if (strlen(sta_config->password) > 0) {
		strncpy(password_buf, sta_config->password, sizeof(password_buf) - 1);
		wifi_cfg.password = (u8 *)password_buf;
		wifi_cfg.password_len = strlen(sta_config->password);
		wifi_cfg.security_type = RTW_SECURITY_WPA2_AES_PSK;
	} else {
		wifi_cfg.security_type = RTW_SECURITY_OPEN;
	}

	/* Configure channel */
	wifi_cfg.channel = sta_config->channel;

	/* Connect */
	ret = wifi_connect(&wifi_cfg, 1);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[%s] wifi_connect failed (ret=%d)\n", netif->if_desc, ret);
		return -1;
	}

	/* Request IP address */
	if (netif->ip_method == RNAPT_IP_METHOD_DHCP_CLIENT) {
		u32 dhcp_status = lwip_request_ip(netif->idx);
		if (dhcp_status == DHCP_ADDRESS_ASSIGNED) {
			uint8_t *ip = lwip_get_ip(netif->idx);
			RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] DHCP got IP: %d.%d.%d.%d\n",
					 netif->if_desc, ip[0], ip[1], ip[2], ip[3]);
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "[%s] DHCP failed to get IP\n", netif->if_desc);
		}
	}

	return 0;
}

/**
 * @brief  Start WiFi AP interface
 * @param  netif R-NAPT object to start
 * @param  arg AP configuration pointer
 * @return 0 on success, negative on failure
 */
static int rnapt_netif_start_ap(rnapt_netif_t *netif, void *arg)
{
	int ret = 0;
	struct rtw_softap_info ap_cfg = {0};
	int timeout = 20;

	/* Check if AP config is provided */
	if (!arg) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[%s] Config not provided\n", netif->if_desc);
		return -1;
	}

	const rnapt_wifi_config_t *ap_config = (const rnapt_wifi_config_t *)arg;
	strncpy((char *)ap_cfg.ssid.val, ap_config->ssid, sizeof(ap_cfg.ssid.val) - 1);
	ap_cfg.ssid.len = strlen(ap_config->ssid);

	/* Configure password and security type */
	if (strlen(ap_config->password) > 0) {
		ap_cfg.password = (u8 *)ap_config->password;
		ap_cfg.password_len = strlen(ap_config->password);
		ap_cfg.security_type = RTW_SECURITY_WPA2_AES_PSK;
	} else {
		ap_cfg.security_type = RTW_SECURITY_OPEN;
	}

	/* Configure channel */
	if (ap_config->channel == 0) {
		/* Auto channel selection via ACS */
		struct rtw_acs_config acs_config = {0};
		ret = wifi_acs_find_ideal_channel(&acs_config, &ap_cfg.channel);
		if (ret != RTK_SUCCESS) {
			/* Fallback to default channel 6 for 2.4GHz */
			ap_cfg.channel = 6;
			RTK_LOGS(TAG, RTK_LOG_WARN, "[%s] ACS failed, using default channel %d\n",
					 netif->if_desc, ap_cfg.channel);
		}
	} else {
		/* Use manually configured channel */
		ap_cfg.channel = ap_config->channel;
	}

	/* Deinit DHCP Server */
	if (netif->ip_method == RNAPT_IP_METHOD_DHCP_SERVER) {
		dhcps_deinit(netif->lwip_netif);
		netif->dhcps_instance = NULL;
	}

	/* Start AP */
	wifi_stop_ap();
	ret = wifi_start_ap(&ap_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[%s] wifi_start_ap failed (ret=%d)\n", netif->if_desc, ret);
		return -1;
	}

	/* Wait for AP to be ready */
	while (timeout > 0) {
		struct rtw_wifi_setting setting;
		wifi_get_setting(SOFTAP_WLAN_INDEX, &setting);
		if (strlen((char *)setting.ssid) > 0) {
			if (strcmp((char *)setting.ssid, (char *)ap_cfg.ssid.val) == 0) {
				break;
			}
		}
		rtos_time_delay_ms(1000);
		timeout--;
	}

	if (timeout == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[%s] Start timeout\n", netif->if_desc);
		return -1;
	}

	/* Initialize and start DHCP Server */
	if (netif->ip_method == RNAPT_IP_METHOD_DHCP_SERVER) {
		netif->dhcps_instance = dhcps_init(netif->lwip_netif);
		dhcps_start(netif->lwip_netif);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] '%s' started on channel %d\n", netif->if_desc, ap_cfg.ssid.val, ap_cfg.channel);

	return 0;
}

#if defined(CONFIG_LWIP_ETHERNET)
/**
 * @brief ETH link up/down callback for R-NAPT
 * @param link_up 1=link up, 0=link down
 */
static void rnapt_eth_link_callback(int link_up)
{
	/* Get the corresponding rnapt_netif */
	rnapt_netif_t *eth_netif = rnapt_netif_get(pnetif_eth);
	if (!eth_netif) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cannot find ETH rnapt_netif\n");
		return;
	}

	if (link_up) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "=== ETH Link UP ===\n");

		if (!netif_is_up(eth_netif->lwip_netif)) {
			netifapi_netif_set_up(eth_netif->lwip_netif);
		}
		netifapi_netif_set_link_up(eth_netif->lwip_netif);

		if (eth_netif->ip_method == RNAPT_IP_METHOD_DHCP_CLIENT) {
			/* WAN mode: DHCP Client */
			RTK_LOGS(TAG, RTK_LOG_INFO, "ETH Mode: DHCP Client\n");

			u32 dhcp_status = lwip_request_ip(NETIF_ETH_INDEX);
			if (dhcp_status == DHCP_ADDRESS_ASSIGNED) {
				uint8_t *ip = lwip_get_ip(NETIF_ETH_INDEX);
				RTK_LOGS(TAG, RTK_LOG_INFO, "ETH DHCP got IP: %d.%d.%d.%d\n",
						 ip[0], ip[1], ip[2], ip[3]);
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "ETH DHCP failed to get IP\n");
			}
		} else if (eth_netif->ip_method == RNAPT_IP_METHOD_DHCP_SERVER) {
			/* LAN mode: DHCP Server */
			RTK_LOGS(TAG, RTK_LOG_INFO, "ETH Mode: DHCP Server\n");

			/* Deinit DHCP server first */
			dhcps_deinit(eth_netif->lwip_netif);
			eth_netif->dhcps_instance = NULL;

			/* Initialize and start DHCP Server */
			eth_netif->dhcps_instance = dhcps_init(eth_netif->lwip_netif);
			if (eth_netif->dhcps_instance) {
				dhcps_start(eth_netif->lwip_netif);
				RTK_LOGS(TAG, RTK_LOG_INFO, "ETH DHCP Server started: %d.%d.%d.%d\n",
						 ip4_addr1(netif_ip4_addr(eth_netif->lwip_netif)),
						 ip4_addr2(netif_ip4_addr(eth_netif->lwip_netif)),
						 ip4_addr3(netif_ip4_addr(eth_netif->lwip_netif)),
						 ip4_addr4(netif_ip4_addr(eth_netif->lwip_netif)));
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Failed to init ETH DHCP Server\n");
			}
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "=== ETH Link DOWN ===\n");

		if (eth_netif->ip_method == RNAPT_IP_METHOD_DHCP_CLIENT) {
			netifapi_netif_set_link_down(eth_netif->lwip_netif);
			lwip_clear_ip(NETIF_ETH_INDEX);

			RTK_LOGS(TAG, RTK_LOG_INFO, "ETH IP released\n");
		} else if (eth_netif->ip_method == RNAPT_IP_METHOD_DHCP_SERVER) {
			dhcps_stop(eth_netif->lwip_netif);
			dhcps_deinit(eth_netif->lwip_netif);
			eth_netif->dhcps_instance = NULL;

			netifapi_netif_set_link_down(eth_netif->lwip_netif);
			RTK_LOGS(TAG, RTK_LOG_INFO, "ETH DHCP Server stopped\n");
		}
	}
}

/**
 * @brief  Start Ethernet interface
 * @param  netif R-NAPT object to start
 * @return 0 on success, negative on failure
 */
static int rnapt_netif_start_eth(rnapt_netif_t *netif)
{
	/* Register eth link callback */
	eth_register_link_cb(rnapt_eth_link_callback);

	/* Initialize ETH */
	eth_init();
	RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Initialized\n", netif->if_desc);

	return 0;
}
#endif

#if defined(CONFIG_LWIP_USB_ETHERNET)
/**
 * @brief  Start USB-ECM interface
 * @param  netif R-NAPT object to start
 * @return 0 on success, negative on failure
 */
static int rnapt_netif_start_usb(rnapt_netif_t *netif)
{
	/* Check if USB has valid link and IP address (may already be connected) */
	if (netif_is_up(netif->lwip_netif) && netif_is_link_up(netif->lwip_netif)) {
		netif->is_active = true;
		RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Connected\n", netif->if_desc);
	}

	return 0;
}
#endif

/**
 * @brief  Start a network interface
 * @param  netif R-NAPT netif object to start
 * @param  arg Generic argument pointer
 * @return 0 on success, negative on failure
 */
int rnapt_netif_start(rnapt_netif_t *netif, void *arg)
{
	if (!netif) {
		return -1;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Starting netif...\n", netif->if_desc);

	switch (netif->idx) {
	case NETIF_WLAN_STA_INDEX:
		return rnapt_netif_start_sta(netif, arg);
	case NETIF_WLAN_AP_INDEX:
		return rnapt_netif_start_ap(netif, arg);
#if defined(CONFIG_LWIP_ETHERNET)
	case NETIF_ETH_INDEX:
		return rnapt_netif_start_eth(netif);
#endif
#if defined(CONFIG_LWIP_USB_ETHERNET)
	case NETIF_USB_ETH_INDEX:
		return rnapt_netif_start_usb(netif);
#endif
	default:
		RTK_LOGS(TAG, RTK_LOG_WARN, "[%s] Unknown interface index %d\n", netif->if_desc, netif->idx);
		return -1;
	}
}

/**
 * @brief  Stop a network interface
 * @param  netif R-NAPT netif object to stop
 * @return 0 on success, negative on failure
 */
int rnapt_netif_stop(rnapt_netif_t *netif)
{
	if (!netif) {
		return -1;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Stopping...\n", netif->if_desc);

	switch (netif->idx) {
	case NETIF_WLAN_STA_INDEX:
		/* STA: Disconnect */
		wifi_disconnect();
		if (netif->ip_method == RNAPT_IP_METHOD_DHCP_CLIENT) {
			lwip_clear_ip(NETIF_WLAN_STA_INDEX);
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Disconnected\n", netif->if_desc);
		break;
	case NETIF_WLAN_AP_INDEX:
		/* AP: Deinit DHCP server and stop AP */
		if (netif->ip_method == RNAPT_IP_METHOD_DHCP_SERVER) {
			dhcps_deinit(netif->lwip_netif);
		}
		wifi_stop_ap();
		RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Stopped\n", netif->if_desc);
		break;
#if defined(CONFIG_LWIP_ETHERNET)
	case NETIF_ETH_INDEX:
		/* ETH: Cannot be stopped manually */
		RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Cannot stop manually, unplug cable\n", netif->if_desc);
		break;
#endif
#if defined(CONFIG_LWIP_USB_ETHERNET)
	case NETIF_USB_ETH_INDEX:
		/* USB: Cannot be stopped manually */
		RTK_LOGS(TAG, RTK_LOG_INFO, "[%s] Cannot stop manually, unplug device\n", netif->if_desc);
		break;
#endif
	default:
		RTK_LOGS(TAG, RTK_LOG_WARN, "[%s] Unknown interface index %d\n", netif->if_desc, netif->idx);
		break;
	}

	return 0;
}

/**
 * @brief Update default gateway based on route priority
 */
void rnapt_update_default_gw(void)
{
	rnapt_netif_t *best_netif = NULL;
	int max_priority = -1;

	/* Iterate through all registered netifs using array */
	for (int i = 0; i < NET_IF_NUM; i++) {
		rnapt_netif_t *current = g_netif_array[i];
		if (!current) {
			continue;
		}

		if (current->is_active && (netif_ip4_addr(current->lwip_netif)->addr != 0)) {

			/* Select highest priority */
			if (current->priority > max_priority) {
				max_priority = current->priority;
				best_netif = current;
			}
		}
	}

	/* Update default gateway if changed */
	if (best_netif && best_netif != g_default_gw) {
		netif_set_default(best_netif->lwip_netif);
		g_default_gw = best_netif;

		RTK_LOGS(TAG, RTK_LOG_INFO, "Default GW changed to: %d.%d.%d.%d (%s, prio=%d)\n",
				 ip4_addr1(netif_ip4_addr(best_netif->lwip_netif)),
				 ip4_addr2(netif_ip4_addr(best_netif->lwip_netif)),
				 ip4_addr3(netif_ip4_addr(best_netif->lwip_netif)),
				 ip4_addr4(netif_ip4_addr(best_netif->lwip_netif)),
				 best_netif->if_desc, max_priority);
	}
}

/**
 * @brief  Get current default gateway netif
 * @return Default gateway netif or NULL
 */
rnapt_netif_t *rnapt_get_default_gw(void)
{
	return g_default_gw;
}

/**
 * @brief Print current R-NAPT status
 */
void rnapt_print_status(void)
{
	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "========== R-NAPT Status ==========\n");

	for (int i = 0; i < NET_IF_NUM; i++) {
		rnapt_netif_t *current = g_netif_array[i];
		if (!current) {
			continue;
		}

		const char *role_str = (current->role == RNAPT_ROLE_WAN) ? "WAN" : "LAN";
		const char *method_str = get_ip_method_str(current->ip_method);
		const char *status = current->is_active ? "UP" : "DOWN";
		const char *is_gw = (current == g_default_gw) ? " [DEFAULT GW]" : "";

		if (netif_is_up(current->lwip_netif)) {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS,
					 "[%s] %s: IP=%d.%d.%d.%d GW=%d.%d.%d.%d Role=%s Method=%s Prio=%d%s\n",
					 current->if_desc, status,
					 ip4_addr1(netif_ip4_addr(current->lwip_netif)),
					 ip4_addr2(netif_ip4_addr(current->lwip_netif)),
					 ip4_addr3(netif_ip4_addr(current->lwip_netif)),
					 ip4_addr4(netif_ip4_addr(current->lwip_netif)),
					 ip4_addr1(netif_ip4_gw(current->lwip_netif)),
					 ip4_addr2(netif_ip4_gw(current->lwip_netif)),
					 ip4_addr3(netif_ip4_gw(current->lwip_netif)),
					 ip4_addr4(netif_ip4_gw(current->lwip_netif)),
					 role_str, method_str, current->priority, is_gw);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ALWAYS,
					 "[%s] %s: IP=0.0.0.0 GW=0.0.0.0 Role=%s Method=%s Prio=%d%s\n",
					 current->if_desc, status, role_str, method_str, current->priority, is_gw);
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ALWAYS, "====================================\n\n");
}

#endif /* CONFIG_RNAPT */
