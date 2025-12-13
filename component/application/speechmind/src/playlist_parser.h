#ifndef _SPEECH_MIND_PLAYLIST_PARSER_
#define _SPEECH_MIND_PLAYLIST_PARSER_

#include "stdint.h"

typedef enum InterruptType {
    PLAYLIST_NONE,
    PLAYLIST_PAUSE,
    PLAYLIST_CLEAR,
} InterruptType;

typedef void (*asr_result)(int id);

typedef struct PlayListParser PlayListParser;
typedef struct AMessage AMessage;

PlayListParser* PlayListParser_create(void);
void PlayListParser_load(PlayListParser* playlist_parser);
void PlayListParser_wakeUp(PlayListParser* playlist_parser, InterruptType type);
void PlayListParser_asrMsg(PlayListParser* playlist_parser, void* data, int32_t length, asr_result cb);
void PlayListParser_vadTimeOut(PlayListParser* playlist_parser);
void PlayListParser_addSong(PlayListParser* playlist_parser, char* url);
void PlayListParser_destroy(PlayListParser* playlist_parser);

#endif /* _SPEECH_MIND_PLAYLIST_PARSER_ */
