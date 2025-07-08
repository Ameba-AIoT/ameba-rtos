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

#include "sys_api.h"

#include "ameba_audio_clock.h"
#include "ameba_audio_hw_usrcfg.h"
#include "ameba_audio_stream.h"
#include "ameba_audio_stream_utils.h"
#include "ameba_audio_stream_control.h"

#include "audio_hw_osal_errnos.h"

#include "ameba_audio_stream_audio_patch.h"

extern void PLL_I2S_24P576M(u32 NewState);
extern void AUDIO_SP_SetMclk(u32 index, u32 NewState);
extern void AUDIO_SP_SetMclkDiv(u32 index, u32 mck_div);

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

static void ameba_audio_stream_audio_patch_tx_sport_init_with_clk(struct AmebaAudioPatchConfig config)
{
    AUDIO_ClockParams Clock_Params;
    AUDIO_InitParams Init_Params;
    uint32_t clock_mode;
    uint32_t mck_div;

    AUDIO_SP_Reset(AUDIO_I2S_OUT_SPORT_INDEX);
    SP_InitTypeDef SP_InitStruct;
    AUDIO_SP_StructInit(&SP_InitStruct);
    SP_InitStruct.SP_SelDataFormat = AUDIO_I2S_OUT_DATA_FORMAT;
    SP_InitStruct.SP_SelI2SMonoStereo = ameba_audio_get_channel(config.channel_count);
    SP_InitStruct.SP_SelWordLen = ameba_audio_get_sp_format(config.format, STREAM_OUT);
    SP_InitStruct.SP_SR = ameba_audio_get_sp_rate(config.sample_rate);
    SP_InitStruct.SP_SelTDM = ameba_audio_get_sp_tdm(config.channel_count);
    SP_InitStruct.SP_SelFIFO = ameba_audio_get_fifo_num(config.channel_count);
#if AUDIO_I2S_OUT_MULTIIO_EN
    SP_InitStruct.SP_SetMultiIO = SP_TX_MULTIIO_EN;
#else
    SP_InitStruct.SP_SetMultiIO = SP_TX_MULTIIO_DIS;
#endif

    Init_Params.chn_len = SP_CL_32;
    Init_Params.sr = SP_InitStruct.SP_SR;
#if AUDIO_I2S_OUT_MULTIIO_EN
    Init_Params.chn_cnt = 2;
#else
    Init_Params.chn_cnt = config.channel_count;
#endif

    if (AUDIO_HW_OUT_SPORT_CLK_TYPE == 0) {
        Init_Params.codec_multiplier_with_rate = 0;
        Init_Params.sport_mclk_fixed_max = 0;
        Audio_Clock_Choose(XTAL_CLOCK, &Init_Params, &Clock_Params);
    }

    if (AUDIO_HW_OUT_SPORT_CLK_TYPE == 1) {
        Init_Params.codec_multiplier_with_rate = AUDIO_HW_OUT_MCLK_MULITIPLIER;
        Init_Params.sport_mclk_fixed_max = 0;
        Audio_Clock_Choose(PLL_CLK, &Init_Params, &Clock_Params);
    }

    if (AUDIO_HW_OUT_SPORT_CLK_TYPE == 2) {
        Init_Params.codec_multiplier_with_rate = 0;
        Init_Params.sport_mclk_fixed_max = AUDIO_HW_OUT_FIXED_MCLK;
        Audio_Clock_Choose(PLL_CLK, &Init_Params, &Clock_Params);
    }

    switch (Clock_Params.Clock) {
    case PLL_CLOCK_24P576M:
        PLL_I2S_24P576M(ENABLE);
        RCC_PeriphClockSource_SPORT(AUDIO_I2S_OUT_SPORT_INDEX, CKSL_I2S_PLL24M);
        PLL_I2S_Div(AUDIO_I2S_OUT_SPORT_INDEX, Clock_Params.PLL_DIV);
        clock_mode = PLL_CLOCK_24P576M / Clock_Params.PLL_DIV;
        break;

    case PLL_CLOCK_45P1584M:
        PLL_I2S_45P158M(ENABLE);
        RCC_PeriphClockSource_SPORT(AUDIO_I2S_OUT_SPORT_INDEX, CKSL_I2S_PLL45M);
        PLL_I2S_Div(AUDIO_I2S_OUT_SPORT_INDEX, Clock_Params.PLL_DIV);
        PLL_I2S_45P158M_ClkTune(NULL, 0, PLL_AUTO);
        clock_mode = PLL_CLOCK_45P1584M / Clock_Params.PLL_DIV;
        break;

    case PLL_CLOCK_98P304M:
        PLL_I2S_98P304M(ENABLE);
        RCC_PeriphClockSource_SPORT(AUDIO_I2S_OUT_SPORT_INDEX, CKSL_I2S_PLL98M);
        PLL_I2S_Div(AUDIO_I2S_OUT_SPORT_INDEX, Clock_Params.PLL_DIV);
        PLL_I2S_98P304M_ClkTune(NULL, 0, PLL_AUTO);
        clock_mode = PLL_CLOCK_98P304M / Clock_Params.PLL_DIV;
        break;

    case I2S_CLOCK_XTAL40M:
        clock_mode = I2S_CLOCK_XTAL40M;
        RCC_PeriphClockSource_SPORT(AUDIO_I2S_OUT_SPORT_INDEX, CKSL_I2S_XTAL40M);
        break;
    }

    SP_InitStruct.SP_SelClk = clock_mode;

    AUDIO_SP_Init(AUDIO_I2S_OUT_SPORT_INDEX, SP_DIR_TX, &SP_InitStruct);

    HAL_AUDIO_INFO("tx sport %d init, %ld, %ld, %ld, %ld, %ld, %ld", AUDIO_I2S_OUT_SPORT_INDEX,
                    SP_InitStruct.SP_SelDataFormat, SP_InitStruct.SP_SelI2SMonoStereo,
                    SP_InitStruct.SP_SelWordLen, SP_InitStruct.SP_SelTDM,
                    SP_InitStruct.SP_SelFIFO, SP_InitStruct.SP_SR);

    if (AUDIO_I2S_OUT_NEED_MCLK_OUT == 1) {
        //enable mclk and set div
        AUDIO_SP_SetMclk(AUDIO_I2S_OUT_SPORT_INDEX, ENABLE);
        switch (Clock_Params.MCLK_DIV) {
        case 1:
            mck_div = 2;
            break;
        case 2:
            mck_div = 1;
            break;
        case 4:
            mck_div = 0;
            break;
        }
        AUDIO_SP_SetMclkDiv(AUDIO_I2S_OUT_SPORT_INDEX, mck_div);
    }

    AUDIO_SP_SetMasterSlave(AUDIO_I2S_OUT_SPORT_INDEX, MASTER);

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
    AUDIO_CODEC_Record(I2S1, APP_DMIC_RECORD, &I2S_InitStruct);

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

    RCC_PeriphClockCmd(APBPeriph_SPORT0, APBPeriph_SPORT0_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_SPORT1, APBPeriph_SPORT1_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_SPORT2, APBPeriph_SPORT2_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_SPORT3, APBPeriph_SPORT3_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_AC, APBPeriph_AC_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_AUDIO, APBPeriph_CLOCK_NULL, ENABLE);

    //choose sys pll for sport 0.
    RCC_PeriphClockSource_SPORT(0, CKSL_I2S_XTAL40M);
    RCC_PeriphClockSource_SPORT(1, CKSL_I2S_XTAL40M);
    RCC_PeriphClockSource_SPORT(2, CKSL_I2S_XTAL40M);
    RCC_PeriphClockSource_SPORT(3, CKSL_I2S_XTAL40M);
    RCC_PeriphClockSource_AUDIOCODEC(CKSL_AC_XTAL);

    //need PLL setting?

    ameba_audio_stream_audio_patch_rx_sport_init(sources[0]);

    if ((sources[0].device) == AMEBA_AUDIO_IN_MIC) {
        HAL_AUDIO_INFO("source mic");
        ameba_audio_stream_audio_patch_codec_init(sources[0]);
    }

    if ((sources[0].device) == AMEBA_AUDIO_IN_I2S) {
        HAL_AUDIO_INFO("source i2s");
        if (AUDIO_I2S_IN_NEED_MCLK_OUT) {
            Pinmux_Config(AUDIO_I2S_IN_MCLK_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_IN_SPORT_INDEX));
        }
        Pinmux_Config(AUDIO_I2S_IN_BCLK_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_IN_SPORT_INDEX));
        Pinmux_Config(AUDIO_I2S_IN_LRCLK_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_IN_SPORT_INDEX));
        Pinmux_Config(AUDIO_I2S_IN_DATA0_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_IN_SPORT_INDEX));
        if (AUDIO_I2S_IN_MULTIIO_EN) {
            Pinmux_Config(AUDIO_I2S_IN_DATA1_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_IN_SPORT_INDEX));
            Pinmux_Config(AUDIO_I2S_IN_DATA2_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_IN_SPORT_INDEX));
            Pinmux_Config(AUDIO_I2S_IN_DATA3_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_IN_SPORT_INDEX));
        }
        HAL_AUDIO_INFO("set I2S IN slave");
        AUDIO_SP_SetMasterSlave(AUDIO_I2S_IN_SPORT_INDEX, SLAVE);
    }

    AUDIO_SP_RXStart(AUDIO_I2S_IN_SPORT_INDEX, ENABLE);

    I2S_InitTypeDef i2s_initstruct;
    AUDIO_CODEC_I2S_StructInit(&i2s_initstruct);
    i2s_initstruct.CODEC_SelI2STxSR = ameba_audio_get_codec_rate(sinks[0].sample_rate);
    i2s_initstruct.CODEC_SelI2STxWordLen = ameba_audio_get_codec_format(sinks[0].format, STREAM_OUT);

    switch (sinks[0].device)
    {
    case AMEBA_AUDIO_DEVICE_SPEAKER:
        HAL_AUDIO_INFO("set speaker");
        AUDIO_CODEC_SetAudioIP(ENABLE);
        AUDIO_CODEC_SetLDOMode(POWER_ON);

        AUDIO_CODEC_Playback(I2S0, APP_LINE_OUT, &i2s_initstruct);
        AUDIO_CODEC_SetDACASRC(i2s_initstruct.CODEC_SelI2STxSR, ENABLE);

        ameba_audio_set_audio_ip_use_status(STREAM_OUT, CODEC, true);
        ameba_audio_ctl_set_amp_state(ameba_audio_get_ctl(), ameba_audio_get_ctl()->amp_state);
        break;
    case AMEBA_AUDIO_DEVICE_I2S:
        HAL_AUDIO_INFO("set I2S out");
        sys_jtag_off();
        if (AUDIO_I2S_OUT_NEED_MCLK_OUT) {
            Pinmux_Config(AUDIO_I2S_OUT_MCLK_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_OUT_SPORT_INDEX));
        }
        Pinmux_Config(AUDIO_I2S_OUT_BCLK_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_OUT_SPORT_INDEX));
        Pinmux_Config(AUDIO_I2S_OUT_LRCLK_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_OUT_SPORT_INDEX));
        Pinmux_Config(AUDIO_I2S_OUT_DATA0_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_OUT_SPORT_INDEX));
        if (AUDIO_I2S_OUT_MULTIIO_EN == 1) {
            Pinmux_Config(AUDIO_I2S_OUT_DATA1_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_OUT_SPORT_INDEX));
            Pinmux_Config(AUDIO_I2S_OUT_DATA2_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_OUT_SPORT_INDEX));
            Pinmux_Config(AUDIO_I2S_OUT_DATA3_PIN, ameba_audio_get_i2s_pin_func(AUDIO_I2S_OUT_SPORT_INDEX));
        }
        break;
    case AMEBA_AUDIO_DEVICE_HEADPHONE:
        HAL_AUDIO_INFO("set headphone");
        AUDIO_CODEC_SetAudioIP(ENABLE);
        AUDIO_CODEC_SetLDOMode(POWER_ON);

        AUDIO_CODEC_Playback(I2S0, APP_HPO_OUT, &i2s_initstruct);
        AUDIO_CODEC_SetDACASRC(i2s_initstruct.CODEC_SelI2STxSR, ENABLE);

        ameba_audio_set_audio_ip_use_status(STREAM_OUT, CODEC, true);
        ameba_audio_ctl_set_amp_state(ameba_audio_get_ctl(), ameba_audio_get_ctl()->amp_state);
        break;
    default:
        HAL_AUDIO_ERROR("unsupported sink type:%ld", sinks[0].device);
        break;
    }

    for (uint32_t i = 0; i < sources[0].channel_count; i ++) {
        /*enable DIRECT_OUT_N*/
        AUDIO_SP_RXSetDirectOutStart(AUDIO_I2S_IN_SPORT_INDEX,
                                     ameba_audio_stream_get_direct_out_channel_idx(i), ENABLE);
    }

    if (sinks[0].device == AMEBA_AUDIO_DEVICE_I2S) {
        ameba_audio_stream_audio_patch_tx_sport_init_with_clk(sinks[0]);
    } else {
        ameba_audio_stream_audio_patch_tx_sport_init(sinks[0]);
    }

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
        AUDIO_SP_TXDirectRegSel(AUDIO_I2S_OUT_SPORT_INDEX, AUDIO_I2S_IN_SPORT_INDEX,
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
    RCC_PeriphClockCmd(APBPeriph_SPORT2, APBPeriph_SPORT0_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_SPORT3, APBPeriph_SPORT1_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_AC, APBPeriph_AC_CLOCK, DISABLE);

    ameba_audio_set_audio_ip_use_status(STREAM_OUT, CODEC, false);

    return HAL_OSAL_OK;
}
