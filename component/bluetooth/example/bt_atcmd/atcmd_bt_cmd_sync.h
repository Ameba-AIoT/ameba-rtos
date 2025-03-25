/*
 *******************************************************************************
 * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#ifndef __ATCMD_BT_CMD_SYNC_H__
#define __ATCMD_BT_CMD_SYNC_H__

#include <bt_utils.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(BT_AT_SYNC) && BT_AT_SYNC

#define BT_AT_SYNC_CONN_HANDLE_INVALID      (-1)

enum bt_at_sync_op_type_e {
	BT_AT_SYNC_OP_TYPE_NONE = 0,
	BT_AT_SYNC_OP_TYPE_START,
	BT_AT_SYNC_OP_TYPE_STOP,
	BT_AT_SYNC_OP_TYPE_MAX,
};

enum bt_at_sync_cmd_type_e {
	BT_AT_SYNC_CMD_TYPE_NONE = 0,
	BT_AT_SYNC_CMD_TYPE_BLE_GAP_ADV,
	BT_AT_SYNC_CMD_TYPE_BLE_GAP_SCAN,
	BT_AT_SYNC_CMD_TYPE_BLE_GAP_CONN,
	BT_AT_SYNC_CMD_TYPE_BLE_GAP_DISCONN,
	BT_AT_SYNC_CMD_TYPE_BLE_GAP_CONN_UPDATE,
	BT_AT_SYNC_CMD_TYPE_BLE_GAP_BOND_DEL,
	BT_AT_SYNC_CMD_TYPE_BLE_GAP_BOND_CLEAR,
	BT_AT_SYNC_CMD_TYPE_BLE_GATTC_DISC,
	BT_AT_SYNC_CMD_TYPE_BLE_GATTC_READ,
	BT_AT_SYNC_CMD_TYPE_BLE_GATTC_WRITE,
	BT_AT_SYNC_CMD_TYPE_BLE_GATTS_NOTIFY,
	BT_AT_SYNC_CMD_TYPE_BLE_GATTS_INDICATE,
	BT_AT_SYNC_CMD_TYPE_MAX,
};

enum bt_at_evt_result_e {
	BT_AT_EVT_RESULT_SUCCESS = 0,
	BT_AT_EVT_RESULT_FAIL = 1,
};

struct bt_at_sync_t {
	uint16_t  exec;
	uint16_t  cmd_type;
	uint16_t  op;
	uint16_t  result;
	int32_t   conn_handle;
	void      *psem;
};

uint16_t bt_at_sync_init(uint16_t cmd_type, uint16_t op, int32_t conn_handle);

uint16_t bt_at_sync_sem_take(void);

void bt_at_sync_sem_give(void);

uint16_t bt_at_sync_get_result(void);

void bt_at_sync_set_result(uint16_t res);

void bt_at_sync_deinit(void);

uint8_t bt_at_sync_event_match_check(uint8_t evt_code);

void bt_at_sync_enable(uint8_t enable);

void bt_at_sync_disconnect_hdl(uint16_t conn_handle);

#endif /* BT_AT_SYNC */

#ifdef __cplusplus
}
#endif

#endif /* __ATCMD_BT_CMD_SYNC_H__ */