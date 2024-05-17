/*
 * Copyright (c) 2020-2023 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef __MICROSECOND_TIMER_H__
#define __MICROSECOND_TIMER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * @brief Init the microsecond timer.
 */
void microsecond_timer_init(void);

/*!
 * @brief Delay for a number of milliseconds.
 */
void sleep_ms(uint32_t delay_ms);

#ifdef __cplusplus
}
#endif

#endif /* __MICROSECOND_TIMER_H__ */
