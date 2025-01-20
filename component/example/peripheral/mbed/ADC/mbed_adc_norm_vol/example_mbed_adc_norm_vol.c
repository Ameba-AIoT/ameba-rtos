/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "example_adc_ext.h"
#include "analogin_api.h"
#include "os_wrapper.h"

#define ADC_CH_CNT		2

static u8 ADC_PAD[ADC_CH_CNT] = {
	ADC_PIN1,
	ADC_PIN2
};

void adc_analogin_voltage(void)
{
	analogin_t adc0, adc1;
	uint16_t adcdat0, adcdat1;
	int32_t v_mv0, v_mv1;

	analogin_init(&adc0, ADC_PAD[0]);
	analogin_init(&adc1, ADC_PAD[1]);

	for (;;) {
		adcdat0 = analogin_read_u16(&adc0);
		adcdat1 = analogin_read_u16(&adc1);

		v_mv0 = analogin_voltage_norm(adcdat0);
		v_mv1 = analogin_voltage_norm(adcdat1);

		RTK_LOGI(NOTAG, "AD0:0x%x => %dmV, AD1:0x%x => %dmV\n", adcdat0, v_mv0, adcdat1, v_mv1);

		rtos_time_delay_ms(500);
	}
}

int example_mbed_adc_norm_vol(void)
{
	if (rtos_task_create(NULL, "MBED_ADC_VOLTAGE_TASK", (rtos_task_t)adc_analogin_voltage, NULL, 3072, (1)) != SUCCESS) {
		RTK_LOGE(NOTAG, "Cannot create MBED_ADC_VOLTAGE_TASK\n");
	}

	//rtos_sched_start();

	return 0;
}
