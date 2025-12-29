#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>
#include <math.h>

#define DEBOUNCE_TIME			CHIPEN_DBC_16MS
#define SHORTPRESS_TIME			CHIPEN_SP_100MS
#define LONGPRESS_TIME			CHIPEN_LP_3S
#define ACK_TIME				CHIPEN_ACK_200MS

// #define ACK_WND_KEEP_LP_INTR
#ifdef ACK_WND_KEEP_LP_INTR
#define CLEAR_DELAY_TIME_MS		((int)(50 * pow(2, ACK_TIME)) * 2)
#else
#define CLEAR_DELAY_TIME_MS		((int)(50 * pow(2, ACK_TIME)) / 2)
#endif

static u32 chipen_irq(UNUSED_WARN_DIS void *Data)
{
	static u8 cnt = 0;
	u32 IntrBit = CHIPEN_GetINT();

	if (IntrBit & AON_BIT_CHIPEN_SP_ISR) {
		printf("SP INT CPUID:%lu \n", SYS_CPUID());
	}

	if (IntrBit & AON_BIT_CHIPEN_LP_ISR) {
		printf("LP INT CPUID:%lu (cnt:%u) \n", SYS_CPUID(), ++cnt);
		DelayMs(CLEAR_DELAY_TIME_MS);
	}

	CHIPEN_ClearINT(IntrBit);

	if (cnt == 10) {
		pmu_acquire_wakelock(PMU_OS);
		printf("acquire lock, and switch to level reset mode. \n");

		CHIPEN_WorkMode(CHIPEN_HW_RESET_MODE);
	}
	return RTK_SUCCESS;
}

static void chipen_intr_task(void)
{
	rtos_time_delay_ms(10000);

	printf("chipen interrupt mode demo.\n");

	printf("Shortpress time:\t%lums\n", 50 * SHORTPRESS_TIME);
	printf("Longpress time:\t\t%lums\n", 1000 + 500 * LONGPRESS_TIME);
	printf("ACK time:\t\t\t%ums\n", (int)(50 * pow(2, ACK_TIME)));
	printf("Clear_int_delay time:\t%ums\n\n", CLEAR_DELAY_TIME_MS);

	/* 1. Set debouce time and shortpress/longpress threshold and ACK window. */
	CHIPEN_DebounceSet(DEBOUNCE_TIME);
	CHIPEN_ThresHoldSet(LONGPRESS_TIME, SHORTPRESS_TIME);
	CHIPEN_AckTimeSet(ACK_TIME);

	CHIPEN_WorkMode(CHIPEN_INT_RESET_MODE);
	/*2. Call API or modify sleepcfg.c to set wakeup mask. */
	SOCPS_SetAPWakeEvent(WAKE_SRC_PWR_DOWN, ENABLE);

	/* 3. Register irq handler */
	InterruptRegister((IRQ_FUN) chipen_irq, PWR_DOWN_IRQ, (u32)NULL, 3);
	InterruptEn(PWR_DOWN_IRQ, 3);

	rtos_task_delete(NULL);
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_reset_chipen_intr(void)
{
	if (rtos_task_create(NULL, "RESET CHIPEN INTR DEMO", (rtos_task_t)chipen_intr_task, NULL, 3072, (1)) != RTK_SUCCESS) {
		printf("Cannot create chipen_intr_task demo task\n\r");
	}

	return 0;
}
