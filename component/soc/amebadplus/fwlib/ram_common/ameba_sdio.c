/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @addtogroup SDIO
  * @{
  */

/** @defgroup SDIO_Exported_Functions SDIO Exported Functions
  * @{
  */

/**
  * @brief  Fill each SDIO_InitStruct member with its default value.
  * @param  SDIO_InitStruct Pointer to an @ref SDIO_InitTypeDef structure which will be
  *         initialized.
  */
void SDIO_StructInit(SDIO_InitTypeDef *SDIO_InitStruct)
{
	/* TXBD */
	SDIO_InitStruct->TXBD_BAR = (u32)NULL; /* init txbd_bar */
	SDIO_InitStruct->TXBD_RING_SIZE = 20; /* init TXBD ring size */
	SDIO_InitStruct->TX_BUFFER_SIZE = 0xFF; /* init txbuf_size */

	/* RXBD */
	SDIO_InitStruct->RXBD_BAR = (u32)NULL; /* init rxbd_bar reg */
	SDIO_InitStruct->RXBD_RING_SIZE = 32; /* init rxbd ring size */
	SDIO_InitStruct->RXBD_FREE_TH = 5; /* init AHB rx_blk_size */
}

/**
  * @brief  Initialize the SDIO according to the specified parameters in SDIO_InitStruct.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @param  SDIOInit_Struct Pointer to an @ref SDIO_InitTypeDef structure that contains
  *         the configuration information of the SDIO peripheral.
  * @note   Pinmux function and CCCR (if needed) should be configured before this function.
  */
void SDIO_Init(SDIO_TypeDef *SDIO, SDIO_InitTypeDef *SDIOInit_Struct)
{
	UNUSED(SDIO);

	u16 Temp = 0;

	/* Set PID to 0x8722 */
	HAL_WRITE16(SDIO_CCCR_REG_BASE, MANF_PID, 0x8722);

	/* CCCR Autoload done */
	HAL_WRITE8(SDIO_REG_BASE, REG_SPDIO_CPU_IND,
			   HAL_READ8(SDIO_REG_BASE, REG_SPDIO_CPU_IND) | BIT_SYSTEM_READEE_DONE);

	/* SDIO pinmux function enable */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_USB_SDIO_CTRL,
				HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_USB_SDIO_CTRL) | LSYS_BIT_SDD_PMUX_FEN);

	/* Reset SDIO DMA */
	HAL_WRITE8(SDIO_REG_BASE, REG_SPDIO_CPU_RST_DMA, BIT_CPU_RST_SDIO_DMA);

	/* initial TX BD */
	HAL_WRITE32(SDIO_REG_BASE, REG_SPDIO_TXBD_ADDR, SDIOInit_Struct->TXBD_BAR);
	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_TXBD_SIZE, SDIOInit_Struct->TXBD_RING_SIZE);

	/* Set TX_BUFF_UNIT_SIZE */
	HAL_WRITE8(SDIO_REG_BASE, REG_SPDIO_TX_BUF_UNIT_SZ, SDIOInit_Struct->TX_BUFFER_SIZE);

	/* Set DISPATCH_TXAGG_PKT */
	HAL_WRITE32(SDIO_REG_BASE, REG_SPDIO_AHB_DMA_CTRL, HAL_READ32(SDIO_REG_BASE, REG_SPDIO_AHB_DMA_CTRL) | DISPATCH_TXAGG_PKT);

	/*  Reset HW TX BD pointer */
	HAL_WRITE32(SDIO_REG_BASE, REG_SPDIO_TXBD_RPTR, HAL_READ32(SDIO_REG_BASE, REG_SPDIO_TXBD_WPTR));

	/* Init RX BD and RX Buffer */
	HAL_WRITE32(SDIO_REG_BASE, REG_SPDIO_RXBD_ADDR, SDIOInit_Struct->RXBD_BAR);
	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_RXBD_SIZE, SDIOInit_Struct->RXBD_RING_SIZE);

	/*  Reset HW RX BD pointer */
	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_RXBD_C2H_WPTR, HAL_READ16(SDIO_REG_BASE, REG_SPDIO_RXBD_C2H_RPTR));

	/* Set the threshold of free RX BD count to trigger interrupt */
	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_RX_BD_FREE_CNT, SDIOInit_Struct->RXBD_FREE_TH);

	/*  Indicate Host this is a iNIC FW */
	Temp = HAL_READ16(SDIO_REG_BASE, REG_SPDIO_CCPWM2);
	Temp |= CPWM2_INIC_FW_RDY_BIT;
	Temp ^= CPWM2_TOGGLE_BIT;
	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_CCPWM2, Temp);
}

/**
  * @brief  Clear specified SDIO interrupt bit(s).
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @param  SDIO_INT Specified interrupt bit(s) to be cleared.
  */
void SDIO_INTClear(SDIO_TypeDef *SDIO, u16 SDIO_INT)
{
	UNUSED(SDIO);

	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_CPU_INT_STAS, SDIO_INT);	// clean the ISR
}

/**
  * @brief  Clear all the SDIO interrupt bits.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  */
void SDIO_INTClearAll(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	u16 IntStatus;

	IntStatus = HAL_READ16(SDIO_REG_BASE, REG_SPDIO_CPU_INT_STAS);
	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_CPU_INT_STAS, IntStatus);	// clean the ISR
}


/**
  * @brief  Enable or disable specified interrupt source(s).
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @param  IntMask Specifies the interrupt source(s) to configure; refer to SPDIO_CPU_INT_MASK
  *         definitions for the target SDIO instance.
  * @param  NewState Interrupt status, which can be ENABLE or DISABLE.
  */
void SDIO_INTConfig(SDIO_TypeDef *SDIO, u16 IntMask, u32 NewState)
{
	UNUSED(SDIO);

	u16 IntMaskTmp = HAL_READ16(SDIO_REG_BASE, REG_SPDIO_CPU_INT_MASK);

	if (NewState == ENABLE) {
		IntMaskTmp |= IntMask;
	} else {
		IntMaskTmp &= ~IntMask;
	}

	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_CPU_INT_MASK, IntMaskTmp);
}

/**
  * @brief  Get SDIO interrupt status.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @return Interrupt status.
  */
u16 SDIO_INTStatus(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	return HAL_READ16(SDIO_REG_BASE, REG_SPDIO_CPU_INT_STAS);
}

/**
  * @brief  Get RPWM1 value from host.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @return RPWM1 value.
  */
u8 SDIO_RPWM1_Get(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	return HAL_READ8(SDIO_REG_BASE, REG_SPDIO_CRPWM);
}

/**
  * @brief  Get RPWM2 value from host.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @return RPWM2 value.
  */
u16 SDIO_RPWM2_Get(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	return HAL_READ16(SDIO_REG_BASE, REG_SPDIO_CRPWM2);
}

/**
  * @brief  Set CPWM1 to host.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @param  Val CPWM1 message to be sent.
  */
void SDIO_CPWM1_Set(SDIO_TypeDef *SDIO, u8 Val)
{
	UNUSED(SDIO);

	u8  Temp = HAL_READ8(SDIO_REG_BASE, REG_SPDIO_CCPWM);

	Temp ^= CPWM1_TOGGLE_BIT;
	Temp |= Val;

	HAL_WRITE8(SDIO_REG_BASE, REG_SPDIO_CCPWM, Temp);
}

/**
  * @brief  Set CPWM2 to host.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @param  Val CPWM2 message to be sent.
  * @param  Newstate New state of CPWM2, which can be ENABLE or DISABLE.
  */
void SDIO_CPWM2_Set(SDIO_TypeDef *SDIO, u16 Val, u32 Newstate)
{
	UNUSED(SDIO);

	u16  Temp = HAL_READ16(SDIO_REG_BASE, REG_SPDIO_CCPWM2);

	Temp ^= CPWM2_TOGGLE_BIT;

	if (Newstate == ENABLE) {
		Temp |= Val;
	} else {
		Temp &= ~Val;
	}

	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_CCPWM2, Temp);
}

/**
  * @brief  Get H2C message from host.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @return H2C message value.
  */
u32 SDIO_H2C_MSG_Get(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	return HAL_READ32(SDIO_REG_BASE, REG_SPDIO_CPU_H2C_MSG);
}

/**
  * @brief  Get RXBD read pointer, which is updated by SDIO IP.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @return RXBD read pointer.
  */
u16 SDIO_RXBD_RPTR_Get(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	return HAL_READ16(SDIO_REG_BASE, REG_SPDIO_RXBD_C2H_RPTR);
}

/**
  * @brief  Set RXBD write pointer, which is updated by CPU.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @param  Val RXBD write pointer.
  */
void SDIO_RXBD_WPTR_Set(SDIO_TypeDef *SDIO, u16 Val)
{
	UNUSED(SDIO);

	HAL_WRITE16(SDIO_REG_BASE, REG_SPDIO_RXBD_C2H_WPTR, Val);
}

/**
  * @brief  Get TXBD write pointer, which is updated by SDIO IP.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @return TXBD write pointer.
  */
u32 SDIO_TXBD_WPTR_Get(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	return HAL_READ32(SDIO_REG_BASE, REG_SPDIO_TXBD_WPTR);
}

/**
  * @brief  Set TXBD read pointer, which is updated by CPU.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @param  Val TXBD read pointer.
  */
void SDIO_TXBD_RPTR_Set(SDIO_TypeDef *SDIO, u32 Val)
{
	UNUSED(SDIO);

	HAL_WRITE32(SDIO_REG_BASE, REG_SPDIO_TXBD_RPTR, Val);
}

/**
  * @brief  Get TXBD read pointer.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @return TXBD read pointer.
  */
u32 SDIO_TXBD_RPTR_Get(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	return HAL_READ32(SDIO_REG_BASE, REG_SPDIO_TXBD_RPTR);
}

/**
  * @brief  Reset SDIO DMA.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  */
void SDIO_DMA_Reset(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	HAL_WRITE8(SDIO_REG_BASE, REG_SPDIO_CPU_RST_DMA, BIT_CPU_RST_SDIO_DMA);
}

/**
  * @brief  Get AHB DMA control register value.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @return AHB DMA control register value.
  */
u32 SDIO_DMA_CTRL_Get(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	return HAL_READ32(SDIO_REG_BASE, REG_SPDIO_AHB_DMA_CTRL);
}

/**
  * @brief  Set SDIO ready bit to tell host if SDIO is ready.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  * @param  ready If SDIO is ready, which can be ENABLE or DISABLE.
  */
void SDIO_SetReady(SDIO_TypeDef *SDIO, int ready)
{
	UNUSED(SDIO);

	u8 reg;

	reg = HAL_READ8(SDIO_REG_BASE, REG_SPDIO_CPU_IND);

	if (ready == ENABLE) {
		reg |= BIT_SYSTEM_TRX_RDY_IND;
	} else {
		reg &= ~BIT_SYSTEM_TRX_RDY_IND;
	}

	HAL_WRITE8(SDIO_REG_BASE, REG_SPDIO_CPU_IND, reg);
}

/**
  * @brief  Set SDIO RX request.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  */
void SDIO_RxReq(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	HAL_WRITE8(SDIO_REG_BASE, REG_SPDIO_HCI_RX_REQ, BIT_HCI_RX_REQ);
}

/**
  * @brief  Abort SDIO RX request.
  * @param  SDIO SDIO device, refer to IS_SDIO_DEVICE() for valid values.
  */
void SDIO_AbortRxReq(SDIO_TypeDef *SDIO)
{
	UNUSED(SDIO);

	u8 reg;

	reg = HAL_READ8(SDIO_REG_BASE, REG_SPDIO_HCI_RX_REQ);
	reg &= ~BIT_HCI_RX_REQ;
	HAL_WRITE8(SDIO_REG_BASE, REG_SPDIO_HCI_RX_REQ, reg);
}
/**
  * @}
  */

/**
  * @}
  */
/**
  * @}
  */
