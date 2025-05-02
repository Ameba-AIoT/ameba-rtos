/**
  ******************************************************************************
  * @file    wifi_init.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include "ameba_soc.h"
#include "rtw_inic_common.h"
#include <zephyr/kernel.h>

#define WIFI_STACK_SIZE_INIT ((512 + 768) * 4)
static struct k_thread rtk_wifi_init_thread;

K_THREAD_STACK_DEFINE(rtk_wifi_init_stack, WIFI_STACK_SIZE_INIT);

extern void wifi_set_rom2flash(void);

#if defined(CONFIG_AS_INIC_AP)
void _init_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
	u32 val32 = 0;

	/* wait for inic_ipc_device ready, after that send WIFI_ON ipc msg to device */
	while ((HAL_READ32(REG_AON_WIFI_IPC, 0) & AON_BIT_WIFI_INIC_NP_READY) == 0) {
		rtos_time_delay_ms(1);
	}
	val32 = HAL_READ32(REG_AON_WIFI_IPC, 0);
	val32 &= ~ AON_BIT_WIFI_INIC_NP_READY;
	HAL_WRITE32(REG_AON_WIFI_IPC, 0, val32);

	wifi_on(RTW_MODE_STA);

	/* Kill init thread after all init tasks done */
	rtos_task_delete(NULL);
}

void wifi_init(void)
{
	wifi_set_rom2flash();
	whc_host_init();

	k_thread_create(&rtk_wifi_init_thread, rtk_wifi_init_stack,
					WIFI_STACK_SIZE_INIT,
					(k_thread_entry_t)_init_thread, NULL, NULL, NULL,
					2, K_USER,
					K_NO_WAIT);
}

#elif defined(CONFIG_SINGLE_CORE_WIFI)

void _init_thread(void *param)
{
	wifi_set_user_config();

	wifi_on(RTW_MODE_STA);

	/* Kill init thread after all init tasks done */
	rtos_task_delete(NULL);
}

void wifi_init(void)
{
	wifi_set_rom2flash();

	k_thread_create(&rtk_wifi_init_thread, rtk_wifi_init_stack,
					WIFI_STACK_SIZE_INIT,
					(k_thread_entry_t)_init_thread, NULL, NULL, NULL,
					5, K_USER,
					K_NO_WAIT);
}

#endif
