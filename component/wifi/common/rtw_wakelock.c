/**
  ******************************************************************************
  * @file    rtw_wakelock.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include <basic_types.h>
#include <os_wrapper.h>
#include <ameba_pmu.h>

void rtw_acquire_wakelock(void)
{
	if (pmu_yield_os_check()) {
		pmu_acquire_wakelock(PMU_WLAN_DEVICE);
	}
}

void rtw_release_wakelock(void)
{
	if (pmu_yield_os_check()) {
		pmu_release_wakelock(PMU_WLAN_DEVICE);
	}
}

void rtw_wakelock_timeout(u32 timeoutms)
{
	if (pmu_yield_os_check()) {
		pmu_set_sysactive_time(timeoutms);
	}
}

