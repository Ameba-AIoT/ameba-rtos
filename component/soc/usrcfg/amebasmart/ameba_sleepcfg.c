/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/*wakeup attribute can be set to WAKEUP_NULL/WAKEUP_LP/WAKEUP_NP/WAKEUP_AP*/
const struct WakeEvent_TypeDef sleep_wevent_config[] = {
//  Module									wakeup
	{WAKE_SRC_BT_WAKE_HOST,					WAKEUP_NULL},
	{WAKE_SRC_AON_WAKEPIN,					WAKEUP_LP},
	{WAKE_SRC_IPC_AP,						WAKEUP_AP},
	{WAKE_SRC_IPC_NP,						WAKEUP_NP},
	{WAKE_SRC_VADBT_OR_VADPC,				WAKEUP_NULL},
	{WAKE_SRC_PWR_DOWN,						WAKEUP_LP},
	{WAKE_SRC_GPIOB,						WAKEUP_AP},		/* Do not modify, uart rx pin is used as gpio wake source*/
	{WAKE_SRC_Timer6,						WAKEUP_NP},
	{WAKE_SRC_WIFI_FTSR_MAILBOX,			WAKEUP_LP},
	{WAKE_SRC_WIFI_FISR_FESR,				WAKEUP_LP},

	{0xFFFFFFFF,							WAKEUP_NULL},	/* Table end */
};

struct PSCFG_TypeDef ps_config = {
	.km0_audio_vad_on = FALSE,
	.keep_osc4m_on = FALSE,		   /* keep OSC4M off or on during sleep */
	.xtal_mode_in_sleep = XTAL_OFF,/* set xtal mode during sleep mode */
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