#ifndef SPEECH_UTILS
#define SPEECH_UTILS

#define FRAME_MS 16
#define IN_CHANNEL 3
#define SAMPLE_RATE 16000
#define BITS 16
#define DUMP_FRAME_BYTES (4*FRAME_MS*SAMPLE_RATE*(BITS/8)/1000LL)

#define VAD_TIMEOUT_MS 10000

// #define SD_DUMP
// #define PC_RECORD

typedef enum PlayStatus {
    IDLE,
    PREPARE,
    PLAYING,
    PAUSE,
    STOP,
} PlayStatus;

#endif /* SPEECH_UTILS */
