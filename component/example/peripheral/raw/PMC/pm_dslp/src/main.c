#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>

#define AON_TIMER_WAKEUP	0
#define AON_WAKEPIN_WAKEUP	1

/* choose wakepin idx */
//#define AON_WAKEPIN_IDX		WAKEPIN_0

/* choose wake up source, user should enble the source in amebalp_sleepcfg.c */
static u32 wakeup_source = AON_TIMER_WAKEUP;
static u32 dslptime = 10000;

extern SLEEP_ParamDef sleep_param;

static void aontimer_dslp_handler(void)
{
	printf("dslp wake from aontimer\n");

	SOCPS_AONTimerClearINT();
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);
}

static void wakepin_dslp_handler(void)
{
	u32 pinidx;
	printf("dslp wake from wakepin\n");

	pinidx = SOCPS_WakePinCheck();
	SOCPS_WakePinClearINT(pinidx);
}

static void dslp_wake_handler(void)
{
	u32 BootReason;

	BootReason = SOCPS_AONWakeReason();
	printf("DSLP WAKE REASON: %lx \n", BootReason);
	printf("BKUP_REG1's value = 0x%08lx \n", BKUP_Read(BKUP_REG1));
	if (BootReason & AON_BIT_TIM_ISR_EVT) {
		RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
		SOCPS_AONTimerINT_EN(ENABLE);
		InterruptRegister((IRQ_FUN)aontimer_dslp_handler, AON_TIM_IRQ, NULL, 3);
		InterruptEn(AON_TIM_IRQ, 3);
	}

#if defined(CONFIG_AMEBASMART)
	if (BootReason & (AON_BIT_GPIO_PIN0_WAKDET_EVT | AON_BIT_GPIO_PIN1_WAKDET_EVT | AON_BIT_GPIO_PIN2_WAKDET_EVT | AON_BIT_GPIO_PIN3_WAKDET_EVT))
#else
	if (BootReason & (AON_BIT_GPIO_PIN0_WAKDET_EVT | AON_BIT_GPIO_PIN1_WAKDET_EVT))
#endif
	{
		InterruptRegister((IRQ_FUN)wakepin_dslp_handler, AON_WAKEPIN_IRQ, NULL, 3);
		InterruptEn(AON_WAKEPIN_IRQ, 3);
	}
}


void app_pmu_init(void)
{
	/* For reference only, users can modify the function if need */
	/* Attention, the handler is needed to clear NVIC pending int and ip int in dslp flow */
	if (BOOT_Reason() & AON_BIT_RSTF_DSLP) {
		dslp_wake_handler();
		pmu_acquire_deepwakelock(PMU_OS);
	}

	/* KM4 need do pmc init */
#if defined (ARM_CORE_CM4)
	SOCPS_sleepInit();
#endif

	/*acquire wakelock to avoid AP enter sleep mode*/
	pmu_acquire_wakelock(PMU_OS);

#if defined(CONFIG_AMEBALITE)

	if (dsp_status_on()) {
		printf("need power off DSP!!!!!!!!!!!!\n");
	}
#endif
}

void pmu_deepsleep_wakeup_task(void)
{
	rtos_time_delay_ms(10000);

	printf("\nAP start enter deepsleep mode ============>\n");

	BKUP_Write(BKUP_REG1, 0x12345678);
	printf("Save 0x12345678 into BKUP_REG1\n");

	sleep_param.sleep_time = 0;
	switch (wakeup_source) {
	case AON_TIMER_WAKEUP:
		printf("set aon timer to wakeup\n");
		RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
		SOCPS_AONTimer(dslptime);
		SOCPS_AONTimerINT_EN(ENABLE);
		break;

	case AON_WAKEPIN_WAKEUP:
		/* enable aon wakepin to wakeup in ameba_sleepcfg.c */
		printf("set aon wakepin to wakeup\n");
		SOCPS_SetWakepinDebounce(100, ENABLE);
		//SOCPS_SetWakepin(AON_WAKEPIN_IDX, 1); // already done in sleepcfg, need if not config sleepcfg.c
		break;

	default:
		printf("Unknown wakeup source\n");
		break;
	}
	sleep_param.dlps_enable = TRUE;

	pmu_release_deepwakelock(PMU_OS);
	pmu_release_wakelock(PMU_OS);

	printf("lockbit:%lx \n", pmu_get_wakelock_status());
	printf("dslp_lockbit:%lx\n", pmu_get_deepwakelock_status());

	rtos_task_delete(NULL);
}

int main(void)
{
	app_pmu_init();

	/*for one round test, will keep active after wake from dslp */
	if ((BOOT_Reason() & AON_BIT_RSTF_DSLP) == 0) {
		if (rtos_task_create(NULL, "PMU PERIPHERAL WAKEUP DEMO", (rtos_task_t)pmu_deepsleep_wakeup_task, NULL, 3072, (1)) != SUCCESS) {
			printf("Cannot create pmu_deepsleep_wakeup_task demo task\n\r");
		}
	}

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
}
