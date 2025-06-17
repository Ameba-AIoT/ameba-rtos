/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usb_regs.h"
#include "usb_hal.h"
#include "usb_cmd.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "USB";
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u32 usb_cmd(u16 argc, u8 *argv[]);

/* Private variables ---------------------------------------------------------*/

/* Public variables ---------------------------------------------------------*/

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usb_cmd_table[] = {
#ifdef CONFIG_SUPPORT_USB_NO_PHY
	{
		(const u8 *)"USB", 3, usb_cmd, (const u8 *)"\tUSB cmd:\n"
		"\t\t usb init\n"
		"\t\t usb deinit\n"
		"\t\t usb status\n"
	}
#else
	{
		(const u8 *)"USB", 3, usb_cmd, (const u8 *)"\tUSB cmd:\n"
		"\t\t usb init\n"
		"\t\t usb deinit\n"
		"\t\t usb status\n"
		"\t\t usb loopback [<mode>]\n"
	}
#endif
};

/* Private functions ---------------------------------------------------------*/

static u32 usb_cmd(u16 argc, u8 *argv[])
{
	int ret = HAL_OK;
	u32 bus_status;
	const char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid USB argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[0];

	if (_stricmp(cmd, "init") == 0) {
		ret = usb_hal_driver.init(0);
		if (ret == HAL_OK) {
			usb_hal_disable_global_interrupt();
			ret = usb_hal_core_init(0);
			if (ret == HAL_OK) {
				usb_hal_set_otg_mode((usb_otg_mode_t)0);
				usb_hal_device_disconnect(0);
				RTK_LOGS(TAG, RTK_LOG_INFO, "USB init done\n");
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "USB core init FAIL: %d\n", ret);
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "USB init FAIL: %d\n", ret);
		}
	} else if (_stricmp(cmd, "deinit") == 0) {
		usb_hal_driver.deinit();
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB deinit done\n");
	} else if (_stricmp(cmd, "status") == 0) {
		ret = usb_hal_get_device_bus_status(&bus_status);
		if (ret == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB bus status: 0x%x\n", bus_status);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Get bus status FAIL: %d\n", ret);
		}
#ifndef CONFIG_SUPPORT_USB_NO_PHY
	} else if (_stricmp(cmd, "loopback") == 0) {
		ret = cmd_usb_loopback_test(argc, argv);
#endif
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid USB cmd: %s\n", cmd);
		ret = HAL_ERR_PARA;
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/

