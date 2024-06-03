/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CHIPINFO_H_
#define _AMEBA_CHIPINFO_H_

#define MCM_PSRAM_APM		0x10000
#define MCM_PSRAM_WB		0x20000
#define MCM_FLASH_3V3		0x40000
#define MCM_FLASH_1V8		0x80000
#define EXT_FLASH_3V3		0x100000
#define EXT_FLASH_1V8		0x200000
#define MEMORY_NOT_CLEAR	0x400000


#define MEMORY_VENDOR_GET(x)	(x & 0x7F0000)
#define MCM_PSRAM_GET(x)		(x & 0x30000)
#define MCM_FLASH_GET(x)		(x & 0xC0000)


#define IS_SIP_PSRAM(x) 	(((x & (MCM_PSRAM_APM | MCM_PSRAM_WB))!=0) ? 1 : 0)
#define IS_MCM_PSRAM_WB(x) 	(((x & (MCM_PSRAM_WB)) !=0)? 1 : 0)
#define IS_MCM_PSRAM_APM(x) (((x & (MCM_PSRAM_APM)) !=0) ? 1 : 0)


#define IS_SIP_FLASH(x) 	((((x) &(MCM_FLASH_3V3 | MCM_FLASH_1V8 | EXT_FLASH_3V3 | EXT_FLASH_1V8))!=0) ? 1 : 0)
#define IS_TWO_FLASH(x) ((((x &(MCM_FLASH_3V3|MCM_FLASH_1V8))!=0) & ((x &(EXT_FLASH_3V3|EXT_FLASH_1V8))!=0)) ? 1 : 0)
#define IS_MCM_FLASH_1V8(x)	((((x) & (MCM_FLASH_1V8))==MCM_FLASH_1V8 ) ? 1 : 0)
#define IS_EXT_FLASH_1V8(x)	((((x) & (EXT_FLASH_1V8))==EXT_FLASH_1V8 ) ? 1 : 0)
#define IS_MCM_EXT_FLASH_1V8(x)	((((x) & (EXT_FLASH_1V8|MCM_FLASH_1V8))==(EXT_FLASH_1V8|MCM_FLASH_1V8) ) ?  1 : 0)


#define ChipInfo_MemoryInfo() ((MCM_PSRAM_GET(ChipInfo_MCM_Info()))==0 ? \
	((MCM_FLASH_GET(ChipInfo_MCM_Info()))==0 ? ((ChipInfo_MCM_Info())|EXT_FLASH_3V3) : ((ChipInfo_MCM_Info()))) : \
	((ChipInfo_MCM_Info())|EXT_FLASH_3V3))

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

#define	CHIPINFO_GET_SUBNUM(x)	((u8)(((x >> 5) & 0x00000007)))
#define	CHIPINFO_GET_PKGNUM(x)	((u8)(((x >> 0) & 0x0000001F)))

/* Exported functions --------------------------------------------------------*/
/** @defgroup HWINFO_Exported_Functions HWINFO Exported Functions
  * @{
  */
_LONG_CALL_ u16 ChipInfo_BDNum(void);
_LONG_CALL_ u8 ChipInfo_PSRAMType(void);
_LONG_CALL_ u32 ChipInfo_MCM_Info(void);
_LONG_CALL_ void ChipInfo_GetSocName_ToBuf(char *buf, size_t buflen);
_LONG_CALL_ void ChipInfo_GetLibVersion_ToBuf(char *buf, size_t buflen);
/**
  * @}
  */

#endif
