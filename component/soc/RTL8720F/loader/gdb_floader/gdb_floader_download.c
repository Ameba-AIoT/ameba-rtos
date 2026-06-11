/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba.h"
#include "build_info.h"

#define FLASHDATALEN 2048
volatile u8  FlashDataBuf[FLASHDATALEN];
volatile u8  FlashDataBuf_check[FLASHDATALEN];
volatile u32 FlashBlockWriteSize;    //The maximum size of each block write is FLASHDATALEN,
volatile u32 FlashAddrForWrite;      //The flash address to be written.
volatile u8  FlashWriteComplete;
volatile u32 FlashDatSrc;
volatile u32 FlashNextEraseAddr;

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

void Gdb_Floader_Program(void)
{
	u32 FlashWriteCnt, sector_addr, tx_len;

	uint32_t irq_status = irq_disable_save();
	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);

	WDG_Refresh(IWDG_DEV);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "==========================================================\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Flash Downloader Build Time: "UTS_VERSION"\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "==========================================================\n");

	FlashDatSrc = (u32)&FlashDataBuf;
	FlashWriteComplete = 0;
	FlashAddrForWrite = 0;
	FlashBlockWriteSize = 0;
	RCC_PeriphClockSourceSet(SPIC, XTAL); // Switch SPIC clock source

	FLASH_Read_HandShake_Cmd(0, DISABLE); // disable flash calib
	FLASH_DeepPowerDown(DISABLE);
	if (flash_init_para.FLASH_addr_phase_len == ADDR_4_BYTE) {
		FLASH_Addr4ByteEn();
	}

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Flash download start\n");

	//4 Program the flash from memory data
	while (1) {
		Gdb_Floader_Program_Start();
		if (FlashWriteComplete == 1) {
			break;
		}

		FlashWriteCnt = 0;
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

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Flash download done\n");
	irq_enable_restore(irq_status);
	Gdb_Floader_Program_End();
}
