#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

static void wakepin_dslp_handler(void)
{
	u32 pinidx;
	printf("dslp wake from wakepin\n");

	pinidx = WakePin_Get_Idx();
	WakePin_ClearINT(pinidx);
}

int example_raw_pmc_wakepin(void)
{
	u32 Temp;

	/*for one round test, will keep active after wake from dslp */
	if ((BOOT_Reason() & AON_BIT_RSTF_DSLP) == 0) {
		/* config wakepin */
		printf("set aon wakepin to wakeup\n");
		Wakepin_Debounce_Setting(100, ENABLE);
		Wakepin_Setting(WAKEPIN_0, HIGH_LEVEL_WAKEUP);

		/* release wake lock and deep sleep wake lock */
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

		pmu_set_sysactive_time(5000);
		printf("enter deepsleep mode after 5S ============>\n");
	}

	/*interrupt should be registed every time*/
	InterruptRegister((IRQ_FUN)wakepin_dslp_handler, AON_WAKEPIN_IRQ, NULL, 3);
	InterruptEn(AON_WAKEPIN_IRQ, 3);

	return 0;
}
