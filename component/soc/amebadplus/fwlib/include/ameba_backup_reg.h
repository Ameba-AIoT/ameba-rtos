/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_BACKUP_REG_H_
#define _AMEBA_BACKUP_REG_H_

/** @addtogroup AmebaD_Platform
  * @{
  */

/** @defgroup BKUP_REG
  * @brief BKUP_REG driver modules
  * @{
  */

/** @addtogroup BKUP_REG
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * backup register size:
  *		-16bytes (4 dwords)
  *
  * usage:
  *		- user can use this registers to save some data before reset happens
  *
  * backup register can not be reset by following functions:
  *		- cpu reset
  *		- system reset
  *		- soc sleep mode
  *
  * backup register will be reset by following functions:
  *		- soc deep sleep mode
  *		- soc power down reset
  *		- soc power off
  *
  * system defined bits (other bits are reserved for user):
  *		- dword0[9]: this is SW set bit before reboot, for uart download
  *		- dword0[10]: this is SW set bit before reboot, for uart download debug
  *		- dword0[27:24]: this is bootcnt for roll-back
  *		- dword0[30]: boot version, 0 for OTA1, 1 for OTA2
  *		- dword0[31]: set when bootcnt > 3, means default version cannot work
  *
  *****************************************************************************************
  * how to use
  *****************************************************************************************
  *		BKUP_Write: write a dword backup register
  *		BKUP_Read: read a dword backup register
  *		BKUP_Set: set 1 to some bits of backup register
  *		BKUP_Clear: set 0 to some bits of backup register
  *****************************************************************************************
  * @endverbatim
  */

/**************************************************************************//**
 * @defgroup AMEBAD_BACKUP_REG
 * @{
 * @brief AMEBAD_BACKUP_REG Register Declaration
 * @note 16 Bytes total
 *****************************************************************************/
typedef struct {
	__IO uint32_t DWORD[4]; /* 0xE0 */
} BACKUP_REG_TypeDef;
/** @} */

/* Exported constants --------------------------------------------------------*/

/** @defgroup BKUP_REG_Exported_Constants BKUP_REG Exported Constants
  * @{
  */

/** @defgroup BKUP_REG_Idx_definitions
  * @{
  */
#define BKUP_REG0				((u32)0x00000000)	/*!< Bit[31,30,27-24,10,9] is used by system */
#define BKUP_REG1				((u32)0x00000001)	/*!< all bits can be used by user */
#define BKUP_REG2				((u32)0x00000002)	/*!< all bits can be used by user */
#define BKUP_REG3				((u32)0x00000003)	/*!< all bits can be used by user */
#define IS_BKUP_REG(IDX)	(((IDX) == BKUP_REG0) || \
							((IDX) == BKUP_REG1) ||\
							((IDX) == BKUP_REG2) ||\
							((IDX) == BKUP_REG3))
/**
  * @}
  */
/**
  * @}
  */


/** @defgroup BKUP_REG_Exported_Functions BKUP_REG Exported Functions
  * @{
  */
_LONG_CALL_ void BKUP_Write(u32 DwordIdx, u32 WriteVal);
_LONG_CALL_ u32 BKUP_Read(u32 DwordIdx);
_LONG_CALL_ void BKUP_Set(u32 DwordIdx, u32 BitMask);
_LONG_CALL_ void BKUP_Clear(u32 DwordIdx, u32 BitMask);
/**
  * @}
  */

/* Registers Definitions --------------------------------------------------------*/
/**************************************************************************//**
 * @defgroup BKUP_REG_Register_Definitions BKUP_REG Register Definitions
 * @{
 *****************************************************************************/

/**************************************************************************//**
* @defgroup BKUP_REG_WDORD0 REG_LP_BOOT_REASON0
* @{
*****************************************************************************/
/*BOOT_OTA_INFO for roll-back*/
/* User should set BOOT_OTA_INFO to zero when bootloader img can work normally */
#define BKUP_BIT_BOOT_CNT_ERR				BIT(31)                   /*!<R/W 0  Set when bootcnt > BOOT_CNT_TRY_LIMIT, means newer bootloader cannot work */
#define BKUP_BIT_BOOT_VER_NUM				BIT(30)                   /*!<R/W 0  boot version, 0 for OTA1, 1 for OTA2 */
#define BKUP_MASK_BOOT_CNT					((u32)0x0000000F << 24)   /*!<R/W 0  [27:24] bootcnt value */
#define BKUP_BOOT_CNT(x)						((u32)(((x) & 0x0000000F) << 24))
#define BKUP_GET_BOOT_CNT(x)					((u32)(((x >> 24) & 0x0000000F)))

#define BKUP_BIT_UARTBURN_DEBUG				BIT(10)	/*!<  this is SW set bit before reboot, for uart download debug */
#define BKUP_BIT_UARTBURN_BOOT				BIT(9)	/*!<  this is SW set bit before reboot, for uart download */

#define BKUP_MASK_UARTBURN_BOOT				0
/** @} */
/** @} */


#define BOOT_CNT_TOTAL_LIMIT			0x06
#define BOOT_CNT_TRY_LIMIT				0x3

/**
  * @}
  */

/**
  * @}
  */

#endif //_RTL8710B_BACKUP_REG_H_