/*
 * Copyright (c) 2018-2022, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_EXAMPLE_H__
#define __PLATFORM_EXAMPLE_H__

#include "tfm_example_api.h"

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * \brief Initializes the secure partition.
 *
 * \return Returns values as specified by the \ref psa_status_t
 */
psa_status_t tfm_example_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __PLATFORM_EXAMPLE_H__ */
