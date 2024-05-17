/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "cmsis.h"
#include "tfm_peripherals_def.h"
#include "uart_stdout.h"
#include "Driver_USART.h"
#include "test_interrupt.h"

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler for discard all non-secure code, ameba does not use it.
 *----------------------------------------------------------------------------*/
#define DEFAULT_IRQ_HANDLER(handler_name)  \
void __WEAK handler_name(void) __NO_RETURN; \
void handler_name(void) { \
    while(1); \
}

DEFAULT_IRQ_HANDLER(Reset_Handler)
DEFAULT_IRQ_HANDLER(NMI_Handler)
DEFAULT_IRQ_HANDLER(HardFault_Handler)
DEFAULT_IRQ_HANDLER(MemManage_Handler)
DEFAULT_IRQ_HANDLER(BusFault_Handler)
DEFAULT_IRQ_HANDLER(UsageFault_Handler)
DEFAULT_IRQ_HANDLER(SecureFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(DebugMon_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(SysTick_Handler)

int32_t tfm_ns_platform_init(void)
{
#ifdef AMEBA_NONEED
	/* Register FPU non-secure test interrupt handler */
	NVIC_SetVector(TFM_FPU_NS_TEST_IRQ, (uint32_t)TFM_FPU_NS_TEST_Handler);

	/* Enable FPU non-secure test interrupt */
	NVIC_EnableIRQ(TFM_FPU_NS_TEST_IRQ);

	stdio_init();
#endif

	return ARM_DRIVER_OK;
}
