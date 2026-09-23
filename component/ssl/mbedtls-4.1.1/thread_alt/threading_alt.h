#ifndef MBEDTLS_THREADING_ALT_H
#define MBEDTLS_THREADING_ALT_H

#include "mbedtls/build_info.h"

#include <stddef.h>
#include <stdint.h>

#include "os_wrapper_mutex.h"
#include "os_wrapper_semaphore.h"

/* 4.x requires both primitives to be typedef'd by the ALT header. */
typedef rtos_mutex_t mbedtls_platform_mutex_t;

typedef struct mbedtls_platform_condition_variable_t {
	rtos_sema_t sema;
	int waiters;
} mbedtls_platform_condition_variable_t;

void mbedtls_threading_init(void);

void mbedtls_threading_free(void);

#endif /* MBEDTLS_THREADING_ALT_H */
