/*
 * Copyright 2012-2019  Realtek Semiconductor Corp.
 *
 * Author: PSP Software Group
 */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H


#define configFLASH_SPEED_NSEC                                                 \
    100 /* External flash access speed (for ROM builds) */

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE:
 *
 * See http://www.freertos.org/a00110.html
 */

/*----------------------------------------------------------*/
#ifndef __IASMARM__
#define __IASMARM__ 0 /* IAR */
#endif
#include "platform_autoconf.h"
#include "ameba_userheapcfg.h"

#define configENABLE_FPU 								1
#define configSUPPORT_STATIC_ALLOCATION					1

/* Constants related to the behaviour or the scheduler. */
#define configUSE_PREEMPTION							1
#define configMAX_PRIORITIES							( 11 )

#define configIDLE_SHOULD_YIELD							1
#define configUSE_16_BIT_TICKS							0 /* Only for 8 and 16-bit hardware. */
/* Constants that describe the hardware and memory usage. */
#define configCPU_CLOCK_HZ								SystemCoreClock
#define configMINIMAL_STACK_SIZE 						((unsigned short)512)
#define configMINIMAL_SECURE_STACK_SIZE					( 1024 )
#define configMAX_TASK_NAME_LEN							( 24 )
/* Constants that build features in or out. */
#define configUSE_MUTEXES								1
#define configUSE_APPLICATION_TASK_TAG					1
#define configUSE_NEWLIB_REENTRANT						1
/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 							0
#define configMAX_CO_ROUTINE_PRIORITIES 				(2)
#define configUSE_COUNTING_SEMAPHORES					1
#define configUSE_RECURSIVE_MUTEXES						1

#define configUSE_TRACE_FACILITY						1
#define configSUPPORT_STATIC_ALLOCATION					1
#define configUSE_POSIX_ERRNO						1
#define configNUM_THREAD_LOCAL_STORAGE_POINTERS			2

/* Constants that define which hook (callback) functions should be used. */
#define configUSE_IDLE_HOOK								1
#define configUSE_TICK_HOOK 							1
#define configUSE_MALLOC_FAILED_HOOK                    1
/* Constants provided for debugging and optimisation assistance. */
#define configCHECK_FOR_STACK_OVERFLOW                  2

#define configRECORD_STACK_HIGH_ADDRESS 				1

#define configISR_STACK_SIZE_WORDS 						1024


#if defined(CONFIG_DEBUG_BACK_TRACE)
#define configFAULT_BACK_TRACE 							1
#endif

#if defined(CONFIG_DEBUG_CRASH_DUMP)
#define configFAULT_CRASH_DUMP 							1
#endif

#if defined(CONFIG_SYS_NESTED_IRQ)
#define configNESTED_IRQ 								1
#endif

/* Software timer definitoions */
#define configUSE_TIMERS								1
#define configSUPPORT_DYNAMIC_ALLOCATION 				1
#define configTIMER_TASK_PRIORITY						 (configMAX_PRIORITIES - 1)
#if defined(CONFIG_AS_INIC_NP) || defined(CONFIG_SINGLE_CORE_WIFI)
#define configTIMER_QUEUE_LENGTH					(10 + 64)
#else
#define configTIMER_QUEUE_LENGTH					10
#endif
#define configTIMER_TASK_STACK_DEPTH				( 512  )

/* Set the following definitions to 1 to include the API function, or zero
 * to exclude the API function.  NOTE:  Setting an INCLUDE_ parameter to 0 is
 * only necessary if the linker does not automatically remove functions that are
 * not referenced anyway. */

#define INCLUDE_vTaskPrioritySet						1
#define INCLUDE_uxTaskPriorityGet						1
#define INCLUDE_vTaskDelete								1
#define INCLUDE_vTaskCleanUpResources					0
#define INCLUDE_vTaskSuspend							1
#define INCLUDE_vTaskDelayUntil							1
#define INCLUDE_vTaskDelay								1
#define INCLUDE_xTaskGetCurrentTaskHandle 				1
#define INCLUDE_xQueueGetMutexHolder 					1

/* This demo makes use of one or more example stats formatting functions.  These
 * format the raw data provided by the uxTaskGetSystemState() function in to
 * human readable ASCII form.  See the notes in the implementation of vTaskList()
 * within FreeRTOS/Source/tasks.c for limitations. */
#define configUSE_STATS_FORMATTING_FUNCTIONS			1

#define traceQUEUE_CREATE(pxNewQueue) \
    {\
        void QueueCreateTrace(QueueHandle_t pxNewQueue); \
        do {QueueCreateTrace(pxNewQueue);} while(0); \
    }

#define traceQUEUE_DELETE(pxQueue) \
    {\
        void QueueDeleteTrace(QueueHandle_t pxQueue); \
        do {QueueDeleteTrace(pxQueue);} while(0); \
    }

#define portHANDLE_INTERRUPT _portHANDLE_INTERRUPT
#define portHANDLE_EXCEPTION _portHANDLE_EXCEPTION


#ifdef CONFIG_DEBUG_RISCV_PMP
#define portUSING_MPU_WRAPPERS 1
#define portRISCV_STACK_PROTECT 1
#else
#define portUSING_MPU_WRAPPERS 0
#define portRISCV_STACK_PROTECT 0
#endif

/* IRQ Configurations */
#define configCLINT_BASE_ADDRESS 0x80000000	// for TR9 PLIC
#define configMTIME_BASE_ADDRESS 0x80000000
#define configMTIMECMP_BASE_ADDRESS 0x80000008
//#define configCLINT_BASE_ADDRESS 0xb1000000	// for KR0 CLIC
#ifdef CONFIG_SYS_VECTORED_IRQ
#define configVECTORED_INTERRRUPT 1
#else
#define configVECTORED_INTERRRUPT 0
#endif


/* Interrupt priorities used by the kernel port layer itself.  These are generic
 * to all Cortex-M ports, and do not rely on any particular library functions. */
#define configKERNEL_INTERRUPT_PRIORITY					1
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
 * See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY			5

/* Constants related to the generation of run time stats. */
#define configGENERATE_RUN_TIME_STATS					1
#if configGENERATE_RUN_TIME_STATS
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() 		//( ulHighFrequencyTimerTicks = 0UL )
#define portGET_RUN_TIME_COUNTER_VALUE() 				    (*((volatile uint32_t*)(0x41018000 + 0x014)))  /* AmebaLite TIMER0_REG_BASE: 0x42009000, COUNT REGISTER offset: 0x014 */
#undef	configUSE_TRACE_FACILITY
#define configUSE_TRACE_FACILITY						1
#define portCONFIGURE_STATS_PEROID_VALUE				1000 //unit Ticks
#else
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()				xTickCount
#endif

#define configTICK_RATE_HZ								( ( TickType_t ) 1000 )

/* Enable tickless power saving. */
#define configUSE_TICKLESS_IDLE                 		1

/* In wlan usage, this value is suggested to use value less than 80 milliseconds */
#define configEXPECTED_IDLE_TIME_BEFORE_SLEEP   		2

/* It's magic trick that let us can use our own sleep function */
#ifndef CONFIG_MP_SHRINK
#define configPRE_SLEEP_PROCESSING( x )         		( pmu_pre_sleep_processing((uint32_t *)&x) )
#define configPOST_SLEEP_PROCESSING( x )        		( pmu_post_sleep_processing((uint32_t *)&x) )
#else
#define configPRE_SLEEP_PROCESSING( x )
#define configPOST_SLEEP_PROCESSING( x )
#endif

/* system assert level controlled by rtk_assert.h header file. */
#ifndef __ASSEMBLER__
#include "rtk_assert.h"
//#define configASSERT(x)    rtk_assert(x)
#endif

#endif /* FREERTOS_CONFIG_H */
