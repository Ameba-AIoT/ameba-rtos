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

#include "include/ameba_media_usrcfg.h"

// ----------------------------------------------------------------------
//MediaSourceConfig
extern void *CreateBufferSource(const char *url);
extern void *CreateFileSource(const char *url);
extern void *CreateHTTPSource(const char *url);

#ifdef MEDIA_PLAYER
MediaSourceConfig kMediaSourceConfigs[] = {
    { "buffer://", 9, CreateBufferSource },
    { "lfs://", 6, CreateFileSource },
    { "vfs://", 6, CreateFileSource },
    { "fat://", 6, CreateFileSource },
#if defined(MEDIA_SOURCE_HTTP)
    { "http://", 7, CreateHTTPSource },
    { "https://", 8, CreateHTTPSource },
#endif
};

size_t kNumMediaSourceConfigs =
    sizeof(kMediaSourceConfigs) / sizeof(kMediaSourceConfigs[0]);
#endif


// ----------------------------------------------------------------------
//MediaExtractorConfig
extern void* GetWAVExtractor();
extern void* GetMP3Extractor();
extern void* GetAACExtractor();
extern void* GetMPEG4Extractor();
extern void* GetFLACExtractor();
extern void* GetOGGExtractor();
extern void* GetAMRExtractor();

#ifdef MEDIA_PLAYER
MediaExtractorConfig kMediaExtractorConfigs[] = {
#if defined(MEDIA_DEMUX_WAV)
    { "wav", GetWAVExtractor() },
#endif
#if defined(MEDIA_DEMUX_AAC)
    { "aac", GetAACExtractor() },
#endif
#if defined(MEDIA_DEMUX_MP4)
    { "mp4", GetMPEG4Extractor() },
#endif
#if defined(MEDIA_DEMUX_MP3)
    { "mp3", GetMP3Extractor() },
#endif
#if defined(MEDIA_DEMUX_FLAC)
    { "flac", GetFLACExtractor() },
#endif
#if defined(MEDIA_DEMUX_OGG)
    { "ogg-vorbis", GetOGGExtractor() },
#endif
#if defined(MEDIA_DEMUX_OGG)
    { "ogg-opus", GetOGGExtractor() },
#endif
#if defined(MEDIA_DEMUX_AMR)
    { "amr", GetAMRExtractor() },
#endif
};

size_t kNumMediaExtractorConfigs =
    sizeof(kMediaExtractorConfigs) / sizeof(kMediaExtractorConfigs[0]);
#endif


// ----------------------------------------------------------------------
//MediaDecoderConfig
void *CreatePCMDec(
        const char *name, const void *callbacks,
        void *appData, void **component);

void *CreateMP3Dec(
        const char *name, const void *callbacks,
        void *appData, void **component);

void *CreateHAACDec(
        const char *name, const void *callbacks,
        void *appData, void **component);

void *CreateVorbisDec(
        const char *name, const void *callbacks,
        void *appData, void **component);

void *CreateOpusDec(
        const char *name, const void *callbacks,
        void *appData, void **component);

void *CreateFLACDec(
        const char *name, const void *callbacks,
        void *appData, void **component);

void *CreateFLACEnc(
        const char *name, const void *callbacks,
        void *appData, void **component);

void *CreateSwAmrDec(
        const char *name, const void *callbacks,
        void *app_data, void **component);

void *CreateSwGsmDec(
        const char *name, const void *callbacks,
        void *app_data, void **component);

void *CreateSwG711Dec(
        const char *name, const void *callbacks,
        void *app_data, void **component);

#ifdef MEDIA_PLAYER
MediaDecoderConfig kMediaDecoderConfigs[] = {
#if defined(MEDIA_CODEC_PCM)
    { "wav", CreatePCMDec },
#endif
#if defined(MEDIA_CODEC_MP3)
    { "mp3", CreateMP3Dec },
#endif
#if defined(MEDIA_CODEC_HAAC)
    { "aac", CreateHAACDec },
#endif
#if defined(MEDIA_CODEC_HAAC)
    { "mp4", CreateHAACDec },
#endif
#if defined(MEDIA_CODEC_PCM)
    { "flac", CreatePCMDec },
#endif
#if defined(MEDIA_CODEC_VORBIS)
    { "ogg-vorbis", CreateVorbisDec },
#endif
#if defined(MEDIA_CODEC_OPUS)
    { "ogg-opus", CreateOpusDec },
#endif
#if defined(MEDIA_DEC_FLAC)
    { "flac-decoder", CreateFLACDec },
#endif
#if defined(MEDIA_ENC_FLAC)
    {"flac-encoder", CreateFLACEnc},
#endif
#if defined(MEDIA_CODEC_AMR)
    { "amrnb", CreateSwAmrDec},
#endif
#if defined(MEDIA_CODEC_AMR)
    { "amrwb", CreateSwAmrDec},
#endif
#if defined(MEDIA_CODEC_GSM)
    { "gsm", CreateSwGsmDec},
#endif
#if defined(MEDIA_CODEC_G711)
    { "g711alaw", CreateSwG711Dec},
#endif
#if defined(MEDIA_CODEC_G711)
    { "g711mlaw", CreateSwG711Dec},
#endif
};

size_t kNumMediaDecoderConfigs =
    sizeof(kMediaDecoderConfigs) / sizeof(kMediaDecoderConfigs[0]);
#endif

// ----------------------------------------------------------------------
//MediaAudioOutputConfig
extern void *CreateBTOutput();
extern void *CreateUACOutput();

#ifdef MEDIA_PLAYER
MediaAudioOutputConfig kMediaAudioOutputConfigs[] = {
#if defined(MEDIA_SINK_BT)
    { "a2dp", CreateBTOutput },
#endif
#if defined(MEDIA_SINK_UAC)
    { "uac", CreateUACOutput },
#endif
};

size_t kNumMediaAudioOutputConfigs =
    sizeof(kMediaAudioOutputConfigs) / sizeof(kMediaAudioOutputConfigs[0]);
#endif