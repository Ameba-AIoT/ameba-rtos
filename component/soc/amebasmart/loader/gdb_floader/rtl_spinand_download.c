#include "ameba.h"

u32 g_erase_blockid;

BOOT_RAM_TEXT_SECTION
void RtlSpiNandInit(void)
{
	g_erase_blockid = 0;
	DBG_8195A("NandFlash\n");
	NAND_StructInit(&flash_init_para);
	NAND_Init(SpicOneBitMode);
	NAND_SetStatus(NAND_REG_BLOCK_LOCK, NAND_BL_ALL_UNLOCKED);
}

BOOT_RAM_TEXT_SECTION
u32 NAND_CHECK_IS_BAD_BLOCK(u32 block_id)
{
	u8 oob[4];

	/*bad block: the first byte of spare area of the fisrt page of block is not 0xFF*/
	NAND_Page_Read(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 4, oob);
	if (oob[0] != 0xFF) { /*((oob[0] != 0xFF) || (oob[1] != 0xFF))*/
		DBG_8195A("Block 0x%x is BB!\n", block_id);
		return TRUE;
	}
	return FALSE;
}

BOOT_RAM_TEXT_SECTION
void NAND_MARK_BAD_BLOCK(u32 block_id)
{
	u8 oob[4];
	oob[0] = 0;

	DBG_8195A("Block 0x%x Mark BB!\n", block_id);
	if (NAND_Page_Write(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 1, oob)) {
		DBG_8195A("Block 0x%x Mark BB Fail!\n", block_id);
	}
}

BOOT_RAM_TEXT_SECTION
u32 RtlSpiNandProgram(u32 StartAddr, u32 DataLen, u8 *pData)
{
	u32 PageAddr, ByteAddr, ByteLen, BlockId;
	u32 page_begin = NAND_ADDR_TO_PAGE_ADDR(StartAddr);
	u32 page_end = NAND_ADDR_TO_PAGE_ADDR(StartAddr + DataLen - 1);
	u32 page_cnt = page_end - page_begin + 1;

	while (page_cnt) {
		PageAddr = NAND_ADDR_TO_PAGE_ADDR(StartAddr);
		BlockId = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);

		if (BlockId >= g_erase_blockid) {
			if (NAND_CHECK_IS_BAD_BLOCK(BlockId)) {
				return FALSE;
			}

			/* Erase Flash block failed, marked as bad*/
			if (NAND_Erase(NAND_BLOCK_ID_TO_PAGE_ADDR(BlockId))) {
				DBG_8195A("Block 0x%x Erase Fail!\n", BlockId);

				NAND_MARK_BAD_BLOCK(BlockId);
				return FALSE;
			}

			g_erase_blockid = BlockId + 1; /* next block we should erase */
		}

		ByteAddr = NAND_ADDR_TO_BYTE_ADDR(StartAddr);
		ByteLen = (page_cnt == 1) ? DataLen : (NAND_PAGE_SIZE_MAIN - ByteAddr);

		if (NAND_Page_Write(PageAddr, ByteAddr, ByteLen, pData)) {
			DBG_8195A("Block 0x%x Page 0x%x Program Fail!\n", BlockId, PageAddr & NAND_BLOCK_PAGE_MASK);

			NAND_MARK_BAD_BLOCK(BlockId);
			return FALSE;
		}

		StartAddr = StartAddr + ByteLen;
		DataLen = DataLen - ByteLen;
		pData += ByteLen;
		page_cnt--;
	}

	return TRUE;
}
