/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __TFM_EXAMPLE_API__
#define __TFM_EXAMPLE_API__

#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include "tfm_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief TFM secure partition platform API version
 */
#define TFM_EXAMPLE_API_VERSION_MAJOR (0)
#define TFM_EXAMPLE_API_VERSION_MINOR (3)

#define TFM_EXAMPLE_API_ID_IOCTL         (1010)

/*!
 * \enum tfm_example_err_t
 *
 * \brief Platform service error types
 *
 */
enum tfm_example_err_t {
    TFM_EXAMPLE_ERR_SUCCESS = 0,
    TFM_EXAMPLE_ERR_SYSTEM_ERROR,
    TFM_EXAMPLE_ERR_INVALID_PARAM,
    TFM_EXAMPLE_ERR_NOT_SUPPORTED,

    /* Following entry is only to ensure the error code of int size */
    TFM_EXAMPLE_ERR_FORCE_INT_SIZE = INT_MAX
};

typedef int32_t tfm_example_ioctl_req_t;

/*!
 * \brief Performs a platform-specific service
 *
 * \param[in]  request      Request identifier (valid values vary
 *                          based on the platform)
 * \param[in]  input        Input buffer to the requested service (or NULL)
 * \param[in,out] output    Output buffer to the requested service (or NULL)
 *
 * \return Returns values as specified by the \ref tfm_example_err_t
 */
enum tfm_example_err_t tfm_example_ioctl(tfm_example_ioctl_req_t request,
                                           psa_invec *input,
                                           psa_outvec *output);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_EXAMPLE_API__ */
