/*
 * Copyright (c) 2020-2023 Arm Limited. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "microsecond_timer.h"
#include "cmsis.h"

/* Maximum count for the SysTick */
#define MAX_SYSTICK_VAL (0x00ffffff)

typedef struct _systick_us_context {
    uint64_t overflow_count; /* Number of overflows; Forms high-order bits of 64-bit elapsed time */
    uint32_t core_clock_per_us; /* Number of core clocks per microsecond */
} systick_us_context_t;

/* Global state for the systick microsecond timer */
static systick_us_context_t s_context = { 0 };

/* @brief SysTick interrupt handler.
 *
 * Increments the high-order bits of the 64-bit elapsed time every time the 24-bit SysTick
 * counter rolls over from 0 and starts counting again from the maximum value.
 */
void SysTick_Handler(void)
{
    s_context.overflow_count += MAX_SYSTICK_VAL + 1;
}

/* Initializes the SysTick timer */
void microsecond_timer_init(void) {
    /* Precompute the number of clocks per microsecond */
    s_context.core_clock_per_us = SystemCoreClock / 1e6;

    /* First disable SysTick */
    SysTick->CTRL = 0;

    /* Init SysTick reload with the maximum count */
    SysTick->LOAD = MAX_SYSTICK_VAL;
    SysTick->VAL = 0; /* Writing any value clears */

    /* Enable the interrupt and start counting */
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

uint64_t get_elapsed_us(void)
{
    uint32_t elapsed_microseconds = (MAX_SYSTICK_VAL - SysTick->VAL) / s_context.core_clock_per_us;
    return s_context.overflow_count + elapsed_microseconds;
}

static uint64_t get_elapsed_ms(void)
{
    return get_elapsed_us() / 1000;
}

void sleep_ms(uint32_t delay_ms)
{
    uint64_t start_time = get_elapsed_ms();
    while (get_elapsed_ms() - start_time < delay_ms) {
    }
}
