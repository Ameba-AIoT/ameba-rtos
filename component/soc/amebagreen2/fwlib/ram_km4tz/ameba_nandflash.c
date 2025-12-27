/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "NANDFLASH";

u32 Nand_Get_NandAddr(u32 RegionStart, u32 HostAddr)
{
	u32 PhyAddr = HostAddr - SPI_FLASH_BASE;
	u32 RegionStartBlock = NAND_PAGE_ADDR_TO_BLOCK_ID(NAND_ADDR_TO_PAGE_ADDR(RegionStart - SPI_FLASH_BASE));
	u32 PhyBlock = NAND_PAGE_ADDR_TO_BLOCK_ID(NAND_ADDR_TO_PAGE_ADDR(PhyAddr));

	for (u32 BlockId = RegionStartBlock; BlockId < PhyBlock; BlockId++) {
		if (NAND_CheckBadBlock(BlockId)) {
			PhyBlock = PhyBlock + 1;
			PhyAddr += NAND_BLOCK_PAGE_CNT * NAND_PAGE_SIZE_MAIN;
		}
	}
	return PhyAddr;
}

u32 Nand_L2P_Table(u32 HostAddr)
{
	u32 app_phy_start_addr = SPI_FLASH_BASE;
	u32 app_phy_end_addr = SPI_FLASH_BASE;
	u32 nand_addr;

	flash_get_layout_info_by_addr(HostAddr, &app_phy_start_addr, &app_phy_end_addr);

	nand_addr = Nand_Get_NandAddr(app_phy_start_addr, HostAddr);
	if (nand_addr > app_phy_end_addr - SPI_FLASH_BASE) {
		RTK_LOGE(TAG, "Nand Addr 0x%x region Bad Block too much\n", HostAddr);
	}

	return nand_addr;
}
