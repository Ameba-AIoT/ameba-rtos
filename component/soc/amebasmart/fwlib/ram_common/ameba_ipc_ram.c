/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

IPC_IRQ_FUN IPC_IrqHandler[IPC_CHANNEL_NUM];
void *IPC_IrqData[IPC_CHANNEL_NUM];

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup IPC IPC
* @brief IPC driver modules
* @{
*/

/**
  * @brief  Get IPC device.
  * @param  cpu_id CPU ID. This parameter can be one of the following values:
  *            @arg 0: CPU_LP
  *            @arg 1: CPU_NP
  *            @arg 2: CPU_AP
  * @return IPC device
  */
IPC_TypeDef *IPC_GetDevById(u32 cpu_id)
{
	/* Check the parameters */
	assert_param(IS_IPC_Valid_CPUID(cpu_id));

	if (cpu_id == 1) {
		return IPCNP_DEV;
	} else if (cpu_id == 2) {
		return IPCAP_DEV;
	} else {
		return IPCLP_DEV;

	}
}

/** @defgroup IPC_Exported_Functions IPC Exported Functions
  * @{
  */
/**
  * @brief  Enables or disables the specified IPC Channel interrupts.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Shiftbit 0 ~ 31.
  * @param  NewState DISABLE/ENABLE.
  */
void IPC_INTConfig(IPC_TypeDef *IPCx, u8 IPC_Shiftbit, u32 NewState)
{
	if (ENABLE == NewState) {
		IPCx->IPC_IMR |= (BIT(IPC_Shiftbit));
	} else {
		IPCx->IPC_IMR &= ~(BIT(IPC_Shiftbit));
	}
}

/**
  * @brief  Set IMR of specified IPC Channel interrupts.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Chs The Channels that want to be enable.
  */
void IPC_IERSet(IPC_TypeDef *IPCx, u32 IPC_Chs)
{
	IPCx->IPC_IMR = IPC_Chs;
}

/**
  * @brief  Get IMR of specified IPC Channel interrupts.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @return The IMR status of specified IPC
  */
u32 IPC_IERGet(IPC_TypeDef *IPCx)
{
	return IPCx->IPC_IMR;
}

/**
  * @brief  Request a core-to-core interrupt.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Dir Specifies core to core direction. This parameter can be a value of @ref IPC_Direction_Mode.
  * @param  IPC_ChNum 0 ~ 7.
  * @return The interrupt request result:
  *         - 1: the interrupt request is sent
  *         - 0: the channel is busy
  */
u32 IPC_INTRequest(IPC_TypeDef *IPCx, IPC_Direction_Mode IPC_Dir, u8 IPC_ChNum)
{
	/* Check the parameters */
	u32 ipc_shift = 0;

	switch (IPC_Dir) {
	case IPC_LP_TO_NP:
	case IPC_NP_TO_LP:
	case IPC_AP_TO_LP:
		ipc_shift = 8;
		break;

	default:
		break;
	}

	if (IPCx != NULL) {
		if (IPCx->IPC_TX_DATA & (BIT(IPC_ChNum + ipc_shift))) {
			return 0;
		} else {
			IPCx->IPC_TX_DATA = (BIT(IPC_ChNum + ipc_shift));
			return 1;
		}
	} else {
		return 0;
	}
}

/**
  * @brief  Get core-to-core interrupt status.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @return Tx_empty or rx_full interrupt status
  */
u32 IPC_INTGet(IPC_TypeDef *IPCx)
{
	return IPCx->IPC_ISR;
}

/**
  * @brief  Clear a core-to-core interrupt.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Shiftbit 0 ~ 31.
  */
void IPC_INTClear(IPC_TypeDef *IPCx, u8 IPC_Shiftbit)
{
	IPCx->IPC_ISR = BIT(IPC_Shiftbit);
}

/**
  * @brief  The common IPC interrupt handler
  * @param  Data The data pointer to IPCx
  * @return 0
  */
u32 IPC_INTHandler(void *Data)
{
	IPC_TypeDef *IPCx = (IPC_TypeDef *)Data;
	u32 IrqPending;
	u32 i;
	IrqPending = IPCx->IPC_ISR & IPCx->IPC_IMR;

	for (i = 0; i < 32; i++) {
		if (IrqPending & BIT(i)) {
			if (IPC_IrqHandler[i] != NULL) {
				IPC_IrqHandler[i](IPC_IrqData[i], IrqPending, i);
				IPCx->IPC_ISR = BIT(i);
			} else {
				IPCx->IPC_ISR = BIT(i);
			}
		}
	}

	return 0;
}

/**
  * @brief  To register a user interrupt handler for a specified IPC channel
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  IPC_Shiftbit 0 ~ 31.
  * @param  IrqHandler The IRQ handler to be assigned to the specified IPC channel
  * @param  IrqData The pointer will be passed to the IRQ handler
  */
void IPC_INTUserHandler(IPC_TypeDef *IPCx, u8 IPC_Shiftbit, void *IrqHandler, void *IrqData)
{
	/* Check the parameters */
	if (IrqHandler == NULL) {
		return;
	}

	IPC_IrqHandler[IPC_Shiftbit] = (IPC_IRQ_FUN)IrqHandler;
	IPC_IrqData[IPC_Shiftbit] = IrqData;
	if (IS_IPC_RX_CHNUM(IPC_Shiftbit)) {
		IPC_INTConfig(IPCx, IPC_Shiftbit, ENABLE);
	}
}

/**@}*/
/**@}*/
/**@}*/