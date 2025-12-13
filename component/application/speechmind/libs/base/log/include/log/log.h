/*
 * Copyright (c) 2025 Realtek, LLC.
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

#ifndef AMEBA_AUDIO_BASE_LOG_INCLUDE_LOG_LOG_H
#define AMEBA_AUDIO_BASE_LOG_INCLUDE_LOG_LOG_H

#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * LOG_TAG is the local tag used for the following simplified
 * logging macros. This preprocessor definition can be changed
 * before using the other macros to change the tag.
 */
#ifndef LOG_TAG
#define LOG_TAG NULL
#endif

#ifndef __predict_false
#define __predict_false(exp) __builtin_expect((exp) != 0, 0)
#endif

// ---------------------------------------------------------------------

typedef enum {
    MEDIA_LOG_VERBOSE = 0,
    MEDIA_LOG_DEBUG,
    MEDIA_LOG_INFO,
    MEDIA_LOG_WARN,
    MEDIA_LOG_ERROR,
    MEDIA_LOG_FATAL,
    MEDIA_LOG_LEVEL_MAX,
} ameba_media_log_t;

/**
 * Logging callback handler definition.
 *
 * @param level One of the MEDIA_LOG_* values.
 * @param message Zero-terminated string message to log.
 */
typedef void (*ameba_media_log_handler_t)(ameba_media_log_t level, const char *message);

/**
 * Get current log level.
 *
 * @return One of MEDIA_LOG_* values.
 */
ameba_media_log_t ameba_media_get_log_level(void);

/**
 * Sets log level to the specified value.
 *
 * @param level One of the MEDIA_LOG_* values.
 */
void ameba_media_set_log_level(ameba_media_log_t level);

/**
 * Set custom log callback handler.
 *
 * @param handler The logging handler to use or @p NULL to use default handler.
 */
void ameba_media_set_log_handler(ameba_media_log_handler_t handler);

/**
 * Writes the given text to output. (Internal function)
 *
 * @param level One of MEDIA_LOG_* values.
 * @param tag The tag to show in log.
 * @param fmt The format string to use.
 */
void ameba_media_log_print(ameba_media_log_t level, const char *tag, const char *fmt, ...);

/**
 * Writes the given text to output. (Internal function)
 *
 * @param cond The cond string to show.
 * @param tag The tag to show in log.
 * @param fmt The format string to use.
 */
void ameba_media_log_assert(const char *cond, const char *tag, const char *fmt, ...);

// ---------------------------------------------------------------------

/*
 * Log a fatal error. If given condition fails, this stops program
 * execution like a normal assertion, but also generating the given message.
 */
#ifndef MEDIA_LOG_ALWAYS_FATAL_IF
#define MEDIA_LOG_ALWAYS_FATAL_IF(cond, ...)                           \
  ((__predict_false(cond))                                             \
       ? (ameba_media_log_assert(#cond, LOG_TAG, "%s", ##__VA_ARGS__)) \
       : (void)0)
#endif

#ifndef MEDIA_LOG_ALWAYS_FATAL
#define MEDIA_LOG_ALWAYS_FATAL(...) \
  (ameba_media_log_assert(NULL, LOG_TAG, "%s", ##__VA_ARGS__))
#endif

/*
 * Assertion that generates a log message when the assertion fails.
 * Stripped out of release builds.  Uses the current LOG_TAG.
 */
#ifndef MEDIA_LOG_ASSERT
#define MEDIA_LOG_ASSERT(cond, ...) MEDIA_LOG_ALWAYS_FATAL_IF(!(cond), ##__VA_ARGS__)
#endif


// ---------------------------------------------------------------------

/*
 * C/C++ logging functions.
 */

#ifndef MEDIA_LOGV
#define MEDIA_LOGV(...) ((void)MEDIA_LOG(MEDIA_LOG_VERBOSE, LOG_TAG, __VA_ARGS__))
#endif

#ifndef MEDIA_LOGV_IF
#define MEDIA_LOGV_IF(cond, ...)                                                        \
  ((__predict_false(cond)) ? ((void)MEDIA_LOG(MEDIA_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)) \
                           : (void)0)
#endif

#ifndef MEDIA_LOGD
#define MEDIA_LOGD(...) ((void)MEDIA_LOG(MEDIA_LOG_DEBUG, LOG_TAG, __VA_ARGS__))
#endif

#ifndef MEDIA_LOGD_IF
#define MEDIA_LOGD_IF(cond, ...)                                                      \
  ((__predict_false(cond)) ? ((void)MEDIA_LOG(MEDIA_LOG_DEBUG, LOG_TAG, __VA_ARGS__)) \
                           : (void)0)
#endif

#ifndef MEDIA_LOGI
#define MEDIA_LOGI(...) ((void)MEDIA_LOG(MEDIA_LOG_INFO, LOG_TAG, __VA_ARGS__))
#endif

#ifndef MEDIA_LOGI_IF
#define MEDIA_LOGI_IF(cond, ...)                                                     \
  ((__predict_false(cond)) ? ((void)MEDIA_LOG(MEDIA_LOG_INFO, LOG_TAG, __VA_ARGS__)) \
                           : (void)0)
#endif

#ifndef MEDIA_LOGW
#define MEDIA_LOGW(...) ((void)MEDIA_LOG(MEDIA_LOG_WARN, LOG_TAG, __VA_ARGS__))
#endif

#ifndef MEDIA_LOGW_IF
#define MEDIA_LOGW_IF(cond, ...)                                                     \
  ((__predict_false(cond)) ? ((void)MEDIA_LOG(MEDIA_LOG_WARN, LOG_TAG, __VA_ARGS__)) \
                           : (void)0)
#endif

#ifndef MEDIA_LOGE
#define MEDIA_LOGE(...) ((void)MEDIA_LOG(MEDIA_LOG_ERROR, LOG_TAG, __VA_ARGS__))
#endif

#ifndef MEDIA_LOGE_IF
#define MEDIA_LOGE_IF(cond, ...)                                                      \
  ((__predict_false(cond)) ? ((void)MEDIA_LOG(MEDIA_LOG_ERROR, LOG_TAG, __VA_ARGS__)) \
                           : (void)0)
#endif

/*
 * MEDIA_LOG - Basic log message macro.
 * Example:
 *     MEDIA_LOG(LOG_WARN, "FooBar", "Failed with error %d", errno);
 */
#ifndef MEDIA_LOG
#define MEDIA_LOG(level, tag, ...) \
    (((int)((level)) >= (int)ameba_media_get_log_level()) ? ameba_media_log_print((level), (tag), __VA_ARGS__) : (void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif // AMEBA_AUDIO_BASE_LOG_INCLUDE_LOG_LOG_H
