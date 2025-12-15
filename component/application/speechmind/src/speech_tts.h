#ifndef _SPEECH_MIND_SPEECH_TTS_
#define _SPEECH_MIND_SPEECH_TTS_

#include <string.h>
#include <stdio.h>

struct tts_info {
    int have_slot;
    char *tts_name;
    char *slot_name;
    char *suffix;
};

struct intent_tts_info {
    int intent_id;
    int sub_tts_nums;
    struct tts_info tts[3];
};

struct tts_content_name {
    char *content;
    char *name;
};


char *get_tts_name_by_content(char *content);
struct intent_tts_info *get_intent_tts_info(int id);

#endif

