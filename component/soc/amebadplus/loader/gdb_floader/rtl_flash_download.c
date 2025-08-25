/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba.h"
#include "build_info.h"

#define FLASHDATALEN 2048
volatile u8  FlashDataBuf[FLASHDATALEN];//#define FLASH_DATA_BUF 	0x00086000
volatile u32 FlashBlockWriteSize;    //The maximum size of each block write is FLASHDATALEN, and the actual value MUST be given by GDB.
volatile u32 FlashAddrForWrite;      //The flash address to be written, and the actual value MUST be given by GDB.
volatile u8  FlashWriteComplete;
volatile u32 FlashDatSrc;
volatile u32 FlashNextEraseAddr = 0;

void RtlFlashProgram(void)__attribute__((optimize("O0")));

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

void RtlFlashProgram(void)
{
	u32 FlashWriteCnt, sector_addr, tx_len;

	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	SCB_DisableDCache();

	//2 Need Modify
	WDG_Refresh(IWDG_DEV);
	InterruptDis(UART_LOG_IRQ);
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "==========================================================\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Flash Downloader Build Time: "UTS_VERSION"\n");
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "==========================================================\n");

	FlashDatSrc = (u32)&FlashDataBuf;
	FlashWriteComplete = 0;
	FlashBlockWriteSize = 0;
	FlashAddrForWrite = 0;
	FlashNextEraseAddr = 0;

	RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_XTAL); // Switch SPIC clock source
	FLASH_Read_HandShake_Cmd(0, DISABLE); // disable flash calib
	FLASH_DeepPowerDown(DISABLE);
	if (flash_init_para.FLASH_addr_phase_len == ADDR_4_BYTE) {
		FLASH_Addr4ByteEn();
	}

	/* Nor Flash, already initialized when boot */

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Flash download start\n");
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

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Flash download done\n");
	Gdb_Floader_Program_End();
}
