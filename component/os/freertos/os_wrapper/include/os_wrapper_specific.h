/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __OS_WRAPPER_SPECIFIC_H__
#define __OS_WRAPPER_SPECIFIC_H__

#include "FreeRTOS.h"

#define RTOS_CONVERT_MS_TO_TICKS(MS) \
        ((MS == 0xFFFFFFFFUL) ? 0xFFFFFFFFUL : (TickType_t)((MS + portTICK_PERIOD_MS - 1) / portTICK_PERIOD_MS))

#endif
