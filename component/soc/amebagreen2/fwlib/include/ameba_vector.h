/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_VECTOR_H_
#define _AMEBA_VECTOR_H_

#if defined (CONFIG_ARM_CORE_CM4)
#define MAX_PERIPHERAL_IRQ_NUM			80
#define MAX_VECTOR_TABLE_NUM			(MAX_PERIPHERAL_IRQ_NUM + 16)
#define MAX_NVIC_IPR_NUM				((MAX_PERIPHERAL_IRQ_NUM + 3) >> 2)// KM0 IPR is U32 in NVIC_Type struct
#define MAX_NVIC_ISER_NUM				((MAX_PERIPHERAL_IRQ_NUM + 31) >> 5)
#define MAX_IRQ_PRIORITY_VALUE			7
#endif

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup IRQ
  * @brief IRQ modules
  * @{
  */

/** @addtogroup IRQ
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * IRQ table, please refer to IRQ Exported Constants->IRQn_enum->IRQn
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *		1. register/unregister IRQ use: InterruptRegister/InterruptUnRegister
  *		2. enable/disable IRQ use: InterruptEn/InterruptDis
  *
  *****************************************************************************************
  * @endverbatim
  */

/* Exported types ------------------------------------------------------------*/

/** @defgroup IRQ_Exported_Types IRQ Exported Types
  * @{
  */
typedef void (*HAL_VECTOR_FUN)(void);
typedef u32(*IRQ_FUN)(void *Data);
typedef u32(*Fault_Patch)(uint32_t *MSP, uint32_t lr, uint32_t fault_id);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup IRQ_Exported_Functions IRQ Exported Functions
  * @{
  */
extern _LONG_CALL_ void irq_enable(IRQn_Type   IrqNum);
extern _LONG_CALL_ void irq_disable(IRQn_Type   IrqNum);
extern _LONG_CALL_ void irq_set_priority(IRQn_Type irqn, uint32_t priority);
extern _LONG_CALL_ uint32_t irq_get_priority(IRQn_Type irqn);
extern _LONG_CALL_ void irq_set_pending(IRQn_Type irqn);
extern _LONG_CALL_ uint32_t irq_get_pending(IRQn_Type irqn);
extern _LONG_CALL_ void irq_clear_pending(IRQn_Type irqn);
extern _LONG_CALL_ bool irq_register(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data, u32 Priority);
extern _LONG_CALL_ bool irq_unregister(IRQn_Type IrqNum);
extern _LONG_CALL_ bool FaultPatch_register(Fault_Patch PatchFun);
extern _LONG_CALL_ void SVCall_irqfunc_set(void (*pfunc)(void));
extern _LONG_CALL_ void *SVCall_irqfunc_get(void);
extern _LONG_CALL_ void PendSV_irqfunc_set(void (*pfunc)(void));
extern _LONG_CALL_ void *PendSV_irqfunc_get(void);
extern _LONG_CALL_ void Systick_irqfunc_set(void (*pfunc)(void));
extern _LONG_CALL_ void *Systick_irqfunc_get(void);


#define InterruptRegister			irq_register
#define InterruptUnRegister			irq_unregister

#define InterruptEn(a,b)			irq_enable(a)
#define InterruptDis(a)				irq_disable(a)
/**
  * @}
  */

/**
  * @}
  */

extern IRQ_FUN UserIrqFunTable[];
extern u32 UserIrqDataTable[];
extern HAL_VECTOR_FUN  RomVectorTable[];

#endif //_AMEBA_VECTOR_H_
