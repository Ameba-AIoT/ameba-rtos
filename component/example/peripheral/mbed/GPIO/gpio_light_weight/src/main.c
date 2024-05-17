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
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>

extern GPIO_TypeDef *GPIO_PORTx[2];

/*  You can improve time cost of gpio write by import source code of
 *  function "gpio_direct_write" based on your needs.
 *  In this example, enable CACHE_WRITE_ACTION as demonstration.
 */
#define CACHE_WRITE_ACTION (0)

void mbed_gpio_light_demo(void)
{
	gpio_t gpio_led;
	gpio_t gpio_btn;

	// Initialize LED control pin
	gpio_init(&gpio_led, GPIO_LED_PIN);
	gpio_dir(&gpio_led, PIN_OUTPUT);    // Direction: Output
	gpio_mode(&gpio_led, PullNone);     // No pull

	// Initialize Push Button pin
	gpio_init(&gpio_btn, GPIO_PUSHBT_PIN);
	gpio_dir(&gpio_btn, PIN_INPUT);     // Direction: Input
	gpio_mode(&gpio_btn, PullUp);       // Pull-High

#if defined(CACHE_WRITE_ACTION) && (CACHE_WRITE_ACTION == 1)
	u8 port_num = PORT_NUM(gpio_led.pin);
	u8 pin_num  = PIN_NUM(gpio_led.pin);
	u32 RegValue;
	GPIO_TypeDef *GPIO = GPIO_PORTx[port_num];
#endif

	while (1) {
#if defined(CACHE_WRITE_ACTION) && (CACHE_WRITE_ACTION == 1)
		if (gpio_read(&gpio_btn)) {
			// turn off LED
			RegValue =  GPIO->PORT[0].GPIO_DR;
			RegValue &= ~(1 << pin_num);
			GPIO->PORT[0].GPIO_DR = RegValue;
		} else {
			// turn on LED
			RegValue =  GPIO->PORT[0].GPIO_DR;
			RegValue |= (1 << pin_num);
			GPIO->PORT[0].GPIO_DR = RegValue;
		}
		//printf("Expect 0x%x, Read: 0x%x \n", RegValue, GPIO->GPIO_EXT_PORT[0]);
#else
		if (gpio_read(&gpio_btn)) {
			// turn off LED
			gpio_direct_write(&gpio_led, 0);
			//printf("Expect 0, Read: %d \n", GPIO_ReadDataBit(GPIO_LED_PIN));
		} else {
			// turn on LED
			gpio_direct_write(&gpio_led, 1);
			//printf("Expect 1, Read: %d \n", GPIO_ReadDataBit(GPIO_LED_PIN));
		}
#endif
		//rtos_time_delay_ms(1000);
	}
}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "MBED_GPIO_LIGHT_TASK", (rtos_task_t)mbed_gpio_light_demo, (void *)NULL, 128 * 4, (1))) {
		printf("Create MBED_GPIO_LIGHT_TASK Err!!!\n");
	}

	rtos_sched_start();
	return 0;
}

