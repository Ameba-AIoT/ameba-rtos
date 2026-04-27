/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/* wakeup attribute can be set to WAKEUP_NULL/WAKEUP_NP/WAKEUP_AP */
const struct WakeEvent_TypeDef sleep_wevent_config[] = {
//  	Module								Wakeup
#ifdef CONFIG_WHC_INTF_SDIO
	{WAKE_SRC_SDIO_WIFI,					WAKEUP_NP},
#else
	{WAKE_SRC_SDIO_WIFI,					WAKEUP_NULL},
#endif
	{WAKE_SRC_BT_WAKE_HOST,					WAKEUP_NULL},
	{WAKE_SRC_BT_SCB,						WAKEUP_NULL},
	{WAKE_SRC_GPIOA,						WAKEUP_AP},
	{WAKE_SRC_pmc_timer1,					WAKEUP_NP},		/* internal usage, do not change it*/
	{WAKE_SRC_pmc_timer0,					WAKEUP_AP},		/* internal usage, do not change it*/
	{WAKE_SRC_AP_WAKE_IRQ,					WAKEUP_NP},
	{WAKE_SRC_WIFI_FTSR_MAILBOX_IRQ,		WAKEUP_NP},		/* For WiFi wakeup, do not change it*/
	{WAKE_SRC_WIFI_FISR_FESR_IRQ,			WAKEUP_NP},		/* For WiFi wakeup, do not change it*/

	{0xFFFFFFFF,							WAKEUP_NULL},	/* Table end */
};

struct PSCFG_TypeDef ps_config = {
	.keep_osc4m_on = FALSE,						/* keep OSC4M off or on during sleep */
	.xtal_mode_in_sleep = XTAL_OFF,				/* set xtal mode during sleep mode*/
	.sleep_to_08V = FALSE,						/* default sleep to 0.7V, set this option to TRUE will sleep to 0.8V */
};

UARTCFG_TypeDef uart_config[4] = {
	/* UART0 */
	{
		.LOW_POWER_RX_ENABLE = DISABLE, /*Enable low power RX*/
	},
	/* UART1 */
	{
		.LOW_POWER_RX_ENABLE = DISABLE,
	},
	/* UART2 */
	{
		.LOW_POWER_RX_ENABLE = DISABLE,
	},
	/* UART3_BT */
	{
		.LOW_POWER_RX_ENABLE = DISABLE,
	},
};
