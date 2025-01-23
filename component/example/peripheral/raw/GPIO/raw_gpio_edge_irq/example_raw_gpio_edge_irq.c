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
#include "os_wrapper.h"
#include <stdio.h>

IRQn_Type GPIO_IrqNum[2] = {GPIOA_IRQ, GPIOB_IRQ};
u32 GPIO_RegBase[2] = {(u32)GPIOA_BASE, (u32)GPIOB_BASE};

void gpio_demo_irq_handler(uint32_t id, u32 event)
{
	UNUSED(event);
	GPIO_InitTypeDef *gpio_id = (GPIO_InitTypeDef *) id;
	u32 pol = gpio_id->GPIO_ITPolarity;

	if (pol == GPIO_INT_POLARITY_ACTIVE_LOW) {
		printf("falling edge event\r\n");

		gpio_id->GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
		GPIO_INTMode(GPIO_IRQ_EDGE_PIN, ENABLE, GPIO_INT_Trigger_EDGE, gpio_id->GPIO_ITPolarity, GPIO_INT_DEBOUNCE_ENABLE);
		PAD_PullCtrl(gpio_id->GPIO_Pin, GPIO_PuPd_DOWN);
	} else  if (pol == GPIO_INT_POLARITY_ACTIVE_HIGH) {
		printf("rising edge event\r\n");

		gpio_id->GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
		GPIO_INTMode(GPIO_IRQ_EDGE_PIN, ENABLE, GPIO_INT_Trigger_EDGE, gpio_id->GPIO_ITPolarity, GPIO_INT_DEBOUNCE_ENABLE);
		PAD_PullCtrl(gpio_id->GPIO_Pin, GPIO_PuPd_UP);
	}
}

void raw_gpio_edge_irq_demo(void)
{
	/* Enable GPIO function and clock */
	// RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
	rtos_time_delay_ms(3000);
	printf("raw_gpio_edge_irq_demo \r\n");

	GPIO_InitTypeDef GPIO_InitStruct_SOURCE;
	GPIO_InitTypeDef GPIO_InitStruct_EDGE_INT;
	u32 port_num;

	GPIO_InitStruct_SOURCE.GPIO_Pin = GPIO_SIGNAL_SOURCE;
	GPIO_InitStruct_SOURCE.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_SOURCE);

	/* Init gpio pin as irq edge int mode */
	GPIO_INTConfig(GPIO_IRQ_EDGE_PIN, DISABLE);
	port_num = PORT_NUM(GPIO_IRQ_EDGE_PIN);

	GPIO_InitStruct_EDGE_INT.GPIO_Pin =  GPIO_IRQ_EDGE_PIN;
	GPIO_InitStruct_EDGE_INT.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct_EDGE_INT.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct_EDGE_INT.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
	GPIO_InitStruct_EDGE_INT.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;

	InterruptRegister((IRQ_FUN)GPIO_INTHandler, GPIO_IrqNum[port_num], (u32)GPIO_RegBase[port_num], 6);
	InterruptEn(GPIO_IrqNum[port_num], 6);

	GPIO_Init(&GPIO_InitStruct_EDGE_INT);
	GPIO_UserRegIrq(GPIO_IRQ_EDGE_PIN, (void *)gpio_demo_irq_handler, &GPIO_InitStruct_EDGE_INT);
	GPIO_INTConfig(GPIO_IRQ_EDGE_PIN, ENABLE);

	while (1) {
		GPIO_WriteBit(GPIO_SIGNAL_SOURCE, 1);
		rtos_time_delay_ms(1000);

		GPIO_WriteBit(GPIO_SIGNAL_SOURCE, 0);
		rtos_time_delay_ms(1000);
	}

}

int example_raw_gpio_edge_irq(void)
{
	if (SUCCESS != rtos_task_create(NULL, "RAW_GPIO_EDGE_IRQ_TASK", (rtos_task_t)raw_gpio_edge_irq_demo, (void *)NULL, (256 * 4), 1)) {
		printf("Create RAW_GPIO_EDGE_IRQ_TASK Err!!\n");
	}

	// rtos_sched_start();
	return 0;
}

