/*
 * Copyright (c) 2021 Realtek, LLC.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @addtogroup HAL
 * @{
 *
 * @brief Declares the structures and interfaces for the Hardware Abstraction Layer (HAL) module.
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file audio_hw_stream_in.h
 *
 * @brief Provides APIs for audio streaming in.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_STREAM_IN_H
#define AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_STREAM_IN_H

#include <sys/types.h>

#include "hardware/audio/audio_hw_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Defines audio stream in structure.
 *
 * @since 1.0
 * @version 1.0
 */
struct AudioHwStreamIn {
	/**
	 * Common interfaces of the audio stream in. This should be the first member of AudioHwStreamIn
	 * because users of this structure will cast a AudioHwStream to AudioHwStreamIn pointer.
	 */
	struct AudioHwStream common;

	/**
	 * @brief Get current stream in frames and timestamp of the current AudioHwStreamIn.
	 *
	 * @param stream is the pointer of the audio stream in.
	 * @param frames is the pointer of the current stream in frames of the current AudioHwStreamIn.
	 * @param timestamp is the pointer of the current timestamp of the current AudioHwStreamIn.
	 * @return Returns 0 if stream in frames and timestamp get successfully;
	 * returns < 0 if error happens.
	 */
	int (*GetCapturePosition)(const struct AudioHwStreamIn *stream, uint64_t *frames, struct timespec *timestamp);

	/**
	 * @brief Get current stream in frames and timestamp of the current AudioHwStreamIn.
	 *
	 * @param stream is the pointer of the audio stream in.
	 * @param now_ns is the pointer of reference system time.
	 * @param audio_ns is the pointer of the capturing audio time.
	 * @return Returns 0 if stream in frames and timestamp get successfully;
	 * returns < 0 if error happens.
	 */
	int (*GetPresentTime)(const struct AudioHwStreamIn *stream, int64_t *now_ns, int64_t *audio_ns);

	/**
	 * @brief Get current stream in frames and timestamp of the current AudioHwStreamIn.
	 *
	 * @param stream is the pointer of the audio stream in.
	 * @return Returns stream in start or stop time;
	 * returns < 0 if error happens.
	 */
	int64_t (*GetTriggerTime)(const struct AudioHwStreamIn *stream);

	/**
	 * @brief Get system latency of the current AudioHwStreamIn.
	 *
	 * @param stream is the pointer of the audio stream in.
	 * @return Returns the current stream in latency;
	 */
	uint32_t (*GetLatency)(const struct AudioHwStreamIn *stream);

	/**
	 * @brief Read data from sound card for the current AudioHwStreamIn.
	 *
	 * @param stream is the pointer of the audio stream in.
	 * @param buffer is the pointer of the current stream in buffer to read data from driver of the current AudioHwStreamIn.
	 * @param bytes  is the size of the buffer.
	 * @return Returns the data size read from driver;
	 */
	ssize_t (*Read)(struct AudioHwStreamIn *stream, void *buffer, size_t bytes);

	/**
	 * @brief Read data from sound card for the current AudioHwStreamIn.
	 *
	 * @param stream is the pointer of the audio stream in.
	 * @param buffer is the pointer of the current stream in buffer to read data from driver of the current AudioHwStreamIn.
	 * @param bytes  is the size of the buffer.
	 * @param time_out_ms  is the timeout ms when read blocks.
	 * @return Returns the data size read from driver, if read blocks more than time_out_ms, return -ETIMEDOUT.
	 */
	ssize_t (*ReadTimeout)(struct AudioHwStreamIn *stream, void *buffer, size_t bytes, uint32_t time_out_ms);
};

/**
 * @brief Get input buffer size for the current AudioHwStreamIn.
 *
 * @param sample_rate is sample rate of the audio stream in.
 * @param format is the sample format of the current AudioHwStreamIn.
 * @param channel_count  is the sample channels of the current AudioHwStreamIn.
 * @return Returns the buffer size read from driver.
 */
size_t GetHwInputBufferSize(uint32_t sample_rate, enum AudioHwFormat format, uint32_t channel_count);

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_STREAM_IN_H
/** @} */
