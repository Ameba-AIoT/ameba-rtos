/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <string.h>
#include <stdio.h>
#include <bt_api_config.h>
#if defined(RTK_BLE_ISO_SUPPORT) && RTK_BLE_ISO_SUPPORT
#include "app_le_iso_common.h"

rtk_bt_le_iso_setup_path_param_t app_le_iso_setup_path_param = {
	0,                                      /*cis_conn_handle */
	0,                                      /*data_path_direction */
	0,                                      /*data_path_id 0x00 is for hci, others for vendor */
	{0x03, 0, 0, 0, 0},                     /*codec_id: Octet 0 for Coding format 0x03 for Transparent Mode
                                                        Octets 1 to 2 company ID
                                                        Octets 3 to 4 Vendor-defined codec ID */
	0,                                      /*controller_delay */
	0,                                      /*codec_config_len 0 for Transparent Mode */
	0,                                      /*codec_data */
};

extern bool le_iso_demo_init_flag;

static bool app_bt_le_iso_check_demo_init(void)
{
	if (le_iso_demo_init_flag) {
		return true;
	} else {
		return false;
	}
}

uint16_t app_bt_le_iso_cis_id_availiable_check(uint8_t le_iso_role, uint8_t cis_id, uint8_t cig_id)
{
	uint16_t ret = RTK_BT_FAIL;

	if (!app_bt_le_iso_check_demo_init()) {
		BT_LOGE("%s bt le iso demo has not been initialized \r\n", __func__);
		ret = RTK_BT_ERR_NOT_READY;
		goto exit;
	}
	if (!cis_id || !cig_id) {
		BT_LOGE("%s cig id 0x%x and cis id 0x%x should not be 0! \r\n", __func__, cis_id, cig_id);
		ret = RTK_BT_ERR_PARAM_INVALID;
		goto exit;
	}
	if (cig_id > RTK_BLE_CIG_MAX_NUM || cis_id > RTK_BLE_CIS_MAX_NUM) {
		BT_LOGE("%s cig id 0x%x or cis id 0x%x is overflow than max cig number or cis number %d \r\n", __func__,
				cig_id, cis_id, (int)RTK_BLE_CIG_MAX_NUM, (int)RTK_BLE_CIS_MAX_NUM);
		ret = RTK_BT_ERR_PARAM_INVALID;
		goto exit;
	}
	switch (le_iso_role) {
	case RTK_BLE_ISO_ROLE_CIS_INITIATOR: {
		uint8_t cig_index = cig_id - 1;
		uint8_t cis_index = cis_id - 1;
		osif_mutex_take(g_le_iso_cis_initiator_status.mutex, BT_TIMEOUT_FOREVER);
		if (!g_le_iso_cis_initiator_status.initiator_info[cig_index].cis_info[cis_index].active) {
			BT_LOGA("%s cig id 0x%x cis id 0x%x is available \r\n", __func__, cig_id, cis_id);
			ret = RTK_BT_OK;
		} else {
			ret = RTK_BT_ERR_BUSY;
		}
		osif_mutex_give(g_le_iso_cis_initiator_status.mutex);
		break;
	}

	case RTK_BLE_ISO_ROLE_CIS_ACCEPTOR: {
		uint8_t cig_index = cig_id - 1;
		uint8_t cis_index = cis_id - 1;
		osif_mutex_take(g_le_iso_cis_acceptor_status.mutex, BT_TIMEOUT_FOREVER);
		if (!g_le_iso_cis_acceptor_status.acceptor_info[cig_index].cis_info[cis_index].active) {
			BT_LOGA("%s cig id 0x%x cis id 0x%x is available \r\n", __func__, cig_id, cis_id);
			ret = RTK_BT_OK;
		} else {
			ret = RTK_BT_ERR_BUSY;
		}
		osif_mutex_give(g_le_iso_cis_acceptor_status.mutex);
		break;
	}

	default:
		BT_LOGE("%s invalid le iso role 0x%x \r\n", __func__, le_iso_role);
		break;
	}

exit:
	return ret;
}


#endif