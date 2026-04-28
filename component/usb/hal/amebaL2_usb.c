/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"
#include "usb_hal.h"

/* Private defines -----------------------------------------------------------*/

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

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB chip specific initialization
  * @param  void
  * @retval Status
  */
static int usb_chip_init(u8 mode)
{
	u32 reg;
	PLL_TypeDef *pll = (PLL_TypeDef *)PLL_REG_BASE;

	/* USB pinmux config */
	Pinmux_Config(USB_PIN_DP, PINMUX_FUNCTION_USB);
	Pinmux_Config(USB_PIN_DM, PINMUX_FUNCTION_USB);

	/* USB pad config */
	if (mode) {
		/* Host mode */
		PAD_PullCtrl(USB_PIN_DP, GPIO_PuPd_NOPULL);
		PAD_PullCtrl(USB_PIN_DM, GPIO_PuPd_DOWN);
	} else {
		/* Device mode */
		PAD_PullCtrl(USB_PIN_DP, GPIO_PuPd_NOPULL);
		PAD_PullCtrl(USB_PIN_DM, GPIO_PuPd_NOPULL);
	}

	reg = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_FEN_USB_PLL);
	reg |= LSYS_BIT_FEN_USB_PLL_USB;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_FEN_USB_PLL, reg);

	pll->PLL_UPLL_CTRL0 |= (PLL_BIT_FS_COMP_EN | PLL_BIT_PWC_UAHV_EN | PLL_BIT_UA_LV2HV_EN);

	RCC_PeriphClockCmd(APBPeriph_USB, APBPeriph_USB_CLOCK, ENABLE);

	return HAL_OK;
}

/**
  * @brief  USB chip specific deinitialization
  * @param  void
  * @retval Status
  */
static int usb_chip_deinit(void)
{
	PLL_TypeDef *pll = (PLL_TypeDef *)PLL_REG_BASE;

	RCC_PeriphClockCmd(APBPeriph_USB, APBPeriph_USB_CLOCK, DISABLE);

	pll->PLL_UPLL_CTRL0 &= ~(PLL_BIT_FS_COMP_EN | PLL_BIT_PWC_UAHV_EN | PLL_BIT_UA_LV2HV_EN);

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
	InterruptEn(USB_IRQ, priority);
}

/**
  * @brief  Disable USB interrupt
  * @retval void
  */
static void usb_chip_disable_interrupt(void)
{
	InterruptDis(USB_IRQ);
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
		InterruptRegister((IRQ_FUN)handler, USB_IRQ, NULL, priority);
	}
}

/**
  * @brief  Unregister USB IRQ handler
  * @retval void
  */
static void usb_chip_unregister_irq_handler(void)
{
	InterruptUnRegister(USB_IRQ);
}

/* Exported functions --------------------------------------------------------*/
