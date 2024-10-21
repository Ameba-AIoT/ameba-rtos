/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"

static const char *TAG = "PMC";

void SOCPS_SleepPG(void)
{
	u32 nDeviceIdOffset = 0;
	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset);


		RTK_LOGD(TAG, "DBG: CA32 Sleep PG blocked because Dev %lx  busy\n", nDeviceIdOffset);

		return;
	}

	RTK_LOGS(NOTAG, "APPG\n");

	SOCPS_SleepPG_LIB();

	RTK_LOGS(NOTAG, "APPW\n");
	/* exec sleep hook functions */
	pmu_exec_wakeup_hook_funs(PMU_MAX);

#if ( configNUM_CORES > 1 )
	/* Now init core1 */
	smp_init();
	pmu_set_secondary_cpu_state(1, CPU1_WAKE_FROM_PG);
#endif
	//pmu_acquire_wakelock(PMU_OS);
}

void SOCPS_SleepCG(void)
{
	u32 nDeviceIdOffset = 0;

	/* exec sleep hook functions */
	nDeviceIdOffset = pmu_exec_sleep_hook_funs();
	if (nDeviceIdOffset != PMU_MAX) {
		pmu_exec_wakeup_hook_funs(nDeviceIdOffset);

		RTK_LOGD(TAG, "DBG: CA32 Sleep CG blocked because Dev %lx  busy\n", nDeviceIdOffset);

		return;
	}

	RTK_LOGS(NOTAG, "APCG\n");

	SOCPS_SleepCG_LIB();

	RTK_LOGS(NOTAG, "APCW\n");

	/* exec sleep hook functions */
	pmu_exec_wakeup_hook_funs(PMU_MAX);
	//pmu_acquire_wakelock(PMU_OS);
}


