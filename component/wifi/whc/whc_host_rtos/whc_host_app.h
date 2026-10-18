#ifndef __WHC_HOST_APP_H__
#define __WHC_HOST_APP_H__
/* -------------------------------- Includes -------------------------------- */
/* external head files */
#include "rtw_wifi_common.h"

#include "os_wrapper.h"

#define WHC_HOST_CMD_USER_TASK_STACK_SIZE   1200
#define CONFIG_WHC_HOST_CMD_USER_TASK_PRIO  3

struct whc_host_cmd_path_priv {
	rtos_sema_t whc_user_rx_sema;
	u8 *whc_rx_buf;       /* original buffer (to be freed after processing) */
};

extern struct whc_host_cmd_path_priv whc_host_cmdpath_data;

/* -------------------------- Function declaration -------------------------- */
void whc_host_pkt_rx_to_user(u8 *payload, u32 len);
void whc_host_update_network_info(u8 wlan_idx);
void whc_host_cmd_path_init(void);

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
#define WHC_WIFI_TEST_MP             0xB
#define WHC_WIFI_TEST_DBG            0xC
#define WHC_WIFI_TEST_OTA            0x10
/* used in fullhan now */
#define WHC_WIFI_TEST_SOFTAP         0x11
#define WHC_WIFI_TEST_CONN_STATUS    0x12
#define WHC_WIFI_TEST_DISCONN        0x13
#define WHC_WIFI_TEST_WIFIOFF        0x14
#define WHC_WIFI_TEST_NETWORK_INFO_UPDATE	0x1a

/* for rtos host only */
#define WHC_WIFI_TEST_SET_HOST_RTOS  0xFF

#endif /* __WHC_HOST_APP_H__ */