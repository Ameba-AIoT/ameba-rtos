/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup KSCAN
* @brief KSCAN driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup KSCAN_Exported_Functions KSCAN Exported Functions
  * @{
  */

/**
  * @brief  Fills each KeyScan_InitStruct member with its default value.
  * @param  KeyScan_InitStruct: pointer to an KeyScan_InitTypeDef structure which will be initialized.
  * @retval None
  */
void KeyScan_StructInit(KeyScan_InitTypeDef *KeyScan_InitStruct)
{
	/* Load HAL initial data structure default value */
	KeyScan_InitStruct->KS_ClkDiv = 199;
	KeyScan_InitStruct->KS_ColSel = 0x03;
	KeyScan_InitStruct->KS_RowSel = 0x03;

	KeyScan_InitStruct->KS_WorkMode = KS_REGULAR_SCAN_MODE;

	KeyScan_InitStruct->KS_DebounceCnt = 0x3e7;
	KeyScan_InitStruct->KS_IntervalCnt = 0x7cf;
	KeyScan_InitStruct->KS_ReleaseCnt = 0x3e7;

	KeyScan_InitStruct->KS_ThreholdLevel = 8;
	KeyScan_InitStruct->KS_LimitLevel = 2;
	KeyScan_InitStruct->KS_OverCtrl = KS_FIFO_OVER_CTRL_DIS_NEW;
}

/**
  * @brief  Initializes the KeyScan peripheral according to the specified
  *			parameters in the KeyScan_InitStruct.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_InitStruct: pointer to a KeyScan_InitTypeDef structure that contains
  * 		the configuration information for the specified KeyScan peripheral.
  * @retval None
  */
void KeyScan_Init(KSCAN_TypeDef *KeyScan, KeyScan_InitTypeDef *KeyScan_InitStruct)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));
	assert_param(KeyScan_InitStruct->KS_ClkDiv <= 0xfff);
	assert_param((KeyScan_InitStruct->KS_ColSel >= 0x1) && (KeyScan_InitStruct->KS_ColSel <= 0xff));
	assert_param((KeyScan_InitStruct->KS_RowSel >= 0x1) && (KeyScan_InitStruct->KS_RowSel <= 0xff));
	assert_param(KeyScan_InitStruct->KS_DebounceCnt <= 0xfff);
	assert_param(KeyScan_InitStruct->KS_IntervalCnt <= 0xfff);
	assert_param(KeyScan_InitStruct->KS_ReleaseCnt <= 0xfff);
	assert_param(IS_KS_WORK_MODE(KeyScan_InitStruct->KS_WorkMode));
	assert_param(IS_KS_FIFO_OVER_CTRL(KeyScan_InitStruct->KS_OverCtrl));
	assert_param(IS_KS_FIFO_LIMIT_LEVEL(KeyScan_InitStruct->KS_LimitLevel));
	assert_param(IS_KS_FIFO_TH_LEVEL(KeyScan_InitStruct->KS_ThreholdLevel));

	/* Disable the IC first */
	KeyScan->KS_CTRL &= (~KS_BIT_RUN_ENABLE);

	/* Mask all keyscan interrupt */
	KeyScan->KS_IMR &= (~KS_BIT_ALL_INT_MSK);

	/* clock divider config */
	KeyScan->KS_CLK_DIV &= (~KS_MASK_CLK_DIV);
	KeyScan->KS_CLK_DIV |= KS_CLK_DIV(KeyScan_InitStruct->KS_ClkDiv);

	/* Config scan mode*/
	KeyScan->KS_CTRL &= (~KS_BIT_WORK_MODE);
	if (KeyScan_InitStruct->KS_WorkMode) {
		KeyScan->KS_CTRL |= KS_BIT_WORK_MODE;
	}

	KeyScan->KS_FIFO_CFG = KS_FIFO_TH_LEVEL(KeyScan_InitStruct->KS_ThreholdLevel) | \
						   KS_FIFO_LIMIT_LEVEL(KeyScan_InitStruct->KS_LimitLevel);
	if (KeyScan_InitStruct->KS_OverCtrl) {
		KeyScan->KS_FIFO_CFG |= KS_BIT_FIFO_OV_CTRL;
	}

	/* time count config */
	KeyScan->KS_TIM_CFG0 &= (~KS_MASK_DEB_TIMER);
	KeyScan->KS_TIM_CFG0 |= KS_DEB_TIMER(KeyScan_InitStruct->KS_DebounceCnt);
	KeyScan->KS_TIM_CFG1 = KS_INTERVAL_TIMER(KeyScan_InitStruct->KS_IntervalCnt) | \
						   KS_RELEASE_TIMER(KeyScan_InitStruct->KS_ReleaseCnt);

	/* Set col map, config which col to work */
	KeyScan->KS_COL_CFG = KS_COL_SEL(KeyScan_InitStruct->KS_ColSel);

	/* Set row map, config which row to work */
	KeyScan->KS_ROW_CFG = KS_ROW_SEL(KeyScan_InitStruct->KS_RowSel);

	/* clear all interrupt status and status flag */
	KeyScan->KS_ICR |= KS_BIT_ALL_INT_CLR;

	return;
}

/**
  * @brief  Enables or disables the specified KeyScan interrupts mask.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_IT: specifies the KeyScan interrupt sources to be enabled or masked.
  *          This parameter can be one or combinations of the following values:
  *            @arg KS_BIT_ALL_DEFAULT_INT_MASK: Unmask all default interrupt
  *            @arg KS_BIT_FALSE_ALARM_INT_MASK: Unmask false alarm interrupt
  *            @arg KS_BIT_STUCK_EVENT_INT_MASK: Unmask Stuck event interrupt
  *            @arg KS_BIT_SCAN_EVENT_INT_MASK: Unmask Scan event interrupt
  *            @arg KS_BIT_FIFO_LIMIT_INT_MASK: Unmask FIFO limit interrupt
  *            @arg KS_BIT_FIFO_OV_INT_MASK: Unmask FIFO overflow interrupt
  *            @arg KS_BIT_FIFO_FULL_INT_MASK: Unmask FIFO full interrupt
  *            @arg KS_BIT_SCAN_FINISH_INT_MASK: Unmask scan finish interrupt
  *            @arg KS_BIT_FIFO_NOTEMPTY_INT_MASK: Unmask FIFO nonempty interrupt
  *            @arg KS_BIT_ALL_RELEASE_INT_MASK: Unmask All Release interrupt
  * @param  NewState: new state of the specified KeyScan interrupts mask.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void KeyScan_INTConfig(KSCAN_TypeDef *KeyScan, u32 KeyScan_IT, u8 newState)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	if (newState == ENABLE) {
		/* Enable KeyScan interrupts */
		KeyScan->KS_IMR |= KeyScan_IT;
	} else {
		/* Disable KeyScan interrupts */
		KeyScan->KS_IMR &= (~KeyScan_IT);
	}
}

/**
  * @brief  Clears the specified KeyScan interrupt pending bit.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_IT: specifies the KeyScan interrupt to be cleared.
  *          This parameter can be one or combinations of the following values:
  *            @arg KS_BIT_ALL_DEFAULT_INT_CLR: all default flag
  *            @arg KS_BIT_FALSE_ALARM_INT_CLR: false alarm flag
  *            @arg KS_BIT_STUCK_INT_CLR: Stuck event interrupt flag
  *            @arg KS_BIT_FIFO_LIMIT_INT_CLR: FIFO limit interrupt flag
  *            @arg KS_BIT_FIFO_OV_INT_CLR: FIFO overflow interrupt flag
  *            @arg KS_BIT_SCAN_FINISH_INT_CLR: Scan finish interrupt flag
  *            @arg KS_BIT_ALL_RELEASE_INT_CLR: All Release interrupt flag
  * @note   KS_BIT_SCAN_EVENT_INT_STATUS is automatically cleared by hardware when the data is read.
  *             KS_BIT_FIFO_FULL_INT_STATUS is automatically cleared by hardware when the buffer level
  *        goes below the KS_FIFO_TH_LEVEL threshold.
  *             KS_BIT_FIFO_NOTEMPTY_INT_STATUS is automatically cleared by hardware when the FIFO is empty.
  * @retval None
  */
void KeyScan_ClearINT(KSCAN_TypeDef *KeyScan, u32 KeyScan_IT)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	KeyScan->KS_ICR |= KeyScan_IT;
}

/**
  * @brief  Get KeyScan Raw Interrupt Status.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval raw interrupt status
  */
u32 KeyScan_GetRawINT(KSCAN_TypeDef *KeyScan)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	return KeyScan->KS_ISR_RAW;
}

/**
  * @brief  Get KeyScan interrupt status.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval interrupt status
  */
u32 KeyScan_GetINT(KSCAN_TypeDef *KeyScan)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	return KeyScan->KS_ISR;
}

/**
  * @brief  Get data number of keyscan FIFO.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval data number of keyscan FIFO
  */
u8 KeyScan_GetDataNum(KSCAN_TypeDef *KeyScan)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	return KS_GET_FIFO_DATA_LEVEL(KeyScan->KS_DATA_NUM);
}

/**
  * @brief  Clears the FIFO data.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval None
  */
void KeyScan_ClearFIFOData(KSCAN_TypeDef *KeyScan)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	KeyScan->KS_FIFO_CFG |= KS_BIT_FIFO_CLR;
}

/**
  * @brief  Get status of keyscan FIFO.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  KeyScan_Flag: specifies the flag to check.
  *   This parameter can be one of the following values:
  *     @arg KS_BIT_FIFO_FULL
  *     @arg KS_BIT_FIFO_EMPTY
  * @retval the new statue of the specified flag.
  *   This parameter can be: TRUE or FALSE.
  */
bool KeyScan_GetFIFOState(KSCAN_TypeDef *KeyScan, u32 KeyScan_Flag)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	if ((KeyScan->KS_DATA_NUM & KeyScan_Flag) != 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
  * @brief  Read data from keyscan FIFO.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  *outBuf: buffer to save data read from KeyScan FIFO.
  * @param  count: number of data to be read.
  * @retval None
  */
void KeyScan_Read(KSCAN_TypeDef *KeyScan, u32 *outBuf, u8 count)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	for (TempVal = 0; TempVal < count; TempVal++) {
		*outBuf++ = KS_GET_DATA(KeyScan->KS_DATA);
	}
}

/**
  * @brief  Enables or disables the specified KeyScan peripheral.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  NewState: new state of the KeyScan peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void KeyScan_Cmd(KSCAN_TypeDef *KeyScan, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	if (NewState != DISABLE) {
		/* Enable the KeyScan peripheral */
		KeyScan->KS_CTRL |= KS_BIT_RUN_ENABLE;
	} else {
		/* Disable the KeyScan peripheral */
		KeyScan->KS_CTRL &= (~KS_BIT_RUN_ENABLE);
	}
}

/**
  * @brief  Set Keyscan Column and Row that used.
  * @param  column_sel: Specifies which column(s) is used.
  *   This parameter must be set to a value in the 0x1-0xff range.
  * @param  row_sel: Specifies which row(s) is used.
  *   This parameter must be set to a value in the 0x1-0xff range.
  * @retval None
  */
void KeyScan_SetColRow(KSCAN_TypeDef *KeyScan, u32 column_sel, u32 row_sel)
{
	assert_param((column_sel >= 0x1) && (column_sel <= 0xff));
	assert_param((row_sel >= 0x1) && (row_sel <= 0xff));

	/* Set col map, config which col to work */
	KeyScan->KS_COL_CFG = KS_COL_SEL(column_sel);

	/* Set row map, config which row to work */
	KeyScan->KS_ROW_CFG = KS_ROW_SEL(row_sel);
}

/**
  * @brief  Enables or disables KeyScan stuck auto check function.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  NewState: new state of the KeyScan stuck auto check function.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void KeyScan_StuckAutoCmd(KSCAN_TypeDef *KeyScan, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	if (NewState != DISABLE) {
		/* Enable the KeyScan stuck auto check function */
		KeyScan->KS_STUCK_CFG0 |= KS_BIT_AUTO_CHECK_STUCK_ENABLE;
	} else {
		/* Disable the KeyScan stuck auto check function */
		KeyScan->KS_STUCK_CFG0 &= (~KS_BIT_AUTO_CHECK_STUCK_ENABLE);
	}
}

/**
  * @brief  Set stuck time threshold.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  ThresholdMs: stuck time threshold convert to ms.
  *   ThresholdMs = (Threshold +1)* Scan_clk(ms).
  *   Threshold must be set to a value in the 0x0-0x7fffffff range.
  * @retval None
  */
void KeyScan_SetStuckThreshold(KSCAN_TypeDef *KeyScan, u32 ThresholdMs)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	u32 Threshold = (ThresholdMs * 40 * 1000) / (KS_CLK_DIV(KeyScan->KS_CLK_DIV) + 1) - 1;
	assert_param(Threshold <= 0x7fffffff);

	KeyScan->KS_STUCK_CFG0 &= (~KS_MASK_STUCK_TIME_THRESHOLD);
	KeyScan->KS_STUCK_CFG0 |= KS_STUCK_TIME_THRESHOLD(Threshold);
}


/**
  * @brief  Set stuckrow detect periodical no pull time.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  PullUpUs: stuckrow detect pullup time convert to us.
  *   PullUpUs = (PullUpCnt +1)* clk_131k(us), max value is 15625us.
  *   PullUpCnt must be set to a value in the 0x0-0x7ff range.
  * @param  NoPullUs: stuckrow detect no pull time convert to us.
  *   When NoPullUs is 0, disable stuckrow detect function.
  *   When NoPullUs is not 0, NoPullUs = (NoPullCnt +1)* clk_131k(us), max value is 31259us.
  *   NoPullUs can be set to a value in the 0x0-0xfff range.
  * @retval None
  */
void KeyScan_StuckPeriodicalPull(KSCAN_TypeDef *KeyScan, u32 PullUpUs, u32 NoPullUs)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	u32 PullUpCnt = PullUpUs * 131 / 1000 - 1;
	u32 NoPullCnt = NoPullUs * 131 / 1000 - 1;
	assert_param(PullUpCnt <= 0x7ff);
	assert_param(NoPullCnt <= 0xfff);

	if (NoPullUs != 0) {
		/* Enable the KeyScan stuckrow detect function */
		KeyScan->KS_STUCK_CFG2 |= KS_BIT_STUCKROW_DETECT_ENABLE;

		KeyScan->KS_STUCK_CFG2 &= ~(KS_MASK_STUCKROW_DETECT_TIME | KS_MASK_STUCKROW_DETECT_TIME_INTERVAL);
		KeyScan->KS_STUCK_CFG2 |= (KS_STUCKROW_DETECT_TIME(PullUpCnt) | KS_STUCKROW_DETECT_TIME_INTERVAL(NoPullCnt));
	} else {
		/* Disable the KeyScan stuckrow detect function */
		KeyScan->KS_STUCK_CFG2 &= (~KS_BIT_STUCKROW_DETECT_ENABLE);
	}
}

/**
  * @brief  Get stuckrow default status.
  * @param  KeyScan: selected KeyScan peripheral.
  * @retval row0 ~ row7 default status.
  *   one bit one row, bit 0 means row0, bit 7 means row7.
  *   1: row stucked
  *   0: row unstucked
  */
u32 KeyScan_GetStuckRow(KSCAN_TypeDef *KeyScan)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	return KS_ROW_STATUS(KeyScan->KS_STUCK_KEY_ROW_STATUS1);
}

/**
  * @brief  Set stuckrow default status.
  * @param  KeyScan: selected KeyScan peripheral.
  * @param  Status: row default status.
  *   one bit one row, bit 0 means row0, bit 7 means row7.
  *   1: row stucked
  *   0: row unstucked
  * @retval None
  */
void KeyScan_SetStuckRow(KSCAN_TypeDef *KeyScan, u32 Status)
{
	/* Check the parameters */
	assert_param(IS_KEYSCAN_ALL_PERIPH(KeyScan));

	KeyScan->KS_STUCK_CFG1 = KS_ROW_DEFAULT_STATUS(~Status);
}

/**
  * @}
  */

/** @} */

/** @} */