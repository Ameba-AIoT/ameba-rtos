/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * This file is derivative of CMSIS V5.9.0 startup_ARMCM0plus.c
 * Git SHA: 2b7495b8535bdcb306dac29b9ded4cfb679d7e5c
 */

#include "cmsis.h"

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;

extern void __PROGRAM_START(void) __NO_RETURN;

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Reset_Handler  (void) __NO_RETURN;

/*----------------------------------------------------------------------------
  Exception / Interrupt Handler
 *----------------------------------------------------------------------------*/
#define DEFAULT_IRQ_HANDLER(handler_name)  \
void __WEAK handler_name(void) __NO_RETURN; \
void handler_name(void) { \
    while(1); \
}

/* Exceptions */
DEFAULT_IRQ_HANDLER(NMI_Handler)
DEFAULT_IRQ_HANDLER(HardFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(SysTick_Handler)

DEFAULT_IRQ_HANDLER(SE_INTERRUPT_EXPANSION_IRQHandler)
DEFAULT_IRQ_HANDLER(CRYPTO_ACCELERATOR_0_IRQHandler)
DEFAULT_IRQ_HANDLER(CRYPTO_ACCELERATOR_1_IRQHandler)
DEFAULT_IRQ_HANDLER(SE_WATCHDOG_TIMER_IRQHandler)
DEFAULT_IRQ_HANDLER(TIMER0_IRQHandler)
DEFAULT_IRQ_HANDLER(TIMER1_IRQHandler)
DEFAULT_IRQ_HANDLER(HS_FIREWALL_TAMPER_IRQHandler)
DEFAULT_IRQ_HANDLER(INTERRUPT_ROUTER_TAMPER_IRQHandler)
DEFAULT_IRQ_HANDLER(SECURE_WATCHDOG_WS1_IRQHandler)
DEFAULT_IRQ_HANDLER(SECNENCTOP_PPU_IRQHandler)
DEFAULT_IRQ_HANDLER(UART_UARTINTR_IRQHandler)
DEFAULT_IRQ_HANDLER(SE_FIREWALL_INTERRUPT_IRQHandler)
DEFAULT_IRQ_HANDLER(SE_CTI_TRIGGER_OUT_2_IRQHandler)
DEFAULT_IRQ_HANDLER(SE_CTI_TRIGGER_OUT_3_IRQHandler)
DEFAULT_IRQ_HANDLER(SEH0_SENDER_COMBINED_IRQHandler)
DEFAULT_IRQ_HANDLER(HSE0_RECEIVER_COMBINED_IRQHandler)
DEFAULT_IRQ_HANDLER(SEH1_SENDER_COMBINED_IRQHandler)
DEFAULT_IRQ_HANDLER(HSE1_RECEIVER_COMBINED_IRQHandler)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[48];
       const VECTOR_TABLE_Type __VECTOR_TABLE[48] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP), /*      Initial Stack Pointer */
  Reset_Handler,                      /*     Reset Handler */
  NMI_Handler,                        /* -14 NMI Handler */
  HardFault_Handler,                  /* -13 Hard Fault Handler */
  0,                                  /*     Reserved */
  0,                                  /*     Reserved */
  0,                                  /*     Reserved */
  0,                                  /*     Reserved */
  0,                                  /*     Reserved */
  0,                                  /*     Reserved */
  0,                                  /*     Reserved */
  SVC_Handler,                        /*  -5 SVCall Handler */
  0,                                  /*     Reserved */
  0,                                  /*     Reserved */
  PendSV_Handler,                     /*  -2 PendSV Handler */
  SysTick_Handler,                    /*  -1 SysTick Handler */

/* SoC Specific Interrupts */
  SE_INTERRUPT_EXPANSION_IRQHandler,  /*   0: Secure Enclave Interrupt Expansion */
  CRYPTO_ACCELERATOR_0_IRQHandler,    /*   1: Crypto Accelerator Interrupt 0 */
  CRYPTO_ACCELERATOR_1_IRQHandler,    /*   2: Crypto Accelerator Interrupt 1 */
  SE_WATCHDOG_TIMER_IRQHandler,       /*   3: Secure Enclave Watchdog Timer Interrupt */
  0,                                  /*   4: Reserved */
  TIMER0_IRQHandler,                  /*   5: CMSDK Timer 0 Interrupt */
  TIMER1_IRQHandler,                  /*   6: CMSDK Timer 1 Interrupt */
  HS_FIREWALL_TAMPER_IRQHandler,      /*   7: Host System Firewall Tamper Interrupt */
  INTERRUPT_ROUTER_TAMPER_IRQHandler, /*   8: Interrupt Router Tamper Interrupt */
  SECURE_WATCHDOG_WS1_IRQHandler,     /*   9: Secure Watchdog WS1 Interrupt */
  SECNENCTOP_PPU_IRQHandler,          /*  10: SECNENCTOP PPU Interrupt */
  UART_UARTINTR_IRQHandler,           /*  11: UART UARTINTR Interrupt */
  SE_FIREWALL_INTERRUPT_IRQHandler,   /*  12: Secure Enclave Firewall Interrupt */
  SE_CTI_TRIGGER_OUT_2_IRQHandler,    /*  13: Secure Enclave CTI Trigger Out 2 */
  SE_CTI_TRIGGER_OUT_3_IRQHandler,    /*  14: Secure Enclave CTI Trigger Out 3 */
  0,                                  /*  15: Reserved */
  0,                                  /*  16: Reserved */
  0,                                  /*  17: Reserved */
  0,                                  /*  18: Reserved */
  0,                                  /*  19: Reserved */
  0,                                  /*  20: Reserved */
  SEH0_SENDER_COMBINED_IRQHandler,    /*  21: SEH0 Sender Combined Interrupt */
  0,                                  /*  22: Reserved */
  HSE0_RECEIVER_COMBINED_IRQHandler,  /*  23: HSE0 Receiver Combined Interrupt */
  0,                                  /*  24: Reserved */
  0,                                  /*  25: Reserved */
  SEH1_SENDER_COMBINED_IRQHandler,    /*  26: SEH1 Sender Combined Interrupt */
  0,                                  /*  27: Reserved */
  HSE1_RECEIVER_COMBINED_IRQHandler,  /*  28: HSE1 Receiver Combined Interrupt */
  0,                                  /*  29: Reserved */
  0,                                  /*  30: Reserved */
  0,                                  /*  31: Reserved */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
    SystemInit();                             /* CMSIS System Initialization */
    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
