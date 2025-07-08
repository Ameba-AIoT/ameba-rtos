/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CHIPINFO_H_
#define _AMEBA_CHIPINFO_H_

/*Green2: Memory define
[31:28]: None
[27:24]: None
[23:20]: None
[19:16]: PSRAM Type(APM OR WB); FLASH type;Single die
[15:12]: DQ8 or DQ16
[11:8]: PSRAM total size
[7:4]: DEV CLK
[3:0]: PSRAM page size
*/

/** @defgroup Memory_Type
  * @{
  */
enum Mem_Type {
	MEM_PSRAM_WB = 0x10000,
	MEM_PSRAM_APM = 0x20000,
	MEM_FLASH_3V3 = 0x40000,
	MEM_SINGLE_DIE = 0x80000,
	MEM_TYPE_UNKNOWN = 0xF0000,
};
/**
  * @}
  */
#define Mem_Type_Get(x)	((x) & 0xF0000)

/** @defgroup CHIP_MEMORY_Type_define
  * @{
  */
enum CHIP_MEMORY_type {
	MEMORY_MCM_PSRAM = 0,
	MEMORY_ONE_FLASH,
	MEMORY_TWO_FLASH,
	MEMORY_SINGLE_DIE,
	MEMORY_NOT_CLEAR,
};
/**
  * @}
  */


/** @defgroup CHIPINFO_TypeDef
  * @{
  */
typedef struct {
	u8 sub_num;
	u8 package_num;
	u16 bd_num;
	u32 memory_type;
} CHIPINFO_TypeDef;
/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup HWINFO_Exported_Functions HWINFO Exported Functions
  * @{
  */
_LONG_CALL_ u8 EFUSE_GetChipVersion(void);
_LONG_CALL_ u16 ChipInfo_BDNum(void);
_LONG_CALL_ u8 ChipInfo_PSRAMType(void);
_LONG_CALL_ u8 ChipInfo_MemoryType(void);
_LONG_CALL_ bool ChipInfo_PsramExists(void);
_LONG_CALL_ u32 ChipInfo_PsramBoundary(void);
_LONG_CALL_ u32 ChipInfo_MCM_Info(void);
_LONG_CALL_ void ChipInfo_GetSocName_ToBuf(char *buf, size_t buflen);
/**
  * @}
  */

#endif
