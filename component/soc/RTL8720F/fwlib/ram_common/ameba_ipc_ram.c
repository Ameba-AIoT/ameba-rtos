/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/* comment the whole function out because members of IPC_TypeDef are different between dplus and lite */
IPC_IRQ_FUN IPC_IrqHandler[IPC_CHANNEL_NUM];

void *IPC_IrqData[IPC_CHANNEL_NUM];

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup IPC IPC
  * @brief  IPC driver modules
  * @{
  */

/**
  * @brief  Set IPC share mode.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  share_mode
  *		 1: with share mem
  *		 0: without share mem
  */
void IPC_SelectMode(IPC_TypeDef *IPCx, u32 share_mode)
{
	IPCx->IPC_MODE = share_mode;
}

/**
  * @brief  Set IPC threshold for tx fifo.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  threshold Must >=1
  */
void IPC_SetTxThreshold(IPC_TypeDef *IPCx, u32 threshold)
{
	u32 Temp;

	assert_param(IS_IPC_VALID_Threshold(threshold));

	Temp = IPCx->IPC_TX_ST;
	Temp &= ~IPC_MASK_TX0_FIFO_AF_LEVEL;
	Temp |= IPC_TX0_FIFO_AF_LEVEL(threshold);
	IPCx->IPC_TX_ST = Temp;

}

/**
  * @brief  Set IPC Tx timeout.
  * @param  IPCx IPC device pointer, which can be any IPCx_DEV defined in Peripheral Declarations.
  * @param  timeout If the number of data in fifo is below the threshold for a period longer than that timeout, an interrupt will be triggered
  */
void IPC_SetTxtTimeout(IPC_TypeDef *IPCx, u32 timeout)
{
	u32 Temp;

	Temp = IPCx->IPC_TX_ST;
	Temp &= ~IPC_MASK_TX0_FIFO_TIMEOUT_TH;
	Temp |= IPC_TX0_FIFO_TIMEOUT_TH(timeout);
	IPCx->IPC_TX_ST = Temp;
}

/**
  * @brief  Get IPC device.
  * @param  IPC_Dir Specifies core to core direction. This parameter can be a value of @ref IPC_Direction_Mode.
  * @param  Is_Rx 0: TX; 1: RX.
  * @return IPC device
  */
IPC_TypeDef *IPC_GetDev(IPC_Direction_Mode IPC_Dir, u32 Is_Rx)
{
	IPC_TypeDef *IPCx_DEV;

	if (Is_Rx) {
		if (IPC_Dir == IPC_AP_TO_NP) {
			IPCx_DEV = IPCNP_DEV;
		} else {
			IPCx_DEV = IPCAP_DEV;
		}
	} else {
		if (IPC_Dir == IPC_AP_TO_NP) {
			IPCx_DEV = IPCAP_DEV;
		} else {
			IPCx_DEV = IPCNP_DEV;
		}
	}

	return IPCx_DEV;
}

/**
  * @brief  Get IPC device.
  * @param  cpu_id CPU ID. This parameter can be one of the following values:
  *            @arg 0: KM4TZ_CPU_ID
  *            @arg 1: KM4NS_CPU_ID
  * @return IPC device
  */
IPC_TypeDef *IPC_GetDevById(u32 cpu_id)
{
	if (cpu_id == KM4TZ_CPU_ID) {
		return IPCAP_DEV;
	} else {
		return IPCNP_DEV;
	}
}

/* Exported functions --------------------------------------------------------*/
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
  * @param  IPC_ChNum 0 ~ 15.
  * @return The interrupt request result:
  *         - 1: the interrupt request is sent
  *         - 0: the channel is busy
  */
u32 IPC_INTRequest(IPC_TypeDef *IPCx, u8 IPC_ChNum)
{
	/* Check the parameters */
	assert_param(IS_IPC_VALID_CHNUM(IPC_ChNum));

	if (IPCx->IPC_TX_CH_ST & (BIT(IPC_ChNum))) {
		return 0;
	} else {
		IPCx->IPC_TX_CH_ST |= BIT(IPC_ChNum);
		return 1;
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
