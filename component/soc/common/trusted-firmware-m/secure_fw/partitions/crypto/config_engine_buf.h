/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __CONFIG_ENGINE_BUF_H__
#define __CONFIG_ENGINE_BUF_H__

/*
 * CRYPTO_ENGINE_BUF_SIZE is decided by multiple components configs.
 * CRYPTO_ENGINE_BUF_SIZE can be overridden below by other component configs.
 * It must be guaranteed that this header file is included at last before CRYPTO_ENGINE_BUF_SIZE
 * is referred in crypto_library.c.
 * Otherwise, crypto_library.c may include an incorrect definition value.
 */

/* The CC312 needs at least 0x4000 of heap size to work properly */
#if defined(CRYPTO_HW_ACCELERATOR_CC312) && (CRYPTO_ENGINE_BUF_SIZE < 0x4000)
#pragma message("CRYPTO_ENGINE_BUF_SIZE is redefined to 0x4000.")
#undef  CRYPTO_ENGINE_BUF_SIZE
#define CRYPTO_ENGINE_BUF_SIZE                 0x4000
#endif

#endif /* __CONFIG_ENGINE_BUF_H__ */
