/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef __TFM_ITS_REQ_MNGR_H__
#define __TFM_ITS_REQ_MNGR_H__

#include <stdint.h>
#include "config_tfm.h"
#include "psa/framework_feature.h"

#ifdef __cplusplus
extern "C" {
#endif

#if PSA_FRAMEWORK_HAS_MM_IOVEC == 1
uint8_t *its_req_mngr_get_vec_base(void);
#else
size_t its_req_mngr_read(uint8_t *buf, size_t num_bytes);
void its_req_mngr_write(const uint8_t *buf, size_t num_bytes);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __TFM_ITS_REQ_MNGR_H__ */

