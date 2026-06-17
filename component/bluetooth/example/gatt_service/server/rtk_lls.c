/*
 *******************************************************************************
 * Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <stdio.h>
#include <string.h>

#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <rtk_bt_att_defs.h>
#include <rtk_bt_gatts.h>
#include <rtk_service_config.h>
#include <rtk_lls.h>
#include <bt_utils.h>

#define LINK_LOSS_SRV_UUID                  0x1803
#define ALERT_LEVEL_CHAR_UUID               0x2A06

#define RTK_BT_UUID_LINK_LOSS_SRV           BT_UUID_DECLARE_16(LINK_LOSS_SRV_UUID)
#define RTK_BT_UUID_ALERT_LEVEL_CHAR        BT_UUID_DECLARE_16(ALERT_LEVEL_CHAR_UUID)

#define ALERT_LEVEL_CHAR_VAL_INDEX          2   /* read & write */

static uint8_t alert_level = LLS_ALERT_LEVEL_NO_ALERT;

static rtk_bt_gatt_attr_t link_loss_attrs[] = {
	/* Primary Service: link_loss */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_LINK_LOSS_SRV),

	/* Characteristic: alert level */
	RTK_BT_GATT_CHARACTERISTIC(RTK_BT_UUID_ALERT_LEVEL_CHAR,
							   RTK_BT_GATT_CHRC_READ | RTK_BT_GATT_CHRC_WRITE,
							   RTK_BT_GATT_PERM_READ | RTK_BT_GATT_PERM_WRITE),
};

static struct rtk_bt_gatt_service link_loss_srv = RTK_BT_GATT_SERVICE(link_loss_attrs, LINK_LOSS_SRV_ID);


void link_loss_srv_callback(uint8_t event, void *data)
{
	uint16_t ret = 0;

	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] LLS register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] LLS register service failed, err: 0x%x\r\n", reg_srv_res->reg_status);
		}

		break;
	}

	case RTK_BT_GATTS_EVT_READ_IND: {
		rtk_bt_gatts_read_ind_t *p_read_ind = (rtk_bt_gatts_read_ind_t *)data;
		rtk_bt_gatts_read_resp_param_t read_resp = {0};
		read_resp.app_id = p_read_ind->app_id;
		read_resp.conn_handle = p_read_ind->conn_handle;
		read_resp.cid = p_read_ind->cid;
		read_resp.index = p_read_ind->index;
		if (ALERT_LEVEL_CHAR_VAL_INDEX == p_read_ind->index) {
			read_resp.data = &alert_level;
			read_resp.len = sizeof(alert_level);
		} else {
			BT_LOGE("[APP] LLS read event unknown index: %d\r\n", p_read_ind->index);
			read_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		}

		ret = rtk_bt_gatts_read_resp(&read_resp);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] LLS response for client read succeed!\r\n");
		} else {
			BT_LOGE("[APP] LLS response for client read failed, err: 0x%x\r\n", ret);
		}
		BT_AT_PRINT("+BLEGATTS:read_rsp,%d,%u,%u,%u,%d\r\n",
					(RTK_BT_OK == ret) ? 0 : -1, read_resp.app_id,
					read_resp.conn_handle, read_resp.index,
					read_resp.err_code);
		break;
	}

	case RTK_BT_GATTS_EVT_WRITE_IND: {
		rtk_bt_gatts_write_ind_t *p_write_ind = (rtk_bt_gatts_write_ind_t *)data;
		uint8_t *value;
		rtk_bt_gatts_write_resp_param_t write_resp = {0};
		write_resp.app_id = p_write_ind->app_id;
		write_resp.conn_handle = p_write_ind->conn_handle;
		write_resp.cid = p_write_ind->cid;
		write_resp.index = p_write_ind->index;
		write_resp.type = p_write_ind->type;

		if (!p_write_ind->len || !p_write_ind->value) {
			BT_LOGE("[APP] LLS write value is empty!\r\n");
			write_resp.err_code = RTK_BT_ATT_ERR_INVALID_VALUE_SIZE;
			goto send_write_rsp;
		}

		if (ALERT_LEVEL_CHAR_VAL_INDEX == p_write_ind->index) {
			const char *level_str;
			value = (uint8_t *)(p_write_ind->value);
			alert_level = *value;
			switch (alert_level) {
			case LLS_ALERT_LEVEL_NO_ALERT:
				level_str = "No Alert";
				break;
			case LLS_ALERT_LEVEL_MILD_ALERT:
				level_str = "Mild Alert";
				break;
			case LLS_ALERT_LEVEL_HIGH_ALERT:
				level_str = "High Alert";
				break;
			default:
				level_str = "Reserved";
				break;
			}
			BT_LOGA("[APP] LLS alert level set to %d (%s), type: %d\r\n",
					alert_level, level_str, p_write_ind->type);
			BT_AT_PRINT("+BLEGATTS:write,%u,%u,%u,%u,%u,%u\r\n",
						p_write_ind->app_id, p_write_ind->conn_handle, p_write_ind->index,
						p_write_ind->len, p_write_ind->type, alert_level);
		} else {
			BT_LOGE("[APP] LLS write event unknown index: %d\r\n", p_write_ind->index);
			write_resp.err_code = RTK_BT_ATT_ERR_ATTR_NOT_FOUND;
		}

send_write_rsp:
		ret = rtk_bt_gatts_write_resp(&write_resp);
		if (RTK_BT_OK == ret) {
			BT_LOGA("[APP] LLS response for client write success!\r\n");
		} else {
			BT_LOGE("[APP] LLS response for client write failed, err: 0x%x\r\n", ret);
		}
		BT_AT_PRINT("+BLEGATTS:write_rsp,%d,%u,%u,%u,%d,%d\r\n",
					(RTK_BT_OK == ret) ? 0 : -1, write_resp.app_id,
					write_resp.conn_handle, write_resp.index,
					write_resp.type, write_resp.err_code);
		break;
	}

	default:
		break;
	}
}

uint8_t link_loss_srv_get_alert_level(void)
{
	return alert_level;
}

uint16_t link_loss_srv_add(void)
{
	link_loss_srv.type = GATT_SERVICE_OVER_BLE;
	link_loss_srv.server_info = 0;
	link_loss_srv.user_data = NULL;
	link_loss_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&link_loss_srv);
}
