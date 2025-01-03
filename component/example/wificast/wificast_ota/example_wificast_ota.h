#ifndef _EXAMPLE_WIFICAST_OTA_H_
#define _EXAMPLE_WIFICAST_OTA_H_

#include "../wificast_defs.h"
#include "os_wrapper.h"
#include "flash_api.h"
#include "sys_api.h"
#include "ameba_ota.h"
#include "kv.h"

#define WIFI_CAST_OTA_PROGRESS_MAX		200
#define WIFI_CAST_OTA_PACKET_SIZE		256
#define WIFI_CAST_OTA_ROUND_MAX			50

//upgrade status
#define WIFI_CAST_OTA_ONGOING	0x1
#define WIFI_CAST_OTA_FINISHED	0x2
#define WIFI_CAST_OTA_RESET		0x3

#define WIFI_CAST_OTA_SENDER	0x1
#define WIFI_CAST_OTA_RECEIVER	0x2

#define WIFI_CAST_OTA_GET_BITS(data, bits)        ( (((uint8_t *)(data))[(bits) >> 0x3]) & ( 1 << ((bits) & 0x7)) )
#define WIFI_CAST_OTA_SET_BITS(data, bits)        do { (((uint8_t *)(data))[(bits) >> 0x3]) |= ( 1 << ((bits) & 0x7)); } while(0);

/**
 * @brief OTA packet head
 */
struct ota_packet_head {
	u32 seq;    /* packet sequence */
} __attribute__((packed));

/**
 * @brief OTA status
 */
struct example_ota_status {
	u16 status;			/* upgrade status */
	u16 packet_num;		/* upgrade packet number */
	u16 image_id;		/* upgrade image id */
	u32 checksum;		/* upgrade image checksum */
	u32 image_addr;		/* upgrade image address */
	u32 total_size;		/* upgrade firmware total size */
	u32 written_size;	/* upgrade firmware size has been written into flash */
	u8 progress_index;	/* upgrade process bitmap */
	u8 progress_array[0][WIFI_CAST_OTA_PROGRESS_MAX]; /* upgrade process bitmap */
} __attribute__((packed));

/**
 * @brief OTA result
 */
struct example_ota_result {
	u32 unfinished_num; /* upgrade total device number */
	u32 successed_num;  /* upgrade successed device number */
};

/**
 * @brief Receive data from wifi cast device
 */
struct example_cb_recv_data {
	u32 data_len;      /* receive data length */
	u8 mac[ETH_ALEN];  /* receive src mac address */
	u8 *data;          /* receive data body */
} __attribute__((packed));

#endif /* _EXAMPLE_WIFICAST_OTA_H_ */