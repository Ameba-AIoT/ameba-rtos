#define LOG_TAG "SpeechT"

#include <stdio.h>
#include <string.h>

#include "log/log.h"
#include "common/audio_errnos.h"
#include "osal_c/osal_mem.h"
#include "osal_c/osal_time.h"
#include "osal_c/osal_thread.h"
#include "media/parcel.h"

#include "cutils/ring_buffer.h"
#include "aivoice_interface.h"

#include "vfs.h"

#include "speech_config.h"
#include "aivoice_manager.h"

struct AiVoiceManager {
    void *ai_voice_handle;
    const struct rtk_aivoice_iface *aivoice;
    AiVoiceManagerCallback callback;
    void* usr_data;
};

int AiVoiceManager_aivoiceCallbackProcess(void *userdata,
        enum aivoice_out_event_type event_type,
        const void *msg, int len)
{
    AiVoiceManager *manager = (AiVoiceManager *)userdata;
    if (manager && manager->callback) {
        manager->callback(event_type, msg, len, manager->usr_data);
    }

    return 0;
}

AiVoiceManager *AiVoiceManager_create(AIVoiceFlowType type, struct aivoice_config *config)
{
    AiVoiceManager *manager = osal_malloc(sizeof(AiVoiceManager));
    if (!manager) {
        MEDIA_LOGE("malloc aivoice manager fail\n");
        return NULL;
    }

    manager->callback = NULL;

    (void) type;

    // Trimmed code: removed switch, only keep FULL_FLOW_V1 type
    manager->aivoice = &aivoice_iface_full_flow_v1;

    // switch (type) {
    // case FULL_FLOW_V1:
    //     manager->aivoice = &aivoice_iface_full_flow_v1;
    //     break;

    // case AFE_KWS_V1:
    //     manager->aivoice = &aivoice_iface_afe_kws_v1;
    //     break;

    // case AFE_KWS_VAD_V1:
    //     manager->aivoice = &aivoice_iface_afe_kws_vad_v1;
    //     break;

    // case AFE_V1:
    //     manager->aivoice = &aivoice_iface_afe_v1;
    //     break;

    // case VAD_V1:
    //     manager->aivoice = &aivoice_iface_vad_v1;
    //     break;

    // case KWS_V1:
    //     manager->aivoice = &aivoice_iface_kws_v1;
    //     break;

    // case ASR_V1:
    //     manager->aivoice = &aivoice_iface_asr_v1;
    //     break;

    // default:
    //     manager->aivoice = &aivoice_iface_full_flow_v1;
    //     break;
    // }

    manager->ai_voice_handle = manager->aivoice->create(config);

    rtk_aivoice_register_callback(manager->ai_voice_handle, AiVoiceManager_aivoiceCallbackProcess, manager);

    return manager;
}


int32_t AiVoiceManager_feed(AiVoiceManager *manager, const void* data, int len)
{
    if (!manager) {
        MEDIA_LOGE("manager is NULL");
        return 0;
    }

    int32_t res = manager->aivoice->feed(manager->ai_voice_handle,
                                         (char *)data, len);

    return res;
}

int32_t AiVoiceManager_destroy(AiVoiceManager *manager)
{
    if (!manager) {
        return AUDIO_ERR_INVALID_PARAM;
    }

    return AUDIO_OK;
}

int32_t AiVoiceManager_setCallback(AiVoiceManager *manager, AiVoiceManagerCallback callback, void* usr_data)
{
    if (!manager) {
        return AUDIO_ERR_INVALID_PARAM;
    }

    manager->callback = callback;
    manager->usr_data = usr_data;

    return AUDIO_OK;
}
