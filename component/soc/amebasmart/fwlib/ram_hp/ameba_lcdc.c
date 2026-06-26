/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup LCDC LCDC
  *
  * @brief  LCDC driver modules
  * @{
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup LCDC_Exported_Functions LCDC Exported Functions
  * @{
  */

/*-----------------------------------------------DMA functions------------------------------------------------*/

/**
* @brief  Configure LCDC DMA burst size.
* @param  LCDCx Where LCDCx can be LCDC.
* @param  BurstSize DMA burst size; Unit 64 Bytes.
*                @arg LCDC_LAYER_BURSTSIZE_1X64BYTES:  Burst Transactions = 1;
*                @arg LCDC_LAYER_BURSTSIZE_2X64BYTES:  Burst Transactions = 2;
*                @arg LCDC_LAYER_BURSTSIZE_3X64BYTES:  Burst Transactions = 3;
*                @arg LCDC_LAYER_BURSTSIZE_4X64BYTES:  Burst Transactions = 4;
* @note
*         - If the BurstSize=1, the actual burstsize = 1x64 Bytes; If the BurstSize=2, the actual burstsize = 2x64 = 128 Bytes; etc.
*         - The parameter "BurstSize" value range: 1 to 4.
*/
void LCDC_DMAModeConfig(LCDC_TypeDef *LCDCx, u32 BurstSize)
{
	/*check the parameters*/
	assert_param(IS_LCDC_LAYER_BURSTSIZE(BurstSize));

	/*fill the RDOTST field in register LCDC_DMA_MODE_CFG*/
	LCDCx->LCDC_DMA_MODE_CFG &= ~LCDC_MASK_RD_OTSD;
	LCDCx->LCDC_DMA_MODE_CFG |= LCDC_RD_OTSD(BurstSize - 1);
}

/**
* @brief  Configure LCDC DMA underflow mode and underflow error data.
* @param  LCDCx Where LCDCx can be LCDC.
* @param  DmaUnFlwMode DMA underflow mode, this parameter can be one of the following values:
*                @arg LCDC_DMAUNFW_OUTPUT_LASTDATA:  output last data
*                @arg LCDC_DMAUNFW_OUTPUT_ERRORDATA:  output error data
* @param  ErrorData The output data when DMA FIFO underflow occurred. When underflow mode is configured as
*                LCDC_DMAUNFW_OUTPUT_ERRORDATA, this parameter is needed, and otherwise it can be ignored.
*/
void LCDC_DMAUnderFlowConfig(LCDC_TypeDef *LCDCx, u32 DmaUnFlwMode, u32 ErrorData)
{
	/*read registers for configuration*/
	u32 Value32 = LCDCx->LCDC_UNDFLW_CFG;

	Value32 &= ~LCDC_BIT_DMA_UN_MODE;
	if (DmaUnFlwMode) {
		/*fill the DMAUNMODE field in register LCDC_UNDFLW_CFG*/
		Value32 |= LCDC_BIT_DMA_UN_MODE;

		/*fill the ERROUTDATA field in register LCDC_UNDFLW_CFG*/
		Value32 &= ~LCDC_MASK_ERROUT_DATA;
		Value32 |= LCDC_ERROUT_DATA(ErrorData);
	}

	/*write the value configured back to registers*/
	LCDCx->LCDC_UNDFLW_CFG = Value32;
}

/**
* @brief  Configure LCDC DMA debug write back function.
* @param  LCDCx Where LCDCx can be LCDC.
* @param  DmaWriteBack Secondary RGB output port enabled, the blended data of entire image will be written back to memory.
*         this parameter can be one of the following values:
*                @arg LCDC_DMA_OUT_ENABLE: Enable the debug function
*                @arg LCDC_DMA_OUT_DISABLE: Disable the debug function
* @param  ImgDestAddr Image DMA destination address. (for debug)
*/
void LCDC_DMADebugConfig(LCDC_TypeDef *LCDCx, u32 DmaWriteBack, u32 ImgDestAddr)
{
	LCDCx->LCDC_DMA_MODE_CFG &= ~LCDC_BIT_LCD_DMA_OUT;

	if (DmaWriteBack) {
		/*fill the LCD_DMA_OUT field in register LCDC_DMA_MODE_CFG*/
		LCDCx->LCDC_DMA_MODE_CFG |= LCDC_BIT_LCD_DMA_OUT;

		/*fill the IMG_DEST_ADDR field in register LCDC_SEC_DEST_ADDR*/
		LCDCx->LCDC_SEC_DEST_ADDR = LCDC_IMG_DEST_ADDR(ImgDestAddr);
	}
}

/*---------------------------------------Interrupt functions--------------------------------------------*/

/**
  * @brief  Enable or disable the specified LCDC interrupts.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  LCDC_IT Specifies the LCDC interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg LCDC_BIT_FRM_START_INTEN: DMA frame start interrupt
  *     @arg LCDC_BIT_LCD_LIN_INTEN: Line interrupt
  *     @arg LCDC_BIT_LCD_FRD_INTEN: LCD refresh frame done interrupt
  *     @arg LCDC_BIT_DMA_UN_INTEN: DMA FIFO underflow interrupt
  * @param  NewState New state of the specified LCDC interrupts.
  *         This parameter can be: ENABLE or DISABLE.
  */
void LCDC_INTConfig(LCDC_TypeDef *LCDCx, u32 LCDC_IT, u32 NewState)
{
	if (NewState != DISABLE) {
		/* Enable the selected LCDC interrupts */
		LCDCx->LCDC_IRQ_EN |= LCDC_IT;
	} else {
		/* Disable the selected LCDC interrupts */
		LCDCx->LCDC_IRQ_EN &= ~LCDC_IT;
	}
}

/**
  * @brief  Configure line interrupt position.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  LineNum The specific line number at which the line interrupt is triggered.
  */
void LCDC_LineINTPosConfig(LCDC_TypeDef *LCDCx, u32 LineNum)
{
	/*fill the LINE_INT_POS field in register LCDC_LINE_INT_POS*/
	LCDCx->LCDC_LINE_INT_POS &= ~LCDC_MASK_LINE_INT_POS;
	LCDCx->LCDC_LINE_INT_POS |= LCDC_LINE_INT_POS(LineNum);
}

/**
  * @brief  Get LCDC interrupt status.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @return The interrupt status register value. Each bit indicates an interrupt source:
  *         - LCDC_BIT_FRM_START_INTS: DMA frame start interrupt status
  *         - LCDC_BIT_LCD_LIN_INTS: Line interrupt status
  *         - LCDC_BIT_LCD_FRD_INTS: LCD refresh frame done interrupt status
  *         - LCDC_BIT_DMA_UN_INTS: DMA FIFO underflow interrupt status
  */
u32 LCDC_GetINTStatus(LCDC_TypeDef *LCDCx)
{
	return LCDCx->LCDC_IRQ_STATUS;
}

/**
  * @brief  Get LCDC Raw Interrupt Status.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @return The raw interrupt status register value. Each bit indicates an interrupt source:
  *         - LCDC_BIT_FRM_START_INTRS: DMA frame start raw interrupt status
  *         - LCDC_BIT_LCD_LIN_INTRS: Line raw interrupt status
  *         - LCDC_BIT_LCD_FRD_INTRS: LCD refresh frame done raw interrupt status
  *         - LCDC_BIT_DMA_UN_INTRS: DMA FIFO underflow raw interrupt status
  */
u32 LCDC_GetRawINTStatus(LCDC_TypeDef *LCDCx)
{
	return LCDCx->LCDC_IRQ_RAW;
}

/**
  * @brief  Clear all of the LCDC interrupt pending bits.
  * @param  LCDCx Where LCDCx can be LCDC.
  */
void LCDC_ClearAllINT(LCDC_TypeDef *LCDCx)
{
	/*write 1 to clear all interrupts*/
	LCDCx->LCDC_IRQ_STATUS = 0xFFFFFFFF;
}

/**
  * @brief  Clear the LCDC's interrupt pending bits.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  LCDC_IT Specifies the interrupt to be cleared.
  *   This parameter can be any combination of the following values:
  *     @arg LCDC_BIT_FRM_START_INTS: DMA frame start interrupt
  *     @arg LCDC_BIT_LCD_LIN_INTS: Line interrupt
  *     @arg LCDC_BIT_LCD_FRD_INTS: LCD refresh frame done interrupt
  *     @arg LCDC_BIT_DMA_UN_INTS: DMA FIFO underflow interrupt
  */
void LCDC_ClearINT(LCDC_TypeDef *LCDCx, u32 LCDC_IT)
{
	/*check the parameters*/
	assert_param(IS_LCDC_CLEAR_IT(LCDC_IT));

	/*clear the specified interrupt*/
	LCDCx->LCDC_IRQ_STATUS = LCDC_IT;
}

/**
  * @brief  Get the current position.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  pCurPosX The current X position pointer.
  * @param  pCurPosY The current Y position pointer.
  */
void LCDC_GetCurPosStatus(LCDC_TypeDef *LCDCx, u32 *pCurPosX, u32 *pCurPosY)
{
	/*Get the X position*/
	*pCurPosX = LCDC_GET_CUR_POS_X(LCDCx->LCDC_CUR_POS_STATUS);

	/*Get the Y position*/
	*pCurPosY = LCDC_GET_CUR_POS_Y(LCDCx->LCDC_CUR_POS_STATUS);
}

/**
  * @brief  Get the DMA FIFO underflow interrupt count.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  DmaUnIntCnt The DMA underflow interrupt count pointer.
  */
void LCDC_GetDmaUnINTCnt(LCDC_TypeDef *LCDCx, u32 *DmaUnIntCnt)
{
	/*get the DMA underflow interrupt count*/
	*DmaUnIntCnt = (LCDCx->LCDC_STATUS & LCDC_MASK_DMA_UNINT_CNT);
}

/*------------------------------------------------Global  APIs------------------------------------------------*/

/**
  * @brief  Enable or disable the LCDC.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  NewState New state of the LCDC.
  *                   This parameter can be: ENABLE or DISABLE.
  * @note
  *         - When NewState is DISABLE, the disable action will be performed instantly.
  *         - HW will reset LCDC internal states and disable LCDC, then clear both the disable bit and the enable bit (LCDCEN).
  */
void LCDC_Cmd(LCDC_TypeDef *LCDCx, u32 NewState)
{
	u32 TempCtrl = LCDCx->LCDC_CTRL;

	if (NewState != DISABLE) {
		/* clear the disable bit, otherwise LCDC cannot be enabled successfully */
		TempCtrl &= ~LCDC_BIT_DIS;

		/*set ENABLE bit*/
		TempCtrl |= LCDC_BIT_EN;
	} else {
		/*set DISABLE bit*/
		/*Note: HW will clear LCDC_BIT_EN, LCDC_BIT_DIS, and stop the DMA module*/
		TempCtrl |= LCDC_BIT_DIS;
	}

	LCDCx->LCDC_CTRL = TempCtrl;
}

/**
  * @brief  Deinitialize the LCDC.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @note   Disable LCDC instantly, clear and disable all interrupts.
  */
void LCDC_DeInit(LCDC_TypeDef *LCDCx)
{
	/*disable LCDC instantly*/
	LCDCx->LCDC_CTRL |= LCDC_BIT_DIS;

	/*clear all interrupts*/
	LCDCx->LCDC_IRQ_STATUS = 0xFFFFFFFF;

	/*disable all interrupts*/
	LCDCx->LCDC_IRQ_EN = 0;
}

/**
  * @brief  Check LCDC is ready to work with VO interface after LCDC is enabled.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @return Status:
  *         - TRUE: Ready
  *         - FALSE: Not ready
  */
u8 LCDC_CheckLCDCReady(LCDC_TypeDef *LCDCx)
{
	/*get the LCDC Ready state of LCDC_STATUS*/
	if (LCDCx->LCDC_STATUS & LCDC_BIT_LCDCREADY) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
  * @brief  Set the LCDC Plane Size
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  ImageWidth The width of image (X-channel based), which means pixel number per line.
  * @param  ImageHeight The height of image (Y-channel based)
  */
void LCDC_SetPlaneSize(LCDC_TypeDef *LCDCx, u32 ImageWidth, u32 ImageHeight)
{
	/*read registers for configuration*/
	u32 Value32 = LCDCx->LCDC_PLANE_SIZE;

	/*configure LCDC Plane Size*/
	Value32 &= ~(LCDC_MASK_IMAGEWIDTH | LCDC_MASK_IMAGEHEIGHT);
	Value32 |= LCDC_IMAGEWIDTH(ImageWidth);
	Value32 |= LCDC_IMAGEHEIGHT(ImageHeight);

	/*write the value configured back to registers*/
	LCDCx->LCDC_PLANE_SIZE = Value32;
}


/**
  * @brief  Set the LCDC background color
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  red_color Red component of background color.
  * @param  green_color Green component of background color.
  * @param  blue_color Blue component of background color.
  */
void LCDC_SetBkgColor(LCDC_TypeDef *LCDCx, u8 red_color, u8 green_color, u8 blue_color)
{
	/*read registers for configuration*/
	u32 Value32 = LCDCx->LCDC_BKG_COLOR;

	/*configure LCDC BackGround Color*/
	Value32 &= ~(LCDC_MASK_BKG_RED | LCDC_MASK_BKG_GREEN | LCDC_MASK_BKG_BLUE);
	Value32 |= LCDC_BKG_RED(red_color);
	Value32 |= LCDC_BKG_GREEN(green_color);
	Value32 |= LCDC_BKG_BLUE(blue_color);

	/*write the value configured back to registers*/
	LCDCx->LCDC_BKG_COLOR = Value32;
}

/**
  * @brief  Trigger Layer's shadow register reload to apply new configuration
  * @param  LCDCx Where LCDCx can be LCDC.
  * @note   The shadow registers read back the active values. Until the reload has been done, the 'old' value is read.
  */
void LCDC_TrigerSHWReload(LCDC_TypeDef *LCDCx)
{
	LCDCx->LCDC_SHW_RLD_CFG |= LCDC_BIT_VBR;
}

/**
  * @brief    Initialize the LCDC Layer according to the specified parameters in the EachLayer.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  LayerId Specifies the LCDC layer to configure.
  * @param  EachLayer Pointer to a @ref LCDC_LayerConfigTypeDef structure that contains
  *              the configuration information for the specified LCDC layer.
  */
void LCDC_LayerConfig(LCDC_TypeDef *LCDCx, u8 LayerId, LCDC_LayerConfigTypeDef *EachLayer)
{
	/*variables for reading register*/
	u32 Ctrl;
	u32 BaseAddr;
	u32 WinXpos;
	u32 WinYpos;
	u32 ColorKey;
	u32 Alpha;

	/*check the parameters*/
	assert_param(IS_LCDC_A_VALID_LAYER(LayerId));

	LCDC_Layer_TypeDef *LCDC_Layerx = &LCDCx->LCDC_LAYER[LayerId];

	/*Note: the setting of this layer is not clear by LCDC_BIT_LAYERx_IMG_LAYER_EN bit*/
	if (DISABLE == EachLayer->LCDC_LayerEn) {
		LCDC_Layerx->LCDC_LAYERx_CTRL = 0;
		return;
	}

	/*read registers for configuration*/
	Ctrl = LCDC_Layerx->LCDC_LAYERx_CTRL;
	BaseAddr = LCDC_Layerx->LCDC_LAYERx_BASE_ADDR;
	WinXpos = LCDC_Layerx->LCDC_LAYERx_WIN_XPOS;
	WinYpos = LCDC_Layerx->LCDC_LAYERx_WIN_YPOS;
	ColorKey = LCDC_Layerx->LCDC_LAYERx_COLOR_KEY;
	Alpha = LCDC_Layerx->LCDC_LAYERx_ALPHA;

	/*configure layer ctrl register*/
	Ctrl &= ~(LCDC_BIT_LAYERx_IMG_LAYER_EN | LCDC_BIT_LAYERx_COLOR_KEYING_EN | LCDC_MASK_LAYERx_IMG_FORMAT);
	Ctrl |= LCDC_BIT_LAYERx_IMG_LAYER_EN;
	if (EachLayer->LCDC_LayerColorKeyingEn) {
		Ctrl |= LCDC_BIT_LAYERx_COLOR_KEYING_EN;
	}
	Ctrl |= LCDC_LAYERx_IMG_FORMAT(EachLayer->LCDC_LayerImgFormat);

	/*configure layer base address*/
	BaseAddr = LCDC_LAYERx_IMG_BASE_ADDR(EachLayer->LCDC_LayerImgBaseAddr);

	/*configure layer windows x position ctrl*/
	WinXpos &= ~(LCDC_MASK_LAYERx_WIN_X_START | LCDC_MASK_LAYERx_WIN_X_STOP);
	WinXpos |= LCDC_LAYERx_WIN_X_START(EachLayer->LCDC_LayerHorizontalStart);/*1-based*/
	WinXpos |= LCDC_LAYERx_WIN_X_STOP(EachLayer->LCDC_LayerHorizontalStop);

	/*configure layer windows y position ctrl*/
	WinYpos &= ~(LCDC_MASK_LAYERx_WIN_Y_START | LCDC_MASK_LAYERx_WIN_Y_STOP);
	WinYpos |= LCDC_LAYERx_WIN_Y_START(EachLayer->LCDC_LayerVerticalStart);/*1-based*/
	WinYpos |= LCDC_LAYERx_WIN_Y_STOP(EachLayer->LCDC_LayerVerticalStop);

	/*configure layer color keying data*/
	ColorKey &= ~LCDC_MASK_LAYERx_COLOR_KEY_VALUE;
	ColorKey |= LCDC_LAYERx_COLOR_KEY_VALUE(EachLayer->LCDC_LayerColorKeyingVal);

	/*configure layer alpha register*/
	Alpha &= ~(LCDC_MASK_LAYERx_BF1 | LCDC_MASK_LAYERx_CONSTA);
	Alpha |= LCDC_LAYERx_BF1(EachLayer->LCDC_LayerBlendConfig);
	Alpha |= LCDC_LAYERx_CONSTA(EachLayer->LCDC_LayerConstAlpha);

	/*write the value configured back to registers*/
	LCDC_Layerx->LCDC_LAYERx_CTRL = Ctrl;
	LCDC_Layerx->LCDC_LAYERx_BASE_ADDR = BaseAddr;
	LCDC_Layerx->LCDC_LAYERx_WIN_XPOS = WinXpos;
	LCDC_Layerx->LCDC_LAYERx_WIN_YPOS = WinYpos;
	LCDC_Layerx->LCDC_LAYERx_COLOR_KEY = ColorKey;
	LCDC_Layerx->LCDC_LAYERx_ALPHA = Alpha;
}

/**
  * @brief  Fill each LCDC_InitTypeDef member with its default value.
  * @param  LCDC_InitStruct Pointer to an @ref LCDC_InitTypeDef structure which will be initialized.
  */
void LCDC_StructInit(LCDC_InitTypeDef *LCDC_InitStruct)
{
	u8 idx;
	LCDC_InitStruct->LCDC_ImageWidth = 0;
	LCDC_InitStruct->LCDC_ImageHeight = 0;
	LCDC_InitStruct->LCDC_BgColorRed = 0;
	LCDC_InitStruct->LCDC_BgColorGreen = 0;
	LCDC_InitStruct->LCDC_BgColorBlue = 0;

	for (idx = 0; idx < LCDC_LAYER_MAX_NUM; idx++) {
		LCDC_InitStruct->layerx[idx].LCDC_LayerEn = DISABLE;
		LCDC_InitStruct->layerx[idx].LCDC_LayerColorKeyingEn = DISABLE;
		LCDC_InitStruct->layerx[idx].LCDC_LayerImgFormat = LCDC_LAYER_IMG_FORMAT_ARGB8888;
		LCDC_InitStruct->layerx[idx].LCDC_LayerImgBaseAddr = 0;
		LCDC_InitStruct->layerx[idx].LCDC_LayerHorizontalStart = 0;
		LCDC_InitStruct->layerx[idx].LCDC_LayerHorizontalStop = 0;
		LCDC_InitStruct->layerx[idx].LCDC_LayerVerticalStart = 0;
		LCDC_InitStruct->layerx[idx].LCDC_LayerVerticalStop = 0;
		LCDC_InitStruct->layerx[idx].LCDC_LayerColorKeyingVal = 0;
		LCDC_InitStruct->layerx[idx].LCDC_LayerBlendConfig = 0;
		LCDC_InitStruct->layerx[idx].LCDC_LayerConstAlpha = 0xFF;
	}
}

/**
  * @brief  Initialize the LCDC according to the specified parameters in the LCDC_InitStruct.
  * @param  LCDCx Where LCDCx can be LCDC.
  * @param  LCDC_InitStruct Pointer to a @ref LCDC_InitTypeDef structure that contains
  *              the configuration information for all LCDC layers.
  */
void LCDC_Init(LCDC_TypeDef *LCDCx, LCDC_InitTypeDef *LCDC_InitStruct)
{
	u8 idx;

	LCDC_SetPlaneSize(LCDCx, LCDC_InitStruct->LCDC_ImageWidth, LCDC_InitStruct->LCDC_ImageHeight);
	LCDC_SetBkgColor(LCDCx, LCDC_InitStruct->LCDC_BgColorRed, LCDC_InitStruct->LCDC_BgColorGreen, LCDC_InitStruct->LCDC_BgColorBlue);

	for (idx = 0; idx < LCDC_LAYER_MAX_NUM; idx++) {
		LCDC_LayerConfig(LCDCx, idx, &LCDC_InitStruct->layerx[idx]);
	}

	LCDC_TrigerSHWReload(LCDCx);
}
/** @} */

/** @} */

/** @} */
