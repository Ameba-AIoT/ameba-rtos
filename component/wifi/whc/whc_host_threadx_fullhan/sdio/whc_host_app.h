
#ifndef __WHC_HOST_APP_H__
#define __WHC_HOST_APP_H__
/* external head files */
#include "rtw_whc_common.h"

typedef unsigned int        WIFI_SECURITY_t;

#define WIFI_SSID_MAX                   32
#define WIFI_BSSID_LEN                  6
#define WIFI_PSK_LEN		            64
#define WIFI_WEP_KEY_LEN		        13

#define WIFI_SHARED_ENABLED             0x00008000
#define WIFI_WPA_SECURITY               0x00200000
#define WIFI_WPA2_SECURITY              0x00400000
#define WIFI_SECURITY_AUTO              0x0FFFFFFF
//for fullhan
typedef struct {
	unsigned char           ssid[WIFI_SSID_MAX + 1];
	unsigned char           bssid[WIFI_BSSID_LEN];
	int                     signal_level;
	WIFI_SECURITY_t         security;
	int                     channel;
} WIFI_SCAN_RESULT_t;

/* -------------------------- Function declaration -------------------------- */
void whc_host_pkt_rx_to_user(uint8_t *payload);

void whc_sdio_host_send_to_dev(uint8_t *buf, uint32_t len);
void whc_sdio_host_send_to_dev_block(uint8_t *buf, uint32_t len, uint8_t *ret, uint32_t ret_len);



//below for api ready
void whc_host_set_host(void);
void whc_host_wifi_on(void);

#endif