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
  * @brief	Get audio codec base address.
  * @return audio codec base address.
  */
AUDIO_CODEC_TypeDef *AUDIO_CODEC_GetAddr(void)
{
	if (TrustZone_IsSecure()) {
		return (AUDIO_CODEC_TypeDef *)AUDIO_REG_BASE_S;
	} else {
		return (AUDIO_CODEC_TypeDef *)AUDIO_REG_BASE;
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
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

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
  * @param  newstate: I2S IP enable or disable.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetI2SIP(u32 i2s_sel, u32 newstate)
{
	assert_param(IS_CODEC_I2S_SEL(i2s_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == ENABLE) {
		audio_base->CODEC_I2S_0_CONTROL |= AUD_BIT_I2S_0_RST_N_REG;
	} else {
		audio_base->CODEC_I2S_0_CONTROL &= ~AUD_BIT_I2S_0_RST_N_REG;
	}
}

/**
  * @brief  Select I2S master source.
  * @param  i2s_sel: select I2S.
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  * @param  src: source
  *          This parameter can be one of the following values:
  *            @arg INTERNAL_SPORT
  *            @arg EXTERNAL_I2S
  * @return  None
  */
void AUDIO_CODEC_SetI2SSRC(u32 i2s_sel, u32 src)
{
	assert_param(IS_CODEC_I2S_SEL(i2s_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (src == INTERNAL_SPORT) {
		audio_base->CODEC_I2S_0_CONTROL &= ~AUD_BIT_I2S_0_MASTER_SEL;
	} else {
		audio_base->CODEC_I2S_0_CONTROL |= AUD_BIT_I2S_0_MASTER_SEL;
	}
}

/**
  * @brief  Select i2s rx and i2s rx tdm mode
  * @param  i2s: select i2s: i2s0
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  * @param  tdmmode: select I2S RX TDM mode
  *			 This parameter can be one of the following values:
  *			   @arg I2S_NOTDM
  * @return  None
  */
void AUDIO_CODEC_SetI2SRXTDM(u32 i2s_sel, u32 tdmmode)
{
	u32 Tmp1;
	u32 Tmp2;

	assert_param(IS_CODEC_I2S_SEL(i2s_sel));
	assert_param(IS_CODEC_I2S_TDM_SEL(tdmmode));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp1 = audio_base->CODEC_I2S_0_CONTROL;
	Tmp1 &= ~(AUD_MASK_I2S_0_TDM_MODE_RX);
	Tmp2 = audio_base->CODEC_I2S_0_CONTROL_1;

	// RX choose I2S_NOTDM mode
	Tmp1 |= AUD_I2S_0_TDM_MODE_RX(0);
	Tmp2 &= ~(AUD_MASK_I2S_0_DATA_CH0_SEL_RX | AUD_MASK_I2S_0_DATA_CH1_SEL_RX | AUD_BIT_I2S_0_DATA_CH0_RX_DISABLE | AUD_BIT_I2S_0_DATA_CH1_RX_DISABLE);
	Tmp2 |= (AUD_I2S_0_DATA_CH0_SEL_RX(0) | AUD_I2S_0_DATA_CH1_SEL_RX(1));

	audio_base->CODEC_I2S_0_CONTROL = Tmp1;
	audio_base->CODEC_I2S_0_CONTROL_1 = Tmp2;
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
	I2S_Struct->CODEC_SelI2STxDataFormat = DF_I2S;
	I2S_Struct->CODEC_SelI2SRxSR = SR_48K;
	I2S_Struct->CODEC_SelI2SRxWordLen = WL_16;
	I2S_Struct->CODEC_SelI2SRxCHLen = CL_32;
	I2S_Struct->CODEC_SelI2SRxDataFormat = DF_I2S;
	I2S_Struct->CODEC_SelRxI2STdm = I2S_NOTDM;
}

/**
  * @brief  Set I2S initialization parameter.
  * @param  i2s_sel: select i2s
  *			 This parameter can be one of the following values:
  *			   @arg I2S0
  * @param  type: audio work path.
  *			 This parameter can be one of the following values:
  *			   @arg ADCPATH
  * @return  None
  */
void AUDIO_CODEC_SetI2SParameters(u32 i2s_sel, u32 path, I2S_InitTypeDef *I2S_Struct)
{
	u32 Tmp;

	assert_param(IS_CODEC_I2S_SEL(i2s_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (path == ADCPATH) {
		Tmp = audio_base->CODEC_I2S_0_CONTROL;
		Tmp &= ~(AUD_MASK_I2S_0_CH_LEN_SEL_RX | AUD_MASK_I2S_0_DATA_LEN_SEL_RX | AUD_MASK_I2S_0_DATA_FORMAT_SEL_RX);
		Tmp |= (AUD_I2S_0_CH_LEN_SEL_RX(I2S_Struct->CODEC_SelI2SRxCHLen) | AUD_I2S_0_DATA_LEN_SEL_RX(I2S_Struct->CODEC_SelI2SRxWordLen)
				| AUD_I2S_0_DATA_FORMAT_SEL_RX(I2S_Struct->CODEC_SelI2SRxDataFormat));
		audio_base->CODEC_I2S_0_CONTROL = Tmp;
		AUDIO_CODEC_SetI2SRXTDM(I2S0, I2S_Struct->CODEC_SelRxI2STdm);
	} else {
		return;

	}
}
/**
  * @brief  Select adc sample rate and source, all adc share source and sample rate.
  * @param  src: source
  *          This parameter can be one of the following values:
  *            @arg SOURCE0
  *            @arg SOURCE1
  * @param  sr: sample rate
  * @return  None
  */
void AUDIO_CODEC_SetADCSRSrc(u32 src, u32 sr)
{
	u32 Tmp;

	assert_param(IS_CODEC_Sample_Rate_Source_SEL(src));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp = audio_base->CODEC_CLOCK_CONTROL_5;
	Tmp &= ~(AUD_MASK_ADC_0_FS_SRC_SEL | AUD_MASK_ADC_1_FS_SRC_SEL);
	Tmp |= AUD_ADC_0_FS_SRC_SEL(src) | AUD_ADC_1_FS_SRC_SEL(src);
	audio_base->CODEC_CLOCK_CONTROL_5 = Tmp;

	Tmp = audio_base->CODEC_CLOCK_CONTROL_4;
	if (src == SOURCE0) {
		Tmp &= ~AUD_MASK_SAMPLE_RATE_0;
		Tmp |= AUD_SAMPLE_RATE_0(sr);
	} else {
		Tmp &= ~AUD_MASK_SAMPLE_RATE_1;
		Tmp |= AUD_SAMPLE_RATE_1(sr);
	}
	audio_base->CODEC_CLOCK_CONTROL_4 = Tmp;
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
	default:
		break;
	}

	return en_bit;
}

/**
  * @brief  Enable per AD and AD fifo channel clock by mask.
  * @param  ad_chn_mask: ad channel mask.
  * The correspondence between bits and channels is as follows:
  * bit1   bit0
  *   |     |
  * ADCHN2 ADCHN1
  * @return  None
  */
void AUDIO_CODEC_EnableADCFifoForMask(u32 ad_chn_mask)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	u32 or_tmp = 0;

	for (u32 i = ADCHN1; i < (ADCHN2 + 1); i++) {
		if ((ad_chn_mask >> (i - 1)) & 1) {
			or_tmp |= AUDIO_CODEC_GetAdFifoEnBitForCh(i);
		}
	}

	audio_base->CODEC_CLOCK_CONTROL_1 |= or_tmp;
}


/**
  * @brief  Enable or disable per AD fifo channel clock.
  * @param  ad_chn: select ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  * @param  newstate: enable or disable per ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableADCFifo(u32 ad_chn, u32 newstate)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));

	switch (ad_chn) {

	case ADCHN1:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_0_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~AUD_BIT_AD_0_FIFO_EN;
		}
		break;
	case ADCHN2:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_1_FIFO_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~AUD_BIT_AD_1_FIFO_EN;
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
	default:
		break;
	}

	return en_bit;
}

/**
  * @brief  Enable per AD and AD fifo channel clock by mask.
  * @param  ad_chn_mask: ad channel mask.
  * The correspondence between bits and channels is as follows:
  * bit1   bit0
  *   |     |
  * ADCHN2 ADCHN1
  *
  * @return  None
  */
void AUDIO_CODEC_EnableADCForMask(u32 ad_chn_mask)
{
	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	u32 or_tmp = 0;

	for (u32 i = ADCHN1; i < (ADCHN2 + 1); i++) {
		if ((ad_chn_mask >> (i - 1)) & 1) {
			or_tmp |= AUDIO_CODEC_GetAdEnBitForCh(i);
		}
	}

	audio_base->CODEC_CLOCK_CONTROL_1 |= or_tmp;
}

/**
  * @brief  Enable or disable per AD channel clock.
  * @param  ad_chn: select ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  * @param  newstate: enable or disable per ad channel.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_EnableADC(u32 ad_chn, u32 newstate)   //enable adc
{
	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	switch (ad_chn) {

	case ADCHN1:
		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_0_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~AUD_BIT_AD_0_EN;
		}
		break;
	case ADCHN2:

		if (newstate == ENABLE) {
			audio_base->CODEC_CLOCK_CONTROL_1 |= AUD_BIT_AD_1_EN;
		} else {
			audio_base->CODEC_CLOCK_CONTROL_1 &= ~AUD_BIT_AD_1_EN;
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
  * @param  gain: the value of gain.
  *          This parameter can be -17.625dB~48dB in 0.375dB step
  *            @arg 0x00 -17.625dB
  *            @arg 0xaf 48dBS
  * @retval None
  */
void AUDIO_CODEC_SetADCVolume(u32 adc_sel, u32 gain)

{
	u32 Tmp;

	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp = audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_1;
	Tmp &= ~AUD_MASK_ADC_x_AD_GAIN;
	Tmp |= AUD_ADC_x_AD_GAIN(gain);
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_1 = Tmp;
}

/**
  * @brief  Set HPF mode and select HPF FC
  * @param  adc_sel: select adc channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
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
	u32 Tmp;

	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp = audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0;
	if (newstate == ENABLE) {
		Tmp &= ~(AUD_MASK_ADC_x_DCHPF_FC_SEL);
		Tmp |= AUD_BIT_ADC_x_DCHPF_EN | AUD_ADC_x_DCHPF_FC_SEL(fc);
	} else {
		Tmp &= ~AUD_BIT_ADC_x_DCHPF_EN;
	}
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 = Tmp;

}

/**
  * @brief  Set ADC ASRC mode.
  * @param  i2s_sel: select i2s.
  *		     This parameter can be one of the following values:
  *			   @arg I2S0
  * @param  adc_sel: select adc channel.
  *          This parameter can be one of the following values:
  *            @arg ADC1
  *            @arg ADC2
  * @param  sr: the value of sample rate.
  * @param  newstate: Enable or disable ASRC mode.
  *			 This parameter can be one of the following values:
  *			   @arg DISABLE
  *			   @arg ENABLE
  * @retval None
  */
void AUDIO_CODEC_SetADCASRC(u32 i2s_sel, u32 adc_sel, u32 sr, u32 newstate)
{
	u32 Tmp;

	assert_param(IS_CODEC_I2S_SEL(i2s_sel));
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp = audio_base->CODEC_ASRC_CONTROL_2;

	if (sr == SR_96K || sr == SR_88P2K) {

		Tmp &= ~AUD_MASK_ASRC_RATE_SEL_RX_0;
		Tmp |= AUD_ASRC_RATE_SEL_RX_0(1);
	} else {

		Tmp &= ~AUD_MASK_ASRC_RATE_SEL_RX_0;
		Tmp |= AUD_ASRC_RATE_SEL_RX_0(0);
	}

	audio_base->CODEC_ASRC_CONTROL_2 = Tmp;

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
  * @param  newstate: mute or unmute option for ad channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE
  *			   @arg MUTE
  * @return  None
  */
void AUDIO_CODEC_SetADCMute(u32 adc_sel, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == MUTE) {
		audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 |= AUD_BIT_ADC_x_AD_MUTE;
	} else {
		audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_BIT_ADC_x_AD_MUTE;
	}

}

/**
  * @brief  Set per adc mix mute ad input path to mute or unmute.
  * @param  adc_num: select adc input path channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  type: adc path input select
  *			 This parameter can be one of the following values:
  *			   @arg DMIC
  * @param  newstate: mute or unmute option for ad channel
  *			 This parameter can be one of the following values:
  *			   @arg UNMUTE
  *			   @arg MUTE
  * @return  None
  */
void AUDIO_CODEC_SetADCMixMute(u32 adc_num, u32 type, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_num));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (newstate == UNMUTE) {
		if (type == DMIC) {
			audio_base->CODEC_ADC_CH_CTRL[adc_num - 1].CODEC_ADC_x_CONTROL_0 &= ~AUD_BIT_ADC_x_DMIC_MIX_MUTE;
		}
	} else {
		if (type == DMIC) {
			audio_base->CODEC_ADC_CH_CTRL[adc_num - 1].CODEC_ADC_x_CONTROL_0 |= AUD_BIT_ADC_x_DMIC_MIX_MUTE;
		}
	}

}

/**
  * @brief  Enable per dmic channel filter clock.
  * @param  adc_num: adc filter channel
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  newstate: enable or disable per adc filter channel clock.
  *			 This parameter can be one of the following values:
  *			   @arg ENABLE
  *			   @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCDmicFilter(u32 adc_num, u32 newstate)
{
	assert_param(IS_CODEC_ADC_SEL(adc_num));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

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

	default:
		break;

	}

}

/**
  * @brief  Enable and select DMIC clock.
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
	u32 Tmp;

	assert_param(IS_CODEC_DMIC_Clock_SEL(clk));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp = audio_base->CODEC_CLOCK_CONTROL_3;

	if (newstate == ENABLE) {
		Tmp &= ~AUD_MASK_DMIC1_CLK_SEL;
		Tmp |= AUD_BIT_DMIC1_CLK_EN | AUD_DMIC1_CLK_SEL(clk);
	} else {
		Tmp &= ~AUD_BIT_DMIC1_CLK_EN;
	}

	audio_base->CODEC_CLOCK_CONTROL_3 = Tmp;

}

/**
  * @brief  Select DMIC source of input ADC channel.
  * @param  ad_chn: select adc input path channel
  *			 This parameter can be one of the following values:
  *			   @arg ADCHN1
  *			   @arg ADCHN2
  * @param  dmic_num: select DMIC number.
  *			 This parameter can be one of the following values:
  *			   @arg DMIC1
  *			   @arg DMIC2
  * @return  None
  */
void AUDIO_CODEC_SetDmicSrc(u32 ad_chn, u32 dmic_num)
{
	u32 Tmp;

	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_DMIC_SEL(dmic_num));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp = audio_base->CODEC_ADC_CH_CTRL[ad_chn - 1].CODEC_ADC_x_CONTROL_0;
	Tmp &= ~AUD_MASK_ADC_x_DMIC_SRC_SEL;
	Tmp |= AUD_ADC_x_DMIC_SRC_SEL(dmic_num - 1);
	audio_base->CODEC_ADC_CH_CTRL[ad_chn - 1].CODEC_ADC_x_CONTROL_0 = Tmp;

}

/**
  * @brief  Enable or disable adc channel EQ clock.
  * @param  ad_chn: select adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCHN1
  *            @arg ADCHN2
  * @param  newstate: Enable or disable adc channel EQ clock
  *          This parameter can be one of the following values:
  *            @arg ENABLE
  *            @arg DISABLE
  * @return  None
  */
void AUDIO_CODEC_SetADCEQClk(u32 ad_chn, u32 newstate)
{
	u32 Tmp;

	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp = audio_base->CODEC_CLOCK_CONTROL_2;

	switch (ad_chn) {
	case ADCHN1:
		if (newstate == ENABLE) {
			Tmp |= AUD_BIT_AD_0_EQ_EN;
		} else {
			Tmp &= ~AUD_BIT_AD_0_EQ_EN;
		}
		break;

	case ADCHN2:
		if (newstate == ENABLE) {
			Tmp |= AUD_BIT_AD_1_EQ_EN;
		} else {
			Tmp &= ~AUD_BIT_AD_1_EQ_EN;
		}
		break;

	default:
		break;
	}

	audio_base->CODEC_CLOCK_CONTROL_2 = Tmp;
}

/**
  * @brief  Set EQ band as filter for DMIC rx.
  * @param  ad_chn: select adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCHN1
  *            @arg ADCHN2
  * @param  band_sel: select EQ band of each adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCEQBD0
  *            @arg ADCEQBD1
  *            @arg ADCEQBD2
  *            @arg ADCEQBD3
  *            @arg ADCEQBD4
  * @param  EQFilterCoefPoint: it is a struct pointer which points a
  *                              struct that includes coefficients of filter.
  * @return  None
  */
void AUDIO_CODEC_SetADCEQFilter(u32 ad_chn, u32 band_sel, CODEC_EQFilterCoef *EQFilterCoefPoint)
{
	CODEC_EQ_BAND_TypeDef *eq_base_addr = NULL;

	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_ADC_EQ_Band_SEL(band_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	if (ad_chn == ADCHN1) {
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_ADC_0_EQ_BAND;
	} else if (ad_chn == ADCHN2) {
		eq_base_addr = (CODEC_EQ_BAND_TypeDef *)audio_base->CODEC_ADC_1_EQ_BAND;
	}

	if (eq_base_addr) {
		eq_base_addr[band_sel].CODEC_BIQUAD_H0_x = EQFilterCoefPoint->H0_Q;
		eq_base_addr[band_sel].CODEC_BIQUAD_B1_x = EQFilterCoefPoint->B1_Q;
		eq_base_addr[band_sel].CODEC_BIQUAD_B2_x = EQFilterCoefPoint->B2_Q;
		eq_base_addr[band_sel].CODEC_BIQUAD_A1_x = EQFilterCoefPoint->A1_Q;
		eq_base_addr[band_sel].CODEC_BIQUAD_A2_x = EQFilterCoefPoint->A2_Q;
	} else {
		RTK_LOGE(TAG, "please check audio codec register!\n");
	}
}

/**
  * @brief  Enable or disable adc channel EQ band.
  * @param  adc_sel: select adc channel
  *          This parameter can be one of the following values:
  *            @arg ADCHN1
  *            @arg ADCHN2
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
	u32 Tmp1, Tmp2;

	assert_param(IS_CODEC_ADCHN_SEL(ad_chn));
	assert_param(IS_CODEC_ADC_EQ_Band_SEL(band_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp1 = audio_base->CODEC_ADC_0_EQ_CTRL;
	Tmp2 = audio_base->CODEC_ADC_1_EQ_CTRL;

	switch (ad_chn) {
	case ADCHN1:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				Tmp1 |= AUD_BIT_ADC_0_BIQUAD_EN_0;
			} else {
				Tmp1 &= ~AUD_BIT_ADC_0_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				Tmp1 |= AUD_BIT_ADC_0_BIQUAD_EN_1;
			} else {
				Tmp1 &= ~AUD_BIT_ADC_0_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				Tmp1 |= AUD_BIT_ADC_0_BIQUAD_EN_2;
			} else {
				Tmp1 &= ~AUD_BIT_ADC_0_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				Tmp1 |= AUD_BIT_ADC_0_BIQUAD_EN_3;
			} else {
				Tmp1 &= ~AUD_BIT_ADC_0_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				Tmp1 |= AUD_BIT_ADC_0_BIQUAD_EN_4;
			} else {
				Tmp1 &= ~AUD_BIT_ADC_0_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;

	case ADCHN2:
		if (band_sel == ADCEQBD0) {
			if (newstate == ENABLE) {
				Tmp2 |= AUD_BIT_ADC_1_BIQUAD_EN_0;
			} else {
				Tmp2 &= ~AUD_BIT_ADC_1_BIQUAD_EN_0;
			}
		} else if (band_sel == ADCEQBD1) {
			if (newstate == ENABLE) {
				Tmp2 |= AUD_BIT_ADC_1_BIQUAD_EN_1;
			} else {
				Tmp2 &= ~AUD_BIT_ADC_1_BIQUAD_EN_1;
			}
		} else if (band_sel == ADCEQBD2) {
			if (newstate == ENABLE) {
				Tmp2 |= AUD_BIT_ADC_1_BIQUAD_EN_2;
			} else {
				Tmp2 &= ~AUD_BIT_ADC_1_BIQUAD_EN_2;
			}
		} else if (band_sel == ADCEQBD3) {
			if (newstate == ENABLE) {
				Tmp2 |= AUD_BIT_ADC_1_BIQUAD_EN_3;
			} else {
				Tmp2 &= ~AUD_BIT_ADC_1_BIQUAD_EN_3;
			}
		} else if (band_sel == ADCEQBD4) {
			if (newstate == ENABLE) {
				Tmp2 |= AUD_BIT_ADC_1_BIQUAD_EN_4;
			} else {
				Tmp2 &= ~AUD_BIT_ADC_1_BIQUAD_EN_4;
			}
		} else {
			return;
		}
		break;

	default:
		break;
	}

	audio_base->CODEC_ADC_0_EQ_CTRL = Tmp1;
	audio_base->CODEC_ADC_1_EQ_CTRL = Tmp2;

}


/**
  * @brief  Set ADC path zero detection function.
  * @param  channel: select adc channel.
  *			 This parameter can be one of the following values:
  *			   @arg ADC1
  *			   @arg ADC2
  * @param  type: zero detection function select
  *			 This parameter can be one of the following values:
  *			   @arg IME: immediate change
  *			   @arg ZDET_IME: zero detection & immediate change
  *			   @arg ZDET_STEP: zdet & step
  *			   @arg ZDET_TIMEOUT: zdet & timeout step
  */
void AUDIO_CODEC_SetADCZDET(u32 adc_sel, u32 type)
{
	u32 Tmp;

	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp = audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0;
	Tmp &= ~AUD_MASK_ADC_x_AD_ZDET_FUNC;
	Tmp |= AUD_ADC_x_AD_ZDET_FUNC(type);
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 = Tmp;
}

/**
  * @brief	Set ADC path zero detection time out.
  * @param	channel: select adc channel.
  * 		 This parameter can be one of the following values:
  * 		   @arg ADC1
  * 		   @arg ADC2
  * @param	time_out: zero detection time out select
  * 		 This parameter can be one of the following values:
  * 		   @arg 0: 1024*16 samples
  * 		   @arg 1: 1024*32 samples
  * 		   @arg 2: 1024*64 samples
  * 		   @arg 3: 64 samples
  */
void AUDIO_CODEC_SetADCZDETTimeOut(u32 adc_sel, u32 time_out)
{
	u32 Tmp;

	assert_param(IS_CODEC_ADC_SEL(adc_sel));

	AUDIO_CODEC_TypeDef *audio_base = AUDIO_CODEC_GetAddr();

	Tmp = audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0;
	Tmp &= ~AUD_MASK_ADC_x_AD_ZDET_TOUT;
	Tmp |= AUD_ADC_x_AD_ZDET_TOUT(time_out);
	audio_base->CODEC_ADC_CH_CTRL[adc_sel - 1].CODEC_ADC_x_CONTROL_0 = Tmp;

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
	AUDIO_CODEC_SetI2SParameters(i2s_sel, ADCPATH, I2S_InitStruct);

	if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_NOTDM) {
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

	if (type == APP_DMIC_RECORD) {

		AUDIO_CODEC_SetDmicClk(DMIC_2P5M, ENABLE);

		if (I2S_InitStruct->CODEC_SelRxI2STdm == I2S_NOTDM) {
			AUDIO_CODEC_SetADCMixMute(ADC1, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCMixMute(ADC2, DMIC, UNMUTE);
			AUDIO_CODEC_SetADCDmicFilter(ADC1, ENABLE);
			AUDIO_CODEC_SetADCDmicFilter(ADC2, ENABLE);
			AUDIO_CODEC_SetDmicSrc(ADCHN1, DMIC1);
			AUDIO_CODEC_SetDmicSrc(ADCHN2, DMIC2);
		}
	}
}
/**
  * @}
  */

/** @} */

/** @} */

/** @} */