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
 * This file is derivative of CMSIS V5.9.0 startup_ARMCM23.c
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

DEFAULT_IRQ_HANDLER(NMI_Handler)
DEFAULT_IRQ_HANDLER(HardFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(SysTick_Handler)

DEFAULT_IRQ_HANDLER(BOD_IRQHandler)
DEFAULT_IRQ_HANDLER(IRC_IRQHandler)
DEFAULT_IRQ_HANDLER(PWRWU_IRQHandler)
DEFAULT_IRQ_HANDLER(SRAM_IRQHandler)
DEFAULT_IRQ_HANDLER(CLKFAIL_IRQHandler)

DEFAULT_IRQ_HANDLER(RTC_IRQHandler)
DEFAULT_IRQ_HANDLER(TAMPER_IRQHandler)
DEFAULT_IRQ_HANDLER(WDT_IRQHandler)
DEFAULT_IRQ_HANDLER(WWDT_IRQHandler)
DEFAULT_IRQ_HANDLER(EINT0_IRQHandler)
DEFAULT_IRQ_HANDLER(EINT1_IRQHandler)
DEFAULT_IRQ_HANDLER(EINT2_IRQHandler)
DEFAULT_IRQ_HANDLER(EINT3_IRQHandler)
DEFAULT_IRQ_HANDLER(EINT4_IRQHandler)
DEFAULT_IRQ_HANDLER(EINT5_IRQHandler)
DEFAULT_IRQ_HANDLER(GPA_IRQHandler)
DEFAULT_IRQ_HANDLER(GPB_IRQHandler)
DEFAULT_IRQ_HANDLER(GPC_IRQHandler)
DEFAULT_IRQ_HANDLER(GPD_IRQHandler)
DEFAULT_IRQ_HANDLER(GPE_IRQHandler)
DEFAULT_IRQ_HANDLER(GPF_IRQHandler)
DEFAULT_IRQ_HANDLER(QSPI0_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI0_IRQHandler)
DEFAULT_IRQ_HANDLER(BRAKE0_IRQHandler)
DEFAULT_IRQ_HANDLER(EPWM0_P0_IRQHandler)
DEFAULT_IRQ_HANDLER(EPWM0_P1_IRQHandler)
DEFAULT_IRQ_HANDLER(EPWM0_P2_IRQHandler)
DEFAULT_IRQ_HANDLER(BRAKE1_IRQHandler)
DEFAULT_IRQ_HANDLER(EPWM1_P0_IRQHandler)
DEFAULT_IRQ_HANDLER(EPWM1_P1_IRQHandler)
DEFAULT_IRQ_HANDLER(EPWM1_P2_IRQHandler)
DEFAULT_IRQ_HANDLER(TFM_TIMER0_IRQ_Handler)
DEFAULT_IRQ_HANDLER(TMR1_IRQHandler)
DEFAULT_IRQ_HANDLER(TIMER1_Handler) /*Just a handler to use TMR2 hardware */
DEFAULT_IRQ_HANDLER(TMR3_IRQHandler)
DEFAULT_IRQ_HANDLER(UART0_IRQHandler)
DEFAULT_IRQ_HANDLER(UART1_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C0_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_IRQHandler)
DEFAULT_IRQ_HANDLER(PDMA0_IRQHandler)
DEFAULT_IRQ_HANDLER(DAC_IRQHandler)
DEFAULT_IRQ_HANDLER(EADC0_IRQHandler)
DEFAULT_IRQ_HANDLER(EADC1_IRQHandler)
DEFAULT_IRQ_HANDLER(ACMP01_IRQHandler)
DEFAULT_IRQ_HANDLER(EADC2_IRQHandler)
DEFAULT_IRQ_HANDLER(EADC3_IRQHandler)
DEFAULT_IRQ_HANDLER(UART2_IRQHandler)
DEFAULT_IRQ_HANDLER(UART3_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI1_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI2_IRQHandler)
DEFAULT_IRQ_HANDLER(USBD_IRQHandler)
DEFAULT_IRQ_HANDLER(USBH_IRQHandler)
DEFAULT_IRQ_HANDLER(USBOTG_IRQHandler)
DEFAULT_IRQ_HANDLER(CAN0_IRQHandler)
DEFAULT_IRQ_HANDLER(SC0_IRQHandler)
DEFAULT_IRQ_HANDLER(SC1_IRQHandler)
DEFAULT_IRQ_HANDLER(SC2_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI3_IRQHandler)
DEFAULT_IRQ_HANDLER(SDH0_IRQHandler)
DEFAULT_IRQ_HANDLER(I2S0_IRQHandler)
DEFAULT_IRQ_HANDLER(OPA0_IRQHandler)
DEFAULT_IRQ_HANDLER(CRPT_IRQHandler)
DEFAULT_IRQ_HANDLER(GPG_IRQHandler)
DEFAULT_IRQ_HANDLER(EINT6_IRQHandler)
DEFAULT_IRQ_HANDLER(UART4_IRQHandler)
DEFAULT_IRQ_HANDLER(UART5_IRQHandler)
DEFAULT_IRQ_HANDLER(USCI0_IRQHandler)
DEFAULT_IRQ_HANDLER(USCI1_IRQHandler)
DEFAULT_IRQ_HANDLER(BPWM0_IRQHandler)
DEFAULT_IRQ_HANDLER(BPWM1_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C2_IRQHandler)
DEFAULT_IRQ_HANDLER(QEI0_IRQHandler)
DEFAULT_IRQ_HANDLER(QEI1_IRQHandler)
DEFAULT_IRQ_HANDLER(ECAP0_IRQHandler)
DEFAULT_IRQ_HANDLER(ECAP1_IRQHandler)
DEFAULT_IRQ_HANDLER(GPH_IRQHandler)
DEFAULT_IRQ_HANDLER(EINT7_IRQHandler)
DEFAULT_IRQ_HANDLER(PDMA1_IRQHandler)
DEFAULT_IRQ_HANDLER(SCU_IRQHandler)
DEFAULT_IRQ_HANDLER(TRNG_IRQHandler)

DEFAULT_IRQ_HANDLER(Default_Handler)


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
  Reset_Handler,                  /* Reset Handler */
  NMI_Handler,                    /* NMI Handler */
  HardFault_Handler,              /* Hard Fault Handler */
  0,                              /* Reserved */
  0,                              /* Reserved */
  0,                              /* Reserved */
  0,                              /* Reserved */
  0,                              /* Reserved */
  0,                              /* Reserved */
  0,                              /* Reserved */
  SVC_Handler,                    /* SVCall Handler */
  0,                              /* Reserved */
  0,                              /* Reserved */
  PendSV_Handler,                 /* PendSV Handler */
  SysTick_Handler,                /* SysTick Handler */

/* External interrupts */
  BOD_IRQHandler,        /* 0   */
  IRC_IRQHandler,        /* 1   */
  PWRWU_IRQHandler,      /* 2   */
  SRAM_IRQHandler,       /* 3   */
  CLKFAIL_IRQHandler,    /* 4   */
  Default_Handler,       /* 5   */
  RTC_IRQHandler,        /* 6   */
  TAMPER_IRQHandler,     /* 7   */
  WDT_IRQHandler,        /* 8   */
  WWDT_IRQHandler,       /* 9   */
  EINT0_IRQHandler,      /* 10  */
  EINT1_IRQHandler,      /* 11  */
  EINT2_IRQHandler,      /* 12  */
  EINT3_IRQHandler,      /* 13  */
  EINT4_IRQHandler,      /* 14  */
  EINT5_IRQHandler,      /* 15  */
  GPA_IRQHandler,        /* 16  */
  GPB_IRQHandler,        /* 17  */
  GPC_IRQHandler,        /* 18  */
  GPD_IRQHandler,        /* 19  */
  GPE_IRQHandler,        /* 20  */
  GPF_IRQHandler,        /* 21  */
  QSPI0_IRQHandler,      /* 22  */
  SPI0_IRQHandler,       /* 23  */
  BRAKE0_IRQHandler,     /* 24  */
  EPWM0_P0_IRQHandler,   /* 25  */
  EPWM0_P1_IRQHandler,   /* 26  */
  EPWM0_P2_IRQHandler,   /* 27  */
  BRAKE1_IRQHandler,     /* 28  */
  EPWM1_P0_IRQHandler,   /* 29  */
  EPWM1_P1_IRQHandler,   /* 30  */
  EPWM1_P2_IRQHandler,   /* 31  */
  TFM_TIMER0_IRQ_Handler,/* 32  */
  TMR1_IRQHandler,       /* 33  */
  TIMER1_Handler,        /* TMR2_IRQHandler 34  */
  TMR3_IRQHandler,       /* 35  */
  UART0_IRQHandler,      /* 36  */
  UART1_IRQHandler,      /* 37  */
  I2C0_IRQHandler,       /* 38  */
  I2C1_IRQHandler,       /* 39  */
  PDMA0_IRQHandler,      /* 40  */
  DAC_IRQHandler,        /* 41  */
  EADC0_IRQHandler,      /* 42  */
  EADC1_IRQHandler,      /* 43  */
  ACMP01_IRQHandler,     /* 44  */
  Default_Handler,       /* 45  */
  EADC2_IRQHandler,      /* 46  */
  EADC3_IRQHandler,      /* 47  */
  UART2_IRQHandler,      /* 48  */
  UART3_IRQHandler,      /* 49  */
  Default_Handler,       /* 50  */
  SPI1_IRQHandler,       /* 51  */
  SPI2_IRQHandler,       /* 52  */
  USBD_IRQHandler,       /* 53  */
  USBH_IRQHandler,       /* 54  */
  USBOTG_IRQHandler,     /* 55  */
  CAN0_IRQHandler,       /* 56  */
  Default_Handler,       /* 57  */
  SC0_IRQHandler,        /* 58  */
  SC1_IRQHandler,        /* 59  */
  SC2_IRQHandler,        /* 60  */
  Default_Handler,       /* 61  */
  SPI3_IRQHandler,       /* 62  */
  Default_Handler,       /* 63  */
  SDH0_IRQHandler,       /* 64  */
  Default_Handler,       /* 65  */
  Default_Handler,       /* 66  */
  Default_Handler,       /* 67  */
  I2S0_IRQHandler,       /* 68  */
  Default_Handler,       /* 69  */
  OPA0_IRQHandler,       /* 70  */
  CRPT_IRQHandler,       /* 71  */
  GPG_IRQHandler,        /* 72  */
  EINT6_IRQHandler,      /* 73  */
  UART4_IRQHandler,      /* 74  */
  UART5_IRQHandler,      /* 75  */
  USCI0_IRQHandler,      /* 76  */
  USCI1_IRQHandler,      /* 77  */
  BPWM0_IRQHandler,      /* 78  */
  BPWM1_IRQHandler,      /* 79  */
  Default_Handler,       /* 80  */
  Default_Handler,       /* 81  */
  I2C2_IRQHandler,       /* 82  */
  Default_Handler,       /* 83  */
  QEI0_IRQHandler,       /* 84  */
  QEI1_IRQHandler,       /* 85  */
  ECAP0_IRQHandler,      /* 86  */
  ECAP1_IRQHandler,      /* 87  */
  GPH_IRQHandler,        /* 88  */
  EINT7_IRQHandler,      /* 89  */
  Default_Handler,       /* 90  */
  Default_Handler,       /* 91  */
  Default_Handler,       /* 92  */
  Default_Handler,       /* 93  */
  Default_Handler,       /* 94  */
  Default_Handler,       /* 95  */
  Default_Handler,       /* 96  */
  Default_Handler,       /* 97  */
  PDMA1_IRQHandler,      /* 98  */
  SCU_IRQHandler,        /* 99  */
  Default_Handler,       /* 100 */
  TRNG_IRQHandler,       /* 101 */
};

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

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
    __set_PSPLIM((uint32_t)(&__STACK_LIMIT));

    __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

    SystemInit();                             /* CMSIS System Initialization */
    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
