/*
 * Copyright (c) 2021 Realtek, LLC.
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

#ifndef AMEBA_COMPONENT_USRCFG_COMMON_MEDIA_USRCFG_H
#define AMEBA_COMPONENT_USRCFG_COMMON_MEDIA_USRCFG_H

#include <stdint.h>
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------
//MediaSourceConfig
typedef void *(*CreateAudioSourceFunc)(const char *);

typedef struct MediaSourceConfig {
    const char *name;
    int prefix_num;
    CreateAudioSourceFunc audio_source_func;
} MediaSourceConfig;

extern MediaSourceConfig kMediaSourceConfigs[];
extern size_t kNumMediaSourceConfigs;


// ----------------------------------------------------------------------
//MediaExtractorConfig
typedef struct MediaExtractorConfig {
	const char *name;
	void *extractor_type;
} MediaExtractorConfig;

extern MediaExtractorConfig kMediaExtractorConfigs[];
extern size_t kNumMediaExtractorConfigs;


// ----------------------------------------------------------------------
//MediaDecoderConfig
typedef void *(*CreateComponentFunc)(
	const char *, const void *,
	void *, void **);

typedef struct MediaDecoderConfig {
	const char *name;
	CreateComponentFunc codec_func;
} MediaDecoderConfig;

extern MediaDecoderConfig kMediaDecoderConfigs[];
extern size_t kNumMediaDecoderConfigs;


// ----------------------------------------------------------------------
//MediaAudioOutputConfig
typedef void *(*CreateAudioOutputFunc)();

typedef struct MediaAudioOutputConfig {
    const char *name;
    CreateAudioOutputFunc audio_output_func;
} MediaAudioOutputConfig;

extern MediaAudioOutputConfig kMediaAudioOutputConfigs[];
extern size_t kNumMediaAudioOutputConfigs;


// ----------------------------------------------------------------------
//MediaCache
extern int64_t kMediaCacheSizeSingleMax;
extern int64_t kMediaCacheSizeTotalMax;
extern int8_t kMediaCacheable;
extern char *kMediaCacheRegionsStart;

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_COMPONENT_USRCFG_COMMON_MEDIA_USRCFG_H
