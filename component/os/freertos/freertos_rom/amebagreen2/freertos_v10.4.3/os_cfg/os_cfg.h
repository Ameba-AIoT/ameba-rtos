/**
  ******************************************************************************
  * @file    os_cfg.h
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

#ifndef _OS_CFG_H_
#define _OS_CFG_H_

#include "basic_types.h"
#include "secure_context.h"

/* Exported constants --------------------------------------------------------*/
/** @defgroup ROM_PATCH_Exported_Constants ROM_PATCH Exported Constants
  * @{
  */

// add your constants here

struct platform_os_cfg {
	/* these functions can be patched or not */
	void (*patch_vApplicationMallocFailedHook)(void);
	void (*patch_vApplicationStackOverflowHook)(void *, char *);
	void (*patch_pvPortMalloc_NoReturn)(size_t);
	void *(*patch_pvPortMalloc)(size_t);
	void (*patch_vPortFree)(void *);
	u32 (*patch_vTaskStartScheduler)(void);
	u32 (*patch_vTaskSwitchContext)(void);
	u32 (*patch_prvIdleTask)(void);
	u32 (*patch_vApplicationIdleHook)(void);
	u32 (*patch_prvProcessExpiredTimer)(uint32_t xNextExpireTime, uint32_t xTimeNow);
	u32 (*patch_prvProcessTimerOrBlockTask)(uint32_t xNextExpireTime, uint32_t xListWasEmpty);
	u32 (*patch_prvSwitchTimerLists)(void);
	void *(*patch_xQueueGenericCreate)(uint32_t uxQueueLength, uint32_t uxItemSize, uint8_t ucQueueType);
	void (*patch_xQueueGenericCreate_NoReturn)(uint32_t uxQueueLength, uint32_t uxItemSize, uint8_t ucQueueType);
	u32 (*patch_vQueueDelete)(void *xQueue);

	/* these functions and viriables must be assinged, or freertos will not work */
	void (*vPortSleepHook)(u32);
	u32 idle_task_stack_size;			// unit: 4 bytes
	u32 timer_task_stack_size;			// unit: 4 bytes
	u32 timer_queue_length;
	u32 system_clock;
	u32 tick_rate_hz;
	u32 port_initial_exc_return;
} ;


#define configMINIMAL_STACK_SIZE						( os_cfg.idle_task_stack_size )
#define configTIMER_TASK_STACK_DEPTH					( os_cfg.timer_task_stack_size )
#define configTIMER_QUEUE_LENGTH						( os_cfg.timer_queue_length )
#define configTICK_RATE_HZ								( os_cfg.tick_rate_hz )
#define configCPU_CLOCK_HZ								( os_cfg.system_clock )


#define PATCH_VPORTFREE(pv)                                                   \
    if (os_cfg.patch_vPortFree) {                                             \
        if(os_cfg.patch_vPortFree(pv)) {                                      \
            return;                                                           \
        }                                                                     \
    } /* PATCH_VPORTFREE */


#define PATCH_PVPORTMALLOC_NORETRUN(xWantedSize)                                       \
    if (os_cfg.patch_pvPortMalloc_NoReturn) {                                          \
        os_cfg.patch_pvPortMalloc_NoReturn(xWantedSize);                        \
    } /* PATCH_PVPORTMALLOC_NORETRUN */


#define PATCH_PVPORTMALLOC(xWantedSize)                                       \
    if (os_cfg.patch_pvPortMalloc) {                                          \
        return os_cfg.patch_pvPortMalloc(xWantedSize);                        \
    } /* PATCH_PVPORTMALLOC */


#define PATCH_VAPPLICATIONMALLOCFAILEDHOOK()                                  \
    if (os_cfg.patch_vApplicationMallocFailedHook) {                          \
        os_cfg.patch_vApplicationMallocFailedHook();                          \
        return;                                                               \
    } /* PATCH_VAPPLICATIONMALLOCFAILEDHOOK */


#define PATCH_VAPPLICATIONSTACKOVERFLOWHOOK(pxTask, pcTaskName)               \
    if (os_cfg.patch_vApplicationStackOverflowHook) {                         \
        os_cfg.patch_vApplicationStackOverflowHook(pxTask, pcTaskName);       \
        return;                                                               \
    } /* PATCH_VAPPLICATIONMALLOCFAILEDHOOK */


#define PATCH_VTASKSTARTSCHEDULER()                                           \
    if (os_cfg.patch_vTaskStartScheduler) {                                   \
        if (os_cfg.patch_vTaskStartScheduler()) {                             \
            return;                                                           \
        }                                                                     \
    } /* PATCH_VTASKSTARTSCHEDULER */


#define PATCH_VTASKSWITCHCONTEXT()                                            \
    if (os_cfg.patch_vTaskSwitchContext) {                                    \
        if (os_cfg.patch_vTaskSwitchContext()){                               \
            return;                                                           \
        }                                                                     \
    } /* PATCH_VTASKSWITCHCONTEXT */


#define PATCH_PRVIDLETASK()                                                   \
    if (os_cfg.patch_prvIdleTask) {                                           \
        if (os_cfg.patch_prvIdleTask()) {                                     \
            return;                                                           \
        }                                                                     \
    } /* PATCH_PRVIDLETASK */


#define PATCH_VAPPLICATIONIDLEHOOK()                                          \
    if (os_cfg.patch_vApplicationIdleHook) {                                  \
        if (os_cfg.patch_vApplicationIdleHook()) {                            \
            return;                                                           \
        }                                                                     \
    } /* PATCH_VAPPLICATIONIDLEHOOK */


#define PATCH_VPORTSLEEPHOOK(xExpectedIdleTime)                               \
    if (os_cfg.vPortSleepHook) {                                              \
        os_cfg.vPortSleepHook(xExpectedIdleTime);                             \
    } /* PATCH_VPORTSLEEPHOOK */


#define PATCH_PRVPROCESSEXPIREDTIMER(xNextExpireTime, xTimeNow)                         \
    if (os_cfg.patch_prvProcessExpiredTimer) {                                          \
        if (os_cfg.patch_prvProcessExpiredTimer(xNextExpireTime, xTimeNow)) {           \
           return;                                                                      \
        }                                                                               \
    } /* PATCH_PRVPROCESSEXPIREDTIMER */


#define PATCH_PRVPROCESSTIMERORBLOCKTASK(xNextExpireTime, xListWasEmpty)                \
    if (os_cfg.patch_prvProcessTimerOrBlockTask) {                                      \
        if (os_cfg.patch_prvProcessTimerOrBlockTask(xNextExpireTime, xListWasEmpty)) {  \
            return;                                                                     \
        }                                                                               \
    } /* PATCH_PRVPROCESSTIMERORBLOCKTASK */


#define PATCH_PRVSWITCHTIMERLISTS()                                           \
    if (os_cfg.patch_prvSwitchTimerLists) {                                   \
        if (os_cfg.patch_prvSwitchTimerLists()) {                             \
            return;                                                           \
        }                                                                     \
    } /* PATCH_PRVSWITCHTIMERLISTS */


#define PATCH_XQUEUEGENERICCREATE_NORETURN(uxQueueLength, uxItemSize, ucQueueType)                 \
    if (os_cfg.patch_xQueueGenericCreate_NoReturn) {                                               \
        os_cfg.patch_xQueueGenericCreate_NoReturn(uxQueueLength, uxItemSize, ucQueueType);         \
    } /* PATCH_XQUEUEGENERICCREATE_NORETURN */


#define PATCH_XQUEUEGENERICCREATE(uxQueueLength, uxItemSize, ucQueueType)                 \
    if (os_cfg.patch_xQueueGenericCreate) {                                               \
        return os_cfg.patch_xQueueGenericCreate(uxQueueLength, uxItemSize, ucQueueType);   \
    } /* PATCH_XQUEUEGENERICCREATE */


#define PATCH_VQUEUEDELETE(xQueue)                                            \
    if (os_cfg.patch_vQueueDelete) {                                          \
        if (os_cfg.patch_vQueueDelete(xQueue)) {                              \
            return;                                                           \
        }                                                                     \
    } /* PATCH_VQUEUEDELETE */


/**
  * @}
  */

/* Exported functions -------------------------------------------------------- */
/** @defgroup ROM_PATCH_Exported_Functions ROM_PATCH Exported Functions
  * @{
  */

// add your function statements here

extern struct platform_os_cfg os_cfg;
extern SecureContextHandle_t (*patch_SecureContext_AllocateContext)(uint32_t ulSecureStackSize);
extern void (*patch_SecureContext_FreeContext)(SecureContextHandle_t xSecureContextHandle);
extern void (*patch_SecureInit_DePrioritizeNSExceptions)(void);
extern void (*patch_SecureContext_LoadContext)(SecureContextHandle_t xSecureContextHandle);
extern void (*patch_SecureContext_SaveContext)(SecureContextHandle_t xSecureContextHandle);
extern void (*patch_SecureContext_Init)(void);
extern void (*patch_vPortAllocateSecureContext)(uint32_t ulSecureStackSize);
extern void (*patch_vPortCleanUpTCB)(uint32_t *pulTCB);
extern void (*patch_traceTaskCreate)(uint32_t *pxNewTCB);
extern void (*patch_traceTaskDelete)(uint32_t *pxTCB);
extern void (*patch_newlib_assign_1)(uint32_t *pxNewLib_reent);
extern void (*patch_newlib_assign_2)(uint32_t *pxNewLib_reent);
extern void (*patch_newlib_reent_init_ptr)(uint32_t *pxNewLib_reent);
extern void (*patch_newlib_reclaim_reent)(uint32_t *pxNewLib_reent);
extern u32 (*patch_portDisable_Interrupts)(void);
extern void (*patch_portEnable_Interrupts)(uint32_t val);


void os_rom_init(void);
void SecureContext_Init_rom(void);
void SecureInit_DePrioritizeNSExceptions_rom(void);
SecureContextHandle_t SecureContext_AllocateContext_rom(uint32_t ulSecureStackSize);
void SecureContext_FreeContext_rom(SecureContextHandle_t xSecureContextHandle);
void SecureContext_LoadContext_rom(SecureContextHandle_t xSecureContextHandle);
void SecureContext_SaveContext_rom(SecureContextHandle_t xSecureContextHandle);
void vPortAllocateSecureContext_rom(uint32_t ulSecureStackSize);
void vPortCleanUpTCB_rom(uint32_t *pulTCB);
void traceTaskCreate_rom(uint32_t *pxNewTCB);
void traceTaskDelete_rom(uint32_t *pxTCB);
void newlib_assign_1_rom(uint32_t *pxNewLib_reent);
void newlib_assign_2_rom(uint32_t *pxNewLib_reent);
void newlib_reent_init_ptr_rom(uint32_t *pxNewLib_reent);
void newlib_reclaim_reent_rom(uint32_t *pxNewLib_reent);


/**
  * @}
  */

#endif /* _OS_CFG_H_ */
/******************* (C) COPYRIGHT 2022 Realtek Semiconductor *****END OF FILE****/

