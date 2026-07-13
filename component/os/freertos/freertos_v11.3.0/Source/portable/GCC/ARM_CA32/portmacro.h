/*
 * FreeRTOS SMP Kernel V202110.00
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
#define portBYTE_ALIGNMENT			8
#define portBYTE_CACHE_ALIGNMENT		64
#define portBYTE_CACHE_ALIGNMENT_MASK	(portBYTE_CACHE_ALIGNMENT - 1)

#define portPrimaryCoreID			0

/*-----------------------------------------------------------*/
/* SMP utilities. */
#if ( configNUMBER_OF_CORES == 1 )
#define portGET_CORE_ID()           (0LU)
#define portYIELD_CORE( x )
#else
#define portGET_CORE_ID()           ( BaseType_t )ulPortGetCoreId()
#define portYIELD_CORE( x )         vPortYieldOtherCore( x )

void vPortYieldOtherCore( uint32_t xCoreID );
uint32_t ulPortGetCoreId(void);
#endif

/* Task utilities. */

/* Called at the end of an ISR that can cause a context switch. */
#define portEND_SWITCHING_ISR( xSwitchRequired )			\
{															\
extern uint32_t ulPortYieldRequired[ configNUMBER_OF_CORES ];		\
															\
	if( xSwitchRequired != pdFALSE )						\
	{														\
		ulPortYieldRequired[ portGET_CORE_ID() ] = pdTRUE;	\
	}														\
}

#define portYIELD_FROM_ISR( x ) portEND_SWITCHING_ISR( x )

/* Realtek: guard SWI against component-level critical sections.
 * portYIELD() triggers SWI (synchronous exception) which is NOT masked by
 * CPSID i.  When a component critical section (rtos_critical_enter/exit) is
 * held, firing SWI could migrate the task to a different core and corrupt
 * the per-cpu uxCriticalNestingCnt[] counter.
 * On SMP: if component nesting > 0, defer by setting xYieldPendings so that
 * vTaskYieldWithinAPI() in rtos_critical_exit() consumes it immediately after
 * the section unwinds.  xYieldPendings is used (not ulPortYieldRequired) so
 * that the deferred yield is serviced at the SWI exit of rtos_critical_exit,
 * not at the next IRQ. */
#if defined( RTOS_NUM_CORES ) && ( RTOS_NUM_CORES > 1 )
    extern uint32_t GetComponentCriticalNesting( uint32_t xCoreID );
    extern volatile BaseType_t xYieldPendings[ configNUMBER_OF_CORES ];
    #define portYIELD()                                                              \
        do {                                                                         \
            if( GetComponentCriticalNesting( ( uint32_t ) portGET_CORE_ID() ) == 0U ) \
            {                                                                        \
                __asm volatile ( "SWI 0" ::: "memory" );                            \
            }                                                                        \
            else                                                                     \
            {                                                                        \
                xYieldPendings[ portGET_CORE_ID() ] = pdTRUE;                       \
            }                                                                        \
        } while( 0 )
#else
    #define portYIELD() __asm volatile ( "SWI 0" ::: "memory" );
#endif

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

/*-----------------------------------------------------------*/


/*-----------------------------------------------------------
 * Critical section control
 *----------------------------------------------------------*/
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );

extern void xPortSpinLockTask( void );
extern void xPortSpinUnLockTask( void );
extern void vPortInstallFreeRTOSVectorTable( void );

extern uint32_t ulPortSetInterruptMask( void );
extern void vPortRestoreInterruptMask( uint32_t ulIntState );
extern BaseType_t xPortCpuIsInInterrupt( void );

#define portRTOS_LOCK_COUNT 2
#define portMAX_CORE_COUNT 2
#define portRTOS_SPINLOCK_COUNT 2

#define portENTER_CRITICAL()		vTaskEnterCritical()
#define portEXIT_CRITICAL()			vTaskExitCritical()
#define portENTER_CRITICAL_FROM_ISR    vTaskEnterCriticalFromISR
#define portEXIT_CRITICAL_FROM_ISR     vTaskExitCriticalFromISR

extern uint32_t xSpinLock[portRTOS_SPINLOCK_COUNT];
extern volatile uint32_t ucRecursionCountByLock[ portRTOS_SPINLOCK_COUNT ];
extern volatile uint32_t ucOwnedByCore[ portMAX_CORE_COUNT ];

uint32_t xGet(volatile uint32_t* x);
void vSet(volatile uint32_t* x, uint32_t value);

int32_t GateSmp_tryLock(volatile uint32_t* gateWord);
void GateSmp_unlock(volatile uint32_t* gateWord);

#if ( configNUMBER_OF_CORES == 1 )
#define portGET_TASK_LOCK( xCoreID )
#define portRELEASE_TASK_LOCK( xCoreID )
#define portGET_ISR_LOCK( xCoreID )
#define portRELEASE_ISR_LOCK( xCoreID )
#else

static inline void vPortRecursiveLock(uint32_t ulLockNum, uint32_t volatile *pxSpinLock, BaseType_t uxAcquire) {
	uint32_t ulCoreNum = portGET_CORE_ID();
	uint32_t ulLockBit = 1u << ulLockNum;

	/* Get the lock */
	if( uxAcquire )
	{
		/* Check if spinlock is available */
		if( GateSmp_tryLock(pxSpinLock) != 0 )
		{
			/* Check if the core owns the spinlock */
			if( xGet(&ucOwnedByCore[ulCoreNum]) & ulLockBit )
			{
				configASSERT(xGet(&ucRecursionCountByLock[ulLockNum]) != 255u );
				vSet( &ucRecursionCountByLock[ulLockNum], ( xGet(&ucRecursionCountByLock[ulLockNum] ) + 1) );
				return;
			}

			/* Preload the gate word into the cache */
			uint32_t dummy = (*pxSpinLock);
			dummy++;

			while ( GateSmp_tryLock(pxSpinLock) != 0 );
		}
		__asm volatile ("dmb" : : : "memory");

		/* The lock has been released and is acquired by current core */
		/* Set lock count to 1 and mark current core owns the lock */
		configASSERT( xGet(&ucRecursionCountByLock[ulLockNum]) == 0 );
		vSet( &ucRecursionCountByLock[ulLockNum], 1);
		vSet( &ucOwnedByCore[ulCoreNum], (xGet(&ucOwnedByCore[ulCoreNum]) | ulLockBit));
	}
	/* Release the lock */
	else
	{
		/* make sure the lock owner and recursive count are legal */
		configASSERT((xGet( &ucOwnedByCore[ulCoreNum] ) & ulLockBit) != 0 );
		configASSERT( xGet( &ucRecursionCountByLock[ulLockNum] ) != 0 );

		vSet( &ucRecursionCountByLock[ulLockNum], (xGet(&ucRecursionCountByLock[ulLockNum]) - 1) );

		if( !xGet( &ucRecursionCountByLock[ulLockNum] ) )
		{
			vSet( &ucOwnedByCore[ulCoreNum], ( xGet( &ucOwnedByCore[ulCoreNum] ) & ~ulLockBit ) );
			GateSmp_unlock(pxSpinLock);
			__asm volatile ("dmb" : : : "memory");
		}
	}
}

/* Index 0 is used for ISR lock and index 1 is used for task lock.
 * xCoreID parameter is accepted for v11.3.0 API compatibility; core ID is
 * resolved internally via portGET_CORE_ID() inside vPortRecursiveLock. */
#define portGET_ISR_LOCK( xCoreID )      vPortRecursiveLock(0, &xSpinLock[0], pdTRUE)
#define portRELEASE_ISR_LOCK( xCoreID )  vPortRecursiveLock(0, &xSpinLock[0], pdFALSE)
#define portGET_TASK_LOCK( xCoreID )     vPortRecursiveLock(1, &xSpinLock[1], pdTRUE)
#define portRELEASE_TASK_LOCK( xCoreID ) vPortRecursiveLock(1, &xSpinLock[1], pdFALSE)
#endif

#define portDISABLE_INTERRUPTS()	\
    __asm volatile ( "CPSID i" ::: "memory" );	\
	__asm volatile ("DSB");						\
	__asm volatile ("ISB")
#define portENABLE_INTERRUPTS()		\
    __asm volatile ( " cpsie i " ::: "memory" );\
	__asm volatile ("DSB");						\
	__asm volatile ("ISB")

#define portSET_INTERRUPT_MASK() 		ulPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK(x)		vPortRestoreInterruptMask(x)
#define portRESTORE_INTERRUPTS(x)		vPortRestoreInterruptMask(x)

#define portSET_INTERRUPT_MASK_FROM_ISR()		ulPortSetInterruptMask()
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x)	vPortRestoreInterruptMask(x)

#if ( configNUMBER_OF_CORES > 1 )
/* AmebaSmart need to fix this assert */
// #define portASSERT_IF_IN_ISR()		vPortAssertIfInISR()
// void vPortAssertIfInISR( void );
#endif

/*-----------------------------------------------------------*/

/* Get the critical nesting count of the calling task. */
UBaseType_t xPortGetCriticalState( void );

/*-----------------------------------------------------------*/

/* Tickless idle/low power functionality. */
#ifndef portSUPPRESS_TICKS_AND_SLEEP
	extern void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime );
	#define portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime ) vPortSuppressTicksAndSleep( xExpectedIdleTime )
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

#ifdef configASSERT
	void vPortValidateInterruptPriority( void );
	#define portASSERT_IF_INTERRUPT_PRIORITY_INVALID() 	vPortValidateInterruptPriority()
#endif /* configASSERT */

#define portNOP() __asm volatile( "NOP" )
#define portINLINE __inline
#define portMEMORY_BARRIER() __asm volatile( "" ::: "memory" )

#ifdef __cplusplus
	} /* extern C */
#endif

/* The number of bits to shift for an interrupt priority is dependent on the
number of bits implemented by the interrupt controller. */
#if configUNIQUE_INTERRUPT_PRIORITIES == 16
	#define portPRIORITY_SHIFT 4
	#define portMAX_BINARY_POINT_VALUE	3
#elif configUNIQUE_INTERRUPT_PRIORITIES == 32
	#define portPRIORITY_SHIFT 3
	#define portMAX_BINARY_POINT_VALUE	2
#elif configUNIQUE_INTERRUPT_PRIORITIES == 64
	#define portPRIORITY_SHIFT 2
	#define portMAX_BINARY_POINT_VALUE	1
#elif configUNIQUE_INTERRUPT_PRIORITIES == 128
	#define portPRIORITY_SHIFT 1
	#define portMAX_BINARY_POINT_VALUE	0
#elif configUNIQUE_INTERRUPT_PRIORITIES == 256
	#define portPRIORITY_SHIFT 0
	#define portMAX_BINARY_POINT_VALUE	0
#else
	#error Invalid configUNIQUE_INTERRUPT_PRIORITIES setting.  configUNIQUE_INTERRUPT_PRIORITIES must be set to the number of unique priorities implemented by the target hardware
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
