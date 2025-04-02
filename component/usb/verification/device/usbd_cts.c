/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include "ameba_soc.h"
#include "usb_regs.h"
#include "usb_hal.h"
#include "usbd.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "CTS";
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int cmd_usbd_cts_phydw(u16 argc, u8 *argv[])
{
	u8 addr;
	u8 val;
	int ret;

	if (argc != 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid arguments, usage:\n");
		RTK_LOGS(TAG, RTK_LOG_ERROR, " usbd phydw <address_hex>\n");
		return HAL_ERR_PARA;
	}

	addr = (u8)(_strtoul((const char *)(argv[1]), (char **)NULL, 16) & 0xFF);
	ret = usb_hal_read_phy_register(addr, &val);
	if (ret == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "PHY R 0x%02x: 0x%02x\n", addr, val);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "PHY R fail: %d\n", ret);
	}

	return ret;
}

int cmd_usbd_cts_phyew(u16 argc, u8 *argv[])
{
	u8 addr;
	u8 val;
	int ret;

	if (argc != 3) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid arguments, usage:\n");
		RTK_LOGS(TAG, RTK_LOG_ERROR, " usbd phyew <address_hex> <value_hex>\n");
		return HAL_ERR_PARA;
	}

	addr = (u8)(_strtoul((const char *)(argv[1]), (char **)NULL, 16) & 0xFF);
	val = (u8)(_strtoul((const char *)(argv[2]), (char **)NULL, 16) & 0xFF);
	ret = usb_hal_write_phy_register(addr, val);
	if (ret == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "PHY W 0x%02x: 0x%02x\n", addr, val);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "PHY W fail: %d\n", ret);
	}

	return ret;
}

