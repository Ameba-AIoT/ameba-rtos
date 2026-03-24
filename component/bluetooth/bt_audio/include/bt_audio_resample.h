/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __BT_AUDIO_RESAMPLE_H__
#define __BT_AUDIO_RESAMPLE_H__

#include "ringbuffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @typedef rtk_bt_audio_resample_t
 * @brief   structure of audio record configuration
 */
typedef struct {
	uint32_t in_channels;
	uint32_t out_channels;
	uint32_t in_rate;
	uint32_t out_rate;
	uint32_t in_frame_size;
	uint32_t out_frame_size;
	uint32_t init_input_frames;
	uint32_t init_output_frames;
	uint32_t output_generated;
	uint32_t out_buffer_size;
	int16_t last_value[2];
	double src_ratio;
	double last_ratio;
	double last_position;
	void *out_buffer;
	RingBuffer *p_buffer;
} rtk_bt_audio_resample_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_audio_resample BT Audio Resample APIs
 * @brief     BT Audio resample related function APIs
 * @ingroup   bt_audio_group
 * @{
 */

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
rtk_bt_audio_resample_t *rtk_bt_audio_resample_alloc(uint32_t in_rate, uint32_t out_rate, uint32_t in_channels, uint32_t out_channels, uint32_t input_frames);

/**
 * @brief     free allocated bt audio resample struct
 * @param[in] presample: pointer to the struct
 * @return    None
 */
void rtk_bt_audio_resample_free(rtk_bt_audio_resample_t *presample);

/**
 * @brief     read available resample data size
 * @param[in] p_bt_resample: resample handle
 * @return
 *            - available data size(bytes)
 */
uint32_t rtk_bt_audio_resample_available_data(rtk_bt_audio_resample_t *p_bt_resample);

/**
 * @brief     feed input data to resample func
 * @param[in] p_bt_resample: resample handle
 * @param[in] input: input data
 * @param[in] input_frames_num: input frames number
 * @return
 *            - buffer input data size
 */
uint32_t rtk_bt_audio_resample_feed(rtk_bt_audio_resample_t *p_bt_resample, uint8_t *input, uint32_t input_frames_num);

/**
 * @brief     read data after resample func
 * @param[in] p_bt_resample: resample handle
 * @param[in] buffer: output buffer
 * @param[in] size: read size
 * @return
 *            - 1  : fail
 *            - 0: Success
 */
int16_t rtk_bt_audio_resample_read(rtk_bt_audio_resample_t *p_bt_resample, void *buffer, uint32_t size);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_AUDIO_RESAMPLE_H__ */