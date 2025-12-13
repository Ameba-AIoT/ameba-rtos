#ifdef AEC_TEST
#define LOG_TAG "PlayerTest"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log/log.h"
#include "osal_c/osal_time.h"
#include "osal_c/osal_mem.h"
#include "osal_c/osal_thread.h"

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "audio/audio_service.h"
#include "media/rtplayer.h"

#include "test_cmd.h"

#define MAX_URL_SIZE 1024
static char g_url[MAX_URL_SIZE];

enum PlayingStatus {
    IDLE,
    PLAYING,
    PAUSED,
    PLAYING_COMPLETED,
    REWIND_COMPLETE,
    STOPPED,
    RESET,
};
static int g_playing_status = IDLE;

static struct RTPlayer *g_player;

static float g_left = 1.0;

static float g_right = 1.0;

static int g_loop = 0;

static osal_thread_t *g_player_thread;

void OnStateChanged(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int state)
{
    MEDIA_LOGD("OnStateChanged(%p %p), (%d)", listener, player, state);

    switch (state) {
    case RTPLAYER_PREPARED: { //entered for async prepare
        break;
    }

    case RTPLAYER_PLAYBACK_COMPLETE: { //eos received, then stop
        g_playing_status = PLAYING_COMPLETED;
        break;
    }

    case RTPLAYER_STOPPED: { //stop received, then reset
        MEDIA_LOGD("start reset");
        g_playing_status = STOPPED;
        break;
    }

    case RTPLAYER_PAUSED: { //pause received when do pause or start rewinding
        MEDIA_LOGD("paused");
        g_playing_status = PAUSED;
        break;
    }

    case RTPLAYER_REWIND_COMPLETE: { //rewind done received, then start
        MEDIA_LOGD("rewind complete");
        g_playing_status = REWIND_COMPLETE;
        break;
    }
    }
}

void OnInfo(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int info, int extra)
{
    MEDIA_LOGD("OnInfo (%p %p), (%d, %d)", listener, player, info, extra);

    switch (info) {
    case RTPLAYER_INFO_BUFFERING_START: {
        MEDIA_LOGD("RTPLAYER_INFO_BUFFERING_START");
        break;
    }

    case RTPLAYER_INFO_BUFFERING_END: {
        MEDIA_LOGD("RTPLAYER_INFO_BUFFERING_END");
        break;
    }

    case RTPLAYER_INFO_BUFFERING_INFO_UPDATE: {
        MEDIA_LOGD("RTPLAYER_INFO_BUFFERING_INFO_UPDATE %d", extra);
        break;
    }

    case RTPLAYER_INFO_NOT_REWINDABLE: {
        MEDIA_LOGD("RTPLAYER_INFO_NOT_REWINDABLE");
        break;
    }
    }
}

void OnError(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int error, int extra)
{
    MEDIA_LOGD("OnError (%p %p), (%d, %d)", player, listener, error, extra);
}

void StartPlay(struct RTPlayer *player, char *url)
{
    if (player == NULL) {
        MEDIA_LOGD("start play fail, player is NULL!");
        return;
    }

    MEDIA_LOGD("start to play: %s", url);
    int32_t ret = 0;

    g_playing_status = PLAYING;

    RTPlayer_SetVolume(player, g_left, g_right);

    MEDIA_LOGD("SetSource");
    ret = RTPlayer_SetSource(player, url);
    if (ret) {
        MEDIA_LOGD("SetDataSource fail:error=%d", (int)ret);
        return ;
    }

    MEDIA_LOGD("Prepare");
    ret = RTPlayer_Prepare(player);
    if (ret) {
        MEDIA_LOGD("prepare  fail:error=%d", (int)ret);
        return ;
    }

    if (g_loop) {
        ret = RTPlayer_SetLooping(player, g_loop);
    }

    MEDIA_LOGD("Start");
    ret = RTPlayer_Start(player);
    if (ret) {
        MEDIA_LOGD("start  fail:error=%d", (int)ret);
        return ;
    }

    int64_t duration = 0;
    RTPlayer_GetDuration(player, &duration);
    MEDIA_LOGD("duration is %lldms", duration);

    while ((g_playing_status == PLAYING) || g_loop) {
        osal_msleep(1000);
    }

    if (g_playing_status == PLAYING_COMPLETED) {
        MEDIA_LOGD("play complete, now stop.");
        RTPlayer_Stop(player);
    }

    while (g_playing_status == PLAYING_COMPLETED) {
        osal_msleep(1000);
    }

    if (g_playing_status == STOPPED) {
        MEDIA_LOGD("play stopped, now reset.");
        RTPlayer_Reset(player);
    }

    MEDIA_LOGD("play %s done!!!!", url);
}

u32 player_pause(u16 argc, u8 *argv[])
{
    (void) argc;
    (void) argv;

    if (g_player && g_playing_status == PLAYING) {
        RTPlayer_Pause(g_player);
    }

    return 0;
}

u32 player_stop(u16 argc, u8 *argv[])
{
    (void) argc;
    (void) argv;

    if (g_player && (g_playing_status == PLAYING || g_playing_status == PAUSED)) {
        RTPlayer_Stop(g_player);
    }

    return 0;
}

u32 player_resume(u16 argc, u8 *argv[])
{
    (void) argc;
    (void) argv;

    if (g_player && g_playing_status == PAUSED) {
        RTPlayer_Start(g_player);
        g_playing_status = PLAYING;
    }

    return 0;
}

u32 player_set_volume(u16 argc, u8 *argv[])
{
    (void) argc;

    float left = 1.0;
    float right = 1.0;
    if (argv[0]) {
        left = (float)atof((const char *)argv[0]);
    }
    if (argv[1]) {
        right = (float)atof((const char *)argv[1]);
    }

    if (g_player) {
        RTPlayer_SetVolume(g_player, left, right);
    }

    g_left = left;
    g_right = right;

    return 0;
}

u32 player_set_loop(u16 argc, u8 *argv[])
{
    (void) argc;

    int loop = (int)atoi((const char *)argv[0]);
    g_loop = loop;

    return 0;
}

int player_test(const char *url)
{
    RTAudioService_Init();
    MEDIA_LOGD("AudioService_Init done");

    struct RTPlayerCallback *callback = (struct RTPlayerCallback *)osal_malloc(sizeof(struct RTPlayerCallback));
    if (!callback) {
        MEDIA_LOGD("Calloc RTPlayerCallback fail.");
        return -1;
    }

    callback->OnRTPlayerStateChanged = OnStateChanged;
    callback->OnRTPlayerInfo = OnInfo;
    callback->OnRTPlayerError = OnError;

    g_player = RTPlayer_Create();

    RTPlayer_SetCallback(g_player, callback);

    StartPlay(g_player, (char *)url);

    osal_free(callback);
    RTPlayer_Destory(g_player);
    g_player = NULL;

    osal_msleep(1000);

    MEDIA_LOGD("exit");
    return 0;
}

static bool example_player_thread(void *param)
{
    (void) param;

    MEDIA_LOGD("player test start......");

    player_test(g_url);
    osal_msleep(1 * 1000);

    MEDIA_LOGD("player test done......");

    return false;
}

void example_player_test_args_handle(char  *argv[])
{
    /* parse command line arguments */
    while (*argv) {
        if (strcmp(*argv, "-F") == 0) {
            argv++;
            if (*argv) {
                memset(g_url, 0, MAX_URL_SIZE);
                if (!strncasecmp("http://", *argv, 7) || !strncasecmp("https://", *argv, 8)) {
                    snprintf(g_url, MAX_URL_SIZE, "%s", *argv);
                } else {
                    snprintf(g_url, MAX_URL_SIZE, "%s", *argv);
                }
            }
        }
        if (*argv) {
            argv++;
        }
    }
    MEDIA_LOGD("Usage: url is %s", g_url);

    MEDIA_LOGD("player test start......");

    osal_thread_param param;
    param.priority = OSAL_THREAD_PRI_NORMAL;
    param.stack_size = 1024 * 8;
    param.joinable = false;
    param.name = (char *)"example_player_thread";
    int32_t res = osal_thread_create(&g_player_thread, example_player_thread, NULL, &param);
    if (res) {
        MEDIA_LOGE("create example_player_thread fail");
        return;
    }

    MEDIA_LOGD("player test done......");

    return;
}

u32 example_player_test(u16 argc, u8 *argv[])
{
    (void) argc;
    example_player_test_args_handle((char **)argv);
    return TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE player_test_cmd_table[] = {
    {
        (const u8 *)"player",  1, example_player_test, (const u8 *)"\t player\n"
    },
    {
        (const u8 *)"player_pause",  1, player_pause, (const u8 *)"\t player_pause\n"
    },
    {
        (const u8 *)"player_stop",  1, player_stop, (const u8 *)"\t player_stop\n"
    },
    {
        (const u8 *)"player_resume",  1, player_resume, (const u8 *)"\t player_resume\n"
    },
    {
        (const u8 *)"player_set_volume",  1, player_set_volume, (const u8 *)"\t player_set_volume\n"
    },
    {
        (const u8 *)"player_set_loop",  1, player_set_loop, (const u8 *)"\t player_set_loop\n"
    },
};
#endif
