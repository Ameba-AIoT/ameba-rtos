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

#ifndef PORTMACRO_H
#define PORTMACRO_H

#ifdef __cplusplus
	extern "C" {
#endif

/*-----------------------------------------------------------
 * Port specific definitions.
 *
 * The settings in this file configure FreeRTOS correctly for the given hardware
 * and compiler.
 *
 * These settings should not be altered.
 *-----------------------------------------------------------
 */

/* Type definitions. */
#define portCHAR		char
#define portFLOAT		float
#define portDOUBLE		double
#define portLONG		long
#define portSHORT		short
#define portSTACK_TYPE	uint32_t
#define portBASE_TYPE	long

typedef portSTACK_TYPE StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;

typedef uint32_t TickType_t;
#define portMAX_DELAY				( TickType_t ) 0xffffffffUL

/* 32-bit tick type on a 32-bit architecture, so reads of the tick count do
not need to be guarded with a critical section. */
#define portTICK_TYPE_IS_ATOMIC		1

#define portCRITICAL_NESTING_IN_TCB	1

/*-----------------------------------------------------------*/

/* Hardware specifics. */
#define portSTACK_GROWTH			( -1 )
#define portTICK_PERIOD_MS			( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define portBYTE_ALIGNMENT			64

/*-----------------------------------------------------------*/

/* Task utilities. */

/* Called at the end of an ISR that can cause a context switch. */
#define portEND_SWITCHING_ISR( xSwitchRequired )			\
{															\
extern uint32_t ulPortYieldRequired[];						\
															\
	if( xSwitchRequired != pdFALSE )						\
	{														\
		ulPortYieldRequired[ portGET_CORE_ID() ] = pdTRUE;	\
	}														\
}

#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )
#define portYIELD() __asm volatile ( "SWI 0" ::: "memory" );

/* The critical section macros only mask interrupts up to an application
determined priority level.  Sometimes it is necessary to turn interrupt off in
the CPU itself before modifying certain hardware registers. */
#define portCPU_IRQ_DISABLE()								\
	__asm volatile ( "CPSID i" ::: "memory" );				\
	__asm volatile ( "DSB" );								\
	__asm volatile ( "ISB" );

#define portCPU_IRQ_ENABLE()								\
	__asm volatile ( "CPSIE i" ::: "memory" );				\
	__asm volatile ( "DSB" );								\
	__asm volatile ( "ISB" );


/*-----------------------------------------------------------
 * Critical section control
 *----------------------------------------------------------*/
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );

extern void xPortSpinLockTask( void );
extern void xPortSpinUnLockTask( void );
extern void vPortEnterCritical( void );
extern void vPortExitCritical( void );
extern uint32_t xPortGetCriticalState( void );
extern void vPortInstallFreeRTOSVectorTable( void );

extern uint32_t ulPortInterruptLock(void);
extern void ulPortInterruptUnLock(uint32_t key);

extern BaseType_t xPortCpuIsInInterrupt(void);

extern int pmu_secondary_cpu_state_is_running(uint32_t CoreID);
extern int pmu_secondary_cpu_state_is_hotplug(uint32_t CoreID);

#if ( configNUM_CORES == 1 )
#define portGET_TASK_LOCK()
#define portRELEASE_TASK_LOCK()
#define portENTER_CRITICAL()		vPortEnterCritical()
#define portEXIT_CRITICAL()			vPortExitCritical()
#else
#define portGET_TASK_LOCK()			xPortSpinLockTask()
#define portRELEASE_TASK_LOCK()		xPortSpinUnLockTask()
#define portENTER_CRITICAL()		vTaskEnterCritical()
#define portEXIT_CRITICAL()			vTaskExitCritical()
#endif

#define portDISABLE_INTERRUPTS()	ulPortInterruptLock()
#define portRESTORE_INTERRUPTS(x)	ulPortInterruptUnLock(x)
#define portENABLE_INTERRUPTS()		portCPU_IRQ_ENABLE()
#define portSET_INTERRUPT_MASK_FROM_ISR()		ulPortInterruptLock()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	ulPortInterruptUnLock(x)

#define portCHECK_IF_IN_ISR()		xPortCpuIsInInterrupt()

/*-----------------------------------------------------------*/

/* Tickless idle/low power functionality. */
#ifndef portSUPPRESS_TICKS_AND_SLEEP
	extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );
	#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime ) vPortSuppressTicksAndSleep( xExpectedIdleTime )
#endif

/*-----------------------------------------------------------*/
/* SMP utilities. */
#if ( configNUM_CORES == 1 )
#define portGET_CORE_ID()			0
#define portYIELD_CORE( x )
#else
#define portGET_CORE_ID()			( BaseType_t )ulPortGetCoreId()
#define portYIELD_CORE( x )			vPortYieldOtherCore( x )

void vPortYieldOtherCore( uint32_t xCoreID );
uint32_t ulPortGetCoreId(void);
#endif

/*-----------------------------------------------------------*/

/* Task function macros as described on the FreeRTOS.org WEB site.  These are
not required for this port but included in case common demo code that uses these
macros is used. */
#define portTASK_FUNCTION_PROTO( vFunction, pvParameters )	void vFunction( void *pvParameters )
#define portTASK_FUNCTION( vFunction, pvParameters )		void vFunction( void *pvParameters )

/* Prototype of the FreeRTOS tick handler.  This must be installed as the
handler for whichever peripheral is used to generate the RTOS tick. */
void FreeRTOS_Tick_Handler( void );

/* Prototype of the FreeRTOS IPI handler. */
void FreeRTOS_IPI_Handler( void );

void FreeRTOS_IPI_CPUHP_Handler( void );
void FreeRTOS_IPI_FLASHPG_Handler( void );

/* If configUSE_TASK_FPU_SUPPORT is set to 1 (or left undefined) then tasks are
created without an FPU context and must call vPortTaskUsesFPU() to give
themselves an FPU context before using any FPU instructions.  If
configUSE_TASK_FPU_SUPPORT is set to 2 then all tasks will have an FPU context
by default. */
#if( configUSE_TASK_FPU_SUPPORT != 2 )
	void vPortTaskUsesFPU( void );
#else
	/* Each task has an FPU context already, so define this function away to
	nothing to prevent it being called accidentally. */
	#define vPortTaskUsesFPU()
#endif
#define portTASK_USES_FLOATING_POINT() vPortTaskUsesFPU()

/* Architecture specific optimisations. */
#ifndef configUSE_PORT_OPTIMISED_TASK_SELECTION
	#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#endif

#if configUSE_PORT_OPTIMISED_TASK_SELECTION == 1

	/* Store/clear the ready priorities in a bit map. */
	#define portRECORD_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) |= ( 1UL << ( uxPriority ) )
	#define portRESET_READY_PRIORITY( uxPriority, uxReadyPriorities ) ( uxReadyPriorities ) &= ~( 1UL << ( uxPriority ) )

	/*-----------------------------------------------------------*/

	#define portGET_HIGHEST_PRIORITY( uxTopPriority, uxReadyPriorities ) uxTopPriority = ( 31UL - ( uint32_t ) __builtin_clz( uxReadyPriorities ) )

#endif /* configUSE_PORT_OPTIMISED_TASK_SELECTION */

#define portNOP() __asm volatile( "NOP" )
#define portINLINE __inline
#define portMEMORY_BARRIER() __asm volatile( "" ::: "memory" )

#ifdef __cplusplus
	} /* extern C */
#endif

/* Interrupt controller access addresses. */
#define portICCPMR_PRIORITY_MASK_OFFSET  					( 0x04 )
#define portICCBPR_BINARY_POINT_OFFSET						( 0x08 )
#define portICCIAR_INTERRUPT_ACKNOWLEDGE_OFFSET 			( 0x0C )
#define portICCEOIR_END_OF_INTERRUPT_OFFSET 				( 0x10 )
#define portICCRPR_RUNNING_PRIORITY_OFFSET					( 0x14 )

#define portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS 		( configINTERRUPT_CONTROLLER_BASE_ADDRESS + configINTERRUPT_CONTROLLER_CPU_INTERFACE_OFFSET )
#define portICCPMR_PRIORITY_MASK_REGISTER 					( *( ( volatile uint32_t * ) ( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCPMR_PRIORITY_MASK_OFFSET ) ) )
#define portICCIAR_INTERRUPT_ACKNOWLEDGE_REGISTER_ADDRESS 	( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCIAR_INTERRUPT_ACKNOWLEDGE_OFFSET )
#define portICCEOIR_END_OF_INTERRUPT_REGISTER_ADDRESS 		( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCEOIR_END_OF_INTERRUPT_OFFSET )
#define portICCPMR_PRIORITY_MASK_REGISTER_ADDRESS 			( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCPMR_PRIORITY_MASK_OFFSET )
#define portICCBPR_BINARY_POINT_REGISTER 					( *( ( const volatile uint32_t * ) ( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCBPR_BINARY_POINT_OFFSET ) ) )
#define portICCRPR_RUNNING_PRIORITY_REGISTER 				( *( ( const volatile uint32_t * ) ( portINTERRUPT_CONTROLLER_CPU_INTERFACE_ADDRESS + portICCRPR_RUNNING_PRIORITY_OFFSET ) ) )

#endif /* PORTMACRO_H */
