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

		RTK_LOGI(NOTAG, "ADC_Vbat: 0x%x => %dmV\n", adc_read, voltage);

		rtos_time_delay_ms(500);
	}
}

int example_mbed_adc_vbat_vol(void)
{
	if (rtos_task_create(NULL, "MBED_ADC_VBAT_TASK", (rtos_task_t)mbed_adc_vbat_demo, NULL, 3072, (1)) != SUCCESS) {
		RTK_LOGE(NOTAG, "Cannot create MBED_ADC_VBAT_TASK\n");
	}

	//rtos_sched_start();

	return 0;
}
