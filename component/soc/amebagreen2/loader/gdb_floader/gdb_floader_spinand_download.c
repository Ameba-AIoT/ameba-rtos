#include "ameba.h"

static u32 good_block_id;
static u32 StartAddr_amend = 0;

void RtlSpiNandInit(void)
{
	good_block_id = 0;
	StartAddr_amend = 0;
	u32 page_size_byte = 0;

	/* init Nand flash */
	if (!SYSCFG_OTP_BigPageNand()) {
		/* If efuse is not burned, the page size will be automatically detected.
			This page size detection method is applicable to most nand flash.
			If it cannot be configured normally, efuse must be burned.
		*/
		if (RTK_SUCCESS == NAND_GetPageSize(&page_size_byte)) {
			if (page_size_byte == 0x1000) {
				flash_init_para.FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_4K_BIT_EXP;
				SPIC_Config(SPIC, &flash_init_para);
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "NAND Page Size Set to 4KB\n");
			}
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "NAND_GetPageSize fail, keep 2KB page size\n");
		}
	}
}

u32 NAND_CHECK_IS_BAD_BLOCK(u32 block_id)
{
	u8 oob[4];

	/*bad block: the first byte of spare area of the fisrt page of block is not 0xFF*/
	NAND_Page_Read(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 4, oob);
	if (oob[0] != 0xFF) { /*((oob[0] != 0xFF) || (oob[1] != 0xFF))*/
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Block 0x%x is BB!\n", block_id);
		return TRUE;
	}
	return FALSE;
}


void NAND_MARK_BAD_BLOCK(u32 block_id)
{
	u8 oob[4];
	oob[0] = 0;

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Block 0x%x Mark BB!\n", block_id);
	if (NAND_Page_Write(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 1, oob)) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Block 0x%x Mark BB Fail!\n", block_id);
	}
}

u32 RtlSpiNandProgram(u32 StartAddr, u32 DataLen, u8 *pData, u8 *FlashDataBuf_check)
{
	u32 PageAddr, ByteAddr, ByteLen, BlockId;

	if (StartAddr_amend != 0) {
		StartAddr = StartAddr_amend;
	}

	u32 page_begin = NAND_ADDR_TO_PAGE_ADDR(StartAddr);
	u32 page_end = NAND_ADDR_TO_PAGE_ADDR(StartAddr + DataLen - 1);
	u32 page_cnt = page_end - page_begin + 1;

	while (page_cnt) {
		PageAddr = NAND_ADDR_TO_PAGE_ADDR(StartAddr);
		BlockId = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);

		if (BlockId >= good_block_id) {
			while (NAND_CheckBadBlock(BlockId)) {
				/*jump to next block when current block is bad block*/
				BlockId = BlockId + 1;
				PageAddr += NAND_BLOCK_PAGE_CNT;
				StartAddr += NAND_BLOCK_PAGE_CNT * NAND_PAGE_SIZE_MAIN;
				StartAddr_amend = StartAddr;
			}

			/* Erase Flash block failed, marked as bad*/
			while (NAND_Erase(NAND_BLOCK_ID_TO_PAGE_ADDR(BlockId))) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Block 0x%x Erase Fail!\n", BlockId);
				NAND_MARK_BAD_BLOCK(BlockId);
				BlockId = BlockId + 1;
				PageAddr += NAND_BLOCK_PAGE_CNT;
				StartAddr += NAND_BLOCK_PAGE_CNT * NAND_PAGE_SIZE_MAIN;
			}

			good_block_id = BlockId + 1; /* next block we should erase */
		}

		ByteAddr = NAND_ADDR_TO_BYTE_ADDR(StartAddr);
		ByteLen = (page_cnt == 1) ? DataLen : (NAND_PAGE_SIZE_MAIN - ByteAddr);

		while (NAND_Page_Write(PageAddr, ByteAddr, ByteLen, pData)) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Block 0x%x Page 0x%x Program Fail!\n", BlockId, PageAddr & NAND_BLOCK_PAGE_MASK);
			NAND_MARK_BAD_BLOCK(BlockId);
			BlockId = BlockId + 1;
			PageAddr += NAND_BLOCK_PAGE_CNT;
			StartAddr += NAND_BLOCK_PAGE_CNT * NAND_PAGE_SIZE_MAIN;
		}

		NAND_Page_Read(PageAddr, ByteAddr, ByteLen, FlashDataBuf_check);
		if (_memcmp(pData, FlashDataBuf_check, ByteLen)) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Block 0x%x Page 0x%x. pData/Flash Data compare Fail !\n", BlockId, PageAddr & NAND_BLOCK_PAGE_MASK);
		}

		StartAddr = StartAddr + ByteLen;
		DataLen = DataLen - ByteLen;
		pData += ByteLen;
		page_cnt--;
	}

	if (StartAddr_amend != 0) {
		StartAddr_amend = StartAddr;
	}

	return TRUE;
}
