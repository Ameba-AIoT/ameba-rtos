/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usbd_cmd.h"
#include "usb_hal.h"
#include "usbd.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "CMD";
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u32 usbd_test(u16 argc, u8 *argv[]);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t usbd_cfg = {
#ifdef CONFIG_USB_FS
	.speed = USB_SPEED_FULL,
#else
	.speed = USB_SPEED_HIGH,
#endif
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
	.intr_use_ptx_fifo = 0U,
	.nptx_max_epmis_cnt = 100U,
};

/* Public variables ---------------------------------------------------------*/

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbd_test_cmd_table[] = {
#ifdef CONFIG_USB_FS
	{
		(const u8 *)"USBD", 3, usbd_test, (const u8 *)"\tUSB device test cmd:\n"
		"\t\t usbd init\n"
		"\t\t usbd deinit\n"
		"\t\t usbd regdump\n"
		"\t\t usbd wakehost\n"
		"\t\t usbd verify subcmd\n"
	}
#else
	{
		(const u8 *)"USBD", 3, usbd_test, (const u8 *)"\tUSB device test cmd:\n"
		"\t\t usbd init\n"
		"\t\t usbd deinit\n"
		"\t\t usbd regdump\n"
		"\t\t usbd wakehost\n"
		"\t\t usbd phydw <addr_hex>\n"
		"\t\t usbd phyew <addr_hex> <value_hex>\n"
		"\t\t usbd verify subcmd\n"
	}
#endif
};

/* Private functions ---------------------------------------------------------*/

static u32 usbd_test(u16 argc, u8 *argv[])
{
	int status = HAL_OK;
	const char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid USB argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[0];

	if (_stricmp(cmd, "init") == 0) {
		status = usbd_init(&usbd_cfg);
		if (status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USBD init\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBD: %d\n", status);
		}
	} else if (_stricmp(cmd, "deinit") == 0) {
		status = usbd_deinit();
		if (status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USBD deinit\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to deinit USBD: %d\n", status);
		}
	} else if (_stricmp(cmd, "wakehost") == 0) {
		usbd_wake_host();
	} else if (_stricmp(cmd, "regdump") == 0) {
		usb_hal_dump_registers();
#ifndef CONFIG_USB_FS
	} else if (_stricmp(cmd, "phydw") == 0) {
		status = cmd_usbd_cts_phydw(argc, argv);
	} else if (_stricmp(cmd, "phyew") == 0) {
		status = cmd_usbd_cts_phyew(argc, argv);
#endif
	} else if (_stricmp(cmd, "verify") == 0) {
		status = cmd_usbd_verify_test_entry(argc, argv);
	} else if (_stricmp(cmd, "acm") == 0) {
		status = cmd_usbd_cdc_acm(argc, argv);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input USB cmd err %s\n", usbd_test_cmd_table[0].msg);
	}

	return status;
}

/* Exported functions --------------------------------------------------------*/

