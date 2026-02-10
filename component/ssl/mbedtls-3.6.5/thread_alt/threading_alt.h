#ifndef MBEDTLS_THREADING_ALT_H
#define MBEDTLS_THREADING_ALT_H

#include "mbedtls/private_access.h"

#include "common.h"

#include <stddef.h>
#include <stdint.h>

#include "os_wrapper_mutex.h"

#if defined(MBEDTLS_THREADING_ALT)
typedef struct mbedtls_threading_mutex_t {
    rtos_mutex_t MBEDTLS_PRIVATE(mutex);  /**< FreeRTOS mutex handle */
    int MBEDTLS_PRIVATE(is_valid);        /**< Flag indicating if mutex is initialized */
} mbedtls_threading_mutex_t;


void mbedtls_threading_init(void);

void mbedtls_threading_free(void);

#endif /* MBEDTLS_THREADING_ALT */

#endif /* MBEDTLS_THREADING_ALT_H */
