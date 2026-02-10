/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_H__
#define __OS_WRAPPER_H__

/**
 * @brief  Necessary headers
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * @brief  Common header file
*/

#include "os_wrapper_memory.h"
#include "os_wrapper_mutex.h"
#include "os_wrapper_semaphore.h"
#include "os_wrapper_task.h"

/**
 * @brief  General macro definition
 */


typedef void *rtos_mutex_t;
typedef void *rtos_sema_t;


#define RTOS_MAX_DELAY    				0xFFFFFFFFUL
#define RTOS_MAX_TIMEOUT    			        0xFFFFFFFFUL

#endif

