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
#define WHC_WIFI_TEST_MP             0xB
#define WHC_WIFI_TEST_DBG            0xC
#define WHC_WIFI_TEST_OTA            0x10
/* used in fullhan now */
#define WHC_WIFI_TEST_SOFTAP         0x11
#define WHC_WIFI_TEST_CONN_STATUS    0x12
#define WHC_WIFI_TEST_DISCONN        0x13
#define WHC_WIFI_TEST_WIFIOFF        0x14
#define WHC_WIFI_TEST_LOG_ENABLE     0x15
#define WHC_WIFI_TEST_LOG_DISABLE    0x16
#define WHC_WIFI_TEST_CLEAR_OTA      0x17
/* host→device: transparent shell command string (NUL-terminated) */
#define WHC_WIFI_TEST_SHELL_CMD      0x18
/* device→host: AT command response text from at_printf() */
#define WHC_WIFI_TEST_AT_RESP        0x19
/* device→host: ACK for LOG_ENABLE/DISABLE; payload = WHC_WIFI_TEST(4B) | LOG_ACK(1B) | op(1B) */
#define WHC_WIFI_TEST_LOG_ACK        0x1B
/* for rtos host only */
#define WHC_WIFI_TEST_SET_HOST_RTOS  0xFF

#define WHC_LOG_EVENT                0x4

#define WHC_RMESH_TEST 0xdda5a5a5
#define WHC_RMESH_TEST_SOCK_INIT    0x1
#define WHC_RMESH_TEST_SOCK_SEND    0x2

#define WHC_WIFI_TEST_BUF_SIZE     32

#define WHC_WHC_CMD_USER_TASK_STACK_SIZE		4096
#define CONFIG_WHC_WHC_CMD_USER_TASK_PRIO 		3

/* In WHC_DEV mode at_printf routes AT responses back to the host via the AT_RESP channel. */
#if !(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
#define at_printf(fmt, args...)    RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, fmt, ##args)
#endif

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

#ifdef CONFIG_LOG_FWD
void whc_dev_log_forward_init(void);
#endif

#endif
