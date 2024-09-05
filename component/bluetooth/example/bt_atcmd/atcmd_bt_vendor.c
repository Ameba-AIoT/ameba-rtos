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

	BT_LOGA("Set tx power gain 0x%lx OK!\r\n", index);

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

int atcmd_bt_sleep_mode(int argc, char *argv[])
{
	(void)argc;

	unsigned int mode = (unsigned int)str_to_int(argv[0]);

	BT_LOGA("Set BT sleep mode to 0x%x.\r\n", mode);

	rtk_bt_sleep_mode(mode);

	return 0;
}

/*int atcmd_bt_ant(int argc, char *argv[])
{
    (void)argc;

    uint8_t ant = atoi(argv[0]);

    if (ant > 1) {
        BT_LOGE("Invalid BT ant!\r\n");
        return 0;
    }

    BT_LOGA("Set BT ant to %s.\r\n", ant ? "ANT_S1" : "ANT_S0");

    rtk_bt_set_bt_antenna(ant);

    return 0;
}*/

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

	if (!rtk_bt_set_tx_power(&tx_power)) {
		BT_LOGA("Set tx power 0x%x OK!\r\n", tx_power.tx_gain);
	}
	return 0;
}

#if defined(RTK_BLE_TX_SOF_EOF_INDICATION) && RTK_BLE_TX_SOF_EOF_INDICATION
void rtk_bt_le_tx_sof_eof_callback(uint8_t flag)
{
	if (flag == RTK_BT_LE_TX_SOF) {
		BT_LOGA("%s SOF,time=%d\r\n", __func__, (int)osif_sys_time_get());
	} else if (flag == RTK_BT_LE_TX_EOF) {
		BT_LOGA("%s EOF,time=%d\r\n", __func__, (int)osif_sys_time_get());
	} else {
		BT_LOGE("%s ERROR\r\n", __func__);
	}
}

int atcmd_bt_sof_eof_ind(int argc, char *argv[])
{
	uint16_t conn_handle = 0;
	uint8_t enable = 0;

	if (argc != 2) {
		BT_LOGE("atcmd_bt_sof_eof_ind fail, wrong parameter number!\r\n");
		return 0;
	}

	conn_handle = (uint16_t)str_to_int(argv[0]);
	enable = (uint8_t)str_to_int(argv[1]);

	BT_LOGA("Set conn_handle(0x%x) sof and eof to %x.\r\n", conn_handle, enable);

	rtk_bt_le_sof_eof_ind(conn_handle, enable, (void *)rtk_bt_le_tx_sof_eof_callback);

	return 0;
}
#else
int atcmd_bt_sof_eof_ind(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	BT_LOGE("BT SOF and EOF indication is not supported on this platform!\r\n");
	return 0;
}
#endif