/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include "platform_autoconf.h"
#include <stdio.h>
#include <osif.h>
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>

static bool b_bt_enabled = false;

extern uint16_t bt_stack_enable(void *app_default_conf);
extern uint16_t bt_stack_disable(void);

bool rtk_bt_is_enable(void)
{
	return b_bt_enabled;
}

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
static bool mesh_stack_is_init = false;
bool rtk_bt_mesh_is_enable(void)
{
	return mesh_stack_is_init;
}
#endif
bool rtk_bt_pre_enable(void);
void rtk_bt_post_enable(void);
uint16_t rtk_bt_enable(rtk_bt_app_conf_t *app_default_conf)
{
	uint16_t err = 0;

	if (b_bt_enabled == true) {
		BT_LOGE("%s: bt is enabled already.\r\n", __func__);
		return RTK_BT_ERR_ALREADY_DONE;
	}

	if (!rtk_bt_pre_enable()) {
		BT_LOGE("%s: rtk_bt_pre_enable fail!\r\n", __func__);
		return RTK_BT_ERR_NOT_READY;
	}

	err = rtk_bt_evt_init();
	if (err) {
		return err;
	}
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	if (app_default_conf->app_profile_support & RTK_BT_PROFILE_MESH) {
		mesh_stack_is_init = true;
	}
#endif

	err = bt_stack_enable(app_default_conf);
	if (err) {
		rtk_bt_evt_deinit();
#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
		if (app_default_conf->app_profile_support & RTK_BT_PROFILE_MESH) {
			mesh_stack_is_init = false;
		}
#endif
		return err;
	}

	rtk_bt_post_enable();
	rtk_bt_log_init();

	/* set the bt enable flag on */
	b_bt_enabled = true;

	return 0;
}

uint16_t rtk_bt_disable(void)
{
	uint16_t err = 0;

	if (b_bt_enabled == false) {
		BT_LOGE("%s: bt is not enabled.\r\n", __func__);
		return RTK_BT_ERR_ALREADY_DONE;
	}

	/* set the bt enable flag off */
	b_bt_enabled = false;

	err = bt_stack_disable();
	if (err) {
		return err;
	}

	err = rtk_bt_evt_deinit();
	if (err) {
		return err;
	}

	rtk_bt_log_deinit();

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
	mesh_stack_is_init = false;
#endif

	return 0;
}


