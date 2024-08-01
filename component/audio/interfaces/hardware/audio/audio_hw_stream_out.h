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
 * @file audio_hw_stream_out.h
 *
 * @brief Provides APIs for audio streaming out.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_STREAM_OUT_H
#define AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_STREAM_OUT_H

#include <sys/types.h>

#include "hardware/audio/audio_hw_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief AudioHwStreamOut is the abstraction of the audio output streaming.
 *
 * @since 1.0
 * @version 1.0
 */
struct AudioHwStreamOut {
	/**
	 * Common interfaces of the audio stream out. This should be the first member of AudioHwStreamOut
	 * because users of this structure will cast a AudioHwStream to AudioHwStreamOut pointer.
	 */
	struct AudioHwStream common;

	/**
	 * @brief Get system latency of the current AudioHwStreamOut.
	 *
	 * @param stream is the pointer of the audio stream out.
	 * @return Returns the current stream out latency;
	 */
	uint32_t (*GetLatency)(const struct AudioHwStreamOut *stream);

	/**
	 * @brief Get current stream out frames and timestamp of the current AudioHwStreamOut.
	 *
	 * @param stream is the pointer of the audio stream out.
	 * @param frames is the pointer of the current stream out frames of the current AudioHwStreamOut.
	 * @param timestamp is the pointer of the current timestamp of the current AudioHwStreamOut.
	 * @return Returns 0 if stream out frames and timestamp get successfully;
	 * returns < 0 if error happens.
	 */
	int (*GetPresentationPosition)(const struct AudioHwStreamOut *stream, uint64_t *frames, struct timespec *timestamp);

	/**
	 * @brief Get current stream out time of the current AudioHwStreamOut.
	 *
	 * @param stream is the pointer of the audio stream out.
	 * @param now_ns is the pointer to system time.
	 * @param audio_ns is the pointer of the playing audio time.
	 * @return Returns 0 if stream out frames and timestamp get successfully;
	 * returns < 0 if error happens.
	 */
	int (*GetPresentTime)(const struct AudioHwStreamOut *stream, int64_t *now_ns, int64_t *audio_ns);

	/**
	 * @brief Get trigger time of the current AudioHwStreamOut.
	 *
	 * @param stream is the pointer of the audio stream out.
	 * @return Returns stream out start or stop time;
	 * returns < 0 if error happens.
	 */
	int64_t (*GetTriggerTime)(const struct AudioHwStreamOut *stream);

	/**
	 * @brief Write data to sound card for the current AudioHwStreamOut.
	 *
	 * @param stream is the pointer of the audio stream out.
	 * @param buffer is the pointer of the current stream out buffer to write to driver of the current AudioHwStreamOut.
	 * @param bytes  is the size of the buffer.
	 * @param block  is if dma buffer is full, should write block.
	 * @return Returns size written to driver;
	 */
	ssize_t (*Write)(struct AudioHwStreamOut *stream, const void *buffer, size_t bytes, bool block);

	/**
	 * @brief Set volume for the current AudioHwStreamOut.
	 *
	 * @param stream is the pointer of the audio stream out.
	 * @param left is volume of left channel, ranges from 0.0 to 1.0.
	 * @param right is volume of right channel, ranges from 0.0 to 1.0.
	 * @return Returns 0 if volume is set successfully;
	 * returns < 0 otherwise.
	 */
	int (*SetVolume)(struct AudioHwStreamOut *stream, float left, float right);

	/**
	 * @brief Pause current audio stream out.
	 *
	 * @param stream is the pointer of the audio stream out.
	 * @return Returns 0 if paused successfully;
	 * returns < 0 otherwise.
	 */
	int (*Pause)(struct AudioHwStreamOut *stream);

	/**
	 * @brief Resume current audio stream out.
	 *
	 * @param stream is the pointer of the audio stream out.
	 * @return Returns 0 if resumed successfully;
	 * returns < 0 otherwise.
	 */
	int (*Resume)(struct AudioHwStreamOut *stream);

	/**
	 * @brief Flush current audio stream out.
	 *
	 * @param stream is the pointer of the audio stream out.
	 * @return Returns 0 if flushed successfully;
	 * returns < 0 otherwise.
	 */
	int (*Flush)(struct AudioHwStreamOut *stream);
};

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_STREAM_OUT_H
/** @} */
