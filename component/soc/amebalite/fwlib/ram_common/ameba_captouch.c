/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *TAG = "CAPTOUCH";
extern BOOL vref_init_done;

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup CAPTOUCH
* @brief CAPTOUCH driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup CAPTOUCH_Exported_Functions CAPTOUCH Exported Functions
  * @{
  */

/**
  * @brief  Fills each CapTouch_InitStruct member with its default value.
  * @param  CapTouch_InitStruct: pointer to an CapTouch_InitTypeDef structure which will be initialized.
  * @retval None
  */
void CapTouch_StructInit(CapTouch_InitTypeDef *CapTouch_InitStruct)
{
	u8 i;
	/* Load HAL initial data structure default value */
	CapTouch_InitStruct->CT_DebounceEn = 1;
	CapTouch_InitStruct->CT_SampleCnt = 6;
	CapTouch_InitStruct->CT_ScanInterval = 60;
	CapTouch_InitStruct->CT_ETCStep = 1;
	CapTouch_InitStruct->CT_ETCFactor = 4;
	CapTouch_InitStruct->CT_ETCScanInterval = 3;

	for (i = 0; i < CT_CHANNEL_NUM; i++) {
		CapTouch_InitStruct->CT_Channel[i].CT_CHEnable = DISABLE;
		CapTouch_InitStruct->CT_Channel[i].CT_DiffThrehold = 0xf0;
		CapTouch_InitStruct->CT_Channel[i].CT_MbiasCurrent = 0x11; 		//6uA
		CapTouch_InitStruct->CT_Channel[i].CT_ETCNNoiseThr = 100;
		CapTouch_InitStruct->CT_Channel[i].CT_ETCPNoiseThr = 100;
	}
}


/**
  * @brief  Initializes the CapTouch peripheral according to the specified
  *			parameters in the CapTouch_InitStruct.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  CapTouch_InitStruct: pointer to a CapTouch_InitTypeDef structure that contains
  * 		the configuration information for the specified CapTouch peripheral.
  * @retval None
  */
void CapTouch_Init(CAPTOUCH_TypeDef *CapTouch, CapTouch_InitTypeDef *CapTouch_InitStruct)
{
	u32 i;
	u32 value;
	u8 vref_sel = 0x7;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(CapTouch_InitStruct->CT_DebounceEn <= 1);
	assert_param(CapTouch_InitStruct->CT_SampleCnt <= 7);
	assert_param(CapTouch_InitStruct->CT_ScanInterval <= 0xFFF);
	assert_param(CapTouch_InitStruct->CT_ETCStep <= 0xF);
	assert_param(CapTouch_InitStruct->CT_ETCFactor <= 0xF);
	assert_param(CapTouch_InitStruct->CT_ETCScanInterval <= 0x7F);

	for (i = 0; i < CT_CHANNEL_NUM ; i++) {
		assert_param(CapTouch_InitStruct->CT_Channel[i].CT_DiffThrehold  <= 0xFFF);
		assert_param(CapTouch_InitStruct->CT_Channel[i].CT_MbiasCurrent <= 0x3F);
		assert_param(CapTouch_InitStruct->CT_Channel[i].CT_ETCNNoiseThr <= 0xFFF);
		assert_param(CapTouch_InitStruct->CT_Channel[i].CT_ETCPNoiseThr <= 0xFFF);
	}

	/* Disable CTC, clear pending interrupt*/
	CapTouch_Cmd(CapTouch, DISABLE);

	/* Calibrate Vref according to EFuse */
	if (!vref_init_done) {
		/* step1: read EFuse */
		OTP_Read8(VREF_SEL_ADDR, &vref_sel);
		/* [2:0]: Vref Selection */
		vref_sel &= 0x7;

		/* step2: update vref sel para */
		value = CapTouch->CT_ANA_ADC_REG0X_LPAD;
		value &= ~(0x7 << 8);
		if (vref_sel == 0x3) {
			value |= (0x3 << 8); // 011:0.85V
		} else {
			value |= (0x2 << 8); // 010:0.80V
		}
		CapTouch->CT_ANA_ADC_REG0X_LPAD = value;

		vref_init_done = _TRUE;
	}

	CapTouch->CT_INTERRUPT_ENABLE = 0;
	CapTouch->CT_INTERRUPT_ALL_CLR = CT_BIT_INTR_ALL_CLR;

	/* Set control register*/
	CapTouch->CT_CTC_CTRL = CT_BIT_BASELINE_INI;
	if (CapTouch_InitStruct->CT_DebounceEn == 1) {
		CapTouch->CT_CTC_CTRL |= CT_BIT_DEBOUNCE_EN;
	} else {
		CapTouch->CT_CTC_CTRL &= ~CT_BIT_DEBOUNCE_EN;
	}

	CapTouch->CT_SCAN_PERIOD = CT_SAMPLE_AVE_CTRL(CapTouch_InitStruct->CT_SampleCnt) | \
							   CapTouch_InitStruct->CT_ScanInterval;

	CapTouch->CT_ETC_CTRL = CT_BASELINE_UPD_STEP(CapTouch_InitStruct->CT_ETCStep) | \
							CT_BASELINE_WT_FACTOR(CapTouch_InitStruct->CT_ETCFactor) | \
							CT_ETC_SCAN_INTERVAL(CapTouch_InitStruct->CT_ETCScanInterval) | \
							CT_BIT_ETC_FUNC_CTRL;

	CapTouch->CT_DEBUG_MODE_CTRL = CT_BIT_CH_SWITCH_CTRL;

	/* Configure each channel */
	for (i = 0; i < CT_CHANNEL_NUM ; i++) {
		if (CapTouch_InitStruct->CT_Channel[i].CT_CHEnable) {
			CapTouch->CT_CH[i].CT_CHx_CTRL = CT_CHx_D_TOUCH_TH(CapTouch_InitStruct->CT_Channel[i].CT_DiffThrehold) | CT_BIT_CHx_EN;
			CapTouch->CT_CH[i].CT_CHx_MBIAS_ATH = CT_CHx_MBIAS(CapTouch_InitStruct->CT_Channel[i].CT_MbiasCurrent);
			CapTouch->CT_CH[i].CT_CHx_NOISE_TH = CT_CHx_N_ENT(CapTouch_InitStruct->CT_Channel[i].CT_ETCNNoiseThr) | \
												 CT_CHx_P_ENT(CapTouch_InitStruct->CT_Channel[i].CT_ETCPNoiseThr);
		}
	}
}


/**
  * @brief  Enables or disables the specified CapTouch peripheral.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  NewState: new state of the CapTouch peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_Cmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch peripheral and baseline */
		CapTouch->CT_CTC_CTRL |= CT_BIT_ENABLE | CT_BIT_BASELINE_INI;
	} else {
		/* Disable the CapTouch peripheral */
		CapTouch->CT_CTC_CTRL &= (~CT_BIT_ENABLE);
	}
}


/**
  * @brief  Enables or disables the specified CapTouch interrupts.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  CapTouch_IT: specifies the CapTouch interrupt to be enabled or masked.
  *          This parameter can be one or combinations of the following values:
  *            @arg CT_BIT_GUARD_RELEASE_INTR_EN: CapTouch guard sensor release interrupt
  *            @arg CT_BIT_GUARD_PRESS_INTR_EN: CapTouch guard sensor press interrupt
  *            @arg CT_BIT_OVER_N_NOISE_TH_INTR_EN: CapTouch negetive noise overflow interrupt
  *            @arg CT_BIT_AFIFO_OVERFLOW_INTR_EN: CapTouch FIFO overflow interrupt
  *            @arg CT_BIT_OVER_P_NOISE_TH_INTR_EN:CapTouch positive noise overflow interrupt
  *            @arg CT_CHX_PRESS_INT(x): CapTouch channel(x) press interrupt, where x can be 0~7
  *            @arg CT_CHX_RELEASE_INT(x): CapTouch channel(x) release interrupt, where x can be 0~7
  * @param  NewState: new state of the specified CapTouch interrupts mask.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_INTConfig(CAPTOUCH_TypeDef *CapTouch, uint32_t CapTouch_IT, u8 newState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_INT_EN(CapTouch_IT));

	if (newState == ENABLE) {
		/* Enable CapTouch interrupts */
		CapTouch->CT_INTERRUPT_ENABLE |= CapTouch_IT;
	} else {
		/* Disable CapTouch interrupts */
		CapTouch->CT_INTERRUPT_ENABLE &= (~CapTouch_IT);
	}
}


/**
  * @brief  Clears the specified CapTouch interrupt pending bit.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  CapTouch_IT: specifies the CapTouch interrupt to be cleared.
  *          This parameter can be one or combinations of the following values:
  *          This parameter can be one or combinations of the following values:
  *            @arg CT_BIT_GUARD_RELEASE_CLR: CapTouch guard sensor release interrupt
  *            @arg CT_BIT_GUARD_PRESS_CLR: CapTouch guard sensor press interrupt
  *            @arg CT_BIT_OVER_N_NOISE_TH_CLR: CapTouch negetive noise overflow interrupt
  *            @arg CT_BIT_AFIFO_OVERFLOW_CLR: CapTouch FIFO overflow interrupt
  *            @arg CT_BIT_OVER_P_NOISE_TH_CLR:CapTouch positive noise overflow interrupt
  *            @arg CT_CHX_PRESS_INT(x): CapTouch channel(x) press interrupt, where x can be 0~7
  *            @arg CT_CHX_RELEASE_INT(x): CapTouch channel(x) release interrupt, where x can be 0~7
  * @retval None
  */
void CapTouch_INTClearPendingBit(CAPTOUCH_TypeDef *CapTouch, u32 CapTouch_IT)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_INT_CLR(CapTouch_IT));

	CapTouch->CT_INTERRUPT_STATUS_CLR |= CapTouch_IT;
}


/**
  * @brief  Get CapTouch Raw Interrupt Status.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @retval raw interrupt status
  */
u32 CapTouch_GetRawISR(CAPTOUCH_TypeDef *CapTouch)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	return CapTouch->CT_RAW_INTERRUPT_STATUS;
}


/**
  * @brief  Get CapTouch interrupt status.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @retval interrupt status
  */
u32 CapTouch_GetISR(CAPTOUCH_TypeDef *CapTouch)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	return CapTouch->CT_INTERRUPT_STATUS;
}

/**
  * @brief  Get CapTouch channel enable status.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @retval channel enable status.
  */
u32 CapTouch_GetChStatus(CAPTOUCH_TypeDef *CapTouch, u32 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	if (CapTouch->CT_CH[Channel].CT_CHx_CTRL & CT_BIT_CHx_EN) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/**
  * @brief  Enable or disable specified channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @param  NewState: new state of the specified channel.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_ChCmd(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	if (NewState != DISABLE) {
		/* Enable the CapTouch crossponding channel */
		CapTouch->CT_CH[Channel].CT_CHx_CTRL |= CT_BIT_CHx_EN;
	} else {
		/* Disable the CapTouch peripheral */
		CapTouch->CT_CH[Channel].CT_CHx_CTRL &= ~CT_BIT_CHx_EN;
	}
}

/**
  * @brief Set CapTouch clock source and divider.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param ClkSrc: clock source, which can be one of the following values:
  *        @arg CTC_CLK_OSC: CTC core clock selection from OSC131k.
  *        @arg CTC_CLK_XTAL: CTC core clock selection from xtal2m.
  * @param ClkDiv: xtal 2m divider for captouch core, which can be 2~16.
  * @retval None
  * @note CTC core clock can be divided only when it is from xtal2m.
  */
void CapTouch_SetClkPara(CAPTOUCH_TypeDef *CapTouch, u8 ClkSrc, u8 ClkDiv)
{
	u32 TempVal0, TempVal1;

	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CTC_CLK_SRC(ClkSrc));
	assert_param(IS_CTC_CLK_DIV(ClkDiv));

	TempVal0 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);
	TempVal1 = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1);

	CapTouch_Cmd(CAPTOUCH_DEV, DISABLE);

	if (ClkSrc == CTC_CLK_OSC) {
		TempVal0 &= ~LSYS_BIT_CKSL_CTC_CORE;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, TempVal0);
	} else  {
		TempVal1 &= ~LSYS_MASK_CKD_XTAL2M_CTC;
		TempVal1 |= LSYS_CKD_XTAL2M_CTC(ClkDiv - 1);
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1, TempVal1);

		TempVal0 |= LSYS_BIT_CKSL_CTC_CORE;
		HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, TempVal0);
	}

	CapTouch_Cmd(CAPTOUCH_DEV, ENABLE);
}

/**
  * @brief  Set CapTouch Scan interval.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param Interval: scan interval in units of ms
  * @retval None
  */
void CapTouch_SetScanInterval(CAPTOUCH_TypeDef *CapTouch, u32 Interval)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(Interval <= 4095);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_SCAN_PERIOD;
	TempVal &= ~CT_MASK_SCAN_INTERVAL;
	TempVal |= Interval;
	CapTouch->CT_SCAN_PERIOD = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
  * @brief  Set Mbias current for specified channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel.
  * @param  Mbias: Mbias value, relate current = 0.25*Mbias.
  * @retval None
  */
void CapTouch_SetChMbias(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 Mbias)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));
	assert_param(Mbias < 64);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_CH[Channel].CT_CHx_MBIAS_ATH;
	TempVal &= ~CT_MASK_CHx_MBIAS;
	TempVal |= CT_CHx_MBIAS(Mbias);
	CapTouch->CT_CH[Channel].CT_CHx_MBIAS_ATH = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
  * @brief  Set relative touch threshold for related channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @param  Threshold: Related Threshold value.
  * @retval None
  */
void CapTouch_SetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u32 Threshold)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));
	assert_param(Threshold < 4096);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_CH[Channel].CT_CHx_CTRL;
	TempVal &= ~CT_MASK_CHx_D_TOUCH_TH;
	TempVal |= CT_CHx_D_TOUCH_TH(Threshold);
	CapTouch->CT_CH[Channel].CT_CHx_CTRL = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
  * @brief  Set N-noise threshold for related channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @param  Threshold: Related N-noise Threshold value.
  * @retval None
  */
void CapTouch_SetNNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u16 Threshold)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));
	assert_param(Threshold < 4096);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH;
	TempVal &= ~CT_MASK_CHx_N_ENT;
	TempVal |= CT_CHx_N_ENT(Threshold);
	CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
  * @brief  Set P-noise threshold for related channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @param  Threshold: Related P-noise Threshold value.
  * @retval None
  */
void CapTouch_SetPNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u16 Threshold)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));
	assert_param(Threshold < 4096);

	CapTouch_Cmd(CapTouch, DISABLE);
	TempVal = CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH;
	TempVal &= ~CT_MASK_CHx_P_ENT;
	TempVal |= CT_CHx_P_ENT(Threshold);
	CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
  * @brief Set CapTouch full level for AFIFO.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param Level: AFIFO full level, which can be 0~63.
  * @note When raw code FIFO is over level, AFIFO_OVERLVL_INTR will occur.
  * @return None
  */
void CapTouch_SetFifoLevel(CAPTOUCH_TypeDef *CapTouch, u8 Level)
{
	u32 TempVal;
	BOOL ctc_is_en = _FALSE;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(Level < 64);

	if (CapTouch->CT_CTC_CTRL & CT_BIT_ENABLE) {
		CapTouch_Cmd(CapTouch, DISABLE);
		ctc_is_en = _TRUE;
	}

	TempVal = CapTouch->CT_RAW_CODE_FIFO_STATUS;
	TempVal &= ~CT_MASK_AFIFO_FULL_LVL;
	TempVal |= CT_AFIFO_FULL_LVL(Level);
	CapTouch->CT_RAW_CODE_FIFO_STATUS = TempVal;

	if (ctc_is_en) {
		CapTouch_Cmd(CapTouch, ENABLE);
	}
}

/**
  * @brief Get Mbias current value for specified channel.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param Channel: specified channel index, which can be 0~8.
  * @note Channel 8 is guard sensor channel.
  * @return Mbias data.
  */
u32 CapTouch_GetChMbias(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_MBIAS(CapTouch->CT_CH[Channel].CT_CHx_MBIAS_ATH);
}

/**
  * @brief  Get relative threshold of touch judgement for specified channel.
   * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @retval Difference threshold of specified channel.
  */
u32 CapTouch_GetChDiffThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_D_TOUCH_TH(CapTouch->CT_CH[Channel].CT_CHx_CTRL);
}

/**
  * @brief  Get Absolute  threshold of touch judgement for specified channel.
   * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @retval Difference threshold of specified channel.
  */
u32 CapTouch_GetChAbsThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_A_TOUCH_TH(CapTouch->CT_CH[Channel].CT_CHx_MBIAS_ATH);
}

/**
  * @brief  Get positive or negative noise threshold for specified channel.
   * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @param  type: can be P_NOISE_THRES or N_NOISE_THRES
  * @retval  Noise threshold of specified channel.
  */
u32 CapTouch_GetNoiseThres(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 type)
{
	u32 value;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	if (type) {
		value = CT_GET_CHx_N_ENT(CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH);
	} else {
		value = CT_GET_CHx_P_ENT(CapTouch->CT_CH[Channel].CT_CHx_NOISE_TH);
	}

	return value;
}

/**
  * @brief  Read Baseline data from specified channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @retval Baseline data
  */
u32 CapTouch_GetChBaseline(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_BASELINE(CapTouch->CT_CH[Channel].CT_CHx_CTRL);
}


/**
  * @brief  Read average data from specified channel.
   * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @retval Average data
  */
u32 CapTouch_GetChAveData(CAPTOUCH_TypeDef *CapTouch, u8 Channel)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

	return CT_GET_CHx_DATA_AVE(CapTouch->CT_CH[Channel].CT_CHx_DATA_INF);
}

/**
  * @brief  Enable or disable Debug mode.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  NewState: new state of the Debug mode.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_DbgCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch debug mode */
		CapTouch->CT_DEBUG_MODE_CTRL |= CT_BIT_DEBUG_EN;
	} else {
		/* Disable the CapTouch peripheral */
		CapTouch->CT_DEBUG_MODE_CTRL |= CT_BIT_CH_SWITCH_CTRL;
		CapTouch->CT_DEBUG_MODE_CTRL &= (~CT_BIT_DEBUG_EN);
	}
}


/**
  * @brief select debug channel
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~8, channel 8 is guard sensor
  * @param  NewState: can be one of the following values:
  *		@arg ENABLE: manual channel switch.
  *		@arg DISABLE: auto channel switch
  * @retval none
  * @note Manual channel switch only works when debug mode is enabled.
  */
void CapTouch_DbgChannel(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 NewState)
{
	u32 data;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	data = CapTouch->CT_DEBUG_MODE_CTRL;

	if (NewState != DISABLE) {
		assert_param(IS_CT_CHANNEL(Channel));

		data &= ~(CT_MASK_CH_CTRL | CT_BIT_CH_SWITCH_CTRL);
		data |= CT_CH_CTRL(Channel);
	} else {
		data |= CT_BIT_CH_SWITCH_CTRL;
	}

	CapTouch->CT_DEBUG_MODE_CTRL = data;
}

/**
  * @brief Enable or disable CapTouch continous sample.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param NewState: whether to enable continous sample or not.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  * @note Continous sample only works when debug mode and manual switch are both enabled.
  */
void CapTouch_DbgContCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable continous sample */
		CapTouch->CT_DEBUG_MODE_CTRL |= CT_BIT_CONTINUE_SAMPLE_EN;
	} else {
		/* Disable continous sample */
		CapTouch->CT_DEBUG_MODE_CTRL &= ~CT_BIT_CONTINUE_SAMPLE_EN;
	}
}

/**
  * @brief Get Raw data in debug mode
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @retval Raw data
  */
u32 CapTouch_DbgRawData(CAPTOUCH_TypeDef *CapTouch)
{
	u32 loop = 0;
	u32 data;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	do {
		data = CapTouch->CT_RAW_CODE_FIFO_READ;

		if (loop++ > 200000) {
			RTK_LOGW(TAG, "sample timeout \n");
			return  0xEAEA;
		}

	} while ((data & CT_BIT_AFIFO_RD_DATA_VLD) == 0);

	return CTC_ID_AND_DATA(data);

}

/**
  * @brief Dump all registers
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @retval NA
  */
void CapTouch_DbgDumpReg(CAPTOUCH_TypeDef *CapTouch)
{
	RTK_LOGI(TAG, "%p: %08lX (CT_CTC_CTRL)\n", &(CapTouch->CT_CTC_CTRL), CapTouch->CT_CTC_CTRL);
	RTK_LOGI(TAG, "%p: %08lX (CT_SCAN_PERIOD)\n", &(CapTouch->CT_SCAN_PERIOD), CapTouch->CT_SCAN_PERIOD);
	RTK_LOGI(TAG, "%p: %08lX (CT_ETC_CTRL)\n", &(CapTouch->CT_ETC_CTRL), CapTouch->CT_ETC_CTRL);
	RTK_LOGI(TAG, "%p: %08lX (CT_SNR_INF)\n", &(CapTouch->CT_SNR_INF), CapTouch->CT_SNR_INF);
	RTK_LOGI(TAG, "%p: %08lX (CT_DEBUG_MODE_CTRL)\n", &(CapTouch->CT_DEBUG_MODE_CTRL), CapTouch->CT_DEBUG_MODE_CTRL);
	RTK_LOGI(TAG, "%p: %08lX (CT_RAW_CODE_FIFO_STATUS)\n", &(CapTouch->CT_RAW_CODE_FIFO_STATUS), CapTouch->CT_RAW_CODE_FIFO_STATUS);
	RTK_LOGI(TAG, "%p: %08lX (CT_RAW_CODE_FIFO_READ)\n", &(CapTouch->CT_RAW_CODE_FIFO_READ), CapTouch->CT_RAW_CODE_FIFO_READ);
	RTK_LOGI(TAG, "%p: %08lX (CT_INTERRUPT_ENABLE)\n", &(CapTouch->CT_INTERRUPT_ENABLE), CapTouch->CT_INTERRUPT_ENABLE);
	RTK_LOGI(TAG, "%p: %08lX (CT_INTERRUPT_STATUS)\n", &(CapTouch->CT_INTERRUPT_STATUS), CapTouch->CT_INTERRUPT_STATUS);
	RTK_LOGI(TAG, "%p: %08lX (CT_RAW_INTERRUPT_STATUS)\n", &(CapTouch->CT_RAW_INTERRUPT_STATUS), CapTouch->CT_RAW_INTERRUPT_STATUS);
	RTK_LOGI(TAG, "%p: %08lX (CT_INTERRUPT_ALL_CLR)\n", &(CapTouch->CT_INTERRUPT_ALL_CLR), CapTouch->CT_INTERRUPT_ALL_CLR);
	RTK_LOGI(TAG, "%p: %08lX (CT_INTERRUPT_STATUS_CLR)\n", &(CapTouch->CT_INTERRUPT_STATUS_CLR), CapTouch->CT_INTERRUPT_STATUS_CLR);
	RTK_LOGI(TAG, "%p: %08lX (CT_DEBUG_SEL)\n", &(CapTouch->CT_DEBUG_SEL), CapTouch->CT_DEBUG_SEL);
	RTK_LOGI(TAG, "%p: %08lX (CT_DEBUG_PORT)\n", &(CapTouch->CT_DEBUG_PORT), CapTouch->CT_DEBUG_PORT);
	RTK_LOGI(TAG, "%p: %08lX (CT_ECO_USE0)\n", &(CapTouch->CT_ECO_USE0), CapTouch->CT_ECO_USE0);
	RTK_LOGI(TAG, "%p: %08lX (CT_ECO_USE1)\n", &(CapTouch->CT_ECO_USE1), CapTouch->CT_ECO_USE1);
	RTK_LOGI(TAG, "%p: %08lX (CT_CTC_COMP_VERSION)\n", &(CapTouch->CT_CTC_COMP_VERSION), CapTouch->CT_CTC_COMP_VERSION);

	RTK_LOGI(TAG, "%p: %08lX (CT_CH[0].CT_CHx_CTRL)\n", &(CapTouch->CT_CH[0].CT_CHx_CTRL), CapTouch->CT_CH[0].CT_CHx_CTRL);
	RTK_LOGI(TAG, "%p: %08lX (CT_CH[0].CT_CHx_NOISE_TH)\n", &(CapTouch->CT_CH[0].CT_CHx_NOISE_TH), CapTouch->CT_CH[0].CT_CHx_NOISE_TH);
	RTK_LOGI(TAG, "%p: %08lX (CT_CH[0].CT_CHx_MBIAS_ATH)\n", &(CapTouch->CT_CH[0].CT_CHx_MBIAS_ATH), CapTouch->CT_CH[0].CT_CHx_MBIAS_ATH);
	RTK_LOGI(TAG, "%p: %08lX (CT_CH[0].CT_CHx_DATA_INF)\n", &(CapTouch->CT_CH[0].CT_CHx_DATA_INF), CapTouch->CT_CH[0].CT_CHx_DATA_INF);

	RTK_LOGI(TAG, "%p: %08lX (CT_ANA_ADC_REG0X_LPAD)\n", &(CapTouch->CT_ANA_ADC_REG0X_LPAD), CapTouch->CT_ANA_ADC_REG0X_LPAD);
	RTK_LOGI(TAG, "%p: %08lX (CT_ANA_ADC_REG1X_LPAD)\n", &(CapTouch->CT_ANA_ADC_REG1X_LPAD), CapTouch->CT_ANA_ADC_REG1X_LPAD);
	RTK_LOGI(TAG, "%p: %08lX (CT_ANA_ADC_REG0X_LPSD)\n", &(CapTouch->CT_ANA_ADC_REG0X_LPSD), CapTouch->CT_ANA_ADC_REG0X_LPSD);
	RTK_LOGI(TAG, "%p: %08lX (CT_ANA_ADC_TIME_SET)\n", &(CapTouch->CT_ANA_ADC_TIME_SET), CapTouch->CT_ANA_ADC_TIME_SET);
}

/**
  * @brief Dump all registers
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  ch: specified channel index, which can be 0~8
  * @note     Channel 8 is guard sensor channel
  * @retval NA
  */
void CapTouch_DbgDumpETC(CAPTOUCH_TypeDef *CapTouch, u32 ch)
{
	u32 DiffThres = 0;
	u32 Baseline = 0;
	u32 AveData = 0;
	u32 NoiseThresP = 0;
	u32 NoiseThresN = 0;
	u32 AbsoThres = 0;

	DiffThres = CapTouch_GetChDiffThres(CapTouch, ch);
	Baseline = CapTouch_GetChBaseline(CapTouch, ch);
	AveData = CapTouch_GetChAveData(CapTouch, ch);
	NoiseThresP = CapTouch_GetNoiseThres(CapTouch, ch, P_NOISE_THRES);
	NoiseThresN = CapTouch_GetNoiseThres(CapTouch, ch, N_NOISE_THRES);
	AbsoThres = CapTouch_GetChAbsThres(CapTouch, ch);

	RTK_LOGI(TAG, "CH[%lu] ETC: ChDiffThres:%08lX (%lu) \n", ch, DiffThres, DiffThres);
	RTK_LOGI(TAG, "CH[%lu] ETC: Baseline:%08lX (%lu) \n", ch, Baseline, Baseline);
	RTK_LOGI(TAG, "CH[%lu] ETC: RawAveData:%08lX (%lu) Diff:%lu \n", ch, AveData, AveData, AveData - Baseline);
	RTK_LOGI(TAG, "CH[%lu] ETC: NoiseThres P:%08lX (%lu) \n", ch, NoiseThresP, NoiseThresP);
	RTK_LOGI(TAG, "CH[%lu] ETC: NoiseThres N:%08lX (%lu) \n", ch, NoiseThresN, NoiseThresN);
	RTK_LOGI(TAG, "CH[%lu] ETC: AbsoThres:%08lX (%lu) \n", ch, AbsoThres, AbsoThres);
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