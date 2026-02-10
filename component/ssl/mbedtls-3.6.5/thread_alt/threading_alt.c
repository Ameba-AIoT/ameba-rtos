#include "common.h"

#if defined(MBEDTLS_THREADING_C) && defined(MBEDTLS_THREADING_ALT)

#include "ameba.h"
#include "os_wrapper_mutex.h"
#include "mbedtls/threading.h"

static void threading_mutex_init_alt(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL) {
        return;
    }

    if (rtos_mutex_create((rtos_mutex_t *)&mutex->mutex) == RTK_SUCCESS) {
        mutex->is_valid = 1;
    } else {
        mutex->is_valid = 0;
    }
}

static void threading_mutex_free_alt(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->is_valid == 0) {
        return;
    }

    rtos_mutex_delete((rtos_mutex_t)mutex->mutex);
    mutex->is_valid = 0;
}

static int threading_mutex_lock_alt(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->is_valid == 0) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (rtos_mutex_take((rtos_mutex_t)mutex->mutex, MUTEX_WAIT_TIMEOUT) != RTK_SUCCESS) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    return 0;
}

static int threading_mutex_unlock_alt(mbedtls_threading_mutex_t *mutex)
{
    if (mutex == NULL || mutex->is_valid == 0) {
        return MBEDTLS_ERR_THREADING_BAD_INPUT_DATA;
    }

    if (rtos_mutex_give((rtos_mutex_t)mutex->mutex) != RTK_SUCCESS) {
        return MBEDTLS_ERR_THREADING_MUTEX_ERROR;
    }

    return 0;
}

void mbedtls_threading_init(void)
{
    mbedtls_threading_set_alt(threading_mutex_init_alt,
                              threading_mutex_free_alt,
                              threading_mutex_lock_alt,
                              threading_mutex_unlock_alt);
}

void mbedtls_threading_free(void)
{
    mbedtls_threading_free_alt();
}

#endif /* MBEDTLS_THREADING_C && MBEDTLS_THREADING_ALT */
