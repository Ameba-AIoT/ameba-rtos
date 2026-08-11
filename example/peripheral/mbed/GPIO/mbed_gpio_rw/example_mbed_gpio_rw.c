/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "example_gpio_ext.h"
#include "device.h"
#include "gpio_api.h"   // mbed
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>

void mbed_gpio_demo(void)
{
	gpio_t gpio_led;
	gpio_t gpio_btn;
#ifdef GPIO_LOOPBACK
	static u8 output_value = 0;
	int input_value;
#endif

	// Initialize LED control pin
	gpio_init(&gpio_led, GPIO_LED_PIN);
	gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
	gpio_mode(&gpio_led, PullNone);     // No pull

	// Initialize Push Button pin
	gpio_init(&gpio_btn, GPIO_PUSHBT_PIN);
	gpio_dir(&gpio_btn, PIN_INPUT);     // Direction: Input
	gpio_mode(&gpio_btn, PullUp);       // Pull-High

	while (1) {
#ifdef GPIO_LOOPBACK
		output_value = !output_value;
		gpio_write(&gpio_led, output_value);
		input_value = gpio_read(&gpio_btn);
		if (input_value == output_value) {
			printf("PASS: Output=%d, Input=%d\r\n", output_value, input_value);
		} else {
			printf("FAIL: Output=%d, Input=%d (Check wire!)\r\n", output_value, input_value);
		}
#else
		if (gpio_read(&gpio_btn)) {
			// turn off LED
			gpio_write(&gpio_led, 0);
			// printf("Expect 0, Read %d \n", gpio_read(&gpio_led));
		} else {
			// turn on LED
			gpio_write(&gpio_led, 1);
			// printf("Expect 1, Read %d \n", gpio_read(&gpio_led));
		}
#endif
		rtos_time_delay_ms(1000);
	}
}

int example_mbed_gpio_rw(void)
{
	if (RTK_SUCCESS != rtos_task_create(NULL, "MBED_GPIO_TASK", (rtos_task_t)mbed_gpio_demo, (void *)NULL, 128 * 16, (1))) {
		printf("Create MBED_GPIO_TASK Err!!!\n");
	}

	return 0;
}