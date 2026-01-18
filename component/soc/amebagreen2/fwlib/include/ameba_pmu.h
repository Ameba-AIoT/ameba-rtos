/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PMU_H_
#define _AMEBA_PMU_H_

typedef enum {
	PMU_OS				= 0,
	PMU_WLAN_DEVICE,
	PMU_CPU0_RUN,
	PMU_CPU1_RUN,
	PMU_WLAN_FW_DEVICE,
	PMU_BT_CONTROLLER,
	PMU_BT_HOST,
	PMU_WHC_WIFI,
	PMU_DHCP_PROCESS,
	PMU_LWIP_STACK,
	PMU_DEV_USER_BASE,
	PMU_MAX
} PMU_DEVICE;

// default locked by OS and not to sleep until OS release wakelock in somewhere
#define DEFAULT_WAKELOCK		(BIT(PMU_OS))
#define DEFAULT_DEEP_WAKELOCK	(BIT(PMU_OS))
#define PMU_SLEEP_FOREVER		(uint32_t) 0xffffffffUL

typedef uint32_t (*PSM_HOOK_FUN)(uint32_t, void *param_ptr);

#define PMU_DEVICE_TIMER_DEFAULT_INTERVAL	2000
#define PMU_DEVICE_TIMER_MAX_INTERVAL		(1000*30) /* max 30sec */

typedef struct {
	uint32_t			nDeviceId;
	PSM_HOOK_FUN		sleep_hook_fun;
	void				*sleep_param_ptr;
	PSM_HOOK_FUN		wakeup_hook_fun;
	void				*wakeup_param_ptr;
} PSM_DD_HOOK_INFO;

uint32_t pmu_yield_os_check(void);
uint32_t pmu_exec_sleep_hook_funs(void);
void pmu_exec_wakeup_hook_funs(uint32_t nDeviceIdMax, uint32_t common_param);
uint32_t pmu_set_sleep_type(uint32_t type);
uint32_t pmu_get_sleep_type(void);
uint32_t pmu_get_sleep_time(void);
void pmu_set_max_sleep_time(uint32_t timer_ms);
void pmu_set_wakeup_timer(uint32_t timeout_ms);
void pmu_set_sleep_time_range(uint32_t min_time, uint32_t max_time);
#ifndef CONFIG_BUILD_ROM
void pmu_deepsleep_cmd(uint32_t NewStatus);
#endif

void pmu_set_dsleep_active_time(uint32_t TimeOutMs);
void pmu_acquire_deepwakelock(uint32_t nDeviceId);
void pmu_release_deepwakelock(uint32_t nDeviceId);

void pmu_set_dev_wakeup_tick(uint32_t nDeviceId, uint32_t Ms);
uint32_t pmu_set_sysactive_time(uint32_t timeout);
void pmu_register_sleep_callback(uint32_t nDeviceId, PSM_HOOK_FUN sleep_hook_fun, void *sleep_param_ptr,
								 PSM_HOOK_FUN wakeup_hook_fun, void *wakeup_param_ptr);
void pmu_unregister_sleep_callback(uint32_t nDeviceId);

int pmu_ready_to_sleep(void);
int pmu_check_wakelock_timeout(void);
int pmu_ready_to_dsleep(void);
void pmu_pre_sleep_processing(uint32_t *expected_idle_time);
void pmu_post_sleep_processing(uint32_t *expected_idle_time);

void pmu_acquire_wakelock(uint32_t nDeviceId);
void pmu_release_wakelock(uint32_t nDeviceId);
uint32_t pmu_get_wakelock_status(void);
uint32_t pmu_get_deepwakelock_status(void);

uint32_t pmc_wakeuptimer_int_hdl(void *Data);
void pmu_init_wakeup_timer(void);
void pmu_set_wakeup_timer(uint32_t timeout_ms);
void pmu_reset_wakeup_timer(void);

extern void SOCPS_HWSleepFilter(uint32_t enable);
extern void Systick_Cmd(uint32_t enable);
extern uint32_t SYSTIMER_GetPassTick(uint32_t start);

extern uint32_t system_can_yield;
extern uint32_t cur_device_id;
#endif
