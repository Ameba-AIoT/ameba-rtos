#define LOG_TAG "SpeechM"

#include <stdlib.h>
#include <string.h>

#include "log/log.h"
#include "common/audio_errnos.h"
#include "osal_c/osal_mem.h"
#include "osal_c/osal_time.h"
#include "osal_c/osal_thread.h"

#include "audio/audio_service.h"
#include "audio/audio_control.h"

#include "foundation/alooper_manager.h"

#include "aivoice_manager.h"
#include "speech_mind.h"
#include "speech_config.h"
#include "audio_capture.h"
#include "playlist_parser.h"
#include "audio_dump.h"
#include "speech_utils.h"

#define ENABLE_SSL_DOA 1

typedef struct SpeechMind {
    AiVoiceManager *manager;
    SpeechMindCallback *speechmind_cb;
    AiVoiceManagerCallback aivoice_cb;
    bool speech_task_running;
    osal_thread_t *speech_thread;
    AudioCapture* audio_capture;
    AudioCaptureCallback capture_cb;
    PlayListParser* parser;
    AudioDump* audio_dump;
    uint8_t dump_data[DUMP_FRAME_BYTES];
    void* origin_data;
} SpeechMind;

static SpeechMind *g_speech_mind = NULL;

static void Asr_result(int id) {
    if (g_speech_mind->speechmind_cb) {
        AsrInfo info;
        info.id = id;
        g_speech_mind->speechmind_cb->onAsr(g_speech_mind->speechmind_cb, &info);
    }
}

void SpeechMind_managerCallback(enum aivoice_out_event_type event_type,
                                const void *msg, int len, void* usr_data)
{
    struct aivoice_evout_vad *vad_out;
    struct aivoice_evout_afe *afe_out;
    static bool vad_feed = false;
    static bool wakeup_triggered = false;
    (void)msg;
    (void)len;

    SpeechMind* speech_mind = (SpeechMind*)usr_data;

    switch (event_type) {
    case AIVOICE_EVOUT_VAD:
        vad_out = (struct aivoice_evout_vad *)msg;
        MEDIA_LOGD("[user] vad. status = %d, offset = %d", vad_out->status, vad_out->offset_ms);
        if (!vad_out->status && vad_out->offset_ms >= VAD_TIMEOUT_MS) {
            MEDIA_LOGD("[user] vad timeout, reset");
            vad_feed = false;
        }

        if (g_speech_mind->speechmind_cb) {
            VadInfo info;
            info.status = vad_out->status;
            info.offset_ms = vad_out->offset_ms;
            g_speech_mind->speechmind_cb->onVad(g_speech_mind->speechmind_cb, &info);
        }

        break;

    case AIVOICE_EVOUT_WAKEUP:
        MEDIA_LOGD("wake up");
        vad_feed = true;
        wakeup_triggered = true;

#if ENABLE_SSL_DOA == 0
        PlayListParser_wakeUp(speech_mind->parser, PLAYLIST_PAUSE);
#endif
        if (g_speech_mind->speechmind_cb) {
            WakeUpInfo info;
            info.len = len;
            info.wakeup_words = msg;
            g_speech_mind->speechmind_cb->onWakeUp(g_speech_mind->speechmind_cb, &info);
        }
        break;

    case AIVOICE_EVOUT_ASR_RESULT:
        PlayListParser_asrMsg(speech_mind->parser, (void*)msg, len, Asr_result);
        break;

    case AIVOICE_EVOUT_AFE:
        afe_out = (struct aivoice_evout_afe *)msg;

        // afe will output audio each frame.
        // in this example, we only print it once to make log clear
        if (wakeup_triggered) {
            wakeup_triggered = false;
            MEDIA_LOGD("[user] afe output %d channels raw audio, others: %s",
                       afe_out->ch_num, afe_out->out_others_json?afe_out->out_others_json:"null");
#if ENABLE_SSL_DOA
            PlayListParser_sslMsg(speech_mind->parser, (void*)afe_out->out_others_json, strlen(afe_out->out_others_json));
#endif
        }

        if (vad_feed && speech_mind->audio_dump) {
            for (int j = 0; j < FRAME_MS * 16; j++) {
                ((short *)(speech_mind->dump_data))[4 * j] = ((short *)speech_mind->origin_data)[3 * j];
                ((short *)(speech_mind->dump_data))[4 * j + 1] = ((short *)speech_mind->origin_data)[3 * j + 1];
                ((short *)(speech_mind->dump_data))[4 * j + 2] = ((short *)speech_mind->origin_data)[3 * j + 2]; //ref
                ((short *)(speech_mind->dump_data))[4 * j + 3] = ((short *)(afe_out->data))[j]; //vad out
            }
            AudioDump_dump(speech_mind->audio_dump, speech_mind->dump_data, DUMP_FRAME_BYTES);
        }

        if (g_speech_mind->speechmind_cb) {
            AfeInfo info;
            info.len = len;
            info.data = (uint8_t*)msg;
            g_speech_mind->speechmind_cb->onAfe(g_speech_mind->speechmind_cb, &info);
        }

        break;

    case AIVOICE_EVOUT_ASR_REC_TIMEOUT:
        PlayListParser_vadTimeOut(g_speech_mind->parser);
        if (g_speech_mind->speechmind_cb) {
            g_speech_mind->speechmind_cb->onAsrRecTimeout(g_speech_mind->speechmind_cb);
        }
        break;

    default:
        break;
    }
}

void SpeechMind_captureCallback(const void *data, int len, void* usr_data)
{
    (void) usr_data;
    if (g_speech_mind && g_speech_mind->manager) {
        AiVoiceManager_feed(g_speech_mind->manager, data, len);
        g_speech_mind->origin_data = (void*)data;
    }
}

int32_t SpeechMind_init(void)
{
    if (g_speech_mind) {
        return AUDIO_ERR_INVALID_OPERATION;
    }

    g_speech_mind = (SpeechMind *)osal_calloc(sizeof(SpeechMind));
    if (!g_speech_mind) {
        MEDIA_LOGE("Create SpeechMind failed");
        return NULL;
    }

    g_speech_mind->parser = PlayListParser_create();
    if (!g_speech_mind->parser) {
        MEDIA_LOGE("Create playlist parser fail");
        osal_free(g_speech_mind);
        g_speech_mind = NULL;
        return NULL;
    }

#ifdef SD_DUMP
    g_speech_mind->audio_dump = AudioDump_create(AUDIO_DUMP_SD_CARD);
    if (!g_speech_mind->audio_dump) {
        PlayListParser_destroy(g_speech_mind->parser);
        osal_free(g_speech_mind);
        g_speech_mind = NULL;
        MEDIA_LOGE("AudioDump_create failed");
        return false;
    }
#elif defined(PC_RECORD)
    g_speech_mind->audio_dump = AudioDump_create(AUDIO_DUMP_PC_RECORD);
    if (!g_speech_mind->audio_dump) {
        PlayListParser_destroy(g_speech_mind->parser);
        osal_free(g_speech_mind);
        g_speech_mind = NULL;
        MEDIA_LOGE("AudioDump_create failed");
        return false;
    }
#else
    g_speech_mind->audio_dump = NULL;
#endif

    ALooperManager_init();

    RTAudioService_Init();

    // set volume
    float volume = SpeechConfig_getAmplifierVolume();
    RTAudioControl_SetHardwareVolume(volume, volume);

    return AUDIO_OK;
}


/* workaround function for AmebaLite compilation issue */
__attribute__((weak)) afe_ns_mode_e AFE_NS_SIGNAL_SET(void) {
    return AFE_NS_SIGNAL;
}

static bool SpeechMind_speechTask(void *param)
{
    MEDIA_LOGI("%s Enter\n", __FUNCTION__);
    (void)param;
    osal_msleep(1000);
    memset(g_speech_mind->dump_data, 0, DUMP_FRAME_BYTES);

    g_speech_mind->audio_capture = AudioCapture_create(SAMPLE_RATE, IN_CHANNEL);
    if (!g_speech_mind->audio_capture) {
        MEDIA_LOGE("set AiVoiceManagerCallback failed");
        return false;
    }

    g_speech_mind->capture_cb = SpeechMind_captureCallback;
    int res = AudioCapture_setCallback(g_speech_mind->audio_capture, g_speech_mind->capture_cb, NULL);

    AIVoiceFlowType type = FULL_FLOW_V1;
    struct aivoice_config config;
    memset(&config, 0, sizeof(config));
    struct afe_config afe_param = AFE_CONFIG_ASR_DEFAULT_2MIC50MM();
#if ENABLE_SSL_DOA
    afe_param.enable_ssl = true;
    afe_param.ssl_resolution = 15;
#else
    afe_param.enable_ssl = false;
#endif

    struct vad_config vad_param = VAD_CONFIG_DEFAULT();
    vad_param.left_margin = 300; // user can change the configure if needed
    struct kws_config kws_param = KWS_CONFIG_DEFAULT();
    struct asr_config asr_param = ASR_CONFIG_DEFAULT();
    struct aivoice_sdk_config aivoice_param = AIVOICE_SDK_CONFIG_DEFAULT();
    aivoice_param.timeout = 10;

    config.afe = &afe_param;
    config.vad = &vad_param;
    config.kws = &kws_param;
    config.asr = &asr_param;
    config.common = &aivoice_param;

    g_speech_mind->manager = AiVoiceManager_create(type, &config);
    if (!g_speech_mind->manager) {
        MEDIA_LOGE("%s fail, create aivoice manager fail!\n", __FUNCTION__);
        AudioCapture_destroy(g_speech_mind->audio_capture);
        return false;
    }

    g_speech_mind->aivoice_cb = SpeechMind_managerCallback;

    res = AiVoiceManager_setCallback(g_speech_mind->manager, g_speech_mind->aivoice_cb, g_speech_mind);
    if (res) {
        AudioCapture_destroy(g_speech_mind->audio_capture);
        AiVoiceManager_destroy(g_speech_mind->manager);
        MEDIA_LOGE("set AiVoiceManagerCallback failed");
        return false;
    }

    AudioCapture_start(g_speech_mind->audio_capture);

    osal_sleep(1);

    if (g_speech_mind->audio_dump) {
        AudioDump_start(g_speech_mind->audio_dump);
    }

    return false;
}

int32_t SpeechMind_start(void)
{
    MEDIA_LOGD("%s Enter\n", __FUNCTION__);
    if (!g_speech_mind) {
        MEDIA_LOGE("%s fail, speech mind not init!\n", __FUNCTION__);
        return AUDIO_ERR_INVALID_OPERATION;
    }

    g_speech_mind->speech_task_running = true;
    osal_thread_param param;
    param.priority = OSAL_THREAD_PRI_AUDIO;
    param.stack_size = 1024 * 10;
    param.joinable = false;
    param.name = (char *)"SpeechTask";
    int32_t res = osal_thread_create(&g_speech_mind->speech_thread, SpeechMind_speechTask, NULL, &param);
    if (res) {
        MEDIA_LOGE("create SpeechTask fail");
        return AUDIO_ERR_NO_MEMORY;
    }

    return AUDIO_OK;
}

int32_t SpeechMind_stop(void)
{
    MEDIA_LOGV("%s Enter\n", __FUNCTION__);
    if (!g_speech_mind) {
        MEDIA_LOGE("%s fail, speech mind not init!\n", __FUNCTION__);
        return AUDIO_ERR_INVALID_OPERATION;
    }

    if (g_speech_mind->speech_thread) {
        g_speech_mind->speech_task_running = false;
        osal_thread_request_exitAndWait(g_speech_mind->speech_thread);
        g_speech_mind->speech_thread = NULL;
    }

    AiVoiceManager_setCallback(g_speech_mind->manager, NULL, NULL);
    osal_free(g_speech_mind->aivoice_cb);

    AiVoiceManager_destroy(g_speech_mind->manager);
    g_speech_mind->manager = NULL;
    return AUDIO_OK;
}

int32_t SpeechMind_setCallback(SpeechMindCallback *callback)
{
    MEDIA_LOGV("%s Enter\n", __FUNCTION__);
    if (!g_speech_mind) {
        MEDIA_LOGE("%s fail, speech mind not init!\n", __FUNCTION__);
        return AUDIO_ERR_INVALID_OPERATION;
    }

    g_speech_mind->speechmind_cb = callback;
    return AUDIO_OK;
}

int32_t SpeechMind_deinit(void)
{
    if (g_speech_mind->parser) {
        PlayListParser_destroy(g_speech_mind->parser);
        g_speech_mind->parser = NULL;
    }

    if (g_speech_mind->audio_dump) {
        AudioDump_destroy(g_speech_mind->audio_dump);
        g_speech_mind->audio_dump = NULL;
    }

    if (g_speech_mind) {
        osal_free(g_speech_mind);
        g_speech_mind = NULL;
    }
    return AUDIO_OK;
}
