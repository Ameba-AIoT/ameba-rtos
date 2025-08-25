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

#if defined(CONFIG_AMEBASMART)
#define WIFI_CAST_OTA_IC   0x1
#elif defined(CONFIG_AMEBALITE)
#define WIFI_CAST_OTA_IC   0x2
#elif defined (CONFIG_AMEBADPLUS)
#define WIFI_CAST_OTA_IC   0x3
#else
#define WIFI_CAST_OTA_IC   0x4
#endif
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
	u32 total_size;		/* upgrade image total size */
	u32 written_size;	/* upgrade image size has been written into flash */
	u8 ic_index;		/* upgrade IC index */
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

#endif /* _EXAMPLE_WIFICAST_OTA_H_ */