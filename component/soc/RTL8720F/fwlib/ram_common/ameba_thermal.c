/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

/** @addtogroup Ameba_Periph_Driver
 * @{
 */

/** @defgroup THERMAL THERMAL
 * @brief THERMAL driver modules
 * @{
 */
/* Current temperature sample value */
u32 CurrentTemp;

/* Exported functions --------------------------------------------------------*/
/** @defgroup THERMAL_Exported_Functions THERMAL Exported Functions
 * @{
 */

/* Threshold format: 1 sign bit + 8 integer bits (two's complement), in Celsius */
/* Keep the threshold format of TM_InitTypeDef consistent with previous ICs (e.g. 0x7D = 125C, 0x1D8 = -40C) */
static float TM_ThreToFloat(u16 Thre)
{
	s32 val = Thre & 0x1FF;

	if (val & 0x100) {
		val -= 0x200;
	}

	return (float)val;
}

/**
 * @brief Initialize the parameters in the TM_InitStruct with default values.
 * @param TM_InitStruct Pointer to a @ref TM_InitTypeDef structure that contains
 *     the configuration information for the thermal peripheral.
 */
void TM_StructInit(TM_InitTypeDef *TM_InitStruct)
{
	TM_InitStruct->TM_HighProtectThreshold = 0x7D; // 125 Celsius
	TM_InitStruct->TM_HighWarnThreshold = 0x69; // 105 Celsius
	TM_InitStruct->TM_LowWarnThreshold = 0x1D8; // -40 Celsius
	TM_InitStruct->TM_TimePeriod = 0x8; // N * 0.65s = 5.2s
}

/**
 * @brief Initialize the thermal according to the specified parameters in TM_InitStruct.
 * @param TM_InitStruct Pointer to a @ref TM_InitTypeDef structure that contains
 *     the configuration information for the thermal peripheral.
 */
void TM_Init(TM_InitTypeDef *TM_InitStruct)
{
	ADC_TypeDef *adc = ADC;
	u32 reg;
	u32 cur_list_len;
	u32 list0, list1;
	u32 i;
	u8 ch8_found;
	u8 ch;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	ADC_Cmd(DISABLE);

	/* add ch8 to adc channel list if not already present */
	cur_list_len = ADC_GET_CVLIST_LEN(adc->ADC_CONF);
	list0 = adc->ADC_CHSW_LIST_0;
	list1 = adc->ADC_CHSW_LIST_1;
	ch8_found = 0;
	for (i = 0; i <= cur_list_len; i++) {
		if (i < 8) {
			ch = (u8)((list0 >> ADC_SHIFT_CHSW0(i)) & 0xF);
		} else {
			ch = (u8)((list1 >> ADC_SHIFT_CHSW1(i)) & 0xF);
		}
		if (ch == ADC_CH8) {
			ch8_found = 1;
			break;
		}
	}
	if (!ch8_found) {
		if (cur_list_len + 1 >= 0x10) {
			RTK_LOGE(NOTAG, "Thermal ch cannot be added to ch list!!!\n");
			RTK_LOGE(NOTAG, "Original ADC ch list is full!!!\n");
			return;
		}
		/* append ch8 by directly updating CVLIST_LEN and the channel switch list */
		reg = adc->ADC_CONF;
		reg &= ~ADC_MASK_CVLIST_LEN;
		reg |= ADC_CVLIST_LEN(cur_list_len + 1);
		adc->ADC_CONF = reg;

		if ((cur_list_len + 1) < 8) {
			reg = adc->ADC_CHSW_LIST_0;
			reg &= ~((u32)0xF << ADC_SHIFT_CHSW0(cur_list_len + 1));
			reg |= (u32)ADC_CH8 << ADC_SHIFT_CHSW0(cur_list_len + 1);
			adc->ADC_CHSW_LIST_0 = reg;
		} else {
			reg = adc->ADC_CHSW_LIST_1;
			reg &= ~((u32)0xF << ADC_SHIFT_CHSW1(cur_list_len + 1));
			reg |= (u32)ADC_CH8 << ADC_SHIFT_CHSW1(cur_list_len + 1);
			adc->ADC_CHSW_LIST_1 = reg;
		}
	}

	/* set op mode to auto mode if not already */
	if (ADC_GET_OP_MOD(adc->ADC_CONF) != ADC_AUTO_MODE) {
		reg = adc->ADC_CONF;
		reg &= ~ADC_MASK_OP_MOD;
		reg |= ADC_OP_MOD(ADC_AUTO_MODE);
		adc->ADC_CONF = reg;
	}

	ADC_Cmd(ENABLE);

	/* Disable thermal, clear pending interrupt */
	TM_INTConfig(ADC_BIT_IT_TM_HIGH_WT_EN | ADC_BIT_IT_TM_LOW_WT_EN, DISABLE);

	TM_Cmd(DISABLE);
	TM_MaxTempClr();
	TM_MinTempClr();

	reg = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_PMC_OPT);
	reg &= ~AON_MASK_THM_RESUME_CNT;
	reg |= AON_THM_RESUME_CNT(TM_InitStruct->TM_TimePeriod);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_PMC_OPT, reg);

	adc -> ADC_TM_MAX_CTRL |= ADC_BIT_TM_MAX_EN;
	adc -> ADC_TM_MIN_CTRL |= ADC_BIT_TM_MIN_EN;

	TM_Cmd(ENABLE);

	TM_INTClear();

	TM_HighPtConfig(TM_ThreToFloat(TM_InitStruct->TM_HighProtectThreshold), ENABLE, TM_DEG_C);
	TM_HighWtConfig(TM_ThreToFloat(TM_InitStruct->TM_HighWarnThreshold), ENABLE, TM_DEG_C);
	TM_LowWtConfig(TM_ThreToFloat(TM_InitStruct->TM_LowWarnThreshold), ENABLE, TM_DEG_C);

	TM_INTConfig(ADC_BIT_IT_TM_HIGH_WT_EN | ADC_BIT_IT_TM_LOW_WT_EN, ENABLE);

	ADC_AutoCSwCmd(ENABLE);
}

/**
 * @brief Enable or disable the specified thermal fields to be programmed.
 * @param NewState ENABLE or DISABLE.
 *     @arg ENABLE: Write the correct password TM_PROG_PWR to unlock programming.
 *     @arg DISABLE: Write 0x0 to lock programming.
 */
void TM_PwrProgCmd(u32 NewState)
{
	ADC_TypeDef *adc = ADC;
	u32 reg;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	reg = adc->ADC_TM_TH_CTRL1;
	reg &= ~ADC_MASK_TM_PWR;

	if (NewState != DISABLE) {
		reg |= ADC_TM_PWR(TM_PROG_PWR);
	}

	adc->ADC_TM_TH_CTRL1 = reg;
}

/**
 * @brief Enable or disable the thermal peripheral.
 * @param NewState New state of the thermal peripheral.
 *  			This parameter can be ENABLE or DISABLE.
 */
void TM_Cmd(u32 NewState)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	TM_PwrProgCmd(ENABLE);

	if (NewState != DISABLE) {
		/* set en_bit only and keep high_pt_thre unchanged */
		adc->ADC_TM_TH_CTRL1 |= ADC_BIT_TM_HIGHCMP_PT_EN;
		TM_INTConfig(ADC_BIT_IT_TM_HIGH_WT_EN | ADC_BIT_IT_TM_LOW_WT_EN, ENABLE);
	} else {
		adc->ADC_TM_TH_CTRL1 &= ~ADC_BIT_TM_HIGHCMP_PT_EN;
		TM_INTConfig(ADC_BIT_IT_TM_HIGH_WT_EN | ADC_BIT_IT_TM_LOW_WT_EN, DISABLE);
	}

	TM_PwrProgCmd(DISABLE);
}

/**
 * @brief Confirm whether thermal data is valid.
 * @return Thermal data valid status:
 *         - RTK_SUCCESS: thermal data is valid
 *         - RTK_FAIL: thermal data is invalid
 */
int TM_PollDataValid(void)
{
	ADC_TypeDef *adc = ADC;
	u32 timeout = 1000000;
	u32 value;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	value = adc->ADC_DATA_CHx[ADC_CH8];

	while (!(value & ADC_BIT_DAT_RDY_CHx)) {
		timeout--;
		if (timeout == 0) {
			return RTK_FAIL;
		}
		value = adc->ADC_DATA_CHx[ADC_CH8];
	}

	CurrentTemp = ADC_DATA_CHx(value);
	return RTK_SUCCESS;
}

/**
 * @brief Enable or disable the thermal interrupt bits.
 * @param TM_IT Specifies the thermal interrupt to be configured.
 * @param NewState ENABLE or DISABLE.
 */
void TM_INTConfig(u32 TM_IT, u32 NewState)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	assert_param(IS_TM_INT(TM_IT));

	if (NewState != DISABLE) {
		adc->ADC_INTR_CTRL |= TM_IT;
	} else {
		adc->ADC_INTR_CTRL &= ~TM_IT;
	}
}

/**
 * @brief Clear all the thermal interrupt pending bits.
 */
void TM_INTClear(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	/* Clear the all IT pending Bits */
	adc->ADC_INTR_STS = (ADC_BIT_IT_TM_LOW_WT_STS | ADC_BIT_IT_TM_HIGH_WT_STS);
}

/**
 * @brief Clear the thermal interrupt pending bits.
 * @param TM_IT Specifies the thermal interrupt pending bits to clear.
 */
void TM_INTClearPendingBits(u32 TM_IT)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	assert_param(IS_TM_INT(TM_IT));

	adc->ADC_INTR_STS = TM_IT;
}

/**
 * @brief Get thermal interrupt status.
 * @return Current thermal interrupt status.
 */
u32 TM_GetISR(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	return adc->ADC_INTR_STS;
}

/**
 * @brief Get thermal temperature result.
 * @return The measured temperature:
 *         - TM_INVALID_VALUE: invalid value
 *         - [0, TM_INVALID_VALUE): measured temperature
 */
u32 TM_GetTempResult(void)
{
	ADC_ClearFIFO();

	if (TM_PollDataValid() != RTK_SUCCESS) {
		return TM_INVALID_VALUE;
	}

	return CurrentTemp;
}

/**
 * @brief Get thermal power on temperature result.
 * @return The measured power on temperature:
 *         - TM_INVALID_VALUE: invalid value
 *         - [0, TM_INVALID_VALUE): measured power on temperature
 */
u32 TM_GetPowOnTemp(void)
{
	return TM_GetTempResult();
}

/**
 * @brief Get thermal max temperature result.
 * @return The measured max temperature:
 *         - TM_INVALID_VALUE: invalid value
 *         - [0, TM_INVALID_VALUE): measured max temperature
 */
u32 TM_GetMaxTemp(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	if (TM_PollDataValid() != RTK_SUCCESS) {
		return TM_INVALID_VALUE;
	}

	return ADC_GET_TM_MAX(adc->ADC_TM_MAX_CTRL);
}

/**
 * @brief Get thermal min temperature result.
 * @return The measured min temperature:
 *         - TM_INVALID_VALUE: invalid value
 *         - [0, TM_INVALID_VALUE): measured min temperature
 */
u32 TM_GetMinTemp(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	if (TM_PollDataValid() != RTK_SUCCESS) {
		return TM_INVALID_VALUE;
	}

	return ADC_GET_TM_MIN(adc->ADC_TM_MIN_CTRL);
}

/**
 * @brief Clear the max temperature recorded by thermal.
 */
void TM_MaxTempClr(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	/* tm_max_clr bit will be cleared automatically in the next period when set to 1 */
	adc->ADC_TM_MAX_CTRL |= ADC_BIT_TM_MAX_CLR;
}

/**
 * @brief Clear the min temperature recorded by thermal.
 */
void TM_MinTempClr(void)
{
	ADC_TypeDef *adc = ADC;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	/* tm_min_clr bit will be cleared automatically in the next period when set to 1 */
	adc->ADC_TM_MIN_CTRL |= ADC_BIT_TM_MIN_CLR;
}

/**
 * @brief Get temperature code according to Celsius Degree.
 * @param TempC Temperature in Celsius Degree, which can be signed float.
 * @return Temperature code.
 */
static u16 TM_GetTempCode(float TempC)
{
	/* sync with HW: T = 0.480828f * ADC_data - 288.768 */
	return (u16)((TempC + 288.768f) / 0.480828f);
}

/**
 * @brief Configure over-temp protect threshold for comparison with the measured temperature.
 * @param TM_HighPtThre Temperature threshold (float). When the measured temperature exceeds the threshold, an AON reset is triggered.
 * @note Only between 70°C and 140°C are valid.
 * @param NewState New state of the thermal over-temp protect comparison.
 *  			This parameter can be ENABLE or DISABLE.
 * @param Unit Unit of TM_HighPtThre.
 *  			This parameter can be TM_DEG_C or TM_DEG_F.
 */
void TM_HighPtConfig(float TM_HighPtThre, u32 NewState, TM_UnitDef Unit)
{
	ADC_TypeDef *adc = ADC;
	u32 reg;
	u16 code;

	/* F = C * 1.8 + 32 */
	float TempC = (Unit == TM_DEG_C) ? TM_HighPtThre : (TM_HighPtThre - 32) / 1.8f;

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	code = TM_GetTempCode(TempC);

	TM_PwrProgCmd(ENABLE);

	if (NewState != DISABLE) {
		reg = adc->ADC_TM_TH_CTRL1;
		reg &= ~ADC_MASK_TM_HIGH_PT_THR;
		reg |= ADC_TM_HIGH_PT_THR(code);
		adc->ADC_TM_TH_CTRL1 = reg;
		adc->ADC_TM_TH_CTRL1 |= ADC_BIT_TM_HIGHCMP_PT_EN;
	} else {
		adc->ADC_TM_TH_CTRL1 &= ~ADC_BIT_TM_HIGHCMP_PT_EN;
	}

	TM_PwrProgCmd(DISABLE);
}

/**
 * @brief Configure over-temp warning threshold for comparison with the measured temperature.
 * @param TM_HighWtThre Temperature threshold (float). When TM_HighWtThre <= measured temperature < TM_HighPtThre, the high temperature warning interrupt pending bit is set.
 * @note Only values greater than 0°C are supported.
 * @param NewState New state of the thermal over-temp warning comparison.
 *  			This parameter can be ENABLE or DISABLE.
 * @param Unit Unit of TM_HighWtThre.
 *  			This parameter can be TM_DEG_C or TM_DEG_F.
 */
void TM_HighWtConfig(float TM_HighWtThre, u32 NewState, TM_UnitDef Unit)
{
	ADC_TypeDef *adc = ADC;
	u32 reg;
	u16 code;
	float TempC;

	if (NewState == DISABLE) {
		return;
	}

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	/* F = C * 1.8 + 32 */
	TempC = (Unit == TM_DEG_C) ? TM_HighWtThre : (TM_HighWtThre - 32) / 1.8f;

	code = TM_GetTempCode(TempC);

	reg = adc->ADC_TM_TH_CTRL0;
	reg &= ~ADC_MASK_TM_HIGH_WT_THR;
	reg |= ADC_TM_HIGH_WT_THR(code);
	adc->ADC_TM_TH_CTRL0 = reg;
}

/**
 * @brief Configure low-temp warning threshold for comparison with the measured temperature.
 * @param TM_LowWtThre Temperature threshold (float). When measured temperature <= TM_LowWtThre, the low temperature warning interrupt pending bit is set.
 * @param NewState New state of the thermal low-temp warning comparison.
 *  			This parameter can be ENABLE or DISABLE.
 * @param Unit Unit of TM_LowWtThre.
 *  			This parameter can be TM_DEG_C or TM_DEG_F.
 */
void TM_LowWtConfig(float TM_LowWtThre, u32 NewState, TM_UnitDef Unit)
{
	ADC_TypeDef *adc = ADC;
	u32 reg;
	u16 code;

	if (NewState == DISABLE) {
		return;
	}

	if (TrustZone_IsSecure()) {
		adc = ADC_S;
	}

	if (Unit == TM_DEG_C) {
		code = TM_GetTempCode(TM_LowWtThre);
	} else {
		/* F = C * 1.8 + 32 */
		code = TM_GetTempCode((TM_LowWtThre - 32) / 1.8f);
	}

	reg = adc->ADC_TM_TH_CTRL0;
	reg &= ~ADC_MASK_TM_LOW_WT_THR;
	reg |= ADC_TM_LOW_WT_THR(code);
	adc->ADC_TM_TH_CTRL0 = reg;
}

/**
 * @brief Get Celsius Degree.
 * @param Data Raw temperature data read from the thermal meter.
 * @return Temperature value of float type.
 */
float TM_GetCdegree(u32 Data)
{
	/* sync with HW: T = 0.480828 * ADC_data - 288.768 */
	return 0.480828f * Data - 288.768f;
}

/**
 * @brief Get Fahrenheit Degree.
 * @param Data Raw temperature data read from the thermal meter.
 * @return Temperature value of float type.
 */
float TM_GetFdegree(u32 Data)
{
	float Fdegree = 0;

	Fdegree = (TM_GetCdegree(Data) * 1.8f) + 32;

	return Fdegree;
}

/**
 * @brief Get temperature value according to the specified unit.
 * @param Data Raw temperature data read from the thermal meter.
 * @param Unit Temperature unit.
 *  			This parameter can be TM_DEG_C or TM_DEG_F.
 * @return Temperature value of float type.
 */
float TM_GetDegree(u32 Data, TM_UnitDef Unit)
{
	if (Unit == TM_DEG_C) {
		return TM_GetCdegree(Data);
	} else {
		return TM_GetFdegree(Data);
	}
}

/** @} */
/** @} */
/** @} */
