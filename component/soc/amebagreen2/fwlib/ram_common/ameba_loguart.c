/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/**
 * @brief Configure LOGUART TX DMA burst size.
 * @param UARTLOG LOGUART device.
 * @param TxDmaBurstSize LOGUART TX DMA burst size.
 * @note LOGUART tx path2 log_fifo depth is 12 in hardware, so TxDmaBurstSize should be no more than 12.
 * @note TxDmaBurstSize should not be smaller than GDMA_DstMsize in function LOGUART_TXGDMA_Init();
 * 		For better performance, it is suggested to make TxDmaBurstSize equal GDMA_DstMsize, which is 8.
 * @return None
 */
void LOGUART_TXDMAConfig(LOGUART_TypeDef *UARTLOG, u32 TxDmaBurstSize)
{
	u32 Temp;

	/* Set LOGUART TX DMA burst size.*/
	Temp = UARTLOG->LOGUART_UART_MISCR;
	Temp &= ~LOGUART_MASK_TXDMA_BURSTSIZE;
	Temp |= LOGUART_TXDMA_BURSTSIZE(TxDmaBurstSize);

	UARTLOG->LOGUART_UART_MISCR = Temp;
}

/**
 * @brief Configure LOGUART RX DMA burst size.
 * @param UARTLOG LOGUART device.
 * @param RxDmaBurstSize LOGUART RX DMA burst size.
 * @note LOGUART rx fifo depth is 16 in hardware, so RxDmaBurstSize should be no more than 16.
 * @note RxDmaBurstSize should not be smaller than GDMA_SrcMsize in function LOGUART_RXGDMA_Init();
 * 		For better performance, it is suggested to make RxDmaBurstSize equal GDMA_SrcMsize, which is 8.
 * @return None
 */
void LOGUART_RXDMAConfig(LOGUART_TypeDef *UARTLOG, u32 RxDmaBurstSize)
{
	u32 Temp;

	/* Set LOGUART Rx DMA burst size */
	Temp = UARTLOG->LOGUART_UART_MISCR;
	Temp &= ~LOGUART_MASK_RXDMA_BURSTSIZE;
	Temp |= LOGUART_RXDMA_BURSTSIZE(RxDmaBurstSize);

	UARTLOG->LOGUART_UART_MISCR = Temp;
}

/**
 * @brief Enable or disable LOGUART TX DMA.
 * @param UARTLOG LOGUART device.
 * @param NewState New state of LOGUART TX DMA.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void LOGUART_TXDMACmd(LOGUART_TypeDef *UARTLOG, u32 NewState)
{
	if (NewState != DISABLE) {
		/* enable LOGUART TX DMA */
		UARTLOG->LOGUART_UART_MISCR |= LOGUART_BIT_TXDMA_EN;
	} else {
		/* disable LOGUART TX DMA */
		UARTLOG->LOGUART_UART_MISCR &= ~LOGUART_BIT_TXDMA_EN;
	}
}

/**
 * @brief Enable or disable LOGUART RX DMA.
 * @param UARTLOG LOGUART device.
 * @param NewState New state of LOGUART RX DMA.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void LOGUART_RXDMACmd(LOGUART_TypeDef *UARTLOG, u32 NewState)
{
	if (NewState != DISABLE) {
		/* enable LOGUART RX DMA */
		UARTLOG->LOGUART_UART_MISCR |= LOGUART_BIT_RXDMA_EN;
	} else {
		/* disable LOGUART RX DMA */
		UARTLOG->LOGUART_UART_MISCR &= ~LOGUART_BIT_RXDMA_EN;
	}
}

/**
 * @brief Initialize and enable LOGUART TX GDMA.
 * @param UARTLOG LOGUART device.
 * @param GDMA_InitStruct Pointer to a GDMA_InitTypeDef structure that contains config info of GDMA.
 * @param CallbackData GDMA callback data.
 * @param CallbackFunc GDMA callback function.
 * @param pTxBuf Tx Buffer.
 * @param TxCount Tx Count.
 * @retval TRUE or FALSE
 */
bool LOGUART_TXGDMA_Init(
	LOGUART_TypeDef *UARTLOG,
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

	// _memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));
	GDMA_StructInit(GDMA_InitStruct);

	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->MaxMuliBlock  = 1;

	GDMA_InitStruct->GDMA_DIR     = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = GDMA_HANDSHAKE_INTERFACE_UART_LOG_TX;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&UARTLOG->LOGUART_UART_THRx[LOGUART_PATH_INDEX_2];
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);

	GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
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
 * @brief Initialize and enable LOGUART RX GDMA.
 * @param UARTLOG LOGUART device.
 * @param GDMA_InitStruct Pointer to a GDMA_InitTypeDef structure that contains config info of GDMA.
 * @param CallbackData GDMA callback data.
 * @param CallbackFunc GDMA callback function.
 * @param pRxBuf Rx Buffer.
 * @param RxCount Rx Count, 0 will use LOGUART as DMA controller.
 * @retval TRUE or FALSE
 */
bool LOGUART_RXGDMA_Init(
	LOGUART_TypeDef *UARTLOG,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxBuf,
	u32 RxCount
)
{
	u8 GdmaChnl;

	assert_param(GDMA_InitStruct != NULL);

	DCache_CleanInvalidate((u32)pRxBuf, RxCount);

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, INT_PRI_MIDDLE);
	if (GdmaChnl == 0xFF) {
		/* No Available DMA channel */
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	if (RxCount == 0) {
		/* If RxCount is 0, configure LOGUART as flow controller */
		GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem_PerCtrl;
		UARTLOG->LOGUART_UART_MISCR |= LOGUART_BIT_RXDMA_OWNER;
	} else {
		/* If RxCount is not 0, configure GDMA as flow controller */
		GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem;
		UARTLOG->LOGUART_UART_MISCR &= ~LOGUART_BIT_RXDMA_OWNER;
	}

	GDMA_InitStruct->GDMA_SrcHandshakeInterface = GDMA_HANDSHAKE_INTERFACE_UART_LOG_RX;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&UARTLOG->LOGUART_UART_RBR;
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;

	if (((u32)(pRxBuf) & 0x03) == 0) {
		/*  4-bytes aligned, move 4 bytes each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize = MsizeOne;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	} else {
		/*  move 1 byte each DMA transaction */
		GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	}
	GDMA_InitStruct->GDMA_BlockSize = RxCount;
	GDMA_InitStruct->GDMA_DstAddr = (u32)(pRxBuf);

	/* multi block close */
	GDMA_InitStruct->MuliBlockCunt  = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock   = 1;

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
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