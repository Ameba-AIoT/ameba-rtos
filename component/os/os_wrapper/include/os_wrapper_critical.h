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
 * @brief  Check if in task interrupt
 * @retval 1: interrupt; 0: context
 */
int rtos_critical_is_in_interrupt(void);

/**
 * @brief  For FreeRTOS, map to taskENTER_CRITICAL / taskENTER_CRITICAL_FROM_ISR
 *         Internally handles interrupt status (PRIMASK/CPSR) save
 */
void rtos_critical_enter(void);

/**
 * @brief  For FreeRTOS, map to taskEXIT_CRITICAL / taskEXIT_CRITICAL_FROM_ISR
 *         Internally handles interrupt status(PRIMASK/CPSR) restore
 */
void rtos_critical_exit(void);


#endif
