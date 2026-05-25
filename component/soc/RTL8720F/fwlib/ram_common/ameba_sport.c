/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/** @defgroup AUDIO_SPORT_Flag
 * @{
 */

SP_RegTypeDef SP_RegFlag[1] = {{0, 0, 0}};
/**
 * @}
 */

/** @defgroup AUDIO_SPORT_Device
 * @{
 */
static const char *const TAG = "AUDIO";
const AUDIO_DevTable AUDIO_DEV_TABLE[1] = {
	{AUDIO_SPORT_DEV, GDMA_HANDSHAKE_INTERFACE_SPORT0F0_TX, GDMA_HANDSHAKE_INTERFACE_SPORT0F0_RX},   /*audio sport */
};
/**
 * @}
 */

/**
  * @brief  Fills each SP_StructInit member with its default value.
  * @param  SP_StructInit: pointer to an SP_StructInit structure which will be
  *         initialized.
  * @retval None
  */
void AUDIO_SP_StructInit(SP_InitTypeDef *SP_InitStruct)
{
	SP_InitStruct->SP_SelWordLen = SP_WL_16;
	SP_InitStruct->SP_SelDataFormat = SP_DF_I2S;
	SP_InitStruct->SP_SelI2SMonoStereo = SP_CH_STEREO;
	SP_InitStruct->SP_SelCh = SP_RX_CH_LR;
	SP_InitStruct->SP_SR = SP_16K;
}

/**
 * @brief  Initializes the AUDIO SPORT registers according to the specified parameters
 *         in SP_InitStruct. *FIFO_0 and FIFO_1 have the same configuration.
 * @param  index: 0
 * @param  direction: SP_DIR_TX or SP_DIR_RX.
 * @param  SP_InitStruct: pointer to an SP_InitTypeDef structure which will be initialized.
 * @retval None
 */
void AUDIO_SP_Init(u32 index, u32 direction, SP_InitTypeDef *SP_InitStruct)
{
	u32 chn_len = 32;
	assert_param(IS_SP_SET_DIR(direction));

	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;

	/* Enable MCLK */
	AUDIO_SP_SetMclk(index, ENABLE);

	if (direction == SP_DIR_TX) {

		/* Check the parameters*/
		assert_param(IS_SP_DATA_FMT(SP_InitStruct->SP_SelDataFormat));
		assert_param(IS_SP_WORD_LEN(SP_InitStruct->SP_SelWordLen));
		assert_param(IS_SP_SEL_TX_CH(SP_InitStruct->SP_SelCh));
		assert_param(IS_SP_CHN_NUM(SP_InitStruct->SP_SelI2SMonoStereo));

		SP_InitStruct->SP_Bclk = (chn_len * 2 * (SP_InitStruct->SP_SR));
		assert_param(AUDIO_SP_Register(index, direction, SP_InitStruct));

		/* Configure parameters: disable TX, AUDIO SPORT mode */
		AUDIO_SP_TXStart(index, DISABLE);

		/* Configure FIFO0 parameters: word length, data format, channel select, channel number, etc */
		// SPORTx->SP_FORMAT &= ~(SP_BIT_TRX_SAME_LENGTH);
		SPORTx->SP_CTRLR0 &= ~(SP_MASK_DATA_LEN_SEL | SP_MASK_DATA_FORMAT_SEL | SP_BIT_EN_PCM_N_MODE | SP_MASK_SEL_I2S_TX_CH);
		SPORTx->SP_CTRLR0 |= ((SP_DATA_LEN_SEL(SP_InitStruct->SP_SelWordLen)) | ((SP_InitStruct->SP_SelDataFormat) << 8) | (SP_SEL_I2S_TX_CH(
								  SP_InitStruct->SP_SelCh)));

		if (SP_InitStruct->SP_SelI2SMonoStereo == SP_CH_STEREO) {
			SPORTx->SP_CTRLR0 &= ~SP_BIT_EN_I2S_MONO;
		} else {
			SPORTx->SP_CTRLR0 |= SP_BIT_EN_I2S_MONO;
		}

		AUDIO_SP_SetLRClkDiv(index, SP_InitStruct->SP_SelClk, SP_InitStruct->SP_SR, SP_NOTDM, chn_len);

	} else {

		/* Check the parameters*/
		assert_param(IS_SP_DATA_FMT(SP_InitStruct->SP_SelDataFormat));
		assert_param(IS_SP_WORD_LEN(SP_InitStruct->SP_SelWordLen));
		assert_param(IS_SP_SEL_RX_CH(SP_InitStruct->SP_SelCh));
		assert_param(IS_SP_CHN_NUM(SP_InitStruct->SP_SelI2SMonoStereo));

		SP_InitStruct->SP_Bclk = (chn_len * 2 * (SP_InitStruct->SP_SR));
		assert_param(AUDIO_SP_Register(index, direction, SP_InitStruct));

		/* Configure parameters: disable TX, AUDIO SPORT mode */
		AUDIO_SP_RXStart(index, DISABLE);

		/* Configure FIFO0 parameters: word length, data format, channel select, channel number, etc */
		SPORTx->SP_CTRLR0 &= ~(SP_MASK_DATA_LEN_SEL | SP_MASK_DATA_FORMAT_SEL | SP_BIT_EN_PCM_N_MODE | SP_MASK_SEL_I2S_RX_CH);
		SPORTx->SP_CTRLR0 |= ((SP_DATA_LEN_SEL(SP_InitStruct->SP_SelWordLen)) | ((SP_InitStruct->SP_SelDataFormat) << 8) | (SP_SEL_I2S_RX_CH(
								  SP_InitStruct->SP_SelCh)));

		if (SP_InitStruct->SP_SelI2SMonoStereo == SP_CH_STEREO) {
			SPORTx->SP_CTRLR0 &= ~SP_BIT_EN_I2S_MONO;
		} else {
			SPORTx->SP_CTRLR0 |= SP_BIT_EN_I2S_MONO;
		}

		AUDIO_SP_SetLRClkDiv(index, SP_InitStruct->SP_SelClk, SP_InitStruct->SP_SR, SP_NOTDM, chn_len);
	}
}

/**
  * @brief  Reset SPORT module.
  * @param  index: select SPORT.
  * @retval None
  */
void AUDIO_SP_Reset(u32 index)
{
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;
	SPORTx->SP_CTRLR0 |= SP_BIT_RESET;
	SPORTx->SP_CTRLR0 &= ~SP_BIT_RESET;
}

/**
 * @brief  register SPORT if this SPORT is used.
 * @param  index: 0.
 * @param  direction: SP_DIR_TX or SP_DIR_RX.
 * @param  SP_StructInit: pointer to an SP_StructInit structure.
 * @retval value: TRUE/FALSE
 */
bool AUDIO_SP_Register(u32 index, u32 direction, SP_InitTypeDef *SP_InitStruct)
{
	if (direction == SP_DIR_TX) {
		if (SP_RegFlag[index].SP_DIR_TX_flag) {
			RTK_LOGW(TAG, "SPORT TX has been registered!\n");
			return 0;
		} else {
			if (SP_RegFlag[index].SP_DIR_RX_flag) {
				if (SP_RegFlag[index].SP_Bclk == SP_InitStruct->SP_Bclk) {
					SP_RegFlag[index].SP_DIR_TX_flag = ENABLE;
					return 1;
				} else {
					RTK_LOGE(TAG, "SPORT RX Bclk can't be changed!\n");
					return 0;
				}
			} else {
				SP_RegFlag[index].SP_DIR_TX_flag = ENABLE;
				SP_RegFlag[index].SP_Bclk = SP_InitStruct->SP_Bclk;
				return 1;
			}
		}
	} else {
		if (SP_RegFlag[index].SP_DIR_RX_flag) {
			RTK_LOGW(TAG, "SPORT RX has been registered!\n");
			return 0;
		} else {
			if (SP_RegFlag[index].SP_DIR_TX_flag) {
				if (SP_RegFlag[index].SP_Bclk == SP_InitStruct->SP_Bclk) {
					SP_RegFlag[index].SP_DIR_RX_flag = ENABLE;
					return 1;
				} else {
					RTK_LOGE(TAG, "SPORT RX Bclk can't be changed!\n");
					return 0;
				}
			} else {
				SP_RegFlag[index].SP_DIR_RX_flag = ENABLE;
				SP_RegFlag[index].SP_Bclk = SP_InitStruct->SP_Bclk;
				return 1;
			}
		}
	}
}

/**
 * @brief  Set AUDIO SPORT MCLK enable or disable.
 * @param  index: SPORT index.
 * @param  NewState: enable or disable.
 * @retval None
 */
void AUDIO_SP_SetMclk(u32 index, u32 NewState)
{
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;

	if (NewState == ENABLE) {
		SPORTx->SP_CTRLR2 |= SP_BIT_ENABLE_MCLK;
	} else {
		SPORTx->SP_CTRLR2 &= ~SP_BIT_ENABLE_MCLK;
	}
}

/**
 * @brief  Set AUDIO SPORT TX BLCK divider factor.
 * @param  index: 0.
 * @param  clock: SPORT clock source.
 * @param  sr: SPORT TX sample rate.
 * @param  tdm: SP_NOTDM.
 * @param  chn_len: SPORT TX channel length: 32.
 * @retval None
 */
void AUDIO_SP_SetLRClkDiv(u32 index, u32 clock, u32 sr, u32 tdm, u32 chn_len)
{
	u32 Tmp;
	u32 MI = 0;
	u32 NI = 0;
	u32 i = 1;
	u32 bclk;
	u32 channel_count;

	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;

	if (!(clock % 40000000)) {
		if (!(sr % 4000)) {
			MI = 1250;
			NI = (chn_len / 4) * (tdm + 1) * (sr / 4000);
		} else {
			if (chn_len == 32) {
				MI = 50000;
				NI = (441 * (chn_len / 4) * (tdm + 1) * (sr / 11025)) / 4;
			}
		}
	} else {
		channel_count = (tdm + 1) * 2;
		bclk = channel_count * chn_len * sr;
		NI = 1;

		if (clock * NI % bclk == 0) {
			MI = clock * NI / bclk;
		} else {
			for (i = 1; i < 0x7FFF; i++) {
				if (clock * i % bclk == 0) {
					NI = i;
					MI = clock * NI / bclk;
					break;
				}
			}
		}
	}

	Tmp = SPORTx->SP_CLK_DIV;
	Tmp &= ~(SP_MASK_MI | SP_MASK_NI | SP_BIT_MI_NI_UPDATE);
	Tmp |= (SP_MI(MI) | SP_NI(NI) | SP_BIT_MI_NI_UPDATE);
	SPORTx->SP_CLK_DIV = Tmp;
}

/**
 * @brief  Set the AUDIO SPORT MCLK divider factor.
 * @param  index: AUDIO SPORT index.
 * @param  NI/MI: MCLK divider.
 * @param  multiple: MCLK = clock * NI / MI.
 * @note   SP_MCLK_MI_NI_UPDATE bit is used to update mi and ni to get the new MCLK.
 *               This bit will be reset automatically when update is done.
 * @retval None
 */
void AUDIO_SP_SetMclkDiv(u32 index, u32 NI, u32 MI)
{
	u32 Tmp;

	assert_param(NI <= 0x7FFF);
	assert_param(MI <= 0xFFFF);

	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;

	Tmp = SPORTx->SP_MCLK_DIV;
	Tmp &= ~(SP_MASK_MCLK_MI | SP_MASK_MCLK_NI | SP_BIT_MCLK_MI_NI_UPDATE);
	Tmp |= (SP_MCLK_MI(MI) | SP_MCLK_NI(NI) | SP_BIT_MCLK_MI_NI_UPDATE);
	SPORTx->SP_MCLK_DIV = Tmp;
}

/**
  * @brief  Start or stop SPORT Tx.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  NewState: new state of the SPORT Tx.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void AUDIO_SP_TXStart(u32 index, u32 NewState)
{
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;
	if (NewState == ENABLE) {
		SPORTx->SP_CTRLR0 &= ~ SP_BIT_TX_DISABLE;
		SPORTx->SP_CTRLR0 |= SP_BIT_START_TX;
	} else {
		SPORTx->SP_CTRLR0 |= SP_BIT_TX_DISABLE;
		SPORTx->SP_CTRLR0 &= ~ SP_BIT_START_TX;
	}
}

/**
  * @brief  Start or stop SPORT Rx.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  NewState: new state of the SPORT Rx.
  *         This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void AUDIO_SP_RXStart(u32 index, u32 NewState)
{
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;
	if (NewState == ENABLE) {
		SPORTx->SP_CTRLR0 &= ~ SP_BIT_RX_DISABLE;
		SPORTx->SP_CTRLR0 |= SP_BIT_START_RX;
	} else {
		SPORTx->SP_CTRLR0 |= SP_BIT_RX_DISABLE;
		SPORTx->SP_CTRLR0 &= ~ SP_BIT_START_RX;
	}
}

/**
 * @brief  SPORT and GDMA handshake on or off.
 * @param  index: select SPORT.
 * @param  NewState: new state of the handshake of GDMA-SPORT.
 *         This parameter can be: ENABLE or DISABLE.
 * @retval None
 */
void AUDIO_SP_DmaCmd(u32 index, u32 NewState)
{
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;

	if (NewState == ENABLE) {
		SPORTx->SP_CTRLR0 &= ~SP_BIT_DSP_CTL_MODE;
	} else {
		SPORTx->SP_CTRLR0 |= SP_BIT_DSP_CTL_MODE;
	}
}

/**
  * @brief  Set SPORT self-loopback mode.
  * @param  index: select SPORT.
  * @retval None
  */
void AUDIO_SP_SetSelfLPBK(u32 index)
{
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;

	SPORTx->SP_CTRLR0 |= SP_BIT_LOOPBACK;
}

/**
  * @brief  Set the AUDIO SPORT word length.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  SP_WordLen: the value of word length.
  *          This parameter can be one of the following values:
  *            @arg SP_WL_16: sample bit is 16 bit
  *            @arg SP_WL_24: sample bit is 24 bit
  *            @arg SP_WL_8: sample bit is 8 bit
  * @retval None
  */
void AUDIO_SP_SetWordLen(u32 index, u32 SP_WordLen)
{
	assert_param(IS_SP_WORD_LEN(SP_WordLen));
	u32 reg_value;
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;

	reg_value = SPORTx->SP_CTRLR0;
	reg_value &= ~(SP_MASK_DATA_LEN_SEL);
	reg_value |= (SP_WordLen << 12);
	SPORTx->SP_CTRLR0 = reg_value;
}

/**
  * @brief Get AUDIO SPORT word length.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @retval the value of word length.
  *            @arg 0: sample bit is 16 bit
  *            @arg 2: sample bit is 24 bit
  *            @arg 3: sample bit is 8 bit
  */

u32 AUDIO_SP_GetWordLen(u32 index)
{
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;
	u32 len = ((SPORTx->SP_CTRLR0) & SP_MASK_DATA_LEN_SEL) >> 12;
	return len;
}

/**
  * @brief  Set the AUDIO SPORT channel number.
  * @param  SPORTx: pointer to the base addr of AUDIO SPORT peripheral.
  * @param  SP_MonoStereo: mono or stereo.
  *          This parameter can be one of the following values:
  *            @arg SP_CH_STEREO: stereo channel, channel number is 2
  *            @arg SP_CH_MONO: mono channel, channel number is 1
  * @retval None
  */
void AUDIO_SP_SetMonoStereo(AUDIO_SPORT_TypeDef *SPORTx, u32 SP_MonoStereo)
{
	u32 reg_value;
	assert_param(IS_SP_CHN_NUM(SP_MonoStereo));

	reg_value = SPORTx->SP_CTRLR0;
	reg_value &= ~(SP_BIT_EN_I2S_MONO);
	reg_value |= (SP_MonoStereo << 11);
	SPORTx->SP_CTRLR0 = reg_value;
}

/**
 * @brief  SPORT0 can be set to master or slave mode.
 * @param  index: select SPORT.
 * @param  SP_MasterSlave: master or slave.
 *          This parameter can be one of the following values:
 *            @arg MASTER: SPORT master mode
 *            @arg SLAVE: SPORT slave mode
 * @retval None
 */
void AUDIO_SP_SetMasterSlave(u32 index, u32 SP_MasterSlave)
{
	assert_param(IS_SP_SEL_MODE(SP_MasterSlave));
	AUDIO_SPORT_TypeDef *SPORTx = AUDIO_DEV_TABLE[index].SPORTx;

	if (SP_MasterSlave == MASTER) {
		SPORTx->SP_CTRLR0 &= ~(SP_BIT_SLAVE_CLK_SEL | SP_BIT_SLAVE_DATA_SEL);
	} else {
		SPORTx->SP_CTRLR0 |= SP_BIT_SLAVE_CLK_SEL | SP_BIT_SLAVE_DATA_SEL;
	}
}

/**
  * @brief    Initialize GDMA peripheral for sending data.
  * @param  Index: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pTxData: Tx Buffer.
  * @param  Length: Tx Count.
  * @retval   TRUE/FLASE
  */
bool AUDIO_SP_TXGDMA_Init(
	u32 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pTxData,
	u32 Length
)
{
	u8 GdmaChnl;
	u32 WordLen = AUDIO_SP_GetWordLen(Index);

	assert_param(GDMA_InitStruct != NULL);
	/*obtain a DMA channel and register DMA interrupt handler*/
	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, INT_PRI7);
 
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	/*set GDMA initial structure member value*/
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock = 1;
	GDMA_InitStruct->GDMA_DIR = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = AUDIO_DEV_TABLE[Index].Tx_HandshakeInterface;
	GDMA_InitStruct->GDMA_DstAddr = (u32)&AUDIO_DEV_TABLE[Index].SPORTx->SP_TX_DR;
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;

	/*  Cofigure GDMA transfer */
	if (WordLen == SP_WL_8) {
		/*  8bits mode */
		if (((Length & 0x03) == 0) && (((u32)(pTxData) & 0x03) == 0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeOne;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
		} else {
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
			GDMA_InitStruct->GDMA_BlockSize = Length;
		}
		GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthOneByte;
	} else  {
		/*  24bits or 16bits mode */
		if (((Length & 0x03) == 0) && (((u32)(pTxData) & 0x03) == 0)) {
			/*  4-bytes aligned, move 4 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
		} else if (((Length & 0x01) == 0) && (((u32)(pTxData) & 0x01) == 0)) {
			/*  2-bytes aligned, move 2 bytes each transfer */
			GDMA_InitStruct->GDMA_SrcMsize   = MsizeEight;
			GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
			GDMA_InitStruct->GDMA_BlockSize = Length >> 1;
		} else {
			RTK_LOGE(TAG, "AUDIO_SP_TXGDMA_Init: Aligment Err: pTxData=0x%x,  Length=%d\n", pTxData, Length);
			return FALSE;
		}
		GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	}

	/*check GDMA block size*/
	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);

	/*configure GDMA source address */
	GDMA_InitStruct->GDMA_SrcAddr = (u32)pTxData;

	/*  Enable GDMA for TX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	DCache_CleanInvalidate((u32)pTxData, Length);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
  * @brief    Initialize GDMA peripheral for receiving data.
  * @param  Index: 0.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pRxData: Rx Buffer.
  * @param  Length: Rx Count.
  * @retval   TRUE/FLASE
  */
bool AUDIO_SP_RXGDMA_Init(
	u32 Index,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u8 *pRxData,
	u32 Length
)
{
	u8 GdmaChnl;
	u32 WordLen = AUDIO_SP_GetWordLen(Index);

	assert_param(GDMA_InitStruct != NULL);
	/*obtain a DMA channel and register DMA interrupt handler*/
	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, INT_PRI7);
 
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return FALSE;
	}

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	/*set GDMA initial structure member value*/
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->MaxMuliBlock = 1;
	GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_SrcHandshakeInterface = AUDIO_DEV_TABLE[Index].Rx_HandshakeInterface;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)&AUDIO_DEV_TABLE[Index].SPORTx->SP_RX_DR;
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;

	/*  Cofigure GDMA transfer */
	if (WordLen == SP_WL_8)  {
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthOneByte;
		GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;

		GDMA_InitStruct->GDMA_DstMsize = MsizeOne;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length;
	} else {
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;

		GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
		GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
	}

	/*check GDMA block size*/
	assert_param(GDMA_InitStruct->GDMA_BlockSize <= 4096);

	/*configure GDMA destination address */
	GDMA_InitStruct->GDMA_DstAddr = (u32)pRxData;

	/*  Enable GDMA for RX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	DCache_CleanInvalidate((u32)pRxData, Length);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
  * @brief  Audio GDMA Tx restart in isr
  * @param  GDMA_Index: GDMA index used in this routine.
  * @param  GDMA_ChNum: GDMA channel number used in this routine.
  * @param  tx_addr: Address of data to be sent.
  * @param  tx_length: Length of data to be sent.
  * @retval TRUE
  */
bool AUDIO_SP_TXGDMA_Restart(
	u8 GDMA_Index,
	u8 GDMA_ChNum,
	u32 tx_addr,
	u32 tx_length
)
{
	GDMA_SetSrcAddr(GDMA_Index, GDMA_ChNum, tx_addr);
	GDMA_SetBlkSize(GDMA_Index, GDMA_ChNum, tx_length >> 2);
	DCache_CleanInvalidate(tx_addr, tx_length);
	GDMA_Cmd(GDMA_Index, GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
  * @brief  Audio GDMA Rx restart in isr
  * @param  GDMA_Index: GDMA index used in this routine.
  * @param  GDMA_ChNum: GDMA channel number used in this routine.
  * @param  tx_addr: Address of data to be received.
  * @param  tx_length: Length of data to be received.
  * @retval TRUE
  */
bool AUDIO_SP_RXGDMA_Restart(
	u8 GDMA_Index,
	u8 GDMA_ChNum,
	u32 rx_addr,
	u32 rx_length
)
{
	GDMA_SetDstAddr(GDMA_Index, GDMA_ChNum, rx_addr);
	GDMA_SetBlkSize(GDMA_Index, GDMA_ChNum, rx_length >> 2);
	DCache_CleanInvalidate(rx_addr, rx_length);
	GDMA_Cmd(GDMA_Index, GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
 * @brief  Initialize GDMA peripheral linklist mode for sending data.
 * @param  Index: 0.
 * @param  SelGDMA: GDMA_INT.
 * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
 *         the configuration information for the GDMA peripheral.
 * @param  CallbackData: GDMA callback data.
 * @param  CallbackFunc: GDMA callback function.
 * @param  Length: BlockSize.
 * @param  MaxLLP: LLP mode max number.
 * @param  Lli: This parameter stores the address pointing to the next Linked List Item in block chaining.
 * @retval TRUE/FLASE
 */
bool AUDIO_SP_LLPTXGDMA_Init(
	u32 Index,
	u32 SelGDMA,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u32 Length,
	u32 MaxLLP,
	struct GDMA_CH_LLI *Lli)
{
	u32 i;
	u8 GdmaChnl;
	UNUSED(SelGDMA);

	assert_param(GDMA_InitStruct != NULL);
	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	u32 pTxData = Lli[0].LliEle.Sarx;

	/*obtain a DMA channel and register DMA interrupt handler*/
	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, INT_PRI7);
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return FALSE;
	}

	/*set GDMA initial structure member value*/
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->GDMA_ReloadDst = 0;
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->MaxMuliBlock = MaxLLP;
	GDMA_InitStruct->GDMA_DIR = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;
	GDMA_InitStruct->GDMA_LlpSrcEn = 1;
	GDMA_InitStruct->GDMA_LlpDstEn = 1;

	GDMA_InitStruct->GDMA_DstHandshakeInterface = AUDIO_DEV_TABLE[Index].Tx_HandshakeInterface;
	for (i = 0; i < MaxLLP; i++) {
		Lli[i].LliEle.Darx = (u32)&AUDIO_DEV_TABLE[Index].SPORTx->SP_TX_DR;
	}

	/*  Cofigure GDMA transfer */
	/*  24bits or 16bits mode */
	if (((Length & 0x03) == 0) && (((u32)(pTxData) & 0x03) == 0)) {
		/*  4-bytes aligned, move 4 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize = MsizeFour;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 2;
	} else if (((Length & 0x01) == 0) && (((u32)(pTxData) & 0x01) == 0)) {
		/*  2-bytes aligned, move 2 bytes each transfer */
		GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
		GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthTwoBytes;
		GDMA_InitStruct->GDMA_BlockSize = Length >> 1;
	} else {
		RTK_LOGE(TAG, "AUDIO_SP_TXGDMA_Init: Aligment Err: pTxData=0x%lx,  Length=%lu\n", pTxData, Length);
		return FALSE;
	}

	for (i = 0; i < MaxLLP; i++) {
		Lli[i].BlockSize = GDMA_InitStruct->GDMA_BlockSize;
	}

	/*configure GDMA source address */
	GDMA_InitStruct->GDMA_SrcAddr = pTxData;

	/*  Enable GDMA for TX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_SetLLP(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct->MaxMuliBlock, Lli, 1);
	DCache_CleanInvalidate((u32)Lli, MaxLLP * sizeof(*Lli));
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}

/**
 * @brief  Initialize GDMA peripheral linklist mode for receiving data.
 * @param  Index: 0.
 * @param  SelGDMA: GDMA Int or Ext.
 * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
 *         the configuration information for the GDMA peripheral.
 * @param  CallbackData: GDMA callback data.
 * @param  CallbackFunc: GDMA callback function.
 * @param  Length: BlockSize.
 * @param  MaxLLP: LLP mode max number.
 * @param  Lli: This parameter stores the address pointing to the next Linked List Item in block chaining.
 * @retval TRUE/FLASE
 */
bool AUDIO_SP_LLPRXGDMA_Init(
	u32 Index,
	u32 SelGDMA,
	GDMA_InitTypeDef *GDMA_InitStruct,
	void *CallbackData,
	IRQ_FUN CallbackFunc,
	u32 Length,
	u32 MaxLLP,
	struct GDMA_CH_LLI *Lli)
{
	u8 GdmaChnl;
	u32 i;
	UNUSED(SelGDMA);

	assert_param(GDMA_InitStruct != NULL);
	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	/*obtain a DMA channel and register DMA interrupt handler*/
	GdmaChnl = GDMA_ChnlAlloc(0, CallbackFunc, (u32)CallbackData, INT_PRI7);
	if (GdmaChnl == 0xFF) {
		// No Available DMA channel
		return FALSE;
	}

	/*set GDMA initial structure member value*/
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->GDMA_ReloadSrc = 0;
	GDMA_InitStruct->GDMA_ReloadDst = 0;
	GDMA_InitStruct->MaxMuliBlock = MaxLLP;
	GDMA_InitStruct->GDMA_DIR = TTFCPeriToMem;
	GDMA_InitStruct->GDMA_Index = 0;
	GDMA_InitStruct->GDMA_ChNum = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_DstInc = IncType;
	GDMA_InitStruct->GDMA_SrcInc = NoChange;
	GDMA_InitStruct->GDMA_LlpSrcEn = 1;
	GDMA_InitStruct->GDMA_LlpDstEn = 1;

	GDMA_InitStruct->GDMA_SrcHandshakeInterface = AUDIO_DEV_TABLE[Index].Rx_HandshakeInterface;
	for (i = 0; i < MaxLLP; i++) {
		Lli[i].LliEle.Sarx = (u32)&AUDIO_DEV_TABLE[Index].SPORTx->SP_RX_DR;
	}

	/*  Cofigure GDMA transfer */
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;

	GDMA_InitStruct->GDMA_DstMsize = MsizeEight;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_BlockSize = Length >> 2;

	/*check GDMA block size*/
	for (i = 0; i < MaxLLP; i++) {
		Lli[i].BlockSize = GDMA_InitStruct->GDMA_BlockSize;
	}

	/*configure GDMA destination address */
	GDMA_InitStruct->GDMA_DstAddr = Lli[0].LliEle.Darx;

	/*  Enable GDMA for RX */
	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);
	GDMA_SetLLP(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct->MaxMuliBlock, Lli, 1);
	DCache_CleanInvalidate((u32)Lli, MaxLLP * sizeof(*Lli));
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);

	return TRUE;
}
/******************* (C) COPYRIGHT 2017 Realtek Semiconductor *****END OF FILE****/
