/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_NAND_MFG_FTL_H_
#define _AMEBA_NAND_MFG_FTL_H_

/* Includes ------------------------------------------------------------------*/

#include "ameba_nand_ftl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exported defines ----------------------------------------------------------*/

#define NAND_MFG_DOSILICON  0xE5U
#define NAND_MFG_GIGADEVICE 0xC8U
#define NAND_MFG_MACRONIX   0xC2U
#define NAND_MFG_MICRON     0x2CU
#define NAND_MFG_WINBOND    0xEFU

/* Exported types ------------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

u8 nand_ftl_mfg_init(struct nand_ftl_device *nand);

#ifdef __cplusplus
}
#endif

#endif // _AMEBA_NAND_MFG_FTL_H_

