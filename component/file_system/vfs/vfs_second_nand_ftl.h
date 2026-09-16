/**
  ******************************************************************************
  * @file    vfs_second_nand_ftl.h
  * @brief   Public interface of the external (second) SPI NAND FTL.
  *
  * Deliberately free of the NAND device structs: vfs_nand_common.h and
  * vfs_nand_ftl.h declare same-named types for the external and the on-chip
  * NAND respectively, so a translation unit that talks to both (the littlefs
  * adapter, vfs.c) must include this header instead of vfs_nand_common.h.
  ******************************************************************************
  */

#ifndef _VFS_SECOND_NAND_FTL_H
#define _VFS_SECOND_NAND_FTL_H

#include "basic_types.h"

/* Geometry published by SECOND_NAND_FTL_Init(), read from the ONFI parameter page. */
extern u32 vfs_second_nand_pagesize;
extern u32 vfs_second_nand_pagenum;
extern u32 vfs_second_nand_capacity;

/* Probe the device and discover its geometry.  Idempotent.
 * Returns HAL_OK on success. */
u8 SECOND_NAND_FTL_Init(void);

/* Bring-up self-test: erase the block containing test_page, program a known
 * pattern into test_page, read it back and log the result via RTK_LOGS.
 * Destroys the contents of that block. Returns 0 on byte-exact read-back. */
u8 SECOND_NAND_FTL_SelfTest(u32 test_page);

#endif /* _VFS_SECOND_NAND_FTL_H */
