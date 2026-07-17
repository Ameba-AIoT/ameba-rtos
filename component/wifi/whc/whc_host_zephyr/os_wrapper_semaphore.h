/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_SEMA_H__
#define __OS_WRAPPER_SEMA_H__

#include <zephyr/kernel.h>

#define RTOS_SEMA_MIN_COUNT    				0x0UL
#define RTOS_SEMA_MAX_COUNT    				0xFFFFFFFFUL

/**
 * @brief  semaphore handle type
 */
typedef struct k_sem *rtos_sema_t;

/**
 * @brief  For Zephyr, map to k_sem_init
 *         Dynamic allocate memory.
 * @note   Usage example:
 * Create:
 *          rtos_sema_t sema_handle;
 *          rtos_sema_create(&sema_handle, 0, 10);
 * Give:
 *          rtos_sema_give(sema_handle);
 * Take:
 *          rtos_sema_take(sema_handle, 100);
 * Delete:
 *          rtos_sema_delete(sema_handle);
 * @param  pp_handle: The handle itself is a pointer, and the pp_handle means a pointer to the handle.
 * @param  init_count:
 * @param  max_count:
 * @retval
 */
int rtos_sema_create(rtos_sema_t *pp_handle, uint32_t init_count, uint32_t max_count);

/**
 * @brief  For Zephyr, map to k_sem_init
 *         Dynamic allocate memory.
 * @note   The semaphore must first be 'given' before it can be 'taken'.
 * Usage example:
 * Create:
 *          rtos_sema_t sema_handle;
 *          rtos_sema_create_binary(&sema_handle);
 * Give:
 *          rtos_sema_give(sema_handle);
 * Take:
 *          rtos_sema_take(sema_handle, 100);
 * Delete:
 *          rtos_sema_delete(sema_handle);
 * @param  pp_handle: The handle itself is a pointer, and the pp_handle means a pointer to the handle.
 * @retval
 */
int rtos_sema_create_binary(rtos_sema_t *pp_handle);

/**
 * @brief  For Zephyr, map to k_free
 * @param  p_handle:
 * @retval
 */
int rtos_sema_delete(rtos_sema_t p_handle);

/**
 * @brief  For Zephyr, map to k_sem_take
 * @note   k_sem_take cannot be called from interrupt context.
 * @param  p_handle:
 * @param  timeout_ms:
 * @retval
 */
int rtos_sema_take(rtos_sema_t p_handle, uint32_t timeout_ms);

/**
 * @brief  For Zephyr, map to k_sem_give
 * @param  p_handle:
 * @retval
 */
int rtos_sema_give(rtos_sema_t p_handle);

/**
 * @brief  For Zephyr, map to k_sem_count_get
 * @param  p_handle:
 * @retval
 */
uint32_t rtos_sema_get_count(rtos_sema_t p_handle);

/**
 * @brief  For Zephyr, map to k_sem_init
 *         Dynamic allocate memory.
 * @note   Usage example:
 * Create:
 *          rtos_sema_t sema_handle;
 *          rtos_sema_create(&sema_handle, 0, 10);
 * Give:
 *          rtos_sema_give(sema_handle);
 * Take:
 *          rtos_sema_take(sema_handle, 100);
 * Delete:
 *          rtos_sema_delete(sema_handle);
 * @param  pp_handle: The handle itself is a pointer, and the pp_handle means a pointer to the handle.
 * @param  init_count:
 * @param  max_count:
 * @retval
 */
int rtos_sema_create_static(rtos_sema_t *pp_handle, uint32_t init_count, uint32_t max_count);

/**
 * @brief  For Zephyr, map to k_sem_reset (resets statically-allocated sema to zero count)
 * @param  p_handle:
 * @retval
 */
int rtos_sema_delete_static(rtos_sema_t p_handle);

#endif
