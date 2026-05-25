/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_LOGGING_LOG_H_
#define ZEPHYR_INCLUDE_LOGGING_LOG_H_

#include <zephyr/logging/log_instance.h>
#include <zephyr/logging/log_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Logging
 * @defgroup logging Logging
 * @{
 * @}
 */

/**
 * @brief Logger API
 * @defgroup log_api Logging API
 * @ingroup logger
 * @{
 */

/**
 * @brief Writes an ERROR level message to the log.
 *
 * @details It's meant to report severe errors, such as those from which it's
 * not possible to recover.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#define LOG_ERR(...)    Z_LOG(LOG_LEVEL_ERR, __VA_ARGS__)

/**
 * @brief Writes a WARNING level message to the log.
 *
 * @details It's meant to register messages related to unusual situations that
 * are not necessarily errors.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#define LOG_WRN(...)   Z_LOG(LOG_LEVEL_WRN, __VA_ARGS__)

/**
 * @brief Writes an INFO level message to the log.
 *
 * @details It's meant to write generic user oriented messages.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#define LOG_INF(...)   Z_LOG(LOG_LEVEL_INF, __VA_ARGS__)

/**
 * @brief Writes a DEBUG level message to the log.
 *
 * @details It's meant to write developer oriented information.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#define LOG_DBG(...)    Z_LOG(LOG_LEVEL_DBG, __VA_ARGS__)

/**
 * @brief Unconditionally print raw log message.
 *
 * The result is same as if printk was used but it goes through logging
 * infrastructure thus utilizes logging mode, e.g. deferred mode.
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#define LOG_PRINTK(...) Z_LOG_PRINT_DETAIL(__VA_ARGS__)

/**
 * @brief Unconditionally print raw log message.
 *
 * Provided string is printed as is without appending any characters (e.g., color or newline).
 *
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#define LOG_RAW(...) Z_LOG_PRINT_DETAIL(__VA_ARGS__)

/**
 * @brief Writes an ERROR level message associated with the instance to the log.
 *
 * Message is associated with specific instance of the module which has
 * independent filtering settings (if runtime filtering is enabled) and
 * message prefix (\<module_name\>.\<instance_name\>). It's meant to report
 * severe errors, such as those from which it's not possible to recover.
 *
 * @param _log_inst Pointer to the log structure associated with the instance.
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#define LOG_INST_ERR(_log_inst, ...) \
	Z_LOG_INSTANCE(LOG_LEVEL_ERR, _log_inst, __VA_ARGS__)

/**
 * @brief Writes a WARNING level message associated with the instance to the
 *        log.
 *
 * Message is associated with specific instance of the module which has
 * independent filtering settings (if runtime filtering is enabled) and
 * message prefix (\<module_name\>.\<instance_name\>). It's meant to register
 * messages related to unusual situations that are not necessarily errors.
 *
 * @param _log_inst Pointer to the log structure associated with the instance.
 * @param ...       A string optionally containing printk valid conversion
 *                  specifier, followed by as many values as specifiers.
 */
#define LOG_INST_WRN(_log_inst, ...) \
	Z_LOG_INSTANCE(LOG_LEVEL_WRN, _log_inst, __VA_ARGS__)

/**
 * @brief Writes an INFO level message associated with the instance to the log.
 *
 * Message is associated with specific instance of the module which has
 * independent filtering settings (if runtime filtering is enabled) and
 * message prefix (\<module_name\>.\<instance_name\>). It's meant to write
 * generic user oriented messages.
 *
 * @param _log_inst Pointer to the log structure associated with the instance.
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#define LOG_INST_INF(_log_inst, ...) \
	Z_LOG_INSTANCE(LOG_LEVEL_INF, _log_inst, __VA_ARGS__)

/**
 * @brief Writes a DEBUG level message associated with the instance to the log.
 *
 * Message is associated with specific instance of the module which has
 * independent filtering settings (if runtime filtering is enabled) and
 * message prefix (\<module_name\>.\<instance_name\>). It's meant to write
 * developer oriented information.
 *
 * @param _log_inst Pointer to the log structure associated with the instance.
 * @param ... A string optionally containing printk valid conversion specifier,
 * followed by as many values as specifiers.
 */
#define LOG_INST_DBG(_log_inst, ...) \
	Z_LOG_INSTANCE(LOG_LEVEL_DBG, _log_inst, __VA_ARGS__)

/**
 * @brief Writes an ERROR level hexdump message to the log.
 *
 * @details It's meant to report severe errors, such as those from which it's
 * not possible to recover.
 *
 * @param _data   Pointer to the data to be logged.
 * @param _length Length of data (in bytes).
 * @param _str    Persistent, raw string.
 */
#define LOG_HEXDUMP_ERR(_data, _length, _str) \
	Z_LOG_HEXDUMP(LOG_LEVEL_ERR, _data, _length, _str)

/**
 * @brief Writes a WARNING level message to the log.
 *
 * @details It's meant to register messages related to unusual situations that
 * are not necessarily errors.
 *
 * @param _data   Pointer to the data to be logged.
 * @param _length Length of data (in bytes).
 * @param _str    Persistent, raw string.
 */
#define LOG_HEXDUMP_WRN(_data, _length, _str) \
	Z_LOG_HEXDUMP(LOG_LEVEL_WRN, _data, _length, _str)

/**
 * @brief Writes an INFO level message to the log.
 *
 * @details It's meant to write generic user oriented messages.
 *
 * @param _data   Pointer to the data to be logged.
 * @param _length Length of data (in bytes).
 * @param _str    Persistent, raw string.
 */
#define LOG_HEXDUMP_INF(_data, _length, _str) \
	Z_LOG_HEXDUMP(LOG_LEVEL_INF, _data, _length, _str)

/**
 * @brief Writes a DEBUG level message to the log.
 *
 * @details It's meant to write developer oriented information.
 *
 * @param _data   Pointer to the data to be logged.
 * @param _length Length of data (in bytes).
 * @param _str    Persistent, raw string.
 */
#define LOG_HEXDUMP_DBG(_data, _length, _str) \
	Z_LOG_HEXDUMP(LOG_LEVEL_DBG, _data, _length, _str)

/**
 * @brief Writes an ERROR hexdump message associated with the instance to the
 *        log.
 *
 * Message is associated with specific instance of the module which has
 * independent filtering settings (if runtime filtering is enabled) and
 * message prefix (\<module_name\>.\<instance_name\>). It's meant to report
 * severe errors, such as those from which it's not possible to recover.
 *
 * @param _log_inst   Pointer to the log structure associated with the instance.
 * @param _data       Pointer to the data to be logged.
 * @param _length     Length of data (in bytes).
 * @param _str        Persistent, raw string.
 */
#define LOG_INST_HEXDUMP_ERR(_log_inst, _data, _length, _str) \
	Z_LOG_HEXDUMP_INSTANCE(LOG_LEVEL_ERR, _log_inst, _data, _length, _str)

/**
 * @brief Writes a WARNING level hexdump message associated with the instance to
 *        the log.
 *
 * @details It's meant to register messages related to unusual situations that
 * are not necessarily errors.
 *
 * @param _log_inst   Pointer to the log structure associated with the instance.
 * @param _data       Pointer to the data to be logged.
 * @param _length     Length of data (in bytes).
 * @param _str        Persistent, raw string.
 */
#define LOG_INST_HEXDUMP_WRN(_log_inst, _data, _length, _str) \
	Z_LOG_HEXDUMP_INSTANCE(LOG_LEVEL_WRN, _log_inst, _data, _length, _str)

/**
 * @brief Writes an INFO level hexdump message associated with the instance to
 *        the log.
 *
 * @details It's meant to write generic user oriented messages.
 *
 * @param _log_inst   Pointer to the log structure associated with the instance.
 * @param _data       Pointer to the data to be logged.
 * @param _length     Length of data (in bytes).
 * @param _str        Persistent, raw string.
 */
#define LOG_INST_HEXDUMP_INF(_log_inst, _data, _length, _str) \
	Z_LOG_HEXDUMP_INSTANCE(LOG_LEVEL_INF, _log_inst, _data, _length, _str)

/**
 * @brief Writes a DEBUG level hexdump message associated with the instance to
 *        the log.
 *
 * @details It's meant to write developer oriented information.
 *
 * @param _log_inst   Pointer to the log structure associated with the instance.
 * @param _data       Pointer to the data to be logged.
 * @param _length     Length of data (in bytes).
 * @param _str        Persistent, raw string.
 */
#define LOG_INST_HEXDUMP_DBG(_log_inst, _data, _length, _str)	\
	Z_LOG_HEXDUMP_INSTANCE(LOG_LEVEL_DBG, _log_inst, _data, _length, _str)

/**
 * @brief Writes an formatted string to the log.
 *
 * @details Conditionally compiled (see CONFIG_LOG_PRINTK). Function provides
 * printk functionality.
 *
 * It is less efficient compared to standard logging because static packaging
 * cannot be used.
 *
 * @param fmt Formatted string to output.
 * @param ap  Variable parameters.
 */
void z_log_vprintk(const char *fmt, va_list ap);

#ifdef __cplusplus
}
#define LOG_IN_CPLUSPLUS 1
#endif

#define LOG_MODULE_REGISTER(...)
#define LOG_MODULE_DECLARE(...)

/**
 * @}
 */

#endif /* ZEPHYR_INCLUDE_LOGGING_LOG_H_ */
