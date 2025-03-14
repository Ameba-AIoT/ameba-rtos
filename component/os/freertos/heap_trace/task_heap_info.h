/**
 * @file	heap_trace.h
 * @brief	A module for tracing heap status for FreeRTOS.
 */
#ifndef __TASK_HEAP_INFO_H__
#define __TASK_HEAP_INFO_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

/**
 * @struct	TASK_HEAP_INFO
 * @brief	Structure to store task and its heap size usage information.
 */
typedef struct TASK_HEAP_INFO {
	TaskHandle_t TaskHandle;	/**< Record the TCB */
	uint32_t heap_size;     	/**< Record the size of heap usage */
	char Task_Name[ configMAX_TASK_NAME_LEN ]; /**< Record the task name */
	uint32_t isExist;        	/**< Record whether the task is deleted by vTaskDelete() */
} task_heap_info_t;

#endif
