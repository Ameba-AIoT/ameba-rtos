/* SPDX-License-Identifier: Apache-2.0 */
/*
 * REALFS - UI-oriented embedded file system
 *
 * O(1) hash-bucket file lookup + extent data mapping + head-in-inode small-file
 * storage + A/B copy-on-write metadata commit for power-loss safety.
 *
 * This header + realfs.c form the portable core (the "single source of truth",
 * see design doc section 10.3): they depend only on <stdint.h>/<string.h> and
 * the block-device abstraction in realfs_bdev.h, so the exact same code compiles
 * into firmware and into PC tools.  All board/RTOS specific includes must stay
 * out of this file (guard with REALFS_PC_BUILD if ever needed).
 *
 * On-disk layout (block_size = bs, typically 4096; N = block_count):
 *
 *   Block 0          Super(A)
 *   Block 1..M       Bitmap(A)         M = ceil(N/8/bs)
 *   Block 1+M..      data region       [data_start, data_end)
 *   Block N-1-M..N-2 Bitmap(B)
 *   Block N-1        Super(B)
 */
#ifndef REALFS_H
#define REALFS_H

#include <stdint.h>
#include "realfs_bdev.h"
#include "realfs_erasedev.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ---- compile-time configuration ------------------------------------------ */

/* Largest logical block size the static buffers are sized for. */
#ifndef REALFS_MAX_BLOCK_SIZE
#define REALFS_MAX_BLOCK_SIZE     4096u
#endif

/* Largest number of hash buckets; bucket_map is held in RAM (4 B each). */
#ifndef REALFS_MAX_BUCKETS
#define REALFS_MAX_BUCKETS        256u
#endif

/* Maximum number of bitmap blocks (M = ceil(N/8/bs)).  With bs=4096 this
 * covers up to 1024*8*4096 = 32 GB.  Stored as a bitset in bm_dirty[]:
 * 1024 bits = 128 bytes.  Increase if larger media are needed. */
#ifndef REALFS_MAX_BITMAP_BLOCKS
#define REALFS_MAX_BITMAP_BLOCKS  1024u
#endif

/* Soft cap on extents per file.  Contiguous allocations merge into one extent,
 * so this only binds when free space is heavily fragmented.  If a write returns
 * REALFS_ERR_FRAGMENTED the filesystem needs defragmentation. */
#ifndef REALFS_MAX_EXTENTS
#define REALFS_MAX_EXTENTS        20u
#endif

/* Longest overflow chain a single bucket may have (each link is one block). */
#ifndef REALFS_MAX_CHAIN
#define REALFS_MAX_CHAIN          32u
#endif

/* Longest file name (path) in bytes, excluding any terminator. */
#define REALFS_MAX_NAME           252u

/* Upper bound on the number of bitmap runs touched by a single transaction:
 * new data extents + inode + a full bucket-chain COW (and likewise for frees). */
#define REALFS_MAX_RUNS           (REALFS_MAX_EXTENTS + REALFS_MAX_CHAIN + 8)

#define REALFS_VERSION            3u   /* v3: SB persists the dirty-bitmap (§9.5) */
#define REALFS_BLOCK_EMPTY        0xFFFFFFFFu   /* "no block" sentinel */

/* ---- error codes (0 = ok, negative = error) ------------------------------ */
#define REALFS_OK                  0
#define REALFS_ERR_IO             (-1)   /* block device read/write failed */
#define REALFS_ERR_CORRUPT        (-2)   /* bad magic / CRC / structure */
#define REALFS_ERR_NOT_FOUND      (-3)   /* path does not exist */
#define REALFS_ERR_NO_SPACE       (-4)   /* not enough free blocks */
#define REALFS_ERR_INVAL          (-5)   /* bad argument */
#define REALFS_ERR_NAME           (-6)   /* name too long / empty */
#define REALFS_ERR_FRAGMENTED     (-7)   /* needs more than REALFS_MAX_EXTENTS runs */
#define REALFS_ERR_NO_MEM         (-8)   /* hash_buckets > REALFS_MAX_BUCKETS, chain too long */

/* A contiguous run of blocks [start, start+count). */
struct realfs_run {
	uint32_t start, count;
};

/* ---- open-metadata cache (speeds repeated opens, e.g. UI asset reload) ----
 * Caches parsed inode metadata keyed by path so a repeat open skips the hash
 * bucket read + inode read (2 SD I/O).  Fixed byte budget; LRU eviction when
 * full.  Set REALFS_META_CACHE_BYTES to 0 to disable (no RAM, no code effect).
 * Entries are only stored for files whose path fits and whose extent list fits
 * (larger ones simply aren't cached; open still works, just uncached). */
#ifndef REALFS_META_CACHE_BYTES
#define REALFS_META_CACHE_BYTES   8192u
#endif
#ifndef REALFS_CACHE_EXTENTS
#define REALFS_CACHE_EXTENTS      4u     /* files with more extents: not cached */
#endif
#ifndef REALFS_CACHE_PATH_MAX
#define REALFS_CACHE_PATH_MAX     64u    /* longer paths: not cached */
#endif

struct realfs_meta_entry {
	uint32_t path_hash;                       /* FNV-1a of path (fast pre-filter) */
	uint32_t inode_block;
	uint32_t file_size;
	uint32_t lru_seq;                         /* larger = more recently used */
	uint16_t head_size;
	uint16_t extent_count;
	uint8_t  in_use;
	uint8_t  path_len;
	struct realfs_run extents[REALFS_CACHE_EXTENTS];
	char     path[REALFS_CACHE_PATH_MAX];
};

#if REALFS_META_CACHE_BYTES > 0
#define REALFS_META_CACHE_ENTRIES \
    (REALFS_META_CACHE_BYTES / sizeof(struct realfs_meta_entry))
#endif

/* ---- in-RAM mount state (fixed size, independent of file count) ---------- */
struct realfs {
	const struct realfs_bdev *bdev;

	uint32_t bs;                /* logical block size */
	uint32_t block_count;       /* N */
	uint32_t hash_buckets;      /* power of two, <= REALFS_MAX_BUCKETS */
	uint32_t data_start;        /* first data block (inclusive) */
	uint32_t data_end;          /* one past last data block (exclusive) */
	uint32_t alloc_hint;        /* next-fit allocation cursor */
	uint32_t free_block_count;
	uint32_t seq;               /* sequence number of the active superblock */

	uint32_t bitmap_blocks;     /* M */
	uint32_t bitmap_start_act;  /* bitmap start block of the active group */
	uint32_t bitmap_start_ina;  /* bitmap start block of the inactive group */
	uint32_t sb_block_act;      /* superblock block of active group (0 or N-1) */
	uint32_t sb_block_ina;

	uint32_t bucket_map[REALFS_MAX_BUCKETS];  /* bucket_map[i] = block of bucket i */

	/* cached active-bitmap block used while scanning for free space */
	int32_t  bm_cache_idx;      /* which bitmap block (0..M-1) is cached, -1 = none */
	uint8_t  bm_cache[REALFS_MAX_BLOCK_SIZE];

	/* Dirty-bitmap optimisation: tracks which bitmap blocks in the inactive
	 * group are stale vs. the active group and must be rewritten at commit.
	 * Initialised to all-ones on mount (assume inactive is fully stale); after
	 * each commit reset to only the blocks touched by that transaction so
	 * subsequent commits skip the unchanged blocks.  Each bit i represents
	 * inactive bitmap block i (0 = clean, 1 = stale). */
	uint8_t  bm_dirty[(REALFS_MAX_BITMAP_BLOCKS + 7) / 8];

	/* scratch buffers used by metadata operations (never by bulk file data).
	 * The superblock is staged in io_buf at commit/format time (io_buf is idle
	 * then), so no dedicated sb buffer is needed. */
	uint8_t  io_buf[REALFS_MAX_BLOCK_SIZE];
	uint8_t  io_buf2[REALFS_MAX_BLOCK_SIZE];

	/* per-transaction pending bitmap changes (applied at commit) */
	struct realfs_run set_runs[REALFS_MAX_RUNS];
	struct realfs_run clr_runs[REALFS_MAX_RUNS];
	int n_set;
	int n_clr;

	/* per-transaction pending bucket_map update (one file per transaction) */
	int      pending_bucket_idx;     /* -1 = none */
	uint32_t pending_bucket_block;   /* new head block (or REALFS_BLOCK_EMPTY) */

#if REALFS_META_CACHE_BYTES > 0
	struct realfs_meta_entry meta_cache[REALFS_META_CACHE_ENTRIES];
	uint32_t meta_lru_clock;
#endif

	/* ---- NAND erase-aware mode (NULL / 0 in NOR/SD mode) ----------------
	 * All fields below are zero-initialised for NOR/SD instances and have
	 * no effect on those code paths.  In NAND mode edev != NULL and the
	 * bdev pointer is ignored.                                             */
	const struct realfs_erasedev *edev;  /* NULL = NOR/SD mode */
	uint16_t *ea_lbt;   /* [vblk] -> (seg<<6)|page_in_seg; 0xFFFF=unmapped */
	uint8_t  *ea_live;  /* [seg]  live page count (0..seg_pages)            */
	uint8_t  *ea_sst;   /* [seg]  EA_SEG_FREE / OPEN / SEALED             */
	uint8_t  *ea_rd_cache;      /* one block: most-recently-read vblk data  */
	uint32_t  ea_rd_cache_vblk; /* vblk in ea_rd_cache; 0xFFFFFFFF = none */
	uint32_t  ea_free_segs;     /* count of EA_SEG_FREE data segments     */
	uint8_t   ea_meta_op;       /* 1 during delete/rename: bypass vblk reserve */
	uint32_t  ea_open_seg;    /* index of the current open segment           */
	uint32_t  ea_page_cur;    /* next page to write inside ea_open_seg     */
	uint32_t  ea_seq;         /* global monotonic page sequence counter      */
	/* superblock ring: two fixed segments at the end of the address space    */
	uint32_t  ea_sb_seg[2];   /* indices of the two SB-ring segments         */
	uint32_t  ea_sb_active;   /* which of sb_seg[] is the current write seg  */
	uint32_t  ea_sb_cursor;   /* next page slot in ea_sb_seg[ea_sb_active] */
};

/* ---- open file handle ----------------------------------------------------- */
struct realfs_file {
	struct realfs *fs;
	uint32_t inode_block;
	uint32_t file_size;
	uint16_t name_len;
	uint16_t extent_count;
	uint16_t head_size;
	uint8_t  from_cache;    /* 1 = extents[] served from cache, inode_buf NOT loaded */
	struct realfs_run extents[REALFS_MAX_EXTENTS];  /* parsed at open; read uses these */
	uint8_t  inode_buf[REALFS_MAX_BLOCK_SIZE];   /* full inode block (head data lives here) */
};

struct realfs_stat {
	uint32_t size;
	uint32_t extent_count;
	uint32_t inode_block;
};

/* ---- streaming write context ---------------------------------------------- */
/* Allows writing large files without buffering the entire content in RAM.
 * Data is staged REALFS_STREAM_STAGING_BLOCKS blocks at a time and flushed with
 * one multi-block write per fill; the inode + superblock COW commit are deferred
 * to realfs_write_end().  Power loss before write_end leaves orphaned data
 * blocks reclaimed as free on the next mount (like realfs_write pre-commit).
 *
 * REALFS_STREAM_STAGING_BLOCKS: blocks buffered before a flush.  Bigger = fewer,
 * larger block-device writes (much faster on SD/eMMC where a single big
 * WRITE_MULTIPLE_BLOCK beats many small ones) at the cost of RAM
 * (K * block_size per open write handle).  Valid 1..100; out of range -> 1. */
#ifndef REALFS_STREAM_STAGING_BLOCKS
#define REALFS_STREAM_STAGING_BLOCKS  4u
#endif
#if (REALFS_STREAM_STAGING_BLOCKS >= 1u) && (REALFS_STREAM_STAGING_BLOCKS <= 100u)
#define REALFS_STAGING_BLOCKS  REALFS_STREAM_STAGING_BLOCKS
#else
#define REALFS_STAGING_BLOCKS  1u
#endif

/* Flush granularity is chosen at runtime from the block size (see
 * staging_blocks() in realfs.c): large blocks (SD/eMMC/NOR-direct 4KB, NAND 2KB)
 * flush REALFS_STAGING_BLOCKS at a time (one big multi-block device write);
 * small blocks (NOR@512B map) flush REALFS_STAGING_BLOCKS_SMALL so one flush
 * fills a whole 4KB NOR erase segment (8 x 512B pages -> one batched seg_prog).
 * Both fit the fixed staging buffer: 8*512 = 4KB <= REALFS_STAGING_BLOCKS*4096. */
#ifndef REALFS_STAGING_BLOCKS_SMALL
#define REALFS_STAGING_BLOCKS_SMALL  8u
#endif

struct realfs_write_ctx {
	struct realfs    *fs;
	char              path[REALFS_MAX_NAME + 1];
	uint32_t          plen;
	struct realfs_run runs[REALFS_MAX_EXTENTS]; /* inode extent list (merged) */
	int               nruns;
	uint32_t          total_written;
	uint32_t          inode_block;
	uint32_t          saved_free_count;         /* restored on abort */
	uint8_t           staging[REALFS_STAGING_BLOCKS * REALFS_MAX_BLOCK_SIZE];
	uint32_t          staging_used;
	int               active;
};

/* ---- API ------------------------------------------------------------------ */

/* Create a fresh file system on bdev.  hash_buckets must be a power of two and
 * <= REALFS_MAX_BUCKETS (and the bucket_map must fit in one block), OR 0 to let
 * realfs auto-size it from the partition (recommended): each bucket block costs
 * a data block, so on small partitions too many buckets waste space -- see
 * realfs_default_buckets(). */
int realfs_format(const struct realfs_bdev *bdev, uint32_t hash_buckets);

/* Bucket count realfs would auto-pick for a partition of `data_blocks` data
 * blocks at block size `bs`.  Exposed so callers/tools can preview it; passing
 * 0 to realfs_format() applies the same value. */
uint32_t realfs_default_buckets(uint32_t bs, uint32_t data_blocks);

/* Mount/unmount.  fs is caller-provided storage (large; usually heap or BSS). */
int realfs_mount(struct realfs *fs, const struct realfs_bdev *bdev);
int realfs_unmount(struct realfs *fs);

/* Open an existing file for reading.  fh is caller-provided. */
int realfs_open(struct realfs *fs, const char *path, struct realfs_file *fh);
int realfs_close(struct realfs_file *fh);

/* Read [offset, offset+length) into buf.  Returns bytes read (>=0) or error. */
int realfs_read(struct realfs_file *fh, void *buf, uint32_t offset, uint32_t length);

/* Create the file or atomically replace it if it already exists. */
int realfs_write(struct realfs *fs, const char *path, const void *data, uint32_t size);

/* Streaming write: open a write transaction (allocates inode block). */
int  realfs_write_begin(struct realfs *fs, const char *path,
						struct realfs_write_ctx *ctx);
/* Feed up to `len` bytes; full blocks are written to flash immediately.
 * Sub-block remainder is buffered in ctx->staging until the next call. */
int  realfs_write_chunk(struct realfs_write_ctx *ctx,
						const void *buf, uint32_t len);
/* Flush staging, write inode, COW-commit superblock.  Frees no memory. */
int  realfs_write_end(struct realfs_write_ctx *ctx);
/* Abort without commit; orphaned blocks reclaimed on next mount. */
void realfs_write_abort(struct realfs_write_ctx *ctx);

/* Delete a file. */
int realfs_delete(struct realfs *fs, const char *path);

/* Stat a file without keeping a handle open. */
int realfs_stat(struct realfs *fs, const char *path, struct realfs_stat *st);

/* ---- shared primitives (exposed for the PC tool format self-check, sec 10.2) */
uint32_t realfs_crc32(const void *data, uint32_t len);   /* standard zlib CRC-32 */
uint32_t realfs_fnv1a(const char *str);                  /* FNV-1a 32-bit, no NUL */

/* ---- erase-aware (route-B) mode -------------------------------------------
 * Shared by raw NAND (page 2048 B) and NOR@512B map mode (page 512 B).  Every
 * physical page carries a 16 B header; block_size = page_size - 16 B and is
 * derived at format/mount from edev->page_size (2032 B on NAND, 496 B on NOR).
 * block_size is generally not a power of two; the erase-aware path relaxes the
 * is_pow2 guard.  REALFS_EA_* below are the NAND geometry / buffer-max
 * defaults; the runtime paths use fs->bs and fs->edev->page_size instead.
 */
#define REALFS_EA_PAGE_SIZE  2048u   /* max physical page size (buffer sizing) */
#define REALFS_EA_PH_SIZE    16u     /* page header size (bytes), all geometries */
#define REALFS_EA_BLOCK_SIZE (REALFS_EA_PAGE_SIZE - REALFS_EA_PH_SIZE) /* 2032 */

/* Page header offsets within the 16 B header */
#define EA_PH_MAGIC  0   /* 4 B: "RPAG"                      */
#define EA_PH_VBLK   4   /* 4 B: logical block#; 0xFFFFFFFF = free page */
#define EA_PH_SEQ    8   /* 4 B: global monotonic counter    */
#define EA_PH_CRC    12  /* 4 B: crc32 over [0,12)           */

/* Segment (erase unit) states */
#define EA_SEG_FREE    0  /* erased, ready for allocation     */
#define EA_SEG_OPEN    1  /* currently being written          */
#define EA_SEG_SEALED  2  /* fully written, GC candidate      */
#define EA_SEG_SB_RING 3  /* reserved for superblock ring     */

/* LBT entry encoding: uint16_t, (seg<<6)|page_in_seg, 0xFFFF = unmapped.
 * Supports up to 1023 segments × 63 pages (covers ≤128MB with 62-page segs). */
#define EA_LBT_NONE  0xFFFFu

/* NAND API (edev must remain valid for the lifetime of the mount) */
int realfs_ea_format(const struct realfs_erasedev *edev, uint32_t hash_buckets);
int realfs_ea_mount(struct realfs *fs, const struct realfs_erasedev *edev);
int realfs_ea_unmount(struct realfs *fs);

/* Diagnostic: print NAND segment/GC accounting via a printf-style sink.
 * Always available (small); pf gets one line at a time. NAND mode only. */
void realfs_ea_dump(struct realfs *fs, int (*pf)(const char *fmt, ...));

/* ---- debug / inspection (compiled only when REALFS_ENABLE_DEBUG) ---------- */
#ifdef REALFS_ENABLE_DEBUG
/* printf-style output sink so the core keeps no stdio/board dependency:
 * PC tools pass printf, firmware passes a DiagPrintf-compatible wrapper. */
typedef int (*realfs_printf_t)(const char *fmt, ...);

/* All read-only; reuse fs scratch buffers, so call only when the FS is idle
 * (between operations), never concurrently with read/write/delete. */
void realfs_dump_super(struct realfs *fs, realfs_printf_t pf);    /* geometry + free space */
void realfs_dump_buckets(struct realfs *fs, realfs_printf_t pf);  /* bucket chains / distribution */
void realfs_dump_files(struct realfs *fs, realfs_printf_t pf);    /* every file + physical extents */
void realfs_dump_bitmap(struct realfs *fs, realfs_printf_t pf);   /* visual used/free block map */
void realfs_dump_all(struct realfs *fs, realfs_printf_t pf);      /* all of the above + check */
int  realfs_check(struct realfs *fs, realfs_printf_t pf);         /* consistency check; returns #problems */

/* One entry passed to the realfs_foreach() callback.  `name` is NUL-terminated
 * and valid only for the duration of the call (copy it if you need to keep it). */
struct realfs_dirent {
	const char *name;
	uint32_t    size;
	uint32_t    extent_count;
	uint32_t    inode_block;
};
/* Return 0 to continue enumeration, non-zero to stop early. */
typedef int (*realfs_iter_cb)(const struct realfs_dirent *ent, void *user);

/* Enumerate every file (read-only).  The callback MAY call realfs_open/read on
 * the same fs (e.g. to copy files out): foreach re-reads its bucket block after
 * each callback, so such reuse of the shared scratch buffers is safe.  Returns
 * the number of files visited, or a negative REALFS_ERR_* on I/O/corruption. */
int  realfs_foreach(struct realfs *fs, realfs_iter_cb cb, void *user);
#endif

#ifdef __cplusplus
}
#endif

#endif /* REALFS_H */
