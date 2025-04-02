/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM0 port.
 *----------------------------------------------------------*/

#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "log.h"

/* NVIC systick includes */
#include "ameba.h"

/* PMC includes */
#include "ameba_pmu.h"
#include "cmsis_gcc.h"

/* Constants required to manipulate the NVIC. */
#define portNVIC_SYSTICK_CTRL			( ( volatile uint32_t * ) 0xe000e010 )
#define portNVIC_SYSTICK_LOAD			( ( volatile uint32_t * ) 0xe000e014 )
#define portNVIC_SYSTICK_CURRENT_VALUE	( ( volatile uint32_t * ) 0xe000e018 )
#define portNVIC_INT_CTRL				( ( volatile uint32_t *) 0xe000ed04 )
#define portNVIC_SYSPRI2				( ( volatile uint32_t *) 0xe000ed20 )
#define portNVIC_SYSTICK_CLK			0x00000004
#define portNVIC_SYSTICK_INT			0x00000002
#define portNVIC_SYSTICK_ENABLE			0x00000001
#define portNVIC_PENDSVSET				0x10000000
#define portMIN_INTERRUPT_PRIORITY		( 255UL )
#define portNVIC_PENDSV_PRI				( portMIN_INTERRUPT_PRIORITY << 16UL )
#define portNVIC_SYSTICK_PRI			( portMIN_INTERRUPT_PRIORITY << 24UL )

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR			( 0x01000000 )

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
#define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
#define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif

/*
 * Setup the timer to generate the tick interrupts.
 */
static void prvSetupTimerInterrupt(void);

/*
 * Exception handlers.
 */
void xPortPendSVHandler(void) __attribute__((naked));
void xPortSysTickHandler(void);
void vPortSVCHandler(void);

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
static void vPortStartFirstTask(void) __attribute__((naked));

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError(void);

/*-----------------------------------------------------------*/

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static UBaseType_t uxCriticalNesting = 0UL;

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters)
{
	/* Simulate the stack frame as it would be created by a context switch
	interrupt. */
	pxTopOfStack--; /* Offset added to account for the way the MCU uses the stack on entry/exit of interrupts. */
	*pxTopOfStack = portINITIAL_XPSR;	/* xPSR */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) pxCode;	/* PC */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) portTASK_RETURN_ADDRESS;	/* LR */
	pxTopOfStack -= 5;	/* R12, R3, R2 and R1. */
	*pxTopOfStack = (StackType_t) pvParameters;	/* R0 */
	pxTopOfStack -= 8; /* R11..R4. */

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError(void)
{
	volatile uint32_t ulDummy = 0UL;

	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).

	Artificially force an assert() to be triggered if configASSERT() is
	defined, then stop here so application writers can catch the error. */
	configASSERT(uxCriticalNesting == ~0UL);
	portDISABLE_INTERRUPTS();
	while (ulDummy == 0) {
		/* This file calls prvTaskExitError() after the scheduler has been
		started to remove a compiler warning about the function being defined
		but never called.  ulDummy is used purely to quieten other warnings
		about code appearing after this function is called - making ulDummy
		volatile makes the compiler think the function could return and
		therefore not output an 'unreachable code' warning for code that appears
		after it. */
	}
}
/*-----------------------------------------------------------*/

void vPortSVCHandler(void)
{
	/* This function is no longer used, but retained for backward
	compatibility. */
}
/*-----------------------------------------------------------*/

void vPortStartFirstTask(void)
{
	/* The MSP stack is not reset as, unlike on M3/4 parts, there is no vector
	table offset register that can be used to locate the initial stack value.
	Not all M0 parts have the application vector table at address 0. */
	__asm volatile(
		"	.syntax unified				\n"
		"	ldr  r2, pxCurrentTCBConst2	\n" /* Obtain location of pxCurrentTCB. */
		"	ldr  r3, [r2]				\n"
		"	ldr  r0, [r3]				\n" /* The first item in pxCurrentTCB is the task top of stack. */
		"	adds r0, #32					\n" /* Discard everything up to r0. */
		"	msr  psp, r0					\n" /* This is now the new top of stack to use in the task. */
		"	movs r0, #2					\n" /* Switch to the psp stack. */
		"	msr  CONTROL, r0				\n"
		"	isb							\n"
		"	pop  {r0-r5}					\n" /* Pop the registers that are saved automatically. */
		"	mov  lr, r5					\n" /* lr is now in r5. */
		"	pop  {r3}					\n" /* Return address is now in r3. */
		"	pop  {r2}					\n" /* Pop and discard XPSR. */
		"	cpsie i						\n" /* The first task has its context and interrupts can be enabled. */
		"	bx   r3						\n" /* Finally, jump to the user defined task code. */
		"								\n"
		"	.align 4					\n"
		"pxCurrentTCBConst2: .word pxCurrentTCB	  "
	);
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
BaseType_t xPortStartScheduler(void)
{
	/* Make PendSV, CallSV and SysTick the same priroity as the kernel. */
	*(portNVIC_SYSPRI2) |= portNVIC_PENDSV_PRI;
	*(portNVIC_SYSPRI2) |= portNVIC_SYSTICK_PRI;

	/* Set system related IRQ */
	__NVIC_SetVector((IRQn_Type)SVCall_IRQn, (u32)(void *)vPortSVCHandler);
	__NVIC_SetVector((IRQn_Type)PendSV_IRQn, (u32)(void *)xPortPendSVHandler);
	__NVIC_SetVector((IRQn_Type)SysTick_IRQn, (u32)(void *)xPortSysTickHandler);

	/* Start the timer that generates the tick ISR.  Interrupts are disabled
	here already. */
	prvSetupTimerInterrupt();

	/* Initialise the critical nesting count ready for the first task. */
	uxCriticalNesting = 0;

	/* Start the first task. */
	vPortStartFirstTask();

	/* Should never get here as the tasks will now be executing!  Call the task
	exit error function to prevent compiler warnings about a static function
	not being called in the case that the application writer overrides this
	functionality by defining configTASK_RETURN_ADDRESS.  Call
	vTaskSwitchContext() so link time optimisation does not remove the
	symbol. */
	vTaskSwitchContext();
	prvTaskExitError();

	/* Should not get here! */
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT(uxCriticalNesting == 1000UL);
}
/*-----------------------------------------------------------*/

void vPortYield(void)
{
	/* Set a PendSV to request a context switch. */
	*(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;

	/* Barriers are normally not required but do ensure the code is completely
	within the specified behaviour for the architecture. */
	__asm volatile("dsb" ::: "memory");
	__asm volatile("isb");
}
/*-----------------------------------------------------------*/

void vPortEnterCritical(void)
{
	portDISABLE_INTERRUPTS();
	uxCriticalNesting++;
	__asm volatile("dsb" ::: "memory");
	__asm volatile("isb");
}
/*-----------------------------------------------------------*/

void vPortExitCritical(void)
{
	configASSERT(uxCriticalNesting);
	uxCriticalNesting--;
	if (uxCriticalNesting == 0) {
		portENABLE_INTERRUPTS();
	}
}
/*-----------------------------------------------------------*/

uint32_t xPortGetCriticalState(void)
{
	return uxCriticalNesting;
}

uint32_t ulSetInterruptMaskFromISR(void)
{
	__asm volatile(
		" mrs r0, PRIMASK	\n"
		" cpsid i			\n"
		" bx lr				  "
		::: "memory"
	);

#if !(defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
	/* To avoid compiler warnings.  The return statement will nevere be reached,
	but some compilers warn if it is not included, while others won't compile if
	it is. */
	return 0;
#endif
}
/*-----------------------------------------------------------*/

void vClearInterruptMaskFromISR(__attribute__((unused)) uint32_t ulMask)
{
	__asm volatile(
		" msr PRIMASK, r0	\n"
		" bx lr				  "
		::: "memory"
	);

#if !(defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
	/* Just to avoid compiler warning.  ulMask is used from the asm code but
	the compiler can't see that.  Some compilers generate warnings without the
	following line, while others generate warnings if the line is included. */
	(void) ulMask;
#endif
}
/*-----------------------------------------------------------*/

void xPortPendSVHandler(void)
{
	/* This is a naked function. */

	__asm volatile
	(
		"	.syntax unified						\n"
		"	mrs r0, psp							\n"
		"										\n"
		"	ldr	r3, pxCurrentTCBConst			\n" /* Get the location of the current TCB. */
		"	ldr	r2, [r3]						\n"
		"										\n"
		"	subs r0, r0, #32					\n" /* Make space for the remaining low registers. */
		"	str r0, [r2]						\n" /* Save the new top of stack. */
		"	stmia r0!, {r4-r7}					\n" /* Store the low registers that are not saved automatically. */
		" 	mov r4, r8							\n" /* Store the high registers. */
		" 	mov r5, r9							\n"
		" 	mov r6, r10							\n"
		" 	mov r7, r11							\n"
		" 	stmia r0!, {r4-r7}					\n"
		"										\n"
		"	push {r3, r14}						\n"
		"	cpsid i								\n"
		"	bl vTaskSwitchContext				\n"
		"	cpsie i								\n"
		"	pop {r2, r3}						\n" /* lr goes in r3. r2 now holds tcb pointer. */
		"										\n"
		"	ldr r1, [r2]						\n"
		"	ldr r0, [r1]						\n" /* The first item in pxCurrentTCB is the task top of stack. */
		"	adds r0, r0, #16					\n" /* Move to the high registers. */
		"	ldmia r0!, {r4-r7}					\n" /* Pop the high registers. */
		" 	mov r8, r4							\n"
		" 	mov r9, r5							\n"
		" 	mov r10, r6							\n"
		" 	mov r11, r7							\n"
		"										\n"
		"	msr psp, r0							\n" /* Remember the new top of stack for the task. */
		"										\n"
		"	subs r0, r0, #32					\n" /* Go back for the low registers that are not automatically restored. */
		" 	ldmia r0!, {r4-r7}					\n" /* Pop low registers.  */
		"										\n"
		"	bx r3								\n"
		"										\n"
		"	.align 4							\n"
		"pxCurrentTCBConst: .word pxCurrentTCB	  "
	);
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler(void)
{
	uint32_t ulPreviousMask;

	ulPreviousMask = portSET_INTERRUPT_MASK_FROM_ISR();
	{
		/* Increment the RTOS tick. */
		if (xTaskIncrementTick() != pdFALSE) {
			/* Pend a context switch. */
			*(portNVIC_INT_CTRL) = portNVIC_PENDSVSET;
		}
	}
	portCLEAR_INTERRUPT_MASK_FROM_ISR(ulPreviousMask);
}
/*-----------------------------------------------------------*/

/*
 * Setup the systick timer to generate the tick interrupts at the required
 * frequency.
 */
void prvSetupTimerInterrupt(void)
{
	/* Stop and reset the SysTick. */
	*(portNVIC_SYSTICK_CTRL) = 0UL;
	*(portNVIC_SYSTICK_CURRENT_VALUE) = 0UL;

	/* Configure SysTick to interrupt at the requested rate. */
	*(portNVIC_SYSTICK_LOAD) = (configCPU_CLOCK_HZ / configTICK_RATE_HZ) - 1UL;
	*(portNVIC_SYSTICK_CTRL) = portNVIC_SYSTICK_CLK | portNVIC_SYSTICK_INT | portNVIC_SYSTICK_ENABLE;
}
/*-----------------------------------------------------------*/
void vApplicationIdleHook(void)
{
	/* Use the idle task to place the CPU into a low power mode.  Greater power
	saving could be achieved by not including any demo tasks that never block. */
	extern void app_taskidle_handler(void);
	app_taskidle_handler();
}

#include "diag.h"
void vApplicationStackOverflowHook(xTaskHandle pxTask, char *pcTaskName)
{
	/* This function will be called if a task overflows its stack, if
	configCHECK_FOR_STACK_OVERFLOW != 0 */

	/* To avoid gcc warnings */
	(void) pxTask;
	(void) pcTaskName;

	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r[%s] STACK OVERFLOW - TaskName(%s)\n\r", __FUNCTION__, pcTaskName);
	for (;;);
}

#if( configSUPPORT_STATIC_ALLOCATION == 1 )

static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
								   StackType_t **ppxIdleTaskStackBuffer,
								   uint32_t *pulIdleTaskStackSize)
{
	/* If the buffers to be provided to the Idle task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */

	/* Pass out a pointer to the StaticTask_t structure in which the Idle
	 * task's state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}


static StaticTask_t xTimerTaskTCB;
static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
									StackType_t **ppxTimerTaskStackBuffer,
									uint32_t *pulTimerTaskStackSize)
{
	/* If the buffers to be provided to the Timer task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */

	/* Pass out a pointer to the StaticTask_t structure in which the Idle
	 * task's state will be stored. */
	*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

	/* Pass out the array that will be used as the Timer task's stack. */
	*ppxTimerTaskStackBuffer = uxTimerTaskStack;

	/* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}
#endif

CONFIG_FW_CRITICAL_CODE_SECTION
void freertos_post_sleep_processing(unsigned int *expected_idle_time)
{
#ifndef configSYSTICK_CLOCK_HZ
	*expected_idle_time = 1 + (SysTick->VAL / (configCPU_CLOCK_HZ / configTICK_RATE_HZ));
#else
	*expected_idle_time = 1 + (SysTick->VAL / (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ));
#endif
}

/* NVIC will power off under sleep power gating mode, so we can */
/* not use systick like FreeRTOS default implementation */
CONFIG_FW_CRITICAL_CODE_SECTION
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
	unsigned int xModifiableIdleTime;
	eSleepModeStatus eSleepStatus;

	system_can_yield = 0;

	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;

	/* Enter a critical section but don't use the taskENTER_CRITICAL()
	method as that will mask interrupts that should exit sleep mode. */
	__disable_irq();

	eSleepStatus = eTaskConfirmSleepModeStatus();

	/* If a context switch is pending or a task is waiting for the scheduler
	to be unsuspended then abandon the low power entry. */
	if (eSleepStatus == eAbortSleep) {
	} else if (freertos_ready_to_sleep()) {
		/* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
		set its parameter to 0 to indicate that its implementation contains
		its own wait for interrupt or wait for event instruction, and so wfi
		should not be executed again.  However, the original expected idle
		time variable must remain unmodified, so a copy is taken. */
		xModifiableIdleTime = (unsigned int)xExpectedIdleTime;
		configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
		if (xModifiableIdleTime > 0) {
			__DSB();
			__WFI();
			__NOP();	 //fix cpu fused instruction issue
			__ISB();
		}
		configPOST_SLEEP_PROCESSING(xModifiableIdleTime);
	}
#if defined (CONFIG_ARM_CORE_CM4)
	else {
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
		__DSB();
		__WFI();
		__NOP();	 //fix cpu fused instruction issue
		__ISB();
	}
#endif
	/* Re-enable interrupts - see comments above the cpsid instruction()
	above. */
	__enable_irq();

	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

	system_can_yield = 1;

#if defined (CONFIG_ARM_CORE_CM0)
	km4_wake_event_update();
	if (km4_get_wake_event()) {
		taskENTER_CRITICAL();
		km4_resume();
		taskEXIT_CRITICAL();
	}
#endif
}

/*-----------------------------------------------------------*/

void vPortCleanUpTCB(uint32_t * pxTCB)
{
	UNUSED(pxTCB);
}
/*-----------------------------------------------------------*/

