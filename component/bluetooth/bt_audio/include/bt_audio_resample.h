/**
 * @file      bt_audio_resample.h
 * @brief     BT Audio resampler type and function definition.
 * @copyright Copyright (c) 2021. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_AUDIO_RESAMPLE_H__
#define __BT_AUDIO_RESAMPLE_H__

#include "ringbuffer.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @typedef   rtk_bt_audio_resample_t
 * @brief     BT Audio resampler context holding the format conversion state and the resampled data.
 */
typedef struct {
	uint32_t in_channels;           /*!< Number of channels of the input PCM data. */
	uint32_t out_channels;          /*!< Number of channels of the output PCM data. */
	uint32_t in_rate;               /*!< Sample rate of the input PCM data. Unit: Hz. */
	uint32_t out_rate;              /*!< Sample rate of the output PCM data. Unit: Hz. */
	uint32_t in_frame_size;         /*!< Size of one input frame. Unit: bytes. */
	uint32_t out_frame_size;        /*!< Size of one output frame. Unit: bytes. */
	uint32_t init_input_frames;     /*!< Maximum number of input frames accepted per feed call. */
	uint32_t init_output_frames;    /*!< Capacity of the intermediate output buffer. Unit: frames. */
	void *presampler;               /*!< Speex resampler state. */
	uint32_t out_buffer_size;       /*!< Size of the intermediate output buffer. Unit: bytes. */
	void *out_buffer;               /*!< Intermediate buffer holding the output of one resample pass. */
	RingBuffer *p_buffer;           /*!< Ring buffer holding the resampled data waiting to be read. */
} rtk_bt_audio_resample_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_audio_resample BT Audio Resample APIs
 * @brief     BT Audio resampler APIs.
 * @ingroup   bt_audio_group
 * @{
 */

/**
 * @brief     Allocate a resampler context.
 *            The input and output data are interleaved 16-bit PCM.
 * @param[in] in_rate: Sample rate of the input data.
 *                     Unit: Hz.
 * @param[in] out_rate: Sample rate of the output data.
 *                      Unit: Hz.
 * @param[in] in_channels: Number of channels of the input data.
 * @param[in] out_channels: Number of channels of the output data.
 * @param[in] input_frames: Input frames.
 * @return
 *            - NULL: Failed to allocate the resampler.
 *            - Others: Pointer to the allocated resampler context.
 */
rtk_bt_audio_resample_t *rtk_bt_audio_resample_alloc(uint32_t in_rate, uint32_t out_rate, uint32_t in_channels, uint32_t out_channels, uint32_t input_frames);

/**
 * @brief     Free a resampler context and the buffers it owns.
 * @param[in] presample: Resampler context returned by @ref rtk_bt_audio_resample_alloc.
 *                       Must not be NULL.
 */
void rtk_bt_audio_resample_free(rtk_bt_audio_resample_t *presample);

/**
 * @brief     Get the amount of resampled data that can be read.
 * @param[in] p_bt_resample: Resampler context.
 *                           Must not be NULL.
 * @return
 *            - Size of the resampled data available for reading, in bytes. 0 if p_bt_resample is invalid.
 */
uint32_t rtk_bt_audio_resample_available_data(rtk_bt_audio_resample_t *p_bt_resample);

/**
 * @brief     Resample input data and store the result for later reading.
 *            When the input and output sample rates are equal, only the channel layout is converted.
 * @param[in] p_bt_resample: Resampler context.
 *                           Must not be NULL.
 * @param[in] input: Interleaved 16-bit PCM input data.
 *                   Must not be NULL.
 * @param[in] input_frames_num: Number of input frames in input.
 *                              Must be non-zero and must not exceed the input_frames passed to @ref rtk_bt_audio_resample_alloc.
 * @return
 *            - Number of input frames consumed.
 *            - 0: Failed due to an invalid parameter, a resample error or not enough room for the resampled data.
 */
uint32_t rtk_bt_audio_resample_feed(rtk_bt_audio_resample_t *p_bt_resample, uint8_t *input, uint32_t input_frames_num);

/**
 * @brief     Read resampled data.
 * @param[in] p_bt_resample: Resampler context.
 *                           Must not be NULL.
 * @param[out] buffer: Buffer that receives the resampled data.
 *                     Must not be NULL.
 * @param[in] size: Number of bytes to read.
 * @return
 *            - 0: Succeed.
 *            - 1: Invalid parameter.
 *            - -1: Not enough resampled data available.
 */
int16_t rtk_bt_audio_resample_read(rtk_bt_audio_resample_t *p_bt_resample, void *buffer, uint32_t size);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_AUDIO_RESAMPLE_H__ */
