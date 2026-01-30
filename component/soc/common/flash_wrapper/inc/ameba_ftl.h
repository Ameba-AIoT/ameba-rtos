/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_FTL_H_
#define _AMEBA_FTL_H_

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"
#include "ameba_ftl_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

#define NAND_ONFI_MFG_LEN				12U
#define NAND_ONFI_MODEL_LEN				20U

/* Exported macros -----------------------------------------------------------*/

#define FTL_MEMSET	_memset
#define FTL_MEMCPY	_memcpy

/* Exported types ------------------------------------------------------------*/

struct __PACKED ameba_flash_info {
	/* For both NOR and NAND */
	u8  mid;
	u8  did;
	u8  ext_did;

	/* For NAND only */
	u8	mfg[NAND_ONFI_MFG_LEN];			/* Manufacture, ONFI parameter page 32-43 */
	u8	model[NAND_ONFI_MODEL_LEN];		/* Device model, ONFI parameter page 44-63 */
	u32 page_size;			/* Bytes per page, ONFI parameter page 80-83 */
	u16 oob_size;			/* OOB bytes per page, ONFI parameter page 84-85 */
	u32 pages_per_block;		/* Pages per erase block, ONFI parameter page 92-95 */
	u32 blocks_per_lun;		/* Blocks per logical unit, ONFI parameter page 96-99 */
	u8  luns_per_target;		/* Logical units per target, ONFI parameter page 100 */
	u16 max_bad_blocks_per_lun;	/* Max bad blocks per logical unit, ONFI parameter page 103-104 */
	u8  request_host_ecc_level;	/* Required host ECC bits, ONFI parameter page 112 */

	u8  targets;			/* targets/dies */
	u32 capacity;			/* capacity */

	u8	reserved[6];		/* Padding to 64 bytes */
};

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void ftl_enable_4byte_address_mode(bool status);

#ifdef __cplusplus
}
#endif

#endif // _AMEBA_FTL_H_

