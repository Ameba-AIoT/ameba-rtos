/************************************************************************************
File Name		:	aivoice_afe_config.h
Discription		:
Revision		:
Date			:
*************************************************************************************/
#include <stdbool.h>

#ifndef AIVOICE_AFE_CONFIG_H
#define AIVOICE_AFE_CONFIG_H


#ifdef __cplusplus
extern "C" {
#endif

/*-------------- Enumeration definition ----------------------*/
// Microphone array geometry
typedef enum {
	AFE_1MIC = 0,                 // single microphone
	AFE_LINEAR_2MIC_30MM = 1,     // 2 microphones, distance = 30mm
	AFE_LINEAR_2MIC_50MM = 2,     // 2 microphones, distance = 50mm
	AFE_LINEAR_2MIC_70MM = 3,     // 2 microphones, distance = 70mm
	AFE_CIRCLE_3MIC_50MM = 4,     // 3 microphones, Circle, diameter = 50mm, the angle corresponding to [mic1,mic2,mic3] = [0,120,240] (Cartesian coordinate)
} afe_mic_geometry_e;

// AFE mode, ASR and Communication
typedef enum {
	AFE_FOR_ASR = 0,            // AFE for ASR application
	AFE_FOR_COM = 1,            // AFE for voice communication application
} afe_mode_e;

// AEC
typedef enum {
	AFE_AEC_SIGNAL = 0,         // AEC: signal process method
	AFE_AEC_NN     = 1,         // AEC: NN method
} afe_aec_mode_e;

typedef enum {
	AFE_AEC_FILTER_LOW  = 0,    // AEC low cost, filter length: short
	AFE_AEC_FILTER_MID  = 1,    // AEC middle cost, filter length: medium
	AFE_AEC_FILTER_HIGH = 2,    // AEC high cost, filter length: long
} afe_aec_filter_tap_e;

typedef enum {
	AFE_AEC_RES_LOW  = 0,       // RES level: weak
	AFE_AEC_RES_MID  = 1,       // RES level: medium
	AFE_AEC_RES_HIGH = 2,       // RES level: strong
} afe_aec_res_aggressive_mode_e;

// NS
typedef enum {
	AFE_NS_SIGNAL = 0,          // NS: signal process method
	AFE_NS_NN = 1,              // NS: NN method
} afe_ns_mode_e;

afe_ns_mode_e AFE_NS_SIGNAL_SET(void);
afe_ns_mode_e AFE_NS_NN_SET(void);

typedef enum {
	AFE_NS_AGGR_LOW  = 0,       // NS level: weak
	AFE_NS_AGGR_MID  = 1,       // NS level: medium
	AFE_NS_AGGR_HIGH = 2,       // NS level: strong
} afe_ns_aggressive_mode_e;

typedef enum {
	AFE_NS_COST_LOW  = 0,       // NS low cost mode: 1 channel
	AFE_NS_COST_HIGH = 1,       // NS high cost mode: all channels
} afe_ns_cost_mode_e;

/*------------------- Structure Definition ------------------*/
typedef struct afe_config {

	// AFE common parameter
	afe_mic_geometry_e  mic_array;          // microphone array. Make sure to choose the matched resource library everytime changed.
	int ref_num;                            // reference channel number, must be 0 or 1. AEC will be disabled if ref_num=0.
	int sample_rate;                        // sampling rate(Hz), must be 16000
	int frame_size;                         // frame length(samples), must be 256

	afe_mode_e afe_mode;                    // AFE mode, for ASR or voice communication.
	bool enable_aec;                        // AEC(Acoustic Echo Cancellation) module switch
	bool enable_ns;                         // NS(Noise Suppression) module switch
	bool enable_agc;                        // AGC(Automation Gain Control) module switch
	bool enable_ssl;                        // SSL(Sound Source Localization) module switch.

	// AEC module parameter
	afe_aec_mode_e aec_mode;                // AEC mode, signal process or NN method. NN method is not supported in current version.
	int aec_enable_threshold;               // ref signal amplitude threshold for AEC, the value should be in [0, 100].
	// larger value means the minimum echo to be cancelled will be larger.
	bool enable_res;                        // AEC residual echo suppression module switch
	afe_aec_filter_tap_e aec_cost;          // higher cost means longer filter length and more echo reduction
	afe_aec_res_aggressive_mode_e res_aggressive_mode;  // higher mode means more residual echo suppression but more distortion

	// NS module parameter
	afe_ns_mode_e ns_mode;                  // NS mode, signal process or NN method. NN method is only supports voice communication mode.
	afe_ns_cost_mode_e ns_cost_mode;        // low cost mode means 1channel NR and poorer noise reduction effect
	afe_ns_aggressive_mode_e ns_aggressive_mode;        // higher mode means more stationary noise suppression but more distortion

	// AGC module parameter
	int agc_fixed_gain;                     // AGC fixed gain(dB) applied on AFE output, the value should be in [0, 18].
	bool enable_adaptive_agc;               // adaptive AGC switch. Only supports voice communication mode.

	// SSL module parameter
	float ssl_resolution;                   // SSL solution(degree)
	int ssl_min_hz;                         // minimum frequency(Hz) of SSL module.
	int ssl_max_hz;                         // maximum frequency(Hz) of SSL module.
} afe_config_t;

/*-------------------- Default config for ASR --------------------*/
#define AFE_CONFIG_ASR_DEFAULT_1MIC() { \
    .mic_array = AFE_1MIC, \
    .ref_num = 1, \
    .sample_rate = 16000, \
    .frame_size = 256,\
    .afe_mode   = AFE_FOR_ASR, \
    .enable_aec = true, \
    .enable_ns  = false, \
    .enable_agc = true, \
    .enable_ssl = false, \
    .aec_mode = AFE_AEC_SIGNAL, \
    .aec_enable_threshold = 5, \
    .enable_res = false, \
    .aec_cost = AFE_AEC_FILTER_MID, \
    .res_aggressive_mode = AFE_AEC_RES_MID, \
    .ns_mode = AFE_NS_SIGNAL_SET(), \
    .ns_cost_mode = AFE_NS_COST_HIGH, \
    .ns_aggressive_mode = AFE_NS_AGGR_LOW, \
    .agc_fixed_gain = 15, \
    .enable_adaptive_agc = false, \
}

#define AFE_CONFIG_ASR_DEFAULT_2MIC30MM() { \
    .mic_array = AFE_LINEAR_2MIC_30MM, \
    .ref_num = 1, \
    .sample_rate = 16000, \
    .frame_size = 256,\
    .afe_mode   = AFE_FOR_ASR, \
    .enable_aec = true, \
    .enable_ns  = false, \
    .enable_agc = true, \
    .enable_ssl = true, \
    .aec_mode = AFE_AEC_SIGNAL, \
    .aec_enable_threshold = 5, \
    .enable_res = false, \
    .aec_cost = AFE_AEC_FILTER_MID, \
    .res_aggressive_mode = AFE_AEC_RES_LOW, \
    .ns_mode = AFE_NS_SIGNAL_SET(), \
    .ns_cost_mode = AFE_NS_COST_HIGH, \
    .ns_aggressive_mode = AFE_NS_AGGR_LOW, \
    .agc_fixed_gain = 15, \
    .enable_adaptive_agc = false, \
    .ssl_resolution = 10, \
    .ssl_min_hz = 500, \
    .ssl_max_hz = 3500, \
}

#define AFE_CONFIG_ASR_DEFAULT_2MIC50MM() { \
    .mic_array = AFE_LINEAR_2MIC_50MM, \
    .ref_num = 1, \
    .sample_rate = 16000, \
    .frame_size = 256,\
    .afe_mode   = AFE_FOR_ASR, \
    .enable_aec = true, \
    .enable_ns  = false, \
    .enable_agc = true, \
    .enable_ssl = true, \
    .aec_mode = AFE_AEC_SIGNAL, \
    .aec_enable_threshold = 5, \
    .enable_res = false, \
    .aec_cost = AFE_AEC_FILTER_MID, \
    .res_aggressive_mode = AFE_AEC_RES_LOW, \
    .ns_mode = AFE_NS_SIGNAL_SET(), \
    .ns_cost_mode = AFE_NS_COST_HIGH, \
    .ns_aggressive_mode = AFE_NS_AGGR_LOW, \
    .agc_fixed_gain = 15, \
    .enable_adaptive_agc = false, \
    .ssl_resolution = 10, \
    .ssl_min_hz = 300, \
    .ssl_max_hz = 3500, \
}

#define AFE_CONFIG_ASR_DEFAULT_2MIC70MM() { \
    .mic_array = AFE_LINEAR_2MIC_70MM, \
    .ref_num = 1, \
    .sample_rate = 16000, \
    .frame_size = 256,\
    .afe_mode   = AFE_FOR_ASR, \
    .enable_aec = true, \
    .enable_ns  = false, \
    .enable_agc = true, \
    .enable_ssl = true, \
    .aec_mode = AFE_AEC_SIGNAL, \
    .aec_enable_threshold = 5, \
    .enable_res = false, \
    .aec_cost = AFE_AEC_FILTER_MID, \
    .res_aggressive_mode = AFE_AEC_RES_LOW, \
    .ns_mode = AFE_NS_SIGNAL_SET(), \
    .ns_cost_mode = AFE_NS_COST_HIGH, \
    .ns_aggressive_mode = AFE_NS_AGGR_LOW, \
    .agc_fixed_gain = 15, \
    .enable_adaptive_agc = false, \
    .ssl_resolution = 10, \
    .ssl_min_hz = 300, \
    .ssl_max_hz = 2200, \
}

#define AFE_CONFIG_ASR_DEFAULT_3MIC() { \
    .mic_array = AFE_CIRCLE_3MIC_50MM, \
    .ref_num = 1, \
    .sample_rate = 16000, \
    .frame_size = 256,\
    .afe_mode   = AFE_FOR_ASR, \
    .enable_aec = true, \
    .enable_ns  = false, \
    .enable_agc = true, \
    .enable_ssl = true, \
    .aec_mode = AFE_AEC_SIGNAL, \
    .aec_enable_threshold = 5, \
    .enable_res = false, \
    .aec_cost = AFE_AEC_FILTER_MID, \
    .res_aggressive_mode = AFE_AEC_RES_LOW, \
    .ns_mode = AFE_NS_SIGNAL_SET(), \
    .ns_cost_mode = AFE_NS_COST_HIGH, \
    .ns_aggressive_mode = AFE_NS_AGGR_LOW, \
    .agc_fixed_gain = 15, \
    .enable_adaptive_agc = false, \
    .ssl_resolution = 10, \
    .ssl_min_hz = 300, \
    .ssl_max_hz = 3500, \
}

/*-------------------- Default config for COM --------------------*/
#define AFE_CONFIG_COM_DEFAULT_1MIC() { \
    .mic_array = AFE_1MIC, \
    .ref_num = 1, \
    .sample_rate = 16000, \
    .frame_size = 256,\
    .afe_mode   = AFE_FOR_COM, \
    .enable_aec = true, \
    .enable_ns  = true, \
    .enable_agc = true, \
    .enable_ssl = false, \
    .aec_mode = AFE_AEC_SIGNAL, \
    .aec_enable_threshold = 5, \
    .enable_res = true, \
    .aec_cost = AFE_AEC_FILTER_MID, \
    .res_aggressive_mode = AFE_AEC_RES_MID, \
    .ns_mode = AFE_NS_SIGNAL_SET(), \
    .ns_cost_mode = AFE_NS_COST_HIGH, \
    .ns_aggressive_mode = AFE_NS_AGGR_MID, \
    .agc_fixed_gain = 5, \
    .enable_adaptive_agc = true, \
}

#define AFE_CONFIG_COM_DEFAULT_2MIC30MM() { \
    .mic_array = AFE_LINEAR_2MIC_30MM, \
    .ref_num = 1, \
    .sample_rate = 16000, \
    .frame_size = 256,\
    .afe_mode   = AFE_FOR_COM, \
    .enable_aec = true, \
    .enable_ns  = true, \
    .enable_agc = true, \
    .enable_ssl = false, \
    .aec_mode = AFE_AEC_SIGNAL, \
    .aec_enable_threshold = 5, \
    .enable_res = true, \
    .aec_cost = AFE_AEC_FILTER_MID, \
    .res_aggressive_mode = AFE_AEC_RES_MID, \
    .ns_mode = AFE_NS_SIGNAL_SET(), \
    .ns_cost_mode = AFE_NS_COST_HIGH, \
    .ns_aggressive_mode = AFE_NS_AGGR_MID, \
    .agc_fixed_gain = 5, \
    .enable_adaptive_agc = true, \
}

#define AFE_CONFIG_COM_DEFAULT_2MIC50MM() { \
    .mic_array = AFE_LINEAR_2MIC_50MM, \
    .ref_num = 1, \
    .sample_rate = 16000, \
    .frame_size = 256,\
    .afe_mode   = AFE_FOR_COM, \
    .enable_aec = true, \
    .enable_ns  = true, \
    .enable_agc = true, \
    .enable_ssl = false, \
    .aec_mode = AFE_AEC_SIGNAL, \
    .aec_enable_threshold = 5, \
    .enable_res = true, \
    .aec_cost = AFE_AEC_FILTER_MID, \
    .res_aggressive_mode = AFE_AEC_RES_MID, \
    .ns_mode = AFE_NS_SIGNAL_SET(), \
    .ns_cost_mode = AFE_NS_COST_HIGH, \
    .ns_aggressive_mode = AFE_NS_AGGR_MID, \
    .agc_fixed_gain = 5, \
    .enable_adaptive_agc = true, \
}

#define AFE_CONFIG_COM_DEFAULT_2MIC70MM() { \
    .mic_array = AFE_LINEAR_2MIC_70MM, \
    .ref_num = 1, \
    .sample_rate = 16000, \
    .frame_size = 256,\
    .afe_mode   = AFE_FOR_COM, \
    .enable_aec = true, \
    .enable_ns  = true, \
    .enable_agc = true, \
    .enable_ssl = false, \
    .aec_mode = AFE_AEC_SIGNAL, \
    .aec_enable_threshold = 5, \
    .enable_res = true, \
    .aec_cost = AFE_AEC_FILTER_MID, \
    .res_aggressive_mode = AFE_AEC_RES_MID, \
    .ns_mode = AFE_NS_SIGNAL_SET(), \
    .ns_cost_mode = AFE_NS_COST_HIGH, \
    .ns_aggressive_mode = AFE_NS_AGGR_MID, \
    .agc_fixed_gain = 5, \
    .enable_adaptive_agc = true, \
}


#ifdef __cplusplus
}
#endif

#endif