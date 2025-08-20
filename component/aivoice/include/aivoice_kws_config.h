#ifndef _AIVOICE_KWS_CONFIG_H
#define _AIVOICE_KWS_CONFIG_H

// KWS mode (single or multi)
typedef enum {
	KWS_SINGLE_MODE = 0,
	KWS_MULTI_MODE = 1,
} kws_mode_e;

// KWS Sensitivity
// The higher, easier to wake up but also more false alarm
typedef enum {
	KWS_SENSITIVITY_LOW  = 0,
	KWS_SENSITIVITY_MID  = 1,
	KWS_SENSITIVITY_HIGH = 2,
} kws_sensitivity_e;

#define MAX_KWS_KEYWORD_NUMS (5)
struct kws_config {
	const char *keywords[MAX_KWS_KEYWORD_NUMS]; /* available keyword depend on kws model */
	float thresholds[MAX_KWS_KEYWORD_NUMS];     /* thresholds for wake up, range: [0,1].
                                                   set to 0 to use sensitivity with predefined thresholds */
	kws_sensitivity_e sensitivity;
	kws_mode_e mode;

	bool enable_age_gender;
};

#define KWS_CONFIG_DEFAULT() {\
    .keywords = {"xiao-qiang-xiao-qiang", "ni-hao-xiao-qiang"},\
    .thresholds = {0.0, 0.0},\
    .sensitivity = KWS_SENSITIVITY_MID,\
    .mode = KWS_MULTI_MODE,\
    .enable_age_gender = false,\
};

#define KWS_CONFIG_EN_DEFAULT() {\
    .keywords = {"HH-EY1-R-IY1-L-T-EH2-K", "HH-AY1-R-IY1-L-T-EH2-K"},\
    .thresholds = {0.0, 0.0},\
    .sensitivity = KWS_SENSITIVITY_MID,\
    .mode = KWS_MULTI_MODE,\
    .enable_age_gender = false,\
};


#endif
