/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

/* Wakeup entry can be set to WAKEUP_NULL/WAKEUP_KM4/WAKEUP_KM0 */
WakeEvent_TypeDef sleep_wevent_config[] = {
//  	Module								Wakeup
	{WAKE_SRC_SDIO,							WAKEUP_NULL},
	{WAKE_SRC_AON_WAKEPIN,					WAKEUP_NULL},
	{WAKE_SRC_AON_TIM,						WAKEUP_NULL},
	{WAKE_SRC_Keyscan,						WAKEUP_NULL},
	{WAKE_SRC_PWR_DOWN,						WAKEUP_NULL},
	{WAKE_SRC_BOR,							WAKEUP_NULL},
	{WAKE_SRC_ADC_COMP,						WAKEUP_NULL},
	{WAKE_SRC_ADC,							WAKEUP_NULL},
	{WAKE_SRC_RTC,							WAKEUP_NULL},
	{WAKE_SRC_CTOUCH,						WAKEUP_NULL},
	{WAKE_SRC_I2C1,							WAKEUP_NULL},
	{WAKE_SRC_I2C0,							WAKEUP_NULL},
	{WAKE_SRC_GPIOB,						WAKEUP_NULL},
	{WAKE_SRC_GPIOA,						WAKEUP_NULL},
	{WAKE_SRC_UART_LOG,						WAKEUP_NULL},
	{WAKE_SRC_UART2_BT,						WAKEUP_NULL},
	{WAKE_SRC_UART1,						WAKEUP_NULL},
	{WAKE_SRC_UART0,						WAKEUP_NULL},
	{WAKE_SRC_pmc_timer1,					WAKEUP_KM0},	/* internal usage, do not change it*/
	{WAKE_SRC_pmc_timer0,					WAKEUP_KM4},	/* internal usage, do not change it*/
	{WAKE_SRC_Timer7,						WAKEUP_NULL},
	{WAKE_SRC_Timer6,						WAKEUP_NULL},
	{WAKE_SRC_Timer5,						WAKEUP_NULL},
	{WAKE_SRC_Timer4,						WAKEUP_NULL},
	{WAKE_SRC_IWDG0,						WAKEUP_NULL},
	{WAKE_SRC_IPC_KM4,						WAKEUP_KM4},	/* IPC can only wakeup KM4, do not change it*/
	{WAKE_SRC_BT_WAKE_HOST,					WAKEUP_NULL},
	{WAKE_SRC_KM4_WAKE_IRQ,					WAKEUP_KM0},	/* internal usage, do not change it*/
	{WAKE_SRC_WIFI_FTSR_MAILBOX,			WAKEUP_KM0},	/* For WiFi wakeup, do not change it*/
	{WAKE_SRC_WIFI_FISR_FESR_IRQ,			WAKEUP_KM0},	/* For WiFi wakeup, do not change it*/

	{0xFFFFFFFF,							WAKEUP_NULL},	/* Table end */
};

/* can be used by sleep mode & deep sleep mode */
/* config can be set to DISABLE_WAKEPIN/HIGH_LEVEL_WAKEUP/LOW_LEVEL_WAKEUP */
WAKEPIN_TypeDef sleep_wakepin_config[] = {
//  wakepin			config
	{WAKEPIN_0,		DISABLE_WAKEPIN},	/* WAKEPIN_0 corresponding to _PB_30 */
	{WAKEPIN_1,		DISABLE_WAKEPIN},	/* WAKEPIN_1 corresponding to _PB_31 */

	{0xFFFFFFFF,	DISABLE_WAKEPIN},	/* Table end */
};

PSCFG_TypeDef ps_config = {
	.keep_OSC4M_on = FALSE,			/* keep OSC4M on or off for sleep */
	.xtal_mode_in_sleep = XTAL_OFF,	/* set xtal mode during sleep mode, see enum xtal_mode_sleep for detail */
	.sleep_to_08V = FALSE,			/* default sleep to 0.7V, set this option to TRUE will sleep to 0.8V */
};

UARTCFG_TypeDef uart_config[3] = {
	/* UART0 */
	{
		.LOW_POWER_RX_ENABLE = DISABLE, /*Enable low power RX*/
	},
	/* UART1 */
	{
		.LOW_POWER_RX_ENABLE = DISABLE,
	},
	/* UART2_BT */
	{
		.LOW_POWER_RX_ENABLE = DISABLE,
	},
};