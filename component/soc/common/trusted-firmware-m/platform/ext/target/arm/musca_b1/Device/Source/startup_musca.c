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

DEFAULT_IRQ_HANDLER(NONSEC_WATCHDOG_RESET_Handler)
DEFAULT_IRQ_HANDLER(NONSEC_WATCHDOG_Handler)
DEFAULT_IRQ_HANDLER(S32K_TIMER_Handler)
DEFAULT_IRQ_HANDLER(TFM_TIMER0_IRQ_Handler)
DEFAULT_IRQ_HANDLER(TIMER1_Handler)
DEFAULT_IRQ_HANDLER(DUALTIMER_Handler)
DEFAULT_IRQ_HANDLER(MHU0_Handler)
DEFAULT_IRQ_HANDLER(MHU1_Handler)
DEFAULT_IRQ_HANDLER(CRYPTOCELL_Handler)
DEFAULT_IRQ_HANDLER(MPC_Handler)
DEFAULT_IRQ_HANDLER(PPC_Handler)
DEFAULT_IRQ_HANDLER(MSC_Handler)
DEFAULT_IRQ_HANDLER(BRIDGE_ERROR_Handler)
DEFAULT_IRQ_HANDLER(INVALID_INSTR_CACHE_Handler)
DEFAULT_IRQ_HANDLER(SYS_PPU_Handler)
DEFAULT_IRQ_HANDLER(CPU0_PPU_Handler)
DEFAULT_IRQ_HANDLER(CPU1_PPU_Handler)
DEFAULT_IRQ_HANDLER(CPU0_DBG_PPU_Handler)
DEFAULT_IRQ_HANDLER(CPU1_DBG_PPU_Handler)
DEFAULT_IRQ_HANDLER(CRYPT_PPU_Handler)
DEFAULT_IRQ_HANDLER(RAM0_PPU_Handler)
DEFAULT_IRQ_HANDLER(RAM1_PPU_Handler)
DEFAULT_IRQ_HANDLER(RAM2_PPU_Handler)
DEFAULT_IRQ_HANDLER(RAM3_PPU_Handler)
DEFAULT_IRQ_HANDLER(DBG_PPU_Handler)
DEFAULT_IRQ_HANDLER(CPU0_CTI_Handler)
DEFAULT_IRQ_HANDLER(CPU1_CTI_Handler)

DEFAULT_IRQ_HANDLER(GpTimer_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C0_IRQHandler)
DEFAULT_IRQ_HANDLER(I2C1_IRQHandler)
DEFAULT_IRQ_HANDLER(I2S_IRQHandler)
DEFAULT_IRQ_HANDLER(SPI_IRQHandler)
DEFAULT_IRQ_HANDLER(QSPI_IRQHandler)
DEFAULT_IRQ_HANDLER(UARTRX0_Handler)
DEFAULT_IRQ_HANDLER(UARTTX0_Handler)
DEFAULT_IRQ_HANDLER(UART0_RxTimeout_IRQHandler)
DEFAULT_IRQ_HANDLER(UART0_ModemStatus_IRQHandler)
DEFAULT_IRQ_HANDLER(UART0_Error_IRQHandler)
DEFAULT_IRQ_HANDLER(UART0_IRQHandler)
DEFAULT_IRQ_HANDLER(UARTRX1_Handler)
DEFAULT_IRQ_HANDLER(UARTTX1_Handler)
DEFAULT_IRQ_HANDLER(UART1_RxTimeout_IRQHandler)
DEFAULT_IRQ_HANDLER(UART1_ModemStatus_IRQHandler)
DEFAULT_IRQ_HANDLER(UART1_Error_IRQHandler)
DEFAULT_IRQ_HANDLER(UART1_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_0_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_1_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_2_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_3_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_4_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_5_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_6_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_7_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_8_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_9_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_10_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_11_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_12_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_13_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_14_IRQHandler)
DEFAULT_IRQ_HANDLER(GPIO_15_IRQHandler)
DEFAULT_IRQ_HANDLER(Combined_IRQHandler)
DEFAULT_IRQ_HANDLER(PVT_IRQHandler)
DEFAULT_IRQ_HANDLER(PWM_0_IRQHandler)
DEFAULT_IRQ_HANDLER(RTC_IRQHandler)
DEFAULT_IRQ_HANDLER(GpTimer0_IRQHandler)
DEFAULT_IRQ_HANDLER(GpTimer1_IRQHandler)
DEFAULT_IRQ_HANDLER(PWM_1_IRQHandler)
DEFAULT_IRQ_HANDLER(PWM_2_IRQHandler)
DEFAULT_IRQ_HANDLER(IOMUX_IRQHandler)

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
  Reset_Handler,                     /*      Reset Handler */
  NMI_Handler,                       /* -14: NMI Handler */
  HardFault_Handler,                 /* -13: Hard Fault Handler */
  MemManage_Handler,                 /* -12: MPU Fault Handler */
  BusFault_Handler,                  /* -11: Bus Fault Handler */
  UsageFault_Handler,                /* -10: Usage Fault Handler */
  SecureFault_Handler,               /*  -9: Secure Fault Handler */
  0,                                 /*  -8: Reserved */
  0,                                 /*  -7: Reserved */
  0,                                 /*  -6: Reserved */
  SVC_Handler,                       /*  -5: SVCall Handler */
  DebugMon_Handler,                  /*  -4: Debug Monitor Handler */
  0,                                 /*  -3: Reserved */
  PendSV_Handler,                    /*  -2: PendSV Handler */
  SysTick_Handler,                   /*  -1: SysTick Handler */

  NONSEC_WATCHDOG_RESET_Handler,     /*   0: Non-Secure Watchdog Reset Request Handler */
  NONSEC_WATCHDOG_Handler,           /*   1: Non-Secure Watchdog Interrupt Handler */
  S32K_TIMER_Handler,                /*   2: S32K Timer Handler */
  TFM_TIMER0_IRQ_Handler,            /*   3: TIMER 0 Handler */
  TIMER1_Handler,                    /*   4: TIMER 1 Handler */
  DUALTIMER_Handler,                 /*   5: Dual Timer Handler */
  MHU0_Handler,                      /*   6: Message Handling Unit 0 */
  MHU1_Handler,                      /*   7: Message Handling Unit 1 */
  CRYPTOCELL_Handler,                /*   8: CryptoCell-312 Handler */
  MPC_Handler,                       /*   9: MPC Combined (Secure) Handler */
  PPC_Handler,                       /*  10: PPC Combined (Secure) Handler */
  MSC_Handler,                       /*  11: MSC Combined (Secure) Handler */
  BRIDGE_ERROR_Handler,              /*  12: Bridge Error (Secure) Handler */
  INVALID_INSTR_CACHE_Handler,       /*  13: CPU Instruction Cache Invalidation Handler */
  0,                                 /*  14: Reserved */
  SYS_PPU_Handler,                   /*  15: SYS PPU Handler */
  CPU0_PPU_Handler,                  /*  16: CPU0 PPU Handler */
  CPU1_PPU_Handler,                  /*  17: CPU1 PPU Handler */
  CPU0_DBG_PPU_Handler,              /*  18: CPU0 DBG PPU_Handler */
  CPU1_DBG_PPU_Handler,              /*  19: CPU1 DBG PPU_Handler */
  CRYPT_PPU_Handler,                 /*  20: CRYPT PPU Handler */
  0,                                 /*  21: Reserved */
  RAM0_PPU_Handler,                  /*  22: RAM0 PPU Handler */
  RAM1_PPU_Handler,                  /*  23: RAM1 PPU Handler */
  RAM2_PPU_Handler,                  /*  24: RAM2 PPU Handler */
  RAM3_PPU_Handler,                  /*  25: RAM3 PPU Handler */
  DBG_PPU_Handler,                   /*  26: DBG PPU Handler */
  0,                                 /*  27: Reserved */
  CPU0_CTI_Handler,                  /*  28: CPU0 CTI Handler */
  CPU1_CTI_Handler,                  /*  29: CPU1 CTI Handler */
  0,                                 /*  30: Reserved */
  0,                                 /*  31: Reserved */

/* External interrupts */
  0,                                 /* 32: Reserved */
  GpTimer_IRQHandler,                /* 33: General Purpose Timer */
  I2C0_IRQHandler,                   /* 34: I2C0 */
  I2C1_IRQHandler,                   /* 35: I2C1 */
  I2S_IRQHandler,                    /* 36: I2S */
  SPI_IRQHandler,                    /* 37: SPI */
  QSPI_IRQHandler,                   /* 38: QSPI */
  UARTRX0_Handler,                   /* 39: UART0 receive FIFO interrupt */
  UARTTX0_Handler,                   /* 40: UART0 transmit FIFO interrupt */
  UART0_RxTimeout_IRQHandler,        /* 41: UART0 receive timeout interrupt */
  UART0_ModemStatus_IRQHandler,      /* 42: UART0 modem status interrupt */
  UART0_Error_IRQHandler,            /* 43: UART0 error interrupt */
  UART0_IRQHandler,                  /* 44: UART0 interrupt */
  UARTRX1_Handler,                   /* 45: UART0 receive FIFO interrupt */
  UARTTX1_Handler,                   /* 46: UART0 transmit FIFO interrupt */
  UART1_RxTimeout_IRQHandler,        /* 47: UART0 receive timeout interrupt */
  UART1_ModemStatus_IRQHandler,      /* 48: UART0 modem status interrupt */
  UART1_Error_IRQHandler,            /* 49: UART0 error interrupt */
  UART1_IRQHandler,                  /* 50: UART0 interrupt */
  GPIO_0_IRQHandler,                 /* 51: GPIO 0 interrupt */
  GPIO_1_IRQHandler,                 /* 52: GPIO 1 interrupt */
  GPIO_2_IRQHandler,                 /* 53: GPIO 2 interrupt */
  GPIO_3_IRQHandler,                 /* 54: GPIO 3 interrupt */
  GPIO_4_IRQHandler,                 /* 55: GPIO 4 interrupt */
  GPIO_5_IRQHandler,                 /* 56: GPIO 5 interrupt */
  GPIO_6_IRQHandler,                 /* 57: GPIO 6 interrupt */
  GPIO_7_IRQHandler,                 /* 58: GPIO 7 interrupt */
  GPIO_8_IRQHandler,                 /* 59: GPIO 8 interrupt */
  GPIO_9_IRQHandler,                 /* 60: GPIO 9 interrupt */
  GPIO_10_IRQHandler,                /* 61: GPIO 10 interrupt */
  GPIO_11_IRQHandler,                /* 62: GPIO 11 interrupt */
  GPIO_12_IRQHandler,                /* 63: GPIO 12 interrupt */
  GPIO_13_IRQHandler,                /* 64: GPIO 13 interrupt */
  GPIO_14_IRQHandler,                /* 65: GPIO 14 interrupt */
  GPIO_15_IRQHandler,                /* 66: GPIO 15 interrupt */
  Combined_IRQHandler,               /* 67: Combined interrupt */
  PVT_IRQHandler,                    /* 68: PVT sensor interrupt */
  0,                                 /* 69: Reserved */
  PWM_0_IRQHandler,                  /* 70: PWM0 interrupt */
  RTC_IRQHandler,                    /* 71: RTC interrupt */
  GpTimer0_IRQHandler,               /* 72: General Purpose Timer0 */
  GpTimer1_IRQHandler,               /* 73: General Purpose Timer1 */
  PWM_1_IRQHandler,                  /* 74: PWM1 interrupt */
  PWM_2_IRQHandler,                  /* 75: PWM2 interrupt */
  IOMUX_IRQHandler,                  /* 76: IOMUX interrupt */
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

    __set_MSPLIM((uint32_t)(&__STACK_LIMIT));
    __set_PSPLIM((uint32_t)(&__STACK_LIMIT));

#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
    __TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

    SystemInit();                             /* CMSIS System Initialization */
    __PROGRAM_START();                        /* Enter PreMain (C library entry point) */
}
