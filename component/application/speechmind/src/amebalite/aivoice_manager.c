#define LOG_TAG "SpeechM"
#include <stdio.h>
#include <string.h>
#include "ameba_soc.h"

#include "log/log.h"
#include "common/audio_errnos.h"
#include "osal_c/osal_mem.h"
#include "osal_c/osal_time.h"

#include "cutils/ring_buffer.h"
#include "media/parcel.h"
#include "aivoice_interface.h"

#include "aivoice_manager.h"

#include "rpc_api.h"
#include "rpc_struct.h"
#include "VoiceRPC_Agent.h"
#include "VoiceRPC_Agent_data.h"
#include "VoiceRPC_System.h"
#include "VoiceRPC_System_data.h"
#include "VoiceRPCBaseDS.h"
#include "VoiceRPCBaseDS_data.h"


//#define DUMP_DATA
#ifdef CONFIG_ARM_CORE_CM4
#define VOICE_RPC_MODE (BLOCK_MODE | (RPC_NP_DSP << 1) | (RPC_DSP_NP << 4))
#elif defined(CONFIG_RSICV_CORE_KR4)
#define VOICE_RPC_MODE (BLOCK_MODE | (RPC_AP_DSP << 1) | (RPC_DSP_AP << 4))
#endif

#if defined(CONFIG_IMQ_EN) && CONFIG_IMQ_EN
extern void ipc_app_init(int32_t task_size, int32_t priority, int32_t log_enable);
#endif

static bool g_dsp_inited = false;

struct AiVoiceManager {
    ring_buffer *mic_ring_buffer;
    AiVoiceManagerCallback callback;
    void* usr_data;
    Parcel *parcel;
};

static HRESULT *AiVoiceManager_aivoiceCallbackProcess(VOICE_RPC_RESULT *pParam, RPC_STRUCT *pRpcStruct, HRESULT *pRes)
{
    (void)pParam;
    (void)pRpcStruct;

    AiVoiceManager *manager = (AiVoiceManager *)pParam->usr_data;
    int event_type = pParam->type;
    void *msg = (void *)pParam->msg_addr;
    uint32_t len = pParam->len;
    DCache_Invalidate((uint32_t)msg, 1024);


    if (manager && manager->callback) {
        manager->callback(event_type, msg, len, manager->usr_data);
    }

    *pRes = 0;
    return pRes;
}

static HRESULT *AiVoiceManager_onState(VOICE_RPC_ERROR_STATE *pParam, RPC_STRUCT *pRpcStruct, HRESULT *pRes)
{
    (void)pParam;
    (void)pRpcStruct;
    *pRes = 0;
    if (pParam->type == 0) {
        g_dsp_inited = true;
        MEDIA_LOGD("OnState Dsp Inited");
    }

    if (pParam->type == 1) {
        AiVoiceManager *manager = (AiVoiceManager *)pParam->data;
        Parcel_Destroy(manager->parcel);
        manager->parcel = NULL;
    }
    return pRes;
}

static void AiVoiceManager_voicePreInit(void)
{
    p_VOICE_RPC_ToSystem_Callback_0_svc = AiVoiceManager_aivoiceCallbackProcess;
    p_VOICE_RPC_ToSystem_State_0_svc = AiVoiceManager_onState;
}

static void AiVoiceManager_adjustTo32Multiple(int num, int *ones, int *twos, int *fours, int *eights) {
    int remainder = num % 32;
    int to_add = 0;

    *ones = 0;
    *twos = 0;
    *fours = 0;
    *eights = 0;

    if (remainder != 0) {
        to_add = 32 - remainder;

        *eights = to_add / 8;
        to_add %= 8;

        *fours = to_add / 4;
        to_add %= 4;

        *twos = to_add / 2;
        *ones = to_add % 2;
    }
}

AiVoiceManager *AiVoiceManager_create(AIVoiceFlowType type, struct aivoice_config *config)
{
    ipc_app_init(1024 * 4, 6, 0);
    while (!RPC_IsInited()) {
        osal_msleep(5);
    }

    AiVoiceManager_voicePreInit();
    while (!g_dsp_inited) {
        osal_msleep(1);
    }

    AiVoiceManager *manager = osal_malloc(sizeof(AiVoiceManager));
    if (!manager) {
        MEDIA_LOGE("malloc aivoice manager fail\n");
        return NULL;
    }

    manager->mic_ring_buffer = ring_buffer_create(1 << 15, RINGBUFFER_IPC);
    if (!manager->mic_ring_buffer) {
        MEDIA_LOGE("create mic local ring buffer fail");
        osal_free(manager);
        return NULL;
    }

    VOICE_RPC_INIT arg;
    arg.voice_iface_flag = type;
    arg.mic_header_addr = (uint32_t)manager->mic_ring_buffer->header;
    arg.mic_header_length = sizeof(ring_buffer_header);
    DCache_Clean((uint32_t)manager->mic_ring_buffer->header, sizeof(ring_buffer_header));

    Parcel *parcel = Parcel_Create();

    //afe config
    Parcel_WriteInt32(parcel, config->afe->mic_array);
    Parcel_WriteInt32(parcel, config->afe->ref_num);
    Parcel_WriteInt32(parcel, config->afe->sample_rate);
    Parcel_WriteInt32(parcel, config->afe->frame_size);
    Parcel_WriteInt32(parcel, config->afe->afe_mode);
    Parcel_WriteBool(parcel, config->afe->enable_aec);
    Parcel_WriteBool(parcel, config->afe->enable_ns);
    Parcel_WriteBool(parcel, config->afe->enable_agc);
    Parcel_WriteBool(parcel, config->afe->enable_ssl);
    Parcel_WriteInt32(parcel, config->afe->aec_mode);
    Parcel_WriteInt32(parcel, config->afe->aec_enable_threshold);
    Parcel_WriteBool(parcel, config->afe->enable_res);
    Parcel_WriteInt32(parcel, config->afe->aec_cost);
    Parcel_WriteInt32(parcel, config->afe->res_aggressive_mode);
    Parcel_WriteInt32(parcel, config->afe->ns_mode);
    Parcel_WriteInt32(parcel, config->afe->ns_cost_mode);
    Parcel_WriteInt32(parcel, config->afe->ns_aggressive_mode);
    Parcel_WriteInt32(parcel, config->afe->agc_fixed_gain);
    Parcel_WriteBool(parcel, config->afe->enable_adaptive_agc);
    Parcel_WriteFloat(parcel, config->afe->ssl_resolution);
    Parcel_WriteInt32(parcel, config->afe->ssl_min_hz);
    Parcel_WriteInt32(parcel, config->afe->ssl_max_hz);

    //vad config
    Parcel_WriteInt32(parcel, config->vad->sensitivity);
    Parcel_WriteUint32(parcel, config->vad->left_margin);
    Parcel_WriteUint32(parcel, config->vad->right_margin);

    //kws config
    int keyword_nums = 2;
    Parcel_WriteInt8(parcel, keyword_nums);
    for (int i = 0; i < keyword_nums; i++) {
        Parcel_WriteCString(parcel, (char *)config->kws->keywords[i]);
        Parcel_WriteFloat(parcel, config->kws->thresholds[i]);
    }
    Parcel_WriteInt32(parcel, config->kws->sensitivity);
    Parcel_WriteInt32(parcel, config->kws->mode);
    Parcel_WriteBool(parcel, config->kws->enable_age_gender);

    //asr config
    Parcel_WriteInt32(parcel, config->asr->sensitivity);

    //aivoice sdk config
    Parcel_WriteInt32(parcel, config->common->timeout);
    Parcel_WriteInt32(parcel, config->common->memory_alloc_mode);

    MEDIA_LOGD("before parcel size %d\n", Parcel_IpcDataSize(parcel));

    bool tmp_one = 1;
    int16_t tmp_two = 2;
    int32_t tmp_four = 3;
    int64_t tmp_eight = 4;
    int data_size = Parcel_IpcDataSize(parcel);
    int ones;
    int twos;
    int fours;
    int eights;
    AiVoiceManager_adjustTo32Multiple(data_size, &ones, &twos, &fours, &eights);
    int i = 0;
    for(; i < eights; i++) {
        Parcel_WriteInt64(parcel, tmp_eight);
    }
    for (i = 0; i < fours; i++) {
        Parcel_WriteInt32(parcel, tmp_four);
    }
    for (i = 0; i < twos; i++) {
        Parcel_WriteInt16(parcel, tmp_two);
    }
    for (i = 0; i < ones; i++) {
        Parcel_WriteBool(parcel, tmp_one);
    }

    MEDIA_LOGD("after parcel size %d\n", Parcel_IpcDataSize(parcel));

    arg.config_addr = (uint32_t)Parcel_IpcData(parcel);
    arg.config_length = Parcel_IpcDataSize(parcel);

    uint8_t* tmp_data = (uint8_t*)arg.config_addr;
    MEDIA_LOGD("%02x %02x %02x %02x %02x %02x",
               tmp_data[0], tmp_data[1], tmp_data[2], tmp_data[3], tmp_data[4], tmp_data[5]);
    DCache_Clean((uint32_t)arg.config_addr, arg.config_length);

    manager->parcel = parcel;

    arg.usr_data = (uint32_t)manager;

    CLNT_STRUCT clnt = RPC_PrepareCLNT(VOICE_RPC_MODE, VOICE_SYSTEM, 0);
    HRESULT *res = VOICE_RPC_ToAgent_Create_0(&arg, &clnt);
    (void) res;

    MEDIA_LOGD("Agent_Init End");

    clnt = RPC_PrepareCLNT(VOICE_RPC_MODE, VOICE_SYSTEM, 0);
    long rpc_param = 1;
    HRESULT *rpc_res = VOICE_RPC_ToAgent_Start_0(&rpc_param, &clnt);
    (void) rpc_res;

    return manager;
}

int32_t AiVoiceManager_feed(AiVoiceManager *manager, const void* data, int len)
{
    if (!manager) {
        MEDIA_LOGE("manager is NULL");
        return AUDIO_ERR_INVALID_PARAM;
    }

    int32_t res = manager->mic_ring_buffer->write(manager->mic_ring_buffer, data, len);
    return res;
}

int32_t AiVoiceManager_destroy(AiVoiceManager *manager)
{
    if (!manager) {
        return AUDIO_ERR_INVALID_PARAM;
    }

    CLNT_STRUCT clnt = RPC_PrepareCLNT(VOICE_RPC_MODE, VOICE_SYSTEM, 0);
    long param = 1;
    HRESULT *res = VOICE_RPC_ToAgent_destroy_0(&param, &clnt);
    (void) res;

    ring_buffer_destroy(manager->mic_ring_buffer);

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
