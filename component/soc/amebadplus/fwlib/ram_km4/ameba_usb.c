/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"

/* Private defines -----------------------------------------------------------*/

#define USB_UTMIFS_CLK                 48000000U
#define USB_UTMIFS_CLK_MAX_ERROR       120000U // ±0.25% as per spec

#define REG_LSYS_USB_CTRL              0x023C

#define LSYS_USB_CTRL_BIT_FS_COMP_EN   ((u32)0x1 << 0)
#define LSYS_USB_CTRL_BIT_PWC_UAHV_EN  ((u32)0x1 << 1)
#define LSYS_USB_CTRL_BIT_UA_LV2HV_EN  ((u32)0x1 << 2)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

USB_DATA_SECTION
static const char *const TAG = "USB";

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB chip specific initialization
  * @param  void
  * @retval HAL status
  */
USB_TEXT_SECTION
int usb_chip_init(void)
{
	int div = 0;
	u32 reg = 0;
	u32 pll_clk;
	u32 usb_clk;

	PAD_PullCtrl(_PA_28, GPIO_PuPd_NOPULL);
	PAD_PullCtrl(_PA_29, GPIO_PuPd_NOPULL);

	/* rename pull resistor, whose value keep unchanged with before */
	PAD_ResistorCtrl(_PA_29, PAD_Resistor_SMALL);
	Pinmux_Config(_PA_29, PINMUX_FUNCTION_USB);
	Pinmux_Config(_PA_28, PINMUX_FUNCTION_USB);

	reg = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0);

	pll_clk = PLL_ClkGet();

	div = CLKDIV_ROUND_UP(pll_clk, USB_UTMIFS_CLK) - 1;
	if (div > 0xF) {
		div = 0xF;
	} else if (div < 0) {
		div = 0;
	}

	usb_clk = pll_clk / (div + 1);
	if ((usb_clk < USB_UTMIFS_CLK - USB_UTMIFS_CLK_MAX_ERROR) || (usb_clk > USB_UTMIFS_CLK + USB_UTMIFS_CLK_MAX_ERROR)) {
		RTK_LOGS(TAG, "[USB] Clock %dHz out of range: pll_clk=%dHz div=%d\n", usb_clk, pll_clk, div + 1);
	}

	reg &= ~LSYS_MASK_CKD_UTMIFS;
	reg |= LSYS_CKD_UTMIFS(div);

	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0, reg);

	reg = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_USB_CTRL);
	reg |= (LSYS_USB_CTRL_BIT_FS_COMP_EN | LSYS_USB_CTRL_BIT_PWC_UAHV_EN | LSYS_USB_CTRL_BIT_UA_LV2HV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_USB_CTRL, reg);

	RCC_PeriphClockCmd(APBPeriph_USB, APBPeriph_USB_CLOCK, ENABLE);

	return HAL_OK;
}

/**
  * @brief  USB chip specific deinitialization
  * @param  void
  * @retval HAL status
  */
USB_TEXT_SECTION
int usb_chip_deinit(void)
{
	u32 reg = 0;

	RCC_PeriphClockCmd(APBPeriph_USB, APBPeriph_USB_CLOCK, DISABLE);

	reg = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_USB_CTRL);
	reg &= ~(LSYS_USB_CTRL_BIT_FS_COMP_EN | LSYS_USB_CTRL_BIT_PWC_UAHV_EN | LSYS_USB_CTRL_BIT_UA_LV2HV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_USB_CTRL, reg);

	return HAL_OK;
}

