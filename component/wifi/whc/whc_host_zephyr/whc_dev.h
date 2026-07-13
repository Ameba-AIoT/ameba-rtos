#ifndef __WHC_DEV_H__
#define __WHC_DEV_H__

#ifdef ZEPHYR_TODO
//#define CONFIG_SDIO_TX_ENABLE_AVAL_INT 1
#endif

/* type */
#define WHC_WIFI_TEST  0x1
/* subtype */
#define WHC_WIFI_TEST_GET_MAC_ADDR 0x1
#define WHC_WIFI_TEST_GET_IP       0x2
#define WHC_WIFI_TEST_SET_READY    0x3
#define WHC_WIFI_TEST_SET_UNREADY  0x4
#define WHC_WIFI_TEST_SET_TICKPS_CMD 0x5
#define WHC_WIFI_TEST_WIFI_SCAN    0x6
#define WHC_WIFI_TEST_WIFI_CONN    0x7
#define WHC_WIFI_TEST_WIFI_DISCONN    0x8

/* for demo api, can be changed by user */
struct whc_api_info {
	u32	event;
	u32	api_id;
};

#endif