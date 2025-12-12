#ifndef _SPEECH_MIND_MUSIC_PLAYER_
#define _SPEECH_MIND_MUSIC_PLAYER_

#include <stdint.h>
#include <stdbool.h>
#include "playlist.h"

typedef struct MusicPlayer MusicPlayer;
typedef struct AMessage AMessage;

MusicPlayer* MusicPlayer_create(AMessage *notify);
int32_t MusicPlayer_addSong(MusicPlayer* player, const char* url);
int32_t MusicPlayer_stop(MusicPlayer* player, bool clear);
int32_t MusicPlayer_pause(MusicPlayer* player);
int32_t MusicPlayer_resume(MusicPlayer* player);
int32_t MusicPlayer_destroy(MusicPlayer* player);


#endif //_SPEECH_MIND_MUSIC_PLAYER_