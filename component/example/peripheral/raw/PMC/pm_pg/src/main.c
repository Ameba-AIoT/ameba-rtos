#include "ameba_soc.h"
#include "main.h"
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
	SOCPS_AONTimerClearINT();
	printf("aontimer handler 2: %x\n", SOCPS_AONWakeReason());
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);

	printf("PG wakeup test pass!!!!!!!!!!!!\n");

	pmu_acquire_wakelock(PMU_OS);

	return 0;
}

void pmu_aontimer_test(void)
{
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
	SOCPS_AONTimer(WAKEUP_TIME);
	SOCPS_AONTimerINT_EN(ENABLE);
	InterruptRegister((IRQ_FUN)pmu_aontimer_int, AON_TIM_IRQ, NULL, 3);
	InterruptEn(AON_TIM_IRQ, 3);

#if defined(CONFIG_AMEBALITE)
	SOCPS_SetAPWakeEvent_MSK1(WAKE_SRC_AON_TIM, ENABLE);
#elif defined(CONFIG_AMEBASMART)
	SOCPS_SetAPWakeEvent_MSK0(WAKE_SRC_AON_TIM, ENABLE);
#elif defined(CONFIG_AMEBADPLUS)
	SOCPS_SetAPWakeEvent(WAKE_SRC_AON_TIM, ENABLE);
#endif

}

void app_pmu_init(void)
{
	/* KM4 need do pmc init */
#if defined (ARM_CORE_CM4)
	SOCPS_sleepInit();
#endif

	/*set AP sleep type*/
	pmu_set_sleep_type(SLEEP_PG);
	/*acquire wakelock to avoid AP enter sleep mode*/
	pmu_acquire_wakelock(PMU_OS);

#if defined(CONFIG_AMEBALITE)
	if (dsp_status_on()) {
		printf("need power off DSP!!!!!!!!!!!!\n");
	}
#endif
}

void pmu_aontimer_wakeup_task(void)
{
	rtos_time_delay_ms(10000);

	pmu_register_sleep_callback(PMU_DEV_USER_BASE, (PSM_HOOK_FUN)app_peripheral_suspend, NULL, (PSM_HOOK_FUN)app_peripheral_resume, NULL);

	pmu_aontimer_test();

	/* release wakelock, to make CPU enter sleep mode */
	pmu_release_wakelock(PMU_OS);

	printf("lockbit:%lx \n", pmu_get_wakelock_status());

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
	app_pmu_init();

	if (rtos_task_create(NULL, "PMU PERIPHERAL WAKEUP DEMO", (rtos_task_t)pmu_aontimer_wakeup_task, NULL, 3072, (1)) != SUCCESS) {
		printf("Cannot create pmu_aontimer_wakeup_task demo task\n\r");
	}

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
}
