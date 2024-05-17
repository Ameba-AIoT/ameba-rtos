#ifndef EXAMPLE_CAPTIVE_PORTAL_H
#define EXAMPLE_CAPTIVE_PORTAL_H

/* ------------------------ FreeRTOS includes ----------------------------- */

/* ------------------------ lwIP includes --------------------------------- */
#include "lwip/api.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/memp.h"
#include "lwip/stats.h"
#include "netif/loopif.h"

/* ------------------------ Project includes ------------------------------ */
#include "main.h"
#include <lwip_netconf.h>

#include "wifi_intf_drv_to_app_basic.h"

void example_captive_portal(void);

enum {
	RTW_FALSE = 0,
	RTW_TRUE  = 1
};

typedef struct {
	char ssid[32 + 1];         /* The SSID of an access point. */
	signed char  ap_power;             /* Received Signal Strength Indication, min: -110, max: 0 */
	char bssid[6];             /* The BSSID of an access point. */
	u8   channel;              /* The RF frequency, 1-13 */
	rtw_security_t sec_type;     /* Security type, @ref wlan_sec_type_t */
} ap_list_t;

/*
 *  Scan result using normal scan.
 */
typedef struct {
	char ap_num; /* The number of access points found in scanning. */
	ap_list_t *ap_list;
} wifi_scan_result_t, scan_ap_list_t;

typedef struct _web_conn {
	struct netconn *conn;
	rtos_task_t task;
	int status;						/*!< Client socket descriptor for connection */
} web_conn;

typedef unsigned long rtw_bool_t;
/**
  * @brief  The structure is used to describe the data needed by scan result handler function.
  */
typedef struct rtw_scan_handler_result {
	rtw_scan_result_t ap_details;
	rtw_bool_t        scan_complete;
	void             *user_data;

} rtw_scan_handler_result_t;

struct rtw_wifi_config {
	unsigned int            boot_mode;
	unsigned char           ssid[32];
	unsigned char           ssid_len;
	unsigned char           security_type;
	unsigned char           password[65];
	unsigned char           password_len;
	unsigned char           channel;
};

#define SOFTAP_SSID				"captive_portal_test"
#define SOFTAP_PASSWORD			"12345678"				// NULL for RTW_SECURITY_OPEN
#define SOFTAP_CHANNEL			6
#define SOFTAP_SECURITY			RTW_SECURITY_WPA2_AES_PSK
#define SCAN_AP_LIST_MAX		10
#define MAX_HTTP_CONNECTIONS	(DEFAULT_ACCEPTMBOX_SIZE + 1)
#define CAPTIVE_PORTAL_DEBUG	0
#define CONFIG_READ_FLASH		0

rtw_softap_info_t SOFTAP_CONFIG;
struct netconn *pxHTTPListener = NULL;
web_conn *web_connections = NULL;

wifi_scan_result_t scan_result = {0};
rtos_sema_t scan_done_sema;
rtos_task_t webs_task = NULL;
rtos_sema_t webs_sema = NULL;
rtos_sema_t webs_wpage_sema = NULL;
u8_t webs_terminate = 0;

u8 connect_target_ap = 0;
u8 scan_ap = 0;

extern rtw_mode_t wifi_mode;
extern struct netif xnetif[NET_IF_NUM]; /* network interface structure */

#endif /* EXAMPLE_CAPTIVE_PORTAL_H */
