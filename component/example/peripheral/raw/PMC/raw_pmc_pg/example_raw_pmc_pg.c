#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define WAKEUP_TIME 2000 //ms

static u32 app_peripheral_suspend(u32 expected_idle_time, void *param)
{
	(void) expected_idle_time;
	(void) param;

	// For peripherals that need turned off before sleep, call disable or deinit peripheral here
	printf("\nAP start enter sleep mode ============>\n");
	printf("app_peripheral_suspend\n");

	return TRUE;
}

static u32 app_peripheral_resume(u32 expected_idle_time, void *param)
{
	(void) expected_idle_time;
	(void) param;

	// For peripherals that are turned off before sleep, call enable or init peripheral here
	printf("app_peripheral_resume\n");

#if defined(CONFIG_AMEBADPLUS)
	printf("wake status - %lx\n", HAL_READ32(PMC_BASE, WAK_STATUS0));
#else
	printf("wake status - %lx %lx\n", HAL_READ32(PMC_BASE, WAK_STATUS0), HAL_READ32(PMC_BASE, WAK_STATUS1));
#endif
	/*set AP active time*/
	pmu_set_sysactive_time(5000);
	return TRUE;
}

u32 pmu_aontimer_int(void *Data)
{
	(void)Data;

	printf("aontimer handler 1: %x\n", SOCPS_AONWakeReason());
	AONTimer_ClearINT();

	printf("aontimer handler 2: %x\n", SOCPS_AONWakeReason());
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);

	printf("PG wakeup test pass!!!!!!!!!!!!\n");

	pmu_acquire_wakelock(PMU_OS);

	return 0;
}


void pmu_aontimer_wakeup_task(void)
{
	rtos_time_delay_ms(10000);

	/*1. Init peripheral parameters*/
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);

	AONTimer_Setting(WAKEUP_TIME);
	AONTimer_INT(ENABLE);

	InterruptRegister((IRQ_FUN)pmu_aontimer_int, AON_TIM_IRQ, NULL, 3);
	InterruptEn(AON_TIM_IRQ, 3);
	/*2. Call API or modify sleepcfg.c to set wakeup mask.*/
#if defined(CONFIG_AMEBALITE)
	SOCPS_SetAPWakeEvent_MSK1(WAKE_SRC_AON_TIM, ENABLE);
#elif defined(CONFIG_AMEBASMART)
	SOCPS_SetAPWakeEvent_MSK0(WAKE_SRC_AON_TIM, ENABLE);
#elif defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBAGREEN2)
	SOCPS_SetAPWakeEvent(WAKE_SRC_AON_TIM, ENABLE);
#endif
	/*3. (optional) Register sleep hook*/
	pmu_register_sleep_callback(PMU_DEV_USER_BASE, (PSM_HOOK_FUN)app_peripheral_suspend, NULL, (PSM_HOOK_FUN)app_peripheral_resume, NULL);

	/*4. (optional) Modify the ameba_sleepcfg.c if current wake-up source requires setting clock and voltage parameters.*/
	/*5. Set sleep type*/
	pmu_set_sleep_type(SLEEP_PG);
	/*6. Release wakelock, to make CPU enter sleep mode */
	pmu_release_wakelock(PMU_OS);

	printf("lockbit:%lx \n", pmu_get_wakelock_status());

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_pmc_pg(void)
{
	if (rtos_task_create(NULL, "PMU PERIPHERAL WAKEUP DEMO", (rtos_task_t)pmu_aontimer_wakeup_task, NULL, 3072, (1)) != RTK_SUCCESS) {
		printf("Cannot create pmu_aontimer_wakeup_task demo task\n\r");
	}

	return 0;
}
