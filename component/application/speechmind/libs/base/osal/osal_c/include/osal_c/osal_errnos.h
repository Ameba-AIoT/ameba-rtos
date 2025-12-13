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

#ifndef AMEBA_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_ERRNOS_H
#define AMEBA_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_ERRNOS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// The type to return error codes
typedef int32_t    status_t;

/*
 * Error codes. 
 * All error codes are negative values.
 */
enum {
    OSAL_OK                      = 0,

    // Common errors
    OSAL_ERR_NO_MEMORY           = -12,   // -ENOMEM,
    OSAL_ERR_INVALID_OPERATION   = -88,   // -ENOSYS,
    OSAL_ERR_INVALID_PARAM       = -22,   // -EINVAL,
    OSAL_ERR_NOT_FOUND           = -2,    // -ENOENT,
    OSAL_ERR_PERMISSION_DENIED   = -1,    // -EPERM,
    OSAL_ERR_NO_INIT             = -19,   // -ENODEV,
    OSAL_ERR_ALREADY_EXISTS      = -17,   // -EEXIST,
    OSAL_ERR_DEAD_OBJECT         = -32,   // -EPIPE,
    OSAL_ERR_BAD_INDEX           = -139,  // -EOVERFLOW,
    OSAL_ERR_NOT_ENOUGH_DATA     = -61,   // -ENODATA,
    OSAL_ERR_WOULD_BLOCK         = -11,   // -EWOULDBLOCK,
    OSAL_ERR_TIMED_OUT           = -116,  // -ETIMEDOUT,
    OSAL_ERR_UNKNOWN_TRANSACTION = -77,   // -EBADMSG,
    OSAL_ERR_CANCELED            = -125,  // -ECANCELED

    OSAL_ERR_UNKNOWN_ERROR       = (-2147483647 - 1), // INT32_MIN value

    OSAL_ERR_OPERATION_FAIL      = (OSAL_ERR_UNKNOWN_ERROR + 1),
    OSAL_ERR_NOT_SUPPORT         = (OSAL_ERR_UNKNOWN_ERROR + 2),
};

#define OSAL_FAILED(status)      ((status) != OSAL_OK)
#define OSAL_SUCCESS(status)     ((status) == OSAL_OK)

#ifdef __cplusplus
}
#endif

#endif // AMEBA_OSAL_OSAL_C_INCLUDE_OSAL_OSAL_ERRNOS_H
/** @} */
