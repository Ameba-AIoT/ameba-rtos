/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 * Copyright (c) 2020, Nordic Semiconductor ASA. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include <stdio.h>
#include "cmsis.h"
#include "tfm_spm_log.h"
#include "spu.h"
#include "exception_info.h"
#include "utilities.h"

static void spu_dump_context(void)
{
    SPMLOG_ERRMSG("Platform Exception: SPU Fault\r\n");

    /* Report which type of violation occured */
    if(NRF_SPU->EVENTS_RAMACCERR)
    {
        SPMLOG_DBGMSG("  RAMACCERR\r\n");
    }
    if(NRF_SPU->EVENTS_PERIPHACCERR)
    {
        SPMLOG_DBGMSG("  PERIPHACCERR\r\n");
    }
    if(NRF_SPU->EVENTS_FLASHACCERR)
    {
        SPMLOG_DBGMSG("  FLASHACCERR\r\n");
    }
}

void SPU_Handler(void)
{
    spu_dump_context();

    /* Clear SPU interrupt flag and pending SPU IRQ */
    spu_clear_events();

    NVIC_ClearPendingIRQ(SPU_IRQn);

    tfm_core_panic();
}

__attribute__((naked)) void SPU_IRQHandler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_PLATFORM);

    __ASM volatile(
        "BL        SPU_Handler             \n"
        "B         .                       \n"
    );
}
