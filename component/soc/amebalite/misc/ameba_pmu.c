/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/


#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "os_wrapper_specific.h"

static const char *const TAG = "PMU";

static uint32_t wakelock     = DEFAULT_WAKELOCK;
static uint32_t sleepwakelock_timeout     = 0;
static uint32_t sleep_type = SLEEP_PG; /* 0 is power gate, 1 is clock gate */
static uint32_t max_sleep_time = 0; /* if user want wakeup peridically, can set this timer*/
SLEEP_ParamDef sleep_param ALIGNMTO(32); /* cacheline aligned for lp & np */

static uint32_t deepwakelock     = DEFAULT_DEEP_WAKELOCK;
static uint32_t deepwakelock_timeout     = 0;
static uint32_t sysactive_timeout_temp = 0;

uint32_t system_can_yield = 1; /* default is can */
uint32_t sysactive_timeout_flag = 0;

static uint32_t timer_min_sleep_time = 0;
static uint32_t timer_max_sleep_time = 0;
/* ++++++++ FreeRTOS macro implementation ++++++++ */

/* psm dd hook info */
PSM_DD_HOOK_INFO gPsmDdHookInfo[PMU_MAX];

uint32_t pmu_exec_sleep_hook_funs(void)
{
	PSM_DD_HOOK_INFO *pPsmDdHookInfo = NULL;
	uint32_t nDeviceIdOffset = 0;
	u32 ret = TRUE;

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
static int pmu_systick_check(u32 time1, u32 time2)
{
	u32 delta = time1 > time2 ? time1 - time2 : time2 - time1;
	if (delta < SYSTICK_THRES) {
		return time1 >= time2 ? TRUE : FALSE;
	} else {	//overflow
		return time1 <= time2 ? TRUE : FALSE;
	}
}


uint32_t pmu_set_sysactive_time(uint32_t timeout)
{
	u32 TimeOut = 0;

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

	if (timeout != 0) {
		TimeOut = RTOS_CONVERT_MS_TO_TICKS(rtos_time_get_current_system_time_ms()) + timeout + RTOS_CONVERT_MS_TO_TICKS(rtos_time_get_current_pended_time_ms());

		if (pmu_systick_check(TimeOut, sleepwakelock_timeout)) {
			sleepwakelock_timeout = TimeOut;
		}
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

/*
 *  It is called in idle task.
 *
 *  @return  true  : System is ready to check conditions that if it can enter sleep.
 *           false : System keep awake.
 **/
int pmu_ready_to_sleep(void)
{
	u32 current_tick = rtos_time_get_current_system_time_ms();

	/* timeout */
	if (pmu_systick_check(current_tick, sleepwakelock_timeout) == FALSE) {
		return FALSE;
	}

	/* DSP shall PG firstly because DSP power is higher than AP */
	if (dsp_status_on()) {
		return FALSE;
	}


	if (wakelock == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

#if defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
int pmu_check_wakelock_timeout(void)
{
	u32 current_tick = rtos_time_get_current_system_time_ms();

	/* timeout */
	if (pmu_systick_check(current_tick, sleepwakelock_timeout) == FALSE) {
		return FALSE;
	} else {
		return TRUE;
	}

}
#endif


/*
 *  It is called in freertos pre_sleep_processing.
 *
 *  @return  true  : System is ready to check conditions that if it can enter dsleep.
 *           false : System can't enter deep sleep.
 **/
int pmu_ready_to_dsleep(void)
{
	u32 current_tick = rtos_time_get_current_system_time_ms();

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

void pmu_pre_sleep_processing(uint32_t *expected_idle_time)
{
	if (pmu_ready_to_dsleep()) {
		sleep_param.sleep_time = pmu_get_sleep_time();// do not wake on system schedule tick
		sleep_param.dlps_enable = ENABLE;
	} else {
#if defined(CONFIG_STANDARD_TICKLESS)
#if (defined(CONFIG_AP_CORE_KM4) && defined(CONFIG_ARM_CORE_CM4)) \
	|| (defined(CONFIG_AP_CORE_KR4) && defined(CONFIG_RSICV_CORE_KR4))
		sleep_param.sleep_time = *expected_idle_time;
#else
		sleep_param.sleep_time = pmu_get_sleep_time();//*expected_idle_time;
#endif
#else
		sleep_param.sleep_time = pmu_get_sleep_time();//*expected_idle_time;
#endif
		sleep_param.dlps_enable = DISABLE;
	}
	sleep_param.sleep_type = sleep_type;

	/*  Store gtimer timestamp before sleep */
	*expected_idle_time = SYSTIMER_TickGet();
	sysactive_timeout_flag = 1;

	if (sleep_type == SLEEP_PG) {
		SOCPS_SleepPG();
	} else {
		SOCPS_SleepCG();
	}
}

/* -------- FreeRTOS macro implementation -------- */

void pmu_acquire_wakelock(uint32_t nDeviceId)
{
	uint32_t PrevStatus = irq_disable_save();
	wakelock |= BIT(nDeviceId);
	irq_enable_restore(PrevStatus);
}

void pmu_release_wakelock(uint32_t nDeviceId)
{
	uint32_t PrevStatus = irq_disable_save();
	wakelock &= ~BIT(nDeviceId);
	irq_enable_restore(PrevStatus);
}

uint32_t pmu_get_wakelock_status(void)
{
	return wakelock;
}

uint32_t pmu_set_sleep_type(uint32_t type)
{
	sleep_type = type;

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

uint32_t pmu_get_sleep_time(void)
{
	u32 time = 0;
	if (timer_max_sleep_time > timer_min_sleep_time) {
		time = _rand() % (timer_max_sleep_time - timer_min_sleep_time + 1) + timer_min_sleep_time;
	} else if (timer_min_sleep_time != 0) {
		time = timer_min_sleep_time;
	}
	return time;
}

void pmu_set_sleep_time_range(uint32_t min_time, uint32_t max_time)
{
	timer_min_sleep_time = min_time;
	timer_max_sleep_time = max_time;
}

void pmu_set_dsleep_active_time(uint32_t TimeOutMs)
{
	u32 timeout = 0;

	timeout = rtos_time_get_current_system_time_ms() + TimeOutMs;
	RTK_LOGD(TAG, "pmu_set_dsleep_active_time: %d %d\n", timeout, deepwakelock_timeout);

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
