/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __BT_AUDIO_RESAMPLE_H__
#define __BT_AUDIO_RESAMPLE_H__

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define RTK_BT_AUDIO_RESAMPLE_DEFAULT_FRACTION      (1LL << 32)
#define RTK_BT_AUDIO_RESAMPLE_TPDF_DITHER           0
#define RTK_BT_AUDIO_RESAMPLE_F32                   0
#if defined(RTK_BT_AUDIO_RESAMPLE_F32) && RTK_BT_AUDIO_RESAMPLE_F32
/* pre filter and post filter should be enabled both */
#define RTK_BT_AUDIO_RESAMPLE_PRE_FILTER            0
#define RTK_BT_AUDIO_RESAMPLE_POST_FILTER           0
#else
#define RTK_BT_AUDIO_RESAMPLE_PRE_FILTER            0
#define RTK_BT_AUDIO_RESAMPLE_POST_FILTER           0
#endif
/* filter related */
#define RTK_BT_AUDIO_FILTER_DEFAULT_GAIN            1.0
#define RTK_BT_AUDIO_FILTER_DEFAULT_BANDWIDTH       0.2
#ifndef M_PI
#define M_PI                                        3.14159265358979324
#endif
#ifndef M_LN2
#define M_LN2                                       0.69314718055994530942
#endif

/**
 * @typedef   rtk_bt_audio_filter_type_t
 * @brief     filter type
 */
typedef enum {
	RTK_BT_AUDIO_LPF,                           /*!< low pass filter */
	RTK_BT_AUDIO_HPF,                           /*!< High pass filter */
	RTK_BT_AUDIO_BPF,                           /*!< band pass filter */
	RTK_BT_AUDIO_NOTCH,                         /*!< Notch Filter */
	RTK_BT_AUDIO_PEQ,                           /*!< Peaking band EQ filter */
	RTK_BT_AUDIO_LSH,                           /*!< Low shelf filter */
	RTK_BT_AUDIO_HSH                            /*!< High shelf filter */
} rtk_bt_audio_filter_type_t;

/**
 * @typedef rtk_bt_audio_biquad_t
 * @brief   structure of bq config
 */
typedef struct {
	float   a0;         /*!< parame a0 -> b0 /a0 */
	float   a1;         /*!< parame a1 -> b1 /a0 */
	float   a2;         /*!< parame a2 -> b2 /a0 */
	float   a3;         /*!< parame a3 -> a1 /a0 */
	float   a4;         /*!< parame a4 -> a2 /a0 */
	float   x1;         /*!< input 1 */
	float   x2;         /*!< input 2 */
	float   y1;         /*!< output 1 */
	float   y2;         /*!< output 2 */
} rtk_bt_audio_biquad_t;

/**
 * @typedef rtk_bt_audio_resample_t
 * @brief   structure of audio record configuration
 */
typedef struct {
	float                   in_rate;                /*!< input rate */
	float                   out_rate;               /*!< output rate */
	uint8_t                 in_channels;            /*!< indicate input source pcm channels */
	uint8_t                 out_channels;           /*!< indicate output pcm channels */
	uint32_t                in_frame_size;          /*!< indicate input frame size */
	uint32_t                out_frame_size;         /*!< indicate output frame size */
	float                   gain;                   /*!< gain parameter */
	uint32_t                input_samples;          /*!< input sample numbers */
	uint32_t                output_generated;       /*!< out put sample numbers */
	double                  last_position;          /*!< last index */
#if defined(RTK_BT_AUDIO_RESAMPLE_F32) && RTK_BT_AUDIO_RESAMPLE_F32
	float                   last_value[2];          /*!< last value for max two channels */
#else
	int16_t                 last_value[2];          /*!< last value for max two channels */
#endif
	double                  src_ratio;              /*!< source resample rate / samplerate */
	double                  last_ratio;             /*!< last src_ratio */
} rtk_bt_audio_resample_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_audio_resample BT Audio Resample APIs
 * @brief     BT Audio resample related function APIs
 * @ingroup   bt_audio_group
 * @{
 */

/**
 * @brief     config biquad filter
 * @param[in] filter: pointer of biquad filter
 * @param[in] type: biquad filter type. list in rtk_bt_audio_filter_type_t
 * @param[in] gain: filter gain(ref. RTK_BT_AUDIO_FILTER_DEFAULT_GAIN)
 * @param[in] freq_center: center frequency or cutoff frequency
 * @param[in] sample_rate: sample rate
 * @param[in] bandwidth: bandwidth(ref. RTK_BT_AUDIO_FILTER_DEFAULT_BANDWIDTH)
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t rtk_bt_audio_bq_config(rtk_bt_audio_biquad_t *filter, int type, float gain, unsigned long freq_center, unsigned long sample_rate, double bandwidth);

/**
 * @brief     filter function
 * @param[in] f: pointer of biquad filter
 * @param[in] buffer: pointer of input data buffer
 * @param[in] num_samples: samples number
 * @param[in] gap: channels
 * @return    None
 */
void rtk_bt_audio_biquad_apply(rtk_bt_audio_biquad_t *f, float *buffer, int num_samples, int gap);

/**
 * @brief     allocate resample struct
 * @param[in] in_rate: input audio data sample rate
 * @param[in] out_rate: output audio data sample rate
 * @param[in] in_channels: input audio data channels
 * @param[in] out_channels: output audio data channels
 * @param[in] input_frames: input frames
 * @return
 *            - NULL  : fail
 *            - others: pointer
 */
rtk_bt_audio_resample_t *rtk_bt_audio_resample_alloc(float in_rate, float out_rate, uint8_t in_channels, uint8_t out_channels, uint32_t input_frames);

/**
 * @brief     free allocated bt audio resample struct
 * @param[in] presample: pointer to the struct
 * @return    None
 */
void rtk_bt_audio_resample_free(rtk_bt_audio_resample_t *presample);

/**
 * @brief     resample entry
 * @param[in] presample: pointer to the struct
 * @param[in] bq_t: pointer to filter
 * @param[in] input: pointer to input data
 * @param[in] input_frames_num: input frames number
 * @param[in] output: pointer to output data
 * @param[in] in_rate: input sample rate
 * @param[in] out_rate: output sample rate
 * @return    resample frames
 */
uint32_t rtk_bt_audio_resample_entry(rtk_bt_audio_resample_t *presample, rtk_bt_audio_biquad_t *bq_t, uint8_t *input, uint32_t input_frames_num,
									 uint8_t *output, uint32_t in_rate, uint32_t out_rate);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_AUDIO_RESAMPLE_H__ */