#ifndef _AIVOICE_VAD_CONFIG_H
#define _AIVOICE_VAD_CONFIG_H

// VAD Sensitivity
// The higher, easier to detect speech but also more false alarm
typedef enum {
	VAD_SENSITIVITY_LOW  = 0,
	VAD_SENSITIVITY_MID  = 1,
	VAD_SENSITIVITY_HIGH = 2,
} vad_sensitivity_e;

struct vad_config {
	vad_sensitivity_e sensitivity;
	unsigned int left_margin;   /* Unit:ms. only affects offset_ms,
                                   it won't affect the event time of status 1.
                                   If you need get the audio during left_margin,
                                   please implement a buffer to keep audio. */
	unsigned int right_margin;  /* Unit:ms. affects both offset_ms and event time of status 0. */
};

#define VAD_CONFIG_DEFAULT() {\
    .sensitivity = VAD_SENSITIVITY_MID,\
    .left_margin = 300,\
    .right_margin = 160,\
};

#endif
