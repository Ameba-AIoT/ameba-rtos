#ifndef SPEECH_MIND_AUDIO_DUMP
#define SPEECH_MIND_AUDIO_DUMP

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum audio_dump_type {
    AUDIO_DUMP_PC_RECORD,
    AUDIO_DUMP_SD_CARD,
} audio_dump_type;

typedef struct AudioDump AudioDump;
AudioDump* AudioDump_create(enum audio_dump_type type);
int32_t AudioDump_start(AudioDump* dump);
int32_t AudioDump_dump(AudioDump* audio_dump, void* data, int32_t length);
int32_t AudioDump_stop(AudioDump* dump);
int32_t AudioDump_destroy(AudioDump* dump);

#ifdef __cplusplus
}
#endif

#endif /* SPEECH_MIND_AUDIO_DUMP */