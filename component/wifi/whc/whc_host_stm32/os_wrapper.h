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
#include <string.h>
#include <stdint.h>
#include "rtw_wifi_common.h"
//#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
/**
 * @brief  Common header file
*/

#include "os_wrapper_memory.h"
#include "os_wrapper_mutex.h"
#include "os_wrapper_semaphore.h"
#include "os_wrapper_task.h"
//#include "os_wrapper_time.h"
//#include "os_wrapper_misc.h"
//#include "os_wrapper_timer.h"

/**
 * @brief  General macro definition
 */

#define RTOS_MAX_DELAY    				0xFFFFFFFFUL
#define RTOS_MAX_TIMEOUT    			0xFFFFFFFFUL

#define RTOS_CONVERT_MS_TO_TICKS(MS) \
        ((MS == 0xFFFFFFFFUL) ? 0xFFFFFFFFUL : (TickType_t)((MS + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS))

#endif

