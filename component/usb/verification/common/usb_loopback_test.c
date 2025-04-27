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

#define USB_REG_GPVNDCTL          0x34
#define USB_REG_UTMI_STATUS_OUT   0x3001C
#define USB_REG_SLBBIST_CTRL      0x30020

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "USB";

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

int cmd_usb_loopback_test(u16 argc, u8 *argv[])
{
	u32 loops = 4;
	u32 count;
	u32 reg;
	int ret;
	u8 mode = 0;

	if (argc > 1) {
		mode = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		if (mode > 1) {
			RTK_LOGE(TAG, "Invalid USB mode: %s\n", argv[1]);
			return HAL_ERR_PARA;
		}
	}

	RTK_LOGI(TAG, "Loopback test...\n");

	ret = usb_chip_init(0);
	if (ret != HAL_OK) {
		RTK_LOGE(TAG, "Init FAIL: %d\n", ret);
		RTK_LOGE(TAG, "Loopback test FAIL\n");
		return ret;
	}

	/* PHY SLB option */
	HAL_WRITE32(USB_REG_BASE, USB_REG_UTMI_STATUS_OUT, 0x3);

	HAL_WRITE32(USB_REG_BASE, USB_REG_GPVNDCTL, 0x0a300F00);
	HAL_WRITE32(USB_REG_BASE, USB_REG_GPVNDCTL, 0x0a300000);

	DelayMs(100);

	/* SLB pattern */
	for (count = 0; count < loops; count ++) {
		HAL_WRITE32(USB_REG_BASE, USB_REG_SLBBIST_CTRL, 0xC0 + count);
		HAL_WRITE32(USB_REG_BASE, USB_REG_SLBBIST_CTRL, 0x80 + count);

		DelayUs(1000);

		reg = HAL_READ32(USB_REG_BASE, USB_REG_SLBBIST_CTRL);
		if ((reg & (BIT4 | BIT5)) == 0x10) {
			continue;
		} else {
			RTK_LOGE(TAG, "Loopback test FAIL: [0x%08x]=0x%08x\n", USB_REG_BASE + USB_REG_SLBBIST_CTRL, reg);
			ret = HAL_ERR_HW;
			break;
		}
	}

	HAL_WRITE32(USB_REG_BASE, USB_REG_SLBBIST_CTRL, 0);

	usb_chip_deinit();

	if (ret == HAL_OK) {
		RTK_LOGI(TAG, "Loopback test PASS\n");
	}

	return ret;
}
