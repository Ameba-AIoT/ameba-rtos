/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_TFM_TARGET_H__
#define __CONFIG_TFM_TARGET_H__

/* Size of secure partition */
#undef FLASH_S_PARTITION_SIZE
#define FLASH_S_PARTITION_SIZE   0x80000

/* Size of non-secure partition */
#undef FLASH_NS_PARTITION_SIZE
#define FLASH_NS_PARTITION_SIZE  0x300000

#endif /* __CONFIG_TFM_TARGET_H__ */
