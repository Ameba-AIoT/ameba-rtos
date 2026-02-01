/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#define PLAT_PHY_ADDR_SPACE_SIZE	(1ull << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ull << 32)
#define MAX_MMAP_REGIONS		16
#define MAX_XLAT_TABLES			7 /* align with 2M will save xlat table, actually used is 4 */
#define MAX_IO_DEVICES			3
#define MAX_IO_HANDLES			4

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_DEF_H */
