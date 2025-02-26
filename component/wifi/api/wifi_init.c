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
#if defined(CONFIG_AS_INIC_AP) && defined(CONFIG_FULLMAC_HOST)  && CONFIG_FULLMAC_HOST
#include "whc_host_api.h"
#elif defined(CONFIG_WHC_INTF_IPC)
#include "whc_ipc.h"
#endif
#include "wifi_conf.h"
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
#include "lwip_netconf.h"
#endif
#ifndef CONFIG_AS_INIC_NP
#include "wifi_fast_connect.h"
#if defined(CONFIG_AS_INIC_AP) || (defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER && defined(CONFIG_SINGLE_CORE_WIFI))
static u32 heap_tmp;
#endif
#endif

//todo clarify
#if defined(CONFIG_WHC_INTF_SDIO)
#if defined(CONFIG_FULLMAC_BRIDGE)
#include "whc_bridge_sdio_dev.h"
#else
#include "whc_fullmac_sdio_dev.h"
#endif
#elif defined(CONFIG_WHC_INTF_SPI)
#if defined(CONFIG_FULLMAC_HOST)
#include "whc_spi_host.h"
#else
#include "whc_spi_dev.h"
#endif
#elif defined(CONFIG_WHC_INTF_USB)
#include "whc_usb_dev.h"
#endif

#define WIFI_STACK_SIZE_INIT ((512 + 768) * 4)
__attribute__((unused)) static const char *const TAG = "WLAN";
extern void wifi_set_rom2flash(void);

#if defined(CONFIG_AS_INIC_AP)
void _init_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
#ifndef CONFIG_SPI_FULLMAC_HOST
	u32 val32 = 0;
#endif

#ifdef CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	LwIP_Init();
#endif

#ifndef CONFIG_SPI_FULLMAC_HOST
	/* wait for inic_ipc_device ready, after that send WIFI_ON ipc msg to device */
	while ((HAL_READ32(REG_AON_WIFI_IPC, 0) & AON_BIT_WIFI_INIC_NP_READY) == 0) {
		rtos_time_delay_ms(1);
	}
	val32 = HAL_READ32(REG_AON_WIFI_IPC, 0);
	val32 &= ~ AON_BIT_WIFI_INIC_NP_READY;
	HAL_WRITE32(REG_AON_WIFI_IPC, 0, val32);
#endif

	wifi_on(RTW_MODE_STA);

#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_config_autoreconnect(1);
#endif

#ifdef CONFIG_WHC_INTF_IPC
	heap_tmp = heap_tmp - rtos_mem_get_free_heap_size() - WIFI_STACK_SIZE_INIC_IPC_HST_EVT_API - WIFI_STACK_SIZE_INIC_IPC_HST_API - WIFI_STACK_SIZE_INIC_MSG_Q -
			   WIFI_STACK_SIZE_INIT;
#endif
#ifdef CONFIG_LWIP_LAYER
	heap_tmp -= TCPIP_THREAD_STACKSIZE * 4;
#endif
	RTK_LOGI(TAG, "AP consume heap %d\n", heap_tmp);
	RTK_LOGI(TAG, "%s(%d), Available heap %d\n", __FUNCTION__, __LINE__, rtos_mem_get_free_heap_size() + WIFI_STACK_SIZE_INIT);

	/* Kill init thread after all init tasks done */
	rtos_task_delete(NULL);
}

void wlan_initialize(void)
{
	heap_tmp = rtos_mem_get_free_heap_size();
	wifi_set_rom2flash();
	whc_host_init();
	wifi_fast_connect_enable(1);

	if (rtos_task_create(NULL, ((const char *)"init"), _init_thread, NULL, WIFI_STACK_SIZE_INIT, 2) != SUCCESS) {
		RTK_LOGE(TAG, "wlan_initialize failed\n");
	}
}

#elif defined(CONFIG_AS_INIC_NP)
void wlan_initialize(void)
{
	u32 value;
	wifi_set_rom2flash();
	whc_dev_init();

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

#if defined(CONFIG_ARM_CORE_CM4) && defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtos_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

#ifdef CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	heap_tmp = rtos_mem_get_free_heap_size();
	LwIP_Init();
	RTK_LOGI(TAG, "LWIP consume heap %d\n", heap_tmp - rtos_mem_get_free_heap_size() - TCPIP_THREAD_STACKSIZE * 4);
#endif

#if defined(CONFIG_FULLMAC_BRIDGEB)
	wifi_fast_connect_enable(0);
	whc_dev_init();
#endif
	wifi_set_user_config();

	wifi_on(RTW_MODE_STA);
#if CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_config_autoreconnect(1);
#endif

#ifdef CONFIG_FULLMAC_BRIDGE
	whc_dev_init_lite();
#endif

	RTK_LOGI(TAG, "%s(%d), Available heap %d\n", __FUNCTION__, __LINE__, rtos_mem_get_free_heap_size() + WIFI_STACK_SIZE_INIT);

	/* Kill init thread after all init tasks done */
	rtos_task_delete(NULL);
}

void wlan_initialize(void)
{
	wifi_set_rom2flash();

	wifi_fast_connect_enable(1);

	if (rtos_task_create(NULL, ((const char *)"init"), _init_thread, NULL, WIFI_STACK_SIZE_INIT, 5) != SUCCESS) {
		RTK_LOGE(TAG, "wlan_initialize failed\n");
	}
}

#endif
#endif
