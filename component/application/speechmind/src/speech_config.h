#ifndef _SPEECH_MIND_SPEECH_CONFIG_
#define _SPEECH_MIND_SPEECH_CONFIG_

#include <stdint.h>

typedef struct KWSConfig KWSConfig;
struct KWSConfig {
    int key_words_num;
    char *key_words_total;
    int *key_words_length;
    int thresholds_num;
    float *thresholds;
};

typedef struct DumpConfig DumpConfig;
struct DumpConfig {
    int sd_dump;
    int64_t index;
    int save_times;
};

typedef struct SpeechConfig SpeechConfig;
struct SpeechConfig {
    KWSConfig kws_config;
    DumpConfig dump_config;
    int afe;
    int rpc;
    float amplifier_volume;
};

void SpeechConfig_load(void);
SpeechConfig *SpeechConfig_getSpeechConfig(void);
float SpeechConfig_getAmplifierVolume(void);

void SpeechConfig_save(void);
#endif /* _SPEECH_MIND_SPEECH_CONFIG_ */
