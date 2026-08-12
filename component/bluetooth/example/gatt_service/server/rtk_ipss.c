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
#include <rtk_ipss.h>
#include <bt_utils.h>

#define IPSS_SRV_UUID                       0x1820

#define RTK_BT_UUID_IPSS                    BT_UUID_DECLARE_16(IPSS_SRV_UUID)

static rtk_bt_gatt_attr_t ipss_attrs[] = {
	/* Primary Service: ipss */
	RTK_BT_GATT_PRIMARY_SERVICE(RTK_BT_UUID_IPSS),
};

static struct rtk_bt_gatt_service ipss_srv = RTK_BT_GATT_SERVICE(ipss_attrs, IPSS_SRV_ID);


void ipss_srv_callback(uint8_t event, void *data)
{
	switch (event) {
	case RTK_BT_GATTS_EVT_REGISTER_SERVICE: {
		rtk_bt_gatts_reg_ind_t *reg_srv_res = (rtk_bt_gatts_reg_ind_t *)data;
		if (RTK_BT_OK == reg_srv_res->reg_status) {
			BT_LOGA("[APP] IPSS register service succeed!\r\n");
		} else {
			BT_LOGE("[APP] IPSS register service failed, err: 0x%x\r\n", reg_srv_res->reg_status);
		}

		break;
	}

	default:
		break;
	}
}

uint16_t ipss_srv_add(void)
{
	ipss_srv.type = GATT_SERVICE_OVER_BLE;
	ipss_srv.server_info = 0;
	ipss_srv.user_data = NULL;
	ipss_srv.register_status = 0;

	return rtk_bt_gatts_register_service(&ipss_srv);
}
