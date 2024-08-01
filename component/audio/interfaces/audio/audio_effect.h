/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from PanKore
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup Audio
 * @{
 *
 * @brief Declares APIs for audio framework.
 *
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_effect.h
 *
 * @brief Provides APIs of the audio effect.
 *
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_FWK_MEDIA_AUDIO_AUDIO_LITE_AUDIO_EFFECT_MODULE_H
#define AMEBA_FWK_MEDIA_AUDIO_AUDIO_LITE_AUDIO_EFFECT_MODULE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct RTAudioEffect;
struct RTEffectAttributesBuilder;

int32_t RTEffectAttributesBuilder_Create(struct RTEffectAttributesBuilder **builder);
void    RTEffectAttributesBuilder_Destroy(struct RTEffectAttributesBuilder *builder);
int32_t RTEffectAttributesBuilder_SetInRate(struct RTEffectAttributesBuilder *builder, uint32_t rate);
int32_t RTEffectAttributesBuilder_SetOutRate(struct RTEffectAttributesBuilder *builder, uint32_t rate);
int32_t RTEffectAttributesBuilder_SetInFormat(struct RTEffectAttributesBuilder *builder, uint32_t format);
int32_t RTEffectAttributesBuilder_SetOutFormat(struct RTEffectAttributesBuilder *builder, uint32_t format);
int32_t RTEffectAttributesBuilder_SetInChannels(struct RTEffectAttributesBuilder *builder, uint32_t channels);
int32_t RTEffectAttributesBuilder_SetOutChannels(struct RTEffectAttributesBuilder *builder, uint32_t channels);
int32_t RTEffectAttributesBuilder_SetFrameCount(struct RTEffectAttributesBuilder *builder, uint32_t frame_count);

int32_t RTAudioEffect_Create(struct RTEffectAttributesBuilder *builder, struct RTAudioEffect **stream);
void    RTAudioEffect_Destroy(struct RTAudioEffect *stream);
int32_t RTAudioEffect_SetConfig(struct RTAudioEffect *stream);
int32_t RTAudioEffect_Start(struct RTAudioEffect *stream);
int32_t RTAudioEffect_Stop(struct RTAudioEffect *stream);
void    RTAudioEffect_SetInBuffer(struct RTAudioEffect *stream, int16_t *buf);
void    RTAudioEffect_SetOutBuffer(struct RTAudioEffect *stream, int16_t *buf);
void    RTAudioEffect_SetFrameCount(struct RTAudioEffect *module, int32_t frame_count);
void    RTAudioEffect_Process(struct RTAudioEffect *stream);
int32_t RTAudioEffect_Command(struct RTAudioEffect *stream, uint32_t cmd_code, uint32_t cmd_size, void *cmd_data, uint32_t *reply_size, void *reply_data);

#ifdef __cplusplus
}
#endif

#endif