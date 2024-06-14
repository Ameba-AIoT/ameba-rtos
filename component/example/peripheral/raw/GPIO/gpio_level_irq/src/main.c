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
#include "os_wrapper.h"
#include <stdio.h>

IRQn_Type GPIO_IrqNum[2] = {GPIOA_IRQ, GPIOB_IRQ};

u32 GPIO_RegBase[2] = {(u32)GPIOA_BASE, (u32)GPIOB_BASE};

void gpio_level_irq_handler(uint32_t id, u32 event)
{
	UNUSED(event);
	GPIO_InitTypeDef *gpio_id = (GPIO_InitTypeDef *) id;
	u32 level = gpio_id->GPIO_ITPolarity;

	// Disable level irq because the irq will keep triggered when it keeps in same level.
	GPIO_INTConfig(GPIO_IRQ_LEVEL_PIN, DISABLE);

	// make some software de-bounce here if the signal source is not stable.
	if (level == GPIO_INT_POLARITY_ACTIVE_LOW) {
		printf("low level event\r\n");

		// Change to listen to high level event
		gpio_id->GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_HIGH;
		GPIO_INTMode(GPIO_IRQ_LEVEL_PIN, ENABLE, GPIO_INT_Trigger_LEVEL, gpio_id->GPIO_ITPolarity, GPIO_INT_DEBOUNCE_ENABLE);
		PAD_PullCtrl(gpio_id->GPIO_Pin, GPIO_PuPd_DOWN);
	} else  if (level == GPIO_INT_POLARITY_ACTIVE_HIGH) {
		printf("high level event\r\n");

		// Change to listen to low level event
		gpio_id->GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
		GPIO_INTMode(GPIO_IRQ_LEVEL_PIN, ENABLE, GPIO_INT_Trigger_LEVEL, gpio_id->GPIO_ITPolarity, GPIO_INT_DEBOUNCE_ENABLE);
		PAD_PullCtrl(gpio_id->GPIO_Pin, GPIO_PuPd_UP);
	}

	//Delay 2 debounce clock for clearing level int status when set gpio_polarity
	DelayUs(61);

	GPIO_INTConfig(GPIO_IRQ_LEVEL_PIN, ENABLE);
}

void raw_gpio_level_irq_demo(void)
{
	GPIO_InitTypeDef GPIO_InitStruct_Source;
	GPIO_InitTypeDef GPIO_InitStruct_IRQ;
	u8 port_num;

	/* Enable GPIO function and clock */
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);

	/* init gpio pin as irq level int mode */
	GPIO_INTConfig(GPIO_IRQ_LEVEL_PIN, DISABLE);
	GPIO_InitStruct_IRQ.GPIO_Pin = GPIO_IRQ_LEVEL_PIN;
	GPIO_InitStruct_IRQ.GPIO_Mode = GPIO_Mode_INT;
	GPIO_InitStruct_IRQ.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct_IRQ.GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
	GPIO_InitStruct_IRQ.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
	GPIO_InitStruct_IRQ.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_ENABLE;
	GPIO_Init(&GPIO_InitStruct_IRQ);

	port_num = PORT_NUM(GPIO_IRQ_LEVEL_PIN);
	InterruptRegister((IRQ_FUN)GPIO_INTHandler, GPIO_IrqNum[port_num], (u32)GPIO_RegBase[port_num], 6);
	InterruptEn(GPIO_IrqNum[port_num], 6);

	/* When enable debounce, Delay is required to avoid trigger IRQ unexpectly */
	DelayUs(61);

	GPIO_UserRegIrq(GPIO_IRQ_LEVEL_PIN, (void *)gpio_level_irq_handler, (&GPIO_InitStruct_IRQ));
	GPIO_INTConfig(GPIO_IRQ_LEVEL_PIN, ENABLE);

	/* init gpio source pin */
	GPIO_InitStruct_Source.GPIO_Pin = GPIO_SIGNAL_SOURCE;
	GPIO_InitStruct_Source.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_Source);

	while (1) {
		GPIO_WriteBit(GPIO_SIGNAL_SOURCE, 1);
		DelayMs(1000);

		GPIO_WriteBit(GPIO_SIGNAL_SOURCE, 0);
		DelayMs(1000);
	}
}

int main(void)
{
	if (SUCCESS != rtos_task_create(NULL, "RAW_GPIO_LEVEL_IRQ_TASK", (rtos_task_t)raw_gpio_level_irq_demo, (void *)NULL, 128 * 4,
									(1))) {
		printf("Create RAW_GPIO_LEVEL_IRQ_TASK Err!!!\n");
	}

	rtos_sched_start();
	return 0;
}
