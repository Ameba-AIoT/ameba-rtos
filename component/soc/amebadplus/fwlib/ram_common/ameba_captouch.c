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

/** @defgroup CAPTOUCH_Exported_Normal_Functions CAPTOUCH Normal Functions
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
		value = PLL_BASE->PLL_LPAD0;
		value &= ~PLL_MASK_VREF_SEL;
		if (vref_sel == 0x3) {
			value |= PLL_VREF_SEL(0x3); // 011:0.875V
		} else {
			value |= PLL_VREF_SEL(0x2); // 010:0.85V
		}
		PLL_BASE->PLL_LPAD0 = value;

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
  *            @arg CT_BIT_CTC_BRK_BY_ADC_INTR_EN: Captouch breaked by adc interrupt
  *            @arg CT_BIT_AFIFO_OVERLVL_INTR_EN: Raw code FIFO over level interrupt
  *            @arg CT_BIT_OVER_N_NOISE_TH_INTR_EN: CapTouch negetive noise overflow interrupt
  *            @arg CT_BIT_AFIFO_OVERFLOW_INTR_EN: CapTouch FIFO overflow interrupt
  *            @arg CT_BIT_OVER_P_NOISE_TH_INTR_EN:CapTouch positive noise overflow interrupt
  *            @arg CT_CHX_PRESS_INT(x): CapTouch channel(x) press interrupt, where x can be 0~3
  *            @arg CT_CHX_RELEASE_INT(x): CapTouch channel(x) release interrupt, where x can be 0~3
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
  *            @arg CT_BIT_CTC_BRK_BY_ADC_CLR: Captouch breaked by adc interrupt
  *            @arg CT_BIT_AFIFO_OVERLVL_CLR: Raw code FIFO over level interrupt
  *            @arg CT_BIT_OVER_N_NOISE_TH_CLR: CapTouch negetive noise overflow interrupt
  *            @arg CT_BIT_AFIFO_OVERFLOW_CLR: CapTouch FIFO overflow interrupt
  *            @arg CT_BIT_OVER_P_NOISE_TH_CLR:CapTouch positive noise overflow interrupt
  *            @arg CT_CHX_PRESS_INT(x): CapTouch channel(x) press interrupt, where x can be 0~3
  *            @arg CT_CHX_RELEASE_INT(x): CapTouch channel(x) release interrupt, where x can be 0~3
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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @brief Set CapTouch clock source and division.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param ClkSrc: clock source, which can be one of the following values:
  *        @arg CTC_CLK_OSC: lp adc 4m clk selection from osc4m.
  *        @arg CTC_CLK_XTAL: lp adc 4m clk selection from xtal.
  * @param ClkDiv: clock division, which can be one of the following values:
  *        @arg CTC_CLK_DIV4: CapTouch clock is divided by 4.
  *        @arg CTC_CLK_DIV8: CapTouch clock is divided by 8.
  *        @arg CTC_CLK_DIV16: CapTouch clock is divided by 16.
  *        @arg CTC_CLK_DIV24: CapTouch clock is divided by 24.
  *        @arg CTC_CLK_DIV32: CapTouch clock is divided by 32.
  *        @arg CTC_CLK_DIV64: CapTouch clock is divided by 64.
  *        @arg CTC_CLK_DIV128: CapTouch clock is divided by 128.
  * @retval None
  */
void CapTouch_SetClkPara(CAPTOUCH_TypeDef *CapTouch, u8 ClkSrc, u8 ClkDiv)
{
	u32 TempVal;

	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CTC_CLK_SRC(ClkSrc));
	assert_param(IS_CTC_CLK_DIV(ClkDiv));

	TempVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);
	if (ClkSrc == CTC_CLK_OSC) {
		TempVal &= ~LSYS_BIT_CKSL_ADC4M;
	} else {
		TempVal |= LSYS_BIT_CKSL_ADC4M;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, TempVal);

	TempVal = CAPTOUCH_DEV->CT_CTC_SAMPLE_CTRL;
	TempVal &= ~CT_MASK_CTC_CLK_DIV;
	TempVal |= CT_CTC_CLK_DIV(ClkDiv);
	CAPTOUCH_DEV->CT_CTC_SAMPLE_CTRL = TempVal;
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
	TempVal &= ~CT_MASK_SLEEP_TIME_CNT;
	TempVal |= Interval;
	CapTouch->CT_SCAN_PERIOD = TempVal;
	CapTouch_Cmd(CapTouch, ENABLE);
}

/**
  * @brief  Set Mbias current for specified channel.
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  Channel: specified channel index, which can be 0~3
  * @param  Mbias: Mbias value, relate current = 0.25*Mbias.
  * @retval None
  */
void CapTouch_SetChMbias(CAPTOUCH_TypeDef *CapTouch, u8 Channel, u8 Mbias)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_CT_CHANNEL(Channel));

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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @param Channel: specified channel index, which can be 0~3.
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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @param  Channel: specified channel index, which can be 0~3
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
  * @}
  */

/** @defgroup CAPTOUCH_Exported_Debug_Functions CAPTOUCH Debug Functions
  * @{
  */


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
  * @param  Channel: specified channel index, which can be 0~3
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
			RTK_LOGI(TAG, "sample timeout \n");
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
	RTK_LOGI(TAG, "\n%p: %08lX (CT_CTC_CTRL)\n", &(CapTouch->CT_CTC_CTRL), CapTouch->CT_CTC_CTRL);
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

	RTK_LOGI(TAG, "%p: %08lX (CT_ANA_ADC_REG0X_LPAD)\n", &(CapTouch->CT_CTC_SAMPLE_CTRL), CapTouch->CT_CTC_SAMPLE_CTRL);
}

/**
  * @brief Dump all registers
  * @param  CapTouch: which should be CAPTOUCH_DEV.
  * @param  ch: specified channel index, which can be 0~3
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

	RTK_LOGI(TAG, "\nCH[%lu] ETC: ChDiffThres:%08lX (%lu) \n", ch, DiffThres, DiffThres);
	RTK_LOGI(TAG, "CH[%lu] ETC: Baseline:%08lX (%lu) \n", ch, Baseline, Baseline);
	RTK_LOGI(TAG, "CH[%lu] ETC: RawAveData:%08lX (%lu) Diff:%lu \n", ch, AveData, AveData, AveData - Baseline);
	RTK_LOGI(TAG, "CH[%lu] ETC: NoiseThres P:%08lX (%lu) \n", ch, NoiseThresP, NoiseThresP);
	RTK_LOGI(TAG, "CH[%lu] ETC: NoiseThres N:%08lX (%lu) \n", ch, NoiseThresN, NoiseThresN);
	RTK_LOGI(TAG, "CH[%lu] ETC: AbsoThres:%08lX (%lu) \n", ch, AbsoThres, AbsoThres);
}

/**
  * @brief Enable or disable ctc filter function.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param NewState: new state of the filter function.
  *  This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_FilterCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 |= CT_BIT_FLT_EN;
	} else {
		/* Disable the CapTouch filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 &= ~CT_BIT_FLT_EN;
	}
}

/**
  * @brief Enable or disable ctc IIR filter function.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param NewState: new state of the IIR filter function.
  *  This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void CapTouch_FilterIIRCmd(CAPTOUCH_TypeDef *CapTouch, u8 NewState)
{
	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	if (NewState != DISABLE) {
		/* Enable the CapTouch IIR filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 |= CT_BIT_IIR_EN;
	} else {
		/* Disable the CapTouch IIR filter function */
		CapTouch->CT_CTC_FILTER_CTRL0 &= ~CT_BIT_IIR_EN;
	}
}

/**
  * @brief Set decimation factor of decimation filter.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param MDec: specifies decimation of sample point.
  * @retval None
  */
void CapTouch_SetMDec(CAPTOUCH_TypeDef *CapTouch, u32 MDec)
{
	u32 TempVal;

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));
	assert_param(IS_DEC_M(MDec));

	TempVal = CapTouch->CT_CTC_FILTER_CTRL0;
	TempVal &= ~CT_MASK_MDEC;
	TempVal |= CT_MDEC(MDec);
	CapTouch->CT_CTC_FILTER_CTRL0 = TempVal;
}

/**
  * @brief Get decimation of sample point with different AC frequency.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param ACFreq: AC frequency, which should be 50 or 60.
  * @return Decimation of sample point.
  */
u32 CapTouch_GetMDec(CAPTOUCH_TypeDef *CapTouch, u32 ACFreq)
{
	u32 clk_div, sample_cnt;
	u8 div_arr[] = {4, 8, 16, 24, 32, 64, 128};

	/* Check the parameters */
	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	clk_div = CT_GET_CTC_CLK_DIV(CapTouch -> CT_CTC_SAMPLE_CTRL);
	sample_cnt = CT_GET_SAMPLE_AVE_CTRL(CapTouch -> CT_SCAN_PERIOD);

	return 4000000 / div_arr[clk_div] / (ACFreq * BIT(sample_cnt + 2)) - 1;
}

/**
  * @brief Set coefficient of median filter.
  * @param CapTouch: which should be CAPTOUCH_DEV.
  * @param MedCoef: specifies coefficient of median filter.
  * @retval None
  */
void CapTouch_SetMedCoef(CAPTOUCH_TypeDef *CapTouch, u32 MedCoef)
{
	u32 TempVal;

	assert_param(IS_CAPTOUCH_ALL_PERIPH(CapTouch));

	TempVal = CapTouch->CT_CTC_FILTER_CTRL0;
	TempVal &= ~CT_MASK_MED_COEF;
	TempVal |= CT_MED_COEF(MedCoef);
	CapTouch->CT_CTC_FILTER_CTRL0 = TempVal;
}
/**
  * @}
  */

/**
  * @}
  */


/** @} */

/** @} */