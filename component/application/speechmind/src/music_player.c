#define LOG_TAG "SpeechP"

#include <stdlib.h>
#include <string.h>

#include "log/log.h"
#include "osal_c/osal_errnos.h"
#include "osal_c/osal_mem.h"
#include "osal_c/osal_time.h"
#include "osal_c/osal_mutex.h"
#include "osal_c/osal_thread.h"
#include "osal_c/osal_macros.h"

#include "cutils/container_of.h"

#include "foundation/amessage.h"
#include "foundation/alooper.h"
#include "foundation/ahandler.h"

#include "media/rtplayer.h"
#include "media/rtplayer_common.h"

#include "playlist.h"
#include "music_player.h"

enum {
    kWhatStart          = 'star',
    kWhatStop           = 'stop',
    kWhatPause          = 'paus',
    kWhatResume         = 'resu',
    kWhatError          = 'erro',
    kWhatStateChange    = 'stau',
    KWhatListEmpty      = 'liem',
};

struct MusicPlayer {
    RTPlayer* player;
    struct RTPlayerCallback callback;
    osal_mutex_t status_mutex;
    int32_t status;
    AHandler handler;
    int32_t handler_id;
    ALooper *looper;
    AMessage *notify;
    Playlist* playlist;
    char* url;
};

int32_t MusicPlayer_start(MusicPlayer* player, const char* url);

static void MusicPlayer_setStatus(MusicPlayer* player, int32_t status) {
    osal_mutex_lock(&player->status_mutex);
    MEDIA_LOGD("old status %d, new status %d", player->status, status);
    if ((player->status == RTPLAYER_STARTED) && (status == RTPLAYER_STOPPED || status == RTPLAYER_PLAYBACK_COMPLETE)) {
        if (status == RTPLAYER_PLAYBACK_COMPLETE) {
            Playlist_removeFirstSong(player->playlist);
            RTPlayer_Stop(player->player);
        }
        if (status == RTPLAYER_STOPPED) {
            RTPlayer_Reset(player->player);
            player->status = RTPLAYER_IDLE;
            char* url = Playlist_getFirstSongTitle(player->playlist);
            if(url) {
                player->status = status;
                MusicPlayer_start(player, url);
            } else {
                MEDIA_LOGD("player is idle");
                if (player->notify && player->url) {
                    MEDIA_LOGD("notify");
                    AMessage *msg = AMessage_duplicate(player->notify);
                    AMessage_setInt32(msg, "what", KWhatListEmpty);
                    AMessage_setPointer(msg, "url", player->url);
                    AMessage_post(msg, 0);
                    AMessage_put(msg);
                }
            }
        }
    } else {
        player->status = status;
    }
    osal_mutex_unlock(&player->status_mutex);
}

static int MusicPlayer_getStatus(MusicPlayer* player) {
    int32_t status = 0;
    osal_mutex_lock(&player->status_mutex);
    status = player->status;
    osal_mutex_unlock(&player->status_mutex);
    return status;
}

int32_t MusicPlayer_start(MusicPlayer* player, const char* url) {
    AMessage *msg = AMessage_create(kWhatStart, &player->handler);
    char* temp_url = (char*) strdup(url);
    AMessage_setPointer(msg, "url", temp_url);
    AMessage_post(msg, 0);
    AMessage_put(msg);
    return OSAL_OK;
}

void MusicPlayer_onStart(MusicPlayer* music_player, AMessage* msg) {
    void* url;
    AMessage_findPointer(msg, "url", &url);

    if (music_player->url) {
        osal_free(music_player->url);
    }

    music_player->url = strdup(url);

    int32_t ret = RTPlayer_SetSource(music_player->player, url);
    if (ret != 0) {
        MEDIA_LOGE("RTPlayer_SetSource FAILED ret=%d", (int)ret);
    } else {
        ret = RTPlayer_Prepare(music_player->player);
        if (ret) {
            MEDIA_LOGE("RTPlayer_Prepare FAILED ret=%d", (int)ret);
            ret = RTPlayer_Reset(music_player->player);
        } else {
            ret = RTPlayer_Start(music_player->player);
            MEDIA_LOGD("RTPlayer_Start ret=%d", (int)ret);
            if (ret) {
                MEDIA_LOGE("RTPlayer_Start FAILED ret=%d", (int)ret);
                ret = RTPlayer_Reset(music_player->player);
            }
        }
    }
    free(url);
}

int32_t MusicPlayer_stop(MusicPlayer* player, bool clear) {
    if (clear) {
        Playlist_clear(player->playlist);
    }
    AMessage *msg = AMessage_create(kWhatStop, &player->handler);
    AMessage_post(msg, 0);
    AMessage_put(msg);
    return OSAL_OK;
}

void MusicPlayer_onStop(MusicPlayer* music_player)
{
    Playlist_removeFirstSong(music_player->playlist);
    RTPlayer_Stop(music_player->player);
    RTPlayer_Reset(music_player->player);
}

int32_t MusicPlayer_resume(MusicPlayer* music_player) {
    MEDIA_LOGE("MusicPlayer_resume");
    AMessage *msg = AMessage_create(kWhatResume, &music_player->handler);
    AMessage_post(msg, 0);
    AMessage_put(msg);
    return OSAL_OK;
}

void MusicPlayer_onResume(MusicPlayer* music_player) {
    if (RTPlayer_Start(music_player->player)) {
        MEDIA_LOGE("MusicPlayer not start, restart");
        char* url = Playlist_getFirstSongTitle(music_player->playlist);
        if(url) {
            MusicPlayer_start(music_player, url);
        }
    }
}

int32_t MusicPlayer_pause(MusicPlayer* music_player) {
    AMessage *msg = AMessage_create(kWhatPause, &music_player->handler);
    AMessage_post(msg, 0);
    AMessage_put(msg);
    return OSAL_OK;
}

void MusicPlayer_onPause(MusicPlayer* music_player)
{
    int res = RTPlayer_Pause(music_player->player);
    if (res != OSAL_OK) {
        MusicPlayer_setStatus(music_player, RTPLAYER_PAUSED);
    }
}

static void
MusicPlayer_onMessage(AHandler *me, AMessage *msg) {
    MusicPlayer *music_player = container_of(me, MusicPlayer, handler);
    switch (AMessage_what(msg)) {
    case kWhatStart:
    {
        MusicPlayer_onStart(music_player, msg);
        break;
    }
    case kWhatStop:
    {
        MusicPlayer_onStop(music_player);
        break;
    }
    case kWhatPause:
    {
        MusicPlayer_onPause(music_player);
        break;
    }
    case kWhatResume:
    {
        MusicPlayer_onResume(music_player);
        break;
    }
    case kWhatStateChange:
    {
        int32_t status = 0;
        AMessage_findInt32(msg, "status", &status);
        MusicPlayer_setStatus(music_player, status);
    }
    }
}

static inline MusicPlayer* GetMusicPlayer(const struct RTPlayerCallback *me) {
    return container_of(me, MusicPlayer, callback);
}

static void OnMusicPlayStateChanged(const struct RTPlayerCallback *listener,
                                    const struct RTPlayer *player,
                                    int state)
{
    MEDIA_LOGD("OnMusicPlayStateChanged(%p %p), (%d)", listener, player, state);
    (void) player;
    MusicPlayer *music_player = GetMusicPlayer(listener);
    AMessage *msg = AMessage_create(kWhatStateChange, &music_player->handler);
    AMessage_setInt32(msg, "status", state);
    AMessage_post(msg, 0);
    AMessage_put(msg);
}

static void OnMusicPlayerInfo(const RTPlayerCallback *listener, const RTPlayer *player, int info, int extra)
{
    (void) listener;
    (void) player;
    (void) info;
    (void) extra;
}

static void OnMusicError(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int error, int extra)
{
    (void) listener;
    (void) player;
    (void) error;
    (void) extra;
    MusicPlayer *music_player = GetMusicPlayer(listener);
    AMessage *msg = AMessage_create(kWhatError, &music_player->handler);
    AMessage_setInt32(msg, "status", RTPLAYER_ERROR);
    AMessage_post(msg, 0);
    AMessage_put(msg);
}

MusicPlayer* MusicPlayer_create(AMessage *notify) {
    (void) notify;
    MusicPlayer* music_player = (MusicPlayer*)osal_malloc(sizeof(MusicPlayer));
    if (!music_player) {
        MEDIA_LOGE("malloc speech player fail");
        return NULL;
    }

    music_player->status = 0;

    music_player->url = NULL;

    osal_mutex_init(&music_player->status_mutex);

    music_player->playlist = Playlist_create();
    if (!music_player->playlist) {
        MEDIA_LOGE("creat playlist fail");
        osal_free(music_player);
        return NULL;
    }

    music_player->player = RTPlayer_Create();
    if (!music_player->player) {
        MEDIA_LOGE("malloc music_player->player fail");
        Playlist_destroy(music_player->playlist);
        osal_free(music_player);
        return NULL;
    }

    music_player->callback.OnRTPlayerStateChanged = OnMusicPlayStateChanged;
    music_player->callback.OnRTPlayerInfo = OnMusicPlayerInfo;
    music_player->callback.OnRTPlayerError = OnMusicError;

    RTPlayer_SetCallback(music_player->player, &music_player->callback);

    music_player->looper = ALooper_create("plp");
    if (!music_player->looper) {
        MEDIA_LOGE("creat alooper fail");
        Playlist_destroy(music_player->playlist);
        osal_free(music_player->player);
        osal_free(music_player);
        return NULL;
    }

    if (notify) {
        music_player->notify = AMessage_get(notify);
    } else {
        music_player->notify = NULL;
    }

    AHandler_init(&music_player->handler, MusicPlayer_onMessage);
    music_player->handler_id = ALooper_registerHandler(music_player->looper, &music_player->handler);
    ALooper_start(music_player->looper, false, OSAL_THREAD_PRI_NORMAL);
    return music_player;
}

int32_t MusicPlayer_addSong(MusicPlayer* player, const char* url) {
    int list_size = Playlist_size(player->playlist);

    Playlist_addSong(player->playlist, url);
    int status = MusicPlayer_getStatus(player);
    if (list_size == 0 && status != RTPLAYER_PAUSED) {
        MEDIA_LOGD("start %s list size %d", url, list_size);
        MusicPlayer_start(player, url);
    } else {
        MEDIA_LOGD("add song start faild list size %d", list_size);
    }
    return OSAL_OK;
}

int32_t MusicPlayer_destroy(MusicPlayer* music_player)
{
    MEDIA_LOGD("%s Enter", __FUNCTION__);
    if (!music_player) {
        return OSAL_ERR_INVALID_PARAM;
    }

    if (music_player->playlist) {
        Playlist_destroy(music_player->playlist);
    }

    ALooper_unregisterHandler(music_player->handler_id);
    ALooper_destroy(music_player->looper);

    if (music_player->notify) {
        AMessage_put(music_player->notify);
    }

    osal_free(music_player);
    return OSAL_OK;
}
