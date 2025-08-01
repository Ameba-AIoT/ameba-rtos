/*
*******************************************************************************
* Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdlib.h>
#include <string.h>
#include <atcmd_service.h>
#include <bt_utils.h>
#include <rtk_bt_vendor.h>
#include <rtk_bt_le_gap.h>
#include <rtk_bt_device.h>
#include <osif.h>
#include <bt_api_config.h>
#include <atcmd_bt_impl.h>

#ifndef false
#define false 0
#endif

#ifndef true
#define true 1
#endif

/**
 * @brief  Read ASCII string and convert to uint32_t.
 *
 * @param p
 * @return
*/
static uint32_t cmd_string2uint32(char *p)
{
	uint32_t result = 0;
	char     ch;
	bool     hex = false;

	/* check if value is dec */
	if (p[0] == 'x' || p[0] == 'X') {
		hex = true;
		p = &p[1];
	} else if ((p[0] == '0') && (p[1] == 'x' || p[1] == 'X')) {
		hex = true;
		p = &p[2];
	}

	for (;;) {
		ch = *(p++) | 0x20;                 /* convert to lower case */

		if (hex) {                          /* dec value */
			/* hex value */
			if ((ch >= 'a') && (ch <= 'f')) {
				ch -= ('a' - 10);
			} else if ((ch >= '0') && (ch <= '9')) {
				ch -= '0';
			} else {
				break;
			}
			result = (result << 4);
			result += (ch & 0x0f);
		} else {
			if (ch < '0' || ch > '9') {
				break;    /* end of string reached */
			}
			result = 10 * result + ch - '0';
		}
	}
	return (result);
}

rtk_bt_app_conf_t app_conf = {
	.app_profile_support = RTK_BT_PROFILE_GATTS,
	.mtu_size = 180,
	.prefer_all_phy = RTK_BT_LE_PHYS_PREFER_ALL,
	.prefer_tx_phy = RTK_BT_LE_PHYS_PREFER_1M | RTK_BT_LE_PHYS_PREFER_2M | RTK_BT_LE_PHYS_PREFER_CODED,
	.prefer_rx_phy = RTK_BT_LE_PHYS_PREFER_1M | RTK_BT_LE_PHYS_PREFER_2M | RTK_BT_LE_PHYS_PREFER_CODED,
	.max_tx_octets = 0x40,
	.max_tx_time = 0x200,
};

int atcmd_bt_enable(int argc, char *argv[])
{
	(void)argc;
	int en = str_to_int(argv[0]);
	if (1 == en) {
		if (RTK_BT_FAIL == rtk_bt_enable(&app_conf)) {
			BT_LOGE("BT enable failed!\r\n");
			return -1;
		}

		BT_LOGA("BT enable OK!\r\n");
	} else if (0 == en) {
		if (RTK_BT_FAIL == rtk_bt_disable()) {
			BT_LOGE("BT disable failed!\r\n");
			return -1;
		}

		BT_LOGA("BT disable OK!\r\n");
	} else {
		BT_LOGE("BT input wrong args!\r\n");
		return -1;
	}

	return 0;
}

int atcmd_bt_power(int argc, char *argv[])
{
	(void)argc;
	int en = str_to_int(argv[0]);
	if (1 == en) {
		rtk_bt_controller_power_on();
		BT_LOGA("BT power on OK!\r\n");
	} else if (0 == en) {
		rtk_bt_controller_power_off();
		BT_LOGA("BT power off OK!\r\n");
	} else {
		BT_LOGE("BT input wrong args!\r\n");
		return -1;
	}

	return 0;
}

int atcmd_bt_tx_power_gain(int argc, char *argv[])
{
	uint32_t index = 0;
	uint8_t index_le, index_br, index_edr2m, index_edr3m;

	if (argc != 1 && argc != 4) {
		BT_LOGE("Set tx power gain fail, wrong parameter number!\r\n");
		return 0;
	}

	index_le = (uint8_t)cmd_string2uint32(argv[0]); //le index
	index |= index_le;

	if (argc == 4) { // for ameba smart & lite
		index_br = (uint8_t)cmd_string2uint32(argv[1]); //br index
		index_edr2m = (uint8_t)cmd_string2uint32(argv[2]); //edr 2m index
		index_edr3m = (uint8_t)cmd_string2uint32(argv[3]); //edr 3m index

		index |= (index_br << 8);
		index |= (index_edr2m << 16);
		index |= (index_edr3m << 24);
	}

	rtk_bt_set_bt_tx_power_gain_index(index);

	BT_LOGA("Set tx power gain 0x%08x OK!\r\n", index);

	return 0;
}

int atcmd_bt_hci_debug_enable(int argc, char *argv[])
{
	(void)argc;
	(void)argv;

	rtk_bt_hci_debug_enable();

	BT_LOGA("HCI debug enable OK!\r\n");

	return 0;
}

int atcmd_bt_debug_port(int argc, char *argv[])
{
	uint8_t bt_sel = 0;  // 0: bt vendor; 1: bt on
	uint8_t type = 0;
	uint32_t bt_bdg_mask = 0;
	uint8_t original = 0;
	uint8_t mapping = 0;

	uint8_t bt_dbg_port = 0;
	char *pad = NULL;

	if (argc < 3 && argc > 5) {
		BT_LOGE("Set bt debug port fail, wrong parameter number!\r\n");
		return 0;
	}

	if (strcmp("enable", argv[0]) == 0) {
		if (strcmp("bt_vendor", argv[1]) == 0) {
			bt_sel = 0;
		} else if (strcmp("bt_on", argv[1]) == 0) {
			bt_sel = 1;
		}

		type = (uint8_t)str_to_int(argv[2]);
		if (type > 1) {
			BT_LOGE("Set bt debug port enable fail, wrong type!\r\n");
			return 0;
		}

		if (type == 0) {
			if (argc != 4) {
				BT_LOGE("Set bt debug port enable fail, wrong parameter number!\r\n");
				return 0;
			}
			bt_bdg_mask = (uint32_t)str_to_int(argv[3]);
			rtk_bt_debug_port_mask_enable(bt_sel, bt_bdg_mask);
		} else if (type == 1) {
			bt_dbg_port = (uint8_t)str_to_int(argv[3]);
			if (argc > 4) {
				pad = argv[4];
			}
			rtk_bt_debug_port_pad_enable(bt_sel, bt_dbg_port, pad);
		}
	} else if (strcmp("shift", argv[0]) == 0) {
		if (argc != 3) {
			BT_LOGE("Set bt debug port shift fail, wrong parameter number!\r\n");
			return 0;
		}

		original = (uint8_t)str_to_int(argv[1]);
		mapping = (uint8_t)str_to_int(argv[2]);
		if (original > 31 || mapping > 7) {
			BT_LOGE("Set bt debug port shift fail, wrong original or mapping!\r\n");
			return 0;
		}
		rtk_bt_debug_port_shift(original, mapping);
	} else {
		BT_LOGE("Set bt debug port fail, wrong parameter argv [%s]!\r\n", argv[0]);
		return 0;
	}

	BT_LOGA("BT debug port set OK!\r\n");
	return 0;
}

int atcmd_bt_gpio(int argc, char *argv[])
{
	uint8_t bt_gpio = 0;
	char *pad = NULL;

	if (argc != 1 && argc != 2) {
		BT_LOGE("Set bt gpio fail, wrong parameter number!\r\n");
		return 0;
	}

	bt_gpio = (uint8_t)str_to_int(argv[0]);
	if (argc == 2) {
		pad = argv[1];
	}
	rtk_bt_gpio_enable(bt_gpio, pad);

	BT_LOGA("BT GPIO set OK!\r\n");
	return 0;
}

int atcmd_bt_set_tx_power(int argc, char *argv[])
{
	rtk_bt_vendor_tx_power_param_t tx_power = {0};

	if (argc != 3 && argc != 4) {
		BT_LOGE("Set tx power fail, wrong parameter number!\r\n");
		return 0;
	}

	tx_power.tx_power_type = str_to_int(argv[0]);
	if (0 == tx_power.tx_power_type && 3 == argc) {
		tx_power.adv_tx_power.type = str_to_int(argv[1]);
		tx_power.tx_gain = str_to_int(argv[2]);

		if (tx_power.adv_tx_power.type > ADV_TX_POW_SET_2M_DEFAULT) {
			BT_LOGE("Set tx power fail, wrong adv tx power!\r\n");
			return 0;
		}

	} else if (1 == tx_power.tx_power_type && 4 == argc) {
		tx_power.conn_tx_power.conn_handle = (uint16_t)str_to_int(argv[1]);
		tx_power.conn_tx_power.is_reset = str_to_int(argv[2]);
		tx_power.tx_gain = str_to_int(argv[3]);

		if (tx_power.conn_tx_power.is_reset != CONN_TX_POW_USER_MODE && tx_power.conn_tx_power.is_reset != CONN_TX_POW_RESET_TO_ORIGINAL) {
			BT_LOGE("Set tx power fail, wrong conn tx power reset parameter!\r\n");
			return 0;
		}

	} else {
		BT_LOGE("Set tx power fail, wrong sub command or parameter number!\r\n");
		return 0;
	}

	if (rtk_bt_set_tx_power(&tx_power)) {
		BT_LOGE("Set tx power 0x%x fail!\r\n", tx_power.tx_gain);
	}
	return 0;
}

#if defined(CONFIG_BT_INIC) && CONFIG_BT_INIC
#include "hci_if_inic.h"

int atcmd_bt_remote_wakeup(int argc, char **argv)
{
	(void)argc;
	(void)argv;
	uint16_t opcode = 0xfe01;
	uint8_t status = 0;
	uint8_t evt[6] = {0x0e, 0x04, 0x03, 0x00, 0x00, 0x00};
	evt[3] = opcode & 0xFF;
	evt[4] = (opcode >> 8) & 0xFF;
	evt[5] = status;

	bt_inic_send_to_host(0x04, evt, sizeof(evt));
	BT_LOGA("Remote wakeup test");
	return 0;
}
#endif