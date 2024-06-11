/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "gpio_ext.h"
#include "device.h"
#include "gpio_api.h"   // mbed
#include "sys_api.h"    // for sys_jtag_off()
#include "os_wrapper.h"
#include <stdio.h>

void mbed_gpio_jtag_demo(void)
{
	/* wait total cpus bring up */
	rtos_time_delay_ms(2000);

	gpio_t gpio_jtag_enable;
	gpio_t gpio_led;

	gpio_init(&gpio_jtag_enable, GPIO_JTAG_ENABLE_PIN);
	gpio_dir(&gpio_jtag_enable, PIN_INPUT);
	gpio_mode(&gpio_jtag_enable, PullUp);

	/* wait GPIO_JTAG_ENABLE_PIN's status be stable */
	DelayMs(10);

	if (gpio_read(&gpio_jtag_enable) == 0) {
		// JTAG enable pin is disabled
		sys_jtag_off();
		printf("jtag off\r\n");

		// Now you can use jtag pin for other gpio usage
		// ex. use PE_0 to blink led
		gpio_init(&gpio_led, GPIO_LED_PIN);
		gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
		gpio_mode(&gpio_led, PullNone);     // No pull

		while (1) {
			gpio_write(&gpio_led, 1);
			DelayMs(1000);
			//printf("Expect 1, Read %d \n", gpio_read(&gpio_led));

			gpio_write(&gpio_led, 0);
			DelayMs(1000);
			//printf("Expect 0, Read %d \n", gpio_read(&gpio_led));
			//rtos_time_delay_ms(1000);
		}
	} else {
		// JTAG enable pin is enabled
		printf("jtag on\r\n");
	}

	for (;;);

}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "MBED_GPIO_JTAG_TASK", (rtos_task_t)mbed_gpio_jtag_demo, (void *)NULL, (1024 * 4), 1)) {
		printf("Create MBED_GPIO_JTAG_TASK Err!!!\n");
	}

	rtos_sched_start();
	return 0;
}

