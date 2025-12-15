#define LOG_TAG "PlayList"
#include "log/log.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "osal_c/osal_errnos.h"
#include "osal_c/osal_mem.h"
#include "osal_c/osal_mutex.h"
#include "osal_c/osal_thread.h"
#include "osal_c/osal_time.h"
#include "osal_c/osal_condition.h"

#include "cutils/container_of.h"

#include "foundation/amessage.h"
#include "foundation/alooper.h"
#include "foundation/ahandler.h"

#include "music_player.h"

#include "playlist_parser.h"

#include "cJSON/cJSON.h"
#include "speech_tts.h"

#define WAKE_UP_AUDIO_FILE "vfs://tts/10001.mp3"
#define VAD_TIMEOUT_AUDIO_FILE "vfs://tts/10002.mp3"
#define PLAY_MUSIC_INTENT_ID "29" // modify to "-1" to disable play music
#define STOP_MUSIC_INTENT_ID "30" // modify to "-1" to disable stop music

#define MAX_TTS_CONTENT_LEN (64)
#define MAX_SUB_TTS (10)

enum {
    kWhatWakeUp          = 'waku',
    kWhatAsrMsg          = 'asrm',
    kWhatSslMsg          = 'sslm',
    kWhatVadTimeOut      = 'vadt',
    kWhatAddSong         = 'adds',
    kWhatTTSCallback     = 'tcak',
    KWhatListEmpty       = 'liem',
};

enum {
    MUSIC_IDLE,
    MUSIC_PLAY,
    MUSIC_PAUSE,
};

typedef struct PlayListParser {
    MusicPlayer *music_player;
    MusicPlayer *tts_player;
    AHandler handler;
    ALooper *looper;
    int32_t handler_id;
    AMessage *tts_notify;
    int music_status;
    asr_result cb;
} PlayListParser;

void PlayListParser_wakeUp(PlayListParser *playlist_parser, InterruptType type)
{
    MEDIA_LOGD("send wake up");
    AMessage *msg = AMessage_create(kWhatWakeUp, &playlist_parser->handler);
    AMessage_setInt32(msg, "type", type);
    AMessage_post(msg, 0);
    AMessage_put(msg);
}

void PlayListParser_onWakeUp(PlayListParser *playlist_parser, AMessage *msg)
{
    MEDIA_LOGD("on wake up");
    int32_t type = 0;
    AMessage_findInt32(msg, "type", &type);
    if (type == PLAYLIST_PAUSE) {
        MusicPlayer_pause(playlist_parser->music_player);
        playlist_parser->music_status = MUSIC_PAUSE;
    } else {
        MusicPlayer_stop(playlist_parser->music_player, true);
        playlist_parser->music_status = MUSIC_IDLE;
    }

    MusicPlayer_stop(playlist_parser->tts_player, true);
    MusicPlayer_addSong(playlist_parser->tts_player, WAKE_UP_AUDIO_FILE);
}

void PlayListParser_sslMsg(PlayListParser *playlist_parser, void *data, int32_t length)
{
    MEDIA_LOGD("send ssl msg");
    AMessage *msg = AMessage_create(kWhatSslMsg, &playlist_parser->handler);
    void *temp_data = (void *)osal_malloc(length);
    memcpy(temp_data, data, length);
    AMessage_setPointer(msg, "data", temp_data);
    AMessage_setInt32(msg, "length", length);
    AMessage_post(msg, 0);
    AMessage_put(msg);
}

void PlayListParser_onSslMsg(PlayListParser *playlist_parser, AMessage *msg)
{
    MEDIA_LOGD("on ssl msg");

    char tts_url[1024] = {0};
    void *data;
    int32_t length;

    MusicPlayer_pause(playlist_parser->music_player);
    playlist_parser->music_status = MUSIC_PAUSE;

    AMessage_findPointer(msg, "data", &data);
    AMessage_findInt32(msg, "length", &length);

    cJSON *root_obj = cJSON_Parse((char *)data);
    if (root_obj != NULL) {
        cJSON *ssl_angle_obj = cJSON_GetObjectItem(root_obj, "ssl_angle");
        if (ssl_angle_obj) {
            float angle = ssl_angle_obj->valuedouble;
            if (angle >= 0) {
                MEDIA_LOGI("voice angle %.1f", angle);
                int iangle = (int) angle;
                if (iangle % 5 != 0) {
                    iangle = iangle / 5 * 5;
                }

                MusicPlayer_addSong(playlist_parser->tts_player, WAKE_UP_AUDIO_FILE);

                sprintf(tts_url, "vfs://tts/angle/%d.mp3", 1000 + iangle);
                MusicPlayer_addSong(playlist_parser->tts_player, tts_url);
            }
        }
        cJSON_Delete(root_obj);
    }
    osal_free(data);
}

void PlayListParser_asrMsg(PlayListParser *playlist_parser, void *data, int32_t length, asr_result cb)
{
    playlist_parser->cb = cb;

    AMessage *msg = AMessage_create(kWhatAsrMsg, &playlist_parser->handler);
    void *temp_data = (void *)osal_malloc(length);
    memcpy(temp_data, data, length);
    AMessage_setPointer(msg, "data", temp_data);
    AMessage_setInt32(msg, "length", length);
    AMessage_post(msg, 0);
    AMessage_put(msg);
}

static void PlayListParser_asrIntent(const char *json, int bytes, char **sub_tts_player_list, int *list_nums)
{
    (void)bytes;

    MEDIA_LOGV("==>rec: %.*s", bytes, json);
    char g_tts_content[MAX_TTS_CONTENT_LEN] = { 0 };

    cJSON *res_obj = cJSON_Parse((char *)json);
    if (res_obj != NULL) {
        cJSON *type_obj = cJSON_GetObjectItem(res_obj, "type");
        cJSON *cmds_obj = cJSON_GetObjectItem(res_obj, "commands");
        assert(type_obj);
        assert(cmds_obj);

        int cmd_nums = cJSON_GetArraySize(cmds_obj);
        const char *id_key_name = "id";
        if (type_obj && (type_obj->valueint == 1))  {
            id_key_name = "intent_id";
        }

        for (int i = 0; i < cmd_nums; i++) {
            cJSON *cmd_obj = cJSON_GetArrayItem(cmds_obj, i);
            cJSON *intent_id_obj = cJSON_GetObjectItem(cmd_obj, id_key_name);
            int intent_id = intent_id_obj->valueint;
            struct intent_tts_info *intent = get_intent_tts_info(intent_id);

            if (intent) {
                int tts_nums = intent->sub_tts_nums;
                *list_nums  = tts_nums;
                memset(sub_tts_player_list, 0, MAX_SUB_TTS * sizeof(char *));
                for (int k = 0; k < tts_nums; k++) {
                    int have_slot = intent->tts[k].have_slot;
                    if (!have_slot) {
                        sub_tts_player_list[k] = intent->tts[k].tts_name;
                    } else {
                        cJSON *slot_list_obj = cJSON_GetObjectItem(cmd_obj, "slots");
                        cJSON *slot_obj = cJSON_GetObjectItem(slot_list_obj, intent->tts[k].slot_name);
                        assert(slot_obj);
                        if (slot_obj) {
                            char *slot_value = slot_obj->valuestring;
                            memset(g_tts_content, 0, MAX_TTS_CONTENT_LEN);
                            if (intent->tts[k].suffix) {
                                sprintf(g_tts_content, "%s%s", slot_value, intent->tts[k].suffix);
                            } else {
                                sprintf(g_tts_content, "%s", slot_value);
                            }

                            char *tts_name = get_tts_name_by_content(g_tts_content);
                            if (tts_name) {
                                sub_tts_player_list[k] = tts_name;
                            }
                        }
                    }
                }
            }
        }

        cJSON_Delete(res_obj);
    }
}

void PlayListParser_onAsrMsg(PlayListParser *playlist_parser, AMessage *msg)
{
    MusicPlayer_pause(playlist_parser->music_player);
    playlist_parser->music_status = MUSIC_PAUSE;

    void *data;
    int32_t length;
    AMessage_findPointer(msg, "data", &data);
    AMessage_findInt32(msg, "length", &length);

    char *sub_tts_player_list[MAX_SUB_TTS];
    char tts_url[1024] = {0};
    int tts_nums;

    PlayListParser_asrIntent(data, length, sub_tts_player_list, &tts_nums);
    for (int i = 0; i < tts_nums; i++) {
        if (playlist_parser->cb) {
            playlist_parser->cb(atoi(sub_tts_player_list[i]));
        }
        sprintf(tts_url, "%s%s%s", "vfs://tts/", sub_tts_player_list[i], ".mp3");
        MusicPlayer_addSong(playlist_parser->tts_player, tts_url);
        if (strcmp(sub_tts_player_list[i], PLAY_MUSIC_INTENT_ID) == 0) {
            MusicPlayer_addSong(playlist_parser->music_player, "vfs://music.mp3");
        }
        if (strcmp(sub_tts_player_list[i], STOP_MUSIC_INTENT_ID) == 0) {
            MusicPlayer_stop(playlist_parser->music_player, true);
        }
    }
    osal_free(data);
}

void PlayListParser_vadTimeOut(PlayListParser *playlist_parser)
{
    AMessage *msg = AMessage_create(kWhatVadTimeOut, &playlist_parser->handler);
    AMessage_post(msg, 0);
    AMessage_put(msg);
}

void PlayListParser_onVadTimeOut(PlayListParser *playlist_parser)
{
    MusicPlayer_pause(playlist_parser->music_player);
    playlist_parser->music_status = MUSIC_PAUSE;
    MusicPlayer_addSong(playlist_parser->tts_player, VAD_TIMEOUT_AUDIO_FILE);
}

void PlayListParser_addSong(PlayListParser *playlist_parser, char *url)
{
    AMessage *msg = AMessage_create(kWhatAddSong, &playlist_parser->handler);
    char *temp_url = (char *) strdup(url);
    AMessage_setPointer(msg, "url", temp_url);
    AMessage_post(msg, 0);
    AMessage_put(msg);
}

void PlayListParser_onAddSong(PlayListParser *playlist_parser, AMessage *msg)
{
    void *url;
    AMessage_findPointer(msg, "url", &url);

    MusicPlayer_addSong(playlist_parser->music_player, url);

    free(url);
}


static void
PlayListParser_onMessage(AHandler *me, AMessage *msg)
{
    PlayListParser *playlist_parser = container_of(me, PlayListParser, handler);
    switch (AMessage_what(msg)) {
    case kWhatWakeUp: {
        MEDIA_LOGD("kWhatWakeUp");
        PlayListParser_onWakeUp(playlist_parser, msg);
        break;
    }
    case kWhatAsrMsg: {
        PlayListParser_onAsrMsg(playlist_parser, msg);
        break;
    }
    case kWhatSslMsg: {
        PlayListParser_onSslMsg(playlist_parser, msg);
        break;
    }
    case kWhatVadTimeOut: {
        PlayListParser_onVadTimeOut(playlist_parser);
        break;
    }
    case kWhatAddSong: {
        PlayListParser_onAddSong(playlist_parser, msg);
        break;
    }
    case kWhatTTSCallback: {
        MEDIA_LOGD("kWhatTTSCallback");
        int32_t what;
        void *url;
        AMessage_findInt32(msg, "what", &what);
        AMessage_findPointer(msg, "url", &url);

        switch (what) {
        case KWhatListEmpty: {
            if (url) {
                MEDIA_LOGD("url %s", url);
                if (playlist_parser->music_status == MUSIC_PAUSE) {
                    MEDIA_LOGD("music resume");
                    MusicPlayer_resume(playlist_parser->music_player);
                    playlist_parser->music_status = MUSIC_PLAY;
                } else {
                    MEDIA_LOGD("music is not pause");
                    playlist_parser->music_status = MUSIC_IDLE;
                }
            } else {
                MEDIA_LOGE("could not find url");
            }
            break;
        }

        default:
            break;
        }
    }
    }
}

PlayListParser *PlayListParser_create(void)
{
    PlayListParser *playlist_parser = (PlayListParser *)osal_malloc(sizeof(PlayListParser));
    if (!playlist_parser) {
        MEDIA_LOGE("malloc playlist parser fail");
        return NULL;
    }

    playlist_parser->music_status = 0;

    playlist_parser->looper = ALooper_create("plp");
    if (!playlist_parser->looper) {
        MEDIA_LOGE("creat alooper fail");
        osal_free(playlist_parser);
        return NULL;
    }

    AHandler_init(&playlist_parser->handler, PlayListParser_onMessage);
    playlist_parser->handler_id = ALooper_registerHandler(playlist_parser->looper, &playlist_parser->handler);
    ALooper_start(playlist_parser->looper, false, OSAL_THREAD_PRI_NORMAL);

    playlist_parser->music_player = MusicPlayer_create(NULL);

    playlist_parser->tts_notify = AMessage_create(kWhatTTSCallback, &playlist_parser->handler);
    playlist_parser->tts_player = MusicPlayer_create(playlist_parser->tts_notify);

    playlist_parser->cb = NULL;
    return playlist_parser;
}

void PlayListParser_destroy(PlayListParser *playlist_parser)
{
    if (!playlist_parser) {
        return;
    }

    ALooper_unregisterHandler(playlist_parser->handler_id);
    ALooper_destroy(playlist_parser->looper);

    osal_free(playlist_parser);
}



