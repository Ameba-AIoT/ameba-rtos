/**
 * @file rnat_netif.h
 * @brief R-NAT Netif Management Header
 */

#ifndef __RNAT_NETIF_H
#define __RNAT_NETIF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "platform_autoconf.h"

#if defined(CONFIG_RNAT) && (CONFIG_RNAT == 1)

#include "lwip_netconf.h"
#include "dhcp/dhcps.h"

/* ======================================================================== */
/*                    Configuration Macros                                   */
/* ======================================================================== */

/**
 * @brief WiFi configuration structure
 */
typedef struct rnat_wifi_config {
	char ssid[33];
	char password[65];
	uint8_t channel;
} rnat_wifi_config_t;

/**
 * @brief IP configuration info structure
 */
typedef struct rnat_ip_info {
	ip4_addr_t ip;
	ip4_addr_t netmask;
	ip4_addr_t gw;
} rnat_ip_info_t;

/**
 * @brief R-NAT network role
 */
typedef enum {
	RNAT_ROLE_WAN = 0,
	RNAT_ROLE_LAN = 1
} rnat_role_t;

/**
 * @brief R-NAT IP configuration method
 */
typedef enum {
	RNAT_IP_METHOD_DHCP_CLIENT = 0,	/* DHCP client mode */
	RNAT_IP_METHOD_DHCP_SERVER = 1,	/* DHCP server mode */
	RNAT_IP_METHOD_STATIC = 2,			/* Static IP mode */
} rnat_ip_method_t;

/* Forward declaration for callback function */
typedef struct rnat_netif_obj rnat_netif_t;

/**
 * @brief R-NAT netif status callback function type
 * @param netif The netif object whose status changed
 * @param is_active true if interface link up, false if link down
 * @param user_data User-provided data pointer (can be NULL)
 */
typedef void (*rnat_netif_status_cb_t)(rnat_netif_t *netif, bool is_active, void *user_data);

/**
 * @brief R-NAT inherent config structure
 */
typedef struct rnat_netif_config {
	rnat_role_t role;
	rnat_ip_method_t ip_method;
	int priority;
	const rnat_ip_info_t *ip_info;
	const char *if_desc;
	rnat_netif_status_cb_t status_callback;
	void *callback_user_data;
} rnat_netif_config_t;

/**
 * @brief R-NAT Netif object structure
 */
typedef struct rnat_netif_obj {
	/* Basic Info */
	uint8_t idx;								/* Interface index (NETIF_WLAN_STA_INDEX, etc.) */
	char if_desc[16];							/* Interface description string */

	/* Status */
	bool is_active;								/* true=interface is active/link up, false=inactive */

	/* Configuration */
	rnat_role_t role;							/* Network role: WAN or LAN */
	rnat_ip_method_t ip_method;				/* IP configuration method */

	/* Management */
	int priority;								/* Priority for route selection */

	/* Callback */
	rnat_netif_status_cb_t status_callback;	/* User status callback function */
	void *callback_user_data;					/* User data passed to callback */

	/* LWIP integration */
	struct netif *lwip_netif;					/* Pointer to global xnetif[idx] */
	dhcps_t *dhcps_instance;					/* DHCPS instance pointer */
} rnat_netif_t;

/* ======================================================================== */
/*                           Core Management API                             */
/* ======================================================================== */

rnat_netif_t *rnat_netif_get(struct netif *lwip_netif);
void rnat_init(void);
void rnat_netif_set_status_callback(rnat_netif_t *netif, rnat_netif_status_cb_t callback, void *user_data);
rnat_netif_t *rnat_netif_create(uint8_t idx, const rnat_netif_config_t *config);
int rnat_netif_destroy(rnat_netif_t *netif);
int rnat_netif_start(rnat_netif_t *netif, void *arg);
int rnat_netif_stop(rnat_netif_t *netif);
void rnat_update_default_gw(void);
rnat_netif_t *rnat_get_default_gw(void);
void rnat_print_status(void);

#endif /* CONFIG_RNAT */

#ifdef __cplusplus
}
#endif

#endif /* __RNAT_NETIF_H */
