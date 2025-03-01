/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_nandflash.h"

static const char *const TAG = "NANDFLASH";
u32 nand_l2p_table[2]; /*init to zero in __image1__*/

/*block_id shall less than 32, bacause the max region is 0x08300000 */
u32 NAND_CHECK_IS_BAD_BLOCK(u32 block_id)
{
	u8 oob[4];

	u32 l2p_table_mark = NAND_L2P_TABLE_GET(block_id);
	if (l2p_table_mark == NAND_L2P_TABLE_MARK_BAD) {
		return TRUE;
	} else if (l2p_table_mark == NAND_L2P_TABLE_MARK_GOOD) {
		return FALSE;
	}

	/*bad block: the first byte of spare area of the fisrt page of block is not 0xFF*/
	NAND_Page_Read(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 4, oob);
	if ((oob[0] != 0xFF) || (oob[1] != 0xFF)) {
		NAND_L2P_TABLE_SET_BAD(block_id);
		RTK_LOGE(TAG, "Blk 0x%lx is BB!\n", block_id);
		return TRUE;
	}

	NAND_L2P_TABLE_SET_GOOD(block_id);
	return FALSE;
}

u32 Nand_Get_NandAddr(u32 RegionStart, u32 HostAddr)
{
	u32 PhyAddr = HostAddr - SPI_FLASH_BASE;
	u32 RegionStartBlock = NAND_PAGE_ADDR_TO_BLOCK_ID(NAND_ADDR_TO_PAGE_ADDR(RegionStart - SPI_FLASH_BASE));
	u32 PhyBlock = NAND_PAGE_ADDR_TO_BLOCK_ID(NAND_ADDR_TO_PAGE_ADDR(PhyAddr));

	for (u32 BlockId = RegionStartBlock; BlockId < PhyBlock; BlockId++) {
		if (NAND_CHECK_IS_BAD_BLOCK(BlockId)) {
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

void NandImgCopy(u8 *pData, u8 *HostAddr, size_t DataLen)
{
	u32 PageAddr, ByteAddr, ByteLen, BlockId;
	u32 NandAddr = Nand_L2P_Table((u32)HostAddr);

	u32 page_begin = NAND_ADDR_TO_PAGE_ADDR(NandAddr);
	u32 page_end = NAND_ADDR_TO_PAGE_ADDR(NandAddr + DataLen - 1);
	u32 page_cnt = page_end - page_begin + 1;
	u32 next_chk_blockid = 0;
	u32 pData_Offset = 0;

	while (page_cnt) {
		/*Maybe lookup the L2P table to find the mapping between logic addr and physical addr. */
		PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr);
		BlockId = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);
		if (BlockId >= next_chk_blockid) {
			while (NAND_CHECK_IS_BAD_BLOCK(BlockId)) {
				/*jump to next block when current block is bad block*/
				BlockId = BlockId + 1;
				PageAddr += NAND_BLOCK_PAGE_CNT;
				NandAddr += NAND_BLOCK_PAGE_CNT * NAND_PAGE_SIZE_MAIN;
			}

			next_chk_blockid = BlockId + 1; /* next block we should check */
		}

		ByteAddr = NAND_ADDR_TO_BYTE_ADDR(NandAddr);
		ByteLen = (page_cnt == 1) ? DataLen : (NAND_PAGE_SIZE_MAIN - ByteAddr);

		/*Read Data from NandAddr to NandAddr+DataLen, and put the return data into pData[pData_Offset].*/
		u8 status = NAND_Page_Read(PageAddr, ByteAddr, ByteLen, &pData[pData_Offset]);
		if (0 != (status & NAND_STATUS_ECC_MASK)) {
			RTK_LOGI(TAG, "Read Page 0x%lx of Block 0x%x May Fail, status 0x%x!\n",
					 PageAddr & NAND_BLOCK_PAGE_MASK, NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr), status);
		}

		pData_Offset = pData_Offset + ByteLen;
		NandAddr = NandAddr + ByteLen;
		DataLen = DataLen - ByteLen;
		page_cnt--;
	}
}
