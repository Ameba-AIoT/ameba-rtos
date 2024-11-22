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

#ifndef AMEBA_AUDIO_AUDIO_HAL_AMEBALITE_AMEBA_AUDIO_STREAM_UTILS_H
#define AMEBA_AUDIO_AUDIO_HAL_AMEBALITE_AMEBA_AUDIO_STREAM_UTILS_H

#include "ameba.h"

#include "ameba_audio_hw_usrcfg.h"

#include "hardware/audio/audio_hw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_AUDIO_WEAK __attribute__((weak))

void ameba_audio_set_native_time(void);
int64_t ameba_audio_get_now_ns(void);
int32_t ameba_audio_get_channel(uint32_t channel_count);
int32_t ameba_audio_get_sp_tdm(uint32_t channel_count);
int32_t ameba_audio_get_codec_tdm(uint32_t channel_count);
int32_t ameba_audio_get_fifo_num(uint32_t channel_count);
int32_t ameba_audio_stream_get_mic_idx(uint32_t mic_category);
int32_t ameba_audio_stream_get_adc_idx(uint32_t index);
int32_t ameba_audio_stream_get_adc_chn_idx(uint32_t index);
int32_t ameba_audio_get_sp_format(enum AudioHwFormat format, uint32_t direction);
int32_t ameba_audio_get_sp_rate(uint32_t rate);
int32_t ameba_audio_get_codec_format(enum AudioHwFormat format, uint32_t direction);
int32_t ameba_audio_get_codec_rate(uint32_t rate);
AUDIO_SPORT_TypeDef *ameba_audio_get_sport_addr(uint32_t index);
int32_t ameba_audio_get_sport_irq(uint32_t index);
void ameba_audio_set_sp_data_out(uint32_t index);
void ameba_audio_set_sp_data_in(uint32_t index);
void ameba_audio_dump_gdma_regs(u8 GDMA_ChNum);
void ameba_audio_dump_sport_regs(uint32_t SPORTx);
void ameba_audio_dump_codec_regs(void);

#ifdef __cplusplus
}
#endif

#endif