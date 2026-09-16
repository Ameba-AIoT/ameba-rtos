/**
  ******************************************************************************
  * @file    lbm.h
  * @brief   LBM - Logical Block Mapping: a thin flash-translation layer for
  *          littlefs over SPI NAND (Tier 1).
  *
  * Presents a contiguous, bad-block-free logical block device on top of the
  * NAND HAL: logical blocks are mapped to physical blocks, bad blocks are
  * masked, runtime bad-block replacement is transparent. Clean-room design;
  * the on-flash layout is an original format, no third-party source is used.
  *
  * See the thin-FTL design document under component/file_system/ for details.
  ******************************************************************************
  */

#ifndef _LBM_H
#define _LBM_H

#include <stdint.h>

/* Return codes (negative = error) */
#define LBM_OK            0
#define LBM_ERR_IO        (-1)   /* generic read/parameter error */
#define LBM_ERR_BAD       (-2)   /* bad-block replacement failed / pool exhausted */
#define LBM_ERR_NOSPACE   (-3)   /* no free physical block left to allocate */
#define LBM_ERR_INIT      (-4)   /* scan/geometry error */

/* On-flash header magics ("LBM0" / "LBM1") */
#define LBM_META_HDR_MAGIC  0x304D424CUL
#define LBM_BLK_HDR_MAGIC   0x314D424CUL
#define LBM_HDR_VERSION     1

/* Physical-block runtime states */
#define LBM_BLK_DIRTY     0   /* needs erase + meta header before use (blank/stale) */
#define LBM_BLK_FREE      1   /* erased, meta header present, ready for a block header */
#define LBM_BLK_USED      2   /* mapped to a logical block */
#define LBM_BLK_BAD       3   /* permanently retired */

/* Meta header: physical-block page 0, first bytes (rest of page padded 0xFF) */
typedef struct __attribute__((packed))
{
	uint32_t magic;          /* LBM_META_HDR_MAGIC */
	uint8_t  version;        /* LBM_HDR_VERSION */
	uint8_t  pad[3];
	uint64_t ec;             /* erase counter */
	uint32_t blk_hdr_page;   /* = 1 */
	uint32_t data_page;      /* = 2 */
	uint32_t hdr_crc;        /* crc32 over preceding bytes */
}
lbm_meta_hdr_t;

/* Block header: physical-block page 1, first bytes (rest of page padded 0xFF) */
typedef struct __attribute__((packed))
{
	uint32_t magic;          /* LBM_BLK_HDR_MAGIC */
	uint8_t  version;        /* LBM_HDR_VERSION */
	uint8_t  copy_flag;      /* Tier1: always 0; Tier2 WL copy in progress = 1 */
	uint8_t  pad[2];
	uint32_t lblk;           /* logical block this physical block carries */
	uint64_t seq;            /* global monotonic sequence, power-loss arbitration */
	uint32_t data_crc;       /* Tier1: 0xFFFFFFFF (unused); Tier2 fills it */
	uint32_t hdr_crc;        /* crc32 over preceding bytes */
}
lbm_blk_hdr_t;

/* NAND back-end hooks.  One instance per physical device, so the same LBM code
 * serves the on-chip SPIC NAND (vfs_nand_ftl.c) and an external SPI-master NAND
 * (vfs_second_nand_ftl.c).  Signatures mirror the NAND_FTL_* API: addr is a page
 * address, the return value is HAL_OK / UERR_*. */
typedef struct {
	uint8_t (*read_page)(uint32_t addr, uint8_t *buf);
	uint8_t (*read_page_fast)(uint32_t addr, uint8_t *buf);   /* skips bad-marker check */
	uint8_t (*write_page)(uint32_t addr, const uint8_t *buf, uint8_t do_erase);
	uint8_t (*erase_block)(uint32_t addr, uint8_t force);
	uint8_t (*mark_bad)(uint32_t addr);
} lbm_dev_ops_t;

/* LBM context (single volume, Tier 1) */
typedef struct {
	/* ---- configuration: filled in by the caller before lbm_init() ---- */
	const lbm_dev_ops_t *ops;  /* NAND back-end */
	uint32_t  cfg_base_addr;   /* byte address of the partition start on the device */
	uint32_t  cfg_size;        /* partition size in bytes */
	uint32_t  cfg_page_size;   /* bytes per NAND page */
	uint32_t  cfg_block_pages; /* pages per NAND erase block */

	/* ---- runtime state, owned by LBM ---- */
	uint8_t   inited;
	uint32_t  part_base_page;  /* absolute page addr of physical block 0 */
	uint32_t  total_blocks;    /* physical blocks in the VFS partition */
	uint32_t  block_pages;     /* pages per physical block (e.g. 64) */
	uint32_t  page_size;       /* bytes per page (e.g. 2048) */
	uint32_t  lblk_size;       /* usable data bytes = (block_pages-2)*page_size */
	uint32_t  reserved_blocks; /* spare pool kept for bad-block replacement */
	uint32_t  bad_count;
	uint32_t  avail_count;     /* FREE + DIRTY physical blocks currently allocatable */
	uint32_t  usable_blocks;   /* total - reserved (block_count cap) */
	uint32_t  alloc_cursor;    /* round-robin start for allocation */
	uint64_t  global_seq;
	int32_t  *map;             /* [lblk] -> pblk, -1 = unmapped (len total_blocks) */
	uint32_t *ec;              /* [pblk] -> erase count      (len total_blocks) */
	uint8_t  *state;           /* [pblk] -> LBM_BLK_*         (len total_blocks) */
	uint8_t  *pagebuf;         /* one page scratch (len page_size) */
} lbm_ctx_t;

/* Back-ends: on-chip SPIC NAND (vfs_nand_ftl.c) and external SPI-master NAND
 * (vfs_second_nand_ftl.c).  Only the one whose source is compiled in exists. */
extern const lbm_dev_ops_t lbm_nand_ops;
extern const lbm_dev_ops_t lbm_second_nand_ops;

/* ---- Tier 1 public API ---- */
int      lbm_init(lbm_ctx_t *ctx);                 /* scan flash, build tables */
void     lbm_deinit(lbm_ctx_t *ctx);               /* free tables */
int      lbm_block_read(lbm_ctx_t *ctx, int lblk, void *buf, uint32_t off, uint32_t len);
int      lbm_block_write(lbm_ctx_t *ctx, int lblk, const void *buf, uint32_t off, uint32_t len);
int      lbm_block_erase(lbm_ctx_t *ctx, int lblk);
uint32_t lbm_block_count(lbm_ctx_t *ctx);          /* usable logical block count */
uint32_t lbm_block_size(lbm_ctx_t *ctx);           /* bytes per logical block */

#endif /* _LBM_H */
