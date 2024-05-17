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
#include "gpio_irq_api.h"   // mbed
#include "wait_api.h"
#include "os_wrapper.h"
#include <stdio.h>

gpio_irq_t gpio_edge;
int current_edge = IRQ_FALL;

void gpio_edge_irq_handler(uint32_t id, gpio_irq_event event)
{
	(void)event;
	gpio_irq_event *edge = (gpio_irq_event *) id;

	if (*edge == IRQ_FALL) {
		printf("falling edge event\r\n");

		*edge = IRQ_RISE;
		gpio_irq_set(&gpio_edge, *edge, 1);
	} else if (*edge == IRQ_RISE) {
		printf("rising edge event\r\n");

		*edge = IRQ_FALL;
		gpio_irq_set(&gpio_edge, *edge, 1);
	}
}

void mbed_gpio_edge_irq_demo(void)
{
	gpio_irq_init(&gpio_edge, GPIO_IRQ_EDGE_PIN, gpio_edge_irq_handler, (uint32_t)(&current_edge));
	gpio_irq_set(&gpio_edge, IRQ_FALL, 1);
	gpio_irq_enable(&gpio_edge);

	gpio_t gpio_src;
	gpio_init(&gpio_src, GPIO_SIGNAL_SOURCE);
	gpio_dir(&gpio_src, PIN_OUTPUT);
	gpio_mode(&gpio_src, PullNone);

	while (1) {
		gpio_write(&gpio_src, 1);
		wait_ms(1000);

		gpio_write(&gpio_src, 0);
		wait_ms(1000);
	}
}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "MBED_GPIO_EDGE_IRQ_TASK", (rtos_task_t)mbed_gpio_edge_irq_demo, (void *)NULL, 128 * 4, (1))) {
		printf("Create MBED_GPIO_EDGE_IRQ_TASK Err!!!\n");
	}

	rtos_sched_start();
	return 0;
}

