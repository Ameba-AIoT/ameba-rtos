/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AMEBA_AUDIO_AUDIO_HAL_COMMON_AUDIO_HW_DEBUG_H
#define AMEBA_AUDIO_AUDIO_HAL_COMMON_AUDIO_HW_DEBUG_H

#include "ameba.h"
#include "basic_types.h"
#include "os_wrapper.h"


/* Debug options */
#define HAL_AUDIO_COMMON_DEBUG                1
#define HAL_AUDIO_VERBOSE_DEBUG               0
#define HAL_AUDIO_PLAYBACK_VERY_VERBOSE_DEBUG 0
#define HAL_AUDIO_CAPTURE_VERY_VERBOSE_DEBUG  0
#define HAL_AUDIO_PLAYBACK_DUMP_DEBUG         0
#define HAL_AUDIO_CAPTURE_DUMP_DEBUG          0

#define HAL_AUDIO_ERROR(fmt, args...)         RTK_LOGE("AudioHal", "[%s]: " fmt "\n", __FUNCTION__, ## args)
#define HAL_AUDIO_DUMP_INFO(fmt, args...)     RTK_LOGI("AudioHal", "[%s]: " fmt "\n", __FUNCTION__, ## args)

#if HAL_AUDIO_COMMON_DEBUG
#define HAL_AUDIO_DEBUG(fmt, args...)         RTK_LOGD("AudioHal", "[%s]: " fmt "\n", __func__, ## args)
#define HAL_AUDIO_INFO(fmt, args...)          RTK_LOGI("AudioHal", "[%s]: " fmt "\n", __FUNCTION__, ## args)
#define HAL_AUDIO_WARN(fmt, args...)          RTK_LOGW("AudioHal", "[%s]: " fmt "\n", __FUNCTION__, ## args)
#define HAL_AUDIO_ENTER                       RTK_LOGA("AudioHal", "[%s]: enter\n", __FUNCTION__)
#define HAL_AUDIO_EXIT                        RTK_LOGA("AudioHal", "[%s]: exit\n", __FUNCTION__)
#define HAL_AUDIO_EXIT_ERR                    RTK_LOGA("AudioHal", "[%s]: error: exit\n", __FUNCTION__)
#define HAL_AUDIO_TRACE                       RTK_LOGA("AudioHal", "[%s]: line:%d\n", __FUNCTION__, __LINE__)
#define HAL_AUDIO_IRQ_INFO(fmt, args...)      DiagPrintf("AudioHal [%s]: " fmt "\n", __FUNCTION__, ## args)
#else
#define HAL_AUDIO_DEBUG(fmt, args...)         do { } while(0)
#define HAL_AUDIO_INFO(fmt, args...)          do { } while(0)
#define HAL_AUDIO_WARN(fmt, args...)          do { } while(0)

#define HAL_AUDIO_ENTER                       do { } while(0)
#define HAL_AUDIO_EXIT                        do { } while(0)
#define HAL_AUDIO_EXIT_ERR                    do { } while(0)
#define HAL_AUDIO_TRACE                       do { } while(0)
#define HAL_AUDIO_IRQ_INFO(fmt, args...)      do { } while(0)
#endif

#if HAL_AUDIO_COMMON_DEBUG && HAL_AUDIO_VERBOSE_DEBUG
#define HAL_AUDIO_VERBOSE(fmt, args...)       RTK_LOGA("AudioHal", "=>V [%s]: " fmt "\n", __func__, ##args)
#else
#define HAL_AUDIO_VERBOSE(fmt, args...)       do { } while(0)
#endif

#if HAL_AUDIO_COMMON_DEBUG && HAL_AUDIO_VERBOSE_DEBUG && HAL_AUDIO_PLAYBACK_VERY_VERBOSE_DEBUG
#define HAL_AUDIO_PVERBOSE(fmt, args...)      RTK_LOGA("AudioHal", "=>PV [%s]: " fmt "\n", __FUNCTION__, ## args)
#else
#define HAL_AUDIO_PVERBOSE(fmt, args...)      do { } while(0)
#endif

#if HAL_AUDIO_COMMON_DEBUG && HAL_AUDIO_VERBOSE_DEBUG && HAL_AUDIO_CAPTURE_VERY_VERBOSE_DEBUG
#define HAL_AUDIO_CVERBOSE(fmt, args...)      RTK_LOGA("AudioHal", "=>CV [%s]: " fmt "\n", __FUNCTION__, ## args)
#else
#define HAL_AUDIO_CVERBOSE(fmt, args...)      do { } while(0)
#endif

#endif
