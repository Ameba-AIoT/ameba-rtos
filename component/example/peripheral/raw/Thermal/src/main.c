
#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#define TEMP_DECIMAL_OUT(x)		((u32)((x) & 0x000003FF))

void TM_Display_Result(u32 Data)
{
	char s;
	u8 integer = 0;
	u16 decimal = 0;
	u32 temp = 0;

	if (Data >= 0x40000) {
		s = '-';
		temp = 0x80000 - Data;
		integer = (int)(temp >> 10);
		decimal = (int)(TEMP_DECIMAL_OUT(temp) * 10000 >> 10);
	} else {
		s = '+';
		integer = (int)(Data >> 10);
		decimal = (int)(TEMP_DECIMAL_OUT(Data) * 10000 >> 10);
	}
	printf("Temperature: %c%d.%4d\n", s, integer, decimal);
}


u32 TMIrqHandler(void *Data)
{
	u32 status = TM_GetISR();
	u32 value;
	(void)(Data);

	if (status & TM_BIT_ISR_TM_LOW_WT) {
		printf("Low warning temperature detect interrupt\n");
	}

	if (status & TM_BIT_ISR_TM_HIGH_WT) {
		printf("High warning temperature detect interrupt\n");

	}

	value = TM_GetTempResult();
	TM_Display_Result(value);

	TM_INTClearPendingBits(status);

	return 0;

}


_OPTIMIZE_NONE_
void raw_thermal_task(void)

{
	float tm;
	// thermal & aon timer clock and fuction enable here or modify in ameba_bootcfg.c
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
	TM_Cmd(ENABLE);

	TM_InitTypeDef TM_InitStruct;


	TM_StructInit(&TM_InitStruct);
	TM_InitStruct.TM_DownSampRate = 2;
	TM_InitStruct.TM_AdcClkDiv = 1;
	TM_InitStruct.TM_HighProtectThreshold = 125;
	TM_InitStruct.TM_HighWarnThreshold = 35;
	TM_InitStruct.TM_LowWarnThreshold = 30;
	TM_InitStruct.TM_TimePeriod = 15;
	TM_Init(&TM_InitStruct);

	InterruptRegister((IRQ_FUN)TMIrqHandler, THERMAL_IRQ, NULL, 5);
	InterruptEn(THERMAL_IRQ, 5);

	TM_Display_Result(TM_GetTempResult());

	tm = TM_GetCdegree(TM_GetTempResult());

	printf("Temperature = %4f C\n", tm);

	tm = TM_GetFdegree(TM_GetTempResult());

	printf("Temperature = %4f F\n", tm);

	while (1);

}

int main(void)
{
	if (rtos_task_create(NULL, "THERMAL DEMO", (rtos_task_t)raw_thermal_task, NULL, 3072, (1)) != SUCCESS) {
		printf("Cannot create thermal task\n\r");
	}

	rtos_sched_start();

	return 0;
}


