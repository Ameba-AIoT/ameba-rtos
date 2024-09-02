/**
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_msc.h"
#include "os_wrapper.h"
#include "ameba_sd.h"

/* Private defines -----------------------------------------------------------*/
static const char *TAG = "MSCD";

// USB speed
#ifdef CONFIG_USB_FS
#define CONFIG_USBD_MSC_SPEED						USB_SPEED_FULL
#else
#define CONFIG_USBD_MSC_SPEED						USB_SPEED_HIGH
#endif

#define CONFIG_USBD_MSC_ISR_THREAD_PRIORITY			7U

/* Private types -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static void msc_cb_status_changed(u8 status);
static u32 usbd_msc_cmd_test(u16 argc, u8 *argv[]);

/* Private variables ---------------------------------------------------------*/
static usbd_config_t msc_cmd_cfg = {
	.speed = CONFIG_USBD_MSC_SPEED,
	.dma_enable = 1U,
	.isr_priority = CONFIG_USBD_MSC_ISR_THREAD_PRIORITY,
	.nptx_max_err_cnt = {0U, 2000U, },
};

static usbd_msc_cb_t msc_cmd_cb = {
	.status_changed = msc_cb_status_changed
};

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbd_msc_cmd_table[] = {
	{
		(const u8 *)"usbd_msc", 8, usbd_msc_cmd_test, (const u8 *)"\tUSBD MSC cmd:\n"
		"\t\t usbd_msc init\n"
		"\t\t usbd_msc deinit\n"
	}
};

/* Private functions ---------------------------------------------------------*/
static void msc_cb_status_changed(u8 status)
{
	RTK_LOGS(TAG, "USBD status: %d\n", status);
}

static u32 usbd_msc_cmd_test(u16 argc, u8 *argv[])
{
	int status = HAL_OK;
	const char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, "Invalid USB argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[0];

	if (_stricmp(cmd, "init") == 0) {
		status = usbd_msc_disk_init();
		if (status != HAL_OK) {
			RTK_LOGS(TAG, "Fail to init disk: %d\n", status);
			return status;
		}
		status = usbd_init(&msc_cmd_cfg);
		if (status != HAL_OK) {
			usbd_msc_disk_deinit();
			RTK_LOGS(TAG, "Fail to init USBD: %d\n", status);
			return status;
		}

		status = usbd_msc_init(&msc_cmd_cb);
		if (status != HAL_OK) {
			usbd_deinit();
			usbd_msc_disk_deinit();
			RTK_LOGS(TAG, "Fail to init MSC\n");
		}
	} else if (_stricmp(cmd, "deinit") == 0) {
		usbd_msc_deinit();
		status = usbd_deinit();
		if (status != HAL_OK) {
			RTK_LOGS(TAG, "Fail to deinit USBD\n");
		}
		status = usbd_msc_disk_deinit();
		if (status != HAL_OK) {
			RTK_LOGS(TAG, "Fail to deinit disk\n");
		}
	} else {
		RTK_LOGS(TAG, "Input cmd err\n");
		RTK_LOGS(TAG, "%s\n", usbd_msc_cmd_table[0].msg);
	}

	return status;
}
