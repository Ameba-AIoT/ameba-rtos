/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "example_adc_ext.h"
#include "os_wrapper.h"

#define ADC_TIM_IDX		2
#define PERIOD_MS		500

u32 adc_irq_handle(void *para)
{
	(void)para;

	u32 status = ADC_GetISR();
	u32 cnt, value;
	s32 vol;

	if (status & ADC_BIT_IT_FIFO_FULL_STS) {
		cnt = ADC_GetRxCount();

		while (cnt--) {
			value = ADC_GET_DATA_GLOBAL(ADC_Read());
			/* vbat channel */
			vol = ADC_GetVBATVoltage(value);
			RTK_LOGI(NOTAG, "ADC_Vbat: 0x%x => %dmV\n", value, vol);
		}
	}

	ADC_INTClearPendingBits(status);

	return 0;
}

void raw_adc_vbat_demo(void)
{
	ADC_InitTypeDef ADC_InitStruct;

	/* Enable ADC clock and function */
	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
#if defined (CONFIG_AMEBASMART)
	RCC_PeriphClockCmd(APBPeriph_CTC, APBPeriph_CTC_CLOCK, ENABLE);
#endif
	RCC_PeriphClockCmd(APBPeriph_TIMx[ADC_TIM_IDX], APBPeriph_TIMx_CLOCK[ADC_TIM_IDX], ENABLE);

	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_OpMode = ADC_TIM_TRI_MODE;
	ADC_InitStruct.ADC_CvlistLen = 0;
	ADC_InitStruct.ADC_Cvlist[0] = VBAT_CH;
	ADC_Init(&ADC_InitStruct);
	ADC_INTConfig(ADC_BIT_IT_FIFO_FULL_EN, ENABLE);

	InterruptRegister((IRQ_FUN)adc_irq_handle, ADC_IRQ, NULL, INT_PRI_MIDDLE);
	InterruptEn(ADC_IRQ, INT_PRI_MIDDLE);

	ADC_Cmd(ENABLE);

	ADC_TimerTrigCmd(ADC_TIM_IDX, PERIOD_MS, ENABLE);

	while (1);
}

int example_raw_adc_tim_trig_vbat(void)
{
	if (rtos_task_create(NULL, "RAW_ADC_VBAT_TASK", (rtos_task_t)raw_adc_vbat_demo, NULL, (2048), (1)) != RTK_SUCCESS) {
		RTK_LOGI(NOTAG, "Cannot create RAW_ADC_VBAT_TASK\n");
	}

	//rtos_sched_start();

	return 0;
}
