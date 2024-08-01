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
 * @file audio_hw_stream.h
 *
 * @brief Provides APIs for audio stream base.
 *
 * @since 1.0
 * @version 1.0
 */


#ifndef AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_STREAM_H
#define AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_STREAM_H

#include "hardware/audio/audio_hw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Provides the basic functions for audio streams including audio stream in and audio stream out.
 * @since 1.0
 * @version 1.0
 */
struct AudioHwStream {

	/**
	 * @brief Return the sampling rate in Hz for AudioHwStream.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @return Returns the sampling rate for the stream
	 */
	uint32_t (*GetSampleRate)(const struct AudioHwStream *stream);

	/**
	 * @brief Set the sampling rate in Hz to AudioHwStream.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @param rate is the sampling rate set to the audio stream.
	 */
	int (*SetSampleRate)(struct AudioHwStream *stream, uint32_t rate);

	/**
	 * @brief Get size of input/output buffer in bytes for this stream for AudioHwStream,
	 * It should be multiple of the frame size.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @return Returns the buffer size for the stream.
	 */
	size_t (*GetBufferSize)(const struct AudioHwStream *stream);

	/**
	 * @brief Return the channel count for AudioHwStream.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @return Returns the channel count for the stream
	 */
	uint32_t (*GetChannels)(const struct AudioHwStream *stream);

	/**
	 * @brief Set the channel count to AudioHwStream.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @param channel is the channel count set to the audio stream.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*SetChannels)(const struct AudioHwStream *stream, uint32_t channel);

	/**
	 * @brief Return the audio format for AudioHwStream.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @return Returns the audio format for the stream.
	 */
	enum AudioHwFormat(*GetFormat)(const struct AudioHwStream *stream);

	/**
	 * @brief Set the audio format to AudioHwStream.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @param format is the audio format set to the audio stream.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*SetFormat)(struct AudioHwStream *stream, enum AudioHwFormat format);

	/**
	 * @brief Notify audio stream to standby.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*Standby)(struct AudioHwStream *stream);

	/**
	 * @brief dump audio stream status.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*Dump)(const struct AudioHwStream *stream, int fd);

	/**
	 * @brief set audio stream parameters.
	 *
	 * @param key is the object to set parameters.
	 * @param value is the value to set parameters.
	 * @return Returns 0 if the operation is successful;
	 * returns < 0 if error happens.
	 */
	int (*SetParameters)(struct AudioHwStream *stream, const char *strs);

	/**
	 * @brief get audio stream parameters.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @param keys is key to get parameters.
	 * @return Returns value of the keys.
	 */
	char *(*GetParameters)(const struct AudioHwStream *stream, const char *keys);

	/**
	 * @brief get buffer status.
	 *
	 * @param stream is the pointer of the audio stream.
	 * @return Returns returns buffer status.
	 */
	uint32_t (*GetBufferStatus)(struct AudioHwStream *stream);
};

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_HARDWARE_INTERFACES_HARDWARE_AUDIO_AUDIO_HW_STREAM_H
/** @} */
