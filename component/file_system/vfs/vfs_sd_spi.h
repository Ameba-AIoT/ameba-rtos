/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef VFS_FATFS_SD_SPI_API_H
#define VFS_FATFS_SD_SPI_API_H


/** @brief SD_Command_Index
  */
typedef enum {
	SD_SPI_CMD_GoIdleSte           = 0,
	EMMC_SPI_CMD_SendOpCond        = 1,   // CMD only for EMMC
	SD_SPI_CMD_AllSendCid          = 2,
	SD_SPI_CMD_SendRelAddr         = 3,
	SD_SPI_CMD_SetDsr              = 4,
	SD_SPI_CMD_SwitchFunc          = 6,
	SD_SPI_CMD_SetBusWidth         = 6,   // ACMD6
	SD_SPI_CMD_SelDeselCard        = 7,
	SD_SPI_CMD_SendIfCond          = 8,   // CMD only for SD card
	EMMC_SPI_CMD_SendExtCsd        = 8,   // CMD only for EMMC
	SD_SPI_CMD_SendCsd             = 9,
	SD_SPI_CMD_SendCid             = 10,
	SD_SPI_CMD_VolSwitch           = 11,  // CMD only for SD card
	SD_SPI_CMD_StopXsmission       = 12,
	SD_SPI_CMD_SendSts             = 13,
	SD_SPI_CMD_SetBlklen           = 16,
	SD_SPI_CMD_RdSingleBlk         = 17,
	SD_SPI_CMD_RdMulBlk            = 18,
	SD_SPI_CMD_SendTuningBlk       = 19,  // CMD only for SD card
	SD_SPI_CMD_SendNumWrBlks       = 22,  // ACMD22
	SD_SPI_CMD_SetBlkCnt           = 23,
	SD_SPI_CMD_SetWrBlkEraseCnt    = 23,  // ACMD23
	SD_SPI_CMD_WrBlk               = 24,
	SD_SPI_CMD_WrMulBlk            = 25,
	SD_SPI_CMD_ProgCsd             = 27,
	SD_SPI_CMD_EraseBlkSt          = 32,  // CMD only for SD card
	SD_SPI_CMD_EraseBlkEd          = 33,  // CMD only for SD card
	EMMC_SPI_CMD_EraseAddrSt       = 35,  // CMD only for EMMC
	EMMC_SPI_CMD_EraseAddrEd       = 36,  // CMD only for EMMC
	SD_SPI_CMD_Erase               = 38,
	SD_SPI_CMD_SdSendOpCond        = 41,  // ACMD41 cmd only for SD card
	SD_SPI_CMD_SendScr             = 51,  // ACMD51 cmd only for SD card
	SD_SPI_CMD_AppCmd              = 55,  // CMD only for SD card
	SD_SPI_CMD_ReadOcr             = 58
} SD_SPI_COMMAND;
/**
  * @}
  */

/** @brief SD_SPI_RESULT Definition
  */
typedef enum {
	SD_SPI_OK = 0,
	SD_SPI_NODISK,
	SD_SPI_INSERT,
	SD_SPI_INIT, // about to initialize card
	SD_SPI_INITERR,
	SD_SPI_PROTECTED,
	SD_SPI_ERROR,
} SD_SPI_RESULT;
/**
  * @}
  */

/** @defgroup SD_Block_Size
  * @{
  */
#define SD_SPI_BLOCK_SIZE					512    //Bytes
/**
  * @}
  */

_LONG_CALL_ SD_SPI_RESULT sd_spi_get_status(void);
_LONG_CALL_ SD_SPI_RESULT sd_spi_device_init(void);
_LONG_CALL_ SD_SPI_RESULT sd_spi_device_deinit(void);
_LONG_CALL_ SD_SPI_RESULT sd_spi_read_data(uint8_t *buf, uint32_t sector, uint32_t cnt);
_LONG_CALL_ SD_SPI_RESULT sd_spi_write_data(uint8_t *buf, uint32_t sector, uint32_t cnt);
_LONG_CALL_ SD_SPI_RESULT sd_spi_get_sector_count(uint32_t *sector_count);

#endif
