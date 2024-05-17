/******************************************************************************
 *
 * Copyright(c) 2007 - 2012 Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/
#include "ameba.h"
#include "build_info.h"

#define FLASHDATALEN 2048
BOOT_RAM_BSS_SECTION volatile u8  FlashDataBuf[FLASHDATALEN];//#define FLASH_DATA_BUF 	0x00086000
BOOT_RAM_BSS_SECTION volatile u32 *pFlashDatSrc;
BOOT_RAM_BSS_SECTION volatile u32 FlashBlockWriteSize;    //The maximum size of each block write is FLASHDATALEN,
//The actual value MUST be given by GDB.
BOOT_RAM_BSS_SECTION volatile u32 FlashAddrForWrite;      //The flash address to be written.
//The actual value MUST be given by GDB.
BOOT_RAM_BSS_SECTION volatile u8  FlashWriteComplete;
BOOT_RAM_BSS_SECTION volatile u32 FlashDatSrc;
BOOT_RAM_BSS_SECTION u32 erase_sector_addr = 0;
BOOT_RAM_BSS_SECTION u32 FlashWriteResult = TRUE;

extern u8 __rom_bss_start__[];
extern u8 __rom_bss_end__[];
extern u8 __ram_start_table_start__[];

extern void RtlSpiNandInit(void);
extern u32 RtlSpiNandProgram(u32 StartAddr, u32 DataLen, u8 *pData);

void RtlFlashProgram(void)__attribute__((optimize("O0")));

void BOOT_RAM_TEXT_SECTION
RtlFlashProgram(void)
{
	volatile u32 FlashWriteCnt = 0;
	u32 BootSelect = SYSCFG_BootFromNor();

	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	SCB_DisableDCache();

	//2 Need Modify
	WDG_Refresh(IWDG_DEV);
	InterruptDis(UART_LOG_IRQ);
	DBG_8195A("==========================================================\n");
	DBG_8195A("Flash Downloader Build Time: "UTS_VERSION"\n");
	DBG_8195A("==========================================================\n");


	FlashDatSrc = (u32)&FlashDataBuf;
	pFlashDatSrc = (u32 *)&FlashDataBuf;
	FlashWriteComplete = 0;
	FlashBlockWriteSize = 0;
	FlashAddrForWrite = 0;
	erase_sector_addr = 0;

	//Cache_Enable(DISABLE);
	//RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK, DISABLE);
	/*Switch SPIC clock source*/
	RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_XTAL);

	DBG_8195A("Flash init start\n");
	if (BootSelect) {
		/* Nor Flash, already initialized when boot */
	} else {
		RtlSpiNandInit();
	}
	DBG_8195A("Flash init done\n");

	DBG_8195A("Flash download start\n");
	//4 Program the flash from memory data
	while (1) {
StartOfFlashBlockWrite:
		asm("nop");
		asm("nop");
		asm("nop");
		asm("nop");

		WDG_Refresh(IWDG_DEV);

		FlashWriteResult = TRUE;
		FlashWriteCnt = 0;
		pFlashDatSrc = (u32 *)&FlashDataBuf[0];
		if (FlashWriteComplete == 1) {
			break;
		}

		if (BootSelect) {
			while (FlashWriteCnt < FlashBlockWriteSize) {
				u32 sector_addr1 = FlashAddrForWrite  & 0xFFFFF000; /* sector of first byte */

				if (sector_addr1 >= erase_sector_addr) {
					FLASH_Erase(EraseSector, sector_addr1);
					erase_sector_addr = sector_addr1 + 0x1000; /* next sector we should erase */
				}

				FLASH_TxData(FlashAddrForWrite, 4, (u8 *)pFlashDatSrc);

				FlashAddrForWrite += 4;
				FlashWriteCnt += 4;
				pFlashDatSrc += 1;
			}
		} else {
			FlashWriteResult = RtlSpiNandProgram(FlashAddrForWrite, FlashBlockWriteSize, (u8 *)FlashDataBuf);
			if (FALSE == FlashWriteResult) {
				DBG_8195A("Flash download Error\n");
			}
			FlashAddrForWrite += FlashBlockWriteSize;
		}
		goto StartOfFlashBlockWrite;
	}

	DBG_8195A("Flash download done\n");

	while (1) {
		if (FlashWriteComplete == 1) {
			FlashWriteComplete = 0;
		}
	}
}
