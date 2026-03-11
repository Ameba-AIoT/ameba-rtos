#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

static u32 dslptime = 5000; // wakeup after 5000ms

static void aontimer_dslp_handler(void)
{
	printf("dslp wake from aontimer\n");

	AONTimer_ClearINT();
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);
	printf("BKUP_REG1's value = 0x%08lx \n", BKUP_Read(BKUP_REG1));
}

void example_raw_pmc_dslp(void)
{
	u32 Temp = 0;

	if ((BOOT_Reason() & AON_BIT_RSTF_DSLP) == 0) {
		printf("enter deepsleep mode after 5S ============>\n");
		rtos_time_delay_ms(5000);
		/*Backup reg can save information in deepsleep mode.*/
		printf("Save 0x12345678 into BKUP_REG1\n");
		BKUP_Write(BKUP_REG1, 0x12345678);

		printf("set aon timer to wakeup\n");
		/*1. Init peripheral parameters*/
		RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
		AONTimer_Setting(dslptime);
		pmu_set_max_sleep_time(dslptime);
		AONTimer_INT(ENABLE);
		/*2. Release wakelock and deepwakelock*/
		pmu_release_deepwakelock(PMU_OS);
		pmu_release_wakelock(PMU_OS);

		/* check whether some wake lock bit is still set*/
		Temp = pmu_get_wakelock_status();
		if (Temp) {
			printf("Sleep Fail Because wake lock bit:%lx\n", Temp);
		}
		Temp = pmu_get_deepwakelock_status();
		if (Temp) {
			printf("Sleep Fail Because deepsleep wake lock bit:%lx\n", Temp);
		}
	}
	/*3.Register interrupt for processing.*/
	InterruptRegister((IRQ_FUN)aontimer_dslp_handler, AON_TIM_IRQ, NULL, 3);
	InterruptEn(AON_TIM_IRQ, 3);
}
