/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2015 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "gpio_ext.h"
#include "device.h"
#include "gpio_irq_api.h"   // mbed
#include "gpio_api.h"
#include "wait_api.h"
#include "os_wrapper.h"
#include <stdio.h>

gpio_irq_t gpio_level;
int current_level = IRQ_LOW;

void gpio_level_irq_handler(uint32_t id, gpio_irq_event event)
{
	UNUSED(event);
	uint32_t *level = (uint32_t *) id;

	// Disable level irq because the irq will keep triggered when it keeps in same level.
	gpio_irq_disable(&gpio_level);

	// make some software de-bounce here if the signal source is not stable.

	if (*level == IRQ_LOW) {
		printf("low level event\r\n");

		// Change to listen to high level event
		*level = IRQ_HIGH;
		gpio_irq_set(&gpio_level, IRQ_HIGH, 1);
		//gpio_irq_pull_ctrl(&gpio_level, GPIO_PuPd_DOWN);
	} else if (*level == IRQ_HIGH) {
		printf("high level event\r\n");

		// Change to listen to low level event
		*level = IRQ_LOW;
		gpio_irq_set(&gpio_level, IRQ_LOW, 1);
		//gpio_irq_pull_ctrl(&gpio_level, GPIO_PuPd_UP);
	}

	//do this internal gpio_irq_enable
	//DelayUs(61);
	gpio_irq_enable(&gpio_level);
}

void mbed_gpio_level_irq_demo(void)
{
	// configure level trigger handler
	gpio_irq_init(&gpio_level, GPIO_IRQ_LEVEL_PIN, gpio_level_irq_handler, (uint32_t)(&current_level));
	gpio_irq_pull_ctrl(&gpio_level, GPIO_PuPd_UP);
	gpio_irq_set(&gpio_level, IRQ_LOW, 1);

	gpio_irq_enable(&gpio_level);

	// configure gpio as signal source for high/low level trigger
	gpio_t gpio_src;
	gpio_init(&gpio_src, GPIO_SIGNAL_SOURCE);
	gpio_dir(&gpio_src, PIN_OUTPUT);    // Direction: Output
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
	if (SUCCESS != rtos_task_create(NULL, "MBED_GPIO_LEVEL_IRQ_TASK", (rtos_task_t)mbed_gpio_level_irq_demo, (void *)NULL, 128 * 4, (1))) {
		printf("Create MBED_GPIO_LEVEL_IRQ_TASK Err!!!\n");
	}

	rtos_sched_start();

	return 0;
}

