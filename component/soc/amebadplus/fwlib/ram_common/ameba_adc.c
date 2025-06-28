/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include <math.h>

ADC_CalParaTypeDef CalParaNorm;
ADC_CalParaTypeDef CalParaVBat;
u8 vref_init_done = FALSE;

/** @addtogroup Ameba_Periph_Driver
 * @{
 */

/** @defgroup ADC
 * @brief ADC driver modules
 * @{
 */

/* Exported functions --------------------------------------------------------*/
/** @defgroup ADC_Exported_Functions ADC Exported Functions
 * @{
 */

/**
 * @brief Initialize the parameters in the ADC_InitStruct with default values.
 * @param ADC_InitStruct Pointer to a ADC_InitTypeDef structure that contains
 * 		the configuration information of the ADC peripheral.
 * @return None
 */
void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct)
{
	u8 i;

	ADC_InitStruct->ADC_OpMode = ADC_SW_TRI_MODE;
	ADC_InitStruct->ADC_ClkDiv = ADC_CLK_DIV_24;
	ADC_InitStruct->ADC_RxThresholdLevel = 0;
	ADC_InitStruct->ADC_CvlistLen = ADC_CH_NUM - 1;
	ADC_InitStruct->ADC_ChanInType = 0;
	ADC_InitStruct->ADC_SpecialCh = 0xFF;

	for (i = 0; i < ADC_InitStruct->ADC_CvlistLen + 1; i++) {
		ADC_InitStruct->ADC_Cvlist[i] = i;
	}
}

/**
 * @brief Initialize ADC according to the specified parameters in ADC_InitStruct.
 * @param ADC_InitStruct Pointer to a ADC_InitTypeDef structure that contains
 * 		the configuration information of the ADC peripheral.
 * @return None
 */
void ADC_Init(ADC_InitTypeDef *ADC_InitStruct)
{
	ADC_TypeDef *adc = ADC;
	u32 value;
	int len, i;
	u8 vref_sel = 0x7;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	assert_param(IS_ADC_MODE(ADC_InitStruct->ADC_OpMode));
	assert_param(IS_ADC_SAMPLE_CLK(ADC_InitStruct->ADC_ClkDiv));
	assert_param(ADC_InitStruct->ADC_CvlistLen < 16);

	/* under trigger mode */
	if (ADC_InitStruct->ADC_OpMode != ADC_AUTO_MODE) {
		for (i = 0; i < ADC_InitStruct->ADC_CvlistLen + 1; i++) {
			assert_param(ADC_InitStruct->ADC_Cvlist[i] != ADC_DUMMY_CYCLE);
		}
	}
	/* Disable ADC, clear pending interrupt, clear FIFO */
	ADC_Cmd(DISABLE);

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

		vref_init_done = TRUE;
	}

	adc->ADC_INTR_CTRL = 0;
	ADC_INTClear();
	ADC_ClearFIFO();

	/* Set clock divider */
	adc->ADC_CLK_DIV = ADC_CLK_DIV(ADC_InitStruct->ADC_ClkDiv);

	/* Set adc configuration*/
	value = adc->ADC_CONF;
	value &= ~(ADC_MASK_CVLIST_LEN | ADC_MASK_OP_MOD);
	value |= ADC_OP_MOD(ADC_InitStruct->ADC_OpMode) | ADC_CVLIST_LEN(ADC_InitStruct->ADC_CvlistLen);
	adc->ADC_CONF = value;

	/* Set channel input type */
	adc->ADC_IN_TYPE = ADC_InitStruct->ADC_ChanInType;

	/* Set channel switch list */
	value = 0;
	len = ((ADC_InitStruct->ADC_CvlistLen + 1) > 8) ? 8 : (ADC_InitStruct->ADC_CvlistLen + 1);
	for (i = 0; i < len; i++) {
		value |= (u32)(ADC_InitStruct->ADC_Cvlist[i] << ADC_SHIFT_CHSW0(i));
	}
	adc->ADC_CHSW_LIST_0 = value;

	value = 0;
	if ((ADC_InitStruct->ADC_CvlistLen + 1) > 8) {
		for (i = 8; i < (ADC_InitStruct->ADC_CvlistLen + 1); i++) {
			value |= (u32)(ADC_InitStruct->ADC_Cvlist[i] << ADC_SHIFT_CHSW1(i));
		}
	}
	adc->ADC_CHSW_LIST_1 = value;

	/* Set particular channel */
	if (ADC_InitStruct->ADC_SpecialCh < ADC_CH_NUM) {
		ADC_INTConfig(ADC_BIT_IT_CHCV_END_EN, ENABLE);
		adc->ADC_IT_CHNO_CON = (u32)ADC_InitStruct->ADC_SpecialCh;
	}

	/* Set FIFO full level */
	adc->ADC_FULL_LVL = (u32)ADC_InitStruct->ADC_RxThresholdLevel;
}

/**
 * @brief Enable or disable the ADC peripheral.
 * @param NewState New state of the ADC peripheral.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void ADC_Cmd(u32 NewState)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	if (NewState != DISABLE) {
		adc->ADC_CONF |= ADC_BIT_ENABLE;
	} else {
		adc->ADC_CONF &= ~ADC_BIT_ENABLE;

		/* Need to clear FIFO, or there will be waste data in FIFO next time ADC is enable. Ameba-D A-cut bug */
		ADC_ClearFIFO();
	}
}

/**
 * @brief Enable or disable ADC interrupt(s).
 * @param ADC_IT ADC interrupt(s) to be configured.
 * 		This parameter can be one or combinations of the following parameters:
 * 		@arg ADC_BIT_IT_COMP_CHx_EN: ADC channel x compare interrupt, where x can be 0-10 corresponding to channelx.
 * 		@arg ADC_BIT_IT_ERR_EN: ADC error state interrupt
 * 		@arg ADC_BIT_IT_DAT_OVW_EN: ADC data overwritten interrupt
 * 		@arg ADC_BIT_IT_FIFO_EMPTY_EN: ADC FIFO empty interrupt
 * 		@arg ADC_BIT_IT_FIFO_OVER_EN: ADC FIFO overflow interrupt
 * 		@arg ADC_BIT_IT_FIFO_FULL_EN: ADC FIFO full interrupt
 * 		@arg ADC_BIT_IT_CHCV_END_EN: ADC particular channel conversion done interrupt
 * 		@arg ADC_BIT_IT_CV_END_EN: ADC conversion end interrupt
 * 		@arg ADC_BIT_IT_CVLIST_END_EN: ADC conversion list end interrupt
 * @param NewState ENABLE or DISABLE.
 * @return None
 */
void ADC_INTConfig(u32 ADC_IT, u32 NewState)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	if (NewState != DISABLE) {
		adc->ADC_INTR_CTRL |= ADC_IT;
	} else {
		adc->ADC_INTR_CTRL &= ~ADC_IT;
	}
}

/**
 * @brief Clear all the ADC interrupt pending bits.
 * @param None
 * @return None
 * @note This function can also be used to clear raw interrupt status.
 */
void ADC_INTClear(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	/* Clear all the IT pending Bits */
	adc->ADC_INTR_STS = ADC_BIT_IT_ALL_STS;
}

/**
 * @brief Clear specified ADC interrupt pending bits.
 * @param ADC_IT Pending bits to be cleared.
 * 		This parameter can be one or combinations of the following values:
 * 		@arg ADC_BIT_IT_COMP_CHx_STS, where x can be 0-10 corresponding to channelx
 * 		@arg ADC_BIT_IT_ERR_STS
 * 		@arg ADC_BIT_IT_DAT_OVW_STS
 * 		@arg ADC_BIT_IT_FIFO_EMPTY_STS
 * 		@arg ADC_BIT_IT_FIFO_OVER_STS
 * 		@arg ADC_BIT_IT_FIFO_FULL_STS
 * 		@arg ADC_BIT_IT_CHCV_END_STS
 * 		@arg ADC_BIT_IT_CV_END_STS
 * 		@arg ADC_BIT_IT_CVLIST_END_STS
 * @return None
 */
void ADC_INTClearPendingBits(u32 ADC_IT)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	adc->ADC_INTR_STS = ADC_IT;
}

/**
 * @brief Get ADC interrupt status.
 * @param None
 * @return Current interrupt status.
 */
u32 ADC_GetISR(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	return adc->ADC_INTR_STS;
}

/**
 * @brief Get ADC raw interrupt status.
 * @param None
 * @return Current raw interrupt status.
 */
u32 ADC_GetRawISR(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	return adc->ADC_INTR_RAW_STS;
}

/**
 * @brief Set list length and channel ID of ADC channel switch list.
 * @param ChanIdBuf Pointer to ADC channel ID buffer, which contains value of @ref ADC_Chn_Selection as following:
 * 		@arg ADC_CH0
 * 		@arg ADC_CH1
 * 		@arg ADC_CH2
 * 		@arg ADC_CH3
 * 		@arg ADC_CH4
 * 		@arg ADC_CH5
 * 		@arg ADC_CH6
 * 		@arg ADC_CH7
 * 		@arg ADC_CH8
 * 		@arg ADC_CH9
 * 		@arg ADC_CH10
 * 		@arg ADC_DUMMY_CYCLE
 * @param ChanLen ADC channel list length, which can be 1 ~ 16.
 * @return None
 */
void ADC_SetChList(u8 *ChanIdBuf, u8 ChanLen)
{
	ADC_TypeDef *adc = ADC;
	u32 value;
	u8 idx;
	u8 *pid = ChanIdBuf;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	assert_param(ChanLen >= 1 && ChanLen <= 16);

	/* Set channel switch list length */
	value = adc->ADC_CONF;
	value &= ~ADC_MASK_CVLIST_LEN;
	value |= ADC_CVLIST_LEN(ChanLen - 1);
	adc->ADC_CONF = value;

	for (idx = 0; idx < ChanLen; idx++) {
		if (idx < 8) {
			value = adc->ADC_CHSW_LIST_0;
			value &= ~(ADC_MASK_CHSW_0 << ADC_SHIFT_CHSW0(idx));
			value |= *pid++ << ADC_SHIFT_CHSW0(idx);
			adc->ADC_CHSW_LIST_0 = value;
		} else {
			value = adc->ADC_CHSW_LIST_1;
			value &= ~(ADC_MASK_CHSW_8 << ADC_SHIFT_CHSW1(idx));
			value |= *pid++ << ADC_SHIFT_CHSW1(idx);
			adc->ADC_CHSW_LIST_1 = value;
		}
	}
}

/**
 * @brief Get the number of valid entries in ADC receive FIFO.
 * @param None.
 * @return The number of valid entries in receive FIFO.
 */
u32 ADC_GetRxCount(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	return ADC_GET_FLR(adc->ADC_FLR);
}

/**
 * @brief Get the last ADC used channel.
 * @param None.
 * @return The last ADC used channel index.
 */
u32 ADC_GetLastChan(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	return ADC_GET_LAST_CH(adc->ADC_LAST_CH);
}

/**
 * @brief Get comparison result of ADC channel.
 * @param ADC_Channel ADC channel index.
 * @return The comparison result of specified ADC channel.
 */
u32 ADC_GetCompStatus(u8 ADC_Channel)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	u32 value = (adc->ADC_COMP_STS & ADC_MASK_COMP_STS_CH(ADC_Channel)) \
				>> ADC_SHIFT_COMP_STS_CH(ADC_Channel);

	return value;
}

/**
 * @brief Set ADC comparison detection mode.
 * @param det_mode This parameter can be one of the following values:
 * 		@arg ADC_COMP_LEVEL_DETECT: Level detection mode.
 * 		@arg ADC_COMP_EDGE_DETECT: Edge detection mode.
 * @return None
 */
void ADC_SetCompMode(u8 det_mode)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	if (ADC_COMP_LEVEL_DETECT == det_mode) {
		adc->ADC_INTR_CTRL &= ~ADC_BIT_IT_COMP_DET_MOD;
	} else {
		adc->ADC_INTR_CTRL |= ADC_BIT_IT_COMP_DET_MOD;
	}
}

/**
 * @brief Set ADC channel threshold and criteria for comparison.
 * @param ADC_channel This parameter can be a value of @ref ADC_Chn_Selection.
 * @param CompThresH Higher threshold of channel for ADC automatic comparison.
 * @param CompThresL lower threshold of channel for ADC automatic comparison.
 * @param CompCtrl This parameter can be a value of @ref ADC_Compare_Control_Definitions as following:
 * 		@arg ADC_COMP_SMALLER_THAN_THL: less than the lower threshold
 * 		@arg ADC_COMP_GREATER_THAN_THH: greater than the higher threshoLd
 * 		@arg ADC_COMP_WITHIN_THL_AND_THH: between the lower and higher threshoLd
 * 		@arg ADC_COMP_OUTSIDE_THL_AND_THH: out the range of the higher and lower threshoLd
 * @return None
 */
void ADC_SetComp(u8 ADC_channel, u16 CompThresH, u16 CompThresL, u8 CompCtrl)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	u32 value;

	assert_param(IS_ADC_CHN_SEL(ADC_channel));
	assert_param(IS_ADC_COMP_CRITERIA(CompCtrl));

	/* Set ADC channel threshold for comparison */
	adc->ADC_COMP_TH_CHx[ADC_channel] =
		(u32)(ADC_COMP_TH_H_CHx(CompThresH) | ADC_COMP_TH_L_CHx(CompThresL));

	/* Set ADC channel comparison criteria */
	value = adc->ADC_COMP_CTRL;
	value &= ~ADC_MASK_COMP_CTRL_CH(ADC_channel);
	value |= (CompCtrl << ADC_SHIFT_COMP_CTRL_CH(ADC_channel));
	adc->ADC_COMP_CTRL = value;

	/* Enable comparison interrupt */
	ADC_INTConfig(ADC_IT_COMP_CH_EN(ADC_channel), ENABLE);
}

/**
 * @brief Determine ADC FIFO is readable or not.
 * @param None.
 * @return ADC FIFO is readable or not:
 * 		- 0: Not readable
 * 		- 1: Readable
 */
u32 ADC_Readable(void)
{
	u32 Status = ADC_GetStatus();
	u32 Readable = (((Status & ADC_BIT_FIFO_EMPTY) == 0) ? 1 : 0);

	return Readable;
}

/**
 * @brief Read data from ADC receive FIFO .
 * @param None
 * @return The conversion data with the channel index that the data belongs to.
 */
u32 ADC_Read(void)
{
	u32 value = ADC_ID_AND_DATA(ADC->ADC_DATA_GLOBAL);

	return value;
}

/**
 * @brief Read data in auto mode continuously.
 * @param pBuf Pointer to buffer to keep sample data.
 * @param len Number of sample data to be read.
 * @return None
 */
void ADC_ReceiveBuf(u32 *pBuf, u32 len)
{
	u32 i = 0;

	ADC_ClearFIFO();

	ADC_AutoCSwCmd(ENABLE);

	for (i = 0; i < len; i++) {
		while (ADC_Readable() == 0);
		pBuf[i] = ADC_Read();
	}
	ADC_AutoCSwCmd(DISABLE);
}

/**
 * @brief Clear ADC FIFO.
 * @param None
 * @return None
 */
void ADC_ClearFIFO(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	adc->ADC_CLR_FIFO = ADC_BIT_CLR_FIFO;
	while (adc->ADC_FLR != 0);
	adc->ADC_CLR_FIFO = 0;
}

/**
 * @brief Get ADC status.
 * @param None
 * @return Current status.
 */
u32 ADC_GetStatus(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	return adc->ADC_BUSY_STS;
}

#if 0
/**
 * @brief Control the ADC module to do a conversion.
 * @param NewState This parameter can be one of the following values:
 * 		@arg ENABLE: Enable the analog module and analog mux, then start a new channel conversion.
 * 		@arg DISABLE: Disable the analog module and analog mux.
 * @return None
 * @note Used in Software Trigger Mode.
 * @note Every time this bit is set to 1, ADC module would switch to a new channel and do one conversion.
 * 		Every time a conversion is done, software must clear this bit manually.
 * @note Sync time: 4*adc_clk + 1.5*sample_clk
 */
#endif
void ADC_SWTrigCmd(u32 NewState)
{
#if 0
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	u8 div = adc->ADC_CLK_DIV;
	u8 sync_time[7] = {3, 4, 7, 10, 13, 25, 49};

	if (NewState != DISABLE) {
		adc->ADC_SW_TRIG = ADC_BIT_SW_TRIG;

		/* Wait to sync signal */
		/* power_on delay: 300us */
		DelayUs(300 + sync_time[div]);
	} else {
		adc->ADC_SW_TRIG = 0;

		/* Wait to sync signal */
		/* power_off delay: 4us */
		DelayUs(sync_time[div] > 4 ? sync_time[div] : 4);
	}
#else
	UNUSED(NewState);

	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "NOTE: ADC software-trigger mode is not supported!!!\n");
#endif
}

/**
 * @brief Enable or disable the automatic channel switch.
 * @param NewState This parameter can be one of the following values:
 * 		@arg ENABLE: Enable the automatic channel switch.
 * 		@arg DISABLE: Disable the automatic channel switch.
 * @return None
 * @note Used in Automatic Mode
 * @note When setting this bit, an automatic channel switch starts from the first channel in the channel switch list.
 * 		If an automatic channel switch is in progress, writing 0 will terminate the automatic channel switch.
 * @note Sync time: 4*adc_clk + 1.5*sample_clk
 */
void ADC_AutoCSwCmd(u32 NewState)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	u8 div = adc->ADC_CLK_DIV;
	u8 sync_time[7] = {3, 4, 7, 10, 13, 25, 49};
	u32 value;

	if (NewState != DISABLE) {
		adc->ADC_AUTO_CSW_CTRL = ADC_BIT_AUTOSW_EN;

		/* Wait to sync signal */
		/* power_on delay: 300us */
		DelayUs(300 + sync_time[div]);
	} else {
		value = PLL_BASE->PLL_LPAD0;
		value &= ~(PLL_BIT_POW_REF | PLL_BIT_POW | PLL_BIT_POW_LDO);
		PLL_BASE->PLL_LPAD0 = value;

		adc->ADC_AUTO_CSW_CTRL = 0;

		/* Wait to sync signal */
		/* power_off delay: 4us */
		DelayUs(sync_time[div] > 4 ? sync_time[div] : 4);
	}
}

#if 0
/**
 * @brief Initialize the trigger timer in ADC Timer-Trigger Mode.
 * @param Tim_Idx Index of a basic timer that would be used to trigger ADC conversion.
 * @note This parameter should be 0-7.
 * @param PeriodMs Period of trigger timer in ms, which can be 1ms-131071ms.
 * @param NewState This parameter can be one of the following values:
 * 		@arg ENABLE: Enable the ADC timer trigger mode.
 * 		@arg DISABLE: Disable the ADC timer trigger mode.
 * @return None
 * @note Used in Timer-Trigger Mode
 */
#endif
void ADC_TimerTrigCmd(u8 Tim_Idx, u32 PeriodMs, u32 NewState)
{
#if 0
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	RTIM_TimeBaseInitTypeDef TIM_InitStruct;

	assert_param(IS_ADC_VALID_TIM(Tim_Idx));
	adc->ADC_EXT_TRIG_TIMER_SEL = Tim_Idx;

	if (NewState != DISABLE) {
		assert_param(PeriodMs > 0 && PeriodMs < 131072); // avoid overflow

		RTIM_TimeBaseStructInit(&TIM_InitStruct);
		TIM_InitStruct.TIM_Idx = Tim_Idx;
		TIM_InitStruct.TIM_Period = (PeriodMs * 32768) / 1000; //ms to tick

		RTIM_TimeBaseInit(TIMx[Tim_Idx], &TIM_InitStruct, TIMx_irq[Tim_Idx], (IRQ_FUN)NULL, (u32)NULL);
		RTIM_Cmd(TIMx[Tim_Idx], ENABLE);
	} else {
		RTIM_Cmd(TIMx[Tim_Idx], DISABLE);
	}
#else
	UNUSED(Tim_Idx);
	UNUSED(PeriodMs);
	UNUSED(NewState);

	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "NOTE: ADC timer-trigger mode is not supported!!!\n");
#endif
}

/**
 * @brief Control ADC oversample function.
 * @param NewState This parameter can be one of the following values:
 * 		@arg ENABLE: Enable ADC oversample.
 * 		@arg DISABLE: Disable ADC oversample.
 * @return None
 */
void ADC_OverSampleCmd(u32 NewState)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	if (ENABLE == NewState) {
		adc->ADC_OVERSAMPLE |= ADC_BIT_OS_EN;
	} else {
		adc->ADC_OVERSAMPLE &= ~ADC_BIT_OS_EN;
	}
}

/**
 * @brief Set ADC oversample parameters.
 * @param OS_Shift Oversample right shift bit.
 * 		This parameter can be one of the following parameters:
 * 		@arg ADC_OSF_NONE: No shift.
 * 		@arg ADC_OSF_1: Right shift 1bit.
 * 		@arg ADC_OSF_2: Right shift 2bit.
 * 		@arg ADC_OSF_3: Right shift 3bit.
 * 		@arg ADC_OSF_4: Right shift 4bit.
 * 		@arg ADC_OSF_5: Right shift 5bit.
 * 		@arg ADC_OSF_6: Right shift 6bit.
 * 		@arg ADC_OSF_7: Right shift 7bit.
 * @param OS_Ratio Oversample Ratio.
 * 		This parameter can be one of the following parameters:
 * 		@arg ADC_OSR_2: 2x.
 * 		@arg ADC_OSR_4: 4x.
 * 		@arg ADC_OSR_8: 8x.
 * 		@arg ADC_OSR_16: 16x.
 * 		@arg ADC_OSR_32: 32x.
 * 		@arg ADC_OSR_64: 64x.
 * 		@arg ADC_OSR_128: 128x.
 * 		@arg ADC_OSR_256: 256x.
 * @param OS_Mode Oversample Mode.
 * 		This parameter can be one of the following parameters:
 * 		@arg ADC_OS_STAGGERED: All oversampling conversions done in staggered sequence.
 * 		@arg ADC_OS_REGULAR: All oversampling conversions done in regular sequence.
 * @return None
 */
void ADC_SetOverSample(u8 OS_Shift, u8 OS_Ratio, u8 OS_Mode)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	u32 value;

	assert_param(IS_ADC_OSF(OS_Shift));
	assert_param(IS_ADC_OSR(OS_Ratio));
	assert_param(IS_ADC_OS_MODE(OS_Mode));

	value = adc->ADC_OVERSAMPLE;
	value &= ~(ADC_MASK_OSF | ADC_MASK_OSR | ADC_BIT_OV_MODE);
	value |= ADC_OSF(OS_Shift) | ADC_OSR(OS_Ratio);

	if (ADC_OS_REGULAR == OS_Mode) {
		value |= ADC_BIT_OV_MODE;
	}

	adc->ADC_OVERSAMPLE = value;
}

/**
 * @brief Initialize ADC calibration parameters according to EFuse.
 * @param CalPara Pointer to ADC calibration parameter structure.
 * @param IsVBatChan Calibration parameter belongs to vbat channel or normal channel.
 * 		This parameter can be one of the following values:
 * 		@arg TRUE: Calibration parameter belongs to vbat channel.
 * 		@arg FALSE: Calibration parameter belongs to normal channel.
 * @return None.
 */
void ADC_InitCalPara(ADC_CalParaTypeDef *CalPara, u8 IsVBatChan)
{
	u8 index;
	u8 EfuseBuf[6];
	u32 AdcCalAddr;
	u16 K_A, K_B, K_C;
	s32 ka, kb, kc;

	if (IsVBatChan) {
		AdcCalAddr = VBAT_VOL_ADDR; /* vbat channel */
	} else {
		AdcCalAddr = NORM_VOL_ADDR; /* normal channel */
	}

	for (index = 0; index < 6; index++) {
		OTP_Read8(AdcCalAddr + index, EfuseBuf + index);
	}
	K_A = EfuseBuf[1] << 8 | EfuseBuf[0];
	K_B = EfuseBuf[3] << 8 | EfuseBuf[2];
	K_C = EfuseBuf[5] << 8 | EfuseBuf[4];

	if (K_A == 0xFFFF) {
		if (IsVBatChan) {
			K_A = 0xFEBC;
		} else {
			K_A = 0xFED7;
		}
	}

	if (K_B == 0xFFFF) {
		if (IsVBatChan) {
			K_B = 0xB8C8;
		} else {
			K_B = 0x75FE;
		}
	}

	if (K_C == 0xFFFF) {
		if (IsVBatChan) {
			K_C = 0xCF48;
		} else {
			K_C = 0xDF51;
		}
	}

	ka = (K_A & BIT15) ? -(0x10000 - K_A) : (K_A & 0x7FFF);
	kb = (K_B & 0xFFFF);
	kc = (K_C & BIT15) ? -(0x10000 - K_C) : (K_C & 0x7FFF);

	CalPara->cal_a = ka;
	CalPara->cal_b = kb;
	CalPara->cal_c = kc;
	CalPara->init_done = TRUE;
}

/**
 * @brief Get normal channel voltage value in mV.
 * @param chan_data ADC conversion data.
 * @return ADC voltage value in mV.
 * @note This function is for all the channels except channel7(VBAT).
 */
s32 ADC_GetVoltage(u32 chan_data)
{
	s64 ka, kb;
	s32 kc;
	s32 ch_vol;

	if (!CalParaNorm.init_done) {
		ADC_InitCalPara(&CalParaNorm, FALSE);
	}

	ka = CalParaNorm.cal_a;
	kb = CalParaNorm.cal_b;
	kc = CalParaNorm.cal_c;

	ch_vol = ((ka * chan_data * chan_data) >> 26) + ((kb * chan_data) >> 15) + (kc >> 6);

	return ch_vol;
}

/**
 * @brief Get VBAT voltage value in mV.
 * @param vbat_data ADC conversion data from channel7(VBAT).
 * @return ADC voltage value in mV.
 * @note This function is only for channel7(VBAT).
 */
s32 ADC_GetVBATVoltage(u32 vbat_data)
{
	s64 ka, kb;
	s32 kc;
	s32 ch_vol;

	if (!CalParaVBat.init_done) {
		ADC_InitCalPara(&CalParaVBat, TRUE);
	}

	ka = CalParaVBat.cal_a;
	kb = CalParaVBat.cal_b;
	kc = CalParaVBat.cal_c;

	ch_vol = ((ka * vbat_data * vbat_data) >> 26) + ((kb * vbat_data) >> 15) + (kc >> 6);

	return ch_vol;
}

/**
 * @brief Get internal R resistance of V33 channels(CH0~CH5) in divided mode.
 * @param none.
 * @return Internal R resistance value in Kohm.
 */
u32 ADC_GetInterR(void)
{
	u8 r_offset;

	OTP_Read8(INTER_R_ADDR, &r_offset);

	return ((u32)r_offset + 400);
}

/**
  * @brief Get normal or vbat sample value according to voltage in mV.
  * @param VolMV: ADC Voltage in mV, which can be 0-3300.
  * @param IsVBatChan: Calibration parameter belongs to vbat channel or normal channel.
  *   This parameter can be one of the following values:
  *        @arg TRUE: Calibration parameter belongs to vbat channel.
  *        @arg FALSE: Calibration parameter belongs to normal channel.
  * @return ADC conversion data.
  */
u32 ADC_GetSampleValue(s32 VolMV, u8 IsVBatChan)
{
	ADC_CalParaTypeDef CalPara;
	s64 ka, kb;
	s32 kc;
	s64 discriminant;
	s64 result;

	if (IsVBatChan == TRUE) {
		if (!CalParaVBat.init_done) {
			ADC_InitCalPara(&CalParaVBat, TRUE);
		}
		CalPara = CalParaVBat;
	} else {
		if (!CalParaNorm.init_done) {
			ADC_InitCalPara(&CalParaNorm, FALSE);
		}
		CalPara = CalParaNorm;
	}

	ka = CalPara.cal_a;
	kb = CalPara.cal_b;
	kc = CalPara.cal_c;

	/* calibrated para A may be 0 */
	if (ka == 0) {
		return ((VolMV << 15) - (kc << 9)) / kb;
	}

	discriminant = kb * kb + 64 * ka * VolMV - ka * kc;
	if (discriminant >= 0) {
		result = (sqrt((float)discriminant) - kb) * 1024 / ka;

		if (result < 0) {
			return 0;
		} else if (result > 0xFFF) {
			return 0xFFF;
		} else {
			return (u32)result;
		}
	} else {
		assert_param(0);
	}
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