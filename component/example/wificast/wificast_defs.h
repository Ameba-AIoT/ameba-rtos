#ifndef _WIFICAST_DEFS_H_
#define _WIFICAST_DEFS_H_

#include "wifi_intf_drv_to_app_cast.h"

/**
 * @brief Application message type
 */
#define WIFI_CAST_CONTROL_LIGHT                BIT(15)
#define WIFI_CAST_CONTROL_BIND                 BIT(14)
#define WIFI_CAST_UART_DATA                    BIT(13)
#define WIFI_CAST_SCAN_REQUEST                 BIT(12)
#define WIFI_CAST_SCAN_RESPONSE                BIT(11)
#define WIFI_CAST_SEC_PUBKEY_EXCHANGE_REQUEST  BIT(10)
#define WIFI_CAST_SEC_PUBKEY_EXCHANGE_RESPONSE BIT(9)
#define WIFI_CAST_OTA_STATUS_REQUEST           BIT(8)
#define WIFI_CAST_OTA_STATUS_RESPONSE          BIT(7)
#define WIFI_CAST_OTA_DATA                     BIT(6)
#define WIFI_CAST_DEBUG_COMMAND                BIT(5)
#define WIFI_CAST_DEBUG_LOG                    BIT(4)

/**
 * @brief Wifi cast application example frame head.
 */
struct example_frame_head {
	uint16_t type;      /* application message type */
	uint16_t len;       /* application message length */
} __attribute__((packed));

/**
 * @brief Wifi cast application example scan info.
 */
struct example_scan_info {
	uint8_t mac[6];      /* device mac address */
} __attribute__((packed));

/**
 * @brief Scan device list.
 */
struct example_node_list {
	uint8_t is_init;                             /* scan list initialize flag */
	int size;                                    /* scan list size */
	struct example_scan_info info[MAX_NODE_NUM]; /* scan list */
};

/**
 * @brief Receive data from wifi cast device
 */
struct example_cb_recv_data {
	u32 data_len;      /* receive data length */
	u8 mac[ETH_ALEN];  /* receive src mac address */
	u8 *data;          /* receive data body */
} __attribute__((packed));

#endif