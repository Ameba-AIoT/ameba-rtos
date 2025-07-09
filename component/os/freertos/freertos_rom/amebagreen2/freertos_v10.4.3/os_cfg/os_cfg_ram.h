/**
  ******************************************************************************
  * @file    os_cfg_ram.h
  * @author
  * @version V1.0.0
  * @date    2022-05-31
  * @brief   This file contains all the patch functions for rom code.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _OS_CFG_RAM_H_
#define _OS_CFG_RAM_H_

#ifdef CONFIG_PLATFORM_FREERTOS_ROM
#include "FreeRTOS.h"
#include "task.h"

/* struct _reent in TCB_t in ROM OS is fixed at 288B*/
#define ROM_OS_NEWLIB_REENT_SIZE	288

#if( configRUN_FREERTOS_SECURE_ONLY == 1 )
/**
 * @brief Initial EXC_RETURN value.
 *
 *     FF         FF         FF         FD
 * 1111 1111  1111 1111  1111 1111  1111 1101
 *
 * Bit[6] - 1 --> The exception was taken from the Secure state.
 * Bit[5] - 1 --> Do not skip stacking of additional state context.
 * Bit[4] - 1 --> The PE did not allocate space on the stack for FP context.
 * Bit[3] - 1 --> Return to the Thread mode.
 * Bit[2] - 1 --> Restore registers from the process stack.
 * Bit[1] - 0 --> Reserved, 0.
 * Bit[0] - 1 --> The exception was taken to the Secure state.
 */
#define portINITIAL_EXC_RETURN_RAM			( 0xfffffffd )
#else
/**
 * @brief Initial EXC_RETURN value.
 *
 *     FF         FF         FF         BC
 * 1111 1111  1111 1111  1111 1111  1011 1100
 *
 * Bit[6] - 0 --> The exception was taken from the Non-Secure state.
 * Bit[5] - 1 --> Do not skip stacking of additional state context.
 * Bit[4] - 1 --> The PE did not allocate space on the stack for FP context.
 * Bit[3] - 1 --> Return to the Thread mode.
 * Bit[2] - 1 --> Restore registers from the process stack.
 * Bit[1] - 0 --> Reserved, 0.
 * Bit[0] - 0 --> The exception was taken to the Non-Secure state.
 */
#define portINITIAL_EXC_RETURN_RAM			( 0xffffffbc )
#endif /* configRUN_FREERTOS_SECURE_ONLY */

#if defined(CONFIG_ARM_CORE_CM4_KM4TZ) && (CONFIG_ARM_CORE_CM4_KM4TZ == 1)

#if defined(CONFIG_WHC_DEV) || defined(CONFIG_WHC_NONE)
#define configTIMER_QUEUE_LENGTH_RAM				(10 + 64)
#else
#define configTIMER_QUEUE_LENGTH_RAM				10
#endif

#define configTIMER_TASK_STACK_DEPTH_RAM					( 512  )
#define configIDLE_TASK_STACK_DEPTH_RAM					( 512  )
#define configTICK_RATE_HZ_RAM								( ( TickType_t ) 1000 )
/* size of DaemonTaskMessage_t in timers.c */
#define DAEMONTASKMESSAGE_T_SIZE					(16)

#else  /* defined(CONFIG_ARM_CORE_CM4_KM4TZ) && (CONFIG_ARM_CORE_CM4_KM4TZ == 1) */

#if defined(CONFIG_WHC_DEV) || defined(CONFIG_WHC_NONE)
#define configTIMER_QUEUE_LENGTH_RAM				(10 + 64)
#else
#define configTIMER_QUEUE_LENGTH_RAM				10
#endif

#define configTIMER_TASK_STACK_DEPTH_RAM					( 512  )
#define configIDLE_TASK_STACK_DEPTH_RAM					( 512  )
#define configTICK_RATE_HZ_RAM								( ( TickType_t ) 1000 )
/* size of DaemonTaskMessage_t in timers.c */
#define DAEMONTASKMESSAGE_T_SIZE					(16)

#endif /* defined(CONFIG_ARM_CORE_CM4_KM4TZ) && (CONFIG_ARM_CORE_CM4_KM4TZ == 1) */

extern uint32_t port_initial_exc_return;
extern uint32_t msp_addr;
extern uint8_t *ucStaticTimerQueueStorage;
extern StaticTask_t *xIdleTaskTCB ;
extern StackType_t *uxIdleTaskStack;
extern StaticTask_t *xTimerTaskTCB;
extern StackType_t *uxTimerTaskStack;

void pmu_post_sleep_processing(TickType_t *tick_before_sleep);
void os_rom_init(void);

#endif /* CONFIG_PLATFORM_FREERTOS_ROM */

#endif /* _OS_CFG_RAM_H_ */
