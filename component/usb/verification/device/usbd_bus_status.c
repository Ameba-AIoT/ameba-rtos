/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */
#include <platform_autoconf.h>
#include "usbd_hal.h"
#include "usbd.h"
#include "os_wrapper.h"

static const char *const TAG = "BUS";


static void usbd_bus_check_usage(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Invalid arguments, usage:\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd bus init\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd bus deinit\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, " usbd bus dump\n");
}

int cmd_usbd_bus_cmd(u16 argc, u8 *argv[])
{
	const char *cmd;
	u32 bus_status = 0;
	int ret = HAL_OK;

	if (argc < 2) {
		usbd_bus_check_usage();
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[1];

	if (_stricmp(cmd, "init") == 0) {

		usb_chip_init(0);
		usb_hal_disable_global_interrupt();
		usb_hal_core_init(0);
		usb_hal_set_otg_mode((usb_otg_mode_t)0);

		usb_hal_device_disconnect(0);

		RTK_LOGS(TAG, RTK_LOG_INFO, " usbd bus init done\n");
	} else if (_stricmp(cmd, "deinit") == 0) {
		usb_chip_deinit();
		RTK_LOGS(TAG, RTK_LOG_INFO, " usbd bus deinit done\n");
	} else if (_stricmp(cmd, "dump") == 0) {
		usbd_get_bus_status(&bus_status);
		RTK_LOGS(TAG, RTK_LOG_INFO, " usbd bus state %d\n", bus_status);
	} else {
		usbd_bus_check_usage();
		ret = HAL_ERR_PARA;
	}

	return ret;
}

