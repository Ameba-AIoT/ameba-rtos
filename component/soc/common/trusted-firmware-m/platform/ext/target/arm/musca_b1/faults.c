/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "target_cfg.h"
#include "Driver_MPC.h"
#include "utilities.h"
#include "exception_info.h"
#include "tfm_hal_platform.h"

/* Import MPC driver */
extern ARM_DRIVER_MPC Driver_EFLASH0_MPC;
extern ARM_DRIVER_MPC Driver_CODE_SRAM_MPC;

void C_MPC_Handler(void)
{
    /* Clear MPC interrupt flags and pending MPC IRQ */
    Driver_EFLASH0_MPC.ClearInterrupt();
    Driver_CODE_SRAM_MPC.ClearInterrupt();
    NVIC_ClearPendingIRQ(S_MPC_COMBINED_IRQn);

    /* Print fault message and block execution */
    ERROR_MSG("Platform Exception: MPC fault!!!");

    tfm_core_panic();
}

__attribute__((naked)) void MPC_Handler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_PLATFORM);

    __ASM volatile(
        "BL        C_MPC_Handler           \n"
        "B         .                       \n"
    );
}

void C_PPC_Handler(void)
{
    /*
     * Due to an issue on the FVP, the PPC fault doesn't trigger a
     * PPC IRQ which is handled by the PPC_handler.
     * In the FVP execution, this code is not execute.
     */

    /* Clear PPC interrupt flag and pending PPC IRQ */
    ppc_clear_irq();
    NVIC_ClearPendingIRQ(S_PPC_COMBINED_IRQn);

    /* Print fault message*/
    ERROR_MSG("Platform Exception: PPC fault!!!");

    tfm_core_panic();
}

__attribute__((naked)) void PPC_Handler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_PLATFORM);

    __ASM volatile(
        "BL        C_PPC_Handler           \n"
        "B         .                       \n"
    );
}

void C_NMI_Handler(void)
{
    /*
     * FixMe: Due to the issue on the MUSCA_B1 board: secure watchdog is not
     * able to reset the system on the second timeout. Hence performing the
     * warm reset as part of watchdog interrupt (Take place in NMI).
     */
    /* Print fault message*/
    ERROR_MSG("Platform Exception: NMI fault!!!");

    /* Trigger warm-reset */
    tfm_hal_system_reset();
}

__attribute__((naked)) void NMI_Handler(void)
{
    EXCEPTION_INFO(EXCEPTION_TYPE_PLATFORM);

    __ASM volatile(
        "BL        C_NMI_Handler           \n"
        "B         .                       \n"
    );
}
