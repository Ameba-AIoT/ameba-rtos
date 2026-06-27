/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/* wakeup attribute can be set to WAKEUP_NULL/WAKEUP_NP/WAKEUP_AP */
const struct WakeEvent_TypeDef sleep_wevent_config[] = {
//  	Module								Wakeup
	{WAKE_SRC_WIFI_FISR_FESR_IRQ,			WAKEUP_NP},		/* For WiFi wakeup, do not change it*/
	{WAKE_SRC_AP_WAKE_IRQ,					WAKEUP_NP},
	{WAKE_SRC_GPIOA,						WAKEUP_AP},
	{WAKE_SRC_BT_CTRL_HIGH,					WAKEUP_NULL},
	{WAKE_SRC_BT_CTRL_LOW,					WAKEUP_NULL},
	{0xFFFFFFFF,							WAKEUP_NULL},	/* Table end */
};

struct PSCFG_TypeDef ps_config = {
	.keep_osc2m_on = FALSE,					/* keep OSC2M off or on during sleep */
	.xtal_mode_in_sleep = XTAL_OFF,				/* set xtal mode during sleep mode, see enum xtal_mode_sleep for detail */
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