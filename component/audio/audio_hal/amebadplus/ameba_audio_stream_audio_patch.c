/*
 * Copyright (c) 2024 Realtek, LLC.
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

#include "ameba.h"

#include "ameba_audio_hw_usrcfg.h"
#include "ameba_audio_stream.h"
#include "ameba_audio_stream_utils.h"
#include "ameba_audio_stream_control.h"

#include "audio_hw_osal_errnos.h"

#include "ameba_audio_stream_audio_patch.h"

static void ameba_audio_stream_audio_patch_rx_sport_init(struct AmebaAudioPatchConfig config)
{
    SP_InitTypeDef SP_InitStruct;

    AUDIO_SP_Reset(AUDIO_I2S_IN_SPORT_INDEX);
    AUDIO_SP_StructInit(&SP_InitStruct);
    SP_InitStruct.SP_SelDataFormat = AUDIO_I2S_IN_DATA_FORMAT;
    SP_InitStruct.SP_SelI2SMonoStereo = ameba_audio_get_channel(config.channel_count);
    SP_InitStruct.SP_SelWordLen = ameba_audio_get_sp_format(config.format, STREAM_IN);
    SP_InitStruct.SP_SelTDM = ameba_audio_get_sp_tdm(config.channel_count);
    SP_InitStruct.SP_SelFIFO = ameba_audio_get_fifo_num(config.channel_count);
    SP_InitStruct.SP_SR = ameba_audio_get_sp_rate(config.sample_rate);
#if AUDIO_I2S_IN_MULTIIO_EN
    SP_InitStruct.SP_SetMultiIO = SP_RX_MULTIIO_EN;
#else
    SP_InitStruct.SP_SetMultiIO = SP_RX_MULTIIO_DIS;
#endif
    AUDIO_SP_Init(AUDIO_I2S_IN_SPORT_INDEX, SP_DIR_RX, &SP_InitStruct);

    HAL_AUDIO_INFO("rx sport %d init, %ld, %ld, %ld, %ld, %ld, %ld", AUDIO_I2S_IN_SPORT_INDEX,
                    SP_InitStruct.SP_SelDataFormat, SP_InitStruct.SP_SelI2SMonoStereo,
                    SP_InitStruct.SP_SelWordLen, SP_InitStruct.SP_SelTDM,
                    SP_InitStruct.SP_SelFIFO, SP_InitStruct.SP_SR);
}

static void ameba_audio_stream_audio_patch_tx_sport_init(struct AmebaAudioPatchConfig config)
{
    AUDIO_SP_Reset(AUDIO_I2S_OUT_SPORT_INDEX);
    SP_InitTypeDef SP_InitStruct;
    AUDIO_SP_StructInit(&SP_InitStruct);
    SP_InitStruct.SP_SelWordLen = ameba_audio_get_sp_format(config.format, STREAM_OUT);
    SP_InitStruct.SP_SR = ameba_audio_get_sp_rate(config.sample_rate);
    SP_InitStruct.SP_SelTDM = ameba_audio_get_sp_tdm(config.channel_count);
    SP_InitStruct.SP_SelFIFO = ameba_audio_get_fifo_num(config.channel_count);
#if AUDIO_I2S_OUT_MULTIIO_EN
    SP_InitStruct.SP_SetMultiIO = SP_TX_MULTIIO_EN;
#else
    SP_InitStruct.SP_SetMultiIO = SP_TX_MULTIIO_DIS;
#endif
    AUDIO_SP_Init(AUDIO_I2S_OUT_SPORT_INDEX, SP_DIR_TX, &SP_InitStruct);

    HAL_AUDIO_INFO("tx sport %d init, %ld, %ld, %ld, %ld, %ld, %ld", AUDIO_I2S_OUT_SPORT_INDEX,
                    SP_InitStruct.SP_SelDataFormat, SP_InitStruct.SP_SelI2SMonoStereo,
                    SP_InitStruct.SP_SelWordLen, SP_InitStruct.SP_SelTDM,
                    SP_InitStruct.SP_SelFIFO, SP_InitStruct.SP_SR);
}

static void ameba_audio_stream_audio_patch_codec_init(struct AmebaAudioPatchConfig config)
{
    I2S_InitTypeDef I2S_InitStruct;

    AUDIO_CODEC_I2S_StructInit(&I2S_InitStruct);
    I2S_InitStruct.CODEC_SelRxI2STdm = ameba_audio_get_codec_tdm(config.channel_count);
    I2S_InitStruct.CODEC_SelI2SRxSR = ameba_audio_get_codec_rate(config.sample_rate);
    AUDIO_CODEC_Record(I2S0, APP_DMIC_RECORD, &I2S_InitStruct);

    if (config.sample_rate == 96000) {
        AUDIO_CODEC_SetDmicClk(DMIC_5M, ENABLE);
    } else {
        AUDIO_CODEC_SetDmicClk(DMIC_2P5M, ENABLE);
    }

    AUDIO_CODEC_SetADCVolume(ADC1, 0x00);
    AUDIO_CODEC_SetADCVolume(ADC2, 0x00);
}

int32_t ameba_audio_stream_create_audio_patch(int32_t num_sources, struct AmebaAudioPatchConfig *sources,
                                              int32_t num_sinks, struct AmebaAudioPatchConfig *sinks)
{
    /*FIXME: suport more indexs*/
    int patch_index = 0;

    if (num_sources > 1) {
        HAL_AUDIO_ERROR("only support one source for patch.");
        return HAL_OSAL_ERR_INVALID_PARAM;
    }

    /*FIXME: to support more sinks.*/
    if (num_sinks > 1) {
        HAL_AUDIO_ERROR("only support one sink for patch.");
        return HAL_OSAL_ERR_INVALID_PARAM;
    }

    HAL_AUDIO_INFO("source:%lx, sink:%lx", sources[0].device, sinks[0].device);

    /*
     *for demo board, we need to provde MCLK to amp
     *please set component/soc/amebadplus/usrcfg/ameba_bootcfg.c's Boot_SocClk_Info_Idx for pll.
     *PLL_677P376M for 44.1k.
     *PLL_688P128M for 48k.
     */
    RCC_PeriphClockCmd(APBPeriph_SPORT0, APBPeriph_SPORT0_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_SPORT1, APBPeriph_SPORT1_CLOCK, ENABLE);
    //choose sys pll for sport 0.
    RCC_PeriphClockSource_SPORT(AUDIO_SPORT0_DEV, CKSL_I2S_CPUPLL);
    RCC_PeriphClockSource_SPORT(AUDIO_SPORT1_DEV, CKSL_I2S_CPUPLL);

    if (sinks[0].sample_rate % 8000 == 0) {
        //0: sport 0 output, 1:NI, 8:MI
        AUDIO_SP_SetMclkDiv(0, 1, 8);
        //0: 98.304M, enable 98.304M
        PLL_I2S0_CLK(ENABLE, 0);
    } else {
        //0: sport 0 output, 1:NI, 4:MI
        AUDIO_SP_SetMclkDiv(0, 1, 4);
        //1: 45.1584M, enable 45.1584M
        PLL_I2S0_CLK(ENABLE, 1);
    }

    ameba_audio_stream_audio_patch_rx_sport_init(sources[0]);

    if ((sources[0].device) == AMEBA_AUDIO_IN_MIC) {
        ameba_audio_stream_audio_patch_codec_init(sources[0]);
    }

    if ((sources[0].device) == AMEBA_AUDIO_IN_I2S) {
        ameba_audio_stream_rx_set_i2s_pin(AUDIO_I2S_IN_SPORT_INDEX);
        AUDIO_SP_SetMasterSlave(AUDIO_I2S_IN_SPORT_INDEX, SLAVE);
        RCC_PeriphClockCmd(APBPeriph_AC, APBPeriph_AC_CLOCK, ENABLE);
        if (AUDIO_I2S_IN_SPORT_INDEX == 0)
            AUDIO_CODEC_SetI2SSRC(I2S0, EXTERNAL_I2S);
    }

    AUDIO_SP_RXStart(AUDIO_I2S_IN_SPORT_INDEX, ENABLE);

    if (sinks[0].device == AMEBA_AUDIO_DEVICE_SPEAKER) {
        ameba_audio_ctl_set_amp_state(ameba_audio_get_ctl(), true);
    }

    ameba_audio_stream_tx_set_i2s_pin(AUDIO_I2S_OUT_SPORT_INDEX);

    for (uint32_t i = 0; i < sources[0].channel_count; i ++) {
        /*enable DIRECT_OUT_N*/
        AUDIO_SP_RXSetDirectOutStart(AUDIO_I2S_IN_SPORT_INDEX,
                                     ameba_audio_stream_get_direct_out_channel_idx(i), ENABLE);
    }

    ameba_audio_stream_audio_patch_tx_sport_init(sinks[0]);

    /*direct mode enable*/
    AUDIO_SP_SetDirectOutMode(AUDIO_I2S_IN_SPORT_INDEX, AUDIO_I2S_OUT_SPORT_INDEX);

    for (uint32_t i = 0; i < sinks[0].channel_count; i ++) {
        /*I2S TX data from DIRECT_REG_N*/
        AUDIO_SP_TXCHNSrcSel(AUDIO_I2S_OUT_SPORT_INDEX,
                             ameba_audio_stream_get_sp_tx_channel_idx(i), DIRECT_REG_CHN);

        /*enable sport DIRECT_REG_N*/
        AUDIO_SP_TXSetDirectRegStart(AUDIO_I2S_OUT_SPORT_INDEX,
                                     ameba_audio_stream_get_direct_reg_idx(i), ENABLE);

        /*DIRECT_IN_N->DIRECT_REG_N, DIRECT_IN_N and DIRECT_OUT_N are the same for one sport.*/
        AUDIO_SP_TXDirectRegSel(AUDIO_I2S_OUT_SPORT_INDEX,
          ameba_audio_stream_get_direct_reg_idx(i), ameba_audio_stream_get_direct_in_channel_idx(i));
    }

    AUDIO_SP_TXStart(AUDIO_I2S_OUT_SPORT_INDEX, ENABLE);

    return patch_index;

}

int32_t ameba_audio_stream_release_audio_patch(int32_t patch_index)
{
    HAL_AUDIO_INFO("release patch %ld\n", patch_index);
    AUDIO_SP_TXStart(AUDIO_I2S_OUT_SPORT_INDEX, DISABLE);
    AUDIO_SP_Deinit(AUDIO_I2S_OUT_SPORT_INDEX, SP_DIR_TX);
    AUDIO_SP_TXStart(AUDIO_I2S_IN_SPORT_INDEX, DISABLE);
    AUDIO_SP_Deinit(AUDIO_I2S_IN_SPORT_INDEX, SP_DIR_RX);

    RCC_PeriphClockCmd(APBPeriph_SPORT0, APBPeriph_SPORT0_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_SPORT1, APBPeriph_SPORT1_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_AC, APBPeriph_AC_CLOCK, DISABLE);

    return HAL_OSAL_OK;
}