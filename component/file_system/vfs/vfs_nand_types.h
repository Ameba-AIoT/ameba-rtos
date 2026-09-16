/**
  ******************************************************************************
  * @file    vfs_nand_types.h
  * @brief   Device/geometry types shared by every NAND FTL in this component.
  *
  * Both the on-chip SPIC NAND (vfs_nand_ftl.h) and the external SPI-master NAND
  * (vfs_nand_common.h) describe the same silicon concepts, so the structs live
  * here once.  They used to be duplicated in those two headers with subtle
  * differences (MfgOps const-ness, a missing ReadParameterPage member, literal
  * vs macro array bounds), which only stayed safe because no translation unit
  * ever included both -- a fragile invariant.  This header is the single
  * definition; it deliberately declares no functions and pulls in no driver.
  ******************************************************************************
  */

#ifndef _VFS_NAND_TYPES_H
#define _VFS_NAND_TYPES_H

#include "basic_types.h"
#include "vfs_nand_err.h"

/* ONFI parameter-page field widths (bytes) */
#define NAND_ONFI_MFG_LEN			12U
#define NAND_ONFI_MODEL_LEN			20U

/* Manufacturer IDs (first RDID byte) */
#define NAND_MFG_DOSILICON			0xE5U
#define NAND_MFG_GIGADEVICE			0xC8U
#define NAND_MFG_MACRONIX			0xC2U
#define NAND_MFG_MICRON				0x2CU
#define NAND_MFG_WINBOND			0xEFU

#define FTL_MEMSET	_memset
#define FTL_MEMCPY	_memcpy

#ifndef __IO
#define __IO	 	volatile
#endif

#ifndef _PACKED_
#if defined (__ICCARM__)
#define _PACKED_	__packed
#else
#define _PACKED_	__attribute__ ((packed))
#endif
#endif

/* Per-page wear verdict derived from the ECC status (see NF_GetPageWornStatus) */
typedef enum {
	NAND_PAGE_WORN_OK = 0,
	NAND_PAGE_WORN_WARN = 1,
	NAND_PAGE_WORN_ERROR = 2,
	NAND_PAGE_WORN_FATAL = 3,
} NAND_FTL_PageWornStatusDef;

/* Chip identity + geometry, mostly decoded from the ONFI parameter page */
typedef struct {
	/* For both NOR and NAND */
	u8  MID;
	u8  DID;
	u8  ExtDID;

	/* For NAND only */
	u8	MFG[NAND_ONFI_MFG_LEN];			/* Manufacture, ONFI parameter page 32-43 */
	u8	Model[NAND_ONFI_MODEL_LEN];		/* Device model, ONFI parameter page 44-63 */
	u32 PageSize;						/* Bytes per page, ONFI parameter page 80-83 */
	u16 OobSize;						/* OOB bytes per page, ONFI parameter page 84-85 */
	u32 PagesPerBlock;					/* Pages per erase block, ONFI parameter page 92-95 */
	u32 BlocksPerLun;					/* Blocks per logical unit, ONFI parameter page 96-99 */
	u8  LunsPerTarget;					/* Logical units per target, ONFI parameter page 100 */
	u16 MaxBadBlocksPerLun;				/* Max bad blocks per logical unit, ONFI parameter page 103-104 */
	u8  ReqHostEccLevel;				/* Required host ECC bits, ONFI parameter page 112 */

	u8  Targets;						/* Targets/dies */
	u32 Capacity;						/* Capacity */

	u8	Reserved[6];					/* Padding to 64 bytes */
} _PACKED_ Flash_InfoTypeDef;

/* One instance per physical NAND, so the on-chip and the external device each
 * keep their own state (that is what makes using both at once possible). */
typedef struct {
	Flash_InfoTypeDef MemInfo;
	const void *MfgOps;                 /* -> NAND_FTL_MfgOpsTypeDef */
	__IO u8 CurTarget;
	__IO u8 Initialized;
	__IO u32 LastErasedBlockAddr;
} NAND_FTL_DeviceTypeDef;

/* Manufacturer-specific hooks.  ReadParameterPage is optional: the on-chip FTL
 * reads the ONFI page through its own path and leaves it NULL. */
typedef struct {
	u8(* Init)(NAND_FTL_DeviceTypeDef *nand);
	u8(* SelectTarget)(NAND_FTL_DeviceTypeDef *nand, u8 target);
	u8(* GetEccStatus)(NAND_FTL_DeviceTypeDef *nand, u8 status);
	u8(* ReadParameterPage)(NAND_FTL_DeviceTypeDef *nand, u8 *data);
} NAND_FTL_MfgOpsTypeDef;

#endif /* _VFS_NAND_TYPES_H */
