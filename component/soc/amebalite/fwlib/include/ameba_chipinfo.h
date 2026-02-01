/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_CHIPINFO_H_
#define _AMEBA_CHIPINFO_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup CHIPINFO_TypeDef
  * @{
  */

/** @defgroup CHIPINFO_MEMINFO_Structure_Type Structure Type
  * @{
  */


/* Struct with memory type and relevant info */
typedef struct {
	uint16_t model        : 3;	/* PSRAM: APM, WB, reserved
								 DDR: DDR2, DDR3, DDR3L, LPDDR1 */

	uint16_t density      : 5;	/* PSRAM: 32Mb, 64Mb, 128Mb, 256Mb, 512Mb
								   DDR: 512Mb, 1Gb, 2Gb */

	uint16_t dqx          : 2;	/* PSRAM: DQ8, DQ16, others reserved
								   DDR: DQ16 */
	uint16_t rsvd         : 6;

} __PACKED DRAM_INFOTypeDef;

typedef struct {
	/* Memory type: Single-die, DDR, PSRAM, NOR, NAND */
	uint16_t mem_type         : 6;	/* If multi-die exist, relevant bits are all set. */

	/* Flash size */
	uint16_t flash_density    : 6;	/* NOR: 64Mb, 128Mb, 256Mb
									   NAND: 1Gb, 2Gb */
	uint16_t rsvd0            : 4;

	/* DDR/PSRAM information */
	DRAM_INFOTypeDef dram_info;

} __PACKED MCM_MemTypeDef;

Compile_Assert(sizeof(MCM_MemTypeDef) == 4, "MCM_MemInfo size shall be 4");
/** * @}  */

typedef struct {
	u8 sub_num;
	u8 package_num;
	u16 bd_num;

	MCM_MemTypeDef mem_info;
} CHIPINFO_MEMTypeDef;

/**
  * @}
  */

#define	CHIPINFO_GET_SUBNUM(x)					((u8)(((x >> 5) & 0x00000007)))
#define	CHIPINFO_GET_PKGNUM(x)					((u8)(((x >> 0) & 0x0000001F)))

#define OTP_CHIPVER								0x7F0
#define OTP_GET_CHIP_VERSION(x)					((u32)(((x >> 0) & 0x0000001F)))
#define OTP_GET_ES_FLAG(x)						((u32)(((x >> 5) & 0x00000001)))

#define OTP_CHIPINFO							0x7FF
#define OTP_GET_PACKAGE_NUM(x)					((u32)(((x >> 0) & 0x0000001F)))
#define OTP_GET_INTERNAL_BDNUM(x)				((u32)(((x >> 5) & 0x00000007)))

#define OTP_UUID								0x7F4
#define OTP_LOT_NUM								0x7F8

/* Exported constants --------------------------------------------------------*/

/** @defgroup CHIPINFO_MEMORY_Constants CHIPINFO MEMORY Constants
  * @{
  */

/*
 * Calculate the actual memory size in Megabits (Mb).
 * If code is 0, it represents a size of 0 Mbits.
 * Otherwise, the size is calculated as 4 << (code - 1).
 * Example: For a code of 0x4, this calculates to 32 Mbits.
 */
#define MCM_MEM_SIZE_MBITS_CALC(code)		((code) == 0 ? 0 : 4 << ((code) - 1))

/*
 * Convert memory size from Megabits (Mb) to Bytes.
 * Example: If the code is 0x4, indecating in 32 Mbits, the conversion yields 0x400000 Bytes.
 */
#define MCM_MEM_SIZE_IN_BYTES(code)			(MCM_MEM_SIZE_MBITS_CALC(code) * 1024 * 1024 / 8)

/*
 * Convert memory size from Megabits (Mb) to KBytes.
 * Example: If the code is 0x4, indecating in 32 Mbits, the conversion yields 4096 KBytes.
 */
#define MCM_MEM_SIZE_IN_KBYTES(code)		(MCM_MEM_SIZE_MBITS_CALC(code) * 1024 * 1024 / 8 / 1024)

/******************* PSRAM Information Start *******************/

/* PSRAM Vendors */
#define MCM_PSRAM_VENDOR_NOTCARE   0x0  /* Vendor not specified */
#define MCM_PSRAM_VENDOR_APM       0x1  /* APM vendor */
#define MCM_PSRAM_VENDOR_WB        0x2  /* Winbond vendor */

/* PSRAM Size Definitions */
#define MCM_PSRAM_SIZE_UNKNOWN     0x0  /* Unknown size */
#define MCM_PSRAM_SIZE_4Mb         0x1  /* 4 << 0 */
#define MCM_PSRAM_SIZE_8Mb         0x2  /* 4 << 1 */
#define MCM_PSRAM_SIZE_16Mb        0x3  /* 4 << 2 */
#define MCM_PSRAM_SIZE_32Mb        0x4  /* 4 << 3 */
#define MCM_PSRAM_SIZE_64Mb        0x5  /* 4 << 4 */
#define MCM_PSRAM_SIZE_128Mb       0x6  /* 4 << 5 */
#define MCM_PSRAM_SIZE_256Mb       0x7  /* 4 << 6 */
#define MCM_PSRAM_SIZE_512Mb       0x8  /* 4 << 7 */

/* PSRAM DQ Types */
#define MCM_PSRAM_DQ_INVALID       0x0  /* Invalid or unspecified data width */
#define MCM_PSRAM_DQ8              0x1  /* 8-bit data width */
#define MCM_PSRAM_DQ16             0x2  /* 16-bit data width */

/******************* PSRAM Information End *******************/

/******************* DDR Information Start *******************/

/* DDR Size Definitions */
#define MCM_DDR_SIZE_UNKNOWN       0x0  /* Unknown size */
#define MCM_DDR_SIZE_512Mb         0x8  /* 4 << 7 */
#define MCM_DDR_SIZE_1Gb           0x9  /* 4 << 8 */
#define MCM_DDR_SIZE_2Gb           0xA  /* 4 << 9 */

/* DDR Model Types */
#define MCM_DDR_TYPE_INVALID       0x0  /* Invalid or unspecified DDR type */
#define MCM_DDR2                   0x1  /* DDR2 */
#define MCM_DDR3                   0x2  /* DDR3 */
#define MCM_DDR3L                  0x3  /* DDR3L */
#define MCM_LPDDR1                 0x4  /* LPDDR1 */

/* DDR DQ Types */
#define MCM_DDR_DQ_INVALID         0x0  /* Invalid or unspecified data width */
#define MCM_DDR_DQ16               0x1  /* 16-bit data width */

/******************* DDR Information End *******************/

/******************* Flash Information Start *******************/

/* Flash Size Definitions */
#define MCM_FLASH_SIZE_UNKNOWN     0x0  /* Unknown size */

/* NOR Flash Size Definitions */
#define MCM_NOR_FLASH_SIZE_4Mb     0x1  /* 4 << 0 */
#define MCM_NOR_FLASH_SIZE_8Mb     0x2  /* 4 << 1 */
#define MCM_NOR_FLASH_SIZE_16Mb    0x3  /* 4 << 2 */
#define MCM_NOR_FLASH_SIZE_32Mb    0x4  /* 4 << 3 */
#define MCM_NOR_FLASH_SIZE_64Mb    0x5  /* 4 << 4 */
#define MCM_NOR_FLASH_SIZE_128Mb   0x6  /* 4 << 5 */
#define MCM_NOR_FLASH_SIZE_256Mb   0x7  /* 4 << 6 */

/* NAND Flash Size Definitions */
#define MCM_NAND_FLASH_SIZE_1Gb    0x9  /* 4 << 8 */
#define MCM_NAND_FLASH_SIZE_2Gb    0xA  /* 4 << 9 */

/* Flash Types */
#define MCM_FLASH_TYPE_INVALID     0x0  /* Invalid or unspecified flash type */
#define MCM_NAND_FLASH             0x1  /* NAND Flash */
#define MCM_NOR_FLASH              0x2  /* NOR Flash */

/******************* Flash Information End *******************/

#define MCM_MEM_RSVD_INFO          0x0  /* Reserved memory information */

/** * @}  */

/**
 * @brief Initializes a memory configuration that includes DRAM only.
 *
 * This macro sets up a memory configuration where only DRAM is present.
 * Based on the type of memory, it automatically adjusts parameters
 * like `dqx`.
 *
 * @param type The memory type, such as MCM_TYPE_PSRAM or MCM_TYPE_DDR.
 * @param model The model or vendor of the DRAM, e.g., MCM_PSRAM_VENDOR_WB,
 *              MCM_PSRAM_VENDOR_APM, MCM_DDR2, MCM_DDR3, etc.
 * @param size The size of the DRAM, such as MCM_PSRAM_SIZE_128Mb.
 * @param dqx The data width of the DRAM. e.g., MCM_PSRAM_DQ8, MCM_DDR_DQ16.
 *
 * @example
 * MCM_MEM_DRAM_ONLY(MCM_TYPE_PSRAM, MCM_PSRAM_VENDOR_WB, MCM_PSRAM_SIZE_128Mb, MCM_PSRAM_DQ16)
 * MCM_MEM_DRAM_ONLY(MCM_TYPE_DDR, MCM_DDR2, MCM_DDR_SIZE_1Gb, NULL)
 */
#define MCM_MEM_DRAM_ONLY(type, model, size, dqx) \
	{                                             \
		.mem_type = type,                         \
		.flash_density = MCM_FLASH_SIZE_UNKNOWN,  \
		.rsvd0 = 0,                               \
		.dram_info =                              \
		{                                         \
			model,                                \
			size,                                 \
			dqx,                                  \
			0                                     \
		}                                         \
	}

/**
 * @brief Initializes a memory configuration that includes Flash only.
 *
 * This macro sets up a memory configuration where only Flash memory is present.
 *
 * @param type The type of flash memory, such as MCM_TYPE_NAND_FLASH
 *             or MCM_TYPE_NOR_FLASH.
 * @param size The size of the flash memory, e.g.,
 *             MCM_NAND_FLASH_SIZE_1Gb or MCM_NOR_FLASH_SIZE_1Gb.
 *
 * @return A MCM_MemTypeDef structure initialized with Flash settings.
 *
 * @example
 * MCM_MEM_FLASH_ONLY(MCM_TYPE_NAND_FLASH, MCM_NAND_FLASH_SIZE_1Gb)
 */
#define MCM_MEM_FLASH_ONLY(type, size) \
	{                                  \
		.mem_type = type,              \
		.flash_density = size,         \
		.rsvd0 = 0,                    \
		.dram_info = {0}               \
	}

/**
 * @brief Initializes a memory configuration that includes both DRAM and Flash.
 *
 * Combines DRAM and Flash memory setups into a single configuration.
 *
 * @param dram_type The DRAM type, e.g., MCM_TYPE_PSRAM or MCM_TYPE_DDR.
 * @param model The model or vendor of the DRAM.
 * @param size The size of the DRAM.
 * @param dqx The data width of the DRAM. e.g., MCM_PSRAM_DQ8, MCM_DDR_DQ16.
 * @param flash_type The type of flash memory, such as MCM_TYPE_NAND_FLASH
 *                   or MCM_TYPE_NOR_FLASH.
 * @param flash_size The size of the flash memory.
 *
 * @example
 * MCM_MEM_DRAM_AND_FLASH(MCM_TYPE_PSRAM, MCM_PSRAM_VENDOR_WB,
 *                        MCM_PSRAM_SIZE_128Mb, MCM_PSRAM_DQ16,
 *                        MCM_TYPE_NAND_FLASH, MCM_NAND_FLASH_SIZE_1Gb)
 */
#define MCM_MEM_DRAM_AND_FLASH(dram_type, model, size, dqx, flash_type, flash_size) \
	{                                                                               \
		.mem_type = dram_type | flash_type,                                         \
		.flash_density = flash_size,                                                \
		.rsvd0 = 0,                                                                 \
		.dram_info =                                                                \
		{                                                                           \
			model,                                                                  \
			size,                                                                   \
			dqx,                                                                    \
			0                                                                       \
		}                                                                           \
	}

/**
 * @brief Initializes a memory configuration representing a single die package.
 *
 * This macro identifies the chip type without specifying specific DRAM or Flash.
 *
 * @example
 * MCM_MEM_SINGLE_DIE
 */
#define MCM_MEM_SINGLE_DIE                       \
	{                                            \
		.mem_type = MCM_SINGLE_DIE,              \
		.flash_density = MCM_FLASH_SIZE_UNKNOWN, \
		.rsvd0 = 0,                              \
		.dram_info = {0}                         \
	}

/**
 * @brief Initializes a memory configuration representing a debug package.
 *
 * @example
 * MCM_MEM_DEBUG_PACKAGE
 */
#define MCM_MEM_DEBUG_PACKAGE                    \
	{                                            \
		.mem_type = MCM_TYPE_INVALID,            \
		.flash_density = MCM_FLASH_SIZE_UNKNOWN, \
		.rsvd0 = 0,                              \
		.dram_info = {0}                         \
	}

/** @defgroup CHIPINFO_MEMTYPE_Enumeration_Type Enumeration Type
  * @{
  */
enum MCM_MemType {
	MCM_TYPE_INVALID     = 0,
	MCM_SINGLE_DIE       = BIT0,   /* Single-die */
	MCM_TYPE_DDR         = BIT1,   /* DDR */
	MCM_TYPE_PSRAM       = BIT2,   /* PSRAM */
	MCM_TYPE_NOR_FLASH   = BIT3,   /* NOR */
	MCM_TYPE_NAND_FLASH  = BIT4,   /* NAND */
};
/** * @}  */

/** @defgroup EFUSE_Physical_Address_functions
  * @{
  */
_LONG_CALL_ u8 EFUSE_GetPackageInfo(void);
_LONG_CALL_ u8 EFUSE_GetBDNum(void);
_LONG_CALL_ u8 EFUSE_GetChipVersion(void);
_LONG_CALL_ u8 EFUSE_GetESFlag(void);
_LONG_CALL_ bool ChipInfo_PsramExists(void);
_LONG_CALL_ u32 ChipInfo_PsramBoundary(void);
_LONG_CALL_ void EFUSE_GetUUID(u32 *UUID);
_LONG_CALL_ u8 ChipInfo_Get(void);
_LONG_CALL_ u16 ChipInfo_BDNum(void);
_LONG_CALL_ u32 ChipInfo_GetSocName_ToBuf(void);
_LONG_CALL_ bool ChipInfo_GetChipSram(void);
_LONG_CALL_ MCM_MemTypeDef ChipInfo_MCMInfo(void);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif

