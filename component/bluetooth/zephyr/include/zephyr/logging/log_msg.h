/*
 * Copyright (c) 2021 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_LOGGING_LOG_MSG_H_
#define ZEPHYR_INCLUDE_LOGGING_LOG_MSG_H_

#include <zephyr/logging/log_instance.h>
#include <zephyr/sys/mpsc_packet.h>
#include <zephyr/sys/cbprintf.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/util.h>
#include <string.h>
#include <zephyr/toolchain.h>

#ifdef __GNUC__
#ifndef alloca
#define alloca __builtin_alloca
#endif
#else
#include <alloca.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_MSG_DEBUG 0
#define LOG_MSG_DBG(...) IF_ENABLED(LOG_MSG_DEBUG, (printk(__VA_ARGS__)))

#ifdef CONFIG_LOG_TIMESTAMP_64BIT
typedef uint64_t log_timestamp_t;
#else
typedef uint32_t log_timestamp_t;
#endif

/**
 * @brief Log message API
 * @defgroup log_msg Log message API
 * @ingroup logger
 * @{
 */

#define Z_LOG_MSG_LOG 0

#define Z_LOG_MSG_PACKAGE_BITS 11

#define Z_LOG_MSG_MAX_PACKAGE BIT_MASK(Z_LOG_MSG_PACKAGE_BITS)

#define LOG_MSG_GENERIC_HDR \
	MPSC_PBUF_HDR;\
	uint32_t type:1

struct log_msg_desc {
	LOG_MSG_GENERIC_HDR;
	uint32_t domain:3;
	uint32_t level:3;
	uint32_t package_len:Z_LOG_MSG_PACKAGE_BITS;
	uint32_t data_len:12;
};

union log_msg_source {
	const struct log_source_const_data *fixed;
	struct log_source_dynamic_data *dynamic;
	void *raw;
};

struct log_msg_hdr {
	struct log_msg_desc desc;
/* Attempting to keep best alignment. When address is 64 bit and timestamp 32
 * swap the order to have 16 byte header instead of 24 byte.
 */
#if (INTPTR_MAX > INT32_MAX) && !CONFIG_LOG_TIMESTAMP_64BIT
	log_timestamp_t timestamp;
	const void *source;
#else
	const void *source;
	log_timestamp_t timestamp;
#endif
};

/* Messages are aligned to alignment required by cbprintf package. */
#define Z_LOG_MSG_ALIGNMENT CBPRINTF_PACKAGE_ALIGNMENT

#define Z_LOG_MSG_PADDING \
	((sizeof(struct log_msg_hdr) % Z_LOG_MSG_ALIGNMENT) > 0 ? \
	(Z_LOG_MSG_ALIGNMENT - (sizeof(struct log_msg_hdr) % Z_LOG_MSG_ALIGNMENT)) : \
		0)

struct log_msg {
	struct log_msg_hdr hdr;
	/* Adding padding to ensure that cbprintf package that follows is
	 * properly aligned.
	 */
	uint8_t padding[Z_LOG_MSG_PADDING];
	uint8_t data[];
};

/**
 * @cond INTERNAL_HIDDEN
 */
BUILD_ASSERT(sizeof(struct log_msg) % Z_LOG_MSG_ALIGNMENT == 0,
	     "Log msg size must aligned");
/**
 * @endcond
 */


struct log_msg_generic_hdr {
	LOG_MSG_GENERIC_HDR;
};

union log_msg_generic {
	union mpsc_pbuf_generic buf;
	struct log_msg_generic_hdr generic;
	struct log_msg log;
};

/** @brief Method used for creating a log message.
 *
 * It is used for testing purposes to validate that expected mode was used.
 */
enum z_log_msg_mode {
	/* Runtime mode is least efficient but supports all cases thus it is
	 * treated as a fallback method when others cannot be used.
	 */
	Z_LOG_MSG_MODE_RUNTIME,
	/* Mode creates statically a string package on stack and calls a
	 * function for creating a message. It takes code size than
	 * Z_LOG_MSG_MODE_ZERO_COPY but is a bit slower.
	 */
	Z_LOG_MSG_MODE_FROM_STACK,

	/* Mode calculates size of the message and allocates it and writes
	 * directly to the message space. It is the fastest method but requires
	 * more code size.
	 */
	Z_LOG_MSG_MODE_ZERO_COPY,
};

#define Z_LOG_MSG_DESC_INITIALIZER(_domain_id, _level, _plen, _dlen) \
{ \
	.valid = 0, \
	.busy = 0, \
	.type = Z_LOG_MSG_LOG, \
	.domain = _domain_id, \
	.level = _level, \
	.package_len = _plen, \
	.data_len = _dlen, \
}

#define Z_LOG_MSG_CBPRINTF_FLAGS(_cstr_cnt) \
	(CBPRINTF_PACKAGE_FIRST_RO_STR_CNT(_cstr_cnt) | \
	(IS_ENABLED(CONFIG_LOG_MSG_APPEND_RO_STRING_LOC) ? \
	 CBPRINTF_PACKAGE_ADD_STRING_IDXS : 0))

#ifdef CONFIG_LOG_USE_VLA
#define Z_LOG_MSG_ON_STACK_ALLOC(ptr, len) \
	long long _ll_buf[ceiling_fraction(len, sizeof(long long))]; \
	long double _ld_buf[ceiling_fraction(len, sizeof(long double))]; \
	ptr = (sizeof(long double) == Z_LOG_MSG_ALIGNMENT) ? \
			(struct log_msg *)_ld_buf : (struct log_msg *)_ll_buf; \
	if (IS_ENABLED(CONFIG_LOG_TEST_CLEAR_MESSAGE_SPACE)) { \
		/* During test fill with 0's to simplify message comparison */ \
		memset(ptr, 0, len); \
	}
#else /* Z_LOG_MSG_USE_VLA */
/* When VLA cannot be used we need to trick compiler a bit and create multiple
 * fixed size arrays and take the smallest one that will fit the message.
 * Compiler will remove unused arrays and stack usage will be kept similar
 * to vla case, rounded to the size of the used buffer.
 */
#define Z_LOG_MSG_ON_STACK_ALLOC(ptr, len) \
	long long _ll_buf32[32 / sizeof(long long)]; \
	long long _ll_buf48[48 / sizeof(long long)]; \
	long long _ll_buf64[64 / sizeof(long long)]; \
	long long _ll_buf128[128 / sizeof(long long)]; \
	long long _ll_buf256[256 / sizeof(long long)]; \
	long double _ld_buf32[32 / sizeof(long double)]; \
	long double _ld_buf48[48 / sizeof(long double)]; \
	long double _ld_buf64[64 / sizeof(long double)]; \
	long double _ld_buf128[128 / sizeof(long double)]; \
	long double _ld_buf256[256 / sizeof(long double)]; \
	if (sizeof(long double) == Z_LOG_MSG_ALIGNMENT) { \
		ptr = (len > 128) ? (struct log_msg *)_ld_buf256 : \
			((len > 64) ? (struct log_msg *)_ld_buf128 : \
			((len > 48) ? (struct log_msg *)_ld_buf64 : \
			((len > 32) ? (struct log_msg *)_ld_buf48 : \
				      (struct log_msg *)_ld_buf32)));\
	} else { \
		ptr = (len > 128) ? (struct log_msg *)_ll_buf256 : \
			((len > 64) ? (struct log_msg *)_ll_buf128 : \
			((len > 48) ? (struct log_msg *)_ll_buf64 : \
			((len > 32) ? (struct log_msg *)_ll_buf48 : \
				      (struct log_msg *)_ll_buf32)));\
	} \
	if (IS_ENABLED(CONFIG_LOG_TEST_CLEAR_MESSAGE_SPACE)) { \
		/* During test fill with 0's to simplify message comparison */ \
		memset(ptr, 0, len); \
	}
#endif /* Z_LOG_MSG_USE_VLA */

#define Z_LOG_MSG_ALIGN_OFFSET \
	offsetof(struct log_msg, data)

#define Z_LOG_MSG_LEN(pkg_len, data_len) \
	(offsetof(struct log_msg, data) + pkg_len + (data_len))

#define Z_LOG_MSG_ALIGNED_WLEN(pkg_len, data_len) \
	ceiling_fraction(ROUND_UP(Z_LOG_MSG_LEN(pkg_len, data_len), \
				  Z_LOG_MSG_ALIGNMENT), \
			 sizeof(uint32_t))

/*
 * With Zephyr SDK 0.14.2, aarch64-zephyr-elf-gcc (10.3.0) fails to ensure $sp
 * is below the active memory during message construction. As a result,
 * interrupts happening in the middle of that process can end up smashing active
 * data and causing a logging fault. Work around this by inserting a compiler
 * barrier after the allocation and before any use to make sure GCC moves the
 * stack pointer soon enough
 */

#define Z_LOG_ARM64_VLA_PROTECT() compiler_barrier()

#define Z_LOG_MSG_STACK_CREATE(_cstr_cnt, _domain_id, _source, _level, _data, _dlen, ...) \
do { \
	int _plen; \
	uint32_t flags = Z_LOG_MSG_CBPRINTF_FLAGS(_cstr_cnt) | \
			 CBPRINTF_PACKAGE_ADD_RW_STR_POS; \
	if (GET_ARG_N(1, __VA_ARGS__) == NULL) { \
		_plen = 0; \
	} else { \
		CBPRINTF_STATIC_PACKAGE(NULL, 0, _plen, Z_LOG_MSG_ALIGN_OFFSET, flags, \
					__VA_ARGS__); \
	} \
	struct log_msg *_msg; \
	Z_LOG_MSG_ON_STACK_ALLOC(_msg, Z_LOG_MSG_LEN(_plen, 0)); \
	Z_LOG_ARM64_VLA_PROTECT(); \
	if (_plen != 0) { \
		CBPRINTF_STATIC_PACKAGE(_msg->data, _plen, \
					_plen, Z_LOG_MSG_ALIGN_OFFSET, flags, \
					__VA_ARGS__);\
	} \
	struct log_msg_desc _desc = \
		Z_LOG_MSG_DESC_INITIALIZER(_domain_id, _level, \
					   (uint32_t)_plen, _dlen); \
	LOG_MSG_DBG("creating message on stack: package len: %d, data len: %d\n", \
			_plen, (int)(_dlen)); \
	z_log_msg_static_create((void *)_source, _desc, _msg->data, _data); \
} while (false)

#ifdef CONFIG_LOG_SPEED
#define Z_LOG_MSG_SIMPLE_CREATE(_cstr_cnt, _domain_id, _source, _level, ...) do { \
	int _plen; \
	CBPRINTF_STATIC_PACKAGE(NULL, 0, _plen, Z_LOG_MSG_ALIGN_OFFSET, \
				Z_LOG_MSG_CBPRINTF_FLAGS(_cstr_cnt), \
				__VA_ARGS__); \
	size_t _msg_wlen = Z_LOG_MSG_ALIGNED_WLEN(_plen, 0); \
	struct log_msg *_msg = z_log_msg_alloc(_msg_wlen); \
	struct log_msg_desc _desc = \
		Z_LOG_MSG_DESC_INITIALIZER(_domain_id, _level, (uint32_t)_plen, 0); \
	LOG_MSG_DBG("creating message zero copy: package len: %d, msg: %p\n", \
			_plen, _msg); \
	if (_msg) { \
		CBPRINTF_STATIC_PACKAGE(_msg->data, _plen, _plen, \
					Z_LOG_MSG_ALIGN_OFFSET, \
					Z_LOG_MSG_CBPRINTF_FLAGS(_cstr_cnt), \
					__VA_ARGS__); \
	} \
	z_log_msg_finalize(_msg, (void *)_source, _desc, NULL); \
} while (false)
#else
/* Alternative empty macro created to speed up compilation when LOG_SPEED is
 * disabled (default).
 */
#define Z_LOG_MSG_SIMPLE_CREATE(...)
#endif

/* Macro handles case when local variable with log message string is created. It
 * replaces original string literal with that variable.
 */
#define Z_LOG_FMT_ARGS_2(_name, ...) \
	COND_CODE_1(CONFIG_LOG_FMT_SECTION, \
		(COND_CODE_0(NUM_VA_ARGS_LESS_1(__VA_ARGS__), \
		   (_name), (_name, GET_ARGS_LESS_N(1, __VA_ARGS__)))), \
		(__VA_ARGS__))

/** @brief Wrapper for log message string with arguments.
 *
 * Wrapper is replacing first argument with a variable from a dedicated memory
 * section if option is enabled. Macro handles the case when there is no
 * log message provided.
 *
 * @param _name Name of the variable with log message string. It is optionally used.
 * @param ... Optional log message with arguments (may be empty).
 */
#define Z_LOG_FMT_ARGS(_name, ...) \
	COND_CODE_0(NUM_VA_ARGS_LESS_1(_, ##__VA_ARGS__), \
		(NULL), \
		(Z_LOG_FMT_ARGS_2(_name, ##__VA_ARGS__)))

#if defined(CONFIG_LOG_USE_TAGGED_ARGUMENTS)

#define Z_LOG_FMT_TAGGED_ARGS_2(_name, ...) \
	COND_CODE_1(CONFIG_LOG_FMT_SECTION, \
		    (_name, Z_CBPRINTF_TAGGED_ARGS(NUM_VA_ARGS_LESS_1(__VA_ARGS__), \
						   GET_ARGS_LESS_N(1, __VA_ARGS__))), \
		    (GET_ARG_N(1, __VA_ARGS__), \
		     Z_CBPRINTF_TAGGED_ARGS(NUM_VA_ARGS_LESS_1(__VA_ARGS__), \
					    GET_ARGS_LESS_N(1, __VA_ARGS__))))

/** @brief Wrapper for log message string with tagged arguments.
 *
 * Wrapper is replacing first argument with a variable from a dedicated memory
 * section if option is enabled. Macro handles the case when there is no
 * log message provided. Each subsequent arguments are tagged by preceding
 * each argument with its type value.
 *
 * @param _name Name of the variable with log message string. It is optionally used.
 * @param ... Optional log message with arguments (may be empty).
 */
#define Z_LOG_FMT_TAGGED_ARGS(_name, ...) \
	COND_CODE_0(NUM_VA_ARGS_LESS_1(_, ##__VA_ARGS__), \
		(Z_CBPRINTF_TAGGED_ARGS(0)), \
		(Z_LOG_FMT_TAGGED_ARGS_2(_name, ##__VA_ARGS__)))

#define Z_LOG_FMT_RUNTIME_ARGS(...) \
	Z_LOG_FMT_TAGGED_ARGS(__VA_ARGS__)

#else

#define Z_LOG_FMT_RUNTIME_ARGS(...) \
	Z_LOG_FMT_ARGS(__VA_ARGS__)

#endif /* CONFIG_LOG_USE_TAGGED_ARGUMENTS */

/* Macro handles case when there is no string provided, in that case variable
 * is not created.
 */
#define Z_LOG_MSG_STR_VAR_IN_SECTION(_name, ...) \
	COND_CODE_0(NUM_VA_ARGS_LESS_1(_, ##__VA_ARGS__), \
		    (/* No args provided, no variable */), \
		    (static const char _name[] \
			__attribute__((__section__(".log_strings"))) = \
			GET_ARG_N(1, __VA_ARGS__);))

/** @brief Create variable in the dedicated memory section (if enabled).
 *
 * Variable is initialized with a format string from the log message.
 *
 * @param _name Variable name.
 * @param ... Optional log message with arguments (may be empty).
 */
#define Z_LOG_MSG_STR_VAR(_name, ...) \
	IF_ENABLED(CONFIG_LOG_FMT_SECTION, \
		   (Z_LOG_MSG_STR_VAR_IN_SECTION(_name, ##__VA_ARGS__)))

/**
 * @}
 */

#include <syscalls/log_msg.h>

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_LOGGING_LOG_MSG_H_ */
