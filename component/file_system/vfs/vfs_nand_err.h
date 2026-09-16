/**
  ******************************************************************************
  * @file    vfs_nand_err.h
  * @brief   Status codes shared by every NAND HAL in this component.
  *
  * Kept separate from vfs_nand_ftl.h (on-chip SPIC NAND) and vfs_nand_common.h
  * (external SPI NAND) so that device-agnostic code - LBM above all - can speak
  * the protocol without pulling in either device's structs.
  ******************************************************************************
  */

#ifndef _VFS_NAND_ERR_H
#define _VFS_NAND_ERR_H

/* Bad-block marker values in the spare area */
#define NF_BAD_BLOCK				0x00U
#define NF_GOOD_BLOCK				0xFFU

#define UERR_INIT					0x20U /* Init error */
#define UERR_PERM					0x21U /* Operation not permitted */
#define UERR_PROTO					0x22U /* Protocol error */
#define UERR_CHK					0x23U /* Check error */
#define UERR_OVERRANGE				0x24U /* Operation overrange */
#define UERR_NAND_BAD_BLOCK			0x25U /* NAND bad block */
#define UERR_NAND_WORN_BLOCK		0x26U /* NAND wore block */
#define UERR_NAND_BITFLIP_WARN		0x27U /* NAND bitflips < ECC level, corrected */
#define UERR_NAND_BITFLIP_ERROR		0x28U /* NAND bitflips equals to ECC level, corrected */
#define UERR_NAND_BITFLIP_FATAL		0x29U /* NAND bitflips higher than ECC level, cannot be corrected */

#endif /* _VFS_NAND_ERR_H */
