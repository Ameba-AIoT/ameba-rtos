#ifndef __WHC_BRIDGE_HOST_APP_H__
#define __WHC_BRIDGE_HOST_APP_H__
/* -------------------------------- Includes -------------------------------- */
/* external head files */
#include "rtw_wifi_common.h"

/* -------------------------- Function declaration -------------------------- */
void whc_host_pkt_rx_to_user(u8 *payload, u32 len);

/* for test demo */
#define WHC_WIFI_TEST 0xffa5a5a5

#define WHC_WIFI_TEST_GET_MAC_ADDR   0x1
#define WHC_WIFI_TEST_GET_IP         0x2
#define WHC_WIFI_TEST_SET_READY      0x3
#define WHC_WIFI_TEST_SET_UNREADY    0x4
#define WHC_WIFI_TEST_SET_TICKPS_CMD 0x5
#define WHC_WIFI_TEST_CONNECT        0x6
#define WHC_WIFI_TEST_SCAN           0x7
#define WHC_WIFI_TEST_DHCP           0x8
#define WHC_WIFI_TEST_WIFION         0x9
#define WHC_WIFI_TEST_SCAN_RESULT    0xA

/* for rtos host only */
#define WHC_WIFI_TEST_SET_HOST_RTOS  0xFF

#endif /* __WHC_BRIDGE_HOST_APP_H__ */