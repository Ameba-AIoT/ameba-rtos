#ifndef _AIVOICE_ASR_CONFIG_H
#define _AIVOICE_ASR_CONFIG_H

// ASR Sensitivity
// The higher, easier to detect commands but also more false alarm
typedef enum {
	ASR_SENSITIVITY_LOW  = 0,
	ASR_SENSITIVITY_MID  = 1,
	ASR_SENSITIVITY_HIGH = 2,
} asr_sensitivity_e;


struct asr_config {
	asr_sensitivity_e sensitivity;
};

#define ASR_CONFIG_DEFAULT() {\
    .sensitivity = ASR_SENSITIVITY_MID,\
};

#endif
