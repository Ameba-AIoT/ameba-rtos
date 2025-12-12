/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_nor_ftl.h"
#include "ameba_ftl.h"

/* Private defines -----------------------------------------------------------*/

#define FLASH_CMD_BLOCK_ERASE_32K               0x52U
#define FLASH_CMD_BLOCK_ERASE_64K               0xD8U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init NOR FTL
  * @param  None
  * @retval None
  */
void nor_ftl_init(struct ameba_flash_info *info)
{
	u8 flash_id[3];

	flash_id[0] = 0;
	flash_id[1] = 0;
	flash_id[2] = 0;

	FLASH_RxCmd(flash_init_para.FLASH_cmd_rd_id, 3, flash_id);

	info->mid = flash_id[0];
	info->did = flash_id[1];
	info->ext_did = flash_id[2];
	info->page_size = 1024;
	info->pages_per_block = 4;
	info->capacity = (u32)1U << flash_id[2];
}

/**
  * @brief  Read NOR Flash page
  * @param  addr : address
  * @param  buf : data buffer
  * @retval None
  */
void nor_ftl_read_page(u32 addr, u8 *buf)
{
	u32 i;
	u32 *p = (u32 *)buf;

	if (IS_FLASH_ADDR(addr)) {
		addr -= SPI_FLASH_BASE;
		for (i = 0; i < 1024 / 4; i++, addr += 4) {
			p[i] = HAL_READ32(SPI_FLASH_BASE, addr);
		}
	}
}

void nor_ftl_erase_block(u32 addr, u32 size)
{
	if (IS_FLASH_ADDR(addr)) {
		if ((addr == SPI_FLASH_BASE) && (size == 0xFFFFFFFF)) {
			FLASH_Erase(EraseChip, 0);
			DCache_Invalidate(SPI_FLASH_BASE, 0x10000000 - SPI_FLASH_BASE);
			return;
		}

		addr -= SPI_FLASH_BASE;

		if (size == 64 * 1024) {
			flash_init_para.FLASH_cmd_block_e = FLASH_CMD_BLOCK_ERASE_64K;
			FLASH_Erase(EraseBlock, addr);
		} else if (size == 32 * 1024) {
			flash_init_para.FLASH_cmd_block_e = FLASH_CMD_BLOCK_ERASE_32K;
			FLASH_Erase(EraseBlock, addr);
		} else {
			FLASH_Erase(EraseSector, addr);
		}

		DCache_Invalidate(SPI_FLASH_BASE + addr, size);
	}
}

/**
  * @brief  Erase NOR Flash block
  * @param  addr : address
  * @param  size : size in byte, shall be aligned to 4KB
  * @retval None
  */
void nor_ftl_erase(u32 addr, u32 size)
{
	u32 erase_addr;
	u32 size_in_4kb = (size + 4095) >> 12;

	if (IS_FLASH_ADDR(addr)) {
		if ((addr == SPI_FLASH_BASE) && (size == 0xFFFFFFFF)) {
			FLASH_Erase(EraseChip, 0);
			WDG_Refresh(IWDG_DEV);
			DCache_Invalidate(SPI_FLASH_BASE, 0x10000000 - SPI_FLASH_BASE);
		} else {
			erase_addr = (addr - SPI_FLASH_BASE) & 0xFFFFF000;
			while (size_in_4kb != 0) {
				/* Address block-aligned and the next block need to be erased */
				if (((erase_addr & 0xFFFF) == 0) && (size_in_4kb >= 16)) {
					flash_init_para.FLASH_cmd_block_e = FLASH_CMD_BLOCK_ERASE_64K;
					FLASH_Erase(EraseBlock, erase_addr);
					erase_addr += 0x10000;
					size_in_4kb -= 16;
				} else if (((erase_addr & 0x7FFF) == 0) && (size_in_4kb >= 8)) {
					flash_init_para.FLASH_cmd_block_e = FLASH_CMD_BLOCK_ERASE_32K;
					FLASH_Erase(EraseBlock, erase_addr);
					erase_addr += 0x8000;
					size_in_4kb -= 8;
				} else {
					FLASH_Erase(EraseSector, erase_addr);
					erase_addr += 0x1000;
					size_in_4kb--;
				}
				WDG_Refresh(IWDG_DEV);
			}
			DCache_Invalidate(addr, size);
		}
	}
}

/**
  * @brief  Write NOR Flash page
  * @param  addr : address
  * @param  buf : data buffer
  * @retval None
  */
void nor_ftl_write_page(u32 addr, const u8 *buf)
{
	u32 index = 0;

	/* write page shall ensure addr is page aligned. */
	assert_param((addr & 0xFF) == 0);

	if (IS_FLASH_ADDR(addr)) {
		addr -= SPI_FLASH_BASE;
		for (index = 0; index < 4; index++) {
			FLASH_TxData(addr + index * 256, 256, (u8 *)(buf + index * 256));
		}
		DCache_Invalidate(SPI_FLASH_BASE + addr, 1024U);
	}
}

/**
  * @brief  Read NOR Flash status register
  * @param  cmd : command
  * @param  size : size in byte
  * @retval Value of NOR Flash status register
  */
u16 nor_ftl_get_status(u8 cmd, u8 size)
{
	u8 status = 0;

	FLASH_RxCmd(cmd, size, &status);

	return status;
}

/**
  * @brief  Write NOR Flash status register
  * @param  cmd : command
  * @param  size : size in byte
  * @param  status : value of NOR Flash status register
  * @retval None
  */
void nor_ftl_set_status(u8 cmd, u8 size, u8 *status)
{
	FLASH_SetStatus(cmd, size, status);
}
