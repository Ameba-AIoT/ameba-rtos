/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "CODEC";
/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup AUDIO
* @brief AUDIO driver modules
* @{
*/

/** @defgroup AUDIO_CODEC
* @brief AUDIO_CODEC driver modules
* @{
*/

/* Exported functions ------------------------------------------------------------*/
/** @defgroup AUDIO_CODEC_Exported_Functions AUDIO_CODEC Exported Functions
  * @{
  */

/**
  * @brief  Get audio codec secure or non-secure base address.
  * @return  Audio codec secure or non-secure base address.
  */
AUDIO_TypeDef *AUDIO_CODEC_GetAddr(void)
{
	if (TrustZone_IsSecure()) {
		return (AUDIO_TypeDef *)AUDIO_REG_BASE_S;
	} else {
		return (AUDIO_TypeDef *)AUDIO_REG_BASE;
	}

}


/**
  * @brief  Get analog audio secure or non-secure base address.
  * @return  Analog audio secure or non-secure base address.
  */
AUD_TypeDef *AUDIO_AUD_GetAddr(void)
{
	if (TrustZone_IsSecure()) {
		return (AUD_TypeDef *)AUD_SYS_BASE_S;
	} else {
		return (AUD_TypeDef *)AUD_SYS_BASE;
	}
}


/**
  * @brief  Enable or disable audio codec IP.
  * @param  newstate: audio IP enable or disable.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetAudioIP(u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CODEC_AUDIO_CONTROL_0 |= AUD_BIT_AUDIO_IP_EN;
	} else {
		audio_base->CODEC_AUDIO_CONTROL_0 &= ~AUD_BIT_AUDIO_IP_EN;
	}
}


/**
  * @brief  Enable or disable I2S IP.
  * @param  i2s_sel: select I2S.
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  *			   @arg I2S1
  * @param  newstate: I2S IP enable or disable.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetI2SIP(u32 i2s_sel, u32 newstate)
{
	assert_param(IS_CODEC_I2S_SEL(i2s_sel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL |= AUD_BIT_I2S_x_RST_N_REG;
	} else {
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL &= ~ AUD_BIT_I2S_x_RST_N_REG;
	}
}

/**
  * @brief  Select I2S master source.
  * @param  i2s_sel: select I2S.
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  *			   @arg I2S1
  * @param  src: source
  *          This parameter can be one of the following values:
  *            @arg INTERNAL_SPORT
  *            @arg EXTERNAL_I2S
  * @return  None
  */
void AUDIO_CODEC_SetI2SSRC(u32 i2s_sel, u32 src)
{
	assert_param(IS_CODEC_I2S_SEL(i2s_sel));
	assert_param(IS_CODEC_I2S_MASTER_SRC(src));

	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (src == INTERNAL_SPORT) {
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL &= ~AUD_BIT_I2S_x_MASTER_SEL;
	} else {
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL |= AUD_BIT_I2S_x_MASTER_SEL;
	}
}

/**
  * @brief  Enable or disable ad/da ananlog clock.
  * @param  newstate: enable or disable dac/adc analog clock.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetANAClk(u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_DA_ANA_CLK_EN | AUD_BIT_AD_ANA_CLK_EN;
	} else {
		audio_base->CODEC_CLOCK_CONTROL_1 &= ~(AUD_BIT_DA_ANA_CLK_EN | AUD_BIT_AD_ANA_CLK_EN);
	}
}

/**
  * @brief  Select i2s rx and i2s rx tdm mode
  * @param  i2s_sel: select i2s: i2s0 or i2s1
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  *			   @arg I2S1
  * @param  tdmmode: select adc input path channel
  *			 This parameter can be one of the following values:
  *			   @arg I2S_NOTDM
  *			   @arg I2S_TDM4
  *			   @arg I2S_TDM6
  *			   @arg I2S_TDM8
  * @return  None
  */
void AUDIO_CODEC_SetI2SRXTDM(u32 i2s_sel, u32 tdmmode)
{
	assert_param(IS_CODEC_I2S_SEL(i2s_sel));
	assert_param(IS_CODEC_I2S_TDM_SEL(tdmmode));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL &= ~(AUD_MASK_I2S_x_TDM_MODE_RX);
	audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL |= AUD_I2S_x_TDM_MODE_RX(tdmmode);

	if (tdmmode == I2S_NOTDM) {
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL_1 &= ~(AUD_MASK_I2S_x_DATA_CH0_SEL_RX | AUD_MASK_I2S_x_DATA_CH1_SEL_RX |
				AUD_BIT_I2S_x_DATA_CH0_RX_DISABLE
				| AUD_BIT_I2S_x_DATA_CH1_RX_DISABLE);
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL_1 |= (AUD_I2S_x_DATA_CH0_SEL_RX(0) | AUD_I2S_x_DATA_CH1_SEL_RX(
					1) | AUD_BIT_I2S_x_DATA_CH2_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH3_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH4_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH5_RX_DISABLE |
				AUD_BIT_I2S_x_DATA_CH6_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH7_RX_DISABLE);
	} else if (tdmmode == I2S_TDM4) {
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL_1 &= ~(AUD_MASK_I2S_x_DATA_CH0_SEL_RX | AUD_MASK_I2S_x_DATA_CH1_SEL_RX |
				AUD_MASK_I2S_x_DATA_CH2_SEL_RX |
				AUD_MASK_I2S_x_DATA_CH3_SEL_RX | AUD_BIT_I2S_x_DATA_CH0_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH1_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH2_RX_DISABLE |
				AUD_BIT_I2S_x_DATA_CH3_RX_DISABLE);
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL_1 |= (AUD_I2S_x_DATA_CH0_SEL_RX(0) | AUD_I2S_x_DATA_CH1_SEL_RX(1) | AUD_I2S_x_DATA_CH2_SEL_RX(
					2) | AUD_I2S_x_DATA_CH3_SEL_RX(3) | AUD_BIT_I2S_x_DATA_CH4_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH5_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH6_RX_DISABLE |
				AUD_BIT_I2S_x_DATA_CH7_RX_DISABLE);
	} else if (tdmmode == I2S_TDM6) {
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL_1 &= ~(AUD_MASK_I2S_x_DATA_CH0_SEL_RX | AUD_MASK_I2S_x_DATA_CH1_SEL_RX |
				AUD_MASK_I2S_x_DATA_CH2_SEL_RX |
				AUD_MASK_I2S_x_DATA_CH3_SEL_RX | AUD_MASK_I2S_x_DATA_CH4_SEL_RX | AUD_MASK_I2S_x_DATA_CH5_SEL_RX | AUD_MASK_I2S_x_DATA_CH6_SEL_RX |
				AUD_MASK_I2S_x_DATA_CH7_SEL_RX | AUD_BIT_I2S_x_DATA_CH0_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH1_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH2_RX_DISABLE |
				AUD_BIT_I2S_x_DATA_CH3_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH4_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH5_RX_DISABLE);
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL_1 |= (AUD_I2S_x_DATA_CH0_SEL_RX(0) | AUD_I2S_x_DATA_CH1_SEL_RX(1) | AUD_I2S_x_DATA_CH2_SEL_RX(
					2) | AUD_I2S_x_DATA_CH3_SEL_RX(3) | AUD_I2S_x_DATA_CH4_SEL_RX(4) | AUD_I2S_x_DATA_CH5_SEL_RX(5) | AUD_BIT_I2S_x_DATA_CH6_RX_DISABLE |
				AUD_BIT_I2S_x_DATA_CH7_RX_DISABLE);
	} else {
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL_1 &= ~(AUD_MASK_I2S_x_DATA_CH0_SEL_RX | AUD_MASK_I2S_x_DATA_CH1_SEL_RX |
				AUD_MASK_I2S_x_DATA_CH2_SEL_RX |
				AUD_MASK_I2S_x_DATA_CH3_SEL_RX | AUD_MASK_I2S_x_DATA_CH4_SEL_RX | AUD_MASK_I2S_x_DATA_CH5_SEL_RX | AUD_MASK_I2S_x_DATA_CH6_SEL_RX |
				AUD_MASK_I2S_x_DATA_CH7_SEL_RX | AUD_BIT_I2S_x_DATA_CH0_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH1_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH2_RX_DISABLE |
				AUD_BIT_I2S_x_DATA_CH3_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH4_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH5_RX_DISABLE | AUD_BIT_I2S_x_DATA_CH6_RX_DISABLE |
				AUD_BIT_I2S_x_DATA_CH7_RX_DISABLE);
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL_1 |= (AUD_I2S_x_DATA_CH0_SEL_RX(0) | AUD_I2S_x_DATA_CH1_SEL_RX(1) | AUD_I2S_x_DATA_CH2_SEL_RX(
					2) | AUD_I2S_x_DATA_CH3_SEL_RX(3) | AUD_I2S_x_DATA_CH4_SEL_RX(4) | AUD_I2S_x_DATA_CH5_SEL_RX(5) | AUD_I2S_x_DATA_CH6_SEL_RX(6) | AUD_I2S_x_DATA_CH7_SEL_RX(7));
	}
}

/**
  * @brief  Default I2S initialization parameter.
  * @return  None
  */
void AUDIO_CODEC_I2S_StructInit(I2S_InitTypeDef *I2S_Struct)
{
	I2S_Struct->CODEC_SelI2STxSR = SR_48K;
	I2S_Struct->CODEC_SelI2STxWordLen = WL_16;
	I2S_Struct->CODEC_SelI2STxCHLen = CL_32;
	I2S_Struct->CODEC_SelI2STxCH = CH_LR;
	I2S_Struct->CODEC_SelI2STxDataFormat = DF_I2S;
	I2S_Struct->CODEC_SelI2SRxSR = SR_48K;
	I2S_Struct->CODEC_SelI2SRxWordLen = WL_16;
	I2S_Struct->CODEC_SelI2SRxCHLen = CL_32;
	I2S_Struct->CODEC_SelI2SRxDataFormat = DF_I2S;
	I2S_Struct->CODEC_SelRxI2STdm = I2S_NOTDM;

}

/**
  * @brief  Set I2S initialization parameter.
  * @param  i2s_sel: select i2s: i2s0 or i2s1
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  *			   @arg I2S1
  * @param  type: audio work path.
  *			 This parameter can be one of the following values:
  *			   @arg DACPATH
  *			   @arg ADCPATH
  * @return  None
  */
void AUDIO_CODEC_SetI2SParameters(u32 i2s_sel, u32 path, I2S_InitTypeDef *I2S_Struct)
{
	assert_param(IS_CODEC_I2S_SEL(i2s_sel));
	assert_param(IS_CODEC_WORK_PATH(path));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (path == DACPATH) {
		assert_param(IS_CODEC_CL_SEL(I2S_Struct->CODEC_SelI2STxCHLen));
		assert_param(IS_CODEC_WL_SEL(I2S_Struct->CODEC_SelI2STxWordLen));
		assert_param(IS_CODEC_DATA_FMT(I2S_Struct->CODEC_SelI2STxDataFormat));
		assert_param(IS_CODEC_TXCHN_SEL(I2S_Struct->CODEC_SelI2STxCH));

		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL &= ~(AUD_MASK_I2S_x_CH_LEN_SEL_TX | AUD_MASK_I2S_x_DATA_LEN_SEL_TX |
				AUD_MASK_I2S_x_DATA_FORMAT_SEL_TX | AUD_MASK_I2S_x_DATA_CH_SEL_TX);
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL |= (AUD_I2S_x_CH_LEN_SEL_TX(I2S_Struct->CODEC_SelI2STxCHLen) | AUD_I2S_x_DATA_LEN_SEL_TX(
					I2S_Struct->CODEC_SelI2STxWordLen) | AUD_I2S_x_DATA_FORMAT_SEL_TX(I2S_Struct->CODEC_SelI2STxDataFormat) | AUD_I2S_x_DATA_CH_SEL_TX(
					I2S_Struct->CODEC_SelI2STxCH));
	} else {
		assert_param(IS_CODEC_CL_SEL(I2S_Struct->CODEC_SelI2SRxCHLen));
		assert_param(IS_CODEC_WL_SEL(I2S_Struct->CODEC_SelI2SRxWordLen));
		assert_param(IS_CODEC_DATA_FMT(I2S_Struct->CODEC_SelI2SRxDataFormat));
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL &= ~(AUD_MASK_I2S_x_CH_LEN_SEL_RX | AUD_MASK_I2S_x_DATA_LEN_SEL_RX |
				AUD_MASK_I2S_x_DATA_FORMAT_SEL_RX);
		audio_base->CODEC_I2S_SRC_CTRL[i2s_sel].CODEC_I2S_x_CONTROL |= (AUD_I2S_x_CH_LEN_SEL_RX(I2S_Struct->CODEC_SelI2SRxCHLen) | AUD_I2S_x_DATA_LEN_SEL_RX(
					I2S_Struct->CODEC_SelI2SRxWordLen) | AUD_I2S_x_DATA_FORMAT_SEL_RX(I2S_Struct->CODEC_SelI2SRxDataFormat));
		AUDIO_CODEC_SetI2SRXTDM(i2s_sel, I2S_Struct->CODEC_SelRxI2STdm);
	}
}

/**
  * @brief  Select adc sample rate and source, all adc share souce and sample rate.
  * @param  src: source
  *          This parameter can be one of the following values:
  *            @arg SOURCE0
  *            @arg SOURCE1
  *            @arg SOURCE2
  * @param  sr: sample rate
  * @return  None
  */
void AUDIO_CODEC_SetADCSRSrc(u32 src, u32 sr)
{
	assert_param(IS_CODEC_SR_SRC(src));
	assert_param(IS_CODEC_SR_SEL(sr));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->CODEC_CLOCK_CONTROL_5 &= ~(AUD_MASK_ADC_0_FS_SRC_SEL | AUD_MASK_ADC_1_FS_SRC_SEL | AUD_MASK_ADC_2_FS_SRC_SEL | AUD_MASK_ADC_3_FS_SRC_SEL
										   | AUD_MASK_ADC_4_FS_SRC_SEL | AUD_MASK_ADC_5_FS_SRC_SEL | AUD_MASK_ADC_6_FS_SRC_SEL | AUD_MASK_ADC_7_FS_SRC_SEL);
	audio_base->CODEC_CLOCK_CONTROL_5 |= AUD_ADC_0_FS_SRC_SEL(src) | AUD_ADC_1_FS_SRC_SEL(src) | AUD_ADC_2_FS_SRC_SEL(src) | AUD_ADC_3_FS_SRC_SEL(src)
										 | AUD_ADC_4_FS_SRC_SEL(src) | AUD_ADC_5_FS_SRC_SEL(src) | AUD_ADC_6_FS_SRC_SEL(src) | AUD_ADC_7_FS_SRC_SEL(src);

	if (src == SOURCE0) {
		audio_base->CODEC_CLOCK_CONTROL_4 &= ~ AUD_MASK_SAMPLE_RATE_0;
		audio_base->CODEC_CLOCK_CONTROL_4 |= AUD_SAMPLE_RATE_0(sr) | AUD_BIT_SAMPLE_RATE_0_EN;
	} else if (src == SOURCE1) {
		audio_base->CODEC_CLOCK_CONTROL_4 &= ~ AUD_MASK_SAMPLE_RATE_1;
		audio_base->CODEC_CLOCK_CONTROL_4 |= AUD_SAMPLE_RATE_1(sr) | AUD_BIT_SAMPLE_RATE_1_EN;
	} else {
		audio_base->CODEC_CLOCK_CONTROL_4 &= ~ AUD_MASK_SAMPLE_RATE_2;
		audio_base->CODEC_CLOCK_CONTROL_4 |= AUD_SAMPLE_RATE_2(sr) | AUD_BIT_SAMPLE_RATE_2_EN;
	}
}

/**
  * @brief  Select dac sample rate and source .
  * @param  src: source
  *          This parameter can be one of the following values:
  *            @arg SOURCE0
  *            @arg SOURCE1
  *            @arg SOURCE2
  * @param  sr: sample rate
  * @return  None
  */
void AUDIO_CODEC_SetDACSRSrc(u32 src, u32 sr)
{
	assert_param(IS_CODEC_SR_SRC(src));
	assert_param(IS_CODEC_SR_SEL(sr));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->CODEC_CLOCK_CONTROL_5 &= ~ AUD_MASK_DAC_FS_SRC_SEL;
	audio_base->CODEC_CLOCK_CONTROL_5 |= AUD_DAC_FS_SRC_SEL(src);

	if (src == SOURCE0) {
		audio_base->CODEC_CLOCK_CONTROL_4 &= ~ AUD_MASK_SAMPLE_RATE_0;
		audio_base->CODEC_CLOCK_CONTROL_4 |= AUD_SAMPLE_RATE_0(sr) | AUD_BIT_SAMPLE_RATE_0_EN;
	} else if (src == SOURCE1) {
		audio_base->CODEC_CLOCK_CONTROL_4 &= ~ AUD_MASK_SAMPLE_RATE_1;
		audio_base->CODEC_CLOCK_CONTROL_4 |= AUD_SAMPLE_RATE_1(sr) | AUD_BIT_SAMPLE_RATE_1_EN;
	} else {
		audio_base->CODEC_CLOCK_CONTROL_4 &= ~ AUD_MASK_SAMPLE_RATE_2;
		audio_base->CODEC_CLOCK_CONTROL_4 |= AUD_SAMPLE_RATE_2(sr) | AUD_BIT_SAMPLE_RATE_2_EN;
	}
}

/**
  * @brief  Disable audio pad according to the work path.
  * @param  path: audio work path.
  *			 This parameter can be one of the following values:
  *			   @arg PAD_DACL
  *			   @arg PAD_DACR
  *			   @arg PAD_MIC1
  *			   @arg PAD_MIC2
  *			   @arg PAD_MIC3
  *			   @arg PAD_MIC4
  *			   @arg PAD_MIC5
  * @return  None
  */
void AUDIO_CODEC_DisPAD(u32 path)
{
	u32 Tmp;
	u32 base = PINMUX_REG_BASE;

	Tmp = HAL_READ32(base, REG_PAD_AUD_PAD_CTRL);

	switch (path) {
	case PAD_DACL:

		Pinmux_Config(_PB_3, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PB_4, PINMUX_FUNCTION_AUDIO);
		Tmp &= ~(BIT(17) | BIT(18));
		break;

	case PAD_DACR:

		Pinmux_Config(_PB_5, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PB_6, PINMUX_FUNCTION_AUDIO);
		Tmp &= ~(BIT(19) | BIT(20));
		break;

	case PAD_MIC1:

		Pinmux_Config(_PA_20, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PA_21, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PA_30, PINMUX_FUNCTION_AUDIO);
		Tmp &= ~(BIT(2) | BIT(3) | BIT(12));
		break;

	case PAD_MIC2:

		Pinmux_Config(_PA_22, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PA_23, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PA_31, PINMUX_FUNCTION_AUDIO);
		Tmp &= ~(BIT(4) | BIT(5) | BIT(13));
		break;

	case PAD_MIC3:

		Pinmux_Config(_PA_24, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PA_25, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PB_0, PINMUX_FUNCTION_AUDIO);
		Tmp &= ~(BIT(6) | BIT(7) | BIT(14));
		break;

	case PAD_MIC4:

		Pinmux_Config(_PA_26, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PA_27, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PB_1, PINMUX_FUNCTION_AUDIO);
		Tmp &= ~(BIT(8) | BIT(9) | BIT(15));
		break;

	case PAD_MIC5:

		Pinmux_Config(_PA_28, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PA_29, PINMUX_FUNCTION_AUDIO);
		Pinmux_Config(_PB_2, PINMUX_FUNCTION_AUDIO);
		Tmp &= ~(BIT(10) | BIT(11) | BIT(16));
		break;
	}

	HAL_WRITE32(base, REG_PAD_AUD_PAD_CTRL, Tmp);
}

/**
  * @brief Get actual calibration value.
  * @param audio_efuse_value: data read from otp
  * @retval actual calibration value.
  */
static u8 AUDIO_CODEC_Calib_Get_Value(u8 audio_efuse_value)
{
	return (u8)(AUDIO_CAL_5BIT_VALUE(audio_efuse_value));
}

/**
  * @brief Load AUDIO_VREF calibration value from OTP to register.
  * @retval none
  */
static void AUDIO_CODEC_VREF_Calib(void)
{
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	u32 Tmp;

	u8 Audio_Vref_F_Value;
	u8 Audio_Vref_S_Value;
	u8 audio_vref_f_data = 0;
	u8 audio_vref_s_data = 0;

	OTP_Read8(VOL_AUDIO_VREF_1V8_F, &audio_vref_f_data);
	OTP_Read8(VOL_AUDIO_VREF_1V8_S, &audio_vref_s_data);

	/* Write AUDIO_VREF */
	Tmp = AUD->AUD_MICBST_CTL1;

	if ((audio_vref_s_data & 0x1F) != 0x1F) {
		Tmp &= ~(AUD_MASK_LDO_TUNE);
		Audio_Vref_S_Value = AUDIO_CODEC_Calib_Get_Value(audio_vref_s_data);
		Tmp |= AUD_LDO_TUNE(Audio_Vref_S_Value);
		AUD->AUD_MICBST_CTL1 = Tmp;
	} else if ((audio_vref_f_data & 0x1F) != 0x1F) {
		Tmp &= ~(AUD_MASK_LDO_TUNE);
		Audio_Vref_F_Value = AUDIO_CODEC_Calib_Get_Value(audio_vref_f_data);
		Tmp |= AUD_LDO_TUNE(Audio_Vref_F_Value);
		AUD->AUD_MICBST_CTL1 = Tmp;
	} else {
		return;
	}

}

/**
  * @brief Audio Codec LDO power on or power down.
  * @param  ldo_mode: LDO power on or power down
  *          This parameter can be one of the following values:
  *            @arg POWER_ON:
  *            @arg POWER_DOWN:
  * @return  None
  */
void AUDIO_CODEC_SetLDOMode(u32 powermode)
{
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();
	assert_param(IS_CODEC_POWER_SEL(powermode));

	if (powermode == POWER_ON) {
		AUD->AUD_ADDA_CTL |= AUD_BIT_POWADDACK;
		/*LDO power on*/
		AUD->RSVD0 &= ~AUD_BIT_LDO_CODEC_DISCHARGE_PATH;
		AUD->AUD_MICBIAS_CTL0 |= (AUD_BIT_MBIAS_POW);
		AUD->AUD_MICBST_CTL1 |= AUD_BIT_LDO_POW | AUD_BIT_LDO_PREC;
		DelayMs(2);
		AUD->AUD_MICBST_CTL1 &= ~(AUD_BIT_LDO_PREC);
		DelayUs(10);
		AUD->AUD_MICBST_CTL1 |= AUD_BIT_LDO_POW_0P9V;
		DelayUs(990);
		AUD->RSVD0 &= ~AUD_BIT_LDO_CODEC_DRIVING_PATH;
		AUDIO_CODEC_VREF_Calib();

	} else {
		AUD->AUD_ADDA_CTL &= ~AUD_BIT_POWADDACK;
		AUD->AUD_MICBST_CTL1 &= ~(AUD_BIT_LDO_POW_0P9V);
		DelayUs(10);
		AUD->AUD_MICBST_CTL1 &= ~(AUD_BIT_LDO_POW);
		AUD->AUD_MICBST_CTL1 |= AUD_BIT_LDO_PREC;
		AUD->RSVD0 |= AUD_BIT_LDO_CODEC_DISCHARGE_PATH;
		DelayMs(9);
		AUD->AUD_MICBST_CTL1 &= ~(AUD_BIT_LDO_PREC);
		AUD->RSVD0 &= ~AUD_BIT_LDO_CODEC_DISCHARGE_PATH;
		AUD->AUD_MICBIAS_CTL0 &= ~(AUD_BIT_MBIAS_POW);
	}
}

static u32 AUDIO_CODEC_GetAdFifoEnBitForCh(u32 ad_chn)
{
	u32 en_bit = 0;
	switch (ad_chn)
	{
	case ADCHN1:
		en_bit = AUD_BIT_AD_0_FIFO_EN;
		break;
	case ADCHN2:
		en_bit = AUD_BIT_AD_1_FIFO_EN;
		break;
	case ADCHN3:
		en_bit = AUD_BIT_AD_2_FIFO_EN;
		break;
	case ADCHN4:
		en_bit = AUD_BIT_AD_3_FIFO_EN;
		break;
	case ADCHN5:
		en_bit = AUD_BIT_AD_4_FIFO_EN;
		break;
	case ADCHN6:
		en_bit = AUD_BIT_AD_5_FIFO_EN;
		break;
	case ADCHN7:
		en_bit = AUD_BIT_AD_6_FIFO_EN;
		break;
	case ADCHN8:
		en_bit = AUD_BIT_AD_7_FIFO_EN;
		break;
	default:
		break;
	}

	return en_bit;
}

/**
  * @brief  Enable per AD and AD fifo channel clock by mask.
  * @param  ad_chn_mask: ad channel mask.
  * The correspondence between bits and channels is as follows:
  * bit7   bit6   bit5   bit4   bit3   bit2   bit1   bit0
  *   |     |      |      |      |      |      |      |
  * ADCHN8 ADCHN7 ADCHN6 ADCHN5 ADCHN4 ADCHN3 ADCHN2 ADCHN1
  * @return  None
  */
void AUDIO_CODEC_EnableADCFifoForMask(u32 ad_chn_mask)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	u32 or_tmp = 0;

	for (u32 i = ADCHN1; i < (ADCHN8 + 1); i++) {
		if ((ad_chn_mask >> (i - 1)) & 1) {
			or_tmp |= AUDIO_CODEC_GetAdFifoEnBitForCh(i);
		}
	}

	audio_base->CODEC_CLOCK_CONTROL_1 |= or_tmp;
}

/**
  * @brief  Enable or disable per AD and AD fifo channel clock.
  * @param  ad_chn: select ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  *			   @arg ADCHN3
  *			   @arg ADCHN4
  *			   @arg ADCHN5
  *			   @arg ADCHN6
  *			   @arg ADCHN7
  *			   @arg ADCHN8
  * @param  newstate: enable or disable per ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableADCFifo(u32 ad_chn, u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));

	switch (ad_chn) {

	case ADCHN1:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_0_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_0_FIFO_EN;
		}
		break;
	case ADCHN2:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_1_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_1_FIFO_EN;
		}
		break;
	case ADCHN3:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_2_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_2_FIFO_EN;
		}
		break;
	case ADCHN4:

		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_3_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_3_FIFO_EN;
		}
		break;

	case ADCHN5:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_4_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_4_FIFO_EN;
		}
		break;
	case ADCHN6:

		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_5_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_5_FIFO_EN;
		}
		break;
	case ADCHN7:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_6_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_6_FIFO_EN;
		}
		break;
	case ADCHN8:

		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_7_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_7_FIFO_EN;
		}
		break;
	default:
		break;

	}
}

static u32 AUDIO_CODEC_GetAdEnBitForCh(u32 ad_chn)
{
	u32 en_bit = 0;
	switch (ad_chn)
	{
	case ADCHN1:
		en_bit = AUD_BIT_AD_0_EN;
		break;
	case ADCHN2:
		en_bit = AUD_BIT_AD_1_EN;
		break;
	case ADCHN3:
		en_bit = AUD_BIT_AD_2_EN;
		break;
	case ADCHN4:
		en_bit = AUD_BIT_AD_3_EN;
		break;
	case ADCHN5:
		en_bit = AUD_BIT_AD_4_EN;
		break;
	case ADCHN6:
		en_bit = AUD_BIT_AD_5_EN;
		break;
	case ADCHN7:
		en_bit = AUD_BIT_AD_6_EN;
		break;
	case ADCHN8:
		en_bit = AUD_BIT_AD_7_EN;
		break;
	default:
		break;
	}

	return en_bit;
}

/**
  * @brief  Enable per AD and AD fifo channel clock by mask.
  * @param  ad_chn_mask: ad channel mask.
  * The correspondence between bits and channels is as follows:
  * bit7   bit6   bit5   bit4   bit3   bit2   bit1   bit0
  *   |     |      |      |      |      |      |      |
  * ADCHN8 ADCHN7 ADCHN6 ADCHN5 ADCHN4 ADCHN3 ADCHN2 ADCHN1
  *
  * @return  None
  */
void AUDIO_CODEC_EnableADCForMask(u32 ad_chn_mask)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	u32 or_tmp = 0;

	for (u32 i = ADCHN1; i < (ADCHN8 + 1); i++) {
		if ((ad_chn_mask >> (i - 1)) & 1) {
			or_tmp |= AUDIO_CODEC_GetAdEnBitForCh(i);
		}
	}

	audio_base->CODEC_CLOCK_CONTROL_1 |= or_tmp;

}

/**
  * @brief  Enable or disable per AD and AD fifo channel clock.
  * @param  ad_chn: select ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  *			   @arg ADCHN3
  *			   @arg ADCHN4
  *			   @arg ADCHN5
  *			   @arg ADCHN6
  *			   @arg ADCHN7
  *			   @arg ADCHN8
  * @param  newstate: enable or disable per ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableADC(u32 ad_chn, u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));

	switch (ad_chn) {

	case ADCHN1:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_0_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_0_EN;
		}

		break;
	case ADCHN2:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_1_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_1_EN;
		}

		break;
	case ADCHN3:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_2_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_2_EN;
		}

		break;
	case ADCHN4:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_3_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_3_EN;
		}

		break;

	case ADCHN5:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_4_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_4_EN;
		}

		break;
	case ADCHN6:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_5_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_5_EN;
		}

		break;
	case ADCHN7:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_6_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_6_EN;
		}

		break;
	case ADCHN8:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_7_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~ AUD_BIT_AD_7_EN;
		}

		break;
	default:
		break;
	}
}


/**
  * @brief  Set the gain of ADC digital volume.
  * @param  adc_sel: select adc number.
  *          This parameter can be one of the following values:
  *            @arg ADC1
  *            @arg ADC2
  *            @arg ADC3
  *            @arg ADC4
  *            @arg ADC5
  *            @arg ADC6
  *            @arg ADC7
  *            @arg ADC8
  * @param  Gain: the value of gain.
  *          This parameter can be -17.625dB~48dB in 0.375dB step
  *            @arg 0x00 -17.625dB
  *            @arg 0xaf 48dB
  * @retval None
  */
void AUDIO_CODEC_SetADCVolume(u32 adc_sel, u32 gain)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_1 &= ~AUD_MASK_ADC_x_AD_GAIN;
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_1 |= AUD_ADC_x_AD_GAIN(gain) ;
}


/**
  * @brief  Set per ADC channel HPF mode and select HPF FC
  * @param  adc_sel: select adc channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  *			   @arg ADC3
  *			   @arg ADC4
  *			   @arg ADC5
  *			   @arg ADC6
  *			   @arg ADC7
  *			   @arg ADC8
  * @param  fc: select high pass filter fc
  *			 This parameter can be one of the following values:
  *			   @arg 0：5e10^-3 Fs
  *			   @arg 1：2.5e10^-3 Fs
  *			   @arg 2：1.125e10^-3 Fs
  *			   @arg 3: 6.25e10^-4 Fs
  *			   @arg 4: 3.125e10^-4 Fs
  *			   @arg 5: 1.5625e10^-4 Fs
  *			   @arg 6: 7.8125e10^-5 Fs
  *			   @arg 7: 3.90625e10^-5 Fs
  * @param  newstate: enable or disable per ad channel HPF mode
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCHPF(u32 adc_sel, u32 fc, u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	if (newstate == ENABLE) {
		audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_MASK_ADC_x_DCHPF_FC_SEL;
		audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 |= AUD_BIT_ADC_x_DCHPF_EN | AUD_ADC_x_DCHPF_FC_SEL(fc);
	} else {
		audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_BIT_ADC_x_DCHPF_EN;
	}
}

/**
  * @brief  Select I2S and set ADC ASRC mode according to the sample rate.
  * @param  i2s_sel: select i2s.
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  *			   @arg I2S1
  * @param  adc_sel: select adc channel.
  *          This parameter can be one of the following values:
  *            @arg ADC1
  *            @arg ADC2
  *            @arg ADC3
  *            @arg ADC4
  *            @arg ADC5
  *            @arg ADC6
  *            @arg ADC7
  *            @arg ADC8
  * @param  sr: the value of sample rate.
  * @param  newstate: the value of tdm mode.
  *			 This parameter can be one of the following values:
  *			   @arg DISABLE
  *			   @arg ENABLE
  * @retval None
  */
void AUDIO_CODEC_SetADCASRC(u32 i2s_sel, u32 adc_sel, u32 sr, u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	assert_param(IS_CODEC_I2S_SEL(i2s_sel));
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	if (sr == SR_96K || sr == SR_88P2K) {
		if (i2s_sel == I2S0) {
			audio_base->CODEC_ASRC_CONTROL_2 &= ~AUD_MASK_ASRC_RATE_SEL_RX_0;
			audio_base->CODEC_ASRC_CONTROL_2 |= AUD_ASRC_RATE_SEL_RX_0(1);
		} else {
			audio_base->CODEC_ASRC_CONTROL_4 &= ~AUD_MASK_ASRC_RATE_SEL_RX_1;
			audio_base->CODEC_ASRC_CONTROL_4 |= AUD_ASRC_RATE_SEL_RX_1(1);
		}
	} else {
		if (i2s_sel == I2S0) {
			audio_base->CODEC_ASRC_CONTROL_2 &= ~AUD_MASK_ASRC_RATE_SEL_RX_0;
			audio_base->CODEC_ASRC_CONTROL_2 |= AUD_ASRC_RATE_SEL_RX_0(0);
		} else {
			audio_base->CODEC_ASRC_CONTROL_4 &= ~AUD_MASK_ASRC_RATE_SEL_RX_1;
			audio_base->CODEC_ASRC_CONTROL_4 |= AUD_ASRC_RATE_SEL_RX_1(0);
		}
	}

	switch (adc_sel) {

	case ADC1:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_6 |= AUD_BIT_ADC_0_ASRC_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_6 &= ~AUD_BIT_ADC_0_ASRC_EN;
		}
		break;

	case ADC2:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_6 |= AUD_BIT_ADC_1_ASRC_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_6 &= ~AUD_BIT_ADC_1_ASRC_EN;
		}
		break;

	case ADC3:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_6 |= AUD_BIT_ADC_2_ASRC_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_6 &= ~AUD_BIT_ADC_2_ASRC_EN;
		}
		break;

	case ADC4:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_6 |= AUD_BIT_ADC_3_ASRC_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_6 &= ~AUD_BIT_ADC_3_ASRC_EN;
		}
		break;

	case ADC5:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_6 |= AUD_BIT_ADC_4_ASRC_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_6 &= ~AUD_BIT_ADC_4_ASRC_EN;
		}
		break;

	case ADC6:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_6 |= AUD_BIT_ADC_5_ASRC_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_6 &= ~AUD_BIT_ADC_5_ASRC_EN;
		}
		break;

	case ADC7:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_6 |= AUD_BIT_ADC_6_ASRC_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_6 &= ~AUD_BIT_ADC_6_ASRC_EN;
		}
		break;

	case ADC8:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_6 |= AUD_BIT_ADC_7_ASRC_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_6 &= ~AUD_BIT_ADC_7_ASRC_EN;
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Mute ADC path.
  * @param  adc_sel: select adc channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  *			   @arg ADC3
  *			   @arg ADC4
  *			   @arg ADC5
  *			   @arg ADC6
  *			   @arg ADC7
  *			   @arg ADC8
  * @param  newstate: mute or unmute option for ad channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE
  *			   @arg MUTE
  * @return  None
  */
void AUDIO_CODEC_SetADCMute(u32 adc_sel, u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	if (newstate == MUTE) {
		audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 |= AUD_BIT_ADC_x_AD_MUTE;
	} else {
		audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_BIT_ADC_x_AD_MUTE;
	}
}


/**
  * @brief  Set per adc mix mute type and state.
  * @param  adc_num: select adc input path channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  *			   @arg ADC3
  *			   @arg ADC4
  *			   @arg ADC5
  *			   @arg ADC6
  *			   @arg ADC7
  *			   @arg ADC8
  * @param  type: adc path input select
  *			 This parameter can be one of the following values:
  *			   @arg DMIC
  *			   @arg ANAAD
  * @param  newstate: mute or unmute option for ad channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE
  *			   @arg MUTE
  * @return  None
  */
void AUDIO_CODEC_SetADCMixMute(u32 adc_num, u32 type,  			      u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_num));
	assert_param(IS_CODEC_ADC_MIXMUTE_SEL(type));
	assert_param(IS_CODEC_MUTE_CTR(newstate));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == UNMUTE) {
		if (type == DMIC) {
			audio_base->CODEC_ADC_CH_CTRL[adc_num - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_BIT_ADC_x_DMIC_MIX_MUTE;
		} else {
			audio_base->CODEC_ADC_CH_CTRL[adc_num - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_BIT_ADC_x_AD_MIX_MUTE;
		}
	} else {
		if (type == DMIC) {
			audio_base->CODEC_ADC_CH_CTRL[adc_num - 1].CODEC_ADC_x_CONTROL_0 |= AUD_BIT_ADC_x_DMIC_MIX_MUTE;
		} else {
			audio_base->CODEC_ADC_CH_CTRL[adc_num - 1].CODEC_ADC_x_CONTROL_0 |= AUD_BIT_ADC_x_AD_MIX_MUTE;
		}
	}
}

/**
  * @brief  Select analog ad source for per ad channel .
  * @param  ad_chn: select per analog adc channel src.
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  *			   @arg ADCHN3
  *			   @arg ADCHN4
  *			   @arg ADCHN5
  *			   @arg ADCHN6
  *			   @arg ADCHN7
  *			   @arg ADCHN8
  * @param  amic_num: select per analog adc channel src.
  *			 This parameter can be one of the following values:
  *			   @arg AMIC1
  *			   @arg AMIC2
  *			   @arg AMIC3
  *			   @arg AMIC4
  *			   @arg AMIC5
  * @return  None
  */
void AUDIO_CODEC_SetADCANASrc(u32 ad_chn, u32 amic_num)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_AMIC_SEL(amic_num));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->CODEC_ADC_CH_CTRL[ad_chn - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_MASK_ADC_x_AD_SRC_SEL;
	audio_base->CODEC_ADC_CH_CTRL[ad_chn - 1].CODEC_ADC_x_CONTROL_0 |= AUD_ADC_x_AD_SRC_SEL(amic_num - 1);
}


/**
  * @brief  Enable or disable per adc ananlog ad filter.
  * @param  adc_num: select ad ana channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  *			   @arg ADC3
  *			   @arg ADC4
  *			   @arg ADC5
  *			   @arg ADC6
  *			   @arg ADC7
  *			   @arg ADC8
  * @param  newstate: enable or disable per ad ana channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCANAFilter(u32 adc_num, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_num));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (adc_num) {

	case ADC1:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_ANA_0_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_ANA_0_EN;
		}
		break;

	case ADC2:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_ANA_1_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_ANA_1_EN;
		}

		break;

	case ADC3:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_ANA_2_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_ANA_2_EN;
		}

		break;

	case ADC4:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_ANA_3_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_ANA_3_EN;
		}

		break;

	case ADC5:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_ANA_4_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_ANA_4_EN;
		}

		break;

	case ADC6:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_ANA_5_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_ANA_5_EN;
		}

		break;

	case ADC7:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_ANA_6_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_ANA_6_EN;
		}

		break;

	case ADC8:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_ANA_7_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_ANA_7_EN;
		}

		break;

	default:
		break;

	}
}

/**
  * @brief Audio Codec micbias power on or power down.
  * @param  micbias_mode: micbias power on or power down
  *          This parameter can be one of the following values:
  *            @arg POWER_ON:
  *            @arg POWER_DOWN:
  * @return  None
  */
void AUDIO_CODEC_SetMicBiasPowerMode(u32 powermode)
{
	assert_param(IS_CODEC_POWER_SEL(powermode));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (powermode == POWER_ON) {
		AUD->AUD_MICBIAS_CTL0 |= AUD_BIT_MICBIAS1_POW;
		AUD->AUD_MICBIAS_CTL0 &= ~ AUD_MASK_MICBIAS1_COMP;
		AUD->AUD_MICBIAS_CTL0 |= AUD_MICBIAS1_COMP(1);
		audio_base->CODEC_AUDIO_CONTROL_1 |= AUD_BIT_CKX_MICBIAS_EN;
		AUD->AUD_MICBIAS_CTL0 &= ~(AUD_MASK_MICBIAS1_VSET);
		AUD->AUD_MICBIAS_CTL0 |= AUD_MICBIAS1_VSET(7);
		AUD->AUD_MBIAS_CTL1 &= ~AUD_MASK_MBIAS_ISEL_MICBIAS1;
		AUD->AUD_MBIAS_CTL1 |= AUD_MBIAS_ISEL_MICBIAS1(2);
	} else {
		AUD->AUD_MICBIAS_CTL0 &= ~ AUD_BIT_MICBIAS1_POW;
	}
}

/**
  * @brief  Set micbst power on or down.
  * @param  amic_num: select amic channel
  *			 This parameter can be one of the following values:
  *			   @arg AMIC1
  *			   @arg AMIC2
  *			   @arg AMIC3
  *			   @arg AMIC4
  *			   @arg AMIC5
  * @param  powermode: power on or down mode
  *			 This parameter can be one of the following values:
  *			   @arg NORMALPOWER
  *			   @arg LOWPOWER
  *			   @arg SHUTDOWN
  * @return  None
  */
void AUDIO_CODEC_SetMicBstPowerMode(u32 amic_num, u32 powermode)
{
	assert_param(IS_CODEC_AMIC_SEL(amic_num));
	assert_param(IS_CODEC_POWER_MODE_SEL(powermode));
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (powermode == NORMALPOWER) {
		AUD->AUD_DTS_CTL &= ~AUD_BIT_LPMODE_EN;
		if (amic_num == AMIC1) {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST_POWL;
			AUD->AUD_MBIAS_CTL1 &= ~(AUD_MASK_MBIAS_ISEL_MICBST_L);
			AUD->AUD_MBIAS_CTL1 |= AUD_MBIAS_ISEL_MICBST_L(2);
			AUD->AUD_ADDA_CTL |= AUD_BIT_DTSDM_CKXEN | AUD_BIT_DTSDM_POW_L;
			AUD->AUD_MBIAS_CTL1 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM_L | AUD_MASK_MBIAS_ISEL_DTSDM_INT1_L);
			AUD->AUD_MBIAS_CTL1 |= AUD_MBIAS_ISEL_DTSDM_L(2) | AUD_MBIAS_ISEL_DTSDM_INT1_L(4);
		} else if (amic_num == AMIC2) {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST_POWR;
			AUD->AUD_MBIAS_CTL1 &= ~(AUD_MASK_MBIAS_ISEL_MICBST_R);
			AUD->AUD_MBIAS_CTL1 |= AUD_MBIAS_ISEL_MICBST_R(2);
			AUD->AUD_ADDA_CTL |= AUD_BIT_DTSDM_CKXEN | AUD_BIT_DTSDM_POW_R;
			AUD->AUD_MBIAS_CTL1 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM_R | AUD_MASK_MBIAS_ISEL_DTSDM_INT1_R);
			AUD->AUD_MBIAS_CTL1 |= AUD_MBIAS_ISEL_DTSDM_R(2) | AUD_MBIAS_ISEL_DTSDM_INT1_R(4);
		} else if (amic_num == AMIC3) {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST2_POWL;
			AUD->AUD_MBIAS_CTL2 &= ~AUD_MASK_MBIAS_ISEL_MICBST2_L;
			AUD->AUD_MBIAS_CTL2 |= AUD_MBIAS_ISEL_MICBST2_L(2);
			AUD->AUD_DTS_CTL |= AUD_BIT_DTSDM2_CKXEN | AUD_BIT_DTSDM2_POW_L;
			AUD->AUD_MBIAS_CTL0 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM2_L | AUD_MASK_MBIAS_ISEL_DTSDM2_INT1_L);
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_DTSDM2_L(2) | AUD_MBIAS_ISEL_DTSDM2_INT1_L(4);
		} else if (amic_num == AMIC4) {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST2_POWR;
			AUD->AUD_MBIAS_CTL2 &= ~AUD_MASK_MBIAS_ISEL_MICBST2_R;
			AUD->AUD_MBIAS_CTL2 |= AUD_MBIAS_ISEL_MICBST2_R(2);
			AUD->AUD_DTS_CTL |= AUD_BIT_DTSDM2_CKXEN | AUD_BIT_DTSDM2_POW_R;
			AUD->AUD_MBIAS_CTL0 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM2_R | AUD_MASK_MBIAS_ISEL_DTSDM2_INT1_R);
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_DTSDM2_R(2) | AUD_MBIAS_ISEL_DTSDM2_INT1_R(4);
		} else {
			AUD->AUD_MICBST_CTL1 |= AUD_BIT_MICBST3_POW;
			AUD->AUD_MBIAS_CTL2 &= ~AUD_MASK_MBIAS_ISEL_MICBST3;
			AUD->AUD_MBIAS_CTL2 |= AUD_MBIAS_ISEL_MICBST3(2);
			AUD->AUD_DTS_CTL |= AUD_BIT_DTSDM3_CKXEN | AUD_BIT_DTSDM3_POW;
			AUD->AUD_MBIAS_CTL0 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM3 | AUD_MASK_MBIAS_ISEL_DTSDM3_INT1);
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_DTSDM3(2) | AUD_MBIAS_ISEL_DTSDM3_INT1(4);
		}
	} else if (powermode == LOWPOWER) {
		AUD->AUD_DTS_CTL |= AUD_BIT_LPMODE_EN;

		if (amic_num == AMIC1) {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST_POWL;
			AUD->AUD_MBIAS_CTL1 &= ~(AUD_MASK_MBIAS_ISEL_MICBST_L);
			AUD->AUD_MBIAS_CTL1 |= AUD_MBIAS_ISEL_MICBST_L(2);
			AUD->AUD_ADDA_CTL |= AUD_BIT_DTSDM_CKXEN | AUD_BIT_DTSDM_POW_L;
			AUD->AUD_MBIAS_CTL1 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM_L | AUD_MASK_MBIAS_ISEL_DTSDM_INT1_L);
			AUD->AUD_MBIAS_CTL1 |= AUD_MBIAS_ISEL_DTSDM_L(2) | AUD_MBIAS_ISEL_DTSDM_INT1_L(4);
		} else if (amic_num == AMIC2) {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST_POWR;
			AUD->AUD_MBIAS_CTL1 &= ~(AUD_MASK_MBIAS_ISEL_MICBST_R);
			AUD->AUD_ADDA_CTL |= AUD_BIT_DTSDM_CKXEN | AUD_BIT_DTSDM_POW_R;
			AUD->AUD_MBIAS_CTL1 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM_R | AUD_MASK_MBIAS_ISEL_DTSDM_INT1_R);
			AUD->AUD_MBIAS_CTL1 |= AUD_MBIAS_ISEL_DTSDM_R(2) | AUD_MBIAS_ISEL_DTSDM_INT1_R(4);
			AUD->AUD_MBIAS_CTL1 |= AUD_MBIAS_ISEL_MICBST_R(2);
		} else if (amic_num == AMIC3) {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST2_POWL;
			AUD->AUD_MBIAS_CTL2 &= ~AUD_MASK_MBIAS_ISEL_MICBST2_L;
			AUD->AUD_MBIAS_CTL2 |= AUD_MBIAS_ISEL_MICBST2_L(2);
			AUD->AUD_DTS_CTL |= AUD_BIT_DTSDM2_CKXEN | AUD_BIT_DTSDM2_POW_L;
			AUD->AUD_MBIAS_CTL0 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM2_L | AUD_MASK_MBIAS_ISEL_DTSDM2_INT1_L);
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_DTSDM2_L(2) | AUD_MBIAS_ISEL_DTSDM2_INT1_L(4);
		} else if (amic_num == AMIC4) {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST2_POWR;
			AUD->AUD_MBIAS_CTL2 &= ~AUD_MASK_MBIAS_ISEL_MICBST2_R;
			AUD->AUD_MBIAS_CTL2 |= AUD_MBIAS_ISEL_MICBST2_R(2);
			AUD->AUD_DTS_CTL |= AUD_BIT_DTSDM2_CKXEN | AUD_BIT_DTSDM2_POW_R;
			AUD->AUD_MBIAS_CTL0 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM2_R | AUD_MASK_MBIAS_ISEL_DTSDM2_INT1_R);
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_DTSDM2_R(2) | AUD_MBIAS_ISEL_DTSDM2_INT1_R(4);
		} else {
			AUD->AUD_MICBST_CTL1 |= AUD_BIT_MICBST3_POW;
			AUD->AUD_MBIAS_CTL2 &= ~AUD_MASK_MBIAS_ISEL_MICBST3;
			AUD->AUD_MBIAS_CTL2 |= AUD_MBIAS_ISEL_MICBST3(2);
			AUD->AUD_DTS_CTL |= AUD_BIT_DTSDM3_CKXEN | AUD_BIT_DTSDM3_POW;
			AUD->AUD_MBIAS_CTL0 &= ~(AUD_MASK_MBIAS_ISEL_DTSDM3 | AUD_MASK_MBIAS_ISEL_DTSDM3_INT1);
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_DTSDM3(2) | AUD_MBIAS_ISEL_DTSDM3_INT1(4);
		}

	} else {
		if (amic_num == AMIC1) {
			AUD->AUD_MICBST_CTL0 &= ~ AUD_BIT_MICBST_POWL;
			AUD->AUD_ADDA_CTL &= ~(AUD_BIT_DTSDM_POW_L);
		} else if (amic_num == AMIC2) {
			AUD->AUD_MICBST_CTL0 &= ~ AUD_BIT_MICBST_POWR;
			AUD->AUD_ADDA_CTL &= ~(AUD_BIT_DTSDM_POW_R);
		} else if (amic_num == AMIC3) {
			AUD->AUD_MICBST_CTL0 &= ~ AUD_BIT_MICBST2_POWL;
			AUD->AUD_DTS_CTL &= ~(AUD_BIT_DTSDM2_POW_L);
		} else if (amic_num == AMIC4) {
			AUD->AUD_MICBST_CTL0 &= ~ AUD_BIT_MICBST2_POWR;
			AUD->AUD_DTS_CTL &= ~(AUD_BIT_DTSDM2_POW_R);
		} else {
			AUD->AUD_MICBST_CTL0 &= ~ AUD_BIT_MICBST3_POW;
			AUD->AUD_DTS_CTL &= ~(AUD_BIT_DTSDM3_POW);
		}
	}
}

/**
  * @brief Audio Codec micbias pcut power on or power down.
  * @param  micbias_num: select micbias channel
  *			 This parameter can be one of the following values:
  *			   @arg MICBIAS1
  *			   @arg MICBIAS2
  *			   @arg MICBIAS3
  *			   @arg MICBIAS4
  *			   @arg MICBIAS5
  * @param	micbias_mode: micbias power on or power down
  * 		 This parameter can be one of the following values:
  * 		   @arg POWER_ON:
  * 		   @arg POWER_DOWN:
  * @return  None
  */
void AUDIO_CODEC_SetMicBiasPCUTMode(u32 micbias_num, u32 pcut_mode)
{
	assert_param(IS_CODEC_MICBIAS_SEL(micbias_num));
	assert_param(IS_CODEC_POWER_SEL(pcut_mode));
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	switch (micbias_num) {
	case MICBIAS1:
		if (pcut_mode == POWER_ON) {
			AUD->AUD_MICBIAS_CTL0 &= ~AUD_BIT_MICBIAS1_PCUT1_EN;
		} else {

			AUD->AUD_MICBIAS_CTL0 |= AUD_BIT_MICBIAS1_PCUT1_EN;
		}
		break;

	case MICBIAS2:
		if (pcut_mode == POWER_ON) {
			AUD->AUD_MICBIAS_CTL0 &= ~AUD_BIT_MICBIAS1_PCUT2_EN;
		} else {

			AUD->AUD_MICBIAS_CTL0 |= AUD_BIT_MICBIAS1_PCUT2_EN;
		}
		break;

	case MICBIAS3:
		if (pcut_mode == POWER_ON) {
			AUD->AUD_MICBIAS_CTL0 &= ~AUD_BIT_MICBIAS1_PCUT3_EN;
		} else {

			AUD->AUD_MICBIAS_CTL0 |= AUD_BIT_MICBIAS1_PCUT3_EN;
		}
		break;

	case MICBIAS4:
		if (pcut_mode == POWER_ON) {
			AUD->AUD_MICBIAS_CTL0 &= ~AUD_BIT_MICBIAS1_PCUT4_EN;
		} else {

			AUD->AUD_MICBIAS_CTL0 |= AUD_BIT_MICBIAS1_PCUT4_EN;
		}
		break;
	case MICBIAS5:
		if (pcut_mode == POWER_ON) {
			AUD->AUD_MICBIAS_CTL0 &= ~AUD_BIT_MICBIAS1_PCUT5_EN;
		} else {

			AUD->AUD_MICBIAS_CTL0 |= AUD_BIT_MICBIAS1_PCUT5_EN;
		}
		break;
	}
}

/**
  * @brief  Set per micbst mute or unmute.
  * @param  amic_sel: select amic channel
  *			 This parameter can be one of the following values:
  *			   @arg AMIC1
  *			   @arg AMIC2
  *			   @arg AMIC3
  *			   @arg AMIC4
  *			   @arg AMIC5
   * @param  type: mute control
  *			 This parameter can be one of the following values:
  *			   @arg MICIN
  *			   @arg LINEIN: amic3/4/5 not support
  * @param  newstate: mute or unmute option for ad channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE: unmute
  *			   @arg MUTE: mute
  * @return  None
  */
void AUDIO_CODEC_SetMicBstChnMute(u32 amic_sel, u32 type, u32 newstate)
{
	assert_param(IS_CODEC_AMIC_SEL(amic_sel));
	assert_param(IS_CODEC_ADC_MUTE_SEL(type));
	assert_param(IS_CODEC_MUTE_CTR(newstate));

	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (newstate == UNMUTE) {
		if (amic_sel == AMIC1) {
			AUD->AUD_MICBST_CTL0 &= ~AUD_MASK_MICBST_MUTE_L;
			AUD->AUD_MICBST_CTL0 |= AUD_MICBST_MUTE_L(type);
		} else if (amic_sel == AMIC2) {
			AUD->AUD_MICBST_CTL0 &= ~AUD_MASK_MICBST_MUTE_R;
			AUD->AUD_MICBST_CTL0 |= AUD_MICBST_MUTE_R(type);
		} else if (amic_sel == AMIC3) {
			AUD->AUD_MICBST_CTL1 &= ~AUD_MASK_MICBST2_MUTE_L;
			if (type == LINEIN) {
				RTK_LOGE(TAG, "AMIC3 NOT SUPPORT LINEIN MODE\n");
				return;
			}
		} else if (amic_sel == AMIC4) {
			AUD->AUD_MICBST_CTL1 &= ~AUD_MASK_MICBST2_MUTE_R;
			if (type == LINEIN) {
				RTK_LOGE(TAG, "AMIC4 NOT SUPPORT LINEIN MODE\n");
				return;
			}
		} else {
			AUD->AUD_MICBST_CTL1 &= ~ AUD_MASK_MICBST3_MUTE;
			if (type == LINEIN) {
				RTK_LOGE(TAG, "AMIC5 NOT SUPPORT LINEIN MODE\n");
				return;
			}
		}
	} else {
		if (amic_sel == AMIC1) {
			AUD->AUD_MICBST_CTL0 |= AUD_MASK_MICBST_MUTE_L;
		} else if (amic_sel == AMIC2) {
			AUD->AUD_MICBST_CTL0 |= AUD_MASK_MICBST_MUTE_R;
		} else if (amic_sel == AMIC3) {
			AUD->AUD_MICBST_CTL1 |= AUD_MASK_MICBST2_MUTE_L;
		} else if (amic_sel == AMIC4) {
			AUD->AUD_MICBST_CTL1 |= AUD_MASK_MICBST2_MUTE_R;
		} else {
			AUD->AUD_MICBST_CTL1 |= AUD_MASK_MICBST3_MUTE;
		}
	}
}


/**
  * @brief  Set micbst single-end or differential mode.
   * @param  amic_num: select amic channel
  *			 This parameter can be one of the following values:
  *			   @arg AMIC1
  *			   @arg AMIC2
  *			   @arg AMIC3
  *			   @arg AMIC4
  *			   @arg AMIC5
  * @param  mode: micbst single-end or differential mode
  *			 This parameter can be one of the following values:
  *			   @arg SINGLE
  *			   @arg DIFF
  * @return  None
  */
void AUDIO_CODEC_SetMicBstInputMode(u32 amic_num, u32 mode)
{
	assert_param(IS_CODEC_AMIC_SEL(amic_num));
	assert_param(IS_CODEC_LO_MODE(mode));
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (amic_num == AMIC1) {
		if (mode == SINGLE) {
			AUD->AUD_MICBST_CTL0 &= ~AUD_BIT_MICBST_ENDFL;
		} else {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST_ENDFL;
		}
	} else if (amic_num == AMIC2) {
		if (mode == SINGLE) {
			AUD->AUD_MICBST_CTL0 &= ~AUD_BIT_MICBST_ENDFR;
		} else {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST_ENDFR;
		}
	} else if (amic_num == AMIC3) {
		if (mode == SINGLE) {
			AUD->AUD_MICBST_CTL0 &= ~AUD_BIT_MICBST2_ENDFL;
		} else {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST2_ENDFL;
		}
	} else if (amic_num == AMIC4) {
		if (mode == SINGLE) {
			AUD->AUD_MICBST_CTL0 &= ~AUD_BIT_MICBST2_ENDFR;
		} else {
			AUD->AUD_MICBST_CTL0 |= AUD_BIT_MICBST2_ENDFR;
		}
	} else {
		if (mode == SINGLE) {
			AUD->AUD_MICBST_CTL1 &= ~AUD_BIT_MICBST3_ENDF;
		} else {
			AUD->AUD_MICBST_CTL1 |= AUD_BIT_MICBST3_ENDF;
		}
	}
}

/**
  * @brief  Set codec MICBST gain.
  * @param  Amic_sel: amic select
  *          This parameter can be one of the following values:
  *            @arg AMIC1
  *            @arg AMIC2
  *            @arg AMIC3
  *            @arg AMIC4
  *            @arg AMIC5
  * @param  gain: ADC channel micbst gain select
  *          This parameter can be one of the following values:
  *            @arg MICBST_GAIN_0DB: 0dB
  *            @arg MICBST_Gain_5dB: 5dB
  *            ...
  *            @arg MICBST_Gain_40dB: 40dB
  * @note ADC digital volume is 0dB~+40dB in 5dB step.
  * @return  None
  */
void AUDIO_CODEC_SetMicBstGain(u32 amic_sel, u32 gain)
{
	assert_param(IS_CODEC_AMIC_SEL(amic_sel));
	assert_param(IS_CODEC_MICBST_GAIN_SEL(gain));
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (amic_sel == AMIC1) {
		AUD->AUD_MICBST_CTL0 &= ~(AUD_MASK_MICBST_GSELL);
		AUD->AUD_MICBST_CTL0 |= AUD_MICBST_GSELL(gain) ;
	} else if (amic_sel == AMIC2) {
		AUD->AUD_MICBST_CTL0 &= ~(AUD_MASK_MICBST_GSELR);
		AUD->AUD_MICBST_CTL0 |= AUD_MICBST_GSELR(gain);
	} else if (amic_sel == AMIC3) {
		AUD->AUD_MICBST_CTL0 &= ~(AUD_MASK_MICBST2_GSELL);
		AUD->AUD_MICBST_CTL0 |= AUD_MICBST2_GSELL(gain);
	} else if (amic_sel == AMIC4) {
		AUD->AUD_MICBST_CTL0 &= ~(AUD_MASK_MICBST2_GSELR);
		AUD->AUD_MICBST_CTL0 |= AUD_MICBST2_GSELR(gain);
	} else {
		AUD->AUD_MICBST_CTL1 &= ~(AUD_MASK_MICBST3_GSEL);
		AUD->AUD_MICBST_CTL1 |= AUD_MICBST3_GSEL(gain);
	}
}


/**
  * @brief  Enable per dimc channel filter.
  * @param  adc_num: select ad ana channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  *			   @arg ADC3
  *			   @arg ADC4
  *			   @arg ADC5
  *			   @arg ADC6
  *			   @arg ADC7
  *			   @arg ADC8
  * @param  newstate: enable or disable per ad ana channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCDmicFilter(u32 adc_num, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_num));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (adc_num) {

	case ADC1:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DMIC_0_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DMIC_0_EN;
		}
		break;

	case ADC2:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DMIC_1_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DMIC_1_EN;
		}

		break;

	case ADC3:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DMIC_2_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DMIC_2_EN;
		}

		break;

	case ADC4:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DMIC_3_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DMIC_3_EN;
		}

		break;

	case ADC5:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DMIC_4_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DMIC_4_EN;
		}

		break;

	case ADC6:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DMIC_5_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DMIC_5_EN;
		}

		break;

	case ADC7:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DMIC_6_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DMIC_6_EN;
		}

		break;

	case ADC8:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DMIC_7_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DMIC_7_EN;
		}

		break;

	default:
		break;
	}
}

/**
  * @brief  Enable and select DMIC clk.
  * @param  clk: dmic clk select
  *          This parameter can be one of the following values:
  *            @arg DMIC_5M
  *            @arg DMIC_2P5M
  *            @arg DMIC_1P25M
  *            @arg DMIC_625K
  *            @arg DMIC_312P5K
  *            @arg DMIC_769P2K
  * @param  newstate: dmic clock enable or disable
  *			 This parameter can be one of the following values:
  *			   @arg DISABLE
  *			   @arg ENABLE
  * @return  None
  */
void AUDIO_CODEC_SetDmicClk(u32 clk, u32 newstate)
{
	assert_param(IS_CODEC_DMIC_CLOCK_SEL(clk));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CODEC_CLOCK_CONTROL_3 &= ~AUD_MASK_DMIC_CLK_SEL;
		audio_base->CODEC_CLOCK_CONTROL_3 |= AUD_BIT_DMIC_CLK_EN | AUD_DMIC_CLK_SEL(clk);
	} else {
		audio_base->CODEC_CLOCK_CONTROL_3 &= ~AUD_BIT_DMIC_CLK_EN;
	}
}

/**
  * @brief  Set dmic channel source.
  * @param  ad_chn: select adc input path channel
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  *			   @arg ADCHN3
  *			   @arg ADCHN4
  *			   @arg ADCHN5
  *			   @arg ADCHN6
  *			   @arg ADCHN7
  *			   @arg ADCHN8
  * @param  dmic_num: mute or unmute option for per DMIC mix.
  *			 This parameter can be one of the following values:
  *			   @arg DMIC1
  *			   @arg DMIC2
  *			   @arg DMIC3
  *			   @arg DMIC4
  *			   @arg DMIC5
  *			   @arg DMIC6
  *			   @arg DMIC7
  *			   @arg DMIC8
  * @return  None
  */
void AUDIO_CODEC_SetDmicSrc(u32 ad_chn, u32 dmic_num)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_DMIC_SEL(dmic_num));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	audio_base->CODEC_ADC_CH_CTRL[ad_chn - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_MASK_ADC_x_DMIC_SRC_SEL;
	audio_base->CODEC_ADC_CH_CTRL[ad_chn - 1].CODEC_ADC_x_CONTROL_0 |= AUD_ADC_x_DMIC_SRC_SEL(dmic_num - 1);
}

/**
  * @brief  Set I2S source for ADC.
  * @param  i2s_sel: select i2s.
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  *			   @arg I2S1
  * @param  adc_num: select adc channel num.
  *          This parameter can be one of the following values:
  *            @arg ADC1
  *            @arg ADC2
  *            @arg ADC3
  *            @arg ADC4
  *            @arg ADC5
  *            @arg ADC6
  *            @arg ADC7
  *            @arg ADC8
  */
void AUDIO_CODEC_SetADCSrc(u32 i2s_sel, u32 adc_num)
{
	assert_param(IS_CODEC_I2S_SEL(i2s_sel));
	assert_param(IS_CODEC_ADC_SEL(adc_num));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (adc_num) {
	case ADC1:

		if (i2s_sel == I2S1) {
			audio_base->CODEC_I2S_AD_SEL_CONTROL |= AUD_BIT_ADC_0_I2S_SRC_SEL;
		} else {
			audio_base->CODEC_I2S_AD_SEL_CONTROL &= ~AUD_BIT_ADC_0_I2S_SRC_SEL;
		}
		break;

	case ADC2:

		if (i2s_sel == I2S1) {
			audio_base->CODEC_I2S_AD_SEL_CONTROL |= AUD_BIT_ADC_1_I2S_SRC_SEL;
		} else {
			audio_base->CODEC_I2S_AD_SEL_CONTROL &= ~AUD_BIT_ADC_1_I2S_SRC_SEL;
		}

		break;
	case ADC3:

		if (i2s_sel == I2S1) {
			audio_base->CODEC_I2S_AD_SEL_CONTROL |= AUD_BIT_ADC_2_I2S_SRC_SEL;
		} else {
			audio_base->CODEC_I2S_AD_SEL_CONTROL &= ~AUD_BIT_ADC_2_I2S_SRC_SEL;
		}

		break;
	case ADC4:

		if (i2s_sel == I2S1) {
			audio_base->CODEC_I2S_AD_SEL_CONTROL |= AUD_BIT_ADC_3_I2S_SRC_SEL;
		} else {
			audio_base->CODEC_I2S_AD_SEL_CONTROL &= ~AUD_BIT_ADC_3_I2S_SRC_SEL;
		}

		break;
	case ADC5:

		if (i2s_sel == I2S1) {
			audio_base->CODEC_I2S_AD_SEL_CONTROL |= AUD_BIT_ADC_4_I2S_SRC_SEL;
		} else {
			audio_base->CODEC_I2S_AD_SEL_CONTROL &= ~AUD_BIT_ADC_4_I2S_SRC_SEL;
		}

		break;
	case ADC6:
		if (i2s_sel == I2S1) {
			audio_base->CODEC_I2S_AD_SEL_CONTROL |= AUD_BIT_ADC_5_I2S_SRC_SEL;
		} else {
			audio_base->CODEC_I2S_AD_SEL_CONTROL &= ~AUD_BIT_ADC_5_I2S_SRC_SEL;
		}

		break;
	case ADC7:
		if (i2s_sel == I2S1) {
			audio_base->CODEC_I2S_AD_SEL_CONTROL |= AUD_BIT_ADC_6_I2S_SRC_SEL;
		} else {
			audio_base->CODEC_I2S_AD_SEL_CONTROL &= ~AUD_BIT_ADC_6_I2S_SRC_SEL;
		}

		break;
	case ADC8:
		if (i2s_sel == I2S1) {
			audio_base->CODEC_I2S_AD_SEL_CONTROL |= AUD_BIT_ADC_7_I2S_SRC_SEL;
		} else {
			audio_base->CODEC_I2S_AD_SEL_CONTROL &= ~AUD_BIT_ADC_7_I2S_SRC_SEL;
		}

		break;
	}
}

/**
  * @brief  Set DAC source.
  * @param  i2s_sel: select i2s.
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  *			   @arg I2S1
  * @param  dac_l_src: dac l source.
  *			 This parameter can be one of the following values:
  *			   @arg I2SL
  *			   @arg I2SR
  *			   @arg I2SLR
  *			   @arg TESTTONE
  * @param  dac_r_src: dac r source.
  *			 This parameter can be one of the following values:
  *			   @arg I2SL
  *			   @arg I2SR
  *			   @arg I2SLR
  *			   @arg TESTTONE
  */
void AUDIO_CODEC_SetDACSrc(u32 i2s_sel, u32 dac_l_src, u32 dac_r_src)
{
	assert_param(IS_CODEC_I2S_SEL(i2s_sel));
	assert_param(IS_CODEC_I2SCHN_SEL(dac_l_src));
	assert_param(IS_CODEC_I2SCHN_SEL(dac_r_src));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (i2s_sel == I2S0) {
		audio_base->CODEC_I2S_AD_SEL_CONTROL &= ~AUD_BIT_DAC_I2S_SRC_SEL;
	} else {
		audio_base->CODEC_I2S_AD_SEL_CONTROL |= AUD_BIT_DAC_I2S_SRC_SEL;
	}

	audio_base->CODEC_DAC_L_CONTROL_0 &= ~AUD_MASK_DAC_L_DA_SRC_SEL;
	audio_base->CODEC_DAC_L_CONTROL_0 |= AUD_DAC_L_DA_SRC_SEL(dac_l_src);
	audio_base->CODEC_DAC_R_CONTROL_0 &= ~AUD_MASK_DAC_R_DA_SRC_SEL;
	audio_base->CODEC_DAC_R_CONTROL_0 |= AUD_DAC_R_DA_SRC_SEL(dac_r_src);
}

/**
  * @brief  Enable or disable da fifo channel clock.
  * @param  newstate: enable or disable da channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableDACFifo(u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_DA_FIFO_EN;
	} else {
		audio_base->CODEC_CLOCK_CONTROL_1 &= ~AUD_BIT_DA_FIFO_EN;
	}
}

/**
  * @brief  Enable or disable da and da fifo channel clock.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DAC_L
  *			   @arg DAC_R
  * @param  newstate: enable or disable da channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableDAC(u32 channel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (channel == DAC_L) {
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_DA_L_EN | AUD_BIT_MOD_L_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~(AUD_BIT_DA_L_EN | AUD_BIT_MOD_L_EN);
		}

	} else {
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_DA_R_EN | AUD_BIT_MOD_R_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~(AUD_BIT_DA_R_EN | AUD_BIT_MOD_R_EN);
		}
	}
}

/**
  * @brief  Mute DAC path.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DAC_L
  *			   @arg DAC_R
  * @param  newstate: mute or unmute option for da channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE: unmute
  *			   @arg MUTE: mute
  */
void AUDIO_CODEC_SetDACMute(u32 channel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_MUTE_CTR(newstate));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (channel == DAC_L) {
		if (newstate == UNMUTE) {
			audio_base->CODEC_DAC_L_CONTROL_1 &= ~AUD_BIT_DAC_L_DA_MUTE;
		} else {
			audio_base->CODEC_DAC_L_CONTROL_1 |= AUD_BIT_DAC_L_DA_MUTE;
		}
	} else {
		if (newstate == UNMUTE) {
			audio_base->CODEC_DAC_R_CONTROL_1 &= ~AUD_BIT_DAC_R_DA_MUTE;
		} else {
			audio_base->CODEC_DAC_R_CONTROL_1 |= AUD_BIT_DAC_R_DA_MUTE;
		}
	}
}


/**
  * @brief  Set the gain of DAC digital volume.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  *            @arg 1: DAC_R
  * @param  Gain: the value of gain.
  *          This parameter can be -62.5dB~0dB in 0.375dB step
  *            @arg 0x00 -65.625dB
  *            @arg 0xaf 0dB
  * @retval None
  */
void AUDIO_CODEC_SetDACVolume(u32 channel, u32 gain)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (channel == DAC_L) {
		audio_base->CODEC_DAC_L_CONTROL_0 &= ~ AUD_MASK_DAC_L_DA_GAIN;
		audio_base->CODEC_DAC_L_CONTROL_0 |= AUD_DAC_L_DA_GAIN(gain);
	} else {
		audio_base->CODEC_DAC_R_CONTROL_0 &= ~ AUD_MASK_DAC_R_DA_GAIN;
		audio_base->CODEC_DAC_R_CONTROL_0 |= AUD_DAC_R_DA_GAIN(gain);
	}
}

/**
  * @brief  Set DAC HPF enable.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  *            @arg 1: DAC_R
  * @param  newstate: enable or disable per ad channel HPF mode
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetDACHPF(u32 channel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (channel == DAC_L) {
		if (newstate == ENABLE) {
			audio_base->CODEC_DAC_L_CONTROL_1 |= AUD_BIT_DAC_L_DAHPF_EN;
		} else {
			audio_base->CODEC_DAC_L_CONTROL_1 &= ~AUD_BIT_DAC_L_DAHPF_EN;
		}
	} else {
		if (newstate == ENABLE) {
			audio_base->CODEC_DAC_R_CONTROL_1 |= AUD_BIT_DAC_R_DAHPF_EN;
		} else {
			audio_base->CODEC_DAC_R_CONTROL_1 &= ~AUD_BIT_DAC_R_DAHPF_EN;
		}
	}
}


/**
  * @brief  Enable or disable DAC ASRC mode and select asrc rate.
  * @param  sr: sample rate
  * @param  newstate: enable or disable per ad channel HPF mode
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @retval None
  */
void AUDIO_CODEC_SetDACASRC(u32 sr, u32 newstate)
{
	assert_param(IS_CODEC_SR_SEL(sr));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {

		audio_base->CODEC_CLOCK_CONTROL_6 |= AUD_BIT_DAC_ASRC_EN;
		audio_base->CODEC_ASRC_CONTROL_0 &= ~AUD_MASK_ASRC_RATE_SEL_TX;

		if (sr == SR_192K || sr == SR_176P4K) {
			audio_base->CODEC_ASRC_CONTROL_0 |= AUD_ASRC_RATE_SEL_TX(2);
		} else if (sr == SR_96K || sr == SR_88P2K) {
			audio_base->CODEC_ASRC_CONTROL_0 |= AUD_ASRC_RATE_SEL_TX(1);
		} else {
			audio_base->CODEC_ASRC_CONTROL_0 |= AUD_ASRC_RATE_SEL_TX(0);
		}
	} else {
		audio_base->CODEC_CLOCK_CONTROL_6 &= ~AUD_BIT_DAC_ASRC_EN;
	}
}


/**
  * @brief  Set DAC power mode.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  *            @arg 1: DAC_R
  * @param  newstate: enable or disable dac power.
  *			 This parameter can be one of the following values:
  *			   @arg POWER_ON
  *			   @arg POWER_DOWN
  */
void AUDIO_CODEC_SetDACPowerMode(u32 channel, u32 powermode)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_POWER_SEL(powermode));
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	AUD->AUD_ADDA_CTL &= ~AUD_BIT_DAC_CKXSEL;
	AUD->AUD_MBIAS_CTL0 &= ~(AUD_MASK_MBIAS_ISEL_DAC);
	AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_DAC(2);
	AUD->AUD_ADDA_CTL |= AUD_BIT_DAC_CKXEN;

	if (channel == DAC_L) {
		if (powermode == POWER_ON) {
			AUD->AUD_ADDA_CTL |= AUD_BIT_DAC_L_POW;
		} else {
			AUD->AUD_ADDA_CTL &= ~(AUD_BIT_DAC_L_POW);
		}
	} else {
		if (powermode == POWER_ON) {
			AUD->AUD_ADDA_CTL |= AUD_BIT_DAC_R_POW;
		} else {
			AUD->AUD_ADDA_CTL &= ~(AUD_BIT_DAC_R_POW);
		}
	}
}

/**
  * @brief  Set lineout power mode.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: CHN_L
  *            @arg 1: CHN_R
  *            @arg 2: CHN_LR
  * @param  newstate: enable or disable dac power.
  *			 This parameter can be one of the following values:
  *			   @arg POWER_ON
  *			   @arg POWER_DOWN
  */
void AUDIO_CODEC_SetLineOutPowerMode(u32 channel, u32 powermode)
{
	assert_param(IS_CODEC_LOORHO_SEL(channel));
	assert_param(IS_CODEC_POWER_SEL(powermode));
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (channel == CHN_L) {
		if (powermode == POWER_ON) {
			AUD->AUD_MBIAS_CTL0 &= ~AUD_MASK_MBIAS_ISEL_HPO;
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_HPO(2);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_ENDPL);
			AUD->AUD_HPO_BIAS_CTL &= ~(AUD_MASK_HPO_CCSELL);
			AUD->AUD_HPO_BIAS_CTL |= AUD_HPO_CCSELL(3);
			AUD->AUD_HPO_CTL &= ~(AUD_MASK_HPO_ML);
			AUD->AUD_HPO_CTL |= AUD_HPO_ML(2);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_SEL);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_ENAL;
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_L_POW;
		} else {
			AUD->AUD_HPO_CTL &= ~AUD_BIT_HPO_L_POW;
		}
	} else if (channel == CHN_R) {
		if (powermode == POWER_ON) {
			AUD->AUD_MBIAS_CTL0 &= ~AUD_MASK_MBIAS_ISEL_HPO;
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_HPO(2);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_ENDPR);
			AUD->AUD_HPO_BIAS_CTL &= ~(AUD_MASK_HPO_CCSELR);
			AUD->AUD_HPO_BIAS_CTL |= AUD_HPO_CCSELR(3);
			AUD->AUD_HPO_CTL &= ~(AUD_MASK_HPO_MR);
			AUD->AUD_HPO_CTL |= AUD_HPO_MR(2);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_SER);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_ENAR;
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_R_POW;
		} else {
			AUD->AUD_HPO_CTL &= ~AUD_BIT_HPO_R_POW;
		}
	} else {
		if (powermode == POWER_ON) {
			AUD->AUD_MBIAS_CTL0 &= ~AUD_MASK_MBIAS_ISEL_HPO;
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_HPO(2);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_ENDPL | AUD_BIT_HPO_ENDPR);
			AUD->AUD_HPO_BIAS_CTL &= ~(AUD_MASK_HPO_CCSELL | AUD_MASK_HPO_CCSELR);
			AUD->AUD_HPO_BIAS_CTL |= AUD_HPO_CCSELL(3) | AUD_HPO_CCSELR(3);
			AUD->AUD_HPO_CTL &= ~(AUD_MASK_HPO_ML | AUD_MASK_HPO_MR);
			AUD->AUD_HPO_CTL |= AUD_HPO_ML(2) | AUD_HPO_MR(2);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_SEL | AUD_BIT_HPO_SER);
			AUD->AUD_HPO_CTL |= (AUD_BIT_HPO_ENAL | AUD_BIT_HPO_ENAR);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_L_POW | AUD_BIT_HPO_R_POW;
		} else {
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_L_POW | AUD_BIT_HPO_R_POW);
		}
	}
}
/**
  * @brief  Set HPO power mode.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  * 		   @arg 0: CHN_L
  * 		   @arg 1: CHN_R
  * 		   @arg 2: CHN_LR
  * @param  newstate: enable or disable dac power.
  *			 This parameter can be one of the following values:
  *			   @arg POWER_ON
  *			   @arg POWER_DOWN
  */
void AUDIO_CODEC_SetHPOPowerMode(u32 channel, u32 powermode)
{
	assert_param(IS_CODEC_LOORHO_SEL(channel));
	assert_param(IS_CODEC_POWER_SEL(powermode));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (channel == CHN_L) {
		if (powermode == POWER_DOWN) {
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_L_POW);

		} else {

			audio_base->CODEC_AUDIO_CONTROL_1 &= ~AUD_MASK_SEL_BB_CK_DEPOP;
			audio_base->CODEC_AUDIO_CONTROL_1 |= AUD_SEL_BB_CK_DEPOP(1) | AUD_BIT_BB_CK_DEPOP_EN;

			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_SEL;
			AUD->AUD_ADDA_CTL &= ~(AUD_MASK_DPRAMP_CSEL);
			AUD->AUD_ADDA_CTL |= AUD_DPRAMP_CSEL(3);
			AUD->AUD_HPO_BIAS_CTL &= ~(AUD_MASK_HPO_CCSELL);
			AUD->AUD_HPO_BIAS_CTL |= AUD_HPO_CCSELL(3);
			AUD->AUD_MBIAS_CTL0 &= ~AUD_MASK_MBIAS_ISEL_HPO;
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_HPO(2);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_ENAL;
			AUD->AUD_HPO_CTL &= ~(AUD_MASK_HPO_ML);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_ENDPL | AUD_HPO_ML(2) | AUD_BIT_HPO_L_POW;
			DelayUs(10);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_MDPL | AUD_BIT_HPO_OPPDPL;
			AUD->AUD_ADDA_CTL |= AUD_BIT_DPRAMP_POW;
			DelayUs(100);
			AUD->AUD_ADDA_CTL |= AUD_BIT_DPRAMP_ENRAMP;
			DelayMs(500);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_MDPL | AUD_BIT_HPO_OPPDPL);
			DelayUs(100);
			AUD->AUD_ADDA_CTL &= ~(AUD_BIT_DPRAMP_POW | AUD_BIT_DPRAMP_ENRAMP);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_ENDPL);
		}
	} else if (channel == CHN_R) {
		if (powermode == POWER_DOWN) {
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_R_POW);

		} else {
			audio_base->CODEC_AUDIO_CONTROL_1 &= ~AUD_MASK_SEL_BB_CK_DEPOP;
			audio_base->CODEC_AUDIO_CONTROL_1 |= AUD_SEL_BB_CK_DEPOP(1) | AUD_BIT_BB_CK_DEPOP_EN;

			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_SER;
			AUD->AUD_ADDA_CTL &= ~(AUD_MASK_DPRAMP_CSEL);
			AUD->AUD_ADDA_CTL |= AUD_DPRAMP_CSEL(3);
			AUD->AUD_HPO_BIAS_CTL &= ~(AUD_MASK_HPO_CCSELR);
			AUD->AUD_HPO_BIAS_CTL |= AUD_HPO_CCSELR(3);
			AUD->AUD_MBIAS_CTL0 &= ~AUD_MASK_MBIAS_ISEL_HPO;
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_HPO(2);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_ENAR;
			AUD->AUD_HPO_CTL &= ~(AUD_MASK_HPO_MR);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_ENDPR | AUD_HPO_MR(2) | AUD_BIT_HPO_R_POW;
			DelayUs(10);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_MDPR | AUD_BIT_HPO_OPPDPR;
			AUD->AUD_ADDA_CTL |= AUD_BIT_DPRAMP_POW;
			DelayUs(100);
			AUD->AUD_ADDA_CTL |= AUD_BIT_DPRAMP_ENRAMP;
			DelayMs(500);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_MDPR | AUD_BIT_HPO_OPPDPR);
			DelayUs(100);
			AUD->AUD_ADDA_CTL &= ~(AUD_BIT_DPRAMP_POW | AUD_BIT_DPRAMP_ENRAMP);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_ENDPR);
		}
	} else {
		if (powermode == POWER_DOWN) {
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_L_POW | AUD_BIT_HPO_R_POW);
		} else {
			audio_base->CODEC_AUDIO_CONTROL_1 &= ~AUD_MASK_SEL_BB_CK_DEPOP;
			audio_base->CODEC_AUDIO_CONTROL_1 |= AUD_SEL_BB_CK_DEPOP(1) | AUD_BIT_BB_CK_DEPOP_EN;

			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_SEL | AUD_BIT_HPO_SER;
			AUD->AUD_ADDA_CTL &= ~(AUD_MASK_DPRAMP_CSEL);
			AUD->AUD_ADDA_CTL |= AUD_DPRAMP_CSEL(3);
			AUD->AUD_HPO_BIAS_CTL &= ~(AUD_MASK_HPO_CCSELL | AUD_MASK_HPO_CCSELR);
			AUD->AUD_HPO_BIAS_CTL |= AUD_HPO_CCSELL(3) | AUD_HPO_CCSELR(3);
			AUD->AUD_MBIAS_CTL0 &= ~AUD_MASK_MBIAS_ISEL_HPO;
			AUD->AUD_MBIAS_CTL0 |= AUD_MBIAS_ISEL_HPO(2);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_ENAL | AUD_BIT_HPO_ENAR;
			AUD->AUD_HPO_CTL &= ~(AUD_MASK_HPO_ML | AUD_MASK_HPO_MR);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_ENDPL | AUD_BIT_HPO_ENDPR | AUD_HPO_ML(2) | AUD_HPO_MR(2) | AUD_BIT_HPO_L_POW | AUD_BIT_HPO_R_POW;
			DelayUs(10);
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_MDPL | AUD_BIT_HPO_MDPR	| AUD_BIT_HPO_OPPDPL | AUD_BIT_HPO_OPPDPR;
			AUD->AUD_ADDA_CTL |= AUD_BIT_DPRAMP_POW;
			DelayUs(100);
			AUD->AUD_ADDA_CTL |= AUD_BIT_DPRAMP_ENRAMP;
			DelayMs(500);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_MDPL | AUD_BIT_HPO_MDPR | AUD_BIT_HPO_OPPDPL | AUD_BIT_HPO_OPPDPR);
			DelayUs(100);
			AUD->AUD_ADDA_CTL &= ~(AUD_BIT_DPRAMP_POW | AUD_BIT_DPRAMP_ENRAMP);
			AUD->AUD_HPO_CTL &= ~(AUD_BIT_HPO_ENDPL | AUD_BIT_HPO_ENDPR);
		}
	}
}


/**
  * @brief  Set Lineout single-end or differential mode.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: CHN_L
  *            @arg 1: CHN_R
  * @param  mode:
  *			 This parameter can be one of the following values:
  *			   @arg SINGLE
  *			   @arg DIFF
  */
void AUDIO_CODEC_SetLineOutMode(u32 channel, u32 mode)
{
	assert_param(IS_CODEC_LOORHO_SEL(channel));
	assert_param(IS_CODEC_LO_MODE(mode));

	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (channel == CHN_L) {
		if (mode == DIFF) {
			AUD->AUD_HPO_CTL &= ~AUD_BIT_HPO_SEL;
		} else {
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_SEL;
		}
	} else {
		if (mode == DIFF) {
			AUD->AUD_HPO_CTL &= ~AUD_BIT_HPO_SER;
		} else {
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_SER;
		}
	}
}

/**
  * @brief  Set HPO single-end or differential mode.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: CHN_L
  *            @arg 1: CHN_R
  * @param  mode:
  *			 This parameter can be one of the following values:
  *			   @arg SINGLE
  *			   @arg DIFF
  */
void AUDIO_CODEC_SetHPOMode(u32 channel, u32 mode)
{
	assert_param(IS_CODEC_LOORHO_SEL(channel));
	assert_param(IS_CODEC_LO_MODE(mode));
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (channel == CHN_L) {
		if (mode == DIFF) {
			AUD->AUD_HPO_CTL &= ~AUD_BIT_HPO_SEL;
		} else {
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_SEL;
		}
	} else {
		if (mode == DIFF) {
			AUD->AUD_HPO_CTL &= ~AUD_BIT_HPO_SER;
		} else {
			AUD->AUD_HPO_CTL |= AUD_BIT_HPO_SER;
		}
	}
}

/**
  * @brief  Set Line out mute control: dac in or analog in.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: CHN_L
  *            @arg 1: CHN_R
  * @param  type: dac in or analog in.
  *			 This parameter can be one of the following values:
  *			   @arg DACIN
  *			   @arg ANALOGIN
  * @param  newstate:
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE
  *			   @arg MUTE
  * @return  None
  */
void AUDIO_CODEC_SetLineOutMute(u32 channel, u32 type, u32 newstate)
{
	assert_param(IS_CODEC_LOORHO_SEL(channel));
	assert_param(IS_CODEC_DAC_MUTE_SEL(type));
	assert_param(IS_CODEC_MUTE_CTR(newstate));

	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (channel == CHN_L) {
		if (newstate == UNMUTE) {
			AUD->AUD_HPO_CTL &= ~AUD_MASK_HPO_ML;
			AUD->AUD_HPO_CTL |= AUD_HPO_ML(type);

		} else {
			AUD->AUD_HPO_CTL |= AUD_MASK_HPO_ML;
		}
	} else {
		if (newstate == UNMUTE) {
			AUD->AUD_HPO_CTL &= ~AUD_MASK_HPO_MR;
			AUD->AUD_HPO_CTL |= AUD_HPO_MR(type);

		} else {
			AUD->AUD_HPO_CTL |= AUD_MASK_HPO_MR;
		}
	}
}

/**
  * @brief  Set HPO mute control: dac in or analog in.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: CHN_L
  *            @arg 1: CHN_R
  * @param  type: dac in or analog in.
  *			 This parameter can be one of the following values:
  *			   @arg DACIN
  *			   @arg ANALOGIN
  * @param  newstate:
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE
  *			   @arg MUTE
  * @return  None
  */
void AUDIO_CODEC_SetHPOMute(u32 channel, u32 type, u32 newstate)
{
	assert_param(IS_CODEC_LOORHO_SEL(channel));
	assert_param(IS_CODEC_DAC_MUTE_SEL(type));
	assert_param(IS_CODEC_MUTE_CTR(newstate));
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	if (channel == CHN_L) {
		if (newstate == UNMUTE) {
			AUD->AUD_HPO_CTL &= ~AUD_MASK_HPO_ML;
			AUD->AUD_HPO_CTL |= AUD_HPO_ML(type);

		} else {
			AUD->AUD_HPO_CTL |= AUD_MASK_HPO_ML;
		}
	} else {
		if (newstate == UNMUTE) {
			AUD->AUD_HPO_CTL &= ~AUD_MASK_HPO_MR;
			AUD->AUD_HPO_CTL |= AUD_HPO_MR(type);

		} else {
			AUD->AUD_HPO_CTL |= AUD_MASK_HPO_MR;
		}
	}
}


/**
  * @brief  Get HPO single-end or differential mode.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: CHN_L
  *            @arg 1: CHN_R
  * @retval the value of HPO mode.
  *            @arg 0: DIFF
  *            @arg 1: SINGLE
*/
u32 AUDIO_CODEC_GetHPOMode(u32 channel)
{
	assert_param(IS_CODEC_LOORHO_SEL(channel));
	AUD_TypeDef *AUD = AUDIO_AUD_GetAddr();

	u32 hpomode;

	if (channel == CHN_L) {
		hpomode = ((AUD->AUD_HPO_CTL) & AUD_BIT_HPO_SEL) >> 26;
	} else {
		hpomode = ((AUD->AUD_HPO_CTL) & AUD_BIT_HPO_SER) >> 27;
	}

	return hpomode;
}


/**
  * @brief  Enable and select or disalbe PDM clk.
  * @param  channel: the value of dac path.
  *          This parameter can be one of the following values:
  *            @arg 0: DAC_L
  *            @arg 1: DAC_R
  * @param  clk: PDM clk select
  *          This parameter can be one of the following values:
  *            @arg PDM_5M
  *            @arg PDM_2P5M
  *            @arg PDM_6P67M
  * @param  newstate: pdm enable or disable
  *			 This parameter can be one of the following values:
  *			   @arg DISABLE
  *			   @arg ENABLE
  * @return  None
  */
void AUDIO_CODEC_SetPDMClk(u32 channel, u32 clk, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_PDM_CLOCK_SEL(clk));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (channel == DAC_L) {
		if (newstate == ENABLE) {
			audio_base->CODEC_DAC_L_CONTROL_1 |= AUD_BIT_DAC_L_PDM_EN;
		} else {
			audio_base->CODEC_DAC_L_CONTROL_1 &= ~AUD_BIT_DAC_L_PDM_EN;
		}
	} else {
		if (newstate == ENABLE) {
			audio_base->CODEC_DAC_R_CONTROL_1 |= AUD_BIT_DAC_R_PDM_EN;
		} else {
			audio_base->CODEC_DAC_R_CONTROL_1 &= ~AUD_BIT_DAC_R_PDM_EN;
		}
	}

	audio_base->CODEC_AUDIO_CONTROL_1 &= ~ AUD_MASK_PDM_CLK_SEL;
	audio_base->CODEC_AUDIO_CONTROL_1 |= AUD_PDM_CLK_SEL(clk);

}

/**
  * @brief  DAC Testtone mode.
  * @param  fc: the value of test frequency.
  *          This parameter can be one of the following values:
  *          @arg fc
  *          frequency= (fs/192)*(fc+1)Hz
  * @param  newstate: the status of testtone mode.
  *          This parameter can be the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */

void AUDIO_CODEC_SetTesttone(u32 fc, u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CODEC_DAC_L_CONTROL_0 &= ~ AUD_MASK_DAC_L_TEST_FC_SEL;
		audio_base->CODEC_DAC_L_CONTROL_0 |= AUD_BIT_DAC_L_TEST_TONE_EN | (AUD_DAC_L_TEST_FC_SEL(fc));
		audio_base->CODEC_DAC_R_CONTROL_0 &= ~ AUD_MASK_DAC_R_TEST_FC_SEL;
		audio_base->CODEC_DAC_R_CONTROL_0 |= AUD_BIT_DAC_R_TEST_TONE_EN | (AUD_DAC_R_TEST_FC_SEL(fc));
	} else {
		audio_base->CODEC_DAC_L_CONTROL_0 &= ~ AUD_BIT_DAC_L_TEST_TONE_EN;
		audio_base->CODEC_DAC_R_CONTROL_0 &= ~ AUD_BIT_DAC_R_TEST_TONE_EN;
	}
}


/**
  * @brief  Set daad loopback mode.
  * @param  newstate: the status of testtone mode.
  *          This parameter can be the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */
void AUDIO_CODEC_SetDAADLPBK(u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CODEC_AUDIO_CONTROL_0 |= AUD_BIT_DAAD_LPBK_EN;
	} else {
		audio_base->CODEC_AUDIO_CONTROL_0 &= ~AUD_BIT_DAAD_LPBK_EN;
	}
}

/**
  * @brief  Set adda loopback mode.
  * @param  newstate: the status of testtone mode.
  *          This parameter can be the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @retval None
  */
void AUDIO_CODEC_SetADDALPBK(u32 newstate)
{
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CODEC_AUDIO_CONTROL_0 |= AUD_BIT_ADDA_LPBK_EN;
	} else {
		audio_base->CODEC_AUDIO_CONTROL_0 &= ~AUD_BIT_ADDA_LPBK_EN;
	}
}

/**
  * @brief  Slect ADC path decimation source
  * @param  adc_sel: select adc channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  *			   @arg ADC3
  *			   @arg ADC4
  *			   @arg ADC5
  *			   @arg ADC6
  *			   @arg ADC7
  *			   @arg ADC8
  * @param  type: select decimation source
  *			 This parameter can be one of the following values:
  *			   @arg AUDIO_ADC
  *			   @arg ANC
  *			   @arg MUSIC
  *			   @arg ANCANDMUSIC
  * @return  None
  */
void AUDIO_CODEC_SetADCDecimationSrc(u32 adc_sel, u32 type)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));
	assert_param(IS_CODEC_ADC_DCM_SEL(type));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 &= ~(AUD_MASK_ADC_x_DECI_SRC_SEL);
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 |=	AUD_ADC_x_DECI_SRC_SEL(type);

}


/**
  * @brief  Set DAC path zero detection function.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DACL
  *			   @arg DACR
  * @param  type: zero detection function select
  *			 This parameter can be one of the following values:
  *			   @arg IME: immediate change
  *			   @arg ZDET_IME: zero detection & immediate change
  *			   @arg ZDET_STEP: zdet & step
  *			   @arg ZDET_TIMEOUT: zdet & timeout step
  */
void AUDIO_CODEC_SetDACZDET(u32 channel, u32 type)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	assert_param(IS_CODEC_ZDET_SEL(type));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (channel == DAC_L) {
		audio_base->CODEC_DAC_L_CONTROL_1 &= ~AUD_MASK_DAC_L_DA_ZDET_FUNC;
		audio_base->CODEC_DAC_L_CONTROL_1 |= AUD_DAC_L_DA_ZDET_FUNC(type);
	} else {
		audio_base->CODEC_DAC_R_CONTROL_1 &= ~AUD_MASK_DAC_R_DA_ZDET_FUNC;
		audio_base->CODEC_DAC_R_CONTROL_1 |= AUD_DAC_R_DA_ZDET_FUNC(type);
	}
}

/**
  * @brief  Set DAC path zero detection time out select.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg DACL
  *			   @arg DACR
  * @param  time_out: zero detection time out select select
  *			 This parameter can be one of the following values:
  *			   @arg 0: 1024*16 samples
  *			   @arg 1: 1024*32 samples
  *			   @arg 2: 1024*64 samples
  *			   @arg 3: 64 samples
  */
void AUDIO_CODEC_SetDACZDETTimeOut(u32 channel, u32 time_out)
{
	assert_param(IS_CODEC_DAC_SEL(channel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (channel == DAC_L) {
		audio_base->CODEC_DAC_L_CONTROL_1 &= ~AUD_MASK_DAC_L_DA_ZDET_TOUT;
		audio_base->CODEC_DAC_L_CONTROL_1 |= AUD_DAC_L_DA_ZDET_TOUT(time_out);
	} else {
		audio_base->CODEC_DAC_R_CONTROL_1 &= ~AUD_MASK_DAC_R_DA_ZDET_TOUT;
		audio_base->CODEC_DAC_R_CONTROL_1 |= AUD_DAC_R_DA_ZDET_TOUT(time_out);
	}
}


/**
  * @brief  Set ADC path zero detection function.
  * @param  channel: select dac channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  *			   @arg ADC3
  *			   @arg ADC4
  *			   @arg ADC5
  *			   @arg ADC6
  *			   @arg ADC7
  *			   @arg ADC8
  * @param  type: zero detection function select
  *			 This parameter can be one of the following values:
  *			   @arg IME: immediate change
  *			   @arg ZDET_IME: zero detection & immediate change
  *			   @arg ZDET_STEP: zdet & step
  *			   @arg ZDET_TIMEOUT: zdet & timeout step
  */
void AUDIO_CODEC_SetADCZDET(u32 adc_sel, u32 type)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));
	assert_param(IS_CODEC_ZDET_SEL(type));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_MASK_ADC_x_AD_ZDET_FUNC;
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 |= AUD_ADC_x_AD_ZDET_FUNC(type);
}


/**
  * @brief	Set ADC path zero detection function.
  * @param	channel: select dac channel.
  * 		 This parameter can be one of the following values:
  * 		   @arg ADC1
  * 		   @arg ADC2
  * 		   @arg ADC3
  * 		   @arg ADC4
  * 		   @arg ADC5
  * 		   @arg ADC6
  * 		   @arg ADC7
  * 		   @arg ADC8
  * @param	time_out: zero detection time out select select
  * 		 This parameter can be one of the following values:
  * 		   @arg 0: 1024*16 samples
  * 		   @arg 1: 1024*32 samples
  * 		   @arg 2: 1024*64 samples
  * 		   @arg 3: 64 samples
  */
void AUDIO_CODEC_SetADCZDETTimeOut(u32 adc_sel, u32 time_out)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_MASK_ADC_x_AD_ZDET_TOUT;
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 |= AUD_ADC_x_AD_ZDET_TOUT(time_out);
}

/**
  * @brief  Enable or disable adc channel EQ clk.
  * @param  ad_chn: select adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCHN1
  *            @arg ADCHN2
  *            @arg ADCHN3
  *            @arg ADCHN4
  *            @arg ADCHN5
  *            @arg ADCHN6
  * @param  newstate: Enable or disable adc channel EQ clk
  *          This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCEQClk(u32 ad_chn, u32 newstate)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (ad_chn) {
	case ADCHN1:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_0_EQ_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_0_EQ_EN;
		}
		break;

	case ADCHN2:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_1_EQ_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_1_EQ_EN;
		}
		break;

	case ADCHN3:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_2_EQ_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_2_EQ_EN;
		}
		break;

	case ADCHN4:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_3_EQ_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_3_EQ_EN;
		}
		break;

	case ADCHN5:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_4_EQ_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_4_EQ_EN;
		}
		break;

	case ADCHN6:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_AD_5_EQ_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_AD_5_EQ_EN;
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Enable or disable adc channel EQ band.
  * @param  adc_sel: select adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCHN1
  *            @arg ADCHN2
  *            @arg ADCHN3
  *            @arg ADCHN4
  *            @arg ADCHN5
  *            @arg ADCHN6
  * @param  band_sel: select EQ band of each adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCEQBD0
  *            @arg ADCEQBD1
  *            @arg ADCEQBD2
  *            @arg ADCEQBD3
  *            @arg ADCEQBD4
  * @param  newstate: Enable or disable adc channel EQ band
  *          This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCEQBand(u32 ad_chn, u32 band_sel, u32 newstate)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_ADC_EQ_Band_SEL(band_sel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (ad_chn) {
	case ADCHN1:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_0_EQ_CTRL |= AUD_BIT_ADC_0_BIQUAD_EN_0;
			} else {
				audio_base->CODEC_ADC_0_EQ_CTRL &= ~AUD_BIT_ADC_0_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_0_EQ_CTRL |= AUD_BIT_ADC_0_BIQUAD_EN_1;
			} else {
				audio_base->CODEC_ADC_0_EQ_CTRL &= ~AUD_BIT_ADC_0_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_0_EQ_CTRL |= AUD_BIT_ADC_0_BIQUAD_EN_2;
			} else {
				audio_base->CODEC_ADC_0_EQ_CTRL &= ~AUD_BIT_ADC_0_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_0_EQ_CTRL |= AUD_BIT_ADC_0_BIQUAD_EN_3;
			} else {
				audio_base->CODEC_ADC_0_EQ_CTRL &= ~AUD_BIT_ADC_0_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_0_EQ_CTRL |= AUD_BIT_ADC_0_BIQUAD_EN_4;
			} else {
				audio_base->CODEC_ADC_0_EQ_CTRL &= ~AUD_BIT_ADC_0_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;

	case ADCHN2:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_1_EQ_CTRL |= AUD_BIT_ADC_1_BIQUAD_EN_0;
			} else {
				audio_base->CODEC_ADC_1_EQ_CTRL &= ~AUD_BIT_ADC_1_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_1_EQ_CTRL |= AUD_BIT_ADC_1_BIQUAD_EN_1;
			} else {
				audio_base->CODEC_ADC_1_EQ_CTRL &= ~AUD_BIT_ADC_1_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_1_EQ_CTRL |= AUD_BIT_ADC_1_BIQUAD_EN_2;
			} else {
				audio_base->CODEC_ADC_1_EQ_CTRL &= ~AUD_BIT_ADC_1_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_1_EQ_CTRL |= AUD_BIT_ADC_1_BIQUAD_EN_3;
			} else {
				audio_base->CODEC_ADC_1_EQ_CTRL &= ~AUD_BIT_ADC_1_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_1_EQ_CTRL |= AUD_BIT_ADC_1_BIQUAD_EN_4;
			} else {
				audio_base->CODEC_ADC_1_EQ_CTRL &= ~AUD_BIT_ADC_1_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;


	case ADCHN3:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_2_EQ_CTRL |= AUD_BIT_ADC_2_BIQUAD_EN_0;
			} else {
				audio_base->CODEC_ADC_2_EQ_CTRL &= ~AUD_BIT_ADC_2_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_2_EQ_CTRL |= AUD_BIT_ADC_2_BIQUAD_EN_1;
			} else {
				audio_base->CODEC_ADC_2_EQ_CTRL &= ~AUD_BIT_ADC_2_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_2_EQ_CTRL |= AUD_BIT_ADC_2_BIQUAD_EN_2;
			} else {
				audio_base->CODEC_ADC_2_EQ_CTRL &= ~AUD_BIT_ADC_2_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_2_EQ_CTRL |= AUD_BIT_ADC_2_BIQUAD_EN_3;
			} else {
				audio_base->CODEC_ADC_2_EQ_CTRL &= ~AUD_BIT_ADC_2_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_2_EQ_CTRL |= AUD_BIT_ADC_2_BIQUAD_EN_4;
			} else {
				audio_base->CODEC_ADC_2_EQ_CTRL &= ~AUD_BIT_ADC_2_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;

	case ADCHN4:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_3_EQ_CTRL |= AUD_BIT_ADC_3_BIQUAD_EN_0;
			} else {
				audio_base->CODEC_ADC_3_EQ_CTRL &= ~AUD_BIT_ADC_3_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_3_EQ_CTRL |= AUD_BIT_ADC_3_BIQUAD_EN_1;
			} else {
				audio_base->CODEC_ADC_3_EQ_CTRL &= ~AUD_BIT_ADC_3_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_3_EQ_CTRL |= AUD_BIT_ADC_3_BIQUAD_EN_2;
			} else {
				audio_base->CODEC_ADC_3_EQ_CTRL &= ~AUD_BIT_ADC_3_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_3_EQ_CTRL |= AUD_BIT_ADC_3_BIQUAD_EN_3;
			} else {
				audio_base->CODEC_ADC_3_EQ_CTRL &= ~AUD_BIT_ADC_3_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_3_EQ_CTRL |= AUD_BIT_ADC_3_BIQUAD_EN_4;
			} else {
				audio_base->CODEC_ADC_3_EQ_CTRL &= ~AUD_BIT_ADC_3_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;

	case ADCHN5:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_4_EQ_CTRL |= AUD_BIT_ADC_4_BIQUAD_EN_0;
			} else {
				audio_base->CODEC_ADC_4_EQ_CTRL &= ~AUD_BIT_ADC_4_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_4_EQ_CTRL |= AUD_BIT_ADC_4_BIQUAD_EN_1;
			} else {
				audio_base->CODEC_ADC_4_EQ_CTRL &= ~AUD_BIT_ADC_4_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_4_EQ_CTRL |= AUD_BIT_ADC_4_BIQUAD_EN_2;
			} else {
				audio_base->CODEC_ADC_4_EQ_CTRL &= ~AUD_BIT_ADC_4_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_4_EQ_CTRL |= AUD_BIT_ADC_4_BIQUAD_EN_3;
			} else {
				audio_base->CODEC_ADC_4_EQ_CTRL &= ~AUD_BIT_ADC_4_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_4_EQ_CTRL |= AUD_BIT_ADC_4_BIQUAD_EN_4;
			} else {
				audio_base->CODEC_ADC_4_EQ_CTRL &= ~AUD_BIT_ADC_4_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;


	case ADCHN6:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_5_EQ_CTRL |= AUD_BIT_ADC_5_BIQUAD_EN_0;
			} else {
				audio_base->CODEC_ADC_5_EQ_CTRL &= ~AUD_BIT_ADC_5_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_5_EQ_CTRL |= AUD_BIT_ADC_5_BIQUAD_EN_1;
			} else {
				audio_base->CODEC_ADC_5_EQ_CTRL &= ~AUD_BIT_ADC_5_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_5_EQ_CTRL |= AUD_BIT_ADC_5_BIQUAD_EN_2;
			} else {
				audio_base->CODEC_ADC_5_EQ_CTRL &= ~AUD_BIT_ADC_5_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_5_EQ_CTRL |= AUD_BIT_ADC_5_BIQUAD_EN_3;
			} else {
				audio_base->CODEC_ADC_5_EQ_CTRL &= ~AUD_BIT_ADC_5_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				audio_base->CODEC_ADC_5_EQ_CTRL |= AUD_BIT_ADC_5_BIQUAD_EN_4;
			} else {
				audio_base->CODEC_ADC_5_EQ_CTRL &= ~AUD_BIT_ADC_5_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Set EQ band as filter for ADC path
  * @param  ad_chn: select adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCHN1
  *            @arg ADCHN2
  *            @arg ADCHN3
  *            @arg ADCHN4
  *            @arg ADCHN5
  *            @arg ADCHN6
  * @param  band_sel: select EQ band of each adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCEQBD0
  *            @arg ADCEQBD1
  *            @arg ADCEQBD2
  *            @arg ADCEQBD3
  *            @arg ADCEQBD4
  * @param  EQFilterCoefPoint: it is a struct pointer which points a struct that includes coefficients of filter.
  * @return  None
  */
void AUDIO_CODEC_SetADCEQFilter(u32 ad_chn, u32 band_sel, CODEC_EQFilterCoef *EQFilterCoefPoint)
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_ADC_EQ_Band_SEL(band_sel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	CODEC_EQ_BAND_TypeDef *eq_base_addr = NULL;

	switch (ad_chn) {
	case ADCHN1:
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_ADC_0_EQ_BAND;
		break;
	case ADCHN2:
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_ADC_1_EQ_BAND;
		break;
	case ADCHN3:
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_ADC_2_EQ_BAND;
		break;
	case ADCHN4:
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_ADC_3_EQ_BAND;
		break;
	case ADCHN5:
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_ADC_4_EQ_BAND;
		break;
	case ADCHN6:
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_ADC_5_EQ_BAND;
		break;
	default:
		break;
	}

	eq_base_addr[band_sel].CODEC_BIQUAD_H0_x  = EQFilterCoefPoint->H0_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_B1_x  = EQFilterCoefPoint->B1_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_B2_x  = EQFilterCoefPoint->B2_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_A1_x  = EQFilterCoefPoint->A1_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_A2_x  = EQFilterCoefPoint->A2_Q;
}

/**
  * @brief  Enable or disable dac channel EQ clk.
  * @param  da_chn: select dac channel
  *          This parameter can be one of the following values:
  *            @arg DAC_L
  *            @arg DAC_R
  * @param  newstate: Enable or disable adc channel EQ clk
  *          This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetDACEQClk(u32 da_chn, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(da_chn));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (da_chn) {
	case DAC_L:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DA_L_EQ_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DA_L_EQ_EN;
		}
		break;

	case DAC_R:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_2 |= AUD_BIT_DA_R_EQ_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_2 &= ~AUD_BIT_DA_R_EQ_EN;
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Enable or disable adc channel EQ band.
  * @param  adc_sel: select adc channel
  *          This parameter can be one of the following values:
  *            @arg DAC_L
  *            @arg DAC_R
  * @param  band_sel: select EQ band of each adc channel
  *          This parameter can be one of the following values:
  *            @arg DACEQBD0
  *            @arg DACEQBD1
  *            @arg DACEQBD2
  *            @arg DACEQBD3
  *            @arg DACEQBD4
  *            @arg DACEQBD5
  *            @arg DACEQBD6
  *            @arg DACEQBD7
  *            @arg DACEQBD8
  *            @arg DACEQBD9
  * @param  newstate: Enable or disable adc channel EQ band
  *          This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetDACEQBand(u32 da_chn, u32 band_sel, u32 newstate)
{
	assert_param(IS_CODEC_DAC_SEL(da_chn));
	assert_param(IS_CODEC_DAC_EQ_Band_SEL(band_sel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (da_chn == DAC_L) {

		switch (band_sel) {
		case DACEQBD0:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_0;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_0;
			}
			break;

		case DACEQBD1:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_1;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_1;
			}
			break;


		case DACEQBD2:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_2;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_2;
			}
			break;

		case DACEQBD3:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_3;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_3;
			}
			break;


		case DACEQBD4:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_4;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_4;
			}
			break;

		case DACEQBD5:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_5;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_5;
			}
			break;


		case DACEQBD6:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_6;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_6;
			}
			break;

		case DACEQBD7:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_7;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_7;
			}
			break;


		case DACEQBD8:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_8;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_8;
			}
			break;

		case DACEQBD9:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_L_EQ_CTRL |= AUD_BIT_DAC_L_BIQUAD_EN_9;
			} else {
				audio_base->CODEC_DAC_L_EQ_CTRL &= ~AUD_BIT_DAC_L_BIQUAD_EN_9;
			}
			break;

		default:
			break;
		}
	} else {

		switch (band_sel) {
		case DACEQBD0:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_0;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_0;
			}
			break;

		case DACEQBD1:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_1;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_1;
			}
			break;


		case DACEQBD2:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_2;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_2;
			}
			break;

		case DACEQBD3:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_3;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_3;
			}
			break;


		case DACEQBD4:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_4;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_4;
			}
			break;

		case DACEQBD5:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_5;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_5;
			}
			break;


		case DACEQBD6:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_6;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_6;
			}
			break;

		case DACEQBD7:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_7;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_7;
			}
			break;


		case DACEQBD8:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_8;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_8;
			}
			break;

		case DACEQBD9:
			if (newstate == ENABLE) {
				audio_base->CODEC_DAC_R_EQ_CTRL |= AUD_BIT_DAC_R_BIQUAD_EN_9;
			} else {
				audio_base->CODEC_DAC_R_EQ_CTRL &= ~AUD_BIT_DAC_R_BIQUAD_EN_9;
			}
			break;

		default:
			break;
		}
	}
}


/**
  * @brief  Set EQ band as filter for DAC path
  * @param  ad_chn: select adc channel
  *          This parameter can be one of the following values:
  *            @arg DAC_L
  *            @arg DAC_R
  * @param  band_sel: select EQ band of each adc channel
  *          This parameter can be one of the following values:
  *            @arg DACEQBD0
  *            @arg DACEQBD1
  *            @arg DACEQBD2
  *            @arg DACEQBD3
  *            @arg DACEQBD4
  *            @arg DACEQBD5
  *            @arg DACEQBD6
  *            @arg DACEQBD7
  *            @arg DACEQBD8
  *            @arg DACEQBD9
  * @param  EQFilterCoefPoint: it is a struct pointer which points a struct that includes coefficients of filter.
  * @return  None
  */
void AUDIO_CODEC_SetDACEQFilter(u32 da_chn, u32 band_sel, CODEC_EQFilterCoef *EQFilterCoefPoint)
{
	assert_param(IS_CODEC_DAC_SEL(da_chn));
	assert_param(IS_CODEC_DAC_EQ_Band_SEL(band_sel));
	AUDIO_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	CODEC_EQ_BAND_TypeDef *eq_base_addr = NULL;

	switch (da_chn) {
	case DAC_L:
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_DAC_L_EQ_BAND;
		break;
	case DAC_R:
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_DAC_R_EQ_BAND;
		break;
	default:
		break;
	}

	eq_base_addr[band_sel].CODEC_BIQUAD_H0_x  = EQFilterCoefPoint->H0_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_B1_x  = EQFilterCoefPoint->B1_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_B2_x  = EQFilterCoefPoint->B2_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_A1_x  = EQFilterCoefPoint->A1_Q;
	eq_base_addr[band_sel].CODEC_BIQUAD_A2_x  = EQFilterCoefPoint->A2_Q;

}


/**
  * @brief	Audio codec record flow for test.
  */
void AUDIO_CODEC_Record(u32 i2s_sel, u32 type, I2S_InitTypeDef *I2S_InitStruct)
{
	AUDIO_CODEC_SetAudioIP(ENABLE);
	AUDIO_CODEC_SetI2SIP(i2s_sel, ENABLE);
	AUDIO_CODEC_SetI2SSRC(i2s_sel, INTERNAL_SPORT);
	AUDIO_CODEC_SetADCSRSrc(SOURCE1, I2S_InitStruct->CODEC_SelI2SRxSR);
	AUDIO_CODEC_SetANAClk(ENABLE);
	AUDIO_CODEC_SetI2SParameters(i2s_sel, ADCPATH, I2S_InitStruct);

	AUDIO_CODEC_SetADCSrc(i2s_sel, ADC1);
	AUDIO_CODEC_SetADCSrc(i2s_sel, ADC2);
	AUDIO_CODEC_SetADCSrc(i2s_sel, ADC3);
	AUDIO_CODEC_SetADCSrc(i2s_sel, ADC4);
	AUDIO_CODEC_SetADCSrc(i2s_sel, ADC5);
	AUDIO_CODEC_SetADCSrc(i2s_sel, ADC6);
	AUDIO_CODEC_SetADCSrc(i2s_sel, ADC7);
	AUDIO_CODEC_SetADCSrc(i2s_sel, ADC8);

	if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_NOTDM || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM4 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 ||
		I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

		AUDIO_CODEC_EnableADC(ADCHN1, ENABLE);
		AUDIO_CODEC_EnableADC(ADCHN2, ENABLE);

		AUDIO_CODEC_EnableADCFifo(ADCHN1, ENABLE);
		AUDIO_CODEC_EnableADCFifo(ADCHN2, ENABLE);
		AUDIO_CODEC_SetADCVolume(ADC1, 0X2F);
		AUDIO_CODEC_SetADCVolume(ADC2, 0X2F);
		AUDIO_CODEC_SetADCHPF(ADC1, 3, ENABLE);
		AUDIO_CODEC_SetADCHPF(ADC2, 3, ENABLE);
		AUDIO_CODEC_SetADCMute(ADC1, UNMUTE);
		AUDIO_CODEC_SetADCMute(ADC2, UNMUTE);
	}

	if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM4 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

		AUDIO_CODEC_EnableADC(ADCHN3, ENABLE);
		AUDIO_CODEC_EnableADC(ADCHN4, ENABLE);
		AUDIO_CODEC_EnableADCFifo(ADCHN3, ENABLE);
		AUDIO_CODEC_EnableADCFifo(ADCHN4, ENABLE);
		AUDIO_CODEC_SetADCVolume(ADC3, 0X2F);
		AUDIO_CODEC_SetADCVolume(ADC4, 0X2F);
		AUDIO_CODEC_SetADCHPF(ADC3, 3, ENABLE);
		AUDIO_CODEC_SetADCHPF(ADC4, 3, ENABLE);
		AUDIO_CODEC_SetADCMute(ADC3, UNMUTE);
		AUDIO_CODEC_SetADCMute(ADC4, UNMUTE);
	}

	if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

		AUDIO_CODEC_EnableADC(ADCHN5, ENABLE);
		AUDIO_CODEC_EnableADC(ADCHN6, ENABLE);
		AUDIO_CODEC_EnableADCFifo(ADCHN5, ENABLE);
		AUDIO_CODEC_EnableADCFifo(ADCHN6, ENABLE);
		AUDIO_CODEC_SetADCVolume(ADC5, 0X2F);
		AUDIO_CODEC_SetADCVolume(ADC6, 0X2F);
		AUDIO_CODEC_SetADCHPF(ADC5, 3, ENABLE);
		AUDIO_CODEC_SetADCHPF(ADC6, 3, ENABLE);
		AUDIO_CODEC_SetADCMute(ADC5, UNMUTE);
		AUDIO_CODEC_SetADCMute(ADC6, UNMUTE);
	}

	if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

		AUDIO_CODEC_EnableADC(ADCHN7, ENABLE);
		AUDIO_CODEC_EnableADC(ADCHN8, ENABLE);
		AUDIO_CODEC_EnableADCFifo(ADCHN7, ENABLE);
		AUDIO_CODEC_EnableADCFifo(ADCHN8, ENABLE);
		AUDIO_CODEC_SetADCVolume(ADC7, 0X2F);
		AUDIO_CODEC_SetADCVolume(ADC8, 0X2F);
		AUDIO_CODEC_SetADCHPF(ADC7, 3, ENABLE);
		AUDIO_CODEC_SetADCHPF(ADC8, 3, ENABLE);
		AUDIO_CODEC_SetADCMute(ADC7, UNMUTE);
		AUDIO_CODEC_SetADCMute(ADC8, UNMUTE);
	}

	if (type == APP_AMIC_RECORD) {

		AUDIO_CODEC_SetMicBiasPowerMode(POWER_ON);

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_NOTDM || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM4 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 ||
			I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

			AUDIO_CODEC_DisPAD(PAD_MIC1);
			AUDIO_CODEC_DisPAD(PAD_MIC2);
			AUDIO_CODEC_SetADCMixMute(ADC1, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC2, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC1, ENABLE);
			AUDIO_CODEC_SetADCANAFilter(ADC2, ENABLE);
			AUDIO_CODEC_SetADCANASrc(ADCHN1, AMIC1);
			AUDIO_CODEC_SetADCANASrc(ADCHN2, AMIC2);
			AUDIO_CODEC_SetMicBstPowerMode(AMIC1, NORMALPOWER);
			AUDIO_CODEC_SetMicBstPowerMode(AMIC2, NORMALPOWER);
			AUDIO_CODEC_SetMicBiasPCUTMode(MICBIAS1, POWER_ON);
			AUDIO_CODEC_SetMicBiasPCUTMode(MICBIAS2, POWER_ON);
			AUDIO_CODEC_SetMicBstChnMute(AMIC1, MICIN, UNMUTE);
			AUDIO_CODEC_SetMicBstChnMute(AMIC2, MICIN, UNMUTE);
			AUDIO_CODEC_SetMicBstInputMode(AMIC1, DIFF);
			AUDIO_CODEC_SetMicBstInputMode(AMIC2, DIFF);
			AUDIO_CODEC_SetMicBstGain(AMIC1, MICBST_GAIN_0DB);
			AUDIO_CODEC_SetMicBstGain(AMIC2, MICBST_GAIN_0DB);
		}

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM4 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

			AUDIO_CODEC_DisPAD(PAD_MIC3);
			AUDIO_CODEC_DisPAD(PAD_MIC4);
			AUDIO_CODEC_SetADCMixMute(ADC3, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC4, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC3, ENABLE);
			AUDIO_CODEC_SetADCANAFilter(ADC4, ENABLE);
			AUDIO_CODEC_SetADCANASrc(ADCHN3, AMIC3);
			AUDIO_CODEC_SetADCANASrc(ADCHN4, AMIC4);
			AUDIO_CODEC_SetMicBstPowerMode(AMIC3, NORMALPOWER);
			AUDIO_CODEC_SetMicBstPowerMode(AMIC4, NORMALPOWER);
			AUDIO_CODEC_SetMicBiasPCUTMode(MICBIAS3, POWER_ON);
			AUDIO_CODEC_SetMicBiasPCUTMode(MICBIAS4, POWER_ON);
			AUDIO_CODEC_SetMicBstChnMute(AMIC3, MICIN, UNMUTE);
			AUDIO_CODEC_SetMicBstChnMute(AMIC4, MICIN, UNMUTE);
			AUDIO_CODEC_SetMicBstInputMode(AMIC3, DIFF);
			AUDIO_CODEC_SetMicBstInputMode(AMIC4, DIFF);
			AUDIO_CODEC_SetMicBstGain(AMIC3, MICBST_GAIN_0DB);
			AUDIO_CODEC_SetMicBstGain(AMIC4, MICBST_GAIN_0DB);
		}


		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

			AUDIO_CODEC_DisPAD(PAD_MIC5);
			AUDIO_CODEC_SetADCMixMute(ADC5, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC5, ENABLE);
			AUDIO_CODEC_SetADCANASrc(ADCHN5, AMIC5);
			AUDIO_CODEC_SetMicBstPowerMode(AMIC5, NORMALPOWER);
			AUDIO_CODEC_SetMicBiasPCUTMode(MICBIAS5, POWER_ON);
			AUDIO_CODEC_SetMicBstChnMute(AMIC5, MICIN, UNMUTE);
			AUDIO_CODEC_SetMicBstInputMode(AMIC5, DIFF);
			AUDIO_CODEC_SetMicBstGain(AMIC5, MICBST_GAIN_0DB);

			//for test
			AUDIO_CODEC_SetADCMixMute(ADC6, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC6, ENABLE);
			AUDIO_CODEC_SetADCANASrc(ADCHN6, AMIC5);
		}
	}


	if (type == APP_DAAD_LPBK) {

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_NOTDM || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM4 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 ||
			I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {
			AUDIO_CODEC_SetADCMixMute(ADC1, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC2, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC1, ENABLE);
			AUDIO_CODEC_SetADCANAFilter(ADC2, ENABLE);
		}

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM4 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

			AUDIO_CODEC_SetADCMixMute(ADC3, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC4, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC3, ENABLE);
			AUDIO_CODEC_SetADCANAFilter(ADC4, ENABLE);
		}

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

			AUDIO_CODEC_SetADCMixMute(ADC5, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC6, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC5, ENABLE);
			AUDIO_CODEC_SetADCANAFilter(ADC6, ENABLE);
		}

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {
			AUDIO_CODEC_SetADCMixMute(ADC7, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC8, ANAAD, UNMUTE);
			AUDIO_CODEC_SetADCANAFilter(ADC7, ENABLE);
			AUDIO_CODEC_SetADCANAFilter(ADC8, ENABLE);
		}
	}

	if (type == APP_DMIC_RECORD) {

		AUDIO_CODEC_SetDmicClk(DMIC_2P5M, ENABLE);

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_NOTDM || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM4 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 ||
			I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

			AUDIO_CODEC_SetADCMixMute(ADC1, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC2, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCDmicFilter(ADC1, ENABLE);
			AUDIO_CODEC_SetADCDmicFilter(ADC2, ENABLE);
			AUDIO_CODEC_SetDmicSrc(ADCHN1, DMIC1);
			AUDIO_CODEC_SetDmicSrc(ADCHN2, DMIC2);
		}

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM4 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

			AUDIO_CODEC_SetADCMixMute(ADC3, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC4, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCDmicFilter(ADC3, ENABLE);
			AUDIO_CODEC_SetADCDmicFilter(ADC4, ENABLE);
			AUDIO_CODEC_SetDmicSrc(ADCHN3, DMIC3);
			AUDIO_CODEC_SetDmicSrc(ADCHN4, DMIC4);
		}

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM6 || I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

			AUDIO_CODEC_SetADCMixMute(ADC5, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC6, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCDmicFilter(ADC5, ENABLE);
			AUDIO_CODEC_SetADCDmicFilter(ADC6, ENABLE);
			AUDIO_CODEC_SetDmicSrc(ADCHN5, DMIC5);
			AUDIO_CODEC_SetDmicSrc(ADCHN6, DMIC6);
		}

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_TDM8) {

			AUDIO_CODEC_SetADCMixMute(ADC7, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC8, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCDmicFilter(ADC7, ENABLE);
			AUDIO_CODEC_SetADCDmicFilter(ADC8, ENABLE);
			AUDIO_CODEC_SetDmicSrc(ADCHN7, DMIC7);
			AUDIO_CODEC_SetDmicSrc(ADCHN8, DMIC8);
		}
	}
}

/**
  * @brief	Audio codec playback flow for test.
  */
void AUDIO_CODEC_Playback(u32 i2s_sel, u32 type, I2S_InitTypeDef *I2S_InitStruct)
{
	AUDIO_CODEC_SetAudioIP(ENABLE);
	AUDIO_CODEC_SetI2SIP(i2s_sel, ENABLE);
	AUDIO_CODEC_SetI2SSRC(i2s_sel, INTERNAL_SPORT);
	AUDIO_CODEC_SetI2SParameters(i2s_sel, DACPATH, I2S_InitStruct); //根据实际情况再具体设定
	AUDIO_CODEC_SetDACSRSrc(SOURCE0, I2S_InitStruct->CODEC_SelI2STxSR);
	AUDIO_CODEC_SetANAClk(ENABLE);
	AUDIO_CODEC_EnableDAC(DAC_L, ENABLE);
	AUDIO_CODEC_EnableDAC(DAC_R, ENABLE);
	AUDIO_CODEC_EnableDACFifo(ENABLE);
	AUDIO_CODEC_SetDACMute(DAC_L, UNMUTE);
	AUDIO_CODEC_SetDACMute(DAC_R, UNMUTE);
	AUDIO_CODEC_SetDACVolume(DAC_L, 0XAF);
	AUDIO_CODEC_SetDACVolume(DAC_R, 0XAF);
	AUDIO_CODEC_SetDACHPF(DAC_L, ENABLE);
	AUDIO_CODEC_SetDACHPF(DAC_R, ENABLE);

	if (type == APP_DAAD_LPBK) {
		AUDIO_CODEC_SetDACSrc(i2s_sel, I2SL, I2SR); // for lineout
	}

	if (type == APP_LINE_OUT) {
		AUDIO_CODEC_SetDACSrc(i2s_sel, I2SL, I2SR); // for lineout
		AUDIO_CODEC_DisPAD(PAD_DACL);
		AUDIO_CODEC_DisPAD(PAD_DACR);
		AUDIO_CODEC_SetDACPowerMode(DAC_L, POWER_ON);
		AUDIO_CODEC_SetDACPowerMode(DAC_R, POWER_ON);
		AUDIO_CODEC_SetLineOutPowerMode(CHN_LR, POWER_ON);
		AUDIO_CODEC_SetLineOutMode(CHN_L, DIFF);
		AUDIO_CODEC_SetLineOutMode(CHN_R, DIFF);
		AUDIO_CODEC_SetLineOutMute(CHN_L, DACIN, UNMUTE);
		AUDIO_CODEC_SetLineOutMute(CHN_R, DACIN, UNMUTE);
	}

	if (type == APP_HPO_OUT) {
		AUDIO_CODEC_SetDACSrc(i2s_sel, I2SL, I2SR); // for lineout
		AUDIO_CODEC_DisPAD(PAD_DACL);
		AUDIO_CODEC_DisPAD(PAD_DACR);
		AUDIO_CODEC_SetDACPowerMode(DAC_L, POWER_ON);
		AUDIO_CODEC_SetDACPowerMode(DAC_R, POWER_ON);
		AUDIO_CODEC_SetHPOPowerMode(CHN_LR, POWER_ON);
		AUDIO_CODEC_SetHPOMode(CHN_L, SINGLE);
		AUDIO_CODEC_SetHPOMode(CHN_R, SINGLE);
		AUDIO_CODEC_SetHPOMute(CHN_L, DACIN, UNMUTE);
		AUDIO_CODEC_SetHPOMute(CHN_R, DACIN, UNMUTE);
	}

	if (type == APP_PDM_OUT) {
		AUDIO_CODEC_SetDACSrc(i2s_sel, I2SL, I2SR);  // for pdm out
		AUDIO_CODEC_SetPDMClk(DAC_L, PDM_5M, ENABLE);
		AUDIO_CODEC_SetPDMClk(DAC_R, PDM_5M, ENABLE);
	}

	if (type == APP_TESTTONE) {
		AUDIO_CODEC_SetDACSrc(i2s_sel, TESTTONE, TESTTONE);  //for testtone
		AUDIO_CODEC_SetTesttone(3, ENABLE);
	}
}

/**
  * @}
  */

/** @} */

/** @} */

/** @} */

