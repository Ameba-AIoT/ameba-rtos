#ifndef SPEECH_MIND
#define SPEECH_MIND

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SpeechMindCallback SpeechMindCallback;

typedef struct VadInfo {
    int status;                     /*  0: vad is changed from speech to silence,
                                            indicating the end point of a speech segment
                                        1: vad is changed from silence to speech,
                                           indicating the start point of a speech segment */
    unsigned int offset_ms;         /* time offset relative to reset point. */
} VadInfo;

typedef struct WakeUpInfo {
    int len;
    const char *wakeup_words;
} WakeUpInfo;

typedef struct AfeInfo {
    int len;
    uint8_t *data;
} AfeInfo;

typedef struct AsrInfo {
    int id;
} AsrInfo;

struct SpeechMindCallback {
    void (*onVad)(SpeechMindCallback *callback, VadInfo *info);
    void (*onWakeUp)(SpeechMindCallback *callback, WakeUpInfo *info);
    void (*onAfe)(SpeechMindCallback *callback, AfeInfo *info);
    void (*onAsr)(SpeechMindCallback *callback, AsrInfo *info);
    void (*onAsrRecTimeout)(SpeechMindCallback *callback);
};

int32_t SpeechMind_init(void);
int32_t SpeechMind_start(void);
int32_t SpeechMind_stop(void);
int32_t SpeechMind_setCallback(SpeechMindCallback *callback);
int32_t SpeechMind_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* SPEECH_MIND */
