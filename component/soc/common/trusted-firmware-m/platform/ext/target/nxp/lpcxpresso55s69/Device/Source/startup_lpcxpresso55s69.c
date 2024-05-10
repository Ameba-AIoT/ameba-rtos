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
 * This file is derivative of CMSIS V5.9.0 startup_ARMCM33.c
 * Git SHA: 2b7495b8535bdcb306dac29b9ded4cfb679d7e5c
 */

#include "cmsis.h"

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
extern uint64_t __STACK_SEAL;
#endif

typedef void(*VECTOR_TABLE_Type)(void);

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
DEFAULT_IRQ_HANDLER(MemManage_Handler)
DEFAULT_IRQ_HANDLER(BusFault_Handler)
DEFAULT_IRQ_HANDLER(UsageFault_Handler)
DEFAULT_IRQ_HANDLER(SecureFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(DebugMon_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(SysTick_Handler)

DEFAULT_IRQ_HANDLER(WDT_BOD_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA0_IRQHandler)
DEFAULT_IRQ_HANDLER(GINT0_IRQHandler)
DEFAULT_IRQ_HANDLER(GINT1_IRQHandler)
DEFAULT_IRQ_HANDLER(PIN_INT0_IRQHandler)
DEFAULT_IRQ_HANDLER(PIN_INT1_IRQHandler)
DEFAULT_IRQ_HANDLER(PIN_INT2_IRQHandler)
DEFAULT_IRQ_HANDLER(PIN_INT3_IRQHandler)
DEFAULT_IRQ_HANDLER(UTICK0_IRQHandler)
DEFAULT_IRQ_HANDLER(MRT0_IRQHandler)
DEFAULT_IRQ_HANDLER(CTIMER0_IRQHandler)
DEFAULT_IRQ_HANDLER(CTIMER1_IRQHandler)
DEFAULT_IRQ_HANDLER(SCT0_IRQHandler)
DEFAULT_IRQ_HANDLER(CTIMER3_IRQHandler)
DEFAULT_IRQ_HANDLER(FLEXCOMM0_IRQHandler)
DEFAULT_IRQ_HANDLER(FLEXCOMM1_IRQHandler)
DEFAULT_IRQ_HANDLER(FLEXCOMM2_IRQHandler)
DEFAULT_IRQ_HANDLER(FLEXCOMM3_IRQHandler)
DEFAULT_IRQ_HANDLER(FLEXCOMM4_IRQHandler)
DEFAULT_IRQ_HANDLER(FLEXCOMM5_IRQHandler)
DEFAULT_IRQ_HANDLER(FLEXCOMM6_IRQHandler)
DEFAULT_IRQ_HANDLER(FLEXCOMM7_IRQHandler)
DEFAULT_IRQ_HANDLER(ADC0_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved39_IRQHandler)
DEFAULT_IRQ_HANDLER(ACMP_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved41_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved42_IRQHandler)
DEFAULT_IRQ_HANDLER(USB0_NEEDCLK_IRQHandler)
DEFAULT_IRQ_HANDLER(USB0_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved46_IRQHandler)
DEFAULT_IRQ_HANDLER(MAILBOX_IRQHandler)
DEFAULT_IRQ_HANDLER(PIN_INT4_IRQHandler)
DEFAULT_IRQ_HANDLER(PIN_INT5_IRQHandler)
DEFAULT_IRQ_HANDLER(PIN_INT6_IRQHandler)
DEFAULT_IRQ_HANDLER(PIN_INT7_IRQHandler)
DEFAULT_IRQ_HANDLER(CTIMER2_IRQHandler)
DEFAULT_IRQ_HANDLER(CTIMER4_IRQHandler)
DEFAULT_IRQ_HANDLER(OS_EVENT_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved55_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved56_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved57_IRQHandler)
DEFAULT_IRQ_HANDLER(SDIO_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved59_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved60_IRQHandler)
DEFAULT_IRQ_HANDLER(Reserved61_IRQHandler)
DEFAULT_IRQ_HANDLER(USB1_PHY_IRQHandler)
DEFAULT_IRQ_HANDLER(USB1_IRQHandler)
DEFAULT_IRQ_HANDLER(USB1_NEEDCLK_IRQHandler)
DEFAULT_IRQ_HANDLER(SEC_HYPERVISOR_CALL_IRQHandler)
DEFAULT_IRQ_HANDLER(SEC_GPIO_INT0_IRQ0_IRQHandler)
DEFAULT_IRQ_HANDLER(SEC_GPIO_INT0_IRQ1_IRQHandler)
DEFAULT_IRQ_HANDLER(PLU_IRQHandler)
DEFAULT_IRQ_HANDLER(SEC_VIO_IRQHandler)
DEFAULT_IRQ_HANDLER(HASHCRYPT_IRQHandler)
DEFAULT_IRQ_HANDLER(CASER_IRQHandler)
DEFAULT_IRQ_HANDLER(PUF_IRQHandler)
DEFAULT_IRQ_HANDLER(PQ_IRQHandler)
DEFAULT_IRQ_HANDLER(DMA1_IRQHandler)
DEFAULT_IRQ_HANDLER(FLEXCOMM8_IRQHandler)

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/

#if defined ( __GNUC__ )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif

extern const VECTOR_TABLE_Type __VECTOR_TABLE[];
       const VECTOR_TABLE_Type __VECTOR_TABLE[] __VECTOR_TABLE_ATTRIBUTE = {
  (VECTOR_TABLE_Type)(&__INITIAL_SP),            /*      Initial Stack Pointer */
  Reset_Handler,                     /* Reset Handler */
  NMI_Handler,                       /* NMI Handler*/
  HardFault_Handler,                 /* Hard Fault Handler*/
  MemManage_Handler,                 /* MPU Fault Handler*/
  BusFault_Handler,                  /* Bus Fault Handler*/
  UsageFault_Handler,                /* Usage Fault Handler*/
  SecureFault_Handler,               /* Secure Fault Handler */
  0,                                 /* Reserved*/
  0,                                 /* Reserved*/
  0,                                 /* Reserved*/
  SVC_Handler,                       /* SVCall Handler*/
  DebugMon_Handler,                  /* Debug Monitor Handler*/
  0,                                 /* Reserved*/
  PendSV_Handler,                    /* PendSV Handler*/
  SysTick_Handler,                   /* SysTick Handler*/

/* External Interrupts*/
  WDT_BOD_IRQHandler,                /* Windowed watchdog timer, Brownout detect, Flash interrupt */
  DMA0_IRQHandler,                   /* DMA0 controller */
  GINT0_IRQHandler,                  /* GPIO group 0 */
  GINT1_IRQHandler,                  /* GPIO group 1 */
  PIN_INT0_IRQHandler,               /* Pin interrupt 0 or pattern match engine slice 0 */
  PIN_INT1_IRQHandler,               /* Pin interrupt 1or pattern match engine slice 1 */
  PIN_INT2_IRQHandler,               /* Pin interrupt 2 or pattern match engine slice 2 */
  PIN_INT3_IRQHandler,               /* Pin interrupt 3 or pattern match engine slice 3 */
  UTICK0_IRQHandler,                 /* Micro-tick Timer */
  MRT0_IRQHandler,                   /* Multi-rate timer */
  CTIMER0_IRQHandler,                /* Standard counter/timer CTIMER0 */
  CTIMER1_IRQHandler,                /* Standard counter/timer CTIMER1 */
  SCT0_IRQHandler,                   /* SCTimer/PWM */
  CTIMER3_IRQHandler,                /* Standard counter/timer CTIMER3 */
  FLEXCOMM0_IRQHandler,              /* Flexcomm Interface 0 (USART, SPI, I2C, I2S, FLEXCOMM) */
  FLEXCOMM1_IRQHandler,              /* Flexcomm Interface 1 (USART, SPI, I2C, I2S, FLEXCOMM) */
  FLEXCOMM2_IRQHandler,              /* Flexcomm Interface 2 (USART, SPI, I2C, I2S, FLEXCOMM) */
  FLEXCOMM3_IRQHandler,              /* Flexcomm Interface 3 (USART, SPI, I2C, I2S, FLEXCOMM) */
  FLEXCOMM4_IRQHandler,              /* Flexcomm Interface 4 (USART, SPI, I2C, I2S, FLEXCOMM) */
  FLEXCOMM5_IRQHandler,              /* Flexcomm Interface 5 (USART, SPI, I2C, I2S, FLEXCOMM) */
  FLEXCOMM6_IRQHandler,              /* Flexcomm Interface 6 (USART, SPI, I2C, I2S, FLEXCOMM) */
  FLEXCOMM7_IRQHandler,              /* Flexcomm Interface 7 (USART, SPI, I2C, I2S, FLEXCOMM) */
  ADC0_IRQHandler,                   /* ADC0  */
  Reserved39_IRQHandler,             /* Reserved interrupt */
  ACMP_IRQHandler,                   /* ACMP  interrupts */
  Reserved41_IRQHandler,             /* Reserved interrupt */
  Reserved42_IRQHandler,             /* Reserved interrupt */
  USB0_NEEDCLK_IRQHandler,           /* USB Activity Wake-up Interrupt */
  USB0_IRQHandler,                   /* USB device */
  RTC_IRQHandler,                    /* RTC alarm and wake-up interrupts */
  Reserved46_IRQHandler,             /* Reserved interrupt */
  MAILBOX_IRQHandler,                /* WAKEUP,Mailbox interrupt (present on selected devices) */
  PIN_INT4_IRQHandler,               /* Pin interrupt 4 or pattern match engine slice 4 int */
  PIN_INT5_IRQHandler,               /* Pin interrupt 5 or pattern match engine slice 5 int */
  PIN_INT6_IRQHandler,               /* Pin interrupt 6 or pattern match engine slice 6 int */
  PIN_INT7_IRQHandler,               /* Pin interrupt 7 or pattern match engine slice 7 int */
  CTIMER2_IRQHandler,                /* Standard counter/timer CTIMER2 */
  CTIMER4_IRQHandler,                /* Standard counter/timer CTIMER4 */
  OS_EVENT_IRQHandler,               /* OSEVTIMER0 and OSEVTIMER0_WAKEUP interrupts */
  Reserved55_IRQHandler,             /* Reserved interrupt */
  Reserved56_IRQHandler,             /* Reserved interrupt */
  Reserved57_IRQHandler,             /* Reserved interrupt */
  SDIO_IRQHandler,                   /* SD/MMC  */
  Reserved59_IRQHandler,             /* Reserved interrupt */
  Reserved60_IRQHandler,             /* Reserved interrupt */
  Reserved61_IRQHandler,             /* Reserved interrupt */
  USB1_PHY_IRQHandler,               /* USB1_PHY */
  USB1_IRQHandler,                   /* USB1 interrupt */
  USB1_NEEDCLK_IRQHandler,           /* USB1 activity */
  SEC_HYPERVISOR_CALL_IRQHandler,    /* SEC_HYPERVISOR_CALL interrupt */
  SEC_GPIO_INT0_IRQ0_IRQHandler,     /* SEC_GPIO_INT0_IRQ0 interrupt */
  SEC_GPIO_INT0_IRQ1_IRQHandler,     /* SEC_GPIO_INT0_IRQ1 interrupt */
  PLU_IRQHandler,                    /* PLU interrupt */
  SEC_VIO_IRQHandler,                /* SEC_VIO interrupt */
  HASHCRYPT_IRQHandler,              /* HASHCRYPT interrupt */
  CASER_IRQHandler,                  /* CASPER interrupt */
  PUF_IRQHandler,                    /* PUF interrupt */
  PQ_IRQHandler,                     /* PQ interrupt */
  DMA1_IRQHandler,                   /* DMA1 interrupt */
  FLEXCOMM8_IRQHandler,              /* Flexcomm Interface 8 (SPI, , FLEXCOMM) */
};

#if defined(__ICCARM__)
#define ARRAY_LENGTH(array) (sizeof((array))/sizeof((array)[0]))
extern const VECTOR_TABLE_Type __attribute__ ((alias ("__vector_table")))
             __Vectors[ARRAY_LENGTH(__VECTOR_TABLE)];
#endif

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    __disable_irq();
#endif
    __set_PSP((uint32_t)(&__INITIAL_SP));

    __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
    __set_PSPLIM((uint32_t)(&__STACK_LIMIT));

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

    SystemInit();                             /* CMSIS System Initialization */
    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
