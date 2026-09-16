/**
  ******************************************************************************
  * @file    vfs_second_nand_spi.c
  * @author
  * @version V1.0.0
  * @date    2023-12
  * @brief   SPI-master driver for an external (second) SPI NAND flash.
  *          Provides the raw NAND primitives - read ID, get/set feature,
  *          page read (array-to-cache + read-from-cache), page program
  *          (data load + program execute) and block erase - on top of the
  *          generic spi_api DMA transfers.
  *
  *          Not thread safe by itself: the shared DMA buffers and the device's
  *          WEL latch make a command sequence (WREN + program + status poll)
  *          indivisible.  vfs_second_nand_ftl.c serialises every entry point.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */
#include <string.h>
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "vfs_second_nand_spi.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "vfs.h"

#define CMD_LENGTH_ONE			1	// Command length is 1 byte
#define CMD_LENGTH_TWO			2	// Command length is 2 byte
#define CMD_LENGTH_THREE		3	// Command length is 3 byte
#define CMD_LENGTH_FOUR			4	// Command length is 4 byte
#define CMD_LENGTH_FIVE			5	// Command length is 5 byte

/* A single transfer never exceeds one page; generous upper bound for the DMA
 * completion semaphores so a wedged bus reports an error instead of hanging. */
#define SPI_XFER_TIMEOUT_MS	1000

/* Busy polling bound.  The slowest NAND operation is a block erase (~10 ms
 * typical, 10x that worst case); each poll is a real SPI transaction, so this
 * is a wall-clock bound of well over a second, not a tight spin count. */
#define NAND_BUSY_POLL_MAX	20000

NandFlash_InitTypeDef g_nandflash_init_para;

/* for dma mode, start address of buffer should be 32-byte aligned*/
static u8 Spi_MasterTxBuf[SPI_BUFFER_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 Spi_MasterRxBuf[SPI_BUFFER_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

spi_t nand_spi_master;

static rtos_sema_t nand_spi_tx_sema = NULL;
static rtos_sema_t nand_spi_rx_sema = NULL;

static void Spi_Master_tr_done_callback(uint32_t id, SpiIrq event)
{
	(void)id;
	switch (event) {
	case SpiRxIrq:
		rtos_sema_give(nand_spi_rx_sema);
		break;
	case SpiTxIrq:
		rtos_sema_give(nand_spi_tx_sema);
		break;
	default:
		VFS_DBG(VFS_WARNING, "unknown interrput evnent!");
	}
}

/* Wait until the bus has actually clocked out everything.  The TX DMA IRQ only
 * says the last byte was pushed into the FIFO, so this is what tells us the
 * echo bytes have all landed in the RX FIFO. */
static void nand_spi_wait_idle(void)
{
	u32 count = 0;

	while (spi_busy(&nand_spi_master)) {
		if (++count > 100000) {
			rtos_time_delay_ms(1);
		}
	}
}

/**
  * @brief  Full-duplex DMA transfer whose received bytes matter (read ID /
  *         get-feature / read-from-cache).  The DMA length is rounded up to a
  *         whole cache line: the RX path pairs with DCache_Invalidate, which
  *         acts on whole lines, so a sub-line length leaves the tail line
  *         incoherent -> intermittent garbage in the read-back.  The extra
  *         clocked bytes are harmless (device keeps shifting out, caller reads
  *         only the meaningful ones).  Mirrors the proven second-NOR read path.
  * @param  len : number of meaningful bytes (cmd + addr + data)
  * @retval HAL_OK / HAL_BUSY / HAL_TIMEOUT
  * @note   Order is load bearing: wait_idle -> flush -> start, so stale RX-FIFO
  *         echo can't shift the received bytes.
  */
static int nand_spi_write_read(u32 len)
{
	u32 dma_len = len + (CACHE_LINE_SIZE - (len % CACHE_LINE_SIZE));

	if (dma_len > SPI_BUFFER_SIZE) {
		return HAL_ERR_PARA;
	}

	nand_spi_wait_idle();
	spi_flush_rx_fifo(&nand_spi_master);

	if (spi_master_write_read_stream_dma(&nand_spi_master, (char *)Spi_MasterTxBuf, (char *)Spi_MasterRxBuf, dma_len) != HAL_OK) {
		return HAL_BUSY;
	}
	if (rtos_sema_take(nand_spi_tx_sema, SPI_XFER_TIMEOUT_MS) != RTK_SUCCESS ||
		rtos_sema_take(nand_spi_rx_sema, SPI_XFER_TIMEOUT_MS) != RTK_SUCCESS) {
		return HAL_TIMEOUT;
	}
	return HAL_OK;
}

/**
  * @brief  Multi-byte transmit (get/set feature write, page-data load, erase,
  *         program-execute).  TX-only DMA with the EXACT length -- unlike the
  *         read path this must NOT be rounded up, or the extra bytes would be
  *         programmed into the page.  Waits on TX completion only.
  *
  *         Single-byte commands (WREN, Reset) do NOT use this: a 1-byte DMA
  *         transfer does not reliably clock the byte out, which left WEL unset
  *         and made every erase/program fail.  They use PIO spi_master_write()
  *         instead (see NandFlash_WriteEn / NandFlash_Reset), matching the
  *         proven littlefs-on-NAND driver.
  * @param  len : number of bytes to send (>= 2)
  * @retval HAL_OK / HAL_BUSY / HAL_TIMEOUT
  */
static int nand_spi_write(u32 len)
{
	if (len > SPI_BUFFER_SIZE) {
		return HAL_ERR_PARA;
	}

	nand_spi_wait_idle();
	spi_flush_rx_fifo(&nand_spi_master);

	/* Use the full-duplex primitive (both TX and RX completions) with the EXACT
	 * length: on this board that is the only reliable path -- a TX-only
	 * write_stream_dma intermittently fails to take effect (WEL stays 0, feature
	 * writes are dropped).  The received bytes are ignored; the length must not
	 * be rounded up or the extra bytes would be programmed into the page. */
	if (spi_master_write_read_stream_dma(&nand_spi_master, (char *)Spi_MasterTxBuf, (char *)Spi_MasterRxBuf, len) != HAL_OK) {
		return HAL_BUSY;
	}
	if (rtos_sema_take(nand_spi_tx_sema, SPI_XFER_TIMEOUT_MS) != RTK_SUCCESS ||
		rtos_sema_take(nand_spi_rx_sema, SPI_XFER_TIMEOUT_MS) != RTK_SUCCESS) {
		return HAL_TIMEOUT;
	}
	return HAL_OK;
}

void NandFlash_Reset(void)
{
	/* single-byte command: use PIO, not DMA (see nand_spi_write note) */
	nand_spi_wait_idle();
	spi_flush_rx_fifo(&nand_spi_master);
	spi_master_write(&nand_spi_master, NAND_CMD_RESET);

	/* tRST: up to 500us for a read reset, ~10ms if a program/erase was aborted */
	DelayMs(10);
}


u8 NandFlash_ReadID(u8 *flash_id)
{
	int ret;

	_memset(Spi_MasterTxBuf, 0, CACHE_LINE_SIZE);
	Spi_MasterTxBuf[0] = g_nandflash_init_para.FLASH_cmd_rd_id;
	Spi_MasterTxBuf[1] = 0xFF;

	ret = nand_spi_write_read(CMD_LENGTH_FIVE);
	if (ret != HAL_OK) {
		VFS_DBG(VFS_ERROR, "read id fail: %d", ret);
		return (u8)ret;
	}

	/* cmd + 1 dummy byte, then MID / DID / ExtDID */
	flash_id[0] = Spi_MasterRxBuf[2];
	flash_id[1] = Spi_MasterRxBuf[3];
	flash_id[2] = Spi_MasterRxBuf[4];

	return HAL_OK;
}


/**
* @brief  This function is used to wait until Flash Busy done or WLE bit set.
* @param  WaitType:  wait type, which can be one of the following parameters:
*			@arg WAIT_FLASH_BUSY: wait until Flash busy done.
*			@arg WAIT_WRITE_EN: wait until flash status WLE(Write Enbale Latch) bit is set.
* @retval status:
*			bit 0:Erase/Program In Progress (BUSY)
*			bit 1:Write Enable Latch (WEL)
*			Others.(timeout is 0xFF)
**/
u8 NandFlash_WaitBusy(u32 WaitType)
{
	u8 status = 0;
	u32 BusyCheck = 0;
	u32 i = 0;

	do {
		status = NandFlash_GetStatus(NAND_REG_STATUS);
		if (status == 0xFFU) {
			return 0xFF;   /* transfer failed; no point polling further */
		}

		if (WaitType == WAIT_WRITE_EN) {
			/* write enable ok & flash not busy */
			BusyCheck = (!(status & g_nandflash_init_para.FLASH_WLE_bit)) || (status & g_nandflash_init_para.FLASH_Busy_bit);
		} else {
			BusyCheck = (status & g_nandflash_init_para.FLASH_Busy_bit);
		}

		if (!BusyCheck) {
			break;
		}

		if (++i > NAND_BUSY_POLL_MAX) {
			VFS_DBG(VFS_ERROR, "wait busy timeout, type=%d status=0x%02x", (int)WaitType, status);
			status = 0xFF;
			break;
		}
	} while (1);

	return status;
}

/**
* @brief  This function is used to send WREN(Write Enable) cmd to set WEL bit in flash status register.
* @param  none
* @note   This function must be used prior to every Program execute,Block Erase, OTP program/OTP protection.
* @retval status: 0 : OK; 0xFF : timeout
*/

u8 NandFlash_WriteEn(void)
{
	u8 status;

	/* Wait for flash busy done */
	status = NandFlash_WaitBusy(WAIT_FLASH_BUSY);
	if (status == 0xFFU) {
		return 0xFF;
	}

	/* WREN is a single-byte command: send it via PIO, not DMA.  A 1-byte DMA
	 * transfer does not reliably clock the byte out here, so WEL stayed 0 and
	 * every erase/program silently failed (erase returned HAL_TIMEOUT from the
	 * WAIT_WRITE_EN poll).  PIO matches the proven littlefs-on-NAND driver. */
	nand_spi_wait_idle();
	spi_flush_rx_fifo(&nand_spi_master);
	spi_master_write(&nand_spi_master, g_nandflash_init_para.FLASH_cmd_wr_en);

	/* Wait for write enable done*/
	status = NandFlash_WaitBusy(WAIT_WRITE_EN);
	if (status == 0xFFU) {
		VFS_DBG(VFS_ERROR, "WEL not latched");
		return 0xFF;
	}

	return 0;
}

/**
  * @brief    NandFlash_GetStatus used to get register status.
  * @param    FeatureID: the address of status register
  * @retval   the value of the status register
  *			C0h register: bit 0:Erase/Program In Progress (BUSY)
  *			             bit 1:Write Enable Latch (WEL)
  *			Others.(timeout is 0xFF)
  */
u8 NandFlash_GetStatus(u8 FeatureID)
{
	_memset(Spi_MasterTxBuf, 0xFF, CACHE_LINE_SIZE);
	Spi_MasterTxBuf[0] = g_nandflash_init_para.FLASH_cmd_rd_status;
	Spi_MasterTxBuf[1] = FeatureID;

	if (nand_spi_write_read(CMD_LENGTH_THREE) != HAL_OK) {
		return 0xFF;
	}

	return Spi_MasterRxBuf[2];
}

/**
  * @brief  NandFlash_SetStatus used to set register status.
  * @param    FeatureID: the address of status register
  * @param    Status: value to be written
  * @retval   Execute status: 0 : OK; others : FAIL
  */

u8 NandFlash_SetStatus(u8 FeatureID, u8 Status)
{
	int ret;

	_memset(Spi_MasterTxBuf, 0xFF, CACHE_LINE_SIZE);
	Spi_MasterTxBuf[0] = g_nandflash_init_para.FLASH_cmd_wr_status;
	Spi_MasterTxBuf[1] = FeatureID;
	Spi_MasterTxBuf[2] = Status;

	ret = nand_spi_write(CMD_LENGTH_THREE);
	if (ret != HAL_OK) {
		VFS_DBG(VFS_ERROR, "set feature 0x%02x fail: %d", FeatureID, ret);
		return 0xFF;
	}

	return 0;
}

/**
  * @brief  NandFlash_SetStatusBits set or clear status bits., used to set protect bit or quad enable bit
  * @param  FeatureID: the address of status register
  * @param  SetBits: bit mask in the status register
  * @param  NewState: ENABLE/DISABLE
  * @retval none
  */

void NandFlash_SetStatusBits(u8 FeatureID, u8 SetBits, u8 NewState)
{
	u8 status = 0;
	u8 status_mask = (u8)(SetBits & 0xFF);

	status = NandFlash_GetStatus(FeatureID);
	if (0xFF == status) { //Timeout
		VFS_DBG(VFS_ERROR, ": TIMEOUT");
		return;
	}

	if (NewState == ENABLE) {
		status |= status_mask;
	} else {
		status &= ~status_mask;
	}

	status = NandFlash_SetStatus(FeatureID, status);
	if (status) {
		VFS_DBG(VFS_ERROR, "Set Status FAILED");
	}
	return;
}

/**
  * @brief  This function is used to erase flash. EraseBlock: Erase specified block(128KB)
  * @param  PageAddr: should 4 byte align. The block which the address in will be erased.
  * @retval status: Success:0 or erase Fail: Others.(timeout is 0xFF)
  */

u8 NandFlash_Erase(u32 PageAddr)
{
	u8 status;

	_memset(Spi_MasterTxBuf, 0, CACHE_LINE_SIZE);

	/* Block Erase Sequence is CMD + PA23-16 + PA15-8 + PA7-0. */
	Spi_MasterTxBuf[0] = g_nandflash_init_para.FLASH_cmd_block_e;
	Spi_MasterTxBuf[1] = (u8)((PageAddr >> 16) & 0xFF);
	Spi_MasterTxBuf[2] = (u8)((PageAddr >> 8) & 0xFF);
	Spi_MasterTxBuf[3] = (u8)(PageAddr & 0xFF);

	if (nand_spi_write(CMD_LENGTH_FOUR) != HAL_OK) {
		return 0xFF;
	}

	/* polling busy done */
	status = NandFlash_WaitBusy(WAIT_FLASH_BUSY);

	if (status & g_nandflash_init_para.FLASH_EFail_bit) {
		return status;
	} else {
		return 0;
	}
}

/**
  * @brief  This function is used to read data from Nand array to Nand Cache.
  * @param  PageAddr: which page will be read
  * @retval status: NAND_STATUS_ECC_NO_BITFLIPS:0 or Need to check: Others.(timeout is 0xFF)
  * @note
  * 	 if status is 0 means NAND_STATUS_ECC_NO_BITFLIPS, otherwise need confirm spi nand datasheet.
  */

u8 NandFlash_Page_Read_ArrayToCache(u32 PageAddr)
{
	_memset(Spi_MasterTxBuf, 0, CACHE_LINE_SIZE);

	/* Page read Sequence is CMD + PA23-16 + PA15-8 + PA7-0. */
	Spi_MasterTxBuf[0] = g_nandflash_init_para.FLASH_cmd_page_read;
	Spi_MasterTxBuf[1] = (u8)((PageAddr >> 16) & 0xFF);
	Spi_MasterTxBuf[2] = (u8)((PageAddr >> 8) & 0xFF);
	Spi_MasterTxBuf[3] = (u8)(PageAddr & 0xFF);

	if (nand_spi_write(CMD_LENGTH_FOUR) != HAL_OK) {
		return 0xFF;
	}

	return NandFlash_WaitBusy(WAIT_FLASH_BUSY);
}

/**
  * @brief  This function is used to read data from cache
  * @param  Cmd: read-from-cache command (0x03 Read Data / 0x0B Fast Read)
  * @param  ByteAddr: the column offset within the page (main area 0..PageSize-1,
  *                   spare area starts at PageSize).  Only honoured when the
  *                   device is in buffer read mode, see NAND_CFG_BUF_READ.
  * @param  ByteLen: the number of bytes to be read
  * @param  pData: pointer to a byte array which is used to save received data.
  * @retval HAL_OK : OK; others : FAIL
  */
u8 NandFlash_Page_Read_FromCache(u8 Cmd, u32 ByteAddr, u32 ByteLen, u8 *pData)
{
	int ret;

	if ((ByteLen + CMD_LENGTH_FOUR) > SPI_BUFFER_SIZE) {
		VFS_DBG(VFS_ERROR, "read len %d exceeds buffer", (int)ByteLen);
		return HAL_ERR_PARA;
	}

	_memset(Spi_MasterTxBuf, 0xFF, ByteLen + CMD_LENGTH_FOUR);

	/* CMD + CA15-8 + CA7-0 + 1 dummy byte, then the data phase */
	Spi_MasterTxBuf[0] = Cmd;
	Spi_MasterTxBuf[1] = (ByteAddr >> 8) & 0x0F;
	Spi_MasterTxBuf[2] = ByteAddr & 0xFF;
	Spi_MasterTxBuf[3] = 0xFF;

	ret = nand_spi_write_read(ByteLen + CMD_LENGTH_FOUR);
	if (ret != HAL_OK) {
		VFS_DBG(VFS_ERROR, "read from cache fail: %d", ret);
		return (u8)ret;
	}

	_memcpy(pData, Spi_MasterRxBuf + CMD_LENGTH_FOUR, ByteLen);

	return HAL_OK;
}

/**
  * @brief  This function is used to Load data to Nand Cache
  * @param  Cmd: The command need to be sent, Can be NAND_CMD_QPP_RANDOM/NAND_CMD_QPP/NAND_CMD_PP_RANDOM/NAND_CMD_PP
  * @param  ByteAddr: the column offset within the page
  * @param  ByteLen: the number of bytes to be transfered
  * @param  pData: pointer to a byte array that is to be sent.
  * @retval HAL_OK : OK; others : FAIL
  * @note   NAND_CMD_PP resets the whole cache to 0xFF first, NAND_CMD_PP_RANDOM
  *         does not - use the latter to patch a single field of a loaded page.
  */
u8 NandFlash_Page_Write_Data_Xfer(u8 Cmd, u32 ByteAddr, u32 ByteLen, u8 *pData)
{
	int ret;

	if ((ByteLen + CMD_LENGTH_THREE) > SPI_BUFFER_SIZE) {
		VFS_DBG(VFS_ERROR, "write len %d exceeds buffer", (int)ByteLen);
		return HAL_ERR_PARA;
	}

	_memset(Spi_MasterTxBuf, 0, CMD_LENGTH_THREE);

	/* CMD + CA15-8 + CA7-0, then the data phase (no dummy byte on program) */
	Spi_MasterTxBuf[0] = Cmd;
	Spi_MasterTxBuf[1] = (ByteAddr >> 8) & 0xFF;
	Spi_MasterTxBuf[2] = ByteAddr & 0xFF;

	_memcpy(Spi_MasterTxBuf + CMD_LENGTH_THREE, pData, ByteLen);

	ret = nand_spi_write(ByteLen + CMD_LENGTH_THREE);
	if (ret != HAL_OK) {
		VFS_DBG(VFS_ERROR, "load to cache fail: %d", ret);
	}

	return (u8)ret;
}

/**
  * @brief  This function is used to program Page
  * @param  PageAddr: which page will be program
  * @retval status: Success:0 or Program Fail: Others.(timeout is 0xFF)
  */
u8 NandFlash_Page_Write_Program_Execute(u32 PageAddr)
{
	u8 status;

	_memset(Spi_MasterTxBuf, 0, CACHE_LINE_SIZE);

	/* Program Execution Sequence is CMD + PA23-16 + PA15-8 + PA7-0. */
	Spi_MasterTxBuf[0] = g_nandflash_init_para.FLASH_cmd_page_write;
	Spi_MasterTxBuf[1] = (u8)((PageAddr >> 16) & 0xFF);
	Spi_MasterTxBuf[2] = (u8)((PageAddr >> 8) & 0xFF);
	Spi_MasterTxBuf[3] = (u8)(PageAddr & 0xFF);

	if (nand_spi_write(CMD_LENGTH_FOUR) != HAL_OK) {
		return 0xFF;
	}

	/* polling busy done */
	status = NandFlash_WaitBusy(WAIT_FLASH_BUSY);

	if (status & g_nandflash_init_para.FLASH_PFail_bit) {
		return status;
	} else {
		return 0;
	}
}


/**
  * @brief  Configure SPI
  * @param  void
  * @retval void
  */
void nandflash_spi_init(void)
{
	if (nand_spi_tx_sema == NULL) {
		rtos_sema_create_binary(&nand_spi_tx_sema);
	}
	if (nand_spi_rx_sema == NULL) {
		rtos_sema_create_binary(&nand_spi_rx_sema);
	}
	if (nand_spi_tx_sema == NULL || nand_spi_rx_sema == NULL) {
		VFS_DBG(VFS_ERROR, "create sema fail");
		return;
	}

	nand_spi_master.spi_idx = SPI_NAND_NODE;
	spi_init(&nand_spi_master, SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CS);
	spi_format(&nand_spi_master, SPI_DATA_FRAME_SIZE_8_BITS, SPI_MODE_0, SPI_ROLE_MASTER);
	spi_frequency(&nand_spi_master, SPI_SCLK_FREQ);

	spi_irq_hook(&nand_spi_master, (spi_irq_handler)Spi_Master_tr_done_callback, (uint32_t)&nand_spi_master);
	spi_flush_rx_fifo(&nand_spi_master);

	VFS_DBG(VFS_INFO, "second nand spi%d init, freq %d Hz", SPI_NAND_NODE & 0x01, SPI_SCLK_FREQ);

	return;
}

void nandflash_spi_deinit(void)
{
	spi_free(&nand_spi_master);
}


/**
* @brief  Initializes the parameters in the NandFlash_InitTypeDef with
*		its default values according to Winbond spec.
* @param FLASH_InitStruct: pointer to a NandFlash_InitTypeDef
*		structure which will be initialized.
* @retval none
*/

void NandFlash_StructInit_DS(NandFlash_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_WINBOND;
	FLASH_InitStruct->FLASH_baud_rate = 1;

	FLASH_InitStruct->FLASH_cur_cmd = FLASH_CMD_READ;

	/* status bit define */
	/* winbond QSPI is enable by default, set Bit1 of feature 0xA0 to disable QSPI*/
	/* WP#, HOLD# are disabled by default*/
	FLASH_InitStruct->FLASH_QuadEn_bit = 0;
	FLASH_InitStruct->FLASH_Busy_bit = NAND_STATUS_BUSY;
	FLASH_InitStruct->FLASH_WLE_bit = NAND_STATUS_WEL;
	FLASH_InitStruct->FLASH_Status2_exist = 0;

	/* other flash commnad set */
	FLASH_InitStruct->FLASH_cmd_wr_en = NAND_CMD_WREN;
	FLASH_InitStruct->FLASH_cmd_rd_id = NAND_CMD_RDID;
	FLASH_InitStruct->FLASH_cmd_rd_status = NAND_CMD_RDSR;
	FLASH_InitStruct->FLASH_cmd_rd_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_wr_status = NAND_CMD_WRSR;
	FLASH_InitStruct->FLASH_cmd_wr_status2 = 0;
	FLASH_InitStruct->FLASH_cmd_chip_e = 0;
	FLASH_InitStruct->FLASH_cmd_block_e = NAND_CMD_BE;
	FLASH_InitStruct->FLASH_cmd_sector_e = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn_release = 0;
	FLASH_InitStruct->FLASH_cmd_pwdn = 0;

	/* new calibration */
	FLASH_InitStruct->phase_shift_idx = 0;

	FLASH_InitStruct->FLASH_rd_fast_single = NAND_CMD_FREAD;
	FLASH_InitStruct->FLASH_addr_phase_len = NAND_COL_ADDR_2_BYTE;
	FLASH_InitStruct->FLASH_dum_en = DISABLE; /*FLASH_dum_en is used to control PRM function in Nor Flash.*/
	FLASH_InitStruct->FLASH_dum_byte = 0;

	FLASH_InitStruct->FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_2K_BIT_EXP;

	/* basic page rd/wr operation */
	FLASH_InitStruct->FLASH_cmd_page_read = NAND_CMD_PAGERD;
	FLASH_InitStruct->FLASH_cmd_page_write = NAND_CMD_PROMEXEC;
	FLASH_InitStruct->FLASH_EFail_bit = NAND_STATUS_ERASE_FAILED;
	FLASH_InitStruct->FLASH_PFail_bit = NAND_STATUS_PROG_FAILED;
	FLASH_InitStruct->FLASH_ECC_bit = BIT_OFFSET_LEN_MASK_32(4, 2);

	/*The default state of the internal ECC is enabled. */
	FLASH_InitStruct->FLASH_ECC_EN_bit = NAND_CFG_ECC_ENABLE;
}


/**
* @brief  Apply the Micron specific deltas on top of NandFlash_StructInit_DS().
* @param FLASH_InitStruct: pointer to a NandFlash_InitTypeDef
*		structure which will be initialized.
* @retval none
* @note   NandFlash_StructInit_DS() must have run first (it does, from
*         SECOND_NAND_FTL_Init) - these helpers only override what differs.
*/

void NandFlash_StructInit_Micron(NandFlash_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_MICRON;

	/* status bit define */
	/* micron QSPI is enable by default, and cannot be disabled!! */
	/* WP#, HOLD# are enabled by default, and can be disabled by set bit1 of feature 0xA0*/
	FLASH_InitStruct->FLASH_QuadEn_bit = 0;

	/* basic page rd/wr operation */
	FLASH_InitStruct->FLASH_ECC_bit = BIT_OFFSET_LEN_MASK_32(4, 3);
}


/**
* @brief  Apply the MXIC specific deltas on top of NandFlash_StructInit_DS().
* @param FLASH_InitStruct: pointer to a NandFlash_InitTypeDef
*		structure which will be initialized.
* @retval none
*/

void NandFlash_StructInit_MXIC(NandFlash_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_MXIC;

	/* status bit define */
	/* mxic  QE bit is bit0 of feature 0xB0, QSPI is disabled by default */
	/* WP#, HOLD# are enabled by default*/
	FLASH_InitStruct->FLASH_QuadEn_bit = NAND_CFG_QUAD_ENABLE;

	/* basic page rd/wr operation */
	FLASH_InitStruct->FLASH_ECC_bit = BIT_OFFSET_LEN_MASK_32(4, 2);
}


/**
* @brief  Apply the GigaDevice specific deltas on top of NandFlash_StructInit_DS().
* @param FLASH_InitStruct: pointer to a NandFlash_InitTypeDef
*		structure which will be initialized.
* @retval none
*/

void NandFlash_StructInit_GD(NandFlash_InitTypeDef *FLASH_InitStruct)
{
	FLASH_InitStruct->FLASH_Id = FLASH_ID_GD;

	/* status bit define */
	/* gigadevice QE bit is bit0 of feature 0xB0, QSPI is disabled by default */
	/* WP#, HOLD# are enabled by default*/
	FLASH_InitStruct->FLASH_QuadEn_bit = NAND_CFG_QUAD_ENABLE;

	FLASH_InitStruct->FLASH_ECC_bit = BIT_OFFSET_LEN_MASK_32(4, 2);
}


/**
* @brief  Initializes the parameters in the NandFlash_InitTypeDef with
*		its default values according to Winbond spec.
* @param FLASH_InitStruct: pointer to a NandFlash_InitTypeDef
*		structure which will be initialized.
* @retval none
*/

void NandFlash_StructInit(NandFlash_InitTypeDef *FLASH_InitStruct)
{
	NandFlash_StructInit_DS(FLASH_InitStruct);
}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor *****END OF FILE****/
