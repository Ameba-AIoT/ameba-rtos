/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/**
  * @brief  Initializes the parameters in the TM_InitStruct with default values.
  * @param  TM_InitStruct: pointer to a TM_InitTypeDef structure that contains
  *         the configuration information for the thermal peripheral.
  * @retval  None
  */
void TM_StructInit(TM_InitTypeDef *TM_InitStruct)
{
	TM_InitStruct->TM_DownSampRate = TM_DSR_512;
	TM_InitStruct->TM_AdcClkDiv = TM_ADC_CLK_DIV_128;
	TM_InitStruct->TM_HighProtectThreshold = 0x7D;
	TM_InitStruct->TM_HighWarnThreshold = 0x69;
	TM_InitStruct->TM_LowWarnThreshold = 0x1D8;
	TM_InitStruct->TM_TimePeriod = 0x0F;
}

/**
  * @brief  Initializes the thermal according to the specified parameters in TM_InitStruct.
  * @param  TM_InitStruct: pointer to a TM_InitTypeDef structure that contains
  *         the configuration information for the thermal peripheral.
  * @retval  None
  */
void TM_Init(TM_InitTypeDef *TM_InitStruct)
{
	THERMAL_TypeDef *thermal = THERMAL;
	u32 Tmp;

	assert_param(IS_TM_DOWN_SAMPLE_RATE(TM_InitStruct->TM_DownSampRate));
	assert_param(IS_TM_ADC_CLK_DIV(TM_InitStruct->TM_AdcClkDiv));
	assert_param((TM_InitStruct->TM_HighProtectThreshold >= 0x46) && (TM_InitStruct->TM_HighProtectThreshold <= 0x8C));
	assert_param(TM_InitStruct->TM_HighWarnThreshold <= 0x1FF);
	assert_param(TM_InitStruct->TM_LowWarnThreshold <= 0x1FF);

	/* Disable thermal, clear pending interrupt */

	TM_INTConfig(TM_BIT_IMR_TM_HIGH_WT | TM_BIT_IMR_TM_LOW_WT, DISABLE);

	TM_Cmd(DISABLE);

	thermal->TM_CTRL &= ~TM_MASK_OSR;
	thermal->TM_CTRL |= TM_OSR(TM_InitStruct->TM_DownSampRate);
	if (TM_InitStruct->TM_AdcClkDiv == TM_ADC_CLK_DIV_128) {
		thermal->TM_CTRL |= TM_BIT_ADCCKSEL;
	} else {
		thermal->TM_CTRL &= ~TM_BIT_ADCCKSEL;
	}

	TM_MaxTempClr();
	TM_MinTempClr();

	thermal->TM_TIMER = TM_TIME_PERIOD(TM_InitStruct->TM_TimePeriod);

	TM_Cmd(ENABLE);

	TM_INTClear();

	Tmp = (TM_BIT_HIGHCMP_PT_EN |
		   TM_HIGH_PT_THR(TM_InitStruct->TM_HighProtectThreshold) |
		   TM_BIT_HIGHCMP_WT_EN |
		   TM_HIGH_WT_THR(TM_InitStruct->TM_HighWarnThreshold) |
		   TM_BIT_LOWCMP_WT_EN |
		   TM_LOW_WT_THR(TM_InitStruct->TM_LowWarnThreshold));
	thermal->TM_TH_CTRL = Tmp;

	TM_INTConfig(TM_BIT_IMR_TM_HIGH_WT | TM_BIT_IMR_TM_LOW_WT, ENABLE);

}

/**
  * @brief  Enable or Disable the thermal peripheral.
  * @param  NewState: new state of the thermal peripheral.
  *   			This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TM_Cmd(u32 NewState)
{
	THERMAL_TypeDef *thermal = THERMAL;

	if (NewState != DISABLE) {
		thermal->TM_CTRL |= TM_BIT_POWCUT;
		thermal->TM_CTRL |= TM_BIT_POW;
		thermal->TM_CTRL |= TM_BIT_RSTB;
	} else {
		thermal->TM_CTRL &= ~TM_BIT_RSTB;
		thermal->TM_CTRL &= ~TM_BIT_POW;
		thermal->TM_CTRL &= ~TM_BIT_POWCUT;
	}
}
/**
 * @brief confirm whether thermal data is valid
 *
 * @return RTK_SUCCESS/RTK_FAIL
 */
int TM_PollDataValid(void)
{
	THERMAL_TypeDef *thermal = THERMAL;
	u32 timeout = 1000000;

	while (thermal->TM_CTRL & TM_BIT_EN_LATCH) {
		timeout--;
		if (timeout == 0) {
			return RTK_FAIL;
		}
	}
	// Data is valid when latch signal is low
	return RTK_SUCCESS;
}
/**
  * @brief  ENABLE/DISABLE  the thermal interrupt bits.
  * @param  TM_IT: specifies the thermal interrupt to be setup.
  *          This parameter can be one or combinations of the following values:
  *            @arg TM_BIT_IMR_TM_LOW_WT:	thermal low temperature warning interrupt
  *            @arg TM_BIT_IMR_TM_HIGH_WT:	thermal high temperature warning interrupt
  * @param  NewState: ENABLE/DISABLE.
  * @retval  None
  */
void TM_INTConfig(u32 TM_IT, u32 NewState)
{
	THERMAL_TypeDef *thermal = THERMAL;

	if (NewState != DISABLE) {
		thermal->TM_INTR_CTRL |= TM_IT;
	} else {
		thermal->TM_INTR_CTRL &= ~TM_IT;
	}
}

/**
  * @brief	 Clears all the thermal interrupt pending bits.
  * @param  None
  * @retval  None
  */
void TM_INTClear(void)
{
	THERMAL_TypeDef *thermal = THERMAL;

	/* Clear the all IT pending Bits */
	thermal->TM_INTR_STS = (TM_BIT_ISR_TM_LOW_WT | TM_BIT_ISR_TM_HIGH_WT);
}

/**
  * @brief  Clears the thermal interrupt pending bits.
  * @param  TM_IT: specifies the pending bit to clear.
  *   This parameter can be any combination of the following values:
  *            @arg TM_BIT_ISR_TM_LOW_WT
  *            @arg TM_BIT_ISR_TM_HIGH_WT
  * @retval  None
  */
void TM_INTClearPendingBits(u32 TM_IT)
{
	THERMAL_TypeDef *thermal = THERMAL;

	thermal->TM_INTR_STS = TM_IT;
}

/**
  * @brief  Get thermal interrupt status.
  * @param  None
  * @retval  Current Interrupt Status, each bit of this value represents one
  *		interrupt status which is as follows:
  *            @arg TM_BIT_ISR_TM_LOW_WT
  *            @arg TM_BIT_ISR_TM_HIGH_WT
  */
u32 TM_GetISR(void)
{
	THERMAL_TypeDef *thermal = THERMAL;

	return thermal->TM_INTR_STS;
}

/**
  * @brief  Get thermal temperature result.
  * @param  None.
  * @retval TM_INVALID_VALUE, invalid value;[0, TM_INVALID_VALUE), The measured temperature.
  */
u32 TM_GetTempResult(void)
{
	THERMAL_TypeDef *thermal = THERMAL;

	if (TM_PollDataValid() != RTK_SUCCESS) {
		return TM_INVALID_VALUE;
	}

	return TM_GET_OUT(thermal->TM_RESULT);
}

/**
  * @brief  Get thermal power on temperature result.
  * @param  None.
  * @retval TM_INVALID_VALUE, invalid value;[0, TM_INVALID_VALUE), The measured power on temperature.
  */
u32 TM_GetPowOnTemp(void)
{
	THERMAL_TypeDef *thermal = THERMAL;

	if (TM_PollDataValid() != RTK_SUCCESS) {
		return TM_INVALID_VALUE;
	}

	return TM_GET_TEMP_OUT_POWERON(thermal->TM_OUT_PWR_ON);
}

/**
  * @brief  Get thermal max temperature result.
  * @param  None.
  * @retval TM_INVALID_VALUE, invalid value;[0, TM_INVALID_VALUE), the measured max temperature.
  */
u32 TM_GetMaxTemp(void)
{
	THERMAL_TypeDef *thermal = THERMAL;

	if (TM_PollDataValid() != RTK_SUCCESS) {
		return TM_INVALID_VALUE;
	}

	return TM_GET_MAX(thermal->TM_MAX_CTRL);
}

/**
  * @brief  Get thermal min temperature result.
  * @param  None.
  * @retval TM_INVALID_VALUE, invalid value;[0, TM_INVALID_VALUE), the measured min temperature.
  */
u32 TM_GetMinTemp(void)
{
	THERMAL_TypeDef *thermal = THERMAL;

	if (TM_PollDataValid() != RTK_SUCCESS) {
		return TM_INVALID_VALUE;
	}

	return TM_GET_MIN(thermal->TM_MIN_CTRL);
}

/**
  * @brief  Clear the max temperature recorded by thermal.
  * @note tm_max_clr bit sets to 1, the record max temperature is cleared;
  *         tm_max_clr bit sets to 0, thermal start to record the max temperature.
  * @retval None
  */
void TM_MaxTempClr(void)
{
	THERMAL_TypeDef *thermal = THERMAL;

	thermal->TM_MAX_CTRL |= TM_BIT_MAX_CLR;
	thermal->TM_MAX_CTRL &= ~TM_BIT_MAX_CLR;
}

/**
  * @brief  Clear the min temperature recorded by thermal.
  * @note tm_min_clr bit sets to 1, the record min temperature is cleared;
  *         tm_min_clr bit sets to 0, thermal start to record the min temperature.
  * @retval None
  */
void TM_MinTempClr(void)
{
	THERMAL_TypeDef *thermal = THERMAL;

	thermal->TM_MIN_CTRL |= TM_BIT_MIN_CLR;
	thermal->TM_MIN_CTRL &= ~TM_BIT_MIN_CLR;
}

/**
  * @brief  Enable or Disable high temperature protect function.
  * @param  NewState: new state of the thermal lathigh temperature protect functionch.
  *   			This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void TM_SetHighPT(u32 NewState)
{
	THERMAL_TypeDef *thermal = THERMAL;

	if (NewState != DISABLE) {
		thermal->TM_TH_CTRL |= TM_BIT_HIGHCMP_PT_EN;
	} else {
		thermal->TM_TH_CTRL &= ~TM_BIT_HIGHCMP_PT_EN;
	}
}

/**
  * @brief  Get Celsius Degree
  * @param  Data: TM_RESULT(Binary complement form)
  * @retval Temperature value of float type
  * 		[0, 0x3FFFF] == > [0, 255.999], Positive temperature range.
  * 		[0x40000, 0x7FFFF] == > [-256, -0.00097], Negative temperature range
  * 		0x80000 ==> 256, invalid value.
  */
float TM_GetCdegree(u32 Data)
{
	int integer = 0;
	float decimal = 0;
	float Cdegree = 0;
	u32 temp = 0;

	if (TM_IS_NEGATIVE(Data)) {
		temp = TM_NEGATIVE_MAX + 1 - Data;
		integer = (int)(temp) / 1024;
		decimal = (float)(TEMP_DECIMAL_OUT(temp)) / 1024;
		Cdegree = 0 - (integer + decimal);
	} else {
		integer = (int)(Data) / 1024;
		decimal = (float)(TEMP_DECIMAL_OUT(Data)) / 1024;
		Cdegree = (integer + decimal);
	}

	return Cdegree;
}

/**
  * @brief  Get Fahrenheit Degree
  * @param  Data: TM_RESULT(Binary complement form)
  * @retval Temperature value of float type
  * 		[0, 0x3FFFF] == > [32, 492.798], Positive temperature range.
  * 		[0x40000, 0x7FFFF] == > [-492.8, 31.998], Negative temperature range
  * 		0x80000 ==> 492.8, invalid degree.
  */
float TM_GetFdegree(u32 Data)
{
	float Fdegree = 0;

	Fdegree = (TM_GetCdegree(Data) * 1.8) + 32;

	return Fdegree;
}

