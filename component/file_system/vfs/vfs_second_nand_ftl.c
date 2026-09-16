/**
  ******************************************************************************
  * @file    vfs_second_nand_ftl.c
  * @author
  * @date    2023-12
  * @brief   FTL functionalities for an external (second) SPI NAND Flash:
  *          geometry discovery via the ONFI parameter page, bad-block marker
  *          handling and page/block level read, program and erase.
  *
  *          This is the device layer under lbm_core.c - it reports bad blocks
  *          and ECC status, LBM does the logical-to-physical mapping.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include "os_wrapper.h"
#include "vfs_nand_common.h"
#include "vfs_second_nand_spi.h"
#include "vfs.h"
#include "lbm.h"

/* Public variables ---------------------------------------------------------*/

u32 vfs_second_nand_pagesize = 0;
u32 vfs_second_nand_pagenum = 0;
u32 vfs_second_nand_capacity = 0;

/* Known-model whitelist, keyed by the three RDID bytes (MID / DID / ExtDID).
 * Purely informational: geometry always comes from the ONFI parameter page
 * (see MfgInit), never from this table -- an unlisted chip runs fine on its
 * detected parameters, we just log that it is not in the map.  Winbond SPI NAND
 * all share MID/DID = EF/AA and differ only in the 3rd (density) byte, so the
 * key must include ExtDID.  Add rows as new parts are qualified. */
typedef struct {
	u8 mid;
	u8 did;
	u8 ext;
	const char *model_name;
} second_nand_model_t;

static const second_nand_model_t second_nand_models[] = {
	{0xEF, 0xAA, 0x21, "W25N01GV"},   /* Winbond 1 Gbit, 3.3V (validated) */
	{0xEF, 0xAA, 0x22, "W25N02KV"},   /* Winbond 2 Gbit, 3.3V */
};

/* Private variables ---------------------------------------------------------*/

static NAND_FTL_DeviceTypeDef NandFlash_Device;

/* The SPI driver keeps a single pair of shared DMA buffers, and a program is a
 * three-step sequence (WREN, load cache, program execute) that must not be
 * interleaved with another command or the WEL latch is lost.  Every exported
 * entry point below therefore runs under this lock. */
static rtos_mutex_t device_lock = NULL;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Check bad block
  * @param  nand : NAND device info
  * @param  addr : page address
  * @param  value : 0 - Good; 1 - Bad
  * @retval HAL_OK : OK; others : FAIL
  * @note   The marker lives in the first two spare bytes of the block's page 0,
  *         i.e. at column PageSize.  Reading it needs buffer read mode, which
  *         SECOND_NAND_FTL_Init() enables (see NAND_CFG_BUF_READ).
  */

static u8 NF_IsBad(NAND_FTL_DeviceTypeDef *nand, u32 addr, u8 *value)
{
	Flash_InfoTypeDef *info = &nand->MemInfo;
	u32 block_addr;
	u8 ret;
	u8 data[2] = {NF_GOOD_BLOCK, NF_GOOD_BLOCK};

	block_addr = NF_GetBlockAddr(nand, addr);

	/* ECC status of the marker read is deliberately ignored: a factory bad block
	 * has no valid ECC, so an uncorrectable report here is expected. */
	ret = NandFlash_Page_Read_ArrayToCache(block_addr);
	if (ret == 0xFFU) {
		return HAL_TIMEOUT;
	}

	if (NandFlash_Page_Read_FromCache(g_nandflash_init_para.FLASH_cur_cmd, info->PageSize, 2, data) != HAL_OK) {
		return HAL_TIMEOUT;
	}

	*value = ((data[0] != NF_GOOD_BLOCK) || (data[1] != NF_GOOD_BLOCK)) ? 1 : 0;

	return HAL_OK;
}

/**
  * @brief  Mark bad block
  * @param  nand : NAND device info
  * @param  addr : page address
  * @retval HAL_OK : OK; others : FAIL
  */

static u8 NF_MarkBad(NAND_FTL_DeviceTypeDef *nand, unsigned int addr)
{
	Flash_InfoTypeDef *info = &nand->MemInfo;
	u32 block_addr;
	u8 ret;
	u8 data[2];

	data[0] = NF_BAD_BLOCK;
	data[1] = NF_BAD_BLOCK;

	block_addr = NF_GetBlockAddr(nand, addr);

	/*WEL bit shall be set, otherwise Nand will ignore the program execute*/
	if (NandFlash_WriteEn() != 0U) {
		VFS_DBG(VFS_ERROR, "Mark block 0x%08X to bad: WEL fail", addr);
		return HAL_TIMEOUT;
	}

	/* NAND_CMD_PP resets the cache to 0xFF, so only the marker bytes are programmed */
	if (NandFlash_Page_Write_Data_Xfer(NAND_CMD_PP, info->PageSize, 2, data) != HAL_OK) {
		return HAL_TIMEOUT;
	}
	ret = NandFlash_Page_Write_Program_Execute(block_addr);

	if (ret == 0U) {
		VFS_DBG(VFS_WARNING, "Mark block 0x%08X to bad", addr);
		ret = HAL_OK;
	} else if (ret == 0xFFU) {
		VFS_DBG(VFS_ERROR, "Mark block 0x%08X to bad: TIMEOUT", addr);
		ret = HAL_TIMEOUT;
	} else {
		VFS_DBG(VFS_ERROR, "Fail to mark bad block 0x%08X: 0x%02X", addr, ret);
		ret = HAL_ERR_HW;
	}

	return ret;
}

/**
  * @brief  Erase block
  * @param  nand : NAND device info
  * @param  addr : page address
  * @retval HAL_OK : OK; others : FAIL
  */

static u8 NF_EraseBlock(NAND_FTL_DeviceTypeDef *nand, unsigned int addr)
{
	u8 ret;

	/*WEL bit shall be set, otherwise Nand will ignore this cmd*/
	if (NandFlash_WriteEn() != 0U) {
		return HAL_TIMEOUT;
	}

	ret = NandFlash_Erase(addr);

	if (ret == 0U) {
		ret = HAL_OK;
	} else if (ret == 0xFFU) {
		ret = HAL_TIMEOUT;
	} else {
		VFS_DBG(VFS_ERROR, "Fail to erase block 0x%08X: 0x%02X", addr, ret);
		ret = NF_MarkBad(nand, addr);
		if (ret == HAL_OK) {
			ret = UERR_NAND_WORN_BLOCK;
		}
	}

	return ret;
}

/**
  * @brief  Read one page, optionally skipping the bad-block marker check
  * @param  addr : page address
  * @param  buf : data buffer, PageSize bytes
  * @param  check_bad : 1 - verify the block is not marked bad first
  * @retval HAL_OK : OK; others : FAIL
  */

static u8 NF_ReadPage(u32 addr, u8 *buf, u8 check_bad)
{
	NAND_FTL_DeviceTypeDef *nand = &NandFlash_Device;
	Flash_InfoTypeDef *info = &nand->MemInfo;
	const NAND_FTL_MfgOpsTypeDef *ops = (const NAND_FTL_MfgOpsTypeDef *)nand->MfgOps;
	u8 status;
	u8 ret;
	u8 is_bad_block;

	if (!nand->Initialized) {
		return UERR_INIT;
	}

	if (check_bad) {
		ret = NF_IsBad(nand, addr, &is_bad_block);
		if (ret != HAL_OK) {
			return ret;
		}
		if (is_bad_block) {
			return UERR_NAND_BAD_BLOCK;
		}
	}

	status = NandFlash_Page_Read_ArrayToCache(addr);
	if (status == 0xFFU) {
		return HAL_TIMEOUT;
	}

	if (NandFlash_Page_Read_FromCache(g_nandflash_init_para.FLASH_cur_cmd, 0, info->PageSize, buf) != HAL_OK) {
		return HAL_TIMEOUT;
	}

	return (status == 0U) ? HAL_OK : ops->GetEccStatus(nand, status);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init the external SPI NAND Flash and its FTL
  * @param  None
  * @retval HAL_OK : OK; others : FAIL
  */

u8 SECOND_NAND_FTL_Init(void)
{
	u8 flash_ID[3];
	u8 Feat_Reg;
	u8 ret;

	NAND_FTL_DeviceTypeDef *nand = &NandFlash_Device;
	Flash_InfoTypeDef *info = &nand->MemInfo;

	if (device_lock == NULL) {
		rtos_mutex_create(&device_lock);
		if (device_lock == NULL) {
			VFS_DBG(VFS_ERROR, "create device_lock fail");
			return UERR_INIT;
		}
	}

	if (nand->Initialized) {
		return HAL_OK;
	}

	rtos_mutex_take(device_lock, MUTEX_WAIT_TIMEOUT);

	nand->LastErasedBlockAddr = 0xFFFFFFFF;

	flash_ID[0] = 0;
	flash_ID[1] = 0;
	flash_ID[2] = 0;

	/* Defaults must be in place before the first command: NandFlash_ReadID()
	 * already uses FLASH_cmd_rd_id from this struct. */
	NandFlash_StructInit(&g_nandflash_init_para);

	nandflash_spi_init();

	/* Soft Reset to clean status, make the Nand device enter the idle state.*/
	NandFlash_Reset();

	ret = NandFlash_ReadID(flash_ID);
	if (ret != HAL_OK) {
		goto exit;
	}

	info->MID = flash_ID[0];
	info->DID = flash_ID[1];
	info->ExtDID = flash_ID[2];

	/* RTK_LOGS only understands %d %u %s %08x %c; pack the 3 ID bytes into one
	 * %08x (0x00MMDDEE) instead of %02x, which would silently drop the line.
	 * Printed via RTK_LOGS (not VFS_DBG, which is compiled out by default) so
	 * the chip ID is visible during bring-up. */
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[secnand] ID(MID/DID/EXT) = %08x\r\n",
			 (flash_ID[0] << 16) | (flash_ID[1] << 8) | flash_ID[2]);

	/* Match the ID against the known-model whitelist for a friendly log line.
	 * Not being in the map is not an error: geometry comes from the ONFI
	 * parameter page below, so we continue with the detected parameters. */
	{
		u32 mi;
		const char *model = NULL;
		for (mi = 0; mi < sizeof(second_nand_models) / sizeof(second_nand_models[0]); mi++) {
			if (second_nand_models[mi].mid == flash_ID[0] &&
				second_nand_models[mi].did == flash_ID[1] &&
				second_nand_models[mi].ext == flash_ID[2]) {
				model = second_nand_models[mi].model_name;
				break;
			}
		}
		if (model != NULL) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "[secnand] detected model: %s\r\n", model);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "[secnand] ID not in known NAND model map, continue with ONFI-detected params\r\n");
		}
	}

	Feat_Reg = NandFlash_GetStatus(NAND_REG_CFG);
	if (0xFF == Feat_Reg) { //Timeout
		ret = HAL_TIMEOUT;
		goto exit;
	}

	/* Enable internal ECC, and keep buffer read mode so that the read column
	 * address is honoured - the bad-block marker and the ONFI parameter page are
	 * both addressed by column.  In continuous read mode the column is ignored
	 * and every read starts at byte 0 of the page. */
	Feat_Reg |= NAND_CFG_ECC_ENABLE;
	if (info->MID == NAND_MFG_WINBOND) {
		Feat_Reg |= NAND_CFG_BUF_READ;
	}

	ret = NandFlash_SetStatus(NAND_REG_CFG, Feat_Reg);
	if (ret != 0U) {
		ret = HAL_TIMEOUT;
		goto exit;
	}

	/* Unlock all blocks: the write-protect bits are set at the factory on most
	 * parts, and a locked device silently ignores erase and program. */
	NandFlash_SetStatus(NAND_REG_BLOCK_LOCK, NAND_BL_ALL_UNLOCKED);

	ret = SECOND_NAND_FTL_MfgInit(nand);
	if (ret == HAL_OK) {
		nand->Initialized = 1;
		vfs_second_nand_pagesize = info->PageSize;
		vfs_second_nand_pagenum = info->PagesPerBlock;
		vfs_second_nand_capacity = info->Capacity;
		VFS_DBG(VFS_INFO, "second nand: page=%d oob=%d pages/blk=%d blocks=%d capacity=%d",
				(int)info->PageSize, (int)info->OobSize, (int)info->PagesPerBlock,
				(int)info->BlocksPerLun, (int)info->Capacity);
	} else {
		VFS_DBG(VFS_ERROR, "second nand MfgInit fail: 0x%02x", ret);
	}

exit:
	rtos_mutex_give(device_lock);
	return ret;
}

/**
  * @brief  Read page
  * @param  addr : page address
  * @param  buf : data buffer
  * @retval HAL_OK : OK; others : FAIL
  */

u8 SECOND_NAND_FTL_ReadPage(u32 addr, u8 *buf)
{
	u8 ret;

	rtos_mutex_take(device_lock, MUTEX_WAIT_TIMEOUT);
	ret = NF_ReadPage(addr, buf, 1);
	rtos_mutex_give(device_lock);

	return ret;
}

/**
  * @brief  Read page without re-checking the bad-block marker
  * @param  addr : page address
  * @param  buf : data buffer
  * @retval HAL_OK : OK; others : FAIL
  * @note   For callers such as LBM that already track bad blocks in RAM: the
  *         marker check costs one extra array-to-cache per read.  ECC status is
  *         still evaluated, so data-integrity reporting is unchanged.
  */

u8 SECOND_NAND_FTL_ReadPageFast(u32 addr, u8 *buf)
{
	u8 ret;

	rtos_mutex_take(device_lock, MUTEX_WAIT_TIMEOUT);
	ret = NF_ReadPage(addr, buf, 0);
	rtos_mutex_give(device_lock);

	return ret;
}

/**
  * @brief  Erase block
  * @param  addr : page address
  * @param  force : erase even if the block is marked bad
  * @retval HAL_OK : OK; others : FAIL
  */

u8 SECOND_NAND_FTL_EraseBlock(u32 addr, u8 force)
{
	NAND_FTL_DeviceTypeDef *nand = &NandFlash_Device;
	u8 ret;
	u8 is_bad_block;

	if (!nand->Initialized) {
		return UERR_INIT;
	}

	rtos_mutex_take(device_lock, MUTEX_WAIT_TIMEOUT);

	ret = NF_IsBad(nand, addr, &is_bad_block);
	if (ret == HAL_OK) {
		if (is_bad_block && (force == 0)) {
			ret = UERR_NAND_BAD_BLOCK;
		} else {
			if (is_bad_block) {
				VFS_DBG(VFS_WARNING, "Force erase bad block 0x%08X", addr);
			}
			ret = NF_EraseBlock(nand, addr);
		}
	}

	rtos_mutex_give(device_lock);

	return ret;
}

/**
  * @brief  Write page
  * @param  addr : page address
  * @param  buf : data writen to the page
  * @param  do_erase : do block erase during write
  * @retval HAL_OK : OK; others : FAIL
  */

u8 SECOND_NAND_FTL_WritePage(u32 addr, const u8 *buf, u8 do_erase)
{
	NAND_FTL_DeviceTypeDef *nand = &NandFlash_Device;
	Flash_InfoTypeDef *info = &nand->MemInfo;
	u8 ret;
	u8 is_bad_block;
	u32 block_addr;

	if (!nand->Initialized) {
		return UERR_INIT;
	}

	rtos_mutex_take(device_lock, MUTEX_WAIT_TIMEOUT);

	ret = NF_IsBad(nand, addr, &is_bad_block);
	if (ret != HAL_OK) {
		goto exit;
	}

	if (is_bad_block) {
		ret = UERR_NAND_BAD_BLOCK;
		goto exit;
	}

	if (do_erase) {
		block_addr = NF_GetBlockAddr(nand, addr);
		if (block_addr != nand->LastErasedBlockAddr) {
			ret = NF_EraseBlock(nand, block_addr);
			if ((ret == HAL_OK) || (ret == UERR_NAND_WORN_BLOCK)) {
				nand->LastErasedBlockAddr = block_addr;
			} else {
				goto exit;
			}
		}
	}

	/*WEL bit shall be set, otherwise Nand will ignore this cmd*/
	if (NandFlash_WriteEn() != 0U) {
		ret = HAL_TIMEOUT;
		goto exit;
	}

	if (NandFlash_Page_Write_Data_Xfer(NAND_CMD_PP, 0, info->PageSize, (u8 *)buf) != HAL_OK) {
		ret = HAL_TIMEOUT;
		goto exit;
	}
	ret = NandFlash_Page_Write_Program_Execute(addr);

	if (ret == 0U) {
		ret = HAL_OK;
	} else if (ret == 0xFFU) {
		ret = HAL_TIMEOUT;
	} else {
		VFS_DBG(VFS_ERROR, "Fail to program page 0x%08X: 0x%02X", addr, ret);
		ret = NF_MarkBad(nand, addr);
		if (ret == HAL_OK) {
			ret = UERR_NAND_WORN_BLOCK;
		}
	}

exit:
	rtos_mutex_give(device_lock);
	return ret;
}

/**
  * @brief  Check bad block
  * @param  addr : page address
  * @param  value : 0 - Good; 1 - Bad
  * @retval HAL_OK : OK; others : FAIL
  */

u8 SECOND_NAND_FTL_IsBad(u32 addr, u8 *value)
{
	NAND_FTL_DeviceTypeDef *nand = &NandFlash_Device;
	u8 ret;

	if (!nand->Initialized) {
		return UERR_INIT;
	}

	rtos_mutex_take(device_lock, MUTEX_WAIT_TIMEOUT);
	ret = NF_IsBad(nand, addr, value);
	rtos_mutex_give(device_lock);

	return ret;
}

/**
  * @brief  Mark bad block
  * @param  addr : page address
  * @retval HAL_OK : OK; others : FAIL
  */

u8 SECOND_NAND_FTL_MarkBad(u32 addr)
{
	NAND_FTL_DeviceTypeDef *nand = &NandFlash_Device;
	u8 ret;

	if (!nand->Initialized) {
		return UERR_INIT;
	}

	rtos_mutex_take(device_lock, MUTEX_WAIT_TIMEOUT);
	ret = NF_MarkBad(nand, addr);
	rtos_mutex_give(device_lock);

	return ret;
}

/**
  * @brief  Query NAND Flash info
  * @param  info : destination for the discovered geometry
  * @retval HAL_OK : OK; others : FAIL
  */

u8 SECOND_NAND_FTL_Query(Flash_InfoTypeDef *info)
{
	u8 ret = UERR_INIT;
	NAND_FTL_DeviceTypeDef *nand = &NandFlash_Device;

	if (nand->Initialized) {
		FTL_MEMCPY((void *)info, (const void *)&nand->MemInfo, sizeof(Flash_InfoTypeDef));
		ret = HAL_OK;
	}

	return ret;
}

/* LBM back-end hooks for the external SPI NAND (see lbm.h / lbm_core.c). */
const lbm_dev_ops_t lbm_second_nand_ops = {
	.read_page = SECOND_NAND_FTL_ReadPage,
	.read_page_fast = SECOND_NAND_FTL_ReadPageFast,
	.write_page = SECOND_NAND_FTL_WritePage,
	.erase_block = SECOND_NAND_FTL_EraseBlock,
	.mark_bad = SECOND_NAND_FTL_MarkBad,
};

/**
  * @brief  Bring-up self-test: erase one block, program a known pattern into one
  *         page, read it back and report the result.  Uses RTK_LOGS directly
  *         (VFS_DBG is compiled out unless VFS_DBG_ON=1), so it prints even on a
  *         release build.  Call after SECOND_NAND_FTL_Init(), before mounting.
  * @param  test_page : page address to use (its whole block is erased first)
  * @retval 0 on byte-exact read-back, -1 otherwise
  */
u8 SECOND_NAND_FTL_SelfTest(u32 test_page)
{
	NAND_FTL_DeviceTypeDef *nand = &NandFlash_Device;
	u32 psize = nand->MemInfo.PageSize;
	u8 *wbuf = NULL, *rbuf = NULL, *rbuf2 = NULL;
	u8 er, wr, rd;
	u32 i, ffcnt = 0, miss = 0, first_miss = 0xFFFFFFFF, fm = 0;
	u8 ret = 0;

	if (!nand->Initialized || psize == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[nandtest] not initialized\r\n");
		return (u8) - 1;
	}

	wbuf = (u8 *)rtos_mem_malloc(psize);
	rbuf = (u8 *)rtos_mem_malloc(psize);
	rbuf2 = (u8 *)rtos_mem_malloc(psize);
	if (!wbuf || !rbuf || !rbuf2) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[nandtest] oom\r\n");
		goto done;
	}
	for (i = 0; i < psize; i++) {
		wbuf[i] = (u8)(i & 0xFF);
	}

	/* 0. dump the key feature registers: protection(A0), config(B0), status(C0).
	 * A0 should be 0x00 (all blocks unlocked); B0 should have ECC-EN(bit4) and,
	 * on Winbond, BUF(bit3) set; C0 low bits show BUSY/WEL, bits5:4 = ECC. */
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[nandtest] reg A0=%08x B0=%08x C0=%08x\r\n",
			 NandFlash_GetStatus(NAND_REG_BLOCK_LOCK),
			 NandFlash_GetStatus(NAND_REG_CFG),
			 NandFlash_GetStatus(NAND_REG_STATUS));

	/* 1. erase the block (force), then confirm it reads back all-0xFF */
	er = SECOND_NAND_FTL_EraseBlock(test_page, 1);
	rd = SECOND_NAND_FTL_ReadPageFast(test_page, rbuf);
	for (i = 0; i < psize; i++) {
		if (rbuf[i] != 0xFF) {
			ffcnt++;
		}
	}
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[nandtest] page=%d erase_ret=%d readFF_ret=%d nonFF=%d C0=%08x after erase\r\n",
			 (int)test_page, er, rd, (int)ffcnt, NandFlash_GetStatus(NAND_REG_STATUS));
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[nandtest] erased rbuf[0..7]=%08x %08x\r\n",
			 (rbuf[0] << 24) | (rbuf[1] << 16) | (rbuf[2] << 8) | rbuf[3],
			 (rbuf[4] << 24) | (rbuf[5] << 16) | (rbuf[6] << 8) | rbuf[7]);

	/* 2. program the pattern (block already erased, so do_erase=0) */
	wr = SECOND_NAND_FTL_WritePage(test_page, wbuf, 0);

	/* 3. read it back TWICE (rbuf, rbuf2) to separate a flaky READ from a bad WRITE */
	rd = SECOND_NAND_FTL_ReadPage(test_page, rbuf);
	SECOND_NAND_FTL_ReadPage(test_page, rbuf2);
	for (i = 0; i < psize; i++) {
		if (rbuf[i] != wbuf[i]) {
			miss++;
			if (first_miss == 0xFFFFFFFF) {
				first_miss = i;
			}
		}
	}
	/* how many bytes differ between the two independent reads of the same page */
	{
		u32 rr = 0;
		for (i = 0; i < psize; i++) {
			if (rbuf[i] != rbuf2[i]) {
				rr++;
			}
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "[nandtest] read-vs-read diff=%d (0 => read is deterministic)\r\n", (int)rr);
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[nandtest] write_ret=%d read_ret=%d mismatch=%d/%d first=%d\r\n",
			 wr, rd, (int)miss, (int)psize, (int)first_miss);

	/* dump 8 bytes at the first mismatch: the corruption shape names the cause
	 * (0x00 => data absent; AND of pattern => not erased; shifted => dummy/echo
	 * offset; random & read-vs-read!=0 => signal integrity / clock too fast). */
	fm = (first_miss == 0xFFFFFFFF) ? 0 : first_miss;
	if (fm + 8 <= psize) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "[nandtest] @%d wrote=%08x %08x  read=%08x %08x  read2=%08x %08x\r\n",
				 (int)fm,
				 (wbuf[fm] << 24) | (wbuf[fm + 1] << 16) | (wbuf[fm + 2] << 8) | wbuf[fm + 3],
				 (wbuf[fm + 4] << 24) | (wbuf[fm + 5] << 16) | (wbuf[fm + 6] << 8) | wbuf[fm + 7],
				 (rbuf[fm] << 24) | (rbuf[fm + 1] << 16) | (rbuf[fm + 2] << 8) | rbuf[fm + 3],
				 (rbuf[fm + 4] << 24) | (rbuf[fm + 5] << 16) | (rbuf[fm + 6] << 8) | rbuf[fm + 7],
				 (rbuf2[fm] << 24) | (rbuf2[fm + 1] << 16) | (rbuf2[fm + 2] << 8) | rbuf2[fm + 3],
				 (rbuf2[fm + 4] << 24) | (rbuf2[fm + 5] << 16) | (rbuf2[fm + 6] << 8) | rbuf2[fm + 7]);
	}

	ret = (miss == 0) ? 0 : (u8) - 1;
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "[nandtest] RESULT %s\r\n", (miss == 0) ? "PASS" : "FAIL");

done:
	if (wbuf) {
		rtos_mem_free(wbuf);
	}
	if (rbuf) {
		rtos_mem_free(rbuf);
	}
	if (rbuf2) {
		rtos_mem_free(rbuf2);
	}
	return ret;
}
