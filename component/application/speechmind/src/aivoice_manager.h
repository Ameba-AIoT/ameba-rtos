#ifndef _SPEECH_MIND_SPEECH_MANAGER_
#define _SPEECH_MIND_SPEECH_MANAGER_

#include <stdint.h>
#include "aivoice_interface.h"

typedef struct AiVoiceManager AiVoiceManager;

typedef enum AIVoiceFlowType {
    FULL_FLOW_V1,
    AFE_KWS_V1,
    AFE_KWS_VAD_V1,
    AFE_V1,
    VAD_V1,
    KWS_V1,
    ASR_V1,
} AIVoiceFlowType;

typedef void (*AiVoiceManagerCallback)( enum aivoice_out_event_type event_type,
                                        const void *msg, int len, void* user_data);

AiVoiceManager *AiVoiceManager_create(AIVoiceFlowType type, struct aivoice_config *config);
int32_t AiVoiceManager_feed(AiVoiceManager *manager, const void *data, int len);
int32_t AiVoiceManager_destroy(AiVoiceManager *manager);
int32_t AiVoiceManager_setCallback(AiVoiceManager *manager, AiVoiceManagerCallback callback, void* usr_data);

#endif //_SPEECH_MIND_SPEECH_MANAGER_
