/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_VECTOR_H_
#define _AMEBA_VECTOR_H_

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
typedef void (*Fault_Patch)(uint32_t *MSP, uint32_t *PSP, uint32_t lr, uint32_t fault_id);
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup IRQ_Exported_Functions IRQ Exported Functions
  * @{
  */
extern _LONG_CALL_ void irq_table_init(u32 StackP);
extern _LONG_CALL_ void irq_enable(IRQn_Type   IrqNum);
extern _LONG_CALL_ void irq_disable(IRQn_Type   IrqNum);
extern _LONG_CALL_ void irq_set_priority(IRQn_Type irqn, uint32_t priority);
extern _LONG_CALL_ uint32_t irq_get_priority(IRQn_Type irqn);
extern _LONG_CALL_ void irq_set_pending(IRQn_Type irqn);
extern _LONG_CALL_ uint32_t irq_get_pending(IRQn_Type irqn);
extern _LONG_CALL_ void irq_clear_pending(IRQn_Type irqn);
extern _LONG_CALL_ BOOL irq_register(IRQ_FUN IrqFun, IRQn_Type IrqNum, u32 Data, u32 Priority);
extern _LONG_CALL_ BOOL irq_unregister(IRQn_Type IrqNum);
extern _LONG_CALL_ BOOL FaultPatch_register(Fault_Patch PatchFun);

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

/* Other Definitions --------------------------------------------------------*/
extern IRQ_FUN UserIrqFunTable[];
extern u32 UserIrqDataTable[];
extern HAL_VECTOR_FUN  NewVectorTable[];

#if defined (ARM_CORE_CM4)
#define MAX_VECTOR_TABLE_NUM			(80+16)
#define MAX_PERIPHERAL_IRQ_NUM			80
#define MAX_IRQ_PRIORITY_VALUE			7
#else
#define MAX_VECTOR_TABLE_NUM			(16+80)
#define MAX_PERIPHERAL_IRQ_NUM			80
#define MAX_NVIC_IPR_NUM				((MAX_PERIPHERAL_IRQ_NUM + 3) >> 2)// KM0 IPR is U32 in NVIC_Type struct
#define MAX_NVIC_ISER_NUM				((MAX_PERIPHERAL_IRQ_NUM + 31) >> 5)
#define MAX_IRQ_PRIORITY_VALUE			3
#endif

#define BIT_EXCRETURN_S			BIT(6)	/* Indicates whether a Secure or Non-secure stack is used to restore stack frame on exception return. 0: Non-secure, 1: Secure*/
#define BIT_EXCRETURN_MODE		BIT(3)	/* Indicates the Mode that was stacked from. 0: Handler mode, 1: Thread mode */
#define BIT_EXCRETURN_SPSEL		BIT(2)	/* Stack pointer selection. The value of this bit indicates the transitory value of the CONTROL.SPSEL bit
											associated with the Security state of the exception as indicated by EXC_RETURN.ES. 0: MSP, 1: PSP*/
#define BIT_EXCRETURN_ES		BIT(0)	/* The security domain the exception was taken to. 0: Non-secure, 1: Secure */

#define BIT_CONTROL_SPSEL		BIT(1)	/* Stack-pointer select. Defines the stack pointer to be used. 0: use MSP, 1: in thread mode, use PSP */

#endif //_AMEBA_VECTOR_H_
