/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "analogin_api.h"
#include "device.h"
#include "os_wrapper.h"
#include <stdio.h>

void adc_delay(void)
{
	int i;
	for (i = 0; i < 1600000; i++) {
		asm(" nop");
	}
}

void mbed_adc_vbat_demo(void)
{
	analogin_t adc_vbat;
	uint16_t adc_read = 0;
	int32_t voltage;

	analogin_init(&adc_vbat, VBAT_MEAS);

	for (;;) {
		adc_read = analogin_read_u16(&adc_vbat);
		/* vbat channel */
		voltage = analogin_voltage_vbat(adc_read);

		printf("ADC_Vbat: 0x%x => %ldmV\n", adc_read, voltage);
		adc_delay();
	}
}

int main(void)
{
	if (rtos_task_create(NULL, "MBED_ADC_VBAT_TASK", (rtos_task_t)mbed_adc_vbat_demo, NULL, 3072, (1)) != SUCCESS) {
		printf("Cannot create MBED_ADC_VBAT_TASK\r\n");
	}

	rtos_sched_start();

	return 0;
}
