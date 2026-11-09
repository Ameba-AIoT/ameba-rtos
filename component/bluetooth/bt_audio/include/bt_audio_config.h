/**
 * @file      bt_audio_config.h
 * @brief     BT Audio framework configuration macros.
 * @copyright Copyright (c) 2021. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_AUDIO_CONFIG_H__
#define __BT_AUDIO_CONFIG_H__

#define AUDIO_STREAM_TASK_STACK_SIZE            1024 * 8    /*!< Stack size of the audio stream handle task. */
#define AUDIO_STREAM_TASK_PRIORITY              4           /*!< Priority of the audio stream handle task. */
#define AUDIO_STREAM_MSG_QUEUE_SIZE             50          /*!< Depth of the audio stream message queue. Enqueueing waits while this many messages are pending. */
#define AUDIO_RENDER_BUFFER_FLAG                0           /*!< Render pre-buffering switch. 1: the stream task waits until AUDIO_RENDER_BUFFER_SIZE messages are queued before rendering. 0: disabled. */
#define AUDIO_RENDER_BUFFER_SIZE                5           /*!< Number of queued messages to accumulate before rendering starts when AUDIO_RENDER_BUFFER_FLAG is 1. */

#endif /* __BT_AUDIO_CONFIG_H__ */
