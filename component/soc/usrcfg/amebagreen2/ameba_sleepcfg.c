/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "ameba_soc.h"

/* wakeup attribute can be set to WAKEUP_NULL/WAKEUP_NP/WAKEUP_AP */
struct WakeEvent_TypeDef sleep_wevent_config[] = {
//  	Module								Wakeup
#ifdef CONFIG_WHC_INTF_SDIO
	{WAKE_SRC_SDIO_WIFI,					WAKEUP_NP},
#else
	{WAKE_SRC_SDIO_WIFI,					WAKEUP_NULL},
#endif
	{WAKE_SRC_BT_WAKE_HOST,					WAKEUP_NULL},
	{WAKE_SRC_BT_SCB,						WAKEUP_NULL},
	{WAKE_SRC_CAN1,							WAKEUP_NULL},
	{WAKE_SRC_CAN0,							WAKEUP_NULL},
	{WAKE_SRC_USB,							WAKEUP_NULL},
	{WAKE_SRC_SDIO_HOST,					WAKEUP_NULL},
	{WAKE_SRC_SDIO_BT,						WAKEUP_NULL},
	{WAKE_SRC_AON_WAKEPIN,					WAKEUP_NULL},
	{WAKE_SRC_AON_TIM,						WAKEUP_NULL},
	{WAKE_SRC_RMII,							WAKEUP_NULL},
	{WAKE_SRC_PWR_DOWN,						WAKEUP_NULL},
	{WAKE_SRC_BOR,							WAKEUP_NULL},
	{WAKE_SRC_CAPTOUCH,						WAKEUP_NULL},
	{WAKE_SRC_ADC,							WAKEUP_NULL},
	{WAKE_SRC_RTC,							WAKEUP_NULL},
	{WAKE_SRC_GPIOC,						WAKEUP_NULL},
	{WAKE_SRC_GPIOB,						WAKEUP_NULL},
	{WAKE_SRC_GPIOA,						WAKEUP_AP},
	{WAKE_SRC_UART_LOG,						WAKEUP_NULL},
	{WAKE_SRC_UART3,						WAKEUP_NULL},
	{WAKE_SRC_UART2,						WAKEUP_NULL},
	{WAKE_SRC_UART1,						WAKEUP_NULL},
	{WAKE_SRC_UART0,						WAKEUP_NULL},
	{WAKE_SRC_pmc_timer1,					WAKEUP_NP},		/* internal usage, do not change it*/
	{WAKE_SRC_pmc_timer0,					WAKEUP_AP},		/* internal usage, do not change it*/
	{WAKE_SRC_Timer3,						WAKEUP_NULL},
	{WAKE_SRC_Timer2,						WAKEUP_NULL},
	{WAKE_SRC_Timer1,						WAKEUP_NULL},
	{WAKE_SRC_Timer0,						WAKEUP_NULL},
	{WAKE_SRC_IWDG,							WAKEUP_NULL},
	{WAKE_SRC_AP_WAKE_IRQ,					WAKEUP_NP},
	{WAKE_SRC_WIFI_FTSR_MAILBOX_IRQ,		WAKEUP_NP},		/* For WiFi wakeup, do not change it*/
	{WAKE_SRC_WIFI_FISR_FESR_IRQ,			WAKEUP_NP},		/* For WiFi wakeup, do not change it*/

	{0xFFFFFFFF,							WAKEUP_NULL},	/* Table end */
};

/* config attribute can be set to LOW_LEVEL_WAKEUP/HIGH_LEVEL_WAKEUP/DISABLE_WAKEPIN */
struct WAKEPIN_TypeDef sleep_wakepin_config[] = {
//  wakepin			config
	{WAKEPIN_0,		DISABLE_WAKEPIN},	/* WAKEPIN_0(_PA_3) config */
	{WAKEPIN_1,		DISABLE_WAKEPIN},	/* WAKEPIN_1(_PA_2) config */
	{WAKEPIN_2,		DISABLE_WAKEPIN},	/* WAKEPIN_2(_PA_1) config */
	{WAKEPIN_3,		DISABLE_WAKEPIN},	/* WAKEPIN_3(_PA_0) config */
	{0xFFFFFFFF,	DISABLE_WAKEPIN},	/* Table end */
};

struct PSCFG_TypeDef ps_config = {
	.keep_osc4m_on_in_sleep = FALSE,				/* keep OSC4M off or on during sleep */
	.xtal_mode_in_sleep = XTAL_OFF,					/* set xtal mode during sleep mode, see enum xtal_mode_sleep for detail */
	.regu_state_in_sleep = STATE1_LDOPC_SWRPFM_07,
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
