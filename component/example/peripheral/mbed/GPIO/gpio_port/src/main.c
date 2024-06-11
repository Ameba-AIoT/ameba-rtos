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
#include "port_api.h"   // mbed
#include "PortNames.h"   // mbed
#include "sys_api.h"
#include "os_wrapper.h"
#include <stdio.h>

port_t port0;

extern void wait_ms(u32);

void mbed_gpio_port_demo(void)
{
	//unsigned int pin_mask;
	//pin_mask = 0x2FF00000;    // each bit map to 1 pin: 0: pin disable, 1: pin enable

#if PORT_OUTPUT_TEST
	int i;
	port_init(&port0, GPIO_TEST_PORT, pin_mask, PIN_OUTPUT);
	port_mode(&port0, PullNone);

	while (1) {
		for (i = 0; i < LED_PATTERN_NUM; i++) {
			port_write(&port0, led_pattern[i]);
			printf("[%d]set port: 0x%x\r\n", i, pin_mask & led_pattern[i]);
			wait_ms(1000);
			printf("[%d]get port: 0x%x\r\n", i, port_read(&port0));
		}
	}
#else
	int value_new, value_tmp, value_old;
	int stable;

	port_init(&port0, GPIO_TEST_PORT, pin_mask, PIN_INPUT);

	//port_mode(&port0, PullNone);//for smart

	value_old = port_read(&port0);
	printf("> 0x%x\r\n", value_old);
	while (1) {
		// De-bounce
		value_new = port_read(&port0);
		stable = 0;
		while (stable < 3) {
			value_tmp = port_read(&port0);
			if (value_new != value_tmp) {
				value_new = value_tmp;
				stable = 0;
			} else {
				stable++;
			}
		}

		if (value_old != value_new) {
			printf("< 0x%x\r\n", value_new);
			value_old = value_new;
		}
		wait_ms(50);
	}
#endif
}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "MBED_GPIO_PORT_TASK", (rtos_task_t)mbed_gpio_port_demo, (void *)NULL, (1024 * 4), 1)) {
		printf("Create MBED_GPIO_PORT_TASK Err!!!\n");
	}

	rtos_sched_start();
	return 0;
}
