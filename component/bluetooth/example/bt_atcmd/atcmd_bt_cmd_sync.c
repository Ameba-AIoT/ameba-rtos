/*
 *******************************************************************************
 * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */
#include <string.h>
#include <basic_types.h>
#include <osif.h>
#include <rtk_bt_common.h>
#include <atcmd_bt_cmd_sync.h>

#if defined(BT_AT_SYNC) && BT_AT_SYNC

static uint8_t sync_enable = 0;
static struct bt_at_sync_t bt_at_sync = {0};


void bt_at_sync_enable(uint8_t enable)
{
	sync_enable = enable;
}

uint16_t bt_at_sync_init(uint16_t cmd_type, uint16_t op, int32_t conn_handle)
{
	if (sync_enable) {
		memset(&bt_at_sync, 0, sizeof(bt_at_sync));

		if (false == osif_sem_create(&bt_at_sync.psem, 0, 1)) {
			return BT_AT_ERR_OS_OPERATION;
		}

		bt_at_sync.exec = TRUE;
		bt_at_sync.cmd_type = cmd_type;
		bt_at_sync.op = op;
		bt_at_sync.conn_handle = conn_handle;
	}

	return BT_AT_OK;
}

void bt_at_sync_deinit(void)
{
	if (sync_enable) {
		if (bt_at_sync.psem) {
			osif_sem_delete(bt_at_sync.psem);
			bt_at_sync.psem = NULL;
		}

		memset(&bt_at_sync, 0, sizeof(bt_at_sync));
	}
}

uint16_t bt_at_sync_sem_take(void)
{
	if (sync_enable && bt_at_sync.exec) {
		if (bt_at_sync.psem != NULL) {
			if (false == osif_sem_take(bt_at_sync.psem, BT_TIMEOUT_FOREVER)) {
				return BT_AT_ERR_CMD_TIMEOUT;
			}
		}
	}

	return BT_AT_OK;
}

void bt_at_sync_sem_give(void)
{
	if (sync_enable && bt_at_sync.exec) {
		if (bt_at_sync.psem != NULL) {
			osif_sem_give(bt_at_sync.psem);
		}
	}
}

uint8_t bt_at_sync_event_match_check(uint8_t evt_code)
{
	uint8_t match = FALSE;

	if (!sync_enable || !bt_at_sync.exec) {
		return FALSE;
	}

	switch (bt_at_sync.cmd_type) {
	case BT_AT_SYNC_CMD_TYPE_BLE_GAP_ADV:
		if (bt_at_sync.op == BT_AT_SYNC_OP_TYPE_START && evt_code == RTK_BT_LE_GAP_EVT_ADV_START_IND) {
			match = TRUE;
		} else if (bt_at_sync.op == BT_AT_SYNC_OP_TYPE_STOP && evt_code == RTK_BT_LE_GAP_EVT_ADV_STOP_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GAP_SCAN:
		if (bt_at_sync.op == BT_AT_SYNC_OP_TYPE_START && evt_code == RTK_BT_LE_GAP_EVT_SCAN_START_IND) {
			match = TRUE;
		} else if (bt_at_sync.op == BT_AT_SYNC_OP_TYPE_STOP && evt_code == RTK_BT_LE_GAP_EVT_SCAN_STOP_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GAP_CONN:
		if (evt_code == RTK_BT_LE_GAP_EVT_CONNECT_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GAP_DISCONN:
		if (evt_code == RTK_BT_LE_GAP_EVT_DISCONN_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GAP_CONN_UPDATE:
		if (evt_code == RTK_BT_LE_GAP_EVT_CONN_UPDATE_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GAP_BOND_DEL:
	case BT_AT_SYNC_CMD_TYPE_BLE_GAP_BOND_CLEAR:
		if (evt_code == RTK_BT_LE_GAP_EVT_BOND_MODIFY_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GATTC_DISC:
		if (evt_code == RTK_BT_GATTC_EVT_DISCOVER_RESULT_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GATTC_READ:
		if (evt_code == RTK_BT_GATTC_EVT_READ_RESULT_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GATTC_WRITE:
		if (evt_code == RTK_BT_GATTC_EVT_WRITE_RESULT_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GATTS_NOTIFY:
		if (evt_code == RTK_BT_GATTS_EVT_NOTIFY_COMPLETE_IND) {
			match = TRUE;
		}
		break;
	case BT_AT_SYNC_CMD_TYPE_BLE_GATTS_INDICATE:
		if (evt_code == RTK_BT_GATTS_EVT_INDICATE_COMPLETE_IND) {
			match = TRUE;
		}
		break;
	default:
		break;
	}

	return match;
}

uint16_t bt_at_sync_get_result(void)
{
	return bt_at_sync.result;
}

void bt_at_sync_set_result(uint16_t res)
{
	bt_at_sync.result = (res == 0) ? BT_AT_OK : BT_AT_ERR_LOWER_STACK;
}

void bt_at_sync_disconnect_hdl(uint16_t conn_handle)
{
	if (!sync_enable || !bt_at_sync.exec) {
		return;
	}

	if (bt_at_sync.conn_handle != conn_handle) {
		return;
	}

	if (bt_at_sync_event_match_check(RTK_BT_LE_GAP_EVT_DISCONN_IND)) {
		bt_at_sync.result = BT_AT_OK;
	} else {
		bt_at_sync.result = BT_AT_ERR_NO_CONNECTION;
	}

	if (bt_at_sync.psem != NULL) {
		osif_sem_give(bt_at_sync.psem);
	}
	return;
}

#endif /* BT_AT_SYNC */