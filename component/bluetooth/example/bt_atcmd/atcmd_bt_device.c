/*
 *******************************************************************************
 * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include <atcmd_service.h>
#include <bt_api_config.h>
#include <rtk_bt_device.h>
#include <bt_utils.h>
#include <atcmd_bt_impl.h>

rtk_bt_app_conf_t app_conf = {
	.app_profile_support = RTK_BT_PROFILE_GATTS,
	.mtu_size = 180,
	.prefer_all_phy = 0,
	.prefer_tx_phy = 1 | 1 << 1 | 1 << 2,
						   .prefer_rx_phy = 1 | 1 << 1 | 1 << 2,
						   .max_tx_octets = 0x40,
						   .max_tx_time = 0x200,
};

int atcmd_bt_device(int argc, char *argv[])
{
	(void)argc;
	int en = str_to_int(argv[0]);
	if (1 == en) {
		if (RTK_BT_FAIL == rtk_bt_enable(&app_conf)) {
			BTDEVICE_AT_PRINTK("BT enable failed!");
			return -1;
		}

		BTDEVICE_AT_PRINTK("BT enable OK!");
	} else if (0 == en) {
		if (RTK_BT_FAIL == rtk_bt_disable()) {
			BTDEVICE_AT_PRINTK("BT disable failed!");
			return -1;
		}

		BTDEVICE_AT_PRINTK("BT disable OK!");
	} else {
		BTDEVICE_AT_PRINTK("BT input wrong args!");
		return -1;
	}

	return 0;
}
