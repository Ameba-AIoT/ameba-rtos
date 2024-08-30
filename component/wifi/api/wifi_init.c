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
#ifdef CONFIG_WLAN
__attribute__((unused)) static const char *TAG = "WLAN";
#include "inic_ipc.h"
#include "wifi_conf.h"
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
#include "lwip_netconf.h"
#endif
extern void wifi_set_rom2flash(void);
#if defined(CONFIG_AS_INIC_AP)
#include "wifi_fast_connect.h"

void _init_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
	u32 val32;

#ifdef CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	LwIP_Init();
#endif

	/* wait for inic_ipc_device ready, after that send WIFI_ON ipc msg to device */
	while ((HAL_READ32(REG_AON_WIFI_IPC, 0) & AON_BIT_WIFI_INIC_NP_READY) == 0) {
		rtos_time_delay_ms(1);
	}
	val32 = HAL_READ32(REG_AON_WIFI_IPC, 0);
	val32 &= ~ AON_BIT_WIFI_INIC_NP_READY;
	HAL_WRITE32(REG_AON_WIFI_IPC, 0, val32);

	wifi_on(RTW_MODE_STA);

#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_config_autoreconnect(1);
#endif

	RTK_LOGI(TAG, "%s(%d), Available heap %d\n", __FUNCTION__, __LINE__, rtos_mem_get_free_heap_size());

	/* Kill init thread after all init tasks done */
	rtos_task_delete(NULL);
}

void wlan_initialize(void)
{
	wifi_set_rom2flash();
	inic_host_init();

	wifi_fast_connect_enable(1);

	if (rtos_task_create(NULL, ((const char *)"init"), _init_thread, NULL, (512 + 768) * 4, 2) != SUCCESS) {
		RTK_LOGE(TAG, "wlan_initialize failed\n");
	}
}

#elif defined(CONFIG_AS_INIC_NP)
void wlan_initialize(void)
{
	u32 value;
	wifi_set_rom2flash();
	inic_dev_init();

	/* set AON_BIT_WIFI_INIC_NP_READY=1 to indicate inic_ipc_device is ready */
	value = HAL_READ32(REG_AON_WIFI_IPC, 0);
	HAL_WRITE32(REG_AON_WIFI_IPC, 0, value | AON_BIT_WIFI_INIC_NP_READY);
}

#elif defined(CONFIG_SINGLE_CORE_WIFI)
#include "wifi_fast_connect.h"
void _init_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;

#if defined(ARM_CORE_CM4) && defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtos_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

#ifdef CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	LwIP_Init();
#endif

#ifdef CONFIG_SDIO_BRIDGE
	wifi_fast_connect_enable(0);
	inic_dev_init();
#endif
	wifi_set_user_config();

	wifi_on(RTW_MODE_STA);
#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_config_autoreconnect(1);
#endif

	RTK_LOGI(TAG, "%s(%d), Available heap %d\n", __FUNCTION__, __LINE__, rtos_mem_get_free_heap_size());

	/* Kill init thread after all init tasks done */
	rtos_task_delete(NULL);
}

void wlan_initialize(void)
{
	wifi_set_rom2flash();
	wifi_fast_connect_enable(1);

	if (rtos_task_create(NULL, ((const char *)"init"), _init_thread, NULL, (512 + 768) * 4, 5) != SUCCESS) {
		RTK_LOGE(TAG, "wlan_initialize failed\n");
	}
}

#endif
#endif
