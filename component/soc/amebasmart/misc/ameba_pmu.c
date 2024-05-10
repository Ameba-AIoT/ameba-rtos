/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#include "FreeRTOS.h"
#include "task.h"
#include "platform_autoconf.h"
#include "ameba_soc.h"


static uint32_t wakelock     = DEFAULT_WAKELOCK;
static uint32_t sleepwakelock_timeout     = 0;
uint32_t system_can_yield = 1; /* default is can */
static uint32_t sleep_type = SLEEP_PG; /* 0 is power gate, 1 is clock gate */
static uint32_t max_sleep_time = 0; /* if user want wakeup peridically, can set this timer*/
SLEEP_ParamDef sleep_param ALIGNMTO(32); /* cacheline aligned for lp & np */

static uint32_t deepwakelock     = DEFAULT_DEEP_WAKELOCK;
static uint32_t deepwakelock_timeout     = 0;
static uint32_t sysactive_timeout_temp = 0;
uint32_t sysactive_timeout_flag = 0;


#ifdef ARM_CORE_CA32
/* cpu hotplug flag for each core */
volatile uint32_t cpuhp_flag[configNUM_CORES];
#endif

/* ++++++++ FreeRTOS macro implementation ++++++++ */

/* psm dd hook info */
PSM_DD_HOOK_INFO gPsmDdHookInfo[PMU_MAX];

uint32_t pmu_exec_sleep_hook_funs(void)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;
	uint32_t nDeviceIdOffset = 0;
	uint32_t ret = TRUE;

	for (nDeviceIdOffset = 0; nDeviceIdOffset < PMU_MAX; nDeviceIdOffset++) {
		pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceIdOffset];

		/*if this device register and sleep_hook_fun not NULL*/
		if (pPsmDdHookInfo && pPsmDdHookInfo->sleep_hook_fun) {
			ret = pPsmDdHookInfo->sleep_hook_fun(0, pPsmDdHookInfo->sleep_param_ptr);

			if (ret == FALSE) {
				break;
			}
		}
	}

	return nDeviceIdOffset;
}

void pmu_exec_wakeup_hook_funs(uint32_t nDeviceIdMax)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;
	uint32_t nDeviceIdOffset = 0;

	for (nDeviceIdOffset = 0; nDeviceIdOffset < nDeviceIdMax; nDeviceIdOffset++) {
		pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceIdOffset];

		/*if this device register and sleep_hook_fun not NULL*/
		if (pPsmDdHookInfo && pPsmDdHookInfo->wakeup_hook_fun) {
			pPsmDdHookInfo->wakeup_hook_fun(0, pPsmDdHookInfo->wakeup_param_ptr);
		}
	}
}

#define SYSTICK_THRES 0x7fffffff
/*
return: TRUE: time1 > time2
*/
static int pmu_systick_check(uint32_t time1, uint32_t time2)
{
	uint32_t delta = time1 > time2 ? time1 - time2 : time2 - time1;
	if (delta < SYSTICK_THRES) {
		return time1 >= time2 ? TRUE : FALSE;
	} else {	//overflow
		return time1 <= time2 ? TRUE : FALSE;
	}
}


uint32_t pmu_set_sysactive_time(uint32_t timeout)
{
	uint32_t TimeOut = 0;

	if (sysactive_timeout_flag) {
		if (timeout > sysactive_timeout_temp) {
			sysactive_timeout_temp = timeout;
		}
		return 1;
	}

	if (sysactive_timeout_temp > timeout) {
		timeout = sysactive_timeout_temp;
	}
	sysactive_timeout_temp = 0;

	TimeOut = rtos_time_get_current_system_time_ms() + timeout;

	if (pmu_systick_check(TimeOut, sleepwakelock_timeout)) {
		sleepwakelock_timeout = TimeOut;
	}

	return 0;
}

void pmu_register_sleep_callback(uint32_t nDeviceId, PSM_HOOK_FUN sleep_hook_fun, void *sleep_param_ptr,
								 PSM_HOOK_FUN wakeup_hook_fun, void *wakeup_param_ptr)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;

	assert_param(nDeviceId < PMU_MAX);
	assert_param((sleep_hook_fun != NULL) || (wakeup_hook_fun != NULL));

	pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceId];
	pPsmDdHookInfo->nDeviceId			= nDeviceId;
	pPsmDdHookInfo->sleep_hook_fun		= sleep_hook_fun;
	pPsmDdHookInfo->sleep_param_ptr		= sleep_param_ptr;
	pPsmDdHookInfo->wakeup_hook_fun		= wakeup_hook_fun;
	pPsmDdHookInfo->wakeup_param_ptr	= wakeup_param_ptr;
}

void pmu_unregister_sleep_callback(uint32_t nDeviceId)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;

	assert_param(nDeviceId < PMU_MAX);

	pPsmDdHookInfo = &gPsmDdHookInfo[nDeviceId];

	_memset(pPsmDdHookInfo, 0x00, sizeof(PSM_DD_HOOK_INFO));
}


/* can not yield CPU under suspend/resume process */
uint32_t pmu_yield_os_check(void)
{
	return system_can_yield;
}

#if defined (ARM_CORE_CM4)
uint32_t ap_clk_status_on(void)
{

	if (HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_CKE) & HSYS_BIT_CKE_AP) {
		return 1;
	} else {
		return 0;
	}
}

uint32_t ap_status_on(void)
{
	if ((HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC) & (HSYS_BIT_PSW_HP_AP_L2))) {
		return 1;
	} else {
		return 0;
	}
}
#endif

/*
 *  It is called in idle task.
 *
 *  @return  true  : System is ready to check conditions that if it can enter sleep.
 *           false : System keep awake.
 **/
int pmu_ready_to_sleep(void)
{
	uint32_t current_time = rtos_time_get_current_system_time_ms();

	/* timeout */
	if (pmu_systick_check(current_time, sleepwakelock_timeout) == FALSE) {
		return FALSE;
	}

#if defined (ARM_CORE_CM4)
	if ((HAL_READ8(SYSTEM_CTRL_BASE_LP, REG_LSYS_AP_STATUS_SW) & LSYS_BIT_AP_ENABLE)) {
		if (!(HAL_READ8(SYSTEM_CTRL_BASE_LP, REG_LSYS_AP_STATUS_SW) & LSYS_BIT_AP_RUNNING)) {
			if (! ap_status_on()) {
				pmu_set_sleep_type(SLEEP_PG);
			} else if (! ap_clk_status_on()) {
				pmu_set_sleep_type(SLEEP_CG);
			} else {
				return FALSE;
			}
			pmu_release_wakelock(PMU_OS);
		} else {
			return FALSE;
		}
	}
#endif

	if (wakelock == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*
 *  It is called in freertos pre_sleep_processing.
 *
 *  @return  true  : System is ready to check conditions that if it can enter dsleep.
 *           false : System can't enter deep sleep.
 **/
int pmu_ready_to_dsleep(void)
{
	uint32_t current_tick = xTaskGetTickCount();

	/* timeout */
	if (current_tick < deepwakelock_timeout) {
		return FALSE;
	}

	if (deepwakelock == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/*
 *  It is called when freertos is going to sleep.
 *  At this moment, all sleep conditons are satisfied. All freertos' sleep pre-processing are done.
 *
 *  @param  expected_idle_time : The time that FreeRTOS expect to sleep.
 *                               If we set this value to 0 then FreeRTOS will do nothing in its sleep function.
 **/
#if defined (ARM_CORE_CM4)
#if 0 //for longrun test
void pg_aontimer_int(uint32_t Data)
{
	DBG_8195A("pg Hp aontimer handler 1\n", SOCPS_AONWakeReason());
	SOCPS_AONTimerClearINT();
	DBG_8195A("pg Hp aontimer handler 2\n", SOCPS_AONWakeReason());
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);
}
#endif

void pmu_pre_sleep_processing(uint32_t *tick_before_sleep)
{
	if (pmu_ready_to_dsleep()) {
		sleep_param.sleep_time = 0;// do not wake on system schedule tick
		sleep_param.dlps_enable = ENABLE;
	} else {
		sleep_param.sleep_time = max_sleep_time;//*expected_idle_time;
		max_sleep_time = 0;
		sleep_param.dlps_enable = DISABLE;
	}
	sleep_param.sleep_type = sleep_type;

	/*  Store gtimer timestamp before sleep */
	*tick_before_sleep = SYSTIMER_TickGet();
	sysactive_timeout_flag = 1;

	/* for test */
#if 0
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
	uint32_t tmp = rand();
	SOCPS_AONTimer(tmp % 800 + 50);
	SOCPS_AONTimerINT_EN(ENABLE);
	InterruptRegister(pg_aontimer_int, AON_TIM_IRQ, NULL, 7);
	InterruptEn(AON_TIM_IRQ, 7);
	SOCPS_SetNPWakeEvent_MSK0_HP(WAKE_SRC_AON_TIM, ENABLE);
#endif

	if (sleep_type == SLEEP_PG) {
		SOCPS_SleepPG();
	} else {
		SOCPS_SleepCG();
	}

	//pmu_set_sysactive_time(5);
}

#elif defined (ARM_CORE_CA32)

void pmu_pre_sleep_processing(uint32_t *tick_before_sleep)
{
	if (pmu_ready_to_dsleep()) {
		sleep_param.sleep_time = 0;// do not wake on system schedule tick
		sleep_param.dlps_enable = ENABLE;
	} else {
		sleep_param.sleep_time = max_sleep_time;//*expected_idle_time;
		max_sleep_time = 0;
		sleep_param.dlps_enable = DISABLE;
	}

	sleep_param.sleep_type = sleep_type;

	/*  Store gtimer timestamp before sleep */
	*tick_before_sleep = SYSTIMER_TickGet();
	sysactive_timeout_flag = 1;

	if (sleep_type == SLEEP_CG) {
		SOCPS_SleepCG();
	} else {
		SOCPS_SleepPG();
	}
}
#else
void pmu_pre_sleep_processing(uint32_t *tick_before_sleep)
{
	/*  Store gtimer timestamp before sleep */
	*tick_before_sleep = SYSTIMER_TickGet();
	sysactive_timeout_flag = 1;

	/* some function call like xTaskGetTickCount may cause IRQ ON, */
	/* so we close IRQ again here to avoid sys irq when enter or exit sleep */
	//__asm volatile( "cpsid i" );

	SOCPS_SWRLDO_Suspend(ENABLE);

	/* Hsram sleep if whole sys goto sleep, for case KM0 only and keep hsram on */
	SOCPS_Hsram_Setting();

	if (sleep_type == SLEEP_PG) {
		SOCPS_SleepPG();
	} else {
		SOCPS_SleepCG();
	}
}
#endif

/* -------- FreeRTOS macro implementation -------- */

void pmu_acquire_wakelock(uint32_t nDeviceId)
{
	uint32_t PrevStatus;
#ifndef ARM_CORE_CA32
	PrevStatus = ulSetInterruptMaskFromISR();
#else
	PrevStatus = portDISABLE_INTERRUPTS();
#endif

	wakelock |= BIT(nDeviceId);

#ifndef ARM_CORE_CA32
	vClearInterruptMaskFromISR(PrevStatus);
#else
	portRESTORE_INTERRUPTS(PrevStatus);
#endif
}

void pmu_release_wakelock(uint32_t nDeviceId)
{
	uint32_t PrevStatus;
#ifndef ARM_CORE_CA32
	PrevStatus = ulSetInterruptMaskFromISR();
#else
	PrevStatus = portDISABLE_INTERRUPTS();
#endif

	wakelock &= ~BIT(nDeviceId);

#ifndef ARM_CORE_CA32
	vClearInterruptMaskFromISR(PrevStatus);
#else
	portRESTORE_INTERRUPTS(PrevStatus);
#endif
}

uint32_t pmu_get_wakelock_status(void)
{
	return wakelock;
}

uint32_t pmu_set_sleep_type(uint32_t type)
{
	sleep_type = type;
#if defined (ARM_CORE_CM0)
	if (ps_config.km0_pg_enable) {
		sleep_type = SLEEP_PG;
	}
#endif
	return 0;
}

uint32_t pmu_get_sleep_type(void)
{
	return sleep_type;
}

void pmu_set_max_sleep_time(uint32_t timer_ms)
{
	max_sleep_time = timer_ms;
}

void pmu_set_dsleep_active_time(uint32_t TimeOutMs)
{
	uint32_t timeout = 0;


	timeout = xTaskGetTickCount() + TimeOutMs;
	//DBG_8195A("pmu_set_dsleep_active_time: %d %d\n", timeout, deepwakelock_timeout);

	if (timeout > deepwakelock_timeout) {
		deepwakelock_timeout = timeout;
	}
}

void pmu_acquire_deepwakelock(uint32_t nDeviceId)
{
	deepwakelock |= BIT(nDeviceId);
}

void pmu_release_deepwakelock(uint32_t nDeviceId)
{
	deepwakelock &= ~BIT(nDeviceId);
}

uint32_t pmu_get_deepwakelock_status(void)
{
	return deepwakelock;
}

#ifdef ARM_CORE_CA32
void pmu_set_secondary_cpu_state(uint32_t CoreID, uint32_t NewStatus)
{
	cpuhp_flag[CoreID] = NewStatus;
}

uint32_t pmu_get_secondary_cpu_state(uint32_t CoreID)
{
	return cpuhp_flag[CoreID];
}

int pmu_secondary_cpu_state_is_running(uint32_t CoreID)
{
	return (cpuhp_flag[CoreID] == CPU1_RUNNING);
}

int pmu_secondary_cpu_state_is_hotplug(uint32_t CoreID)
{
	return (cpuhp_flag[CoreID] == CPU1_HOTPLUG);
}

int pmu_secondary_cpu_state_is_wake(uint32_t CoreID)
{
	return (cpuhp_flag[CoreID] == CPU1_WAKE_FROM_PG);
}
#endif

