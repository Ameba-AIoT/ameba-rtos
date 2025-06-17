/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef _AMEBA_PMU_H_
#define _AMEBA_PMU_H_

enum PMU_DEVICE {
	PMU_OS				= 0,
	PMU_WLAN_DEVICE,
	PMU_KM4_RUN,
	PMU_KR4_RUN,
	PMU_DSP_RUN,
	PMU_WLAN_FW_DEVICE,
	PMU_BT_DEVICE,

	PMU_DEV_USER_BASE		= 7, /*number 7 ~ 31 is reserved for customer use*/
	PMU_MAX			= 31,
};

// default locked by OS and not to sleep until OS release wakelock in somewhere
#define DEFAULT_WAKELOCK		(BIT(PMU_OS))
#define DEFAULT_DEEP_WAKELOCK		(BIT(PMU_OS))
#define SLEEP_MAX_DELAY		(u32) 0xffffffffUL

typedef uint32_t (*PSM_HOOK_FUN)(uint32_t, void *param_ptr);

typedef struct {
	uint32_t					nDeviceId;
	PSM_HOOK_FUN		sleep_hook_fun;
	void				*sleep_param_ptr;
	PSM_HOOK_FUN		wakeup_hook_fun;
	void				*wakeup_param_ptr;
} PSM_DD_HOOK_INFO;

uint32_t pmu_yield_os_check(void);
uint32_t pmu_exec_sleep_hook_funs(void);
void pmu_exec_wakeup_hook_funs(uint32_t nDeviceIdMax);
uint32_t pmu_set_sleep_type(uint32_t type);
uint32_t pmu_get_sleep_type(void);
uint32_t pmu_get_sleep_time(void);
void pmu_set_max_sleep_time(uint32_t timer_ms);
void pmu_set_sleep_time_range(uint32_t min_time, uint32_t max_time);

void pmu_set_dsleep_active_time(uint32_t TimeOutMs);
void pmu_acquire_deepwakelock(uint32_t nDeviceId);
void pmu_release_deepwakelock(uint32_t nDeviceId);

uint32_t pmu_set_sysactive_time(uint32_t timeout);
void pmu_register_sleep_callback(uint32_t nDeviceId, PSM_HOOK_FUN sleep_hook_fun, void *sleep_param_ptr,
								 PSM_HOOK_FUN wakeup_hook_fun, void *wakeup_param_ptr);
void pmu_unregister_sleep_callback(uint32_t nDeviceId);

int pmu_ready_to_sleep(void);
int pmu_ready_to_dsleep(void);
int pmu_check_wakelock_timeout(void);

void pmu_pre_sleep_processing(uint32_t *expected_idle_time);
void pmu_post_sleep_processing(uint32_t *expected_idle_time);

void pmu_acquire_wakelock(uint32_t nDeviceId);
void pmu_release_wakelock(uint32_t nDeviceId);
uint32_t pmu_get_wakelock_status(void);
uint32_t pmu_get_deepwakelock_status(void);

extern void Systick_Cmd(uint32_t enable);
extern uint32_t SYSTIMER_GetPassTick(uint32_t start);

extern uint32_t sysactive_timeout_flag;
extern uint32_t system_can_yield;
#endif
