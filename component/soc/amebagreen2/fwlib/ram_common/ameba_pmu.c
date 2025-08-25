/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#include "ameba_soc.h"

uint32_t missing_tick = 0;

static uint32_t wakelock     = DEFAULT_WAKELOCK;
static uint32_t sleep_type = SLEEP_PG; /* 0 is power gate, 1 is clock gate */
static uint32_t max_sleep_time = 0; /* if user want wakeup peridically, can set this timer*/
SLEEP_ParamDef sleep_param ALIGNMTO(32); /* cacheline aligned for lp & np */

static uint32_t deepwakelock     = DEFAULT_DEEP_WAKELOCK;

uint32_t system_can_yield = 1; /* default is can */
static uint32_t timer_min_sleep_time = 0;
static uint32_t timer_max_sleep_time = 0;
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

uint32_t pmu_set_sysactive_time(uint32_t timeout)
{
	uint32_t New_Cnt = 0;
	PMCTIMER_TpyeDef *PMC_TIMER = PMC_TIMER_DEV;

	New_Cnt = (u32)((((u64)timeout) << 15) / 1000); /*convert ms to cnt*/
	PMCTimerCnt_Set(PMC_TIMER, PMC_SLEEP_TIMER, New_Cnt);

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

//for test
#if 0
u32 aontimer_int(void *Data)
{
	(void)Data;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "aontimer handler 1: %x\n", SOCPS_AONWakeReason());
	AONTimer_ClearINT();
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "aontimer handler 2: %x\n", SOCPS_AONWakeReason());
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);
	return 0;
}

void aontimer_test(void)
{
#if defined (CONFIG_ARM_CORE_CM4)
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
	AONTimer_Setting(2000);
	AONTimer_INT(ENABLE);
	InterruptRegister((IRQ_FUN)aontimer_int, AON_TIM_IRQ, NULL, 3);
	InterruptEn(AON_TIM_IRQ, 3);
	SOCPS_SetAPWakeEvent(WAKE_SRC_AON_TIM, ENABLE);
	SOCPS_SetNPWakeEvent(WAKE_SRC_AP_WAKE_IRQ, ENABLE);
#else
#endif
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
	PMCTIMER_TpyeDef *PMC_TIMER = PMC_TIMER_DEV;

	/* check timeout */
	if ((PMCTimer_Valid_Check(PMC_TIMER, PMC_SLEEP_TIMER) == FALSE) || (PMCTimerCnt_Get(PMC_TIMER, PMC_SLEEP_TIMER) > 0)) {
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
	PMCTIMER_TpyeDef *PMC_TIMER = PMC_TIMER_DEV;

	/* check timeout */
	if ((PMCTimer_Valid_Check(PMC_TIMER, PMC_SLEEP_TIMER) == FALSE) || (PMCTimerCnt_Get(PMC_TIMER, PMC_SLEEP_TIMER) > 0)) {
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
	PMCTIMER_TpyeDef *PMC_TIMER = PMC_TIMER_DEV;

	/* check timeout */
	if ((PMCTimer_Valid_Check(PMC_TIMER, PMC_DSLP_TIMER) == FALSE) || (PMCTimerCnt_Get(PMC_TIMER, PMC_DSLP_TIMER) > 0)) {
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

void pmu_pre_sleep_processing(uint32_t *tick_before_sleep)
{
	if (pmu_ready_to_dsleep()) {
		sleep_param.sleep_time = pmu_get_sleep_time();// do not wake on system schedule tick
		sleep_param.dlps_enable = ENABLE;
	} else {
		sleep_param.sleep_time = pmu_get_sleep_time();//*expected_idle_time;
		sleep_param.dlps_enable = DISABLE;
	}

	/*  Store gtimer timestamp before sleep */
	*tick_before_sleep = SYSTIMER_TickGet();
	/*Disable systick interrupt to avoid interrupting sleep flow*/
	Systick_Cmd(DISABLE);

#if defined(CONFIG_WIFI_HOST_CONTROL)
	SOCPS_Sleep_FULLMAC(sleep_type);
#else
	if (sleep_type == SLEEP_PG) {
		SOCPS_SleepPG();
	} else {
		SOCPS_SleepCG();
	}
#endif
	Systick_Cmd(ENABLE);
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
u32 pmc_wakeuptimer_int_hdl(void *Data)
{
	UNUSED(Data);
	PMCTimer_INTClear(PMC_TIMER_DEV, PMC_WAKEUP_TIMER);
	return TRUE;
}

void pmu_init_wakeup_timer(void)
{
	InterruptRegister(pmc_wakeuptimer_int_hdl, PMC_TIMER_IRQ, NULL, PMC_TIMER_INT_PRIO);
	InterruptEn(PMC_TIMER_IRQ, PMC_TIMER_INT_PRIO);
	PMCTimer_INTConfig(PMC_TIMER_DEV, PMC_WAKEUP_TIMER, ENABLE);
}

void pmu_set_wakeup_timer(uint32_t timeout_ms)
{
	uint32_t timeout_cnt = 0;
	PMCTIMER_TpyeDef *PMC_TIMER = PMC_TIMER_DEV;

	timeout_cnt = (timeout_ms << 15) / 1000;  /*convert ms to cnt*/

	PMCTimerCnt_Set(PMC_TIMER, PMC_WAKEUP_TIMER, timeout_cnt);
}

void pmu_reset_wakeup_timer(void)
{
	PMCTIMER_TpyeDef *PMC_TIMER = PMC_TIMER_DEV;

	PMCTimerCnt_Reset(PMC_TIMER, PMC_WAKEUP_TIMER);
}

void pmu_set_dsleep_active_time(uint32_t TimeOutMs)
{
	uint32_t New_Cnt = 0;
	PMCTIMER_TpyeDef *PMC_TIMER = PMC_TIMER_DEV;

	New_Cnt = (TimeOutMs << 15) / 1000;  /*convert ms to cnt*/
	PMCTimerCnt_Set(PMC_TIMER, PMC_DSLP_TIMER, New_Cnt);
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
