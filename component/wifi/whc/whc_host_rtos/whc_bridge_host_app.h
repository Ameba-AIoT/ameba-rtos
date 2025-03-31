#ifndef __WHC_BRIDGE_HOST_APP_H__
#define __WHC_BRIDGE_HOST_APP_H__
/* -------------------------------- Includes -------------------------------- */
/* external head files */
#include "rtw_wifi_common.h"

/* -------------------------- Function declaration -------------------------- */
void whc_bridge_host_pkt_rx_to_user(u8 *payload, u32 len);
void whc_bridge_spi_host_send_to_dev(u8 *buf, u32 len);

/* for test demo */
#define WHC_WIFI_TEST 0xffa5a5a5

#define WHC_WIFI_TEST_GET_MAC_ADDR 0x1
#define WHC_WIFI_TEST_GET_IP       0x2
#define WHC_WIFI_TEST_SET_READY    0x3

#endif /* __WHC_BRIDGE_HOST_APP_H__ */