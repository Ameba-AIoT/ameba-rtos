/**
  ******************************************************************************
  * @file    inic_ipc_host_api_basic.c
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
#include "whc_host.h"

#include <rtw_timer.h>
#include <rtw_wakelock.h>
#include "ameba_pmu.h"
#include <wifi_intf_ram_to_rom.h>
#include "wpa_lite_intf.h"
#include "rom_hal_rom_to_flash.h"

/* Give default value if not defined */
/******************************************************
 *               Function Definitions
 ******************************************************/
static void *_my_calloc(size_t nelements, size_t elementSize)
{
	size_t size;
	void *ptr = NULL;

	size = nelements * elementSize;
	ptr = rtos_mem_zmalloc(size);

	return ptr;
}

static void _my_free(void *pbuf)
{
	rtos_mem_free(pbuf);
}

static int _my_random(void *p_rng, unsigned char *output, size_t output_len)
{
	/* To avoid gcc warnings */
	(void) p_rng;

	TRNG_get_random_bytes(output, output_len);
	return 0;
}
int wifi_set_platform_rom_func(void *(*calloc_func)(size_t, size_t),
							   void (*free_func)(void *),
							   int (*rand_func)(void *, unsigned char *, size_t))
{
	/* Realtek added to initialize HW crypto function pointers
	* mbedtls RAM codes use function pointers in platform memory implementation
	* Not use malloc/free in ssl ram map for mbedtls RAM codes
	*/
	p_wifi_rom_func_map = (struct _wifi_rom_func_map *)&wifi_rom_func_map;
	p_wifi_rom_func_map->zmalloc = calloc_func;
	p_wifi_rom_func_map->mfree = free_func;
	p_wifi_rom_func_map->random = rand_func;

	return (0);
}

static void rtos_critical_enter_wifi_rom2flash(void)
{
	rtos_critical_enter(RTOS_CRITICAL_WIFI);
}

static void rtos_critical_exit_wifi_rom2flash(void)
{
	rtos_critical_exit(RTOS_CRITICAL_WIFI);
}

static void wifi_set_platform_rom_os_func(void)
{
	/* Realtek added for code in rom
	*/
	p_wifi_rom2flash = (struct _wifi_rom_to_flash_func_map *)&wifi_rom2flash;

	/* mutex */
	p_wifi_rom2flash->rtw_rtos_mutex_give_t = rtos_mutex_give;
	p_wifi_rom2flash->rtw_rtos_mutex_take_t = rtos_mutex_take;
	p_wifi_rom2flash->rtos_mutex_delete_static_t = rtos_mutex_delete_static;
	p_wifi_rom2flash->rtos_mutex_create_static_t = rtos_mutex_create_static;

	/* sema */
	p_wifi_rom2flash->rtos_sema_give = rtos_sema_give;
	p_wifi_rom2flash->rtos_sema_take = rtos_sema_take;

	/* critical */
	p_wifi_rom2flash->rtw_rtos_critical_enter_t = rtos_critical_enter_wifi_rom2flash;
	p_wifi_rom2flash->rtw_rtos_critical_exit_t = rtos_critical_exit_wifi_rom2flash;

	/* os */
	p_wifi_rom2flash->rtos_time_delay_ms_t = rtos_time_delay_ms;
	p_wifi_rom2flash->rtos_time_get_current_system_time_ms = rtos_time_get_current_system_time_ms;

	/* timer */
	p_wifi_rom2flash->rtw_init_timer_t = rtw_init_timer;
	p_wifi_rom2flash->rtw_set_timer_t = rtw_set_timer;

	p_wifi_rom2flash->rtw_cancel_timer_t = rtw_cancel_timer;
	p_wifi_rom2flash->rtw_del_timer_t = rtw_del_timer;

	/* pmu */
	p_wifi_rom2flash->pmu_set_sysactive_time_t = pmu_set_sysactive_time;
	p_wifi_rom2flash->rtw_wakelock_timeout = rtw_wakelock_timeout;

	/* wakelock */
	p_wifi_rom2flash->rtw_acquire_wakelock_t = rtw_acquire_wakelock;
	p_wifi_rom2flash->rtw_release_wakelock_t = rtw_release_wakelock;

	/* skbuff not in ap*/
}

void wifi_set_rom2flash(void)
{
	wifi_set_platform_rom_func(_my_calloc, _my_free, _my_random);
	wifi_set_platform_rom_os_func();
}
