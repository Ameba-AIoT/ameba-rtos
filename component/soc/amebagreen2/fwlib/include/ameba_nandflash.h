/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_NANDFLASH_H
#define _AMEBA_NANDFLASH_H

#ifdef __cplusplus
extern "C" {
#endif

u32 Nand_Get_NandAddr(u32 RegionStart, u32 HostAddr);
u32 Nand_L2P_Table(u32 HostAddr);

#ifdef __cplusplus
}
#endif

#endif  //_AMEBA_FLASH_CLK_H