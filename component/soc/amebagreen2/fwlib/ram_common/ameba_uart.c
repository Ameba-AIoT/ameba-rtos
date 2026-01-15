/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

const UART_DevTable UART_DEV_TABLE[MAX_UART_INDEX] = {
	{UART0_DEV, GDMA_HANDSHAKE_INTERFACE_UART0_TX, GDMA_HANDSHAKE_INTERFACE_UART0_RX, UART0_IRQ}, /*uart0 */
	{UART1_DEV, GDMA_HANDSHAKE_INTERFACE_UART1_TX, GDMA_HANDSHAKE_INTERFACE_UART1_RX, UART1_IRQ}, /*uart1 */
	{UART2_DEV, GDMA_HANDSHAKE_INTERFACE_UART2_TX, GDMA_HANDSHAKE_INTERFACE_UART2_RX, UART2_IRQ}, /*uart2 */
	{UART3_DEV, GDMA_HANDSHAKE_INTERFACE_UART3_TX, GDMA_HANDSHAKE_INTERFACE_UART3_RX, UART3_IRQ} /*uart3_bt */
};

const u32 APBPeriph_UARTx[MAX_UART_INDEX] = {
	APBPeriph_UART0,
	APBPeriph_UART1,
	APBPeriph_UART2,
	APBPeriph_UART3,
};

const u32 APBPeriph_UARTx_CLOCK[MAX_UART_INDEX] = {
	APBPeriph_UART0_CLOCK,
	APBPeriph_UART1_CLOCK,
	APBPeriph_UART2_CLOCK,
	APBPeriph_UART3_CLOCK,
};

u32 UART_StateTx[MAX_UART_INDEX];
u32 UART_StateRx[MAX_UART_INDEX];

/** @addtogroup Ameba_Periph_Driver
 * @{
 */

/** @defgroup UART
 * @brief UART driver modules
 * @{
 */

/* Exported functions --------------------------------------------------------*/
/** @defgroup UART_Exported_Functions UART Exported Functions
 * @{
 */

/**
 * @brief Configure UART TX DMA burst size.
 * @param UARTx UART device, where x can be 0~3.
 * @param TxDmaBurstSize UART TX DMA burst size.
 * @note UART tx FIFO depth is 16 in hardware, so TxDmaBurstSize should be no more than 16.
 * @note TxDmaBurstSize should not be smaller than GDMA_DstMsize in function UART_TXGDMA_Init();
 * 		For better performance, it is suggested to make TxDmaBurstSize be equal to GDMA_DstMsize, which is 4.
 * @return None
 */
void UART_TXDMAConfig(UART_TypeDef *UARTx, u32 TxDmaBurstSize)
{
	u32 TempMiscr;

	/* Set UART TX DMA burst size.*/
	TempMiscr = UARTx->MISCR;
	TempMiscr &= ~RUART_MASK_TXDMA_BURSTSIZE;
	TempMiscr |= RUART_TXDMA_BURSTSIZE(TxDmaBurstSize);
	UARTx->MISCR = TempMiscr;
}

/**
 * @brief Configure UART RX DMA burst size.
 * @param UARTx UART device, where x can be 0~3.
 * @param RxDmaBurstSize UART RX DMA burst size.
 * @note Although UART rx FIFO depth is 64 in hardware, RxDmaBurstSize should be no more than 32 due to DMA FIFO depth limit.
 * @note RxDmaBurstSize should not be smaller than GDMA_SrcMsize in function UART_RXGDMA_Init();
 * 		For better performance, it is suggested to make RxDmaBurstSize be equal to GDMA_SrcMsize, which is 16.
 * @return None
 */
void UART_RXDMAConfig(UART_TypeDef *UARTx, u32 RxDmaBurstSize)
{
	u32 TempMiscr;

	/* Set UART Rx DMA burst size */
	TempMiscr = UARTx->MISCR;
	TempMiscr &= ~RUART_MASK_RXDMA_BURSTSIZE;
	TempMiscr |= RUART_RXDMA_BURSTSIZE(RxDmaBurstSize);
	UARTx->MISCR = TempMiscr;
}

/**
 * @brief Enable or disable UART TX DMA.
 * @param UARTx UART device, where x can be 0~3.
 * @param NewState New state of UART TX DMA.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void UART_TXDMACmd(UART_TypeDef *UARTx, u32 NewState)
{
	if (NewState != DISABLE) {
		/* enable the UART TX DMA */
		UARTx->MISCR |= RUART_BIT_TXDMA_EN;
	} else {
		/* disable the UART TX DMA */
		UARTx->MISCR &= ~RUART_BIT_TXDMA_EN;
	}
}

/**
 * @brief Enable or disable UART RX DMA.
 * @param UARTx UART device, where x can be 0~3.
 * @param NewState New state of UART RX DMA.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void UART_RXDMACmd(UART_TypeDef *UARTx, u32 NewState)
{
	if (NewState != DISABLE) {
		/* enable the UART RX DMA */
		UARTx->MISCR |= RUART_BIT_RXDMA_EN;
	} else {
		/* disable the UART RX DMA */
		UARTx->MISCR &= ~RUART_BIT_RXDMA_EN;
	}
}

/**
 * @brief Initialize and enable UART TX GDMA.
 * @param UartIndex UART index, which can be 0~3.
 * @param GDMA_InitStruct Pointer to a GDMA_InitTypeDef structure that contains configuration information of the GDMA peripheral.
 * @param CallbackData GDMA callback data.
 * @param CallbackFunc GDMA callback function.
 * @param pTxBuf Pointer to tx buffer.
 * @param TxCount Tx count.
 * @retval TRUE or FALSE.
 */
bool UART_TXGDMA_Init(
	u8 UartIndex,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTxBuf,
	u32 TxCount
)
{
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32)pTxBuf, TxCount);

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		/*  No Available DMA channel */
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->MaxMuliBlock  = 1;

	GDMA_InitStruct->GDMA_DIR     = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = UART_DEV_TABLE[UartIndex].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&UART_DEV_TABLE[UartIndex].UARTx->RBR_OR_UART_THR;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);

	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	if (((TxCount & 0x03) == 0) && (((u32)(pTxBuf) & 0x03) == 0)) {
		/* 4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeOne;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = TxCount >> 2;
	} else {
		/* move 1 byte each transfer */
		GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
		GDMA_InitStruct->GDMA_BlockSize = TxCount;
	}

	GDMA_InitStruct->GDMA_SrcAddr = (u32)(pTxBuf);

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
 * @brief Initialize and enable UART RX GDMA.
 * @param UartIndex UART index, which can be 0~3.
 * @param GDMA_InitStruct Pointer to a GDMA_InitTypeDef structure that contains configuration information of the GDMA peripheral.
 * @param CallbackData GDMA callback data.
 * @param CallbackFunc GDMA callback function.
 * @param pRxBuf Pointer to rx buffer.
 * @param RxCount Rx count, 0 means using UART as DMA flow controller.
 * @retval TRUE or FALSE.
 */
bool UART_RXGDMA_Init(
	u8 UartIndex,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxBuf,
	u32 RxCount
)
{
	u8 GdmaChnl;
	UART_TypeDef *UARTx;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32)pRxBuf, RxCount);

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		/* No Available DMA channel */
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	UARTx = UART_DEV_TABLE[UartIndex].UARTx;
	if (RxCount == 0) {
		/*if length is 0, configure uart as the flow controller*/
		GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem_PerCtrl;
		UARTx->MISCR |= RUART_BIT_RXDMA_OWNER;
	} else {
		/*if length isn`t 0, configure GDMA as the flow controller*/
		GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem;
		UARTx->MISCR &= (~RUART_BIT_RXDMA_OWNER);
	}

	GDMA_InitStruct->GDMA_SrcHandshakeInterface = UART_DEV_TABLE[UartIndex].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&UART_DEV_TABLE[UartIndex].UARTx->RBR_OR_UART_THR;
	GDMA_InitStruct->GDMA_DstAddr = (u32)(pRxBuf);
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_SrcMsize = MsizeOne;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstMsize = MsizeOne;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_BlockSize = RxCount;

	/* multi block close */
	GDMA_InitStruct->MuliBlockCunt  = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock   = 1;

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
  * @brief Configure uart monitor parameters.
  * @param BitNumThres Configure bit number threshold of one monitor period.
  * @param OscPerbitUpdCtrl OSC cycnum_perbit update bit, which can be ENABLE or DISABLE.
  * @return None
  */
void UART_MonitorParaConfig(UART_TypeDef *UARTx, u32 BitNumThres, u32 OscPerbitUpdCtrl)
{
	u32 RegValue = 0;

	/*check the parameters*/
	assert_param((BitNumThres > 0) && (BitNumThres < 128));

	/* Step 1: Reset Low Power Rx Path */
	UARTx->RX_PATH_CTRL &= ~RUART_BIT_R_RST_NEWRX_N;

	RegValue = UARTx->MON_BAUD_CTRL;

	/*step 2: Configure bit number threshold of one monitor period.*/
	RegValue &= ~RUART_MASK_R_BIT_NUM_THRES;
	RegValue |= RUART_R_BIT_NUM_THRES(BitNumThres);

	/*step 3: Configure the OSC cycnum_perbit update bit (REG_MON_BAUD_CTRL[29])*/
	if (OscPerbitUpdCtrl != DISABLE) {
		/*enable OSC cycnum_perbit update*/
		RegValue |= RUART_BIT_R_UPD_OSC_IN_XTAL;
	} else {
		/*disable OSC cycnum_perbit update*/
		RegValue &= ~RUART_BIT_R_UPD_OSC_IN_XTAL;
	}
	UARTx->MON_BAUD_CTRL = RegValue;
}

/**
 * @brief Set uart baud rate of low power rx path.
 * @param UARTx UART device, where x can be 0~3.
 * @param BaudRate Desired baud rate.
 * @param RxIPClockHz UART rx clock in Hz.
 * @note According to the baud rate calculation formula in low power rx path,
 * 		CycPerbit = round(fpclock/BaudRate)
 * @return None
 */
void UART_LPRxBaudSet(UART_TypeDef *UARTx, u32 BaudRate, u32 RxIPClockHz)
{
	u32 CycPerbit = 0;
	u32 RegValue = 0;
	u32 RegOscBaud = 0;

	/*check the parameters*/
	assert_param((BaudRate > 0 && BaudRate <= 8000000));

	/*Calculate the r_cycnum_perbit field of REG_MON_BAUD_STS,
	   according to clock and the desired baud rate*/
	if ((RxIPClockHz % BaudRate) >= (BaudRate + 1) / 2) {
		CycPerbit = RxIPClockHz / BaudRate + 1;
	} else {
		CycPerbit = RxIPClockHz / BaudRate;
	}

	/* Average clock cycle number of one bit. MON_BAUD_STS[19:0] */
	RegValue = UARTx->MON_BAUD_STS;
	RegValue &= ~RUART_MASK_R_CYCNUM_PERBIT_XTAL;
	RegValue |= RUART_R_CYCNUM_PERBIT_XTAL(CycPerbit);
	/* set CycPerbit */
	UARTx->MON_BAUD_STS = RegValue;

	/* Average clock cycle number of one bit OSC. REG_MON_BAUD_CTRL[28:9] */
	RegOscBaud = UARTx->MON_BAUD_CTRL;
	RegOscBaud &= ~RUART_MASK_R_CYCNUM_PERBIT_OSC;
	RegOscBaud |= RUART_R_CYCNUM_PERBIT_OSC(CycPerbit);
	/*set the OSC CycPerbit*/
	UARTx->MON_BAUD_CTRL = RegOscBaud;

	UARTx->RX_PATH_CTRL &= ~RUART_MASK_RXBAUD_ADJ_10_0;
}

/**
 * @brief Enable or disable the monitor function in Low Power Rx Path.
 * @param UARTx UART device, where x can be 0~3.
 * @param NewState New state of monitoring.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void UART_RxMonitorCmd(UART_TypeDef *UARTx, u32 NewState)
{
	/* configure Low Power rx monitoring function*/
	if (NewState != DISABLE) {
		/* Function enable of monitoring rx baud */
		UARTx->MON_BAUD_CTRL |= RUART_BIT_R_MON_BAUD_EN;
	} else {
		/* Function disable of monitoring rx baud */
		UARTx->MON_BAUD_CTRL &= ~RUART_BIT_R_MON_BAUD_EN;
	}
}

/**
 * @brief Get the Monitor Baud Rate control register value of the Low Power Rx Path.
 * @param UARTx UART device, where x can be 0~3.
 * @note MON_BAUD_CTRL[28:9] field value is the monitor result under OSC2M Clock.
 * @return The value of the MON_BAUD_CTRL register.
 */
u32 UART_RxMonBaudCtrlRegGet(UART_TypeDef *UARTx)
{
	/*return the monitor baud rate control register value(MON_BAUD_CTRL)*/
	return UARTx->MON_BAUD_CTRL;
}

/**
 * @brief Get the status of the Low Power Rx Monitoring.
 * @param UARTx UART device, where x can be 0~3.
 * @note Read this register can clear the monitor interrupt status.
 * 		Besides, REG_MON_BAUD_STS[19:0] field value is the monitor result under XTAL2M Clock.
 * @return The value of the REG_MON_BAUD_STS register.
 */
u32 UART_RxMonitorSatusGet(UART_TypeDef *UARTx)
{
	/*return the monitor status register value*/
	return UARTx->MON_BAUD_STS;
}

/**
 * @brief Fill each IrDA_InitStruct member with its default value.
 * @param IrDA_InitStruct Pointer to a IrDA_InitTypeDef structure which will be initialized.
 * @return None
 */
void UART_IrDAStructInit(IrDA_InitTypeDef *IrDA_InitStruct)
{
	/* Set the default value */
	IrDA_InitStruct->UART_IrDARxInv     = DISABLE;
	IrDA_InitStruct->UART_IrDATxInv     = DISABLE;
	IrDA_InitStruct->UART_LowShift      = UART_IRDA_PULSE_LEFT_SHIFT;
	IrDA_InitStruct->UART_LowShiftVal   = 0;
	IrDA_InitStruct->UART_UpperShift    = UART_IRDA_PULSE_LEFT_SHIFT;
	IrDA_InitStruct->UART_UpperShiftVal = 0;
	IrDA_InitStruct->UART_RxFilterCmd   = ENABLE;
	IrDA_InitStruct->UART_RxFilterThres = 7;
}

/**
 * @brief Configures the UART's IrDA interface.
 * @param UARTx UART device, where x can be 0~3.
 * @param IrDA_InitStruct Pointer to a IrDA_InitTypeDef structure that contains
 * 		the configuration information of the IrDA module.
 * @verbatim
 * 		IrDA_InitStruct-> UART_IrDARxInv:
 * 		IrDA Rx invert bit--------------------MISCR[14]
 * 			- ENABLE: invert irda rx
 * 			- DISABLE: not invert irda rx
 * 		IrDA_InitStruct->UART_IrDATxInv:
 * 		IrDA Tx invert bit---------------------MISCR[13]
 * 			- ENABLE: invert irda tx
 * 			- DISABLE: not invert irda tx
 *
 * 		IrDA_InitStruct->UART_UpperShift:
 * 		Upperbound(right edge) Shift direction---SIR_TX_PWC0[31]
 * 			- UART_IRDA_PULSE_LEFT_SHIFT: shift left
 * 			- UART_IRDA_PULSE_RIGHT_SHIFT: shift right
 * 		IrDA_InitStruct->UpperShiftVal:
 * 		Upperbound Shift value---------------SIR_TX_PWC0[30:16]
 *
 * 		IrDA_InitStruct->UART_LowShift:
 * 		Lowbound(left edge) Shift direction-----SIR_TX_PWC0[15]
 * 			- UART_IRDA_PULSE_LEFT_SHIFT: shift left
 * 			- UART_IRDA_PULSE_RIGHT_SHIFT: shift right
 * 		IrDA_InitStruct->UART_LowShiftVal:
 * 		Lowbound Shift value----------------SIR_TX_PWC0[14:0]
 *
 * 		IrDA_InitStruct->UART_RxFilterThres:
 * 		IrDA RX filter threshold---------------SIR_RX_PFC[15:1]
 * 		IrDA_InitStruct->UART_RxFilterCmd:
 * 		IrDA RX filter enable or disable---------SIR_RX_PFC[0]
 * 			- ENABLE: enable IrDA rx filter
 * 			- DISABLE: disable IrDA rx filter
 * @endverbatim
 * @return None
  */
void UART_IrDAInit(UART_TypeDef *UARTx, IrDA_InitTypeDef *IrDA_InitStruct)
{
	u32 TempMiscr;
	u32 TempTxpulse;
	u32 TempRxPulse;

	/* Check the parameters */
	assert_param(IrDA_InitStruct->UART_UpperShiftVal <= 0x7fff);
	assert_param(IrDA_InitStruct->UART_LowShiftVal <= 0x7fff);
	assert_param(IS_IRDA_PUL_SHIFT(IrDA_InitStruct->UART_LowShift));
	assert_param(IS_IRDA_PUL_SHIFT(IrDA_InitStruct->UART_UpperShift));
	assert_param((IrDA_InitStruct->UART_RxFilterThres <= 0x7fff));

	/*Get the MISCR register value*/
	TempMiscr = UARTx->MISCR;

	/*configure the IrDA RX invert bit*/
	if (IrDA_InitStruct->UART_IrDARxInv != DISABLE) {
		/*invert the irda_rx_i*/
		TempMiscr |= RUART_BIT_IRDA_RX_INV;
	} else {
		/*not invert the irda_rx_i*/
		TempMiscr &= ~RUART_BIT_IRDA_RX_INV;
	}

	/*configure the IrDA TX invert bit*/
	if (IrDA_InitStruct->UART_IrDATxInv != DISABLE) {
		/*invert the irda_tx_o*/
		TempMiscr |= RUART_BIT_IRDA_TX_INV;
	} else {
		/*not invert the irda_tx_o*/
		TempMiscr &= ~RUART_BIT_IRDA_TX_INV;
	}
	UARTx->MISCR = TempMiscr;

	/*Get the TXPLSR register value*/
	TempTxpulse = UARTx->SIR_TX_PWC0;

	/*configure IrDA SIR TX Pulse Width*/
	/*configure Upperbound(right edge) shift direction*/
	if (IrDA_InitStruct->UART_UpperShift == UART_IRDA_PULSE_RIGHT_SHIFT) {
		TempTxpulse |= RUART_BIT_UPPERBOUND_SHIFTRIGHT;
	} else {
		TempTxpulse &= ~RUART_BIT_UPPERBOUND_SHIFTRIGHT;
	}

	/*configure the Upperbound shift value*/
	TempTxpulse &= ~RUART_MASK_UPPERBOUND_SHIFTVAL;
	TempTxpulse |= RUART_UPPERBOUND_SHIFTVAL(IrDA_InitStruct->UART_UpperShiftVal);

	/*configure Lowbound(left edge) shift direction*/
	if (IrDA_InitStruct->UART_LowShift == UART_IRDA_PULSE_RIGHT_SHIFT) {
		TempTxpulse |= RUART_BIT_LOWBOUND_SHIFTRIGHT;
	} else {
		TempTxpulse &= ~RUART_BIT_LOWBOUND_SHIFTRIGHT;
	}

	/*configure the Lowbound shift value*/
	TempTxpulse &= ~RUART_MASK_LOWBOUND_SHIFTVAL;
	TempTxpulse |= RUART_LOWBOUND_SHIFTVAL(IrDA_InitStruct->UART_LowShiftVal);

	UARTx->SIR_TX_PWC0 = TempTxpulse;

	/*Get the RXPLSR register value*/
	TempRxPulse = UARTx->SIR_RX_PFC;

	/*configure IrDA RX filter threshold*/
	TempRxPulse &= ~RUART_MASK_R_SIR_RX_FILTER_THRS;
	TempRxPulse |= RUART_R_SIR_RX_FILTER_THRS(IrDA_InitStruct->UART_RxFilterThres);

	if (IrDA_InitStruct->UART_RxFilterCmd != DISABLE) {
		/*enable IrDA rx filter*/
		TempRxPulse |= RUART_BIT_R_SIR_RX_FILTER_EN;
	} else {
		/*disable IrDA rx filter*/
		TempRxPulse &= ~RUART_BIT_R_SIR_RX_FILTER_EN;
	}

	UARTx->SIR_RX_PFC = TempRxPulse;
}

/**
 * @brief Enable or disable the IrDA function.
 * @param UARTx UART device, where x can be 0~3.
 * @param NewState New state of the IrDA.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void UART_IrDACmd(UART_TypeDef *UARTx, u32 NewState)
{
	if (NewState != DISABLE) {
		/* enable the IrDA */
		UARTx->MISCR |= RUART_BIT_IRDA_ENABLE;
	} else {
		/* disable the IrDA */
		UARTx->MISCR &= ~RUART_BIT_IRDA_ENABLE;
	}
}

/**
 * @brief Configure UART RX Debounce Cycle.
 * @param UARTx UART device, where x can be 0~3.
 * @param RxDebounceCycle UART RX Debounce Cycle.
 * @note unit is cycle of sclk.
 * @return None
 */
void UART_RxDebounceConfig(UART_TypeDef *UARTx, u32 RxDebounceCycle)
{
	u32 TempRXDBCR;

	/* Set UART TX DMA burst size.*/
	TempRXDBCR = UARTx->RXDBCR;
	TempRXDBCR &= ~RUART_MASK_DBNC_CYC;
	TempRXDBCR |= RUART_DBNC_CYC(RxDebounceCycle);
	UARTx->RXDBCR = TempRXDBCR;
}

/**
 * @brief Enable or disable the rx debounce function.
 * @param UARTx UART device, where x can be 0~3.
 * @param NewState New state of rx debounce function.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void UART_RxDebounceCmd(UART_TypeDef *UARTx, u32 NewState)
{
	if (NewState != DISABLE) {
		/* Enable rx debounce */
		UARTx->RXDBCR |= RUART_BIT_DBNC_FEN;
	} else {
		/* Disable rx debounce */
		UARTx->RXDBCR &= ~RUART_BIT_DBNC_FEN;
	}
}

/**
 * @brief Configure UART RX DMA dummy data.
 * @param UARTx UART device, where x can be 0~3.
 * @param Byte dummy data.
 * @return None
 */
void UART_RxDMADummyDataConfig(UART_TypeDef *UARTx, u8 Byte)
{
	u32 TempMiscr;

	/* Set UART RX DMA flow control mode.*/
	TempMiscr = UARTx->MISCR;
	TempMiscr &= ~RUART_MASK_DUMMY_DATA;
	TempMiscr |= RUART_DUMMY_DATA(Byte);
	UARTx->MISCR = TempMiscr;
}

/**
 * @brief Get UART RX DMA dummy flag.
 * @param UARTx UART device, where x can be 0~3.
 * @return Status value, which can be
 * 		- 0: master not read dummy data from rx FIFO
 * 		- 1: master read dummy data from rx FIFO
 */
u32 UART_GetRxDMADummyFlag(UART_TypeDef *UARTx)
{
	if (UARTx->MISCR & RUART_BIT_CLR_DUMMY_FLAG) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * @brief Clear UART RX DMA dummy flag.
 * @param UARTx UART device, where x can be 0~3.
 * @return None
 */
void UART_RxClearDMADummyFlag(UART_TypeDef *UARTx)
{
	UARTx->MISCR |= RUART_BIT_CLR_DUMMY_FLAG;
}

/**
 * @brief Force UART RTS_output to be logic 1(inacitve) or logic 0(active).
 * @param UARTx: where x can be 0/1/2.
 * @param NewState: This parameter can be one of the following values:
 * 		@arg ENABLE: Force RTS_output to be inactive, informing peer device to stop sending data.
 * 		@arg DISABLE: RTS_output stays active until Rx FIFO level reaches RECVTRG and becomes active again once
 * 		Rx FIFO level is below (RECVTRG - 2), informing peer device to continue sending data.
 * @retval None
 * @note BIT_AFE should be set to 1 when call UART_RTSForceCmd().
 */
void UART_RTSForceCmd(UART_TypeDef *UARTx, u32 NewState)
{
	if (NewState == ENABLE) {
		/* RTS is logic 1 */
		UARTx->MCR &= ~RUART_BIT_RTS;
	} else {
		/* RTS is logic 0 */
		UARTx->MCR |= RUART_BIT_RTS;
	}
}

/**
 * @brief Configure UART rx timeout threshold.
 * @param UARTx UART device, where x can be 0~3.
 * @param RxTimeout Rx timeout value, unit is one bit time.
 * @return None
 */
void UART_RxToThreConfig(UART_TypeDef *UARTx, u32 RxTimeout)
{
	assert_param(RxTimeout <= 0xFFFF);

	u32 reg = UARTx->RX_PATH_CTRL;
	reg &= ~RUART_MASK_R_RXTO_THRS;
	reg |= RUART_R_RXTO_THRS(RxTimeout);
	UARTx->RX_PATH_CTRL = reg;
}

/**
 * @brief Configure UART rxfifo empty timeout threshold.
 * @param UARTx UART device, where x can be 0~3.
 * @param RxTimeout Rx timeout counter works in uart_rclk.
 * @return None
 */
void UART_REToThreConfig(UART_TypeDef *UARTx, u32 RxTimeout)
{
	assert_param(RxTimeout <= 0xFFFFFFF);

	u32 reg = UARTx->TH_RETI;
	reg &= ~RUART_MASK_TH_RETI;
	reg |= RUART_TH_RETI(RxTimeout);
	UARTx->TH_RETI = reg;
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