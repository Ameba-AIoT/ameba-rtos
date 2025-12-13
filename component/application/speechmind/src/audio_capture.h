#ifndef SPEECH_MIND_AUDIO_CAPTURE
#define SPEECH_MIND_AUDIO_CAPTURE

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*AudioCaptureCallback)(const void *data, int len, void* usr_data);

typedef struct AudioCapture AudioCapture;
AudioCapture* AudioCapture_create(uint32_t sample_rate, uint32_t channels);
int32_t AudioCapture_setCallback(AudioCapture* capture, AudioCaptureCallback callback, void* usr_data);
int32_t AudioCapture_start(AudioCapture* capture);
int32_t AudioCapture_stop(AudioCapture* capture);
int32_t AudioCapture_destroy(AudioCapture* capture);

#ifdef __cplusplus
}
#endif

#endif /* SPEECH_MIND_AUDIO_CAPTURE */