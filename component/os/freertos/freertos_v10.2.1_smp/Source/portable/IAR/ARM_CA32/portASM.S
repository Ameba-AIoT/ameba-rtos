/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */
	/* Variables and functions. */
	EXTERN ulMaxAPIPriorityMask
	EXTERN _freertos_vector_table
	EXTERN pxCurrentTCBs
        EXTERN ulPortYieldRequired
	EXTERN vTaskSwitchContext
	EXTERN vApplicationIRQHandler
	EXTERN ulPortInterruptNesting
	EXTERN ulPortTaskHasFPUContext
        EXTERN vApplicationFPUSafeIRQHandler

	PUBLIC FreeRTOS_IRQ_Handler
	PUBLIC FreeRTOS_SWI_Handler
	PUBLIC vPortRestoreTaskContext


SYS_MODE			EQU		0x1f
SVC_MODE			EQU		0x13
IRQ_MODE			EQU		0x12


	SECTION .text:CODE:ROOT(2)
	ARM
	INCLUDE portASM.h
        
portSAVE_CONTEXT macro

	/* Save the LR and SPSR onto the system mode stack before switching to
	system mode to save the remaining system mode registers. */
	SRSDB	sp!, #SYS_MODE
	CPS		#SYS_MODE
	PUSH	{R0-R12, R14}

	/* Does the task have a floating point context that needs saving?  If
	ulPortTaskHasFPUContext is 0 then no. */
	LDR		R2, ulPortTaskHasFPUContextConst
	LDR		R3, [R2]
	CMP		R3, #0

	/* Save the floating point context, if any. */
	FMRXNE  R1,  FPSCR
	VPUSHNE {D0-D15}
	VPUSHNE	{D16-D31}
	PUSHNE	{R1}

	/* Save ulPortTaskHasFPUContext itself. */
	PUSH	{R3}

	/* Save the stack pointer in the TCB. r0 holds the value for core id */
	MRC		P15, 0, R0, C0, C0, 5
	AND		R0, R0, #0xFF
	LDR		R1, pxCurrentTCBConst
	LDR		R2, [R1, R0, LSL #2]
	STR		SP, [R2]

	endm

; /**********************************************************************/

portRESTORE_CONTEXT macro

	/* Set the SP to point to the stack of the task being restored. */
	MRC		P15, 0, R0, C0, C0, 5
	AND		R0, R0, #0xFF
	LDR		R1, pxCurrentTCBConst
	LDR		R2, [R1, R0, LSL #2]
	LDR		SP, [R2]

	/* Is there a floating point context to restore?  If the restored
	ulPortTaskHasFPUContext is zero then no. */
	LDR		R0, ulPortTaskHasFPUContextConst
	POP		{R1}
	STR		R1, [R0]
	CMP		R1, #0

	/* Restore the floating point context, if any. */
	POPNE 	{R0}
	VPOPNE	{D16-D31}
	VPOPNE	{D0-D15}
	VMSRNE  FPSCR, R0

	/* Restore all system mode registers other than the SP (which is already
	being used). */
	POP		{R0-R12, R14}

	/* Return to the task code, loading CPSR on the way. */
	RFEIA	sp!

	endm
        
        //SECTION .image33asm:CODE:ROOT(2)
	//ARM
        
/******************************************************************************
 * vPortRestoreTaskContext is used to start the scheduler.
 *****************************************************************************/
vPortRestoreTaskContext:
	/* Switch to system mode. */
	CPS		#SYS_MODE
	MRC		P15, 0, R0, C0, C0, 5
	AND		R0, R0, #0xFF
	LDR		R1, vTaskSwitchContextConst
	BLX		R1
	portRESTORE_CONTEXT
        
        
/******************************************************************************
 * SVC handler is used to start the scheduler.
 *****************************************************************************/
FreeRTOS_SWI_Handler:
	/* Save the context of the current task and select a new task to run. */
	portSAVE_CONTEXT
	LDR		R1, vTaskSwitchContextConst
	BLX		R1
	portRESTORE_CONTEXT

FreeRTOS_IRQ_Handler:
	/* Return to the interrupted instruction. */
	SUB		LR, LR, #4

	/* Push the return address and SPSR. */
	PUSH	{LR}
	MRS		LR, SPSR
	PUSH	{LR}

	/* Push used registers. */
	PUSH	{R0-R4, R12}

	/* Increment nesting count.  r3 holds the address of ulPortInterruptNesting
	for future use.  r1 holds the original ulPortInterruptNesting value for
	future use. */
	MRC 	P15, 0, R0, C0, C0, 5
	AND 	R2, R0, #0xFF
	LDR 	R3, =ulPortInterruptNesting
	LDR 	R1, [R3, R2, LSL #2]
	ADD 	R4, R1, #1
	STR 	R4, [R3, R2, LSL #2]

	/* Change to supervisor mode to allow reentry. */
	CPS		#SVC_MODE

	/* Ensure bit 2 of the stack pointer is clear.  r2 holds the bit 2 value for
	future use.  _RB_ Does this ever actually need to be done provided the start
	of the stack is 8-byte aligned? */
	MOV		R2, SP
	AND		R2, R2, #4
	SUB		SP, SP, R2

	/* Call the interrupt handler.  r4 pushed to maintain alignment. */
	PUSH	{R0-R4, LR}
	LDR		R1, vApplicationIRQHandlerConst
	BLX		R1
	POP		{R0-R4, LR}
	ADD		SP, SP, R2

	/* Change to IRQ mode and pop IRQ stack. */
	CPS		#IRQ_MODE

	/* ulPortInterruptNesting-- */
	MRC		P15, 0, R0, C0, C0, 5
	AND		R2, R0, #0xFF
	LDR		R3, =ulPortInterruptNesting
	LDR		R1, [R3, R2, LSL #2]
	SUB		R4, R1, #1
	STR		R4, [R3, R2, LSL #2]

	/* A context switch is never performed if the nesting count is not 0. */
	CMP             R4, #0
	BNE             exit_without_switch

	/* Did the interrupt request a context switch?  r1 holds the address of
	ulPortYieldRequired and r0 the value of ulPortYieldRequired for future
	use. */
	MRC		P15, 0, R0, C0, C0, 5
	AND		R2, R0, #0xFF
	LDR		R1, =ulPortYieldRequired
	LDR		R0, [R1, R2, LSL #2]
	CMP		R0, #0
	BNE		switch_before_exit

exit_without_switch:
	/* No context switch.  Restore used registers, LR_irq and SPSR before
	returning. */
	POP		{R0-R4, R12}
	POP		{LR}
	MSR		SPSR_cxsf, LR
	POP		{LR}
	MOVS	PC, LR

switch_before_exit:
	/* A context swtich is to be performed.  Clear the context switch pending
	flag. */
	MOV		R0, #0
	STR		R0, [R1, R2, LSL #2]

	/* Restore used registers, LR-irq and SPSR before saving the context
	to the task stack. */
	POP		{R0-R4, R12}
	POP		{LR}
	MSR		SPSR_cxsf, LR
	POP		{LR}
	portSAVE_CONTEXT

	CPS		#IRQ_MODE

	/* Call the function that selects the new task to execute.
	vTaskSwitchContext() if vTaskSwitchContext() uses LDRD or STRD
	instructions, or 8 byte aligned stack allocated data.  LR does not need
	saving as a new LR will be loaded by portRESTORE_CONTEXT anyway. */
	LDR		R1, vTaskSwitchContextConst
	BLX		R1

	CPS		#SYS_MODE

	/* Restore the context of, and branch to, the task selected to execute
	next. */
	portRESTORE_CONTEXT


/******************************************************************************
 * If the application provides an implementation of vApplicationIRQHandler(),
 * then it will get called directly without saving the FPU registers on
 * interrupt entry, and this weak implementation of
 * vApplicationIRQHandler() will not get called.
 *
 * If the application provides its own implementation of
 * vApplicationFPUSafeIRQHandler() then this implementation of
 * vApplicationIRQHandler() will be called, save the FPU registers, and then
 * call vApplicationFPUSafeIRQHandler().
 *
 * Therefore, if the application writer wants FPU registers to be saved on
 * interrupt entry their IRQ handler must be called
 * vApplicationFPUSafeIRQHandler(), and if the application writer does not want
 * FPU registers to be saved on interrupt entry their IRQ handler must be
 * called vApplicationIRQHandler().

WEAK vApplicationIRQHandler:
	PUSH	{LR}
	FMRX	R1,  FPSCR
	VPUSH	{D0-D15}
	VPUSH	{D16-D31}
	PUSH	{R1}

	LDR		R1, vApplicationFPUSafeIRQHandler
	BLX		R1

	POP		{R0}
	VPOP	{D16-D31}
	VPOP	{D0-D15}
	VMSR	FPSCR, R0

	POP {PC}
 *****************************************************************************/
pxCurrentTCBConst DCD pxCurrentTCBs
ulPortTaskHasFPUContextConst DCD ulPortTaskHasFPUContext
vTaskSwitchContextConst DCD vTaskSwitchContext
vApplicationIRQHandlerConst DCD vApplicationIRQHandler
vApplicationFPUSafeIRQHandlerConst DCD vApplicationFPUSafeIRQHandler
        END





