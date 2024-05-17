/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __SOFT_CRC_H__
#define __SOFT_CRC_H__

#include <stddef.h>
#include <stdint.h>

/* Calculate crc32 */
uint32_t crc32(const void *buf, size_t len);

#endif /* __SOFT_CRC_H__ */

