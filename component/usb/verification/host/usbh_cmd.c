/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usbh_cmd.h"
#include "usb_regs.h"
#include "usb_hal.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "VFY";

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u32 usbh_test(u16 argc, u8 *argv[]);
static int usbh_usr_process(usb_host_t *host, u8 id);

/* Private variables ---------------------------------------------------------*/

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = FALSE,
	.main_task_priority = 3U,
	.isr_task_priority = 4U,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_usr_process
};

/* Public variables ---------------------------------------------------------*/

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_test_cmd_table[] = {
	{
		(const u8 *)"USBH", 3, usbh_test, (const u8 *)"\tUSB host test cmd:\n"
		"\t\t usbh init\n"
		"\t\t usbh deinit\n"
		"\t\t usbh phydw <address, hex>\n"
		"\t\t usbh phyew <address, hex> <value, hex>\n"
		"\t\t usbh emc\n"
		"\t\t usbh tmod <value>\n"
		"\t\t usbh resume\n"
		"\t\t usbh suspend\n"
		"\t\t usbh xfer <subcmd>\n"
		"\t\t usbh verify <subcmd>\n"
	}
};

/* Private functions ---------------------------------------------------------*/

static int usbh_usr_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB disconnected\n");
		break;

	case USBH_MSG_CONNECTED:
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB connected\n");
		break;

	default:
		break;
	}

	return HAL_OK;
}

static u32 usbh_test(u16 argc, u8 *argv[])
{
	int ret;
	const char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[0];

	if (_stricmp(cmd, "init") == 0) {
		ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
		if (ret == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USBH initialized\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBH: %d\n", ret);
		}
	} else if (_stricmp(cmd, "deinit") == 0) {
		ret = usbh_deinit();
		if (ret == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USBH deinitialized\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to deinit USBH: %d\n", ret);
		}
	} else if (_stricmp(cmd, "regdump") == 0) {
		usb_hal_dump_registers();
	} else if (_stricmp(cmd, "phydw") == 0) {
		ret = cmd_usbh_cts_phydw(argc, argv);
	} else if (_stricmp(cmd, "phyew") == 0) {
		ret = cmd_usbh_cts_phyew(argc, argv);
	} else if (_stricmp(cmd, "emc") == 0) {
		ret = cmd_usbh_emc_test();
	} else if (_stricmp(cmd, "tmod") == 0) {
		ret = cmd_usbh_tmod_test(argc, argv);
	} else if (_stricmp(cmd, "resume") == 0) {
		ret = cmd_usbh_resume_test();
	} else if (_stricmp(cmd, "suspend") == 0) {
		ret = cmd_usbh_suspend_test();
	} else if (_stricmp(cmd, "xfer") == 0) {
		ret = cmd_usbh_xfer_test(argc, argv);
	} else if (_stricmp(cmd, "verify") == 0) {
		ret = cmd_usbh_verify_test_entry(argc, argv);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd is err %s\n", usbh_test_cmd_table[0].msg);
	}

	return (u32)ret;
}

/* Exported functions --------------------------------------------------------*/

