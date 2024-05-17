/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

/* FIXME: This shouldn't be required when TFM_PLAT_SPECIFIC_MULTI_CORE_COMM is
 * enabled.
 */

#include "tfm_ns_mailbox.h"

int32_t tfm_ns_mailbox_hal_init(struct ns_mailbox_queue_t *queue)
{
    (void)queue;
    return 0;
}

int32_t tfm_ns_mailbox_hal_notify_peer(void)
{
    return 0;
}

void tfm_ns_mailbox_hal_enter_critical(void)
{
}

void tfm_ns_mailbox_hal_exit_critical(void)
{
}

void tfm_ns_mailbox_hal_enter_critical_isr(void)
{
}

void tfm_ns_mailbox_hal_exit_critical_isr(void)
{
}

int32_t tfm_platform_ns_wait_for_s_cpu_ready(void)
{
    return 0;
}
