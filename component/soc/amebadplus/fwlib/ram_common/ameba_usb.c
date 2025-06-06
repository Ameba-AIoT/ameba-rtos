/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"
#include "usb_hal.h"

/* Private defines -----------------------------------------------------------*/

#define USB_PIN_DM                     _PA_28
#define USB_PIN_DP                     _PA_29

#define USB_UTMIFS_CLK                 48000000U
#define USB_UTMIFS_CLK_MAX_ERROR       120000U // ±0.25% as per spec

#define REG_LSYS_USB_CTRL              0x023C

#define LSYS_USB_CTRL_BIT_FS_COMP_EN   ((u32)0x1 << 0)
#define LSYS_USB_CTRL_BIT_PWC_UAHV_EN  ((u32)0x1 << 1)
#define LSYS_USB_CTRL_BIT_UA_LV2HV_EN  ((u32)0x1 << 2)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usb_chip_init(u8 mode);
static int usb_chip_deinit(void);
static void usb_chip_enable_interrupt(u8 priority);
static void usb_chip_disable_interrupt(void);
static void usb_chip_register_irq_handler(void *handler, u8 priority);
static void usb_chip_unregister_irq_handler(void);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "USB";

/* Exported variables --------------------------------------------------------*/

usb_hal_driver_t usb_hal_driver = {
	.init = usb_chip_init,
	.deinit = usb_chip_deinit,
	.get_cal_data = NULL,
	.enable_interrupt = usb_chip_enable_interrupt,
	.disable_interrupt = usb_chip_disable_interrupt,
	.register_irq_handler = usb_chip_register_irq_handler,
	.unregister_irq_handler = usb_chip_unregister_irq_handler,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  USB chip specific initialization
  * @param  void
  * @retval HAL status
  */
static int usb_chip_init(u8 mode)
{
	UNUSED(mode);

	int div = 0;
	u32 reg = 0;
	u32 pll_clk;
	u32 usb_clk;

	PAD_PullCtrl(USB_PIN_DM, GPIO_PuPd_NOPULL);
	PAD_PullCtrl(USB_PIN_DP, GPIO_PuPd_NOPULL);

	/* rename pull resistor, whose value keep unchanged with before */
	PAD_ResistorCtrl(USB_PIN_DP, PAD_Resistor_SMALL);
	Pinmux_Config(USB_PIN_DP, PINMUX_FUNCTION_USB);
	Pinmux_Config(USB_PIN_DM, PINMUX_FUNCTION_USB);

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
		RTK_LOGS(TAG, RTK_LOG_INFO, "[USB] Clock %dHz out of range: pll_clk=%dHz div=%d\n", usb_clk, pll_clk, div + 1);
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
static int usb_chip_deinit(void)
{
	u32 reg = 0;

	RCC_PeriphClockCmd(APBPeriph_USB, APBPeriph_USB_CLOCK, DISABLE);

	reg = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_USB_CTRL);
	reg &= ~(LSYS_USB_CTRL_BIT_FS_COMP_EN | LSYS_USB_CTRL_BIT_PWC_UAHV_EN | LSYS_USB_CTRL_BIT_UA_LV2HV_EN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_USB_CTRL, reg);

	Pinmux_Config(USB_PIN_DP, PINMUX_FUNCTION_GPIO);
	Pinmux_Config(USB_PIN_DM, PINMUX_FUNCTION_GPIO);

	return HAL_OK;
}

/**
  * @brief  Enable USB interrupt
  * @param  priority: IRQ priority
  * @retval void
  */
static void usb_chip_enable_interrupt(u8 priority)
{
	UNUSED(priority);
	InterruptEn(USB_INT_IRQ, priority);
}

/**
  * @brief  Disable USB interrupt
  * @retval void
  */
static void usb_chip_disable_interrupt(void)
{
	InterruptDis(USB_INT_IRQ);
}

/**
  * @brief  Register USB IRQ handler
  * @param  handler: IRQ handler
  * @param  priority: IRQ priority
  * @retval void
  */
static void usb_chip_register_irq_handler(void *handler, u8 priority)
{
	if (handler != NULL) {
		InterruptRegister((IRQ_FUN)handler, USB_INT_IRQ, NULL, priority);
	}
}

/**
  * @brief  Unregister USB IRQ handler
  * @retval void
  */
static void usb_chip_unregister_irq_handler(void)
{
	InterruptUnRegister(USB_INT_IRQ);
}

/* Exported functions --------------------------------------------------------*/
