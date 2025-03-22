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
#include "os_wrapper.h"
#include <stdio.h>

void raw_gpio_demo(void)
{
	GPIO_InitTypeDef GPIO_InitStruct_LED;
	GPIO_InitTypeDef GPIO_InitStruct_PUSHBT;

	printf("raw_gpio_rw_demo \r\n");

	/* Enable GPIO function and clock */
	// RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

	// Init LED control pin
	GPIO_InitStruct_LED.GPIO_Pin = GPIO_LED_PIN;
	GPIO_InitStruct_LED.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_LED);

	// Init Push Button pin
	GPIO_InitStruct_PUSHBT.GPIO_Pin = GPIO_PUSHBT_PIN;
	GPIO_InitStruct_PUSHBT.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct_PUSHBT.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(&GPIO_InitStruct_PUSHBT);

	while (1) {
		if (GPIO_ReadDataBit(GPIO_PUSHBT_PIN)) {
			GPIO_WriteBit(GPIO_LED_PIN, 0); //turn off LED
			// printf("0: read 0x%x, %lu \n", GPIO_LED_PIN, GPIO_ReadDataBit(GPIO_LED_PIN));
		} else {
			GPIO_WriteBit(GPIO_LED_PIN, 1); // turn on LED
			// printf("1: read 0x%x, %lu \n", GPIO_LED_PIN, GPIO_ReadDataBit(GPIO_LED_PIN));
		}
		rtos_time_delay_ms(1000);
	}
}

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int example_raw_gpio_rw(void)
{
	if (RTK_SUCCESS != rtos_task_create(NULL, "RAW_GPIO_DEMO_TASK", (rtos_task_t)raw_gpio_demo, (void *)NULL, (128 * 16),
										(1))) {
		printf("Create RAW_GPIO_DEMO_TASK Err!!!\n");
	}

	// rtos_sched_start();
	return 0;
}

