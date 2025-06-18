/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include "ameba_soc.h"
#include "usb_regs.h"
#include "usb_hal.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "USBH";

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int cmd_usbh_tmod_test(u16 argc, u8 *argv[])
{
	u32 mode;
	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid arguments, usage:\n");
		RTK_LOGS(TAG, RTK_LOG_ERROR, "  usbh tmod <value>\n");
		return HAL_ERR_PARA;
	}

	mode = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
	RTK_LOGS(TAG, RTK_LOG_INFO, "cmd_usbh_tmod_test mode=%lu\n", mode);

	return usbh_port_test_ctrl(mode);
}

int cmd_usbh_resume_test(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter Resume test\n");

	return usbh_enter_suspend(0);
}

int cmd_usbh_suspend_test(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter Suspend test \n");

	return usbh_enter_suspend(1);
}
