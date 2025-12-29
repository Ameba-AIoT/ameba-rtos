/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/*wakeup attribute can be set to WAKEUP_NULL/WAKEUP_NP/WAKEUP_AP/WAKEUP_DSP*/
struct WakeEvent_TypeDef sleep_wevent_config[] = {
//  Module									wakeup
	{WAKE_SRC_VAD,							WAKEUP_NULL},
	{WAKE_SRC_AON_WAKEPIN,					WAKEUP_NULL},
	{WAKE_SRC_AON_TIM,						WAKEUP_NULL},
	{WAKE_SRC_PWR_DOWN,						WAKEUP_NULL},
	{WAKE_SRC_BOR,							WAKEUP_NULL},
	{WAKE_SRC_ADC_COMP,						WAKEUP_NULL},
	{WAKE_SRC_ADC,							WAKEUP_NULL},
	{WAKE_SRC_AON_RTC,						WAKEUP_NULL},
	{WAKE_SRC_SPI1,							WAKEUP_NULL},
	{WAKE_SRC_SPI0,							WAKEUP_NULL},
	{WAKE_SRC_CTOUCH,						WAKEUP_NULL},
	{WAKE_SRC_GPIOB,						WAKEUP_NULL},
	{WAKE_SRC_GPIOA,						WAKEUP_AP},/* Do not modify, uart rx pin is used as gpio wake source*/
	{WAKE_SRC_UART_LOG,						WAKEUP_AP},
	{WAKE_SRC_UART3,						WAKEUP_NULL},
	{WAKE_SRC_UART2,						WAKEUP_NULL},
	{WAKE_SRC_UART1,						WAKEUP_NULL},
	{WAKE_SRC_UART0,						WAKEUP_NULL},
	{WAKE_SRC_Timer7,						WAKEUP_NULL},
	{WAKE_SRC_Timer6,						WAKEUP_NULL},
	{WAKE_SRC_Timer5,						WAKEUP_NULL},
	{WAKE_SRC_Timer4,						WAKEUP_NULL},
	{WAKE_SRC_Timer3,						WAKEUP_NULL},
	{WAKE_SRC_Timer2,						WAKEUP_NULL},
	{WAKE_SRC_Timer1,						WAKEUP_NULL},
	{WAKE_SRC_Timer0,						WAKEUP_NULL},
	{WAKE_SRC_WDG0,							WAKEUP_NULL},
	{WAKE_SRC_BT_WAKE_HOST,					WAKEUP_NULL},
	{WAKE_SRC_DSP_WAKE,						WAKEUP_NULL},
	{WAKE_SRC_AP_WAKE,						WAKEUP_NULL},
	{WAKE_SRC_WIFI_FTSR_MAILBOX,			WAKEUP_NP},
	{WAKE_SRC_WIFI_FISR_FESR,				WAKEUP_NP},

	{0xFFFFFFFF,							WAKEUP_NULL},	/* Table end */
};

struct PSCFG_TypeDef ps_config = {
	.keep_osc4m_on = FALSE,			/* keep OSC4M on or off for sleep */
	.xtal_mode_in_sleep = XTAL_OFF,	/* set xtal mode during sleep mode, see enum xtal_mode_sleep for detail */
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