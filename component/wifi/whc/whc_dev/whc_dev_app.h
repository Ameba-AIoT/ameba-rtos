#ifndef _WHC_DEV_APP_H_
#define _WHC_DEV_APP_H_

#include "whc_dev.h"
#include "whc_dev_ota.h"

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
/* Placeholder, used in linux host now */
#define WHC_WIFI_TEST_MP             0xB
#define WHC_WIFI_TEST_DBG            0xC

#define WHC_WIFI_TEST_OTA            0x10

#define WHC_RMESH_TEST 0xdda5a5a5
#define WHC_RMESH_TEST_SOCK_INIT    0x1
#define WHC_RMESH_TEST_SOCK_SEND    0x2

/* for rtos host only */
#define WHC_WIFI_TEST_SET_HOST_RTOS  0xFF

#define WHC_WIFI_TEST_BUF_SIZE     32

#define WHC_WHC_CMD_USER_TASK_STACK_SIZE		4096
#define CONFIG_WHC_WHC_CMD_USER_TASK_PRIO 		3

#define at_printf(fmt, args...)    RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, fmt, ##args)

struct whc_cmd_path_priv {
	rtos_sema_t whc_user_rx_sema;
	rtos_mutex_t whc_user_blocksend_mutex;
	rtos_sema_t whc_user_blocksend_sema;
	u8 *whc_rx_msg;
	/* spi add header before msg, different from others */
	/* real addr needed for mem free */
	u8 *whc_rx_msg_free_addr;
	u16 rx_msg_size;

	u8 *ret;
	u32 ret_len;
};

extern struct whc_cmd_path_priv whc_cmdpath_data;

#endif
