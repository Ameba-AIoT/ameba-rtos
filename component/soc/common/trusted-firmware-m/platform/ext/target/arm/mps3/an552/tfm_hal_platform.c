/*
 * Copyright (c) 2021-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "common_target_cfg.h"
#include "tfm_hal_platform.h"
#include "tfm_peripherals_def.h"
#include "uart_stdout.h"
#include "device_definition.h"
#if defined(TEST_NS_FPU) || defined(TEST_S_FPU)
#include "test_interrupt.h"
#endif

/* Get address of memory regions to configure MPU */
extern const struct memory_region_limits memory_regions;

enum tfm_hal_status_t tfm_hal_platform_init(void)
{
    enum tfm_plat_err_t plat_err = TFM_PLAT_ERR_SYSTEM_ERR;
    enum syscounter_armv8_m_cntrl_error_t counter_err = SYSCOUNTER_ARMV8_M_ERR_NONE;

    plat_err = enable_fault_handlers();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_HAL_ERROR_GENERIC;
    }

    plat_err = system_reset_cfg();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_HAL_ERROR_GENERIC;
    }

    plat_err = init_debug();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_HAL_ERROR_GENERIC;
    }

    /* Syscounter enabled by default. This way App-RoT partitions can use
     * systimers without the need to add the syscounter as an mmio devide.
     */
    counter_err = syscounter_armv8_m_cntrl_init(&SYSCOUNTER_CNTRL_ARMV8_M_DEV_S);
    if (counter_err != SYSCOUNTER_ARMV8_M_ERR_NONE) {
        return TFM_HAL_ERROR_GENERIC;
    }

    __enable_irq();
    stdio_init();

    plat_err = nvic_interrupt_target_state_cfg();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_HAL_ERROR_GENERIC;
    }

    plat_err = nvic_interrupt_enable();
    if (plat_err != TFM_PLAT_ERR_SUCCESS) {
        return TFM_HAL_ERROR_GENERIC;
    }

#if defined(TEST_S_FPU) || defined(TEST_NS_FPU)
    /* Set IRQn in secure mode */
    NVIC_ClearTargetState(TFM_FPU_S_TEST_IRQ);

    /* Register FPU secure test interrupt handler */
    NVIC_SetVector(TFM_FPU_S_TEST_IRQ, (uint32_t)TFM_FPU_S_TEST_Handler);

    /* Enable FPU secure test interrupt */
    NVIC_EnableIRQ(TFM_FPU_S_TEST_IRQ);
#endif

#if defined(TEST_NS_FPU)
    /* Set IRQn in non-secure mode */
    NVIC_SetTargetState(TFM_FPU_NS_TEST_IRQ);
#if (TFM_LVL >= 2)
    /* On isolation level 2, FPU test ARoT service runs in unprivileged mode.
     * Set SCB.CCR.USERSETMPEND as 1 to enable FPU test service to access STIR
     * register.
     */
    SCB->CCR |= SCB_CCR_USERSETMPEND_Msk;
#endif
#endif

    return TFM_HAL_SUCCESS;
}
