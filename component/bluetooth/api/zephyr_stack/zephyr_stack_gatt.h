/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __ZEPHYR_STACK_GATT_H__
#define __ZEPHYR_STACK_GATT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr_config.h>
//GATTS Definition
//
//

#define BT_GATTS_MAX_INCL_SRV_NUM 32

typedef struct {
	struct list_head list;
	uint16_t app_id;
	struct bt_gatt_service svc;
	bool registered;
	uint8_t include_ref;
} zephyr_svc_node;

typedef struct {
	uint16_t app_id;
	uint16_t attr_index;
	struct bt_gatt_indicate_params params;
} zephyr_indicate_req_t;

//GATTC Definition
//
//
typedef enum {
	BT_STACK_GATTC_DISC_REQ  = 1,
	BT_STACK_GATTC_READ_REQ,
	BT_STACK_GATTC_WRITE,
	BT_STACK_GATTC_SUBSCRIBE,
	BT_STACK_GATTC_UNKNOWN = 0xFF,
} zephyr_bt_gattc_req_type_t;

typedef struct {
	uint16_t profile_id;
	rtk_bt_gattc_discover_type_t type;
	struct bt_gatt_discover_params params;
	struct bt_uuid_128 uuid;
} zephyr_bt_disc_req_t;

typedef struct {
	uint16_t profile_id;
	struct bt_gatt_read_params params;
	struct bt_uuid_128 uuid;
} zephyr_bt_read_req_t;

typedef struct {
	uint16_t profile_id;
	rtk_bt_gattc_write_type_t type;
	uint16_t handle;
	struct bt_gatt_write_params params;
} zephyr_bt_write_req_t;

typedef enum {
	ZEPHYR_SUBSCRIBE_INIT,
	ZEPHYR_SUBSCRIBE_PROGRESSING,
	ZEPHYR_SUBSCRIBE_DONE,
} zephyr_bt_subscribe_status_t;

typedef struct {
	struct list_head list;
	uint16_t conn_handle;
	uint16_t profile_id;
	bool notify;
	zephyr_bt_subscribe_status_t status;
	struct bt_gatt_subscribe_params params;
} zephyr_bt_subscribe_t;

#ifdef __cplusplus
}
#endif

#endif  /* __RTK_STACK_GATT_H__ */
