#include "ameba_soc.h"
#include "LzmaDec.h"

#define BOOTLZMA_CMP_DATA_BEFORE_ERASE

/* Macro Defines */
#define BOOTLZMA_ERASE_SECTOR_SIZE 4096
#define BOOTLZMA_ERASE_SECTOR_MASK (~(BOOTLZMA_ERASE_SECTOR_SIZE - 1U))
#define BOOTLZMA_ERASE_BLOCK_SIZE (4096*16)
#define BOOTLZMA_ERASE_BLOCK_MASK (~(BOOTLZMA_ERASE_BLOCK_SIZE - 1U))

#define BOOTLZMA_DECOMP_FILE_SIZE 16384 //16kb = 16 * 1024

#define BOOTLZMA_HDR_TOTALFILES_SIZE 2 //LZMA total files support up to 65535 files
#define BOOTLZMA_HDR_HASHOVER_SIZE 0 //Indicates if Hash overflowed into a new lzma file, this shows the start location of hash stored on second last file
#define BOOTLZMA_HDR_FILE_SIZE 2 //LZMA file size should be less than 16k bytes

#define BOOTLZMA_LZMA_PROP_OFFSET 0 //0th byte of the header is lzma properties
#define BOOTLZMA_LZMA_UNCOMPRESSED_SIZE 8
#define BOOTLZMA_LZMA_FORMAT_IDENTIFIER 13 //13th byte of the header is always 0
#define BOOTLZMA_LZMA_LC 3  //Properties = (pb * 5 + lp) * 9 + lc
#define BOOTLZMA_LZMA_LP 0
#define BOOTLZMA_LZMA_PB 2

u8 *output_buf;
u32 g_last_block_addr, g_curr_block_addr = 0xFFFFFFFF;

static const char *TAG = "BOOT";

/**
  * @brief  Write a stream of data to specified address
  * @param  address: Specifies the starting address to write to.
  * @param  len: Specifies the length of the data to write.
  * @param  data: Pointer to a byte array that is to be written.
 *  @return void
  */
static void bootLzma_flash_write_stream(u32 address, u32 len, u8 *pbuf)
{
	// Check address: 4byte aligned & page(256bytes) aligned
	u32 page_begin = address & (~0xff);
	u32 page_end = (address + len - 1) & (~0xff);
	u32 page_cnt = ((page_end - page_begin) >> 8) + 1;

	u32 addr_begin = address;
	u32 addr_end = (page_cnt == 1) ? (address + len) : (page_begin + 0x100);
	u32 size = addr_end - addr_begin;

	while (page_cnt) {
		FLASH_TxData(addr_begin, size, pbuf);
		pbuf += size;

		page_cnt--;
		addr_begin = addr_end;
		addr_end = (page_cnt == 1) ? (address + len) : (addr_begin + 0x100);
		size = addr_end - addr_begin;
	}

	/* DCache_Invalidate at last */
}

/**
 *  @brief Description of bootLzma_flash_write
 *
 *  -Erase blocks in 4k bytes size before write
 *  -Write in designated address using hal_flash_burst_write
 *
 *  @param u32 write_addr		:The starting of write address in flash.
 *  @param u32 write_length		:The write data length.
 *  @param u8 *write_data		:The address of write data about to be programmed to the flash.
 *
 *  @return void
 */
static void bootLzma_flash_write(u32 write_addr, u32 write_length, u8 *write_data)
{
#ifdef BOOTLZMA_CMP_DATA_BEFORE_ERASE
	/* 1. Read 64KB need 64KB*8bit/(100MHz*4IO)=1.28ms, so if data in flash is same as write_data, no need to erase and program */
	/* 2. Erase 4KB ~50ms, Erase 64KB ~150ms, Program 256B ~0.4ms, so if erase size > 16K, use erase block instead of erase sector. */
	u32 sector_addr = write_addr & BOOTLZMA_ERASE_SECTOR_MASK;
	u32 end_addr = write_addr + write_length;
	u32 cmp_len;

	assert_param(sector_addr == write_addr);
	while (sector_addr < end_addr) {
		cmp_len = ((end_addr - sector_addr) >= BOOTLZMA_ERASE_SECTOR_SIZE) ? BOOTLZMA_ERASE_SECTOR_SIZE : (end_addr - sector_addr);

		/* if data in flash is not same as write_data, then erase flash and update flash */
		if (0 != _memcmp((const void *)&write_data[sector_addr - write_addr], (const void *)(SPI_FLASH_BASE + sector_addr), cmp_len)) {
			/* try to erase block when need write */
			if ((sector_addr == (sector_addr & BOOTLZMA_ERASE_BLOCK_MASK)) && (sector_addr < g_last_block_addr)) {
				FLASH_Erase(EraseBlock, sector_addr);
				g_curr_block_addr = sector_addr;
			} else {
				if ((sector_addr > g_curr_block_addr) && (sector_addr < g_curr_block_addr + BOOTLZMA_ERASE_BLOCK_SIZE)) {
					/* already erased by erase block. */
				} else {
					FLASH_Erase(EraseSector, sector_addr);
				}
			}

			bootLzma_flash_write_stream(sector_addr, cmp_len, &write_data[sector_addr - write_addr]);
		}
		sector_addr += BOOTLZMA_ERASE_SECTOR_SIZE;
	}
#else
	bootLzma_flash_write_stream(write_addr, write_length, write_data);
	//FLASH_WriteStream(sector_addr, cmp_len, &write_data[sector_addr - write_addr]);
#endif
}

/**
 *  @brief bootLzma_flash_range_erase is used to erase [start_addr, start_addr + erase_size_bytes)
 *  @param u32 start_addr    	 :The starting of write address in flash.
 *  @param u32 erase_size_bytes  :The total bytes to be erased in flash
 *  @return void.
 */
static void bootLzma_flash_range_erase(u32 start_addr, u32 erase_size_bytes)
{
	u32 end_addr = start_addr + erase_size_bytes;

	u32 start_sector_addr = start_addr & BOOTLZMA_ERASE_SECTOR_MASK;
	u32 end_sector_addr = (end_addr + BOOTLZMA_ERASE_SECTOR_SIZE - 1) & BOOTLZMA_ERASE_SECTOR_MASK;

	if ((start_sector_addr != start_addr) || (end_sector_addr != end_addr)) {
		RTK_LOGE(TAG, "start_addr: 0x%x or erase_size_bytes: 0x%x not 4KB aligned.\r\n", start_addr, erase_size_bytes);
	}

	g_last_block_addr = end_sector_addr & BOOTLZMA_ERASE_BLOCK_MASK;

#ifndef BOOTLZMA_CMP_DATA_BEFORE_ERASE
	/**
	 *  erase range: [start_sector_addr, end_sector_addr)
	 *  1. start_sector_addr <= first_block_addr;
	 * 	2. last_block_addr may less than first_block_addr;
	 *	3. last_block_addr <= end_sector_addr
	 */
	u32 first_block_addr = (start_sector_addr + BOOTLZMA_ERASE_BLOCK_SIZE - 1) & BOOTLZMA_ERASE_BLOCK_MASK;
	u32 last_block_addr = end_sector_addr & BOOTLZMA_ERASE_BLOCK_MASK;
	u32 erase_addr = start_sector_addr;

	while (erase_addr < end_sector_addr) {
		if (erase_addr < first_block_addr) {
			FLASH_Erase(EraseSector, erase_addr);
			erase_addr += BOOTLZMA_ERASE_SECTOR_SIZE;
		} else {
			if (erase_addr < last_block_addr) {
				FLASH_Erase(EraseBlock, erase_addr);
				erase_addr += BOOTLZMA_ERASE_BLOCK_SIZE;
			} else {
				FLASH_Erase(EraseSector, erase_addr);
				erase_addr += BOOTLZMA_ERASE_SECTOR_SIZE;
			}
		}
	}
#endif
	/* DCache_Invalidate at last */
}

/**
 * @brief Description of bootLzma_decompress
 *
 *  bootLzma_decompress is used to
 *  -Check the LZMA file header before decompress
 *  -Decompress each LZMA file
 *  -Write to FW1 address if needed after decompression (Note: Write function is not used for first decompression)
 *
 *  @param void *adaptor      		:The pointer of the flash adaptor.
 *  @param u32 read_addr      		:The address of start of comp_file header
 *  @param u32 lzmafile_read_addr	:The address of start of first LZMA file
 *  @param u32 write_addr      		:The address of write for data about to be programmed to the flash
 *                                  :This also used as argument for HashStart addr also during Hash checking as writting to FW1 is not needed
 *  @param u16 totalFiles			:The total files of lzma available
 *  @param u8 isWriteNeeded			:Function switch to turn On/Off the write function, write is not needed during first decompression
 *
 *  @return u8 retval				:TRUE -> Decompression failure, could be Hash or LZMA; FALSE -> Decompression OK
 */
static u8 bootLzma_decompress(/* void *adaptor,*/ u32 read_addr, u32 lzmafile_read_addr, u32 write_addr, u16 totalFiles, u8 isWriteNeeded)
{
	u8 retVal = FALSE;
	u8 loop_u8 = 0U;
	u8 prop_size = ((BOOTLZMA_LZMA_PB * 5 + BOOTLZMA_LZMA_LP) * 9) + BOOTLZMA_LZMA_LC;
	u8 *p_lzma_file_size_addr = (u8 *)(read_addr + BOOTLZMA_HDR_TOTALFILES_SIZE + BOOTLZMA_HDR_HASHOVER_SIZE);
	u8 *p_curr_lzma_file_addr = (u8 *)(lzmafile_read_addr);
	u16 n_file, curr_lzmaFileSize;
	u32 outputFileSize = 0U;
	int result = 0U;
	/* LZMA stuff */
	ELzmaStatus lzma_status = LZMA_STATUS_NOT_SPECIFIED;
	SizeT srcLen;

	for (n_file = 0; n_file < totalFiles; n_file++) {
		if (n_file % (totalFiles / 4) == 0) {
			RTK_LOGI(NOTAG, "\r%% %d\r\n", n_file * 100 / totalFiles);
		}

		curr_lzmaFileSize = 0; //Reset buffer, then calculate for current LZMA file size
		for (loop_u8 = 0; loop_u8 < BOOTLZMA_HDR_FILE_SIZE; loop_u8++) {
			curr_lzmaFileSize |= p_lzma_file_size_addr[(n_file * BOOTLZMA_HDR_FILE_SIZE) + loop_u8] << (loop_u8 * 8);
		}

		/* LZMA Header Check and LZMA format check*/
		if ((p_curr_lzma_file_addr[BOOTLZMA_LZMA_FORMAT_IDENTIFIER] == 0) && (p_curr_lzma_file_addr[BOOTLZMA_LZMA_PROP_OFFSET] == prop_size)) {
			/* Get Decompressed File (output) size, byte 5 to 8, little endian */
			outputFileSize = 0; //Reset file size, reuse
			outputFileSize |= (u32)(p_curr_lzma_file_addr[BOOTLZMA_LZMA_UNCOMPRESSED_SIZE]) << 24; //MSB
			outputFileSize |= (u32)(p_curr_lzma_file_addr[BOOTLZMA_LZMA_UNCOMPRESSED_SIZE - 1]) << 16;
			outputFileSize |= (u32)(p_curr_lzma_file_addr[BOOTLZMA_LZMA_UNCOMPRESSED_SIZE - 2]) << 8;
			outputFileSize |= (u32)(p_curr_lzma_file_addr[BOOTLZMA_LZMA_UNCOMPRESSED_SIZE - 3]); //LSB

			srcLen = (curr_lzmaFileSize - LZMA_PROPS_SIZE) - BOOTLZMA_LZMA_UNCOMPRESSED_SIZE;
			/* LZMA DECODE */
			result = (int)LzmaDecode(&output_buf[0], (SizeT *)&outputFileSize,
									 &p_curr_lzma_file_addr[LZMA_PROPS_SIZE + BOOTLZMA_LZMA_UNCOMPRESSED_SIZE], &srcLen,
									 p_curr_lzma_file_addr, LZMA_PROPS_SIZE, LZMA_FINISH_ANY, &lzma_status);
			if (result != SZ_OK) {
				RTK_LOGE(TAG, "\r\n LZMA Decode Error: Error Code is %d \r\n", result);
				retVal = TRUE;
				break;
			}

			/* Preparation for next file */
			p_curr_lzma_file_addr += curr_lzmaFileSize; //offset by previous file size
			if (isWriteNeeded == TRUE) {
				/* Flash Write */
				bootLzma_flash_write(write_addr, outputFileSize, &output_buf[0]);
				write_addr = write_addr + outputFileSize;
			}
		} else {
			RTK_LOGE(TAG, "\r\n LZMA Header Error: Data is not in LZMA Format \r\n");
			retVal = TRUE;
			break;
		}
	}

	return retVal;
}

/**
  * @brief  The LZMA algorithm splits the source file into N 16KB segments, compresses each segment individually, and then concatenates the compressed files in sequence.
  * @param  read_addr： The address of compressed file
  * @param  write_addr_st: start address of decompressed file
  * @param  write_addr_end: end address of decompressed file
  * @retval 0 is successful
  */
u8 bootLzma_main_function(u32 read_addr, u32 write_addr_st, u32 write_addr_end)
{
	u8 *p_curr_header_addr;
	u8 loop_u8, retVal = TRUE;
	u16 totalFiles = 0U;
	u32 lzmafile_read_addr;

	u32 write_allowed_size = write_addr_end - write_addr_st;
	write_addr_st -= SPI_FLASH_BASE;

	/* Step 1. decode LZMA Firmware Header: LZMA_File_CNT(2B) + HashAddr(2B) + LZMA_File_CNT * LZMA_Compressed_File_Size(2B)  */
	p_curr_header_addr = (u8 *)(read_addr);
	for (loop_u8 = 0; loop_u8 < BOOTLZMA_HDR_TOTALFILES_SIZE; loop_u8++) {
		totalFiles |= p_curr_header_addr[loop_u8] << (loop_u8 * 8);
	}
#if BOOTLZMA_HDR_HASHOVER_SIZE
	u16 hashStartAddr = 0U;
	for (loop_u8 = 0; loop_u8 < BOOTLZMA_HDR_HASHOVER_SIZE; loop_u8++) {
		hashStartAddr |= p_curr_header_addr[BOOTLZMA_HDR_TOTALFILES_SIZE + loop_u8] << (loop_u8 * 8);
	}
#endif

	lzmafile_read_addr = read_addr + (BOOTLZMA_HDR_TOTALFILES_SIZE + BOOTLZMA_HDR_HASHOVER_SIZE + (totalFiles *
									  BOOTLZMA_HDR_FILE_SIZE)); //offset by header size to reach actual LZMA header

	/* Step 2. avoid decompressed file overflow */
	if (totalFiles * BOOTLZMA_DECOMP_FILE_SIZE <= write_allowed_size) {
		/* !!! the last lzma file may only 4KB, here use 16KB to calculate, actual size can get from manifest when decompressed.!!! */
		bootLzma_flash_range_erase(write_addr_st, totalFiles * BOOTLZMA_DECOMP_FILE_SIZE);
	} else {
		RTK_LOGE(TAG, "OTA file is too big\r\n");
		return retVal;
	}

	/* Step 3. check if hash check ok */
	// if (bootLzma_decompress(read_addr, lzmafile_read_addr, hashStartAddr, totalFiles, FALSE) != FALSE) {
	//    DBG_8195A("decompress error \r\n");
	//    return retVal;
	// }

	/* Step 4. 2nd Decompression and write decompressed data */
	retVal = bootLzma_decompress(read_addr, lzmafile_read_addr, write_addr_st, totalFiles, TRUE);

	/* invalid physical address and logical address in flash */
	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	return retVal;
}

/* output buffer need 16KB, Lzma_StaticProbs need 16256 Byte */
void bootLzma_buffer_set(u8 *bd_ram_addr)
{
	output_buf = bd_ram_addr;
}
