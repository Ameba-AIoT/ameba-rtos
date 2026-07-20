/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/** @addtogroup Ameba_Logsys
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * Log System:
  *   - Six severity levels: NONE, ALWAYS, ERROR, WARN, INFO, DEBUG
  *   - COMPIL_LOG_LEVEL gates messages at compile time; calls above the threshold
  *     produce no code or data in the binary
  *   - Per-tag runtime level cache (up to LOG_TAG_CACHE_ARRAY_SIZE entries) allows
  *     fine-grained output control without rebuilding
  *   - Two write paths: rtk_log_write() for normal images,
  *     rtk_log_write_nano() for ROM/bootloader (reduced footprint)
  *   - On CA32 cores, rtk_log_mutex_init() must be called before any log output
  *     to initialize the internal print mutex
  *
  *****************************************************************************************
  * How to use the Log System
  *****************************************************************************************
  *   1. Optionally set COMPIL_LOG_LEVEL before including this header to adjust
  *      the compile-time gate (default: RTK_LOG_INFO):
  *        #define COMPIL_LOG_LEVEL  RTK_LOG_DEBUG
  *        #include "log.h"
  *
  *   2. Define a module tag string:
  *        static const char *const TAG = "WIFI";
  *
  *   3. Use RTK_LOGS() to emit messages — recommended entry point:
  *        RTK_LOGS(TAG, RTK_LOG_INFO, "Link up, RSSI=%d dBm\n", rssi);
  *
  *   4. Adjust per-tag runtime level on demand:
  *        rtk_log_level_set(TAG, RTK_LOG_DEBUG);
  *
  *   @note RTK_LOGx() macros (RTK_LOGA / RTK_LOGE / RTK_LOGW / RTK_LOGI / RTK_LOGD)
  *         are kept for backward compatibility; RTK_LOGS() is preferred for new code.
  *   @note Use NOTAG as the tag argument when no module label is needed.
  *
  *****************************************************************************************
  * @endverbatim
  */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup Ameba_Logsys Log System
  * @brief Ameba leveled log subsystem
  * @{
  */

#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported Types --------------------------------------------------------*/
/** @addtogroup Logsys_Exported_Types Exported Types
  * @{
  */

/**
  * @brief Log output level definition, in ascending verbosity order.
  */
typedef enum {
	RTK_LOG_NONE,       /*!< Logging completely disabled */
	RTK_LOG_ALWAYS,     /*!< Always printed; not classified as warning or error */
	RTK_LOG_ERROR,      /*!< Error conditions */
	RTK_LOG_WARN,       /*!< Warning conditions */
	RTK_LOG_INFO,       /*!< Informational messages (default level) */
	RTK_LOG_DEBUG       /*!< Debug-level messages */
} rtk_log_level_t;

/**
  * @}
  */

/** @addtogroup Logsys_Exported_Constants
  * @{
  */

/**
  * @brief Maximum number of entries held in the runtime tag-level cache.
  */
#define LOG_TAG_CACHE_ARRAY_SIZE    4

/**
  * @brief Maximum tag string length, excluding the NUL terminator.
  */
#define LOG_TAG_MAX_LEN             9

/**
  * @}
  */

/** @addtogroup Logsys_Exported_Types
  * @{
  */

/**
  * @brief Per-tag runtime log level cache entry.
  */
typedef struct {
	rtk_log_level_t level;              /*!< Cached log level for this tag */
	char            tag[LOG_TAG_MAX_LEN + 1]; /*!< NUL-terminated tag string (max LOG_TAG_MAX_LEN chars) */
} rtk_log_tag_t, *rtk_log_tag_p;

/** @cond private */
extern rtk_log_tag_t rtk_log_tag_array[LOG_TAG_CACHE_ARRAY_SIZE];
/** @endcond */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup Logsys_Exported_Constants Exported Constants
  * @{
  */

/** @defgroup Logsys_Compile_Options Compile-time Options
  * @{
  */

/**
  * @brief Default log level used when COMPIL_LOG_LEVEL is not overridden.
  *        Resolves to RTK_LOG_INFO.
  */
#define RTK_LOG_DEFAULT_LEVEL        RTK_LOG_INFO

/**
  * @brief Compile-time log level gate.
  *
  * Log calls whose level exceeds this value are completely removed by the
  * preprocessor and produce neither code nor data in the binary.
  * @note Define before including this header to override the default.
  */
#ifndef COMPIL_LOG_LEVEL
#define COMPIL_LOG_LEVEL                RTK_LOG_DEFAULT_LEVEL
#endif

/**
  * @}
  */

/** @defgroup Logsys_Logging_Macros Logging Macros
  * @{
  */

/**
  * @brief Special tag token for log messages that carry no module label.
  */
#define NOTAG "#"

/**
  * @brief Number of 32-bit words printed per row by rtk_log_memory_dump_word().
  */
#define DISPLAY_NUMBER 8

/**
  * @brief Number of bytes printed per row by rtk_log_memory_dump_byte().
  */
#define BYTES_PER_LINE 16

/**
  * @brief  Core log dispatch macro; compile-time gated by COMPIL_LOG_LEVEL.
  * @param  level   Log level (::rtk_log_level_t).
  * @param  tag     Module tag string; use NOTAG for untagged output.
  * @param  format  printf-style format string.
  * @param  letter  Single-character level indicator: A/E/W/I/D.
  * @param  ...     Additional arguments for format.
  * @note   Calls above COMPIL_LOG_LEVEL are removed entirely by the preprocessor.
  *         Use RTK_LOGx() or RTK_LOGS() wrappers in application code.
  */
#define RTK_LOG_ITEM(level, tag, format, letter, ...) do {               \
        if ( COMPIL_LOG_LEVEL >= level ) rtk_log_write(level, tag, letter, format, ##__VA_ARGS__); \
    } while(0)

/**
  * @brief  Log at ALWAYS level (bypasses runtime tag filter). @see RTK_LOGS
  */
#define RTK_LOGA( tag, format, ... ) RTK_LOG_ITEM(RTK_LOG_ALWAYS,  tag, format, 'A', ##__VA_ARGS__)

/**
  * @brief  Log at ERROR level. @see RTK_LOGS
  */
#define RTK_LOGE( tag, format, ... ) RTK_LOG_ITEM(RTK_LOG_ERROR,   tag, format, 'E', ##__VA_ARGS__)

/**
  * @brief  Log at WARN level. @see RTK_LOGS
  */
#define RTK_LOGW( tag, format, ... ) RTK_LOG_ITEM(RTK_LOG_WARN,    tag, format, 'W', ##__VA_ARGS__)

/**
  * @brief  Log at INFO level. @see RTK_LOGS
  */
#define RTK_LOGI( tag, format, ... ) RTK_LOG_ITEM(RTK_LOG_INFO,    tag, format, 'I', ##__VA_ARGS__)

/**
  * @brief  Log at DEBUG level. @see RTK_LOGS
  */
#define RTK_LOGD( tag, format, ... ) RTK_LOG_ITEM(RTK_LOG_DEBUG,   tag, format, 'D', ##__VA_ARGS__)

/** @cond private */
#define RTK_LOG_ITEMS(level, tag, format, ...) do { 					  \
        if (level==RTK_LOG_ALWAYS )         { rtk_log_write_nano(RTK_LOG_ALWAYS,   tag, 'A', format, ##__VA_ARGS__); } \
        else if (level==RTK_LOG_ERROR )     { rtk_log_write_nano(RTK_LOG_ERROR,    tag, 'E', format, ##__VA_ARGS__); } \
        else if (level==RTK_LOG_WARN )      { rtk_log_write_nano(RTK_LOG_WARN,     tag, 'W', format, ##__VA_ARGS__); } \
        else if (level==RTK_LOG_INFO )      { rtk_log_write_nano(RTK_LOG_INFO,     tag, 'I', format, ##__VA_ARGS__); } \
        else                                { rtk_log_write_nano(RTK_LOG_DEBUG,    tag, 'D', format, ##__VA_ARGS__); } \
	}while(0)

#define RTK_LOG_ITEMS_LEVEL(level, tag, format, ...) do {               \
		if ( COMPIL_LOG_LEVEL >= level ) RTK_LOG_ITEMS(level, tag, format, ##__VA_ARGS__); \
	} while(0)
/** @endcond */

/**
  * @brief  Recommended log macro — emit a formatted message at the specified level.
  * @param  tag     Module tag string; use NOTAG for untagged output.
  * @param  level   Log level (::rtk_log_level_t).
  * @param  format  printf-style format string.
  * @param  ...     Additional arguments for format.
  * @note   Gated by COMPIL_LOG_LEVEL at compile time and by the per-tag runtime
  *         level cache at runtime.
  * @code{.c}
  * static const char *const TAG = "WIFI";
  * RTK_LOGS(TAG, RTK_LOG_INFO, "Link up, RSSI=%d dBm\n", rssi);
  * @endcode
  */
#define RTK_LOGS( tag, level, format, ... ) RTK_LOG_ITEMS_LEVEL(level, tag,  format, ##__VA_ARGS__)

/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup Logsys_Exported_Functions Exported Functions
  * @{
  */

/** @defgroup Logsys_Level_Control Level Control
  * @{
  */

/**
  * @brief  Clear all entries in the per-tag runtime level cache.
  *         After this call @ref rtk_log_level_get() returns RTK_LOG_DEFAULT_LEVEL
  *         for every tag until new entries are written via @ref rtk_log_level_set().
  */
void rtk_log_array_clear(void);

/**
  * @brief  Query the effective log level for a tag.
  *         Searches the runtime cache for tag. Returns the cached level when
  *         found, or RTK_LOG_DEFAULT_LEVEL if the tag has no cache entry.
  * @param  tag  NUL-terminated module tag string.
  * @return Effective ::rtk_log_level_t for tag.
  */
rtk_log_level_t rtk_log_level_get(const char *tag);

/**
  * @brief  Set the runtime log level for a tag.
  *         Writes or updates a tag entry in the runtime level cache. When the
  *         cache is full and the tag is new, the oldest entry is evicted.
  * @param  tag    NUL-terminated module tag string to configure.
  * @param  level  New log level (::rtk_log_level_t).
  * @return Status code:
  *         - 0: Success.
  *         - -1: Invalid argument; tag is NULL or level exceeds RTK_LOG_DEBUG.
  */
int rtk_log_level_set(const char *tag, rtk_log_level_t level);

/**
  * @brief  Print all entries in a tag-level cache array.
  * @param  log_tag_array      Pointer to the cache array to print.
  * @return Status code:
  *         - 0: Success (RTK_SUCCESS).
  *         - -1: log_tag_array is NULL (RTK_FAIL).
  */
int rtk_log_array_print(rtk_log_tag_t *log_tag_array);

/**
  * @}
  */

/** @defgroup Logsys_Write Write
  * @{
  */

/**
  * @brief  Low-level formatted log write (standard path).
  *         Formats and outputs a log line prefixed with the level letter and tag.
  *         Called internally by RTK_LOG_ITEM(); use RTK_LOGS() or RTK_LOGx() macros
  *         in application code.
  * @param  level   Log level (::rtk_log_level_t).
  * @param  tag     Module tag string.
  * @param  letter  Single-character level indicator (A/E/W/I/D).
  * @param  fmt     printf-style format string.
  * @param  ...     Additional arguments for fmt.
  */
void rtk_log_write(rtk_log_level_t level, const char *tag, const char letter, const char *fmt, ...);

/**
  * @brief  Low-footprint log write variant optimized for ROM/bootloader.
  *         Functionally equivalent to @ref rtk_log_write() but uses a compact internal
  *         buffer to minimize code size. Called internally by RTK_LOG_ITEMS().
  * @param  level   Log level (::rtk_log_level_t).
  * @param  tag     Module tag string.
  * @param  letter  Single-character level indicator (A/E/W/I/D).
  * @param  fmt     printf-style format string.
  * @param  ...     Additional arguments for fmt.
  */
void rtk_log_write_nano(rtk_log_level_t level, const char *tag, const char letter, const char *fmt, ...);

#ifdef CONFIG_ARM_CORE_CA32
/**
  * @brief  Initialize the internal print mutex for CA32 multi-core log safety.
  * @note   Must be called once during system initialization on CA32 cores before
  *         any log output; no-op on other core types.
  */
void rtk_log_mutex_init(void);
#endif

/**
  * @}
  */

/** @defgroup Logsys_Memory_Dump Memory Dump
  * @{
  */

/**
  * @brief  Dump a memory region as 32-bit words to the log output.
  *         Prints len words starting at src in a formatted hexadecimal table,
  *         DISPLAY_NUMBER words per row, with address offsets.
  * @param  src  Pointer to the start of the memory region (must be 4-byte aligned).
  * @param  len  Number of 32-bit words to dump.
  */
void rtk_log_memory_dump_word(uint32_t *src, uint32_t len);

/**
  * @brief  Dump a memory region as bytes to the log output.
  *         Prints len bytes starting at src in a formatted hex + ASCII table,
  *         BYTES_PER_LINE bytes per row, with address offsets.
  * @param  src  Pointer to the start of the memory region.
  * @param  len  Number of bytes to dump.
  */
void rtk_log_memory_dump_byte(uint8_t *src, uint32_t len);

/**
  * @brief  Dump a memory region as bytes to the log output.
  *         Prints buff_len bytes starting at src_buff in a formatted hex + ASCII table,
  *         BYTES_PER_LINE bytes per row, with address offsets.
  * @param  src_buff  Pointer to the source buffer.
  * @param  buff_len  Number of bytes to dump.
  */
void rtk_log_memory_dump2char(const char *src_buff, uint32_t buff_len);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

/**
  * @}
  */

/**
  * @}
  */
