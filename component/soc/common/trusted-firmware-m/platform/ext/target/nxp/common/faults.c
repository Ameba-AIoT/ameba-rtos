/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 * Copyright 2019-2020 NXP. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "utilities.h"
#include "exception_info.h"

void C_SEC_VIO_IRQHandler(void)
{
	/* Clear interrupt flag and pending IRQ */
    NVIC_ClearPendingIRQ(SEC_VIO_IRQn);

    /* Print fault message and block execution */
    ERROR_MSG("Platform Exception: MPC fault!!!");

    tfm_core_panic();
}

__attribute__((naked)) void SEC_VIO_IRQHandler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_PLATFORM);

    __ASM volatile(
        "BL        C_SEC_VIO_IRQHandler    \n"
        "B         .                       \n"
    );
}
