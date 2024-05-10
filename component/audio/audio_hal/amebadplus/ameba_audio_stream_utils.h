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

#ifndef AMEBA_COMPONENT_AUDIO_AUDIO_HAL_RTL8721DA_AMEBA_AUDIO_STREAM_UTILS_H
#define AMEBA_COMPONENT_AUDIO_AUDIO_HAL_RTL8721DA_AMEBA_AUDIO_STREAM_UTILS_H

#include "ameba_audio_hw_usrcfg.h"
#include "ameba.h"
#include "basic_types.h"
#include "platform_stdlib.h"
#include "hardware/audio/audio_hw_types.h"

#define SYSTICK_CURRENT *(__IO uint32_t *)0xE000E018
#define SYSTICK_RELOAD  *(__IO uint32_t *)0xE000E014
#define WL_REG_BASE                                  0x40000000
#define REG_P0_TSFTR_L                               0x0560
#define REG_P0_TSFTR_H                               0x0564
#define REG_P1_TSFTR_L                               0x0568
#define REG_P1_TSFTR_H                               0x056C
#define REG_WL_0x1BC                                 0x1BC
#define REG_WL_0x1B8                                 0x1B8
#define REG_WL_0x94                                  0x94
#define REG_WL_0x1104                                0x1104
#define REG_WL_0x1C0                                 0x1C0
#define REG_AUDIO_TSFT_CTRL0                         0x1584
#define BIT_INC_I2S_TSF_WL                           ((u32)0x00000001 << 3)
#define BIT_CLR_I2S_TSF_WL                           ((u32)0x00000001 << 2)

#if AUDIO_I2S_OUT_SPORT_INDEX
#define AUDIO_SPORT_TX_CTRL0                         0x4012B004
#else
#define AUDIO_SPORT_TX_CTRL0                         0x4012A004
#endif

#if AUDIO_I2S_IN_SPORT_INDEX
#define AUDIO_SPORT_RX_LRCLK_ADDR                    0x4012B030
#else
#define AUDIO_SPORT_RX_LRCLK_ADDR                    0x4012A030
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_CLINTWOOD) && CONFIG_CLINTWOOD
#include "user_heap.h"
#define ameba_audio_gdma_calloc User_Heap_calloc
#define ameba_audio_gdma_free User_free
#else
#define ameba_audio_gdma_calloc calloc
#define ameba_audio_gdma_free free
#endif

uint32_t ameba_audio_get_channel(uint32_t channel_count);
uint32_t ameba_audio_get_sp_tdm(uint32_t channel_count);
uint32_t ameba_audio_get_codec_tdm(uint32_t channel_count);
uint32_t ameba_audio_get_fifo_num(uint32_t channel_count);
uint32_t ameba_audio_stream_get_mic_idx(uint32_t mic_category);
uint32_t ameba_audio_stream_get_adc_idx(uint32_t index);
uint32_t ameba_audio_stream_get_adc_chn_idx(uint32_t index);
int ameba_audio_get_sp_format(AudioHwFormat format, uint32_t direction);
int ameba_audio_get_sp_rate(uint32_t rate);
int ameba_audio_get_codec_format(AudioHwFormat format, uint32_t direction);
int ameba_audio_get_codec_rate(uint32_t rate);
AUDIO_SPORT_TypeDef *ameba_audio_get_sport_addr(uint32_t index);
uint32_t ameba_audio_get_sport_irq(uint32_t index);
void ameba_audio_set_sp_data_out(uint32_t index);
void ameba_audio_set_sp_data_in(uint32_t index);
void ameba_audio_dump_gdma_regs(u8 GDMA_ChNum);
void ameba_audio_dump_sport_regs(uint32_t SPORTx);
void ameba_audio_dump_codec_regs(void);
uint64_t ameba_audio_get_wmac_ns(void);
int64_t ameba_audio_get_audio_ns(void);
void ameba_audio_reset_latch_audio(void);

static inline u32 ameba_audio_sp_get_rx_sport0_phase_val(void)
{
	return (AUDIO_SPORT0_DEV->SP_RX_COUNTER2) & SP_MASK_RX_FS_PHASE_RPT;
}

static inline u32 ameba_audio_sp_get_rx_sport1_phase_val(void)
{
	return (AUDIO_SPORT1_DEV->SP_RX_COUNTER2) & SP_MASK_RX_FS_PHASE_RPT;
}

static inline u32 ameba_audio_sp_get_sport0_counter(void)
{
	return ((AUDIO_SPORT0_DEV->SP_RX_COUNTER2) & SP_MASK_RX_SPORT_COUNTER) >> 5;
}

static inline u32 ameba_audio_sp_get_sport1_counter(void)
{
	return ((AUDIO_SPORT1_DEV->SP_RX_COUNTER2) & SP_MASK_RX_SPORT_COUNTER) >> 5;
}

static inline void ameba_audio_stream_rx_latch_phase(void)
{
	(*((u32 *)(AUDIO_SPORT_RX_LRCLK_ADDR))) |= SP_BIT_EN_FS_PHASE_LATCH;
}

static inline uint32_t ameba_audio_stream_rx_get_phase(void)
{
#if (AUDIO_I2S_IN_SPORT_INDEX == 0)
	return ameba_audio_sp_get_rx_sport0_phase_val();
#endif
#if (AUDIO_I2S_IN_SPORT_INDEX == 1)
	return ameba_audio_sp_get_rx_sport1_phase_val();
#endif
}

static inline uint32_t ameba_audio_stream_rx_get_counter(void)
{
#if (AUDIO_I2S_IN_SPORT_INDEX == 0)
	return ameba_audio_sp_get_sport0_counter();
#endif
#if (AUDIO_I2S_IN_SPORT_INDEX == 1)
	return ameba_audio_sp_get_sport1_counter();
#endif
}

static inline void ameba_audio_sport_start(void)
{
	(*((u32 *)AUDIO_SPORT_TX_CTRL0)) &= ~SP_BIT_TX_DISABLE;
	(*((u32 *)AUDIO_SPORT_TX_CTRL0)) |= SP_BIT_START_TX;
}

static inline void ameba_audio_start_latch_audio(void)
{
	u32 value32 = HAL_READ32(WL_REG_BASE, REG_AUDIO_TSFT_CTRL0);
	value32 |= BIT_INC_I2S_TSF_WL;
	HAL_WRITE32(WL_REG_BASE, REG_AUDIO_TSFT_CTRL0, value32);
}

static inline int64_t ameba_audio_fast_get_p0_ns(void)
{
	u32 tsf_l = HAL_READ32(WL_REG_BASE, REG_P0_TSFTR_L);
	u32 tsf_h = HAL_READ32(WL_REG_BASE, REG_P0_TSFTR_H);
	return (((u64)tsf_h) << 32 | tsf_l) * 1000LL;
}

static inline int64_t ameba_audio_fast_get_p1_ns(void)
{
	u32 tsf_l = HAL_READ32(WL_REG_BASE, REG_P1_TSFTR_L);
	u32 tsf_h = HAL_READ32(WL_REG_BASE, REG_P1_TSFTR_H);
	return (((u64)tsf_h) << 32 | tsf_l) * 1000LL;
}

static inline uint32_t ameba_audio_fast_get_p1_l(void)
{
	return HAL_READ32(WL_REG_BASE, REG_P1_TSFTR_L);
}

static inline uint32_t ameba_audio_fast_get_p1_h(void)
{
	return HAL_READ32(WL_REG_BASE, REG_P1_TSFTR_H);
}

#ifdef __cplusplus
}
#endif

#endif
