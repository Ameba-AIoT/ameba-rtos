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

/* Standard includes. */
#include <stdlib.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "ameba_soc.h"

/* PMU includes. */
#include "ameba_pmu.h"

#include "platform_autoconf.h"

#ifndef configINTERRUPT_CONTROLLER_BASE_ADDRESS
#error configINTERRUPT_CONTROLLER_BASE_ADDRESS must be defined.  See http://www.freertos.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html
#endif

#ifndef configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET
#error configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET must be defined.  See http://www.freertos.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html
#endif

#ifndef configSETUP_TICK_INTERRUPT
#error configSETUP_TICK_INTERRUPT() must be defined.  See http://www.freertos.org/Using-FreeRTOS-on-Cortex-A-Embedded-Processors.html
#endif /* configSETUP_TICK_INTERRUPT */

#ifndef configSETUP_IPI_INTERRUPT
#error configSETUP_IPI_INTERRUPT() must be defined.
#endif /* configSETUP_TICK_INTERRUPT */

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1
/* Check the configuration. */
#if( configMAX_PRIORITIES > 32 )
#error configUSE_PORT_OPTIMISED_TASK_SELECTION can only be set to 1 when configMAX_PRIORITIES is less than or equal to 32.  It is very rare that a system requires more than 10 to 15 difference priorities as tasks that share a priority will time slice.
#endif
#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

/* Some vendor specific files default configCLEAR_TICK_INTERRUPT() in
portmacro.h. */
#ifndef configCLEAR_TICK_INTERRUPT
#define configCLEAR_TICK_INTERRUPT()
#endif

/* Some vendor specific files default configCLEAR_IPI_INTERRUPT() in
portmacro.h. */
#ifndef configCLEAR_IPI_INTERRUPT
#define configCLEAR_IPI_INTERRUPT()
#endif

/* A critical section is exited when the critical section nesting count reaches
this value. */
#define portNO_CRITICAL_NESTING			( ( uint32_t ) 0 )

/* Tasks are not created with a floating point context, but can be given a
floating point context after they have been created.  A variable is stored as
part of the tasks context that holds portNO_FLOATING_POINT_CONTEXT if the task
does not have an FPU context, or any other value if the task does have an FPU
context. */
#define portNO_FLOATING_POINT_CONTEXT	( ( StackType_t ) 0 )

/* Constants required to setup the initial task context. */
#define portINITIAL_SPSR				( ( StackType_t ) 0x1f ) /* System mode, ARM mode, IRQ enabled FIQ enabled. */
#define portTHUMB_MODE_BIT				( ( StackType_t ) 0x20 )
#define portINTERRUPT_ENABLE_BIT		( 0x80UL )
#define portTHUMB_MODE_ADDRESS			( 0x01UL )

/* Masks all bits in the APSR other than the mode bits. */
#define portAPSR_MODE_BITS_MASK			( 0x1F )

/* The value of the mode bits in the APSR when the CPU is executing in user
mode. */
#define portAPSR_USER_MODE				( 0x10 )

/* The value of the mode bits in the APSR when the CPU is executing in user
mode. */
#define portAPSR_IRQ_MODE				( 0x12 )

#define portINTERRUPT_PRIORITY_REGISTER_OFFSET		0x420UL
#define portMAX_8_BIT_VALUE							( ( uint8_t ) 0xff )
#define portBIT_0_SET								( ( uint8_t ) 0x01 )

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
#define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
#define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif

/* The space on the stack required to hold the FPU registers.  This is 32 64-bit
registers, plus a 32-bit status register. */
#define portFPU_REGISTER_WORDS	( ( 32 * 2 ) + 1 )

/*-----------------------------------------------------------*/

/*
 * Starts the first task executing.  This function is necessarily written in
 * assembly code so is implemented in portASM.s.
 */
extern void vPortRestoreTaskContext(void);

/*
 * Returns the yield pending count for the calling core.
 */
extern BaseType_t xTaskGetCurrentYieldPending(void);

/*
 * Used to catch tasks that attempt to return from their implementing function.
 */
static void prvTaskExitError(void);
void TaskExitError(void);

/*
 * If the application provides an implementation of vApplicationIRQHandler(),
 * then it will get called directly without saving the FPU registers on
 * interrupt entry, and this weak implementation of
 * vApplicationFPUSafeIRQHandler() is just provided to remove linkage errors -
 * it should never actually get called so its implementation contains a
 * call to configASSERT() that will always fail.
 *
 * If the application provides its own implementation of
 * vApplicationFPUSafeIRQHandler() then the implementation of
 * vApplicationIRQHandler() provided in portASM.S will save the FPU registers
 * before calling it.
 *
 * Therefore, if the application writer wants FPU registers to be saved on
 * interrupt entry their IRQ handler must be called
 * vApplicationFPUSafeIRQHandler(), and if the application writer does not want
 * FPU registers to be saved on interrupt entry their IRQ handler must be
 * called vApplicationIRQHandler().
 */
void vApplicationFPUSafeIRQHandler(uint32_t ulICCIAR) __attribute__((weak));

/*-----------------------------------------------------------*/

/* A variable is used to keep track of the critical section nesting.  This
variable has to be stored as part of the task context and must be initialised to
a non zero value to ensure interrupts don't inadvertently become unmasked before
the scheduler starts.  As it is stored as part of the task context it will
automatically be set to 0 when the first task is started. */
volatile uint32_t ulCriticalNesting = 0UL;

/* Saved as part of the task context.  If ulPortTaskHasFPUContext is non-zero then
a floating point context must be saved and restored for the task. */
volatile uint32_t ulPortTaskHasFPUContext = pdFALSE;

/* Set to 1 to pend a context switch from an ISR. */
volatile uint32_t ulPortYieldRequired[ configNUM_CORES ] = { pdFALSE };

/* Counts the interrupt nesting depth.  A context switch is only performed if
if the nesting depth is 0. */
volatile uint32_t ulPortInterruptNesting[ configNUM_CORES ] = { pdFALSE };

/* Used for smp  */
spinlock_t task_lock;

volatile uint32_t ulFlashPG_Flag = 0;

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
StackType_t *pxPortInitialiseStack(StackType_t *pxTopOfStack, TaskFunction_t pxCode, void *pvParameters)
{
	/* Setup the initial stack of the task.  The stack is set exactly as
	expected by the portRESTORE_CONTEXT() macro.

	The fist real value on the stack is the status register, which is set for
	system mode, with interrupts enabled.  A few NULLs are added first to ensure
	GDB does not try decoding a non-existent return address. */
	*pxTopOfStack = (StackType_t) NULL;
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) NULL;
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) NULL;
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) portINITIAL_SPSR;

	if (((uint32_t) pxCode & portTHUMB_MODE_ADDRESS) != 0x00UL) {
		/* The task will start in THUMB mode. */
		*pxTopOfStack |= portTHUMB_MODE_BIT;
	}

	pxTopOfStack--;

	/* Next the return address, which in this case is the start of the task. */
	*pxTopOfStack = (StackType_t) pxCode;
	pxTopOfStack--;

	/* Next all the registers other than the stack pointer. */
	*pxTopOfStack = (StackType_t) portTASK_RETURN_ADDRESS;	/* R14 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x12121212;	/* R12 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x11111111;	/* R11 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x10101010;	/* R10 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x09090909;	/* R9 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x08080808;	/* R8 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x07070707;	/* R7 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x06060606;	/* R6 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x05050505;	/* R5 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x04040404;	/* R4 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x03030303;	/* R3 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x02020202;	/* R2 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) 0x01010101;	/* R1 */
	pxTopOfStack--;
	*pxTopOfStack = (StackType_t) pvParameters;   /* R0 */

#if( configUSE_TASK_FPU_SUPPORT == 1 )
	{
		/* The task will start without a floating point context.  A task that
		uses the floating point hardware must call vPortTaskUsesFPU() before
		executing any floating point instructions. */
		pxTopOfStack--;
		*pxTopOfStack = portNO_FLOATING_POINT_CONTEXT;
	}
#elif( configUSE_TASK_FPU_SUPPORT == 2 )
	{
		/* The task will start with a floating point context.  Leave enough
		space for the registers - and ensure they are initialised to 0. */
		pxTopOfStack -= portFPU_REGISTER_WORDS;
		memset(pxTopOfStack, 0x00, portFPU_REGISTER_WORDS * sizeof(StackType_t));

		pxTopOfStack--;
		*pxTopOfStack = pdTRUE;
		ulPortTaskHasFPUContext = pdTRUE;
	}
#else
	{
#error Invalid configUSE_TASK_FPU_SUPPORT setting - configUSE_TASK_FPU_SUPPORT must be set to 1, 2, or left undefined.
	}
#endif

	return pxTopOfStack;
}
/*-----------------------------------------------------------*/

static void prvTaskExitError(void)
{
	/* A function that implements a task must not exit or attempt to return to
	its caller as there is nothing to return to.  If a task wants to exit it
	should instead call vTaskDelete( NULL ).
	Stop here so application writers can catch the error. */
	portDISABLE_INTERRUPTS();
	for (;;);
}

__attribute__((naked)) void TaskExitError(void)
{
    __asm volatile("nop");
    __disable_irq();
    while (1);
}
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler(void)
{
	uint32_t ulAPSR;

	/* Only continue if the CPU is not in User mode.  The CPU must be in a
	Privileged mode for the scheduler to start. */
	__asm volatile("MRS %0, APSR" : "=r"(ulAPSR) :: "memory");
	ulAPSR &= portAPSR_MODE_BITS_MASK;
	configASSERT(ulAPSR != portAPSR_USER_MODE);

	if (ulAPSR != portAPSR_USER_MODE) {
		/* Interrupts are turned off in the CPU itself to ensure tick does
		not execute	while the scheduler is being started.  Interrupts are
		automatically turned back on in the CPU when the first task starts
		executing. */
		portCPU_IRQ_DISABLE();

#if ( configNUM_CORES > 1 )
		{
			/* Enable ipi for yield core */
			configSETUP_IPI_INTERRUPT();
		}
#endif

		/* Start the timer that generates the tick ISR. */
		configSETUP_TICK_INTERRUPT();

		/* Start the first task executing. */
		vPortRestoreTaskContext();
	}

	/* Will only get here if vTaskStartScheduler() was called with the CPU in
	a non-privileged mode or the binary point register was not set to its lowest
	possible value.  prvTaskExitError() is referenced to prevent a compiler
	warning about it being defined but not referenced in the case that the user
	defines their own exit address. */
	(void) prvTaskExitError;
	return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
	/* Not implemented in ports where there is nothing to return to.
	Artificially force an assert. */
	configASSERT(ulCriticalNesting == 1000UL);
}
/*-----------------------------------------------------------*/

void xPortSpinLockTask(void)
{
	spin_lock(&task_lock);
}
/*-----------------------------------------------------------*/

void xPortSpinUnLockTask(void)
{
	spin_unlock(&task_lock);
}
/*-----------------------------------------------------------*/

uint32_t xPortGetCriticalState( void)
{
	return ulCriticalNesting;
}

void vPortEnterCritical(void)
{
	/* Mask interrupts up to the max syscall interrupt priority. */
	portDISABLE_INTERRUPTS();

	/* Now interrupts are disabled ulCriticalNesting can be accessed
	directly.  Increment ulCriticalNesting to keep a count of how many times
	portENTER_CRITICAL() has been called. */
	ulCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical(void)
{
	if (ulCriticalNesting > portNO_CRITICAL_NESTING) {
		/* Decrement the nesting count as the critical section is being
		exited. */
		ulCriticalNesting--;

		/* If the nesting level has reached zero then all interrupt
		priorities must be re-enabled. */
		if (ulCriticalNesting == portNO_CRITICAL_NESTING) {
			/* Critical nesting has reached zero so enable interrupt. */
			if (portCHECK_IF_IN_ISR() == pdFALSE) {
				if (xTaskGetCurrentYieldPending() != pdFALSE) {
					portYIELD();
				}

				portENABLE_INTERRUPTS();
			}
		}
	}
}
/*-----------------------------------------------------------*/

void FreeRTOS_Tick_Handler(void)
{
	/* Only core 0 do xTaskIncrementTick() */
	BaseType_t xCoreID = portGET_CORE_ID();

	if (xCoreID == 0) {
		/* Increment the RTOS tick. */
		ulPortYieldRequired[ xCoreID ] = xTaskIncrementTick();
	} else {
		ulPortYieldRequired[ xCoreID ] = xTaskGetCurrentYieldPending();
	}

	configCLEAR_TICK_INTERRUPT();
}
/*-----------------------------------------------------------*/

void vApplicationFPUSafeIRQHandler(uint32_t ulICCIAR)
{
	(void) ulICCIAR;
	configASSERT((volatile void *) NULL);
}
/*-----------------------------------------------------------*/

#if( configUSE_TASK_FPU_SUPPORT != 2 )

void vPortTaskUsesFPU(void)
{
	uint32_t ulInitialFPSCR = 0;

	/* A task is registering the fact that it needs an FPU context.  Set the
	FPU flag (which is saved as part of the task context). */
	ulPortTaskHasFPUContext = pdTRUE;

	/* Initialise the floating point status register. */
	__asm volatile("FMXR 	FPSCR, %0" :: "r"(ulInitialFPSCR) : "memory");
}

#endif /* configUSE_TASK_FPU_SUPPORT */
/*-----------------------------------------------------------*/

uint32_t ulPortInterruptLock(void)
{
	uint32_t key;

	__asm volatile("mrs	%0, cpsr	\n": "=r"(key) :: "memory");
	portCPU_IRQ_DISABLE();

	return key;
}
/*-----------------------------------------------------------*/

void ulPortInterruptUnLock(uint32_t key)
{
	__asm volatile("msr	cpsr_c, %0	\n" :: "r"(key) : "memory");
}
/*-----------------------------------------------------------*/

BaseType_t xPortCpuIsInInterrupt(void)
{
	return ((__get_mode() != CPSR_M_USR) && (__get_mode() != CPSR_M_SYS)) ? pdTRUE : pdFALSE;
//	uint32_t cpsr;
//	__asm volatile (	"mrs	%0, cpsr	\n" : "=r" (cpsr));
//	return ((cpsr & portAPSR_MODE_BITS_MASK) == portAPSR_IRQ_MODE) || ulPortInterruptNesting[portGET_CORE_ID()];
}

/*-----------------------------------------------------------*/
/* SMP utilities. */
/*-----------------------------------------------------------*/
#if ( configNUM_CORES > 1 )

void FreeRTOS_IPI_Handler(void)
{
	ulPortYieldRequired[ portGET_CORE_ID() ] = pdTRUE;

	//configCLEAR_IPI_INTERRUPT(); /*No Need For GICC_CTLR.EOImode is set to 0*/
}
/*-----------------------------------------------------------*/

uint32_t ulPortGetCoreId(void)
{
	uint32_t mpidr;

	__asm volatile("mrc	p15, 0, %0, c0, c0, 5\n" : "=r"(mpidr));

	return (mpidr & 0xFF) ? 1 : 0;
}
/*-----------------------------------------------------------*/
void FreeRTOS_IPI_CPUHP_Handler(void)
{
	BaseType_t xCoreID;
	xCoreID = ulPortGetCoreId();

	/* Make this core to do a context switch */
	ulPortYieldRequired[ xCoreID ] = pdTRUE;

	//arm_gic_deactive_irq(IPI_CPUHP_IRQ);/*No Need For GICC_CTLR.EOImode is set to 0*/
}

SRAMDRAM_ONLY_TEXT_SECTION
void FreeRTOS_IPI_FLASHPG_Handler(void)
{
	uint32_t PrevIrqStatus = portSET_INTERRUPT_MASK_FROM_ISR();
	/* The completion of a DSB that completes a TLB maintenance operation ensures that all accesses that used the old mapping have completed. */
	MMU_InvalidateTLB();
	L1C_InvalidateBTAC();

	while (ulFlashPG_Flag) {
		__WFE();
	}

	portCLEAR_INTERRUPT_MASK_FROM_ISR(PrevIrqStatus);
}

void vPortYieldOtherCore(uint32_t xCoreID)
{
	configYieldOtherCore(xCoreID);
}
/*-----------------------------------------------------------*/
#else
uint32_t ulPortGetCoreId(void)
{
	return 0;
}
#endif

/*-----------------------------------------------------------*/
__attribute__((optimize("O0")))
void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
	(void) pcTaskName;
	(void) xTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\rSTACK OVERFLOW - TaskName(%s)\n\r", pcTaskName);
	taskDISABLE_INTERRUPTS();
	for (;;);
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook(void)
{

}
/*-----------------------------------------------------------*/

#if( configSUPPORT_STATIC_ALLOCATION == 1 )

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
								   StackType_t **ppxIdleTaskStackBuffer,
								   uint32_t *pulIdleTaskStackSize)
{
	/* If the buffers to be provided to the Idle task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */
	static StaticTask_t xIdleTaskTCB;
	static StackType_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

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

void vApplicationGetPassiveIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer,
		configSTACK_DEPTH_TYPE *puxIdleTaskStackSize,
		BaseType_t xPassiveIdleTaskIndex)
{
	/* If the buffers to be provided to the Idle task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exist after this function exits. */
#if configNUM_CORES == 1
	(void) ppxIdleTaskTCBBuffer;
	(void) ppxIdleTaskStackBuffer;
	(void) puxIdleTaskStackSize;
	(void) xPassiveIdleTaskIndex;
	RTK_LOGE(NOTAG, "CA32 single core, should not into vApplicationGetPassiveIdleTaskMemory\n");
	while(1);
#else
	static StaticTask_t xIdleTaskTCBs[ configNUM_CORES - 1 ];
	static StackType_t uxIdleTaskStacks[ configNUM_CORES - 1 ][ configMINIMAL_STACK_SIZE ];
	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	 * state will be stored. */
	*ppxIdleTaskTCBBuffer = &(xIdleTaskTCBs[ xPassiveIdleTaskIndex  ]);

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = &(uxIdleTaskStacks[ xPassiveIdleTaskIndex  ][ 0 ]);

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	 * Note that, as the array is necessarily of type StackType_t,
	 * configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*puxIdleTaskStackSize = configMINIMAL_STACK_SIZE;
#endif
}

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
									StackType_t **ppxTimerTaskStackBuffer,
									uint32_t *pulTimerTaskStackSize)
{
	/* If the buffers to be provided to the Timer task are declared inside this
	 * function then they must be declared static - otherwise they will be allocated on
	 * the stack and so not exists after this function exits. */
	static StaticTask_t xTimerTaskTCB;
	static StackType_t uxTimerTaskStack[ configTIMER_TASK_STACK_DEPTH ];

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

uint32_t check_core1_wfe_state(void)
{
	CA32_TypeDef *ca32 = CA32_BASE;
	uint32_t core_state = (ca32->CA32_C0_CPU_STATUS);

	if (core_state & CA32_STANDBYWFE_CORE1) {
		/* core1 already in WFE state*/
		return 1;
	} else {
		return 0;
	}
}

void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
	uint32_t xModifiableIdleTime;
	eSleepModeStatus eSleepStatus;

	if (portGET_CORE_ID() == 0) {
		/* Enter a critical section but don't use the taskENTER_CRITICAL()
		method as that will mask interrupts that should exit sleep mode. */
		portDISABLE_INTERRUPTS();

		/* mask sys tick interrupt*/
		arm_arch_timer_int_mask(pdTRUE);

		system_can_yield = 0;

		portGET_TASK_LOCK();
		eSleepStatus = eTaskConfirmSleepModeStatus();
		portRELEASE_TASK_LOCK();

		/* If a context switch is pending or a task is waiting for the scheduler
		to be unsuspended then abandon the low power entry. */
		if (eSleepStatus == eAbortSleep) {
			arm_arch_timer_int_mask(pdFALSE);
		} else if (pmu_ready_to_sleep()) {

			//HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_AON_BACKUP2, 0);
			/* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
			set its parameter to 0 to indicate that its implementation contains
			its own wait for interrupt or wait for event instruction, and so wfi
			should not be executed again.  However, the original expected idle
			time variable must remain unmodified, so a copy is taken. */
			xModifiableIdleTime = (uint32_t)xExpectedIdleTime;

#if ( configNUM_CORES > 1 )
			/*PG flow */
			if (pmu_get_sleep_type() == SLEEP_PG) {
				/* CPU1 just come back from pg, so can't sleep here */
				if (pmu_get_secondary_cpu_state(1) == CPU1_WAKE_FROM_PG) {
					goto EXIT;
				}

				/* CPU1 is in task schedular, tell CPU1 to enter hotplug */
				if (pmu_get_secondary_cpu_state(1) == CPU1_RUNNING) {
					/* CPU1 may in WFI idle state. Wake it up to enter hotplug itself */
					portENABLE_INTERRUPTS();
					arm_gic_raise_softirq(1, 0);
					arm_arch_timer_int_mask(pdFALSE);
					DelayUs(100);
					goto EXIT;
				}

				/* CG flow */
			} else {
				if (!check_core1_wfe_state()) {  // CG need wfe to avoid core1 still running when gating clk
					goto EXIT;
				}
			}
#endif
			configPRE_SLEEP_PROCESSING(xModifiableIdleTime);

			/* When wake from pg, arm timer has been reset, so a new compare value is necessary to
			   trigger an timer interrupt */
			if (pmu_get_sleep_type() == SLEEP_PG) {
				if(!(arm_arch_timer_check_enable())) {
					arm_arch_timer_enable(pdTRUE);
				}
				extern uint64_t GenericTimerFreq;
				arm_arch_timer_set_compare(arm_arch_timer_count() + (GenericTimerFreq/RTOS_TICK_RATE_HZ));
			}
			arm_arch_timer_int_mask(pdFALSE);
			//if (xModifiableIdleTime > 0) {
			//	__asm volatile("dsb");
			//	__asm volatile("wfi");
			//	__asm volatile("isb");
			//}
			configPOST_SLEEP_PROCESSING(xModifiableIdleTime);
		} else {
			/* power saving when idle*/
			arm_arch_timer_int_mask(pdFALSE);
			__asm(" DSB");
			__asm(" WFI");
			__asm(" ISB");
			/* power saving when idle*/
		}
#if ( configNUM_CORES > 1 )
EXIT:
#endif

		if (check_core1_wfe_state()) {
			__asm(" sev");
		}

		system_can_yield = 1;
		/* Re-enable interrupts and sys tick*/
		portENABLE_INTERRUPTS();
	} else if (portGET_CORE_ID() == 1) {

		if (pmu_ready_to_sleep()) {
			if (pmu_get_sleep_type() == SLEEP_PG) {
				/* CPU1 will enter hotplug state. Raise a task yield to migrate its task */
				pmu_set_secondary_cpu_state(1, CPU1_HOTPLUG);
				portYIELD();
			} else {
				portDISABLE_INTERRUPTS();
				__asm(" DSB");
				__asm(" WFE");	// trap core1 in wfe if cg to avoid running out
				__asm(" WFE");
				__asm(" ISB");
				portENABLE_INTERRUPTS();
			}
		} else {
			portDISABLE_INTERRUPTS();
			__asm(" DSB");
			__asm(" WFI");
			__asm(" ISB");
			portENABLE_INTERRUPTS();

		}
	}
	(void)xModifiableIdleTime;
}

void pmu_post_sleep_processing(uint32_t *tick_before_sleep)
{
	static uint32_t missing_tick = 0;
	uint32_t tick_passed;
	volatile uint32_t ms_passed = 0;

	/*	update kernel tick by calculating passed tick from gtimer */
	/*	get current gtimer timestamp */
	tick_passed = SYSTIMER_GetPassTick(*tick_before_sleep);

	tick_passed += missing_tick;
	missing_tick = tick_passed & 0x1F;

	/* ms =x*1000/32768 = (x *1000) >>15 */
	ms_passed = (uint32_t)((((uint64_t)tick_passed) * 1000) >> 15);

	/* update xTickCount and mark to trigger task list update in xTaskResumeAll */
	vTaskCompTick(ms_passed);

	/* update sleepwakelock_timeout if sysactive_timeout_temp not 0 */
	sysactive_timeout_flag = 0;
	pmu_set_sysactive_time(0);

	RTK_LOGD(NOTAG, "sleeped:[%d] ms\n", ms_passed);

}

/*-----------------------------------------------------------*/

void vPortCleanUpTCB(uint32_t * pxTCB)
{
	UNUSED(pxTCB);
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
	extern void sys_thread_sem_deinit_tcb(uint32_t *pxTCB);
	sys_thread_sem_deinit_tcb(pxTCB);
#endif
}
/*-----------------------------------------------------------*/

