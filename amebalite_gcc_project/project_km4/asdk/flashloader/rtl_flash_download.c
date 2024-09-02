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
volatile u8  FlashDataBuf[FLASHDATALEN];//#define FLASH_DATA_BUF 	0x00086000
volatile u32 FlashBlockWriteSize;    //The maximum size of each block write is FLASHDATALEN, and the actual value MUST be given by GDB.
volatile u32 FlashAddrForWrite;      //The flash address to be written, and the actual value MUST be given by GDB.
volatile u8  FlashWriteComplete;
volatile u32 FlashDatSrc;
volatile u32 FlashNextEraseAddr = 0;

__NO_INLINE
void Gdb_Floader_Program_Start(void)
{
	__NOP();
	__NOP();
	__NOP();
	__NOP();
	WDG_Refresh(IWDG_DEV);
}

__NO_INLINE
void Gdb_Floader_Program_End(void)
{
	while (1) {
		if (FlashWriteComplete == 1) {
			FlashWriteComplete = 0;
		}
	}
}


void RtlFlashProgram(void)__attribute__((optimize("O0")));

void RtlFlashProgram(void)
{
	u32 FlashWriteCnt, sector_addr, tx_len;

	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	SCB_DisableDCache();

	//2 Need Modify
	WDG_Refresh(IWDG_DEV);
	InterruptDis(UART_LOG_IRQ);
	DBG_8195A("==========================================================\n");
	DBG_8195A("Flash Downloader Build Time: "UTS_VERSION"\n");
	DBG_8195A("==========================================================\n");


	FlashDatSrc = (u32)&FlashDataBuf;
	FlashWriteComplete = 0;
	FlashBlockWriteSize = 0;
	FlashAddrForWrite = 0;
	FlashNextEraseAddr = 0;

	//Cache_Enable(DISABLE);
	//RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK, DISABLE);
	RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_LBUS); // Switch SPIC clock source
	FLASH_Read_HandShake_Cmd(0, DISABLE); // disable flash calib


	/* Nor Flash, already initialized when boot */

	DBG_8195A("Flash download start\n");
	//4 Program the flash from memory data
	while (1) {
		Gdb_Floader_Program_Start();

		FlashWriteCnt = 0;
		if (FlashWriteComplete == 1) {
			break;
		}

		while (FlashWriteCnt < FlashBlockWriteSize) {
			sector_addr = FlashAddrForWrite  & 0xFFFFF000; /* sector of first byte */

			if (sector_addr >= FlashNextEraseAddr) {
				FLASH_Erase(EraseSector, sector_addr);
				FlashNextEraseAddr = sector_addr + 0x1000; /* next sector we should erase */
			}

			tx_len = MIN(0x100, FlashBlockWriteSize - FlashWriteCnt);
			FLASH_TxData(FlashAddrForWrite, tx_len, (u8 *)&FlashDataBuf[FlashWriteCnt]);

			FlashAddrForWrite += tx_len;
			FlashWriteCnt += tx_len;
		}
	}

	DBG_8195A("Flash download done\n");
	Gdb_Floader_Program_End();
}
