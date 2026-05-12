/**
 * @file rnapt_netif.h
 * @brief R-NAPT Netif Management Header
 */

#ifndef __RNAPT_NETIF_H
#define __RNAPT_NETIF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "platform_autoconf.h"

#if defined(CONFIG_RNAPT) && (CONFIG_RNAPT == 1)

#include "lwip_netconf.h"
#include "dhcp/dhcps.h"

/* ======================================================================== */
/*                    Configuration Macros                                   */
/* ======================================================================== */

/**
 * @brief WiFi configuration structure
 */
typedef struct rnapt_wifi_config {
	char ssid[33];
	char password[65];
	uint8_t channel;
} rnapt_wifi_config_t;

/**
 * @brief IP configuration info structure
 */
typedef struct rnapt_ip_info {
	ip4_addr_t ip;
	ip4_addr_t netmask;
	ip4_addr_t gw;
} rnapt_ip_info_t;

/**
 * @brief R-NAPT network role
 */
typedef enum {
	RNAPT_ROLE_WAN = 0,
	RNAPT_ROLE_LAN = 1
} rnapt_role_t;

/**
 * @brief R-NAPT IP configuration method
 */
typedef enum {
	RNAPT_IP_METHOD_DHCP_CLIENT = 0,	/* DHCP client mode */
	RNAPT_IP_METHOD_DHCP_SERVER = 1,	/* DHCP server mode */
	RNAPT_IP_METHOD_STATIC = 2,			/* Static IP mode */
} rnapt_ip_method_t;

/* Forward declaration for callback function */
typedef struct rnapt_netif_obj rnapt_netif_t;

/**
 * @brief R-NAPT netif status callback function type
 * @param netif The netif object whose status changed
 * @param is_active true if interface link up, false if link down
 * @param user_data User-provided data pointer (can be NULL)
 */
typedef void (*rnapt_netif_status_cb_t)(rnapt_netif_t *netif, bool is_active, void *user_data);

/**
 * @brief R-NAPT inherent config structure
 */
typedef struct rnapt_netif_config {
	rnapt_role_t role;
	rnapt_ip_method_t ip_method;
	int priority;
	const rnapt_ip_info_t *ip_info;
	const char *if_desc;
	rnapt_netif_status_cb_t status_callback;
	void *callback_user_data;
} rnapt_netif_config_t;

/**
 * @brief R-NAPT Netif object structure
 */
typedef struct rnapt_netif_obj {
	/* Basic Info */
	uint8_t idx;								/* Interface index (NETIF_WLAN_STA_INDEX, etc.) */
	char if_desc[16];							/* Interface description string */

	/* Status */
	bool is_active;								/* true=interface is active/link up, false=inactive */

	/* Configuration */
	rnapt_role_t role;							/* Network role: WAN or LAN */
	rnapt_ip_method_t ip_method;				/* IP configuration method */

	/* Management */
	int priority;								/* Priority for route selection */

	/* Callback */
	rnapt_netif_status_cb_t status_callback;	/* User status callback function */
	void *callback_user_data;					/* User data passed to callback */

	/* LWIP integration */
	struct netif *lwip_netif;					/* Pointer to global xnetif[idx] */
	dhcps_t *dhcps_instance;					/* DHCPS instance pointer */
} rnapt_netif_t;

/* ======================================================================== */
/*                           Core Management API                             */
/* ======================================================================== */

rnapt_netif_t *rnapt_netif_get(struct netif *lwip_netif);
void rnapt_init(void);
void rnapt_netif_set_status_callback(rnapt_netif_t *netif, rnapt_netif_status_cb_t callback, void *user_data);
rnapt_netif_t *rnapt_netif_create(uint8_t idx, const rnapt_netif_config_t *config);
int rnapt_netif_destroy(rnapt_netif_t *netif);
int rnapt_netif_start(rnapt_netif_t *netif, void *arg);
int rnapt_netif_stop(rnapt_netif_t *netif);
void rnapt_update_default_gw(void);
rnapt_netif_t *rnapt_get_default_gw(void);
void rnapt_print_status(void);

#endif /* CONFIG_RNAPT */

#ifdef __cplusplus
}
#endif

#endif /* __RNAPT_NETIF_H */
