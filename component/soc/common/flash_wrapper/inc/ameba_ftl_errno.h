/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_FTL_ERRNO_H_
#define _AMEBA_FTL_ERRNO_H_

enum ftl_errno {
	ERR_NONE = 0x00,		/* No error*/
	ERR_FAIL = 0xE0,		/* General failure */
	ERR_BUSY,				/* Device busy */
	ERR_TIMEOUT,			/* Timeout*/
	ERR_HARDWARE,			/* Hardware error */
	ERR_FULL,				/* Buffer full*/
	ERR_INVALID,			/* Invalid command or parameter */
	ERR_LENGTH,				/* Invalid frame length*/
	ERR_CHECKSUM,			/* Checksum error*/
	ERR_ADDRESS,			/* Unsupported address or address overrange */
	ERR_NAND_BAD_BLOCK,		/* NAND bad block*/
	ERR_NAND_WORN_BLOCK,	/* NAND wore block */
	ERR_NAND_BITFLIP_WARN,	/* NAND bitflips < ECC level, corrected */
	ERR_NAND_BITFLIP_ERROR,	/* NAND bitflips equals to ECC level, corrected */
	ERR_NAND_BITFLIP_FATAL,	/* NAND bitflips higher than ECC level, cannot be corrected*/
};

#endif

