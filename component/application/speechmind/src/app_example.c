#define LOG_TAG "AppE"
#include <stdio.h>

#include "log/log.h"
#include "common/audio_errnos.h"
#include "osal_c/osal_mem.h"

#include "speech_config.h"
#include "speech_mind.h"

void OnVad(SpeechMindCallback *callback, VadInfo *info)
{
    (void)callback;
    MEDIA_LOGD("vad info status %ld offset_ms %ld", info->status, info->offset_ms);
}

void OnWakeUp(SpeechMindCallback *callback, WakeUpInfo *info)
{
    (void)callback;
    MEDIA_LOGD("wake up info len %ld words %s", info->len, info->wakeup_words);
}

void OnAfe(SpeechMindCallback *callback, AfeInfo *info)
{
    (void)callback;
    (void)info;
}

void OnAsr(SpeechMindCallback *callback, AsrInfo *info)
{
    (void)callback;
    MEDIA_LOGD("asr info id %ld", info->id);
}

void OnAsrRecTimeout(SpeechMindCallback *callback)
{
    (void)callback;
    MEDIA_LOGD("asr timeout");
}

void app_example(void)
{
    SpeechConfig_load();

    SpeechMind_init();

    struct SpeechMindCallback* callback = (struct SpeechMindCallback*)osal_malloc(sizeof(struct SpeechMindCallback));
    if (!callback) {
        MEDIA_LOGE("malloc speechmind callback fail, no memory!");
        SpeechMind_deinit();
        return;
    }
    callback->onVad = OnVad;
    callback->onWakeUp = OnWakeUp;
    callback->onAfe = OnAfe;
    callback->onAsr = OnAsr;
    callback->onAsrRecTimeout = OnAsrRecTimeout;
    SpeechMind_setCallback(callback);

    SpeechMind_start();
}
