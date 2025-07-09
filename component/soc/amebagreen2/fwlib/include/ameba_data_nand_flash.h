/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DATA_NAND_FLASH_H_
#define _AMEBA_DATA_NAND_FLASH_H_


/** @defgroup DATA_FLASH_NAND_Functions FLASH NAND Functions
  * @{
  */
void DATA_NAND_RxCmd(u8 cmd, u8 addr_len, u8 *paddr, u32 read_len, u8 *read_data);
void DATA_NAND_RxData(u8 cmd, u32 StartAddr, u32 read_len, u8 *read_data);
u8 DATA_NAND_WaitBusy(u32 WaitType);
void DATA_NAND_WriteEn(void);
void DATA_NAND_TxCmd(u8 cmd, u8 AddrLen, u8 *pAddr, u32 DataLen, u8 *pData);
void DATA_NAND_TxData(u8 cmd, u32 StartAddr, u32 ByteLen, u8 *pData);
u8 DATA_NAND_GetStatus(u8 FeatureID);
void DATA_NAND_SetStatus(u8 FeatureID, u8 Status);
void DATA_NAND_SetStatusBits(u8 FeatureID, u8 SetBits, u8 NewState);
u8 DATA_NAND_Erase(u32 PageAddr);
u8 DATA_NAND_Page_Read_ArrayToCache(u32 PageAddr);
void DATA_NAND_Page_Read_FromCache(u8 Cmd, u32 ByteAddr, u32 ByteLen, u8 *pData);
u8 DATA_NAND_Page_Read(u32 PageAddr, u32 ByteAddr, u32 ByteLen, u8 *pData);
void DATA_NAND_Page_Write_Data_Xfer(u8 Cmd, u32 ByteAddr, u32 ByteLen, u8 *pData);
u8 DATA_NAND_Page_Write_Program_Execute(u32 PageAddr);
u8 DATA_NAND_Page_Write(u32 PageAddr, u32 ByteAddr, u32 ByteLen, u8 *pData);
int DATA_NAND_GetPageSize(u32 *page_size_byte);
u32 DATA_NAND_CheckBadBlock(u32 block_id);
void DATA_SPIC_Config(FLASH_InitTypeDef *FLASH_InitStruct);
void DATA_NAND_SetSpiMode(FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode);
void DATA_NAND_StructInit_Micron(FLASH_InitTypeDef *FLASH_InitStruct);
void DATA_NAND_StructInit_MXIC(FLASH_InitTypeDef *FLASH_InitStruct);
void DATA_NAND_StructInit_GD(FLASH_InitTypeDef *FLASH_InitStruct);
void DATA_NAND_StructInit(FLASH_InitTypeDef *FLASH_InitStruct);
u8 DATA_NAND_Init(u8 SpicBitMode);
void data_nand_get_vendor(void);
/** @} */

#endif
