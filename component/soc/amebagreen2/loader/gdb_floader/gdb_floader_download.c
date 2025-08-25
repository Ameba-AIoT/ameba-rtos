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

extern void RtlSpiNandInit(void);
extern u32 RtlSpiNandProgram(u32 StartAddr, u32 DataLen, u8 *pData, u8 *FlashDataBuf_check);

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
	int FlashWriteResult;
	int boot_from_nor = SYSCFG_OTP_BootFromNor();

	__disable_irq();
	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);

	WDG_Refresh(IWDG_DEV);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "==========================================================\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Flash Downloader Build Time: "UTS_VERSION"\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "==========================================================\n");

	FlashDatSrc = (u32)&FlashDataBuf;
	FlashWriteComplete = 0;
	FlashAddrForWrite = 0;
	FlashBlockWriteSize = 0;
	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_FPGA) {
		RCC_PeriphClockSourceSet(SPIC, XTAL); // Switch SPIC clock source
	}
	FLASH_Read_HandShake_Cmd(0, DISABLE); // disable flash calib
	FLASH_DeepPowerDown(DISABLE);
	if (flash_init_para.FLASH_addr_phase_len == ADDR_4_BYTE) {
		FLASH_Addr4ByteEn();
	}

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Flash download start\n");

	// remove nand flash protection
	if (!boot_from_nor) {
		RtlSpiNandInit();
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Download Nand Flash Image \n");
		if (NAND_BL_ALL_UNLOCKED != NAND_GetStatus(NAND_REG_BLOCK_LOCK)) {
			NAND_SetStatus(NAND_REG_BLOCK_LOCK, NAND_BL_ALL_UNLOCKED);
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "protection unlocked: 0x%x\n", NAND_GetStatus(NAND_REG_BLOCK_LOCK));
		}
	}

	//4 Program the flash from memory data
	while (1) {
		Gdb_Floader_Program_Start();
		if (FlashWriteComplete == 1) {
			break;
		}

		if (boot_from_nor) {
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
		} else {
			FlashWriteResult = RtlSpiNandProgram(FlashAddrForWrite, FlashBlockWriteSize, (u8 *)FlashDataBuf, (u8 *)FlashDataBuf_check);

			if (FALSE == FlashWriteResult) {
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Flash download Error\n");
			}
			FlashAddrForWrite += FlashBlockWriteSize;
		}
	}

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "Flash download done\n");
	__enable_irq();
	Gdb_Floader_Program_End();
}

