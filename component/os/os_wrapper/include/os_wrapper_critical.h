/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_CRITCAL_H__
#define __OS_WRAPPER_CRITCAL_H__

/**
 * @brief  Define core numbers settings. This macro will be checked during compilation to ensure
 * that it is consistent with the configuration of the operating system.
 */
#if defined CONFIG_CPUS_NUM && (CONFIG_CPUS_NUM == 2)
#define RTOS_NUM_CORES 2
#else
#define RTOS_NUM_CORES 1
#endif

/**
 * Define CA32 SMP spin lock id
*/
typedef enum {
	RTOS_CRITICAL_DEFAULT = 0,
	RTOS_CRITICAL_SOC,
	RTOS_CRITICAL_AUDIO,
	RTOS_CRITICAL_WIFI,
	RTOS_CRITICAL_BT,
	RTOS_CRITICAL_USB,
	RTOS_CRITICAL_MAX
} RTOS_CRITICAL_LIST;

/**
 * @brief  Check if in task interrupt
 * @retval 1: interrupt; 0: context
 */
int rtos_critical_is_in_interrupt(void);

/**
 * @brief  Internally handles interrupt status (PRIMASK/CPSR) save
 */
void rtos_critical_enter(uint32_t component_id);
void rtos_critical_enter_old(void); // replace this API with rtos_critical_enter. This API will be removed

/**
 * @brief  Internally handles interrupt status(PRIMASK/CPSR) restore
 */
void rtos_critical_exit(uint32_t component_id);
void rtos_critical_exit_old(void);  // replace this API with rtos_critical_exit. This API will be removed

/**
 * @brief  get task enter critical state
 * @retval >0: in critical state; 0: exit critical state
 */
uint32_t rtos_get_critical_state(void);
#endif
