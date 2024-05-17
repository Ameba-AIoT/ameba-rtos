/*
 * Copyright (c) 2022-2023 Arm Limited. All rights reserved.
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
#if defined(TEST_NS_FPU) || defined(TEST_S_FPU)
#include "test_interrupt.h"
#endif
#include <string.h>
#include "sysreg_lsys.h"
#include "sysreg_aon.h"

/*----------------------------------------------------------------------------
  External References
 *----------------------------------------------------------------------------*/
extern uint32_t __INITIAL_SP;
extern uint32_t __STACK_LIMIT;
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
extern uint64_t __STACK_SEAL;
#endif

extern void __PROGRAM_START(void) __NO_RETURN;
extern int main(void) __NO_RETURN;
extern void SOCPS_Core_ReFill(void);

/*----------------------------------------------------------------------------
  Internal References
 *----------------------------------------------------------------------------*/
void Reset_Handler(void) __NO_RETURN;

__section(".psa_bss_un_init")
u8 DerivedKey_Bkup[16];

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
DEFAULT_IRQ_HANDLER(MemManage_Handler)
DEFAULT_IRQ_HANDLER(BusFault_Handler)
DEFAULT_IRQ_HANDLER(UsageFault_Handler)
DEFAULT_IRQ_HANDLER(SecureFault_Handler)
DEFAULT_IRQ_HANDLER(SVC_Handler)
DEFAULT_IRQ_HANDLER(DebugMon_Handler)
DEFAULT_IRQ_HANDLER(PendSV_Handler)
DEFAULT_IRQ_HANDLER(SysTick_Handler)


#if defined(TEST_NS_FPU) || defined(TEST_S_FPU)
#define UART0_Combined_Handler TFM_FPU_S_TEST_Handler
#else
DEFAULT_IRQ_HANDLER(UART0_Combined_Handler)
#endif

/*----------------------------------------------------------------------------
  Exception / Interrupt Vector table
 *----------------------------------------------------------------------------*/
#ifdef AMEBA_NONEED
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
	0,                                 /*   8: Reserved */
	MPC_Handler,                       /*   9: MPC Combined (Secure) Handler */
	PPC_Handler,                       /*  10: PPC Combined (Secure) Handler */
	MSC_Handler,                       /*  11: MSC Combined (Secure) Handler */
	BRIDGE_ERROR_Handler,              /*  12: Bridge Error (Secure) Handler */
	INVALID_INSTR_CACHE_Handler,       /*  13: CPU Instruction Cache Invalidation Handler */
};

#if defined ( __GNUC__ )
#pragma GCC diagnostic pop
#endif

#endif

u32 BOOT_Reason(void)
{
	return HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW);
}

/*----------------------------------------------------------------------------
  Reset Handler called on controller reset
 *----------------------------------------------------------------------------*/
void Reset_Handler(void)
{
#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
	__disable_irq();
#endif
	__set_MSP((uint32_t)(&__INITIAL_SP));
	__set_PSP((uint32_t)(&__INITIAL_SP));

	__set_MSPLIM((uint32_t)(&__STACK_LIMIT));
	__set_PSPLIM((uint32_t)(&__STACK_LIMIT));


#if defined (__ARM_FEATURE_CMSE) && (__ARM_FEATURE_CMSE == 3U)
	__TZ_set_STACKSEAL_S((uint32_t *)(&__STACK_SEAL));
#endif

	__NVIC_SetVector(SVCall_IRQn, (uint32_t)SVC_Handler);
	__NVIC_SetVector(PendSV_IRQn, (uint32_t)PendSV_Handler);

#if 0
	__PROGRAM_START();         /* Enter PreMain (C library entry point) -> __cmsis_start -> _mainCRTStartup(call __libc_init_array/main) */
#else

	typedef struct {
		uint32_t *dest;
		uint32_t	wlen;
	} __zero_table_t;

	extern const __zero_table_t __tfm_zero_table_start__;
	extern const __zero_table_t __tfm_zero_table_end__;

	for (__zero_table_t const *pTable = &__tfm_zero_table_start__; pTable < &__tfm_zero_table_end__; ++pTable) {
		for (uint32_t i = 0u; i < pTable->wlen; ++i) {
			pTable->dest[i] = 0u;
		}
	}

	if ((BOOT_Reason() == 0) || (BOOT_Reason() == AON_BIT_RSTF_DSLP)) {
		memcpy(DerivedKey_Bkup, DerivedKey, sizeof(DerivedKey));
	}
	main();
#endif
}

typedef struct {
	void (*RamStartFun)(void);
	void (*RamWakeupFun)(void);
	uint32_t Manifest_Addr;
} RAM_START_FUNCTION, *PRAM_START_FUNCTION;

RAM_START_FUNCTION TFMEntryFun __VECTOR_TABLE_ATTRIBUTE = {
	Reset_Handler,
	SOCPS_Core_ReFill,
	(uint32_t)0
};

uint32_t tfm_core_get_Manifest_Addr(void)
{
	return TFMEntryFun.Manifest_Addr;
}

