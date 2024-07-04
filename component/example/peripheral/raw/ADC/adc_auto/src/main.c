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

#define ADC_BUF_SIZE	256
u32 BufferForADCRx[ADC_BUF_SIZE];
#define ADC_CH_CNT		2

static u8 ADC_CHAN[ADC_CH_CNT] = {
	ADC_ID1,
	ADC_ID2
};

static u8 ADC_PAD[ADC_CH_CNT] = {
	ADC_PIN1,
	ADC_PIN2
};

void adc_auto_demo(void)
{
	ADC_InitTypeDef ADC_InitStruct;
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

	/* Initialize ADC to auto mode to get continuous sample data */
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_OpMode = ADC_AUTO_MODE;
	ADC_InitStruct.ADC_CvlistLen = ADC_CH_CNT - 1; /* ADC_CvlistLen should be channel number -1*/
	ADC_InitStruct.ADC_Cvlist[0] = ADC_CHAN[0];
	ADC_InitStruct.ADC_Cvlist[1] = ADC_CHAN[1];
#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	ADC_InitStruct.ADC_ChIDEn = ENABLE;
#endif
	ADC_Init(&ADC_InitStruct);
	ADC_Cmd(ENABLE);

	/* Continuous Read data in auto mode */
	ADC_ReceiveBuf(BufferForADCRx, ADC_BUF_SIZE);

	while (1) {
		for (i = 0; i < ADC_BUF_SIZE; i++) {

			adc_idx = ADC_GET_CH_NUM_GLOBAL(BufferForADCRx[i]);
			adc_data = ADC_GET_DATA_GLOBAL(BufferForADCRx[i]);
			adc_vol = ADC_GetVoltage(adc_data);

			printf("AD%lu:0x%lx => %ldmV\n", adc_idx, adc_data, adc_vol);
		}

		rtos_task_delete(NULL);
	}
}

int main(void)
{
	if (rtos_task_create(NULL, "RAW_ADC_AUTO_TASK", (rtos_task_t)adc_auto_demo, NULL, (3072), (1)) != SUCCESS) {
		printf("Cannot create RAW_ADC_AUTO_TASK \r\n");
	}

	rtos_sched_start();

	return 0;
}
