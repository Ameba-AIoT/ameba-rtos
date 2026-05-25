#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>

#if defined(CONFIG_RTL8720F)
#define TM_BIT_ISR_TM_LOW_WT ADC_BIT_IT_TM_LOW_WT_STS
#define TM_BIT_ISR_TM_HIGH_WT ADC_BIT_IT_TM_HIGH_WT_STS
#define THERMAL_IRQ ADC_IRQ
#endif

void TM_Display_Result(u32 Data)
{
	char s;
	u8 integer = 0;
	u16 decimal = 0;

#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBAGREEN2)
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
#elif defined(CONFIG_RTL8720F)
	float Cdegree = TM_GetCdegree(Data);

	if (Cdegree >= 0) {
		s = '+';
		integer = (int)(Cdegree);
		decimal = (int)((Cdegree - integer) * 10000);
	} else {
		s = '-';
		integer = (int)(abs(Cdegree));
		decimal = (int)((abs(Cdegree) - integer) * 10000);
	}
#endif
	printf("Temperature: %c%d.%4d C\n", s, integer, decimal);
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
	/* note: %f cannot be printed in irq handler */
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
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBAGREEN2)
	TM_InitStruct.TM_DownSampRate = 2;
	TM_InitStruct.TM_AdcClkDiv = 1;
	TM_InitStruct.TM_HighProtectThreshold = 125; // 125 Celsius
	TM_InitStruct.TM_HighWarnThreshold = 50;	 // 50 Celsius
	TM_InitStruct.TM_LowWarnThreshold = 20;		 // 20 Celsius
	TM_InitStruct.TM_TimePeriod = 15;
#elif defined(CONFIG_RTL8720F)
	TM_InitStruct.TM_HighProtectThreshold = 841; // 125 Celsius
	TM_InitStruct.TM_HighWarnThreshold = 690;	 // 50 Celsius
	TM_InitStruct.TM_LowWarnThreshold = 630;	 // 20 Celsius
	TM_InitStruct.TM_TimePeriod = 0x8;			 // N * 0.65s = 5.2s
#endif

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

int example_raw_thermal(void)
{
	if (rtos_task_create(NULL, "THERMAL DEMO", (rtos_task_t)raw_thermal_task, NULL, 3072, (1)) != RTK_SUCCESS) {
		printf("Cannot create thermal task\n\r");
	}

	//rtos_sched_start();

	return 0;
}
