/*
 * FreeRTOS Kernel V10.4.3
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
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/

#include <stdio.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"

/* Standard includes. */
#include "string.h"
#include "log.h"

/* PMU includes. */
#include "ameba_pmu.h"
#include "cmsis_gcc_riscv.h"

#include "platform_autoconf.h"

#if defined(CONFIG_STANDARD_TICKLESS) && defined(CONFIG_LWIP_LAYER)
extern void lwip_update_internal_counter(uint32_t ms);
#endif

#ifdef configCLINT_BASE_ADDRESS
#undef configCLINT_BASE_ADDRESS
#endif

#ifndef configMTIME_BASE_ADDRESS
#warning configMTIME_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtime register then set configMTIME_BASE_ADDRESS to the mapped address.  Otherwise set configMTIME_BASE_ADDRESS to 0.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

#ifndef configMTIMECMP_BASE_ADDRESS
#warning configMTIMECMP_BASE_ADDRESS must be defined in FreeRTOSConfig.h.  If the target chip includes a memory-mapped mtimecmp register then set configMTIMECMP_BASE_ADDRESS to the mapped address.  Otherwise set configMTIMECMP_BASE_ADDRESS to 0.  See https://www.FreeRTOS.org/Using-FreeRTOS-on-RISC-V.html
#endif

/* Let the user override the pre-loading of the initial LR with the address of
prvTaskExitError() in case it messes up unwinding of the stack in the
debugger. */
#ifdef configTASK_RETURN_ADDRESS
#define portTASK_RETURN_ADDRESS	configTASK_RETURN_ADDRESS
#else
#define portTASK_RETURN_ADDRESS	prvTaskExitError
#endif

/* The stack used by interrupt service routines.  Set configISR_STACK_SIZE_WORDS
to use a statically allocated array as the interrupt stack.  Alternative leave
configISR_STACK_SIZE_WORDS undefined and update the linker script so that a
linker variable names __freertos_irq_stack_top has the same value as the top
of the stack used by main.  Using the linker script method will repurpose the
stack that was used by main before the scheduler was started for use as the
interrupt stack after the scheduler has started. */
#ifdef configISR_STACK_SIZE_WORDS
__attribute__((aligned(32))) StackType_t xISRStack[ configISR_STACK_SIZE_WORDS ] = { 0 };
const StackType_t xISRStackTop = (StackType_t) & (xISRStack[ configISR_STACK_SIZE_WORDS & ~portBYTE_ALIGNMENT_MASK ]);

/* Don't use 0xa5 as the stack fill bytes as that is used by the kernerl for
the task stacks, and so will legitimately appear in many positions within
the ISR stack. */
#define portISR_STACK_FILL_BYTE	0xee
#else
extern const uint32_t __freertos_irq_stack_top[];
const StackType_t xISRStackTop = (StackType_t) __freertos_irq_stack_top;
#endif

extern uint32_t SystemCoreClock;     /*!< System Clock Frequency (Core Clock)  */

/*
 * Setup the timer to generate the tick interrupts.  The implementation in this
 * file is weak to allow application writers to change the timer used to
 * generate the tick interrupt.
 */
void vPortSetupTimerInterrupt(void) __attribute__((weak));

/*-----------------------------------------------------------*/

/* Used to program the machine timer compare register. */
uint64_t ullNextTime = 0ULL;
const uint64_t *pullNextTime = &ullNextTime;
size_t uxTimerIncrementsForOneTick;          /* Assumes increment won't go over 32-bits. */
uint32_t const ullMachineTimerCompareRegisterBase = configMTIMECMP_BASE_ADDRESS;
volatile uint64_t *pullMachineTimerCompareRegister = NULL;
#if( configUSE_TICKLESS_IDLE == 1 )
volatile uint64_t *pullMachineTimerCounterRegister = NULL;
#endif

/* Set configCHECK_FOR_STACK_OVERFLOW to 3 to add ISR stack checking to task
stack checking.  A problem in the ISR stack will trigger an assert, not call the
stack overflow hook function (because the stack overflow hook is specific to a
task stack, not the ISR stack). */
#if defined( configISR_STACK_SIZE_WORDS ) && ( configCHECK_FOR_STACK_OVERFLOW > 2 )
#warning This path not tested, or even compiled yet.

static const uint8_t ucExpectedStackBytes[] = {
	portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
	portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
	portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
	portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE,		\
	portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE, portISR_STACK_FILL_BYTE
};
\

#define portCHECK_ISR_STACK() configASSERT( ( memcmp( ( void * ) xISRStack, ( void * ) ucExpectedStackBytes, sizeof( ucExpectedStackBytes ) ) == 0 ) )
#else
/* Define the function away. */
#define portCHECK_ISR_STACK()
#endif /* configCHECK_FOR_STACK_OVERFLOW > 2 */

/*-----------------------------------------------------------*/

#if( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 )

void vPortSetupTimerInterrupt(void)
{
	uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
	volatile uint32_t *const pulTimeHigh = (volatile uint32_t *const)((configMTIME_BASE_ADDRESS) +
										   4UL);          /* 8-byte typer so high 32-bit word is 4 bytes up. */
	volatile uint32_t *const pulTimeLow = (volatile uint32_t *const)(configMTIME_BASE_ADDRESS);
	volatile uint32_t ulHartId;

	uxTimerIncrementsForOneTick = (size_t)((configCPU_CLOCK_HZ) / (configTICK_RATE_HZ));

	__asm volatile("csrr %0, mhartid" : "=r"(ulHartId));
	pullMachineTimerCompareRegister  = (volatile uint64_t *)(ullMachineTimerCompareRegisterBase + (ulHartId * sizeof(uint64_t)));
#if( configUSE_TICKLESS_IDLE == 1 )
	pullMachineTimerCounterRegister = (volatile uint64_t *)(configMTIME_BASE_ADDRESS + (ulHartId * sizeof(uint64_t)));
#endif
	do {
		ulCurrentTimeHigh = *pulTimeHigh;
		ulCurrentTimeLow = *pulTimeLow;
	} while (ulCurrentTimeHigh != *pulTimeHigh);

	ullNextTime = (uint64_t) ulCurrentTimeHigh;
	ullNextTime <<= 32ULL; /* High 4-byte word is 32-bits up. */
	ullNextTime |= (uint64_t) ulCurrentTimeLow;
	ullNextTime += (uint64_t) uxTimerIncrementsForOneTick;
	*pullMachineTimerCompareRegister = ullNextTime;

	/* Prepare the time to use after the next tick interrupt. */
	ullNextTime += (uint64_t) uxTimerIncrementsForOneTick;
}

#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIME_BASE_ADDRESS != 0 ) */
/*-----------------------------------------------------------*/

BaseType_t xPortStartScheduler(void)
{
	extern void xPortStartFirstTask(void);

#if( configASSERT_DEFINED == 1 )
	{
		volatile uint32_t mtvec = 0;

		/* Check the least significant two bits of mtvec are 00 - indicating
		single vector mode. */
		__asm volatile("csrr %0, mtvec" : "=r"(mtvec));
		configASSERT((mtvec & 0x03UL) == 0);

		/* Check alignment of the interrupt stack - which is the same as the
		stack that was being used by main() prior to the scheduler being
		started. */
		configASSERT((xISRStackTop & portBYTE_ALIGNMENT_MASK) == 0);

#ifdef configISR_STACK_SIZE_WORDS
		{
			memset((void *) xISRStack, portISR_STACK_FILL_BYTE, sizeof(xISRStack));
		}
#endif	 /* configISR_STACK_SIZE_WORDS */
	}
#endif /* configASSERT_DEFINED */

	/* If there is a CLINT then it is ok to use the default implementation
	in this file, otherwise vPortSetupTimerInterrupt() must be implemented to
	configure whichever clock is to be used to generate the tick interrupt. */
	vPortSetupTimerInterrupt();

#if( ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) )
	{
		/* Enable mtime and external interrupts.  1<<7 for timer interrupt, 1<<11
		for external interrupt.  _RB_ What happens here when mtime is not present as
		with pulpino? */
		__asm volatile("csrs mie, %0" :: "r"(0x880));
	}
#else
	{
		/* Enable external interrupts. */
		__asm volatile("csrs mie, %0" :: "r"(0x800));
	}
#endif /* ( configMTIME_BASE_ADDRESS != 0 ) && ( configMTIMECMP_BASE_ADDRESS != 0 ) */

	xPortStartFirstTask();

	/* Should not get here as after calling xPortStartFirstTask() only tasks
	should be executing. */
	return pdFAIL;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler(void)
{
	/* Not implemented. */
	for (;;);
}


#include "diag.h"
void vApplicationTickHook(void)
{
	return;
}

void vApplicationIdleHook(void)
{
	volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();
#if !defined(CONFIG_MP_SHRINK) && defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
	extern int driver_suspend_ret;
#if defined (CONFIG_WLAN)
	extern int wlan_driver_check_and_suspend(void);
	driver_suspend_ret = wlan_driver_check_and_suspend();
#endif
#endif
#if !defined(CONFIG_MP_SHRINK) && defined (CONFIG_WLAN)
#if (defined(CONFIG_AS_INIC_NP) && CONFIG_AS_INIC_NP) || (defined(CONFIG_SINGLE_CORE_WIFI) && CONFIG_SINGLE_CORE_WIFI)
	extern void wififw_task_idle(void);
	wififw_task_idle();
#endif
#endif

	if (xFreeStackSpace > 100) {
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}

/*-----------------------------------------------------------*/
void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	/* To avoid gcc warnings */
	(void) pxTask;

	/* This function will be called if a task overflows its stack, if
	configCHECK_FOR_STACK_OVERFLOW != 0.  It might be that the function
	parameters have been corrupted, depending on the severity of the stack
	overflow.  When this is the case pxCurrentTCB can be inspected in the
	debugger to find the offending task. */
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r[%s] STACK OVERFLOW - TaskName(%s)\n\r", __FUNCTION__, pcTaskName);
	for (;;);
}

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

void pmu_post_sleep_processing(uint32_t *expected_idle_time)
{
	uint32_t tick_passed;
	static uint32_t missing_tick = 0;
	volatile uint32_t ms_passed = 0;

	/*  update kernel tick by calculating passed tick from gtimer */
	/*  get current gtimer timestamp */
	tick_passed = SYSTIMER_GetPassTick(*expected_idle_time);
	tick_passed += missing_tick;
	missing_tick = tick_passed & 0x1F;

	/* ms =x*1000/32768 = (x *1000) >>15 */
	ms_passed = (u32)((((u64)tick_passed) * 1000) >> 15);

	/* update xTickCount and mark to trigger task list update in xTaskResumeAll */
	vTaskCompTick(ms_passed);
	/* update sleepwakelock_timeout if sysactive_timeout_temp not 0 */
	sysactive_timeout_flag = 0;
	pmu_set_sysactive_time(0);
#if defined(CONFIG_STANDARD_TICKLESS) && defined(CONFIG_LWIP_LAYER)
	lwip_update_internal_counter(ms_passed);
#endif

	RTK_LOGD(NOTAG, "%s sleeped:[%lu] ms\n", "KM4", ms_passed);	

}

/* NVIC will power off under sleep power gating mode, so we can */
/* not use systick like FreeRTOS default implementation */
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
       uint32_t xModifiableIdleTime;
       eSleepModeStatus eSleepStatus;
       (void) xExpectedIdleTime;

       system_can_yield = 0;

       /* disable systick count */
       Systick_Cmd(DISABLE);
       /* disable interrupts, including systick */
       __disable_irq();

       eSleepStatus = eTaskConfirmSleepModeStatus();

       /* If a context switch is pending or a task is waiting for the scheduler
       to be unsuspended then abandon the low power entry. */
       if (eSleepStatus == eAbortSleep) {
       } else if (pmu_ready_to_sleep()) {
               /* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
               set its parameter to 0 to indicate that its implementation contains
               its own wait for interrupt or wait for event instruction, and so wfi
               should not be executed again.  However, the original expected idle
               time variable must remain unmodified, so a copy is taken. */
               xModifiableIdleTime = (uint32_t)xExpectedIdleTime;
               configPRE_SLEEP_PROCESSING(xModifiableIdleTime);
               configPOST_SLEEP_PROCESSING(xModifiableIdleTime);
       } else {
               Systick_Cmd(ENABLE);
               __DSB();
               __WFI();
               __NOP();                //fix cpu fused instruction issue
               __ISB();
       }

       /* Re-enable interrupts */
       __enable_irq();
       /* Re-enable systick count */
       Systick_Cmd(ENABLE);

       system_can_yield = 1;

	(void)xModifiableIdleTime;
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
