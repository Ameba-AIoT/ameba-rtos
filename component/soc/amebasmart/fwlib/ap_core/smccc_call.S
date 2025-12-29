/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */
#include "asm_defs.h"

	.macro SMCCC_SMC
	smc #0
	.endm

	.macro SMCCC instr
	push	{r4-r7}
	add r4, sp, #(4 * 4)
	ldm     r4, {r4-r7}
	\instr
	ldr     r4, [sp, #(8 * 4)]
	stm     r4, {r0-r3}
	pop	{r4-r7}
	bx	lr
	.endm

/*
 * void smccc_smc(unsigned long a0, unsigned long a1, unsigned long a2,
 *		  unsigned long a3, unsigned long a4, unsigned long a5,
 *		  unsigned long a6, unsigned long a7, struct arm_smccc_res *res,
 *		  struct arm_smccc_quirk *quirk)
 */
	.__func_arm_thumb
	.global __arm_smccc_smc
	.type __arm_smccc_smc, %function
__arm_smccc_smc:
	SMCCC SMCCC_SMC
