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
#if defined(CONFIG_AS_INIC_AP) && defined(CONFIG_WHC_HOST)  && CONFIG_WHC_HOST
#include "whc_host_api.h"
#elif defined(CONFIG_WHC_INTF_IPC)
#include "whc_ipc.h"
#endif
#include "rtw_skbuff.h"
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
#include "lwip_netconf.h"
#endif
#include "wifi_intf_drv_to_upper.h"

#if defined(CONFIG_WHC_BRIDGE)
#include "whc_dev_bridge.h"
#include "whc_bridge_dev_api.h"
#endif

//todo clarify
#if defined(CONFIG_WHC_INTF_SDIO)
#if defined(CONFIG_WHC_BRIDGE)
#include "whc_bridge_sdio_dev.h"
#else
#include "whc_fullmac_sdio_dev.h"
#endif
#elif defined(CONFIG_WHC_INTF_SPI)
#if defined(CONFIG_WHC_HOST)
#include "whc_spi_host.h"
#else
#include "whc_spi_dev.h"
#endif
#elif defined(CONFIG_WHC_INTF_USB)
#include "whc_usb_dev.h"
#endif

#define WIFI_STACK_SIZE_INIT ((512 + 768) * 4)

/**********************************************************************************************
 *                                WHC host performs wifi init
 *********************************************************************************************/
#if defined(CONFIG_AS_INIC_AP)
void wifi_init_thread(void *param)
{
	UNUSED(param);
#ifdef CONFIG_LWIP_LAYER
	LwIP_Init();
#endif

	whc_host_init();

#ifndef CONFIG_WHC_BRIDGE_HOST
	wifi_on(RTW_MODE_STA);

	RTK_LOGI(TAG_WLAN_DRV, "Available heap after wifi init %d\n", rtos_mem_get_free_heap_size() + WIFI_STACK_SIZE_INIT);
#endif

	rtos_task_delete(NULL);
}

/**********************************************************************************************
 *                                 WHC Device performs wifi init
 *********************************************************************************************/
#elif defined(CONFIG_AS_INIC_NP)
void wifi_init_thread(void *param)
{
	UNUSED(param);

	whc_dev_init();
	rtos_task_delete(NULL);
}

/**********************************************************************************************
 *                               Single core mode performs wifi init
 *********************************************************************************************/
#elif defined(CONFIG_SINGLE_CORE_WIFI)/*Single core processor do wifi init*/
void wifi_init_thread(void *param)
{
	UNUSED(param);
#if defined(CONFIG_ARM_CORE_CM4) && defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtos_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif

#ifdef CONFIG_LWIP_LAYER
	LwIP_Init();
#endif

#if defined(CONFIG_WHC_BRIDGEB)
	whc_dev_init();
#endif

	wifi_on(RTW_MODE_STA);

#ifdef CONFIG_WHC_BRIDGE
	whc_bridge_dev_init_user_task();
	whc_dev_init_lite();
#endif

	RTK_LOGI(TAG_WLAN_DRV, "Available heap after wifi init %d\n", rtos_mem_get_free_heap_size() + WIFI_STACK_SIZE_INIT);

	/* Kill init thread after all init tasks done */
	rtos_task_delete(NULL);
}

#endif

void wifi_init(void)
{
	wifi_set_rom2flash();
	if (rtos_task_create(NULL, ((const char *)"wifi_init_thread"), wifi_init_thread, NULL, WIFI_STACK_SIZE_INIT, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG_WLAN_DRV, "wifi_init failed\n");
	}
}
#endif
