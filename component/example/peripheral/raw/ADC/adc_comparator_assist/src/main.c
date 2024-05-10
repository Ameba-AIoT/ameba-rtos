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

#define CMP_TIM_IDX		3
#define PERIOD_MS		100

#define ADC_CH_CNT		2

static u8 ADC_CHAN[ADC_CH_CNT] = {
	ADC_ID1,
	ADC_ID2
};

static u8 ADC_PAD[ADC_CH_CNT] = {
	ADC_PIN1,
	ADC_PIN2
};

void CMPIrqHandle(void)
{
	u32 value;
	u32 adc_idx, adc_data;
	s32 adc_vol;
	u32 sts = CMP_GetISR();

	while (ADC_Readable() == 0);

	value = ADC_Read();

	adc_idx = ADC_GET_CH_NUM_GLOBAL(value);
	adc_data = ADC_GET_DATA_GLOBAL(value);
	adc_vol = ADC_GetVoltage(adc_data);

	printf("AD%lu:0x%lx => %ldmV\n", adc_idx, adc_data, adc_vol);

	CMP_INTClearPendingBit(sts);
}

void adc_comp_demo(void)
{
	ADC_InitTypeDef ADC_InitStruct;
	CMP_InitTypeDef CMP_InitStruct;
	u32 i;

	/* Enable ADC clock and function */
	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_CTC, APBPeriph_CTC_CLOCK, ENABLE);
#endif
	RCC_PeriphClockCmd(APBPeriph_TIMx[CMP_TIM_IDX], APBPeriph_TIMx_CLOCK[CMP_TIM_IDX], ENABLE);

	/* Initialize ADC pad */
	for (i = 0; i < ADC_CH_CNT; i++) {
		Pinmux_Config(ADC_PAD[i], PINMUX_FUNCTION_ADC);
		PAD_PullCtrl(ADC_PAD[i], GPIO_PuPd_NOPULL);
		PAD_SleepPullCtrl(ADC_PAD[i], GPIO_PuPd_NOPULL);
		PAD_InputCtrl(ADC_PAD[i], DISABLE);
	}

	/* Initialize ADC to comparator-assist mode. ADC is in low-power mode until Comparator
	sends wakeup signal to ADC */
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_OpMode = ADC_COMP_ASSIST_MODE;
	ADC_InitStruct.ADC_CvlistLen = ADC_CH_CNT - 1; /* ADC_CvlistLen should be channel number -1*/
	ADC_InitStruct.ADC_Cvlist[0] = ADC_CHAN[0];
	ADC_InitStruct.ADC_Cvlist[1] = ADC_CHAN[1];
#if defined (CONFIG_AMEBASMART) || defined (CONFIG_AMEBALITE)
	ADC_InitStruct.ADC_ChIDEn = ENABLE;
#endif
	ADC_Init(&ADC_InitStruct);
	ADC_Cmd(ENABLE);

	/* Initialize Comparator to timer-trigger mode to scan channels */
	CMP_StructInit(&CMP_InitStruct);
	for (i = 0; i < ADC_CH_CNT; i++) {

		/* comparator channel can only be 0~3 */
		assert_param(ADC_CHAN[i] < COMP_CH_NUM);

		CMP_InitStruct.CMP_ChanCtrl[ADC_CHAN[i]].CMP_Ref0 = 10; // 1V
		CMP_InitStruct.CMP_ChanCtrl[ADC_CHAN[i]].CMP_Ref1 = 20; // 2V
		/* wake up system and ADC when Vin > 1V (Vref0) && Vin < 2V (Vref1)*/
		CMP_InitStruct.CMP_ChanCtrl[ADC_CHAN[i]].CMP_WakeType = COMP_WK_SYS | COMP_WK_ADC;
		CMP_InitStruct.CMP_ChanCtrl[ADC_CHAN[i]].CMP_WakeSysCtrl = COMP_WITHIN_REF0_AND_REF1;
		CMP_InitStruct.CMP_ChanCtrl[ADC_CHAN[i]].CMP_WakeADCCtrl = COMP_WITHIN_REF0_AND_REF1;
	}

	CMP_Init(&CMP_InitStruct);
	CMP_Cmd(ENABLE);
	CMP_TimerTrigCmd(CMP_TIM_IDX, PERIOD_MS, ENABLE);

	/* When input voltage of each channel (0~4) matches the criteria, comparator will send
	wakeup signal to system and trigger ADC to sample */
	InterruptRegister((IRQ_FUN)CMPIrqHandle, ADC_COMP_IRQ, NULL, INT_PRI_MIDDLE);
	InterruptEn(ADC_COMP_IRQ, INT_PRI_MIDDLE);

	while (1);
}

int main(void)
{
	if (rtos_task_create(NULL, "RAW_ADC_COMP_TASK", (rtos_task_t)adc_comp_demo, NULL, (2048), (1)) != SUCCESS) {
		printf("Cannot create RAW_ADC_COMP_TASK \r\n");
	}

	rtos_sched_start();

	return 0;
}
