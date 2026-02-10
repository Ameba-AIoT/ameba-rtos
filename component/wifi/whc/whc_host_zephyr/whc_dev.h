#ifndef __WHC_DEV_H__
#define __WHC_DEV_H__

#ifdef ZEPHYR_TODO
//#define CONFIG_SDIO_TX_ENABLE_AVAL_INT 1
#endif

#define WHC_WIFI_TEST_GET_MAC_ADDR 0x1
#define WHC_WIFI_TEST_GET_IP       0x2
#define WHC_WIFI_TEST_SET_READY    0x3
#define WHC_WIFI_TEST_SET_UNREADY  0x4
#define WHC_WIFI_TEST_SET_TICKPS_CMD 0x5
#define WHC_WIFI_TEST_WIFI_SCAN    0x6
#define WHC_WIFI_TEST_WIFI_CONN    0x7
#define WHC_WIFI_TEST_WIFI_DISCONN    0x8

struct whc_msg_info {
	uint32_t	event;
	uint32_t	wlan_idx;
	uint32_t	data_len;
	uint32_t	pad_len;
};

/* for demo api, can be changed by user */
struct whc_api_info {
	u32	event;
	u32	api_id;
};

enum WHC_WIFI_CTRL_TYPE {
	INIC_WIFI_EVT_XIMT_PKTS = 0xa5a5a500,
	INIC_WIFI_EVT_RECV_PKTS,

	WHC_WIFI_TEST = 0xffa5a5a5,

	INIC_WIFI_EVT_MAX,
};

struct event_priv_t {
	struct k_mutex send_mutex;
	struct k_sem api_ret_sema;

	uint8_t *rx_ret_msg;

	uint8_t b_waiting_for_ret: 1;
};

#endif