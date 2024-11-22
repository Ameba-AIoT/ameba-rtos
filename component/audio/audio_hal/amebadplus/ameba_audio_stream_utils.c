/*
 * Copyright (c) 2022 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "basic_types.h"

#include "ameba.h"
#include "ameba_audio_stream.h"
#include "ameba_audio_types.h"

#include "audio_hw_debug.h"
#include "audio_hw_osal_errnos.h"

#include "ameba_audio_stream_utils.h"

int32_t ameba_audio_get_channel(uint32_t channel_count)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel_count) {
	case 1:
		tmp = SP_CH_MONO;
		break;
	case 2:
	case 4:
	case 6:
	case 8:
		tmp = SP_CH_STEREO;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_sp_tdm(uint32_t channel_count)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel_count) {
	case 1:
	case 2:
		tmp = SP_TX_NOTDM;
		break;
	case 4:
		tmp = SP_TX_TDM4;
		break;
	case 6:
		tmp = SP_TX_TDM6;
		break;
	case 8:
		tmp = SP_TX_TDM8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_codec_tdm(uint32_t channel_count)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel_count) {
	case 1:
	case 2:
		tmp = I2S_NOTDM;
		break;
	// case 4:
	// 	tmp = I2S_TDM4;
	// 	break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_fifo_num(uint32_t channel_count)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel_count) {
	case 1:
	case 2:
		tmp = SP_RX_FIFO2;
		break;
	case 4:
		tmp = SP_RX_FIFO4;
		break;
	case 6:
		tmp = SP_RX_FIFO6;
		break;
	case 8:
		tmp = SP_RX_FIFO8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_sp_format(enum AudioHwFormat format, uint32_t direction)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (format) {
	case AUDIO_HW_FORMAT_PCM_32_BIT:
		if (direction == STREAM_OUT) {
			tmp = SP_TXWL_32;
		} else {
			tmp = SP_RXWL_32;
		}
		break;
	case AUDIO_HW_FORMAT_PCM_8_24_BIT:
		if (direction == STREAM_OUT) {
			tmp = SP_TXWL_24;
		} else {
			tmp = SP_RXWL_24;
		}
		break;
	case AUDIO_HW_FORMAT_PCM_16_BIT:
		if (direction == STREAM_OUT) {
			tmp = SP_TXWL_16;
		} else {
			tmp = SP_RXWL_16;
		}
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_sp_rate(uint32_t rate)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (rate) {
	case 8000:
		tmp = SP_8K;
		break;
	case 11025:
		tmp = SP_11P025K;
		break;
	case 16000:
		tmp = SP_16K;
		break;
	case 22050:
		tmp = SP_22P05K;
		break;
	case 24000:
		tmp = SP_24K;
		break;
	case 32000:
		tmp = SP_32K;
		break;
	case 44100:
		tmp = SP_44P1K;
		break;
	case 48000:
		tmp = SP_48K;
		break;
	case 88200:
		tmp = SP_88P2K;
		break;
	case 96000:
		tmp = SP_96K;
		break;
	case 192000:
		tmp = SP_192K;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid rate");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_codec_format(enum AudioHwFormat format, uint32_t direction)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	(void)direction;
	switch (format) {
	case AUDIO_HW_FORMAT_PCM_8_24_BIT:
		tmp = WL_24;
		break;
	case AUDIO_HW_FORMAT_PCM_16_BIT:
		tmp = WL_16;
		break;
	case AUDIO_HW_FORMAT_PCM_8_BIT:
		tmp = WL_8;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid format");
		break;
	}
	return tmp;
}

int32_t ameba_audio_get_codec_rate(uint32_t rate)
{
	int32_t tmp = HAL_OSAL_ERR_INVALID_PARAM;
	switch (rate) {
	case 8000:
		tmp = SR_8K;
		break;
	case 11025:
		tmp = SR_11P025K;
		break;
	case 16000:
		tmp = SR_16K;
		break;
	case 22050:
		tmp = SR_22P05K;
		break;
	case 24000:
		tmp = SR_24K;
		break;
	case 32000:
		tmp = SR_32K;
		break;
	case 44100:
		tmp = SR_44P1K;
		break;
	case 48000:
		tmp = SR_48K;
		break;
	case 88200:
		tmp = SR_88P2K;
		break;
	case 96000:
		tmp = SR_96K;
		break;
	case 192000:
		tmp = SR_192K;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] invalid rate");
		break;
	}
	return tmp;
}

int32_t ameba_audio_stream_get_mic_idx(uint32_t mic_category)
{
	uint32_t mic_num = HAL_OSAL_ERR_INVALID_PARAM;
	switch (mic_category) {
	case AMEBA_AUDIO_DMIC1:
		mic_num = DMIC1;
		break;
	case AMEBA_AUDIO_DMIC2:
		mic_num = DMIC2;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] mic category %ld not supported", mic_category);
		break;
	}

	return mic_num;
}

int32_t ameba_audio_stream_get_adc_chn_idx(uint32_t index)
{
	uint32_t adc_chn = HAL_OSAL_ERR_INVALID_PARAM;
	switch (index) {
	case 1:
		adc_chn = ADCHN1;
		break;
	case 2:
		adc_chn = ADCHN2;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] adc channel index: %ld not supported", index);
		break;
	}

	return adc_chn;
}

int32_t ameba_audio_stream_get_adc_idx(uint32_t index)
{
	uint32_t adc_num = HAL_OSAL_ERR_INVALID_PARAM;
	switch (index) {
	case 1:
		adc_num = ADC1;
		break;
	case 2:
		adc_num = ADC2;
		break;
	default:
		HAL_AUDIO_ERROR("[AmebaAudioUtils] adc index: %ld not supported", index);
		break;
	}

	return adc_num;
}

void ameba_audio_dump_gdma_regs(u8 GDMA_ChNum)
{
	GDMA_TypeDef *GDMA = ((GDMA_TypeDef *)GDMA_BASE);

	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].SAR:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].SAR);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].DAR:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].DAR);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].CTL_LOW:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].CTL_LOW);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].CTL_HIGH:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].CTL_HIGH);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].CFG_LOW:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].CFG_LOW);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].CFG_HIGH:%lx", GDMA_ChNum, GDMA->CH[GDMA_ChNum].CFG_HIGH);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].ChEnReg:%lx", GDMA_ChNum, GDMA->ChEnReg);

	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].MASK_TFR:%lx", GDMA_ChNum, GDMA->MASK_TFR);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].MASK_BLOCK:%lx", GDMA_ChNum, GDMA->MASK_BLOCK);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].MASK_ERR:%lx", GDMA_ChNum, GDMA->MASK_ERR);
	HAL_AUDIO_DUMP_INFO("GDMA->CH[%d].STATUS_BLOCK:%lx", GDMA_ChNum, GDMA->STATUS_BLOCK);

}

void ameba_audio_dump_sport_regs(uint32_t SPORTx)
{
	uint32_t tmp;
	AUDIO_SPORT_TypeDef *sportx = (AUDIO_SPORT_TypeDef *)SPORTx;
	HAL_AUDIO_DUMP_INFO("dump sportx:0x%p", sportx);
	tmp = sportx->SP_REG_MUX;
	HAL_AUDIO_DUMP_INFO("REG_SP_REG_MUX:%lx", tmp);
	tmp = sportx->SP_CTRL0;
	HAL_AUDIO_DUMP_INFO("REG_SP_CTRL0:%lx", tmp);
	tmp = sportx->SP_CTRL1;
	HAL_AUDIO_DUMP_INFO("REG_SP_CTRL1:%lx", tmp);
	tmp = sportx->SP_INT_CTRL;
	HAL_AUDIO_DUMP_INFO("REG_SP_INT_CTRL:%lx", tmp);
	tmp = sportx->RSVD0;
	HAL_AUDIO_DUMP_INFO("REG_RSVD0:%lx", tmp);
	tmp = sportx->SP_TRX_COUNTER_STATUS;
	HAL_AUDIO_DUMP_INFO("REG_SP_TRX_COUNTER_STATUS:%lx", tmp);
	tmp = sportx->SP_ERR;
	HAL_AUDIO_DUMP_INFO("REG_SP_ERR:%lx", tmp);
	tmp = sportx->SP_SR_TX_BCLK;
	HAL_AUDIO_DUMP_INFO("REG_SP_SR_TX_BCLK:%lx", tmp);
	tmp = sportx->SP_TX_LRCLK;
	HAL_AUDIO_DUMP_INFO("REG_SP_TX_LRCLK:%lx", tmp);
	tmp = sportx->SP_FIFO_CTRL;
	HAL_AUDIO_DUMP_INFO("REG_SP_FIFO_CTRL:%lx", tmp);
	tmp = sportx->SP_FORMAT;
	HAL_AUDIO_DUMP_INFO("REG_SP_FORMAT:%lx", tmp);
	tmp = sportx->SP_RX_BCLK;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_BCLK:%lx", tmp);
	tmp = sportx->SP_RX_LRCLK;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_LRCLK:%lx", tmp);
	tmp = sportx->SP_DSP_COUNTER;
	HAL_AUDIO_DUMP_INFO("REG_SP_DSP_COUNTER:%lx", tmp);
	tmp = sportx->RSVD1;
	HAL_AUDIO_DUMP_INFO("REG_RSVD1:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL0;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL0:%lx", tmp);
	tmp = sportx->RSVD2;
	HAL_AUDIO_DUMP_INFO("REG_RSVD2:%lx", tmp);
	tmp = sportx->SP_FIFO_IRQ;
	HAL_AUDIO_DUMP_INFO("REG_SP_FIFO_IRQ:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL1;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL1:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL2;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL2:%lx", tmp);
	tmp = sportx->RSVD3;
	HAL_AUDIO_DUMP_INFO("REG_RSVD3:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL3;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL3:%lx", tmp);
	tmp = sportx->SP_DIRECT_CTRL4;
	HAL_AUDIO_DUMP_INFO("REG_SP_DIRECT_CTRL4:%lx", tmp);
	tmp = sportx->SP_RX_COUNTER1;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_COUNTER1:%lx", tmp);
	tmp = sportx->SP_RX_COUNTER2;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_COUNTER2:%lx", tmp);
	tmp = sportx->SP_TX_FIFO_0_WR_ADDR;
	HAL_AUDIO_DUMP_INFO("REG_SP_TX_FIFO_0_WR_ADDR:%lx", tmp);
	tmp = sportx->SP_RX_FIFO_0_RD_ADDR;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_FIFO_0_RD_ADDR:%lx", tmp);
	tmp = sportx->SP_TX_FIFO_1_WR_ADDR;
	HAL_AUDIO_DUMP_INFO("REG_SP_TX_FIFO_1_WR_ADDR:%lx", tmp);
	tmp = sportx->SP_RX_FIFO_1_RD_ADDR;
	HAL_AUDIO_DUMP_INFO("REG_SP_RX_FIFO_1_RD_ADDR:%lx", tmp);

}

void ameba_audio_dump_codec_regs(void)
{
}

AUDIO_SPORT_TypeDef *ameba_audio_get_sport_addr(uint32_t index)
{
	AUDIO_SPORT_TypeDef *addr;
	switch (index) {
	case 0:
		addr = AUDIO_SPORT0_DEV;
		break;
	case 1:
		addr = AUDIO_SPORT1_DEV;
		break;
	default:
		HAL_AUDIO_ERROR("unsupported sport:%lu", index);
		addr = NULL;
		break;
	}
	return addr;
}

int32_t ameba_audio_get_sport_irq(uint32_t index)
{
	uint32_t irq = HAL_OSAL_ERR_INVALID_PARAM;
	switch (index) {
	case 0:
		irq = SPORT0_IRQ;
		break;
	case 1:
		irq = SPORT1_IRQ;
		break;
	default:
		HAL_AUDIO_ERROR("unsupported sport:%lu", index);
		break;
	}
	return irq;
}

void ameba_audio_set_sp_data_out(uint32_t index)
{
	uint32_t tmp;
	tmp = HAL_READ32(PINMUX_REG_BASE, REG_I2S_CTRL);
	if (index == 1) {
		tmp |= PAD_BIT_SP1_DIO0_MUXSEL | PAD_BIT_SP1_DIO1_MUXSEL | PAD_BIT_SP1_DIO2_MUXSEL | PAD_BIT_SP1_DIO3_MUXSEL;
	} else if (index == 0) {
		tmp |= PAD_BIT_SP0_DIO0_MUXSEL | PAD_BIT_SP0_DIO1_MUXSEL | PAD_BIT_SP0_DIO2_MUXSEL | PAD_BIT_SP0_DIO3_MUXSEL;
	}
	HAL_WRITE32(PINMUX_REG_BASE, REG_I2S_CTRL, tmp);
}

void ameba_audio_set_sp_data_in(uint32_t index)
{
	uint32_t tmp;
	tmp = HAL_READ32(PINMUX_REG_BASE, REG_I2S_CTRL);
	if (index == 1) {
		tmp &= ~(PAD_BIT_SP1_DIO0_MUXSEL | PAD_BIT_SP1_DIO1_MUXSEL | PAD_BIT_SP1_DIO2_MUXSEL | PAD_BIT_SP1_DIO3_MUXSEL);
	} else if (index == 0) {
		tmp &= ~(PAD_BIT_SP0_DIO0_MUXSEL | PAD_BIT_SP0_DIO1_MUXSEL | PAD_BIT_SP0_DIO2_MUXSEL | PAD_BIT_SP0_DIO3_MUXSEL);
	}
	HAL_WRITE32(PINMUX_REG_BASE, REG_I2S_CTRL, tmp);
}

void ameba_audio_stream_rx_set_i2s_pin(uint32_t index)
{
	switch (index) {
	case 0:
		if (AUDIO_I2S_IN_MCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_MCLK_PIN, PINMUX_FUNCTION_I2S0_MCLK);
		}
		if (AUDIO_I2S_IN_BCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_BCLK_PIN, PINMUX_FUNCTION_I2S0_BCLK);
		}
		if (AUDIO_I2S_IN_LRCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_LRCLK_PIN, PINMUX_FUNCTION_I2S0_WS);
		}

		ameba_audio_set_sp_data_in(0);

		if (AUDIO_I2S_IN_DATA0_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_DATA0_PIN, PINMUX_FUNCTION_I2S0_DIO0);
		}

		if (AUDIO_I2S_IN_MULTIIO_EN) {
			if (AUDIO_I2S_IN_DATA1_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_DATA1_PIN, PINMUX_FUNCTION_I2S0_DIO1);
			}
			if (AUDIO_I2S_IN_DATA2_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_DATA2_PIN, PINMUX_FUNCTION_I2S0_DIO2);
			}
			if (AUDIO_I2S_IN_DATA3_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_DATA3_PIN, PINMUX_FUNCTION_I2S0_DIO3);
			}
		}
		break;
	case 1:
		if (AUDIO_I2S_IN_MCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_MCLK_PIN, PINMUX_FUNCTION_I2S1_MCLK);
		}
		if (AUDIO_I2S_IN_BCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_BCLK_PIN, PINMUX_FUNCTION_I2S1_BCLK);
		}
		if (AUDIO_I2S_IN_LRCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_LRCLK_PIN, PINMUX_FUNCTION_I2S1_WS);
		}

		ameba_audio_set_sp_data_in(1);

		if (AUDIO_I2S_IN_DATA0_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_DATA0_PIN, PINMUX_FUNCTION_I2S1_DIO0);
		}

		if (AUDIO_I2S_IN_MULTIIO_EN) {
			// HAL_AUDIO_INFO("PB_21:%" PRId32 "\n", Pinmux_ConfigGet(AUDIO_I2S_IN_DATA1_PIN));
			// HAL_AUDIO_INFO("PB_22:%" PRId32 "\n", Pinmux_ConfigGet(AUDIO_I2S_IN_DATA2_PIN));
			// HAL_AUDIO_INFO("PB_3:%" PRId32 "\n", Pinmux_ConfigGet(AUDIO_I2S_IN_DATA3_PIN));
			if (AUDIO_I2S_IN_DATA1_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_DATA1_PIN, PINMUX_FUNCTION_I2S1_DIO1);
			}
			if (AUDIO_I2S_IN_DATA2_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_DATA2_PIN, PINMUX_FUNCTION_I2S1_DIO2);
			}
			if (AUDIO_I2S_IN_DATA3_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_DATA3_PIN, PINMUX_FUNCTION_I2S1_DIO3);
			}
		}
		break;
	default:
		break;
	}

}

void ameba_audio_stream_rx_set_i2s_extra_pin(uint32_t index)
{
	switch (index) {
	case 0:
		if (AUDIO_I2S_IN_EXTRA_MCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_EXTRA_MCLK_PIN, PINMUX_FUNCTION_I2S0_MCLK);
		}
		if (AUDIO_I2S_IN_EXTRA_BCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_EXTRA_BCLK_PIN, PINMUX_FUNCTION_I2S0_BCLK);
		}
		if (AUDIO_I2S_IN_EXTRA_LRCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_EXTRA_LRCLK_PIN, PINMUX_FUNCTION_I2S0_WS);
		}

		ameba_audio_set_sp_data_in(0);

		if (AUDIO_I2S_IN_EXTRA_DATA0_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_EXTRA_DATA0_PIN, PINMUX_FUNCTION_I2S0_DIO0);
		}
		if (AUDIO_I2S_IN_EXTRA_MULTIIO_EN) {
			if (AUDIO_I2S_IN_EXTRA_DATA1_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_EXTRA_DATA1_PIN, PINMUX_FUNCTION_I2S0_DIO1);
			}
			if (AUDIO_I2S_IN_EXTRA_DATA2_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_EXTRA_DATA2_PIN, PINMUX_FUNCTION_I2S0_DIO2);
			}
			if (AUDIO_I2S_IN_EXTRA_DATA3_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_EXTRA_DATA3_PIN, PINMUX_FUNCTION_I2S0_DIO3);
			}
		}
		break;
	case 1:
		if (AUDIO_I2S_IN_EXTRA_MCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_EXTRA_MCLK_PIN, PINMUX_FUNCTION_I2S1_MCLK);
		}
		if (AUDIO_I2S_IN_EXTRA_BCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_EXTRA_BCLK_PIN, PINMUX_FUNCTION_I2S1_BCLK);
		}
		if (AUDIO_I2S_IN_EXTRA_LRCLK_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_EXTRA_LRCLK_PIN, PINMUX_FUNCTION_I2S1_WS);
		}

		ameba_audio_set_sp_data_in(1);

		if (AUDIO_I2S_IN_EXTRA_DATA0_PIN != -1) {
			Pinmux_Config(AUDIO_I2S_IN_EXTRA_DATA0_PIN, PINMUX_FUNCTION_I2S1_DIO0);
		}
		if (AUDIO_I2S_IN_EXTRA_MULTIIO_EN) {
			// HAL_AUDIO_INFO("PB_21:%" PRId32 "\n", Pinmux_ConfigGet(AUDIO_I2S_IN_DATA1_PIN));
			// HAL_AUDIO_INFO("PB_22:%" PRId32 "\n", Pinmux_ConfigGet(AUDIO_I2S_IN_DATA2_PIN));
			// HAL_AUDIO_INFO("PB_3:%" PRId32 "\n", Pinmux_ConfigGet(AUDIO_I2S_IN_DATA3_PIN));
			if (AUDIO_I2S_IN_EXTRA_DATA1_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_EXTRA_DATA1_PIN, PINMUX_FUNCTION_I2S1_DIO1);
			}
			if (AUDIO_I2S_IN_EXTRA_DATA2_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_EXTRA_DATA2_PIN, PINMUX_FUNCTION_I2S1_DIO2);
			}
			if (AUDIO_I2S_IN_EXTRA_DATA3_PIN != -1) {
				Pinmux_Config(AUDIO_I2S_IN_EXTRA_DATA3_PIN, PINMUX_FUNCTION_I2S1_DIO3);
			}
		}
		break;
	default:
		break;
	}

}

void ameba_audio_stream_tx_set_i2s_pin(uint32_t index)
{
	switch (index) {
	case 0:
		#if AUDIO_I2S_OUT_MCLK_PIN != -1
		Pinmux_Config(AUDIO_I2S_OUT_MCLK_PIN, PINMUX_FUNCTION_I2S0_MCLK);
		#endif

		#if AUDIO_I2S_OUT_BCLK_PIN != -1
		Pinmux_Config(AUDIO_I2S_OUT_BCLK_PIN, PINMUX_FUNCTION_I2S0_BCLK);
		#endif

		#if AUDIO_I2S_OUT_LRCLK_PIN != -1
		Pinmux_Config(AUDIO_I2S_OUT_LRCLK_PIN, PINMUX_FUNCTION_I2S0_WS);
		#endif

		ameba_audio_set_sp_data_out(0);

		#if AUDIO_I2S_OUT_DATA0_PIN != -1
		Pinmux_Config(AUDIO_I2S_OUT_DATA0_PIN, PINMUX_FUNCTION_I2S0_DIO3);
		#endif

		#if AUDIO_I2S_OUT_MULTIIO_EN == 1
			#if AUDIO_I2S_OUT_DATA1_PIN != -1
			Pinmux_Config(AUDIO_I2S_OUT_DATA1_PIN, PINMUX_FUNCTION_I2S0_DIO2);
			#endif

			#if AUDIO_I2S_OUT_DATA2_PIN != -1
			Pinmux_Config(AUDIO_I2S_OUT_DATA2_PIN, PINMUX_FUNCTION_I2S0_DIO1);
			#endif

			#if AUDIO_I2S_OUT_DATA3_PIN != -1
			Pinmux_Config(AUDIO_I2S_OUT_DATA3_PIN, PINMUX_FUNCTION_I2S0_DIO0);
			#endif
		#endif
		break;
	case 1:
		#if AUDIO_I2S_OUT_MCLK_PIN != -1
		Pinmux_Config(AUDIO_I2S_OUT_MCLK_PIN, PINMUX_FUNCTION_I2S1_MCLK);
		#endif

		#if AUDIO_I2S_OUT_BCLK_PIN != -1
		Pinmux_Config(AUDIO_I2S_OUT_BCLK_PIN, PINMUX_FUNCTION_I2S1_BCLK);
		#endif

		#if AUDIO_I2S_OUT_LRCLK_PIN != -1
		Pinmux_Config(AUDIO_I2S_OUT_LRCLK_PIN, PINMUX_FUNCTION_I2S1_WS);
		#endif

		ameba_audio_set_sp_data_out(1);

		#if AUDIO_I2S_OUT_DATA0_PIN != -1
		Pinmux_Config(AUDIO_I2S_OUT_DATA0_PIN, PINMUX_FUNCTION_I2S1_DIO3);
		#endif

		#if AUDIO_I2S_OUT_MULTIIO_EN == 1
			#if AUDIO_I2S_OUT_DATA1_PIN != -1
			Pinmux_Config(AUDIO_I2S_OUT_DATA1_PIN, PINMUX_FUNCTION_I2S1_DIO2);
			#endif

			#if AUDIO_I2S_OUT_DATA2_PIN != -1
			Pinmux_Config(AUDIO_I2S_OUT_DATA2_PIN, PINMUX_FUNCTION_I2S1_DIO1);
			#endif

			#if AUDIO_I2S_OUT_DATA3_PIN != -1
			Pinmux_Config(AUDIO_I2S_OUT_DATA3_PIN, PINMUX_FUNCTION_I2S1_DIO0);
			#endif
		#endif
		break;
	default:
		break;
	}
}

int32_t ameba_audio_stream_get_direct_out_channel_idx(uint32_t channel)
{
	uint32_t direct_out_channel = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel)
	{
	case 0:
		direct_out_channel = DIRECT_OUT_CHN0;
		break;
	case 1:
		direct_out_channel = DIRECT_OUT_CHN1;
		break;
	case 2:
		direct_out_channel = DIRECT_OUT_CHN2;
		break;
	case 3:
		direct_out_channel = DIRECT_OUT_CHN3;
		break;
	case 4:
		direct_out_channel = DIRECT_OUT_CHN4;
		break;
	case 5:
		direct_out_channel = DIRECT_OUT_CHN5;
		break;
	case 6:
		direct_out_channel = DIRECT_OUT_CHN6;
		break;
	case 7:
		direct_out_channel = DIRECT_OUT_CHN7;
		break;
	default:
		HAL_AUDIO_ERROR("channel not supported for direct out:%ld", channel);
		break;
	}

	return direct_out_channel;
}

int32_t ameba_audio_stream_get_direct_in_channel_idx(uint32_t channel)
{
	uint32_t direct_in_channel = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel)
	{
	case 0:
		direct_in_channel = DIRECT_IN_CHN0;
		break;
	case 1:
		direct_in_channel = DIRECT_IN_CHN1;
		break;
	case 2:
		direct_in_channel = DIRECT_IN_CHN2;
		break;
	case 3:
		direct_in_channel = DIRECT_IN_CHN3;
		break;
	case 4:
		direct_in_channel = DIRECT_IN_CHN4;
		break;
	case 5:
		direct_in_channel = DIRECT_IN_CHN5;
		break;
	case 6:
		direct_in_channel = DIRECT_IN_CHN6;
		break;
	case 7:
		direct_in_channel = DIRECT_IN_CHN7;
		break;
	default:
		HAL_AUDIO_ERROR("channel not supported for direct in:%ld", channel);
		break;
	}

	return direct_in_channel;
}

int32_t ameba_audio_stream_get_direct_reg_idx(uint32_t channel)
{
	uint32_t direct_reg = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel)
	{
	case 0:
		direct_reg = DIRECT_REG_0;
		break;
	case 1:
		direct_reg = DIRECT_REG_1;
		break;
	case 2:
		direct_reg = DIRECT_REG_2;
		break;
	case 3:
		direct_reg = DIRECT_REG_3;
		break;
	case 4:
		direct_reg = DIRECT_REG_4;
		break;
	case 5:
		direct_reg = DIRECT_REG_5;
		break;
	case 6:
		direct_reg = DIRECT_REG_6;
		break;
	case 7:
		direct_reg = DIRECT_REG_7;
		break;
	default:
		HAL_AUDIO_ERROR("channel not supported for direct reg:%ld", channel);
		break;
	}

	return direct_reg;
}

int32_t ameba_audio_stream_get_sp_tx_channel_idx(uint32_t channel)
{
	uint32_t sp_tx_channel = HAL_OSAL_ERR_INVALID_PARAM;
	switch (channel)
	{
	case 0:
		sp_tx_channel = TXCHN0;
		break;
	case 1:
		sp_tx_channel = TXCHN1;
		break;
	case 2:
		sp_tx_channel = TXCHN2;
		break;
	case 3:
		sp_tx_channel = TXCHN3;
		break;
	case 4:
		sp_tx_channel = TXCHN4;
		break;
	case 5:
		sp_tx_channel = TXCHN5;
		break;
	case 6:
		sp_tx_channel = TXCHN6;
		break;
	case 7:
		sp_tx_channel = TXCHN7;
		break;
	default:
		HAL_AUDIO_ERROR("channel not supported for direct reg:%ld", channel);
		break;
	}

	return sp_tx_channel;
}