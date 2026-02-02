/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_DATA_FLASH_H_
#define _AMEBA_DATA_FLASH_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Exported functions --------------------------------------------------------*/
/** @defgroup FLASH_Exported_Functions FLASH Exported Functions
  * @{
  */

/** @defgroup FLASH_Normal_Functions FLASH Normal Functions
  * @{
  */
_LONG_CALL_ void DATA_FLASH_Erase(u32 EraseType, u32 Address);
_LONG_CALL_ void DATA_FLASH_SetStatus(u8 Cmd, u32 Len, u8 *Status);
_LONG_CALL_ void DATA_FLASH_SetStatusBits(u32 SetBits, u32 NewState);
_LONG_CALL_ void DATA_FLASH_WaitBusy_InUserMode(u32 WaitType);
_LONG_CALL_ void DATA_FLASH_TxCmd_InUserMode(u8 cmd, u8 DataPhaseLen, u8 *pData);
_LONG_CALL_ void DATA_FLASH_Addr4ByteEn(void);

_LONG_CALL_ void DATA_FLASH_TxCmd(u8 cmd, u8 DataPhaseLen, u8 *pData);
_LONG_CALL_ void DATA_FLASH_RxCmd(u8 cmd, u32 read_len, u8 *read_data);
_LONG_CALL_ void DATA_FLASH_StructInit(FLASH_InitTypeDef *FLASH_InitStruct);
_LONG_CALL_ void DATA_FLASH_StructInit_Micron(FLASH_InitTypeDef *FLASH_InitStruct);
_LONG_CALL_ void DATA_FLASH_StructInit_MXIC(FLASH_InitTypeDef *FLASH_InitStruct);
_LONG_CALL_ void DATA_FLASH_StructInit_GD(FLASH_InitTypeDef *FLASH_InitStruct);
_LONG_CALL_ u8 DATA_FLASH_Init(u8 SpicBitMode);
_LONG_CALL_ void DATA_FLASH_Wait_Boot_Finish(void);
_LONG_CALL_ void DATA_FLASH_SetSpiMode(SPIC_TypeDef *spi_flash, FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode);
_LONG_CALL_ void DATA_FLASH_DeepPowerDown(u32 NewState);
_LONG_CALL_ void DATA_FLASH_TxData(u32 StartAddr, u32 DataPhaseLen, u8 *pData);
_LONG_CALL_ void DATA_FLASH_RxData(u8 cmd, u32 StartAddr, u32 read_len, u8 *read_data);

_LONG_CALL_ u32 DATA_FLASH_CalibrationPLL_GetSrc(void);
_LONG_CALL_ void DATA_FLASH_Calibration_PLL_Open(void);
_LONG_CALL_ void DATA_FLASH_Calibration_PLL_Close(void);

_LONG_CALL_ void DATA_FLASH_Read_HandShake_Cmd(u32 Dphy_Dly_Cnt, u32 NewState);
_LONG_CALL_ u32 DATA_FLASH_Read_HandShake(FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode, u8 Dphy_Dly_Cnt);
_LONG_CALL_ u32 DATA_FLASH_Read_DataIsRight(void);

_LONG_CALL_ void DATA_FLASH_ClockSwitch(u32 Source, u32 Protection);
_LONG_CALL_ int DATA_FLASH_WriteStream(u32 address, u32 len, u8 *data);
_LONG_CALL_ void data_flash_highspeed_setup(void);
_LONG_CALL_ void DATA_FLASH_UserMode_Enter(void);
_LONG_CALL_ void DATA_FLASH_UserMode_Exit(void);

_LONG_CALL_ void DATA_FLASH_RxCmdXIP(u8 cmd, u32 read_len, u8 *read_data);
_LONG_CALL_ void DATA_FLASH_SetStatusXIP(u8 Cmd, u32 Len, u8 *Status);
_LONG_CALL_ void DATA_FLASH_SetStatusBitsXIP(u32 SetBits, u32 NewState);
_LONG_CALL_ void DATA_FLASH_TxDataXIP(u32 StartAddr, u32 DataPhaseLen, u8 *pData);
_LONG_CALL_ void DATA_FLASH_EraseXIP(u32 EraseType, u32 Address);
_LONG_CALL_ int DATA_FLASH_ReadStream(u32 address, u32 len, u8 *pbuf);
_LONG_CALL_ int DATA_FLASH_WriteStream(u32 address, u32 len, u8 *pbuf);
/**
  * @}
  */

/** @defgroup FLASH_XIP_Functions FLASH XIP Functions
  * @note These functions will lock cpu when exec to forbit XIP, and flush cache after exec.
  * @{
  */
void DATA_FLASH_Write_Lock(void);
void DATA_FLASH_Write_Unlock(void);
void DATA_FLASH_RxCmdXIP(u8 cmd, u32 read_len, u8 *read_data);
void DATA_FLASH_SetStatusXIP(u8 Cmd, u32 Len, u8 *Status);
void DATA_FLASH_SetStatusBitsXIP(u32 SetBits, u32 NewState);
void DATA_FLASH_TxData12BXIP(u32 StartAddr, u8 DataPhaseLen, u8 *pData);
void DATA_FLASH_TxData256BXIP(u32 StartAddr, u32 DataPhaseLen, u8 *pData);
void DATA_FLASH_EraseXIP(u32 EraseType, u32 Address);
void DATA_FLASH_Write_IPC_Int(void *Data, u32 IrqStatus, u32 ChanNum);
/**
  * @}
  */

/**
  * @}
  */

/** @} */

/** @} */

/* Other definitions --------------------------------------------------------*/
extern FLASH_InitTypeDef data_flash_init_para;
extern u32 SPIC_CALIB_PATTERN[2];

#ifdef __cplusplus
}
#endif

#endif
