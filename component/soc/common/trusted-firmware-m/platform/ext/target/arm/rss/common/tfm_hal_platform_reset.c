/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"

void tfm_hal_system_reset(void)
{
    struct rss_sysctrl_t *rss_sysctrl = (void *)RSS_SYSCTRL_BASE_S;

    __DSB();
    rss_sysctrl->swreset = 0x1u << 5;
    __DSB();

    while(1) {
        __NOP();
    }
}

void tfm_hal_system_halt(void)
{
    /*
     * Disable IRQs to stop all threads, not just the thread that
     * halted the system.
     */
    __disable_irq();

    /*
     * Enter sleep to reduce power consumption and do it in a loop in
     * case a signal wakes up the CPU.
     */
    while (1) {
        __WFE();
    }
}
