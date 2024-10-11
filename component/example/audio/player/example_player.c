#define LOG_NDEBUG 0
#define LOG_TAG "PlayerTest"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "audio/audio_service.h"
#include "media/rtplayer.h"

#include "example_player.h"

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
int g_playing_status = IDLE;

struct RTPlayer *g_player;

void OnStateChanged(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int state)
{
	printf("OnStateChanged(%p %p), (%d)\n", listener, player, state);

	switch (state) {
	case RTPLAYER_PREPARED: { //entered for async prepare
		break;
	}

	case RTPLAYER_PLAYBACK_COMPLETE: { //eos received, then stop
		g_playing_status = PLAYING_COMPLETED;
		break;
	}

	case RTPLAYER_STOPPED: { //stop received, then reset
		printf("start reset\n");
		g_playing_status = STOPPED;
		break;
	}

	case RTPLAYER_PAUSED: { //pause received when do pause or start rewinding
		printf("paused\n");
		g_playing_status = PAUSED;
		break;
	}

	case RTPLAYER_REWIND_COMPLETE: { //rewind done received, then start
		printf("rewind complete\n");
		g_playing_status = REWIND_COMPLETE;
		break;
	}
	}
}

void OnInfo(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int info, int extra)
{
	printf("OnInfo (%p %p), (%d, %d)\n", listener, player, info, extra);

	switch (info) {
	case RTPLAYER_INFO_BUFFERING_START: {
		printf("RTPLAYER_INFO_BUFFERING_START\n");
		break;
	}

	case RTPLAYER_INFO_BUFFERING_END: {
		printf("RTPLAYER_INFO_BUFFERING_END\n");
		break;
	}

	case RTPLAYER_INFO_BUFFERING_INFO_UPDATE: {
		printf("RTPLAYER_INFO_BUFFERING_INFO_UPDATE %d\n", extra);
		break;
	}

	case RTPLAYER_INFO_NOT_REWINDABLE: {
		printf("RTPLAYER_INFO_NOT_REWINDABLE\n");
		break;
	}
	}
}

void OnError(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int error, int extra)
{
	printf("OnError (%p %p), (%d, %d)\n", player, listener, error, extra);
}

void StartPlay(struct RTPlayer *player, const char *url)
{
	if (player == NULL) {
		printf("start play fail, player is NULL!\n");
		return;
	}

	printf("start to play: %s\n", url);
	int32_t ret = 0;

	g_playing_status = PLAYING;

	printf("SetSource\n");
	ret = RTPlayer_SetSource(player, url);
	if (ret) {
		printf("SetDataSource fail:error=%d\n", (int)ret);
		return ;
	}

	printf("Prepare\n");
	ret = RTPlayer_Prepare(player);
	if (ret) {
		printf("prepare  fail:error=%d\n", (int)ret);
		return ;
	}

	printf("Start\n");
	ret = RTPlayer_Start(player);
	if (ret) {
		printf("start  fail:error=%d\n", (int)ret);
		return ;
	}

	int64_t duration = 0;
	RTPlayer_GetDuration(player, &duration);
	printf("duration is %lldms\n", duration);

	while (g_playing_status == PLAYING) {
		rtos_time_delay_ms(1000);
	}

	if (g_playing_status == PLAYING_COMPLETED) {
		printf("play complete, now stop.\n");
		RTPlayer_Stop(player);
	}

	while (g_playing_status == PLAYING_COMPLETED) {
		rtos_time_delay_ms(1000);
	}

	if (g_playing_status == STOPPED) {
		printf("play stopped, now reset.\n");
		RTPlayer_Reset(player);
	}

	printf("play %s done!!!!\n", url);
}

int player_test(const char *url)
{
	RTAudioService_Init();
	printf("AudioService_Init done\n");

	struct RTPlayerCallback *callback = (struct RTPlayerCallback *)malloc(sizeof(struct RTPlayerCallback));
	if (!callback) {
		printf("Calloc RTPlayerCallback fail.\n");
		return -1;
	}

	callback->OnRTPlayerStateChanged = OnStateChanged;
	callback->OnRTPlayerInfo = OnInfo;
	callback->OnRTPlayerError = OnError;

	g_player = RTPlayer_Create();

	RTPlayer_SetCallback(g_player, callback);

	StartPlay(g_player, url);

	free(callback);
	RTPlayer_Destory(g_player);
	g_player = NULL;

	rtos_time_delay_ms(1000);

	printf("exit\n");
	return 0;
}

#ifdef CMD_TEST
void example_player_thread(void *param)
{
	(void) param;

	printf("player test start......\n");

	player_test(g_url);
	rtos_time_delay_ms(1 * 1000);

	printf("player test done......\n");
	printf("\n\n");

	rtos_task_delete(NULL);
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
	printf("Usage: url is %s", g_url);

	printf("player test start......\n");

	if (rtos_task_create(NULL, ((const char *)"example_player_thread"), example_player_thread, NULL, 8 * 1024, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(example_player_thread) failed", __FUNCTION__);
	}

	printf("player test done......\n");
	printf("\n\n");

	return;
}

u32 example_player_test(u16 argc, u8 *argv[])
{
	(void) argc;
	example_player_test_args_handle((char**)argv);
	return _TRUE;
}
#else

void example_player_thread(void *param)
{
	(void) param;

	printf("player test start......\n");

	player_test("http://192.168.31.226/1.mp3");
	rtos_time_delay_ms(1 * 1000);

	printf("player test done......\n");
	printf("\n\n");

	rtos_task_delete(NULL);
}

void example_player(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_player_thread"), example_player_thread, NULL, 8 * 1024, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(example_player_thread) failed", __FUNCTION__);
	}
}
#endif
