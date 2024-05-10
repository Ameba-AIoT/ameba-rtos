/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "FreeRTOS.h"

extern void vRegisterIRQHandler(uint32_t ulID, ISRCallback_t pxHandler, void *pvContext);

void irq_enable(IRQn_Type irqn)
{
	arm_gic_irq_enable(32 + irqn);
}

void irq_disable(IRQn_Type irqn)
{
	arm_gic_irq_disable(32 + irqn);
}

BOOL irq_register_edge(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data,  u32 Priority)
{
	vRegisterIRQHandler((32 + IrqNum), (ISRCallback_t)IrqFun, (void *)Data);

	arm_gic_irq_set_priority((32 + IrqNum), Priority, IRQ_TYPE_EDGE);

	return _TRUE;
}

BOOL irq_register(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data,  u32 Priority)
{
	vRegisterIRQHandler((32 + IrqNum), (ISRCallback_t)IrqFun, (void *)Data);
	arm_gic_irq_set_priority((32 + IrqNum), Priority, IRQ_TYPE_LEVEL);

	return _TRUE;
}

/**
  * @brief  Unregister IRQ Handler.
  * @param  IrqNum: a value of @ref IRQn_enum.
  */
BOOL irq_unregister(IRQn_Type   IrqNum)
{
	vRegisterIRQHandler((32 + IrqNum), NULL, NULL);

	return _TRUE;
}

void irq_clear_pending(IRQn_Type irqn)
{
	arm_gic_clear_pending_irq(32 + irqn);
}

void irq_set_pending(IRQn_Type irqn)
{
	/*cat not Set-pending bits for SGIs*/
	arm_gic_set_pending_irq(32 + irqn);
}

uint32_t irq_get_pending(IRQn_Type irqn)
{
	return arm_gic_irq_is_pending(32 + irqn);
}

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
