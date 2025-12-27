/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "GDMA";
static u8 GDMA_IrqNum[8] = {
	GDMA0_CHANNEL0_IRQ,
	GDMA0_CHANNEL1_IRQ,
	GDMA0_CHANNEL2_IRQ,
	GDMA0_CHANNEL3_IRQ,
	GDMA0_CHANNEL4_IRQ,
	GDMA0_CHANNEL5_IRQ,
	GDMA0_CHANNEL6_IRQ,
	GDMA0_CHANNEL7_IRQ,
};

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup GDMA
  * @brief	GDMA driver modules
  * @{
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup GDMA_Exported_Functions GDMA Exported Functions
  * @{
  */

/**
  * @brief  Fill each GDMA_InitStruct member with its default value.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure which will be
  *         initialized.
  * @retval None
  */
__weak
void GDMA_StructInit(PGDMA_InitTypeDef GDMA_InitStruct)
{
	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));

	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_SrcMsize = MsizeEight;
	GDMA_InitStruct->GDMA_DstMsize = MsizeEight;

	GDMA_InitStruct->SecureTransfer = 0;
}

/**
  * @brief  Initialize the GDMA registers according to the specified parameters
  *         in GDMA_InitStruct.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @retval   None
  */
__weak
void GDMA_Init(u8 GDMA_Index, u8 GDMA_ChNum, PGDMA_InitTypeDef GDMA_InitStruct)
{
	u32 CtlxLow = 0;
	u32 CtlxUp = 0;
	u32 CfgxLow = 0;
	u32 CfgxUp = 0;
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	/* first open secure transfer, then set other registers */
	if (GDMA_InitStruct->SecureTransfer) {
		GDMA->CH[GDMA_ChNum].GDMA_CFGx_H &= ~GDMA_BIT_CFGx_H_PROTCTL;
	} else {
		GDMA->CH[GDMA_ChNum].GDMA_CFGx_H |= GDMA_BIT_CFGx_H_PROTCTL;
	}

	/* Check chanel is avaliable */
	if (GDMA_GET_CHENREG_L_1_CH_EN(GDMA->GDMA_CHENREG_L_1) & BIT(GDMA_ChNum)) {
		/* Disable Channel */
		RTK_LOGW(TAG, "Channel had used; Disable Channel!!!!\n");

		GDMA_Cmd(GDMA_Index, GDMA_ChNum, DISABLE);

	}

	/* Check if there are the pending isr; TFR, Block, Src Tran, Dst Tran, Error */
	GDMA_ClearINT(GDMA_Index, GDMA_ChNum);

	/* Fill in SARx register */
	GDMA->CH[GDMA_ChNum].GDMA_SARx = GDMA_InitStruct->GDMA_SrcAddr;
	/* Fill in DARx register */
	GDMA->CH[GDMA_ChNum].GDMA_DARx = GDMA_InitStruct->GDMA_DstAddr;

	/* CTLx_L: Channel direction/Width/Burst Size/Addr Mode*/
	CtlxLow = GDMA_CTLx_L_DST_TR_WIDTH(GDMA_InitStruct->GDMA_DstDataWidth) |
			  GDMA_CTLx_L_SRC_TR_WIDTH(GDMA_InitStruct->GDMA_SrcDataWidth) |
			  GDMA_CTLx_L_DINC(GDMA_InitStruct->GDMA_DstInc) |
			  GDMA_CTLx_L_SINC(GDMA_InitStruct->GDMA_SrcInc) |
			  GDMA_CTLx_L_DEST_MSIZE(GDMA_InitStruct->GDMA_DstMsize) |
			  GDMA_CTLx_L_SRC_MSIZE(GDMA_InitStruct->GDMA_SrcMsize) |
			  GDMA_CTLx_L_TT_FC(GDMA_InitStruct->GDMA_DIR);
	/*Linked List Enable.*/
	if (GDMA_InitStruct->GDMA_LlpDstEn) {
		CtlxLow |= GDMA_BIT_CTLx_L_LLP_DST_EN;
	}

	if (GDMA_InitStruct->GDMA_LlpSrcEn) {
		CtlxLow |= GDMA_BIT_CTLx_L_LLP_SRC_EN;
	}
	/*Block Size*/
	CtlxUp = GDMA_CTLx_H_BLOCK_TS(GDMA_InitStruct->GDMA_BlockSize);

	/* Fill in CTLx register */
	GDMA->CH[GDMA_ChNum].GDMA_CTLx_L = CtlxLow;
	GDMA->CH[GDMA_ChNum].GDMA_CTLx_H = CtlxUp;

	/* CFGx: Reload enable in muli-block mode*/
	if (GDMA_InitStruct->GDMA_ReloadSrc) {
		CfgxLow |= GDMA_BIT_CFGx_L_RELOAD_SRC;
	}

	if (GDMA_InitStruct->GDMA_ReloadDst) {
		CfgxLow |= GDMA_BIT_CFGx_L_RELOAD_DST;
	}

	CfgxUp = GDMA->CH[GDMA_ChNum].GDMA_CFGx_H;
	/*Handshake set*/
	CfgxUp &= ~(GDMA_MASK_CFGx_H_SRC_PER | GDMA_MASK_CFGx_H_DEST_PER |
				GDMA_BIT_CFGx_H_EXTENDED_SRC_PER1 | GDMA_BIT_CFGx_H_EXTENDED_DEST_PER1);
	/* Src Handshake*/
	if (GDMA_InitStruct->GDMA_SrcHandshakeInterface >= 16) {
		CfgxUp |= GDMA_CFGx_H_SRC_PER(GDMA_InitStruct->GDMA_SrcHandshakeInterface) | GDMA_BIT_CFGx_H_EXTENDED_SRC_PER1;
	} else {
		CfgxUp |= GDMA_CFGx_H_SRC_PER(GDMA_InitStruct->GDMA_SrcHandshakeInterface);
	}
	/* Dst Handshake*/
	if (GDMA_InitStruct->GDMA_DstHandshakeInterface >= 16) {
		CfgxUp |= GDMA_CFGx_H_DEST_PER(GDMA_InitStruct->GDMA_DstHandshakeInterface) | GDMA_BIT_CFGx_H_EXTENDED_DEST_PER1;
	} else {
		CfgxUp |= GDMA_CFGx_H_DEST_PER(GDMA_InitStruct->GDMA_DstHandshakeInterface);
	}

	GDMA->CH[GDMA_ChNum].GDMA_CFGx_L = CfgxLow;
	GDMA->CH[GDMA_ChNum].GDMA_CFGx_H = CfgxUp;

	GDMA_INTConfig(GDMA_Index, GDMA_ChNum, GDMA_InitStruct->GDMA_IsrType, ENABLE);
}

/**
  * @brief  Set LLP mode when use GDMA GDMA_LLPx_L (AmebaZ not support this mode).
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @param  MultiBlockCount: block counts.
  * @param  pGdmaChLli: LLP node list, point to a Linked List Item.
  * @param  round:
  *        @arg 0 Linear list, last node id last one
  *        @arg 1 Last node is not the end.
  * @retval   None
  */
__weak
void GDMA_SetLLP(u8 GDMA_Index, u8 GDMA_ChNum, u32 MultiBlockCount, struct GDMA_CH_LLI *pGdmaChLli, u32 round)
{
	u32 CtlxLow, CtlxUp;
	PGDMA_CH_LLI_ELE pLliEle = &pGdmaChLli->LliEle;
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	//DBG_GDMA_INFO("Block Count %d\n", MultiBlockCount);

	CtlxLow = GDMA->CH[GDMA_ChNum].GDMA_CTLx_L;
	CtlxUp = GDMA->CH[GDMA_ChNum].GDMA_CTLx_H;

	CtlxUp |= GDMA_CTLx_H_BLOCK_TS(pGdmaChLli->BlockSize) ;
	GDMA->CH[GDMA_ChNum].GDMA_CTLx_H = CtlxUp;

	/* GDMA_LLPx_L register */
	GDMA->CH[GDMA_ChNum].GDMA_LLPx_L = (u32) & (pGdmaChLli->LliEle);

	while (MultiBlockCount > 0) {
		pLliEle = &pGdmaChLli->LliEle;

		/* Clear the last element llp enable bit */
		if ((1 == MultiBlockCount) & (~round)) {
			CtlxLow &= ~(GDMA_BIT_CTLx_L_LLP_DST_EN |
						 GDMA_BIT_CTLx_L_LLP_SRC_EN);
		}
		/* Update block size for transfer */
		CtlxUp &= ~(GDMA_MASK_CTLx_H_BLOCK_TS);
		CtlxUp |= GDMA_CTLx_H_BLOCK_TS(pGdmaChLli->BlockSize);

		/* Updatethe Llpx context */
		pLliEle->CtlxLow = CtlxLow;
		pLliEle->CtlxUp = CtlxUp;
		pLliEle->Llpx = (u32)&pGdmaChLli->pNextLli->LliEle;

		DCache_CleanInvalidate((u32)pGdmaChLli, sizeof(struct GDMA_CH_LLI));

		/* Update the Lli and Block size list point to next llp */
		pGdmaChLli = pGdmaChLli->pNextLli;
		MultiBlockCount--;
	}
}

/**
  * @brief  Clear the specidied Pending Interrupt status.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @param  GDMA_IT: specifies the UARTx interrupt sources whose interrupt status will be cleared.
  *          This parameter can be one or combinations of the following values:
  *            @arg TransferType
  *            @arg BlockType
  *            @arg ErrType
  * @note  This function completes clearing the specified type interrupt status. Which type
  *            interrupt status will be cleared, refer to the parameter "GDMA_IT".
  * @return   Active interrupt types whose interrupt status is cleared.
  *               This return value refers to @ref DMA_interrupts_definition
  */
__weak  u32
GDMA_ClearINTPendingBit(u8 GDMA_Index, u8 GDMA_ChNum, u32 GDMA_IT)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	uint32_t temp_isr = 0;

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));
	assert_param(IS_GDMA_CONFIG_IT(GDMA_IT));

	/* get isr */
	if (GDMA_INT_STATUS(GDMA->GDMA_RAWTFR_L, GDMA->GDMA_STATUSTFR_L, GDMA_ChNum)) {
		temp_isr |= (TransferType & GDMA_IT);
	}
	if (GDMA_INT_STATUS(GDMA->GDMA_RAWBLOCK_L, GDMA->GDMA_STATUSBLOCK_L, GDMA_ChNum)) {
		temp_isr |= (BlockType & GDMA_IT);
	}
	/*GDMA_RAWERR_L: protocol error*/
	if (GDMA_INT_STATUS(GDMA->GDMA_RAWERR_L, GDMA->GDMA_STATUSERR_L, GDMA_ChNum)) {
		temp_isr |= (ErrType & GDMA_IT);
	}
	/*GDMA_RAWERR_H, secure error. If DMAC_SECURE_MODE has not been defined, it is rereserved. */
	if (GDMA_INT_STATUS(GDMA->GDMA_RAWERR_H, GDMA->GDMA_STATUSERR_H, GDMA_ChNum)) {
		temp_isr |= (ErrType & GDMA_IT);
	}

	/* clear the selected DMA interrupts */
	if (GDMA_IT & TransferType) {
		GDMA_INT_SET(GDMA->GDMA_CLEARTFR_L, GDMA_ChNum);
	}

	if (GDMA_IT & BlockType) {
		GDMA_INT_SET(GDMA->GDMA_CLEARBLOCK_L, GDMA_ChNum);
	}

	if (GDMA_IT & ErrType) {
		GDMA_INT_SET(GDMA->GDMA_CLEARERR_L, GDMA_ChNum);
		GDMA_INT_SET(GDMA->GDMA_CLEARERR_H, GDMA_ChNum);
	}

	return temp_isr;
}

/**
  * @brief  Clear all the Pending Interrupt status.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @note    Clear all the active interrupts status for the current GDMA channel.
  * @return   Active interrupt types whose interrupt status is cleared.
  *               This return value refers to @ref DMA_interrupts_definition
  */
__weak  u32
GDMA_ClearINT(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	uint32_t temp_isr = 0;

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	/* get isr */
	if (GDMA_INT_STATUS(GDMA->GDMA_RAWTFR_L, GDMA->GDMA_STATUSTFR_L, GDMA_ChNum)) {
		temp_isr |= TransferType;
	}
	if (GDMA_INT_STATUS(GDMA->GDMA_RAWBLOCK_L, GDMA->GDMA_STATUSBLOCK_L, GDMA_ChNum)) {
		temp_isr |= BlockType;
	}
	/*GDMA_RAWERR_L: protocol error*/
	if (GDMA_INT_STATUS(GDMA->GDMA_RAWERR_L, GDMA->GDMA_STATUSERR_L, GDMA_ChNum)) {
		temp_isr |= ErrType;
	}
	/*GDMA_RAWERR_H, secure error. If DMAC_SECURE_MODE has not been defined, it is rereserved. */
	if (GDMA_INT_STATUS(GDMA->GDMA_RAWERR_H, GDMA->GDMA_STATUSERR_H, GDMA_ChNum)) {
		temp_isr |= ErrType;
	}

	/* clear all types of interrupt */
	GDMA_INT_SET(GDMA->GDMA_CLEARTFR_L, GDMA_ChNum);
	GDMA_INT_SET(GDMA->GDMA_CLEARBLOCK_L, GDMA_ChNum);
	GDMA_INT_SET(GDMA->GDMA_CLEARERR_L, GDMA_ChNum);
	GDMA_INT_SET(GDMA->GDMA_CLEARERR_H, GDMA_ChNum);

	return temp_isr;
}

/**
  * @brief  Enables or disables the specified GDMA interrupts.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @param  GDMA_IT: specifies the GDMA interrupt sources to be enabled or disabled.
  *          This parameter can be one or combinations of the following values:
  *            @arg TransferType
  *            @arg BlockType
  *            @arg ErrType
  * @param  NewState: DISABLE/ENABLE.
  * @retval   None
  */
__weak  void
GDMA_INTConfig(u8 GDMA_Index, u8 GDMA_ChNum, u32 GDMA_IT, u32 NewState)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));
	assert_param(IS_GDMA_CONFIG_IT(GDMA_IT));
	if (NewState != DISABLE) {
		/* Enable the Interrupt sources */
		GDMA->CH[GDMA_ChNum].GDMA_CTLx_L |= GDMA_BIT_CTLx_L_INT_EN;
	} else {
		/* Disable the Interrupt sources */
		GDMA->CH[GDMA_ChNum].GDMA_CTLx_L &= ~GDMA_BIT_CTLx_L_INT_EN;
	}
	if (NewState != DISABLE) {
		/* Enable the selected DMA Chan interrupts */
		if (GDMA_IT & TransferType) {
			GDMA_INT_SET(GDMA->GDMA_MASKTFR_L, GDMA_ChNum);
		}

		if (GDMA_IT & BlockType) {
			GDMA_INT_SET(GDMA->GDMA_MASKBLOCK_L, GDMA_ChNum);
		}

		if (GDMA_IT & ErrType) {
			GDMA_INT_SET(GDMA->GDMA_MASKERR_L, GDMA_ChNum);
			GDMA_INT_SET(GDMA->GDMA_MASKERR_H, GDMA_ChNum);
		}
	} else {
		/* Disable the selected DMA Chan interrupts */
		if (GDMA_IT & TransferType) {
			GDMA_INT_MASK(GDMA->GDMA_MASKTFR_L, GDMA_ChNum);
		}

		if (GDMA_IT & BlockType) {
			GDMA_INT_MASK(GDMA->GDMA_MASKBLOCK_L, GDMA_ChNum);
		}

		if (GDMA_IT & ErrType) {
			GDMA_INT_MASK(GDMA->GDMA_MASKERR_L, GDMA_ChNum);
			GDMA_INT_MASK(GDMA->GDMA_MASKERR_H, GDMA_ChNum);
		}
	}
}

/**
  * @brief  GDMA channel Disable/Enable.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @param  NewState: Disable/Enable.
  * @retval   None
  */
__weak  void
GDMA_Cmd(u8 GDMA_Index, u8 GDMA_ChNum, u32 NewState)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	u32 ValTemp = 0;

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	/* ChEnWE is used to tell HW just set the needed ChEn bit */
	/* This allows software to set a mask bit without */
	/* performing a read-modified write operation. */
	ValTemp |= GDMA_CHENREG_L_1_CH_EN_WE(BIT(GDMA_ChNum));

	if (NewState == ENABLE) {
		ValTemp |= GDMA_CHENREG_L_1_CH_EN(BIT(GDMA_ChNum));
	} else {
		ValTemp &= ~GDMA_CHENREG_L_1_CH_EN(BIT(GDMA_ChNum));
	}

	GDMA->GDMA_CHENREG_L_1 = ValTemp;
}

/**
  * @brief  GDMA AutoReload set.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @param  CleanType: This parameter can be any combination of the following values:
  *		 @arg CLEAN_RELOAD_SRC
  *		 @arg CLEAN_RELOAD_DST
  * @retval   None
  */
__weak  void
GDMA_ChCleanAutoReload(u8 GDMA_Index, u8 GDMA_ChNum, u32 CleanType)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	u32 CfgxLow;

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	CfgxLow = GDMA->CH[GDMA_ChNum].GDMA_CFGx_L;

	if (CleanType == CLEAN_RELOAD_SRC) {
		CfgxLow &= ~GDMA_BIT_CFGx_L_RELOAD_SRC;
	} else if (CleanType == CLEAN_RELOAD_DST) {
		CfgxLow &= ~GDMA_BIT_CFGx_L_RELOAD_DST;
	} else {
		CfgxLow &= ~GDMA_BIT_CFGx_L_RELOAD_SRC;
		CfgxLow &= ~GDMA_BIT_CFGx_L_RELOAD_DST;
	}

	GDMA->CH[GDMA_ChNum].GDMA_CFGx_L = CfgxLow;
}

/**
  * @brief  Set source address.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @param  SrcAddr: source address
  * @retval   None
  */
__weak
void GDMA_SetSrcAddr(u8 GDMA_Index, u8 GDMA_ChNum, u32 SrcAddr)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	GDMA->CH[GDMA_ChNum].GDMA_SARx = SrcAddr;
}

/**
  * @brief  Get the source reading address during DMA transmission.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @return Source address
  */
__weak
u32 GDMA_GetSrcAddr(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	return GDMA->CH[GDMA_ChNum].GDMA_CURR_SARx;
}

/**
  * @brief  Get the destination writing address during DMA transmission.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @return Destination address
  */
__weak
u32 GDMA_GetDstAddr(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	return GDMA->CH[GDMA_ChNum].GDMA_CURR_DARx;
}

/**
  * @brief  Set destination address.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @param  DstAddr: destination address
  * @retval   None
  */
__weak
void GDMA_SetDstAddr(u8 GDMA_Index, u8 GDMA_ChNum, u32 DstAddr)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	GDMA->CH[GDMA_ChNum].GDMA_DARx = DstAddr;
}

/**
  * @brief  Set GDMA block size
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @param  BlkSize:
  * @retval   None
  */
__weak
void GDMA_SetBlkSize(u8 GDMA_Index, u8 GDMA_ChNum, u32 BlkSize)
{
	u32 BlkSizeTemp = 0;
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	BlkSizeTemp = GDMA->CH[GDMA_ChNum].GDMA_CTLx_H;

	BlkSizeTemp &= ~GDMA_MASK_CTLx_H_BLOCK_TS;
	BlkSizeTemp |= GDMA_CTLx_H_BLOCK_TS(BlkSize);

	GDMA->CH[GDMA_ChNum].GDMA_CTLx_H = BlkSizeTemp;
}

/**
  * @brief  Get the total number of data bytes written to the target memory or peripheral device during DMA transmission.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @return   Block size configured for the current GDMA channel
  */
__weak
u32 GDMA_GetBlkSize(u8 GDMA_Index, u8 GDMA_ChNum)
{
	u32 BlkSize = 0;
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	BlkSize = GDMA->CH[GDMA_ChNum].GDMA_CTLx_H;
	return BlkSize;
}

/**
  * @brief  Register channel if this channel is used.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @retval None
  */
static void GDMA_ChnlRegister(u8 GDMA_Index, u8 GDMA_ChNum, IRQ_FUN IrqFun, u32 IrqData, u32 IrqPriority)
{
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	u8 IrqNum = 0;

	u8 ValTemp = HAL_READ8(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW + 3);

	/* register idle channel. */
	ValTemp |= BIT(GDMA_ChNum);
	HAL_WRITE8(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW + 3, ValTemp);

	if (IrqFun != NULL) {
		IrqNum = GDMA_IrqNum[GDMA_ChNum];
		InterruptRegister(IrqFun, IrqNum, IrqData, IrqPriority);
		InterruptEn(IrqNum, IrqPriority);
	}
}

/**
  * @brief  Unregister channel if this channel is not used.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @retval   None
  */
static void GDMA_ChnlUnRegister(u8 GDMA_Index, u8 GDMA_ChNum)
{
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	u8 IrqNum = 0;

	u8 ValTemp = HAL_READ8(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW + 3);

	ValTemp &= ~BIT(GDMA_ChNum);
	HAL_WRITE8(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW + 3, ValTemp);

	IrqNum = GDMA_IrqNum[GDMA_ChNum];
	InterruptDis(IrqNum);
	InterruptUnRegister(IrqNum);
}

/**
  * @brief  Allocate a free channel.
  * @param  GDMA_Index: 0 .
  * @param  IrqFun: GDMA IRQ callback function.
  * @param  IrqData: GDMA IRQ callback data.
  * @param  IrqPriority: GDMA IrqPriority.
  * @retval GDMA_ChNum
  */
__weak  u8
GDMA_ChnlAlloc(u32 GDMA_Index, IRQ_FUN IrqFun, u32 IrqData, u32 IrqPriority)
{
	u32 found = 0;
	u32 GDMA_ChNum = 0xFF;
	u8 ValTemp = 0;

	assert_param(IS_GDMA_Index(GDMA_Index));

	if (IPC_SEMTake(IPC_SEM_GDMA, 1000) == FALSE) {
		return GDMA_ChNum;
	}

	/* Match idle channel. */
	ValTemp = HAL_READ8(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW + 3);

	for (GDMA_ChNum = 0; GDMA_ChNum <= MAX_GDMA_CHNL; GDMA_ChNum++) {
		if ((ValTemp & BIT(GDMA_ChNum)) == 0) {
			found = 1;
			break;
		}
	}

	/*If idle channel is found,  register it. */
	if (found) {
		GDMA_ChnlRegister(GDMA_Index, GDMA_ChNum, IrqFun, IrqData, IrqPriority);
	} else {
		GDMA_ChNum = 0xFF;
	}

	IPC_SEMFree(IPC_SEM_GDMA);
	return GDMA_ChNum;
}

/**
  * @brief  Free a channel, this channel will not be used.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @retval TRUE/FLASE
  */
__weak  u8
GDMA_ChnlFree(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = NULL;
	u8 ret = FALSE;
	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	if (IPC_SEMTake(IPC_SEM_GDMA, 1000) == FALSE) {
		return ret;
	}

	if (TrustZone_IsSecure()) {
		/* disable secure, or non-secure can not use this channel */
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
		GDMA->CH[GDMA_ChNum].GDMA_CFGx_H |= GDMA_BIT_CFGx_H_PROTCTL;
	}

	GDMA_ChnlUnRegister(GDMA_Index, GDMA_ChNum);

	IPC_SEMFree(IPC_SEM_GDMA);

	ret = TRUE;
	return ret;
}
/**
 * @brief Get whether the fifo is empty
 *
 * @param  GDMA_Index: 0.
 * @param  GDMA_ChNum: 0 ~ 7.
 * @retval TRUE/FALSE
 */
__weak u8
GDMA_ChnlFIFOIsEmpty(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	u8 ret = FALSE;
	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	if (GDMA->CH[GDMA_ChNum].GDMA_CFGx_L & GDMA_BIT_CFGx_L_FIFO_EMPTY) {
		ret = TRUE;
	}

	return ret;
}
/**
  * @brief  Get irq number for a channel.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @retval   IrqNum
  */
__weak  u8
GDMA_GetIrqNum(u8 GDMA_Index, u8 GDMA_ChNum)
{
	u8 IrqNum;

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));

	IrqNum = GDMA_IrqNum[GDMA_ChNum];

	return IrqNum;
}

/**
  * @brief  Set channel priority.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @retval   IrqNum
  */
__weak  void
GDMA_SetChnlPriority(u8 GDMA_Index, u8 GDMA_ChNum, u32 ChnlPriority)
{
	u32 CfgxLow;
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);

	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}

	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	CfgxLow = GDMA->CH[GDMA_ChNum].GDMA_CFGx_L;

	CfgxLow &= ~GDMA_MASK_CFGx_L_CH_PRIOR;
	CfgxLow |= GDMA_CFGx_L_CH_PRIOR(ChnlPriority);
	GDMA->CH[GDMA_ChNum].GDMA_CFGx_L = CfgxLow;

}

/**
  * @brief  Suspend a channel.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @retval   None
  */
__weak  void
GDMA_Suspend(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}
	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	GDMA->CH[GDMA_ChNum].GDMA_CFGx_L |= GDMA_BIT_CFGx_L_CH_SUSP;
}

/**
  * @brief  Resume a channel.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @retval   None
  */
__weak  void
GDMA_Resume(u8 GDMA_Index, u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}
	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	GDMA->CH[GDMA_ChNum].GDMA_CFGx_L &= ~GDMA_BIT_CFGx_L_CH_SUSP;
}

/**
  * @brief  Abort a channel.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 7.
  * @retval TRUE/FALSE
  */
__weak  u8
GDMA_Abort(u8 GDMA_Index, u8 GDMA_ChNum)
{
	u32 timeout = 500;
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}
	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(IS_GDMA_ChannelNum(GDMA_ChNum));

	GDMA_Suspend(GDMA_Index, GDMA_ChNum);
	/*If ChEnReg[GDMA_ChNum] is not equal to 0, it means that
	  the channel is working and the Suspend status must be checked.*/
	while (timeout--) {
		if ((GDMA_GET_CHENREG_L_1_CH_EN(GDMA->GDMA_CHENREG_L_1) & BIT(GDMA_ChNum)) == 0 || \
			(GDMA->CH[GDMA_ChNum].GDMA_CFGx_L & GDMA_BIT_CFGx_L_INACTIVE)) {
			break;
		}
	}
	/*If the channel is still active after the timeout period, resume is required*/
	if (timeout == 0) {
		GDMA_Resume(GDMA_Index, GDMA_ChNum);
		return FALSE;
	}

	GDMA_Cmd(GDMA_Index, GDMA_ChNum, DISABLE);
	return TRUE;
}

/**
  * @brief  Set Source Gather.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 3.
  * @param  Src_GatherCount: Source continuous transfer count between consecutive gather boundaries,
  * 						  align to src data width.
  * @param  Src_GatherInterval: Source gather interval, align to src data width.
  * @note   1.Must be used after GDMA_Init.
  * @note   2.Only available when the CTLx_L.SINC bit indicates an incrementing address control and
  * 	    SARx should be aligned with SRC_TR_WIDTH
  * @retval   None
  */
__weak
void GDMA_SourceGather(u8 GDMA_Index, u8 GDMA_ChNum, u32 Src_GatherCount, u32 Src_GatherInterval)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}
	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(GDMA_ChNum < 4);
	/*Source gather enable*/
	GDMA->CH[GDMA_ChNum].GDMA_CTLx_L |= GDMA_BIT_CTLx_L_SRC_GATHER_EN;
	GDMA->CH[GDMA_ChNum].GDMA_SGRx_L = GDMA_SGRx_L_SGC(Src_GatherCount) | GDMA_SGRx_L_SGI(Src_GatherInterval);
}

/**
  * @brief  Set destination Scatter.
  * @param  GDMA_Index: 0.
  * @param  GDMA_ChNum: 0 ~ 3.
  * @param  Dst_ScatterCount: Destination contiguous transfer count between successive scatter boundaries,
  * 						   align to dest data width.
  * @param  Dst_ScatterInterval: Destination scatter interval, align to dest data width.
  * @note   1.Must be used after GDMA_Init.
  * @note 	2.Only available when CTLx_L.DINC bit indicates an incrementing address control and
  *         DARx should be aligned with DST_TR_WIDTH.
  * @retval   None
  */
__weak
void GDMA_DestinationScatter(u8 GDMA_Index, u8 GDMA_ChNum, u32 Dst_ScatterCount, u32 Dst_ScatterInterval)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *) GDMA_BASE);
	if (TrustZone_IsSecure()) {
		GDMA = ((GDMA_TypeDef *) GDMA0_REG_BASE_S);
	}
	/* Check the parameters */
	assert_param(IS_GDMA_Index(GDMA_Index));
	assert_param(GDMA_ChNum < 4);
	/*Destination scatter enable*/
	GDMA->CH[GDMA_ChNum].GDMA_CTLx_L |= GDMA_BIT_CTLx_L_DST_SCATTER_EN;
	GDMA->CH[GDMA_ChNum].GDMA_DSRx_L = GDMA_DSRx_L_DSC(Dst_ScatterCount) | GDMA_DSRx_L_DSI(Dst_ScatterInterval);
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