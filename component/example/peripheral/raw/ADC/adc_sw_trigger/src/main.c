/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "adc_ext.h"
#include "os_wrapper.h"
#include <stdio.h>

#define ADC_CH_CNT	2
static u8 ADC_CHAN[ADC_CH_CNT] = {
	ADC_ID1,
	ADC_ID2
};

static u8 ADC_PAD[ADC_CH_CNT] = {
	ADC_PIN1,
	ADC_PIN2
};

void adc_swtrig_demo(void)
{
	ADC_InitTypeDef ADC_InitStruct;
	u32 value;
	u32 i;
	u32 adc_idx, adc_data;
	s32 adc_vol;

	/* Enable ADC clock and function */
	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_CTC, APBPeriph_CTC_CLOCK, ENABLE);
#endif

	/* Initialize ADC pad */
	for (i = 0; i < ADC_CH_CNT; i++) {
		Pinmux_Config(ADC_PAD[i], PINMUX_FUNCTION_ADC);
		PAD_PullCtrl(ADC_PAD[i], GPIO_PuPd_NOPULL);
		PAD_SleepPullCtrl(ADC_PAD[i], GPIO_PuPd_NOPULL);
		PAD_InputCtrl(ADC_PAD[i], DISABLE);
	}

	/* Initialize ADC to Software-trigger mode */
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_OpMode = ADC_SW_TRI_MODE;
	ADC_InitStruct.ADC_CvlistLen = ADC_CH_CNT - 1; /* ADC_CvlistLen should be channel number -1*/
	ADC_InitStruct.ADC_Cvlist[0] = ADC_CHAN[0];
	ADC_InitStruct.ADC_Cvlist[1] = ADC_CHAN[1];
#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	ADC_InitStruct.ADC_ChIDEn = ENABLE;
#endif
	ADC_Init(&ADC_InitStruct);

	ADC_Cmd(ENABLE);

	while (1) {
		/* SW trigger to switch channel and sample */
		ADC_SWTrigCmd(ENABLE);
		while (ADC_Readable() == 0);
		ADC_SWTrigCmd(DISABLE);

		while (ADC_Readable()) {

			value = ADC_Read();

			adc_idx = ADC_GET_CH_NUM_GLOBAL(value);
			adc_data = ADC_GET_DATA_GLOBAL(value);
			adc_vol = ADC_GetVoltage(adc_data);

			printf("AD%lu:0x%lx => %ldmV\n", adc_idx, adc_data, adc_vol);
		}

		DelayMs(500);
	}
}

int main(void)
{
	if (rtos_task_create(NULL, "RAW_ADC_SW_TRIG_TASK", (rtos_task_t)adc_swtrig_demo, NULL, (3072), (1)) != SUCCESS) {
		printf("Cannot create RAW_ADC_SW_TRIG_TASK \r\n");
	}

	rtos_sched_start();

	return 0;
}
