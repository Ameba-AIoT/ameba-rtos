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

/**
 * @file audio_errnos.h
 *
 * @brief Declares the enumerated values returned by the function
 *
 * @since 1.0
 * @version 1.0
 */
#ifndef AMEBA_AUDIO_INTERFACES_AUDIO_ERRNOS_H
#define AMEBA_AUDIO_INTERFACES_AUDIO_ERRNOS_H

#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
	AUDIO_OK                      = 0,    // Everything's swell.


	AUDIO_ERR_UNKNOWN_ERROR       = (-2147483647 - 1), // INT32_MIN value

	AUDIO_ERR_NO_MEMORY           = -ENOMEM,
	AUDIO_ERR_INVALID_OPERATION   = -ENOSYS,
	AUDIO_ERR_INVALID_PARAM       = -EINVAL,
	AUDIO_ERR_NAMERR_NOT_FOUND    = -ENOENT,
	AUDIO_ERR_PERMISSION_DENIED   = -EPERM,
	AUDIO_ERR_NO_INIT             = -ENODEV,
	AUDIO_ERR_ALREADY_EXISTS      = -EEXIST,
	AUDIO_ERR_DEAD_OBJECT         = -EPIPE,
	AUDIO_ERR_BAD_INDEX           = -EOVERFLOW,
	AUDIO_ERR_NOT_ENOUGH_DATA     = -ENODATA,
	AUDIO_ERR_WOULD_BLOCK         = -EWOULDBLOCK,
	AUDIO_ERR_TIMED_OUT           = -ETIMEDOUT,
	AUDIO_ERR_UNKNOWN_TRANSACTION = -EBADMSG,

	AUDIO_ERR_FAILURE             = (AUDIO_ERR_UNKNOWN_ERROR + 1),
	AUDIO_ERR_NOT_SUPPORT         = (AUDIO_ERR_UNKNOWN_ERROR + 2),
	AUDIO_ERR_NOT_FOUND           = (AUDIO_ERR_UNKNOWN_ERROR + 3),
};

#ifdef __cplusplus
}
#endif

#endif // AMEBA_AUDIO_INTERFACES_AUDIO_ERRNOS_H
/** @} */
