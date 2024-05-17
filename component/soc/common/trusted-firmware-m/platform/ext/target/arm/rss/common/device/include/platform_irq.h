/*
 * Copyright (c) 2019-2022 Arm Limited. All rights reserved.
 *
 * Licensed under the Apache License Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing software
 * distributed under the License is distributed on an "AS IS" BASIS
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __PLATFORM_IRQ_H__
#define __PLATFORM_IRQ_H__

typedef enum _IRQn_Type {
    NonMaskableInt_IRQn                = -14,  /* Non Maskable Interrupt */
    HardFault_IRQn                     = -13,  /* HardFault Interrupt */
    MemoryManagement_IRQn              = -12,  /* Memory Management Interrupt */
    BusFault_IRQn                      = -11,  /* Bus Fault Interrupt */
    UsageFault_IRQn                    = -10,  /* Usage Fault Interrupt */
    SecureFault_IRQn                   = -9,   /* Secure Fault Interrupt */
    SVCall_IRQn                        = -5,   /* SV Call Interrupt */
    DebugMonitor_IRQn                  = -4,   /* Debug Monitor Interrupt */
    PendSV_IRQn                        = -2,   /* Pend SV Interrupt */
    SysTick_IRQn                       = -1,   /* System Tick Interrupt */
    NONSEC_WATCHDOG_RESET_REQ_IRQn     = 0,    /* Non-Secure Watchdog Reset
                                                * Request Interrupt
                                                */
    NONSEC_WATCHDOG_IRQn               = 1,    /* Non-Secure Watchdog Interrupt */
    SLOWCLK_TIMER_IRQn                 = 2,    /* SLOWCLK Timer Interrupt */
    TIMER0_IRQn                        = 3,    /* TIMER 0 Interrupt */
    TIMER1_IRQn                        = 4,    /* TIMER 1 Interrupt */
    TIMER2_IRQn                        = 5,    /* TIMER 2 Interrupt */
    /* Reserved                        = 6:8,     Reserved */
    MPC_IRQn                           = 9,    /* MPC Combined (Secure) Interrupt */
    PPC_IRQn                           = 10,   /* PPC Combined (Secure) Interrupt */
    MSC_IRQn                           = 11,   /* MSC Combined (Secure) Interrput */
    BRIDGE_ERROR_IRQn                  = 12,   /* Bridge Error Combined (Secure) Interrupt */
    /* Reserved                        = 13,      Reserved */
    PPU_Combined_IRQn                  = 14,   /* PPU Combined (Secure) Interrupt */
    /* Reserved                        = 15,      Reserved */
    NPU0_IRQn                          = 16,   /* NPU0 Interrupt */
    NPU1_IRQn                          = 17,   /* NPU1 Interrupt */
    NPU2_IRQn                          = 18,   /* NPU2 Interrupt */
    NPU3_IRQn                          = 19,   /* NPU3 Interrupt */
    KMU_S_IRQn                         = 20,   /* KMU (Secure) Interrupt */
    /* Reserved                        = 21:23,   Reserved */
    DMA_Combined_S_IRQn                = 24,   /* DMA350 Combined (Secure) Interrupt */
    DMA_Combined_NS_IRQn               = 25,   /* DMA350 Combined (Non-Secure) Interrupt */
    DMA_Security_Violation_IRQn        = 26,   /* DMA350 Security Violation Interrupt */
    TIMER3_AON_IRQn                    = 27,   /* TIMER 3 AON Interrupt */
    CPU0_CTI_0_IRQn                    = 28,   /* CPU0 CTI IRQ 0 Interrupt */
    CPU0_CTI_1_IRQn                    = 29,   /* CPU0 CTI IRQ 1 Interrupt */
    /* Reserved                        = 30:31,   Reserved */
    SAM_Critical_Sec_Fault_S_IRQn      = 32,   /* SAM Critical Security Fault (Secure) Interrupt */
    SAM_Sec_Fault_S_IRQn               = 33,   /* SAM Security Fault (Secure) Interrupt */
    GPIO_Combined_S_IRQn               = 34,   /* GPIO Combined (Secure) Interrupt */
    SDC_IRQn                           = 35,   /* Secure Debug channel Interrupt */
    FPU_IRQn                           = 36,   /* FPU Exceptions */
    SRAM_TRAM_ECC_Err_Combined_S_IRQn  = 37,
    /* SRAM and TRAM Corrected ECC Error Combined (Secure) Interrupt */
    SIC_S_IRQn                         = 38,   /* SICache (Secure) Interrupt */
    ATU_S_IRQn                         = 39,   /* ATU (Secure) Interrupt */
    CMU_MHU0_Sender_IRQn               = 40,   /* CMU MHU0 Sender Interrupt */
    CMU_MHU0_Receiver_IRQn             = 41,   /* CMU MHU0 Receiver Interrupt */
    CMU_MHU1_Sender_IRQn               = 42,   /* CMU MHU1 Sender Interrupt */
    CMU_MHU1_Receiver_IRQn             = 43,   /* CMU MHU1 Receiver Interrupt */
    CMU_MHU2_Sender_IRQn               = 44,   /* CMU MHU2 Sender Interrupt */
    CMU_MHU2_Receiver_IRQn             = 45,   /* CMU MHU2 Receiver Interrupt */
    CMU_MHU3_Sender_IRQn               = 46,   /* CMU MHU3 Sender Interrupt */
    CMU_MHU3_Receiver_IRQn             = 47,   /* CMU MHU3 Receiver Interrupt */
    CMU_MHU4_Sender_IRQn               = 48,   /* CMU MHU4 Sender Interrupt */
    CMU_MHU4_Receiver_IRQn             = 49,   /* CMU MHU4 Receiver Interrupt */
    CMU_MHU5_Sender_IRQn               = 50,   /* CMU MHU5 Sender Interrupt */
    CMU_MHU5_Receiver_IRQn             = 51,   /* CMU MHU5 Receiver Interrupt */
    CMU_MHU6_Sender_IRQn               = 52,   /* CMU MHU6 Sender Interrupt */
    CMU_MHU6_Receiver_IRQn             = 53,   /* CMU MHU6 Receiver Interrupt */
    CMU_MHU7_Sender_IRQn               = 54,   /* CMU MHU7 Sender Interrupt */
    CMU_MHU7_Receiver_IRQn             = 55,   /* CMU MHU7 Receiver Interrupt */
    CMU_MHU8_Sender_IRQn               = 56,   /* CMU MHU8 Sender Interrupt */
    CMU_MHU8_Receiver_IRQn             = 57,   /* CMU MHU8 Receiver Interrupt */
    Crypto_Engine_S_IRQn               = 58,   /* Crypto Engine (Secure) Interrupt */
    SoC_System_Timer0_AON_IRQn         = 59,   /* SoC System Timer 0 AON Interrupt */
    SoC_System_Timer1_AON_IRQn         = 60,   /* SoC System Timer 1 AON Interrupt */
    /* Reserved                        = 61:95,   Reserved */
} IRQn_Type;

#endif  /* __PLATFORM_IRQ_H__ */
