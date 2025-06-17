/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include "ameba_soc.h"
#include "usb_regs.h"
#include "usb_hal.h"

/* Private defines -----------------------------------------------------------*/

#define USB_TEST_LOOPS            5
#define USB_TEST_ERROR_THRESHOLD  2

#define USB_REG_GPVNDCTL          (USB_REG_BASE + 0x34)
#define USB_REG_UTMI_STATUS_OUT   (USB_ADDON_REG_BASE + 0x1C)
#define USB_REG_SLBBIST_CTRL      (USB_ADDON_REG_BASE + 0x20)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "USB";

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int cmd_usb_loopback_test(u16 argc, u8 *argv[])
{
	u32 loop;
	u32 reg;
	int ret;
	int error_count = 0;
	u8 mode = 0;

	if (argc > 1) {
		mode = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		if (mode > 1) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid USB mode: %s\n", argv[1]);
			return HAL_ERR_PARA;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Loopback test...\n");

	ret = usb_hal_driver.init(0);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init FAIL: %d\n", ret);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Loopback test FAIL\n");
		return ret;
	}

	/* PHY SLB option */
	HAL_WRITE32(USB_REG_UTMI_STATUS_OUT, 0, 0x3);

	HAL_WRITE32(USB_REG_GPVNDCTL, 0, 0x0a300F00);
	HAL_WRITE32(USB_REG_GPVNDCTL, 0, 0x0a300000);

	DelayMs(100);

	/* SLB pattern */
	for (loop = 0; loop < USB_TEST_LOOPS; loop ++) {
		HAL_WRITE32(USB_REG_SLBBIST_CTRL, 0, 0xC0 + loop);
		HAL_WRITE32(USB_REG_SLBBIST_CTRL, 0, 0x80 + loop);

		DelayUs(1000);

		reg = HAL_READ32(USB_REG_SLBBIST_CTRL, 0);
		if ((reg & (BIT4 | BIT5)) == 0x10) {
			error_count = 0;
			RTK_LOGS(TAG, RTK_LOG_INFO, "Loop %d# PASS\n", loop);
			continue;
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Loop %d# FAIL\n", loop);
			error_count++;
			if (error_count >= USB_TEST_ERROR_THRESHOLD) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Loopback test FAIL: [0x%08x]=0x%08x\n", USB_REG_SLBBIST_CTRL, reg);
				ret = HAL_ERR_HW;
				break;
			} else {
				continue;
			}
		}
	}

	HAL_WRITE32(USB_REG_SLBBIST_CTRL, 0, 0);

	usb_hal_driver.deinit();

	if (ret == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Loopback test PASS\n");
	}

	return ret;
}
