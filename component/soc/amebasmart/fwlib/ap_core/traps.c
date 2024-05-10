/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

#include <stdio.h>
#include "basic_types.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"

uint32_t UndefinedExceptionAddr;
uint32_t DataAbortAddr;
uint32_t PrefetchAbortAddr;

/* Stringify preprocessors, no leading underscore. ('STRINGIFY') */
#define STRINGIFY_EXPAND(x) #x
#define M2S(m) STRINGIFY_EXPAND(m)
#define CONFIG_STACK_DUMP_DEPTH 	 128
// ARM v8M register
enum {
	REG_R4,
	REG_R5,
	REG_R6,
	REG_R7,
	REG_R8,
	REG_R9,
	REG_R10,
	REG_R11,
	REG_R0,
	REG_R1,
	REG_R2,
	REG_R3,
	REG_R12,
	REG_LR,
	REG_END
};

const char *const crash_reg_name[] = {
	[REG_R4]   = "R4",
	[REG_R5]   = "R5",
	[REG_R6]   = "R6",
	[REG_R7]   = "R7",
	[REG_R8]   = "R8",
	[REG_R9]   = "R9",
	[REG_R10]  = "R10",
	[REG_R11]  = "R11",
	[REG_R0]   = "R0",
	[REG_R1]   = "R1",
	[REG_R2]   = "R2",
	[REG_R3]   = "R3",
	[REG_R12]  = "R12",
	[REG_LR]   = "LR",
};

_WEAK void IRQHandler(void)
{
	for (;;);
}

_WEAK void FIQHandler(void)
{
	for (;;);
}

_WEAK void Crash_GetExStack(uint32_t spsr, uint32_t *pExSP, uint32_t *pExLR)
{
	uint32_t except_stack, except_lr;
	uint32_t cpsr = __get_CPSR();
	//	__set_mode(spsr & CPSR_M_Msk);
	switch (spsr & CPSR_M_Msk) {
	case CPSR_M_USR:
		__asm volatile("CPS #"M2S(CPSR_M_USR) ::: "memory");
		break;
	case CPSR_M_IRQ:
		__asm volatile("CPS #"M2S(CPSR_M_IRQ) ::: "memory");
		break;
	case CPSR_M_SVC:
		__asm volatile("CPS #"M2S(CPSR_M_SVC) ::: "memory");
		break;
	case CPSR_M_ABT:
		__asm volatile("CPS #"M2S(CPSR_M_ABT) ::: "memory");
		break;
	case CPSR_M_UND:
		__asm volatile("CPS #"M2S(CPSR_M_UND) ::: "memory");
		break;
	default:
		__asm volatile("CPS #"M2S(CPSR_M_SYS) ::: "memory");
		break;
	}

	__asm volatile("MOV %0, SP" : "=r"(except_stack) :: "memory");
	__asm volatile("MOV %0, LR" : "=r"(except_lr) :: "memory");
	__set_CPSR(cpsr);
	__DSB();
	__ISB();

	*pExSP = except_stack;
	*pExLR = except_lr;
}

/* Actual is DiagPrintf */
_WEAK void Crash_DumpReg(uint32_t cstack[], uint32_t spsr)
{
	uint32_t idx;
	u32 xCoreID = portGET_CORE_ID();

	/* 1. R0 - R3, R12, LR. */
	for (idx = REG_R0; idx <= REG_LR; idx++) {
		printf("Core[%lx] %s = 0x%08lx\n", xCoreID, crash_reg_name[idx], cstack[idx]);
	}

	/* 2.R4-R11 have been push into current SP. */
	for (idx = REG_R4; idx <= REG_R11; idx++) {
		printf("Core[%lx] %s = 0x%08lx\n", xCoreID, crash_reg_name[idx], cstack[idx]);
	}

	/* 3. Print CPSR/LR value. */
	printf("Core[%lx] Previous Mode's CPSR is 0x%08lx\n", xCoreID, (u32)spsr);
	u32 except_stack, except_lr;
	Crash_GetExStack(spsr, &except_stack, &except_lr);
	printf("Core[%lx] Previous Mode's LR is 0x%08lx\n", xCoreID, except_lr);

	/* 4. dump stack. */
	int depth = CONFIG_STACK_DUMP_DEPTH;
	int i = 0;
	u32 *sp = (uint32_t *)except_stack;
	printf("Core[%lx] Current Stack Pointer = 0x%08lx, and dump stack depth = %d\r\n", xCoreID, (u32)sp, depth);

	while (depth > i) {
		printf("Core[%lx] [%08lx] 0x%08lx 0x%08lx 0x%08lx 0x%08lx\n", xCoreID, (u32)(sp + i), sp[i], sp[i + 1], sp[i + 2], sp[i + 3]);
		i += 4;
	}

	DCache_Clean((uint32_t)sp, depth);
}

_WEAK void UndefinedExceptionHandler(uint32_t cstack[], uint32_t spsr)
{
	printf("Address of the undefined instruction 0x%08x\n", (unsigned int)UndefinedExceptionAddr);

	Crash_DumpReg(cstack, spsr);
	for (;;);
}

_WEAK void DataAbortHandler(uint32_t cstack[], uint32_t spsr)
{
	uint32_t FaultStatus;

	asm volatile("mrc	p15, 0, %0, c5, c0, 0" : "=r"(FaultStatus));

	printf("Data abort with Data Fault Status Register  0x%08x\n", (unsigned int)FaultStatus);
	printf("Address of Instruction causing Data abort 0x%08x\n", (unsigned int)DataAbortAddr);

	Crash_DumpReg(cstack, spsr);
	for (;;);
}

_WEAK void PrefetchAbortHandler(uint32_t cstack[], uint32_t spsr)
{
	uint32_t FaultStatus;

	asm volatile("mrc	p15, 0, %0, c5, c0, 1" : "=r"(FaultStatus));

	printf("Prefetch abort with Instruction Fault Status Register 0x%08x\n", (unsigned int)FaultStatus);
	printf("Address of Instruction causing Data abort 0x%08x\n", (unsigned int)PrefetchAbortAddr);

	Crash_DumpReg(cstack, spsr);
	for (;;);
}

