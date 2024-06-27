/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "adc_ext.h"
#include "analogin_api.h"
#include "os_wrapper.h"
#include <stdio.h>

#define ADC_CH_CNT		2

static u8 ADC_PAD[ADC_CH_CNT] = {
	ADC_PIN1,
	ADC_PIN2
};

void adc_delay(void)
{
	int i;
	for (i = 0; i < 1600000; i++) {
		asm(" nop");
	}
}

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

		printf("AD0:0x%x => %ldmV, AD1:0x%x => %ldmV\n", adcdat0, v_mv0, adcdat1, v_mv1);
		adc_delay();
	}
}

int main(void)
{
	if (rtos_task_create(NULL, "MBED_ADC_VOLTAGE_TASK", (rtos_task_t)adc_analogin_voltage, NULL, 3072, (1)) != SUCCESS) {
		printf("Cannot create MBED_ADC_VOLTAGE_TASK \r\n");
	}

	rtos_sched_start();

	return 0;
}
