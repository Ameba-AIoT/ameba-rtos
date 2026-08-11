/* SPDX-License-Identifier: Apache-2.0 */
/*
 * REALFS core - see realfs.h for the design summary and on-disk layout.
 *
 * Portable single-source-of-truth implementation: depends only on the C
 * standard library (string.h) and the block device abstraction.  No RTOS or
 * board headers here so the same object compiles into firmware and PC tools.
 */
#include "realfs.h"
#include <string.h>
#include <stdlib.h>   /* malloc/free — erase-aware mode only */

/* ------------------------------------------------------------------------- */
/* On-disk field offsets (little-endian, manual packing - no struct casts).  */
/* ------------------------------------------------------------------------- */

/* Superblock (block 0 = group A, block N-1 = group B) */
#define SB_MAGIC          0   /* 8 bytes "REALFS02" */
#define SB_VERSION        8
#define SB_BLOCK_SIZE     12
#define SB_BLOCK_COUNT    16
#define SB_HASH_BUCKETS   20
#define SB_DATA_START     24
#define SB_ALLOC_HINT     28
#define SB_SEQ            32
#define SB_BITMAP_START   36
#define SB_BITMAP_BLOCKS  40
#define SB_FREE_COUNT     44
#define SB_DATA_END       48
#define SB_BUCKET_MAP     64   /* hash_buckets * 4 bytes */
/* crc-32 lives in the last 4 bytes of the block, covering [0, bs-4) */

static const char REALFS_MAGIC_STR[8] = { 'R', 'E', 'A', 'L', 'F', 'S', '0', '2' };

/* Inode (one per file, one block) */
#define IN_MAGIC          0   /* 4 bytes "INOD" */
#define IN_CRC            4    /* covers [8, bs) */
#define IN_FILE_SIZE      8
#define IN_CREATE         12
#define IN_MODIFY         16
#define IN_FLAGS          20
#define IN_NAME_LEN       22
#define IN_EXT_COUNT      24
#define IN_HEAD_SIZE      26
#define IN_RESERVED       28
#define IN_HEADER         32   /* filename starts here */

/* Hash bucket (one block) */
#define BK_REVISION       0
#define BK_ENTRY_COUNT    4
#define BK_ENTRIES        8    /* entry i: hash at 8+i*8, inode at 12+i*8 */
/* overflow_next at bs-8, crc at bs-4 (crc covers [0, bs-4)) */

/* ------------------------------------------------------------------------- */
/* Little-endian load/store helpers                                          */
/* ------------------------------------------------------------------------- */
static uint32_t ld32(const uint8_t *p, uint32_t off)
{
	const uint8_t *q = p + off;
	return (uint32_t)q[0] | ((uint32_t)q[1] << 8) |
		   ((uint32_t)q[2] << 16) | ((uint32_t)q[3] << 24);
}
static uint16_t ld16(const uint8_t *p, uint32_t off)
{
	const uint8_t *q = p + off;
	return (uint16_t)((uint16_t)q[0] | ((uint16_t)q[1] << 8));
}
static void st32(uint8_t *p, uint32_t off, uint32_t v)
{
	uint8_t *q = p + off;
	q[0] = (uint8_t)v;
	q[1] = (uint8_t)(v >> 8);
	q[2] = (uint8_t)(v >> 16);
	q[3] = (uint8_t)(v >> 24);
}
static void st16(uint8_t *p, uint32_t off, uint16_t v)
{
	uint8_t *q = p + off;
	q[0] = (uint8_t)v;
	q[1] = (uint8_t)(v >> 8);
}

static uint32_t ceil_div(uint32_t a, uint32_t b)
{
	return (a + b - 1) / b;
}

/* ------------------------------------------------------------------------- */
/* CRC-32 (standard zlib: reflected, poly 0xEDB88320, init/final 0xFFFFFFFF)  */
/* FNV-1a 32-bit.  Both pinned here so the PC tool matches byte-for-byte.     */
/* ------------------------------------------------------------------------- */
uint32_t realfs_crc32(const void *data, uint32_t len)
{
	const uint8_t *p = (const uint8_t *)data;
	uint32_t crc = 0xFFFFFFFFu;
	for (uint32_t i = 0; i < len; i++) {
		crc ^= p[i];
		for (int b = 0; b < 8; b++) {
			uint32_t mask = (uint32_t) - (int32_t)(crc & 1u);
			crc = (crc >> 1) ^ (0xEDB88320u & mask);
		}
	}
	return ~crc;
}

uint32_t realfs_fnv1a(const char *str)
{
	uint32_t h = 0x811C9DC5u;          /* FNV offset basis */
	while (*str) {                     /* hashes the path bytes, excludes NUL */
		h ^= (uint8_t) * str++;
		h *= 0x01000193u;              /* FNV prime */
	}
	return h;
}

/* ------------------------------------------------------------------------- */
/* Block device helpers (single-block convenience wrappers)                  */
/* ------------------------------------------------------------------------- */
/* Forward declarations: NAND helpers are defined at end of file. */
static uint16_t ea_lbt_get(const struct realfs *fs, uint32_t vblk);
static int  ea_read_vblk(struct realfs *fs, uint32_t vblk, void *buf);
static int  ea_write_vblk(struct realfs *fs, uint32_t vblk, const void *buf);
static int  ea_write_run(struct realfs *fs, uint32_t blk, uint32_t n, const void *buf);
static void ea_mark_dead(struct realfs *fs, uint32_t vblk);
static int  ea_find_runs(struct realfs *fs, uint32_t need,
						 struct realfs_run *out, int max_runs, int *nout);
static int  ea_commit(struct realfs *fs);
static int  ea_gc_one(struct realfs *fs);

static int rd1(struct realfs *fs, uint32_t blk, void *buf)
{
	if (fs->edev) {
		return ea_read_vblk(fs, blk, buf);
	}
	return fs->bdev->read_blocks(fs->bdev, blk, 1, buf) ? REALFS_ERR_IO : REALFS_OK;
}
static int wr1(struct realfs *fs, uint32_t blk, const void *buf)
{
	if (fs->edev) {
		return ea_write_vblk(fs, blk, buf);
	}
	return fs->bdev->write_blocks(fs->bdev, blk, 1, buf) ? REALFS_ERR_IO : REALFS_OK;
}
static int rdn(struct realfs *fs, uint32_t blk, uint32_t n, void *buf)
{
	if (fs->edev) {
		/* erase-aware mode: read n consecutive vblks one page at a time */
		for (uint32_t i = 0; i < n; i++) {
			int rc = ea_read_vblk(fs, blk + i,
								  (uint8_t *)buf + i * fs->bs);
			if (rc) {
				return rc;
			}
		}
		return REALFS_OK;
	}
	return fs->bdev->read_blocks(fs->bdev, blk, n, buf) ? REALFS_ERR_IO : REALFS_OK;
}
static int wrn(struct realfs *fs, uint32_t blk, uint32_t n, const void *buf)
{
	if (fs->edev) {
		/* Erase-aware: program consecutive vblks in one batched seg_prog per
		 * segment (see ea_write_run) instead of one flash transaction per page. */
		return ea_write_run(fs, blk, n, buf);
	}
	return fs->bdev->write_blocks(fs->bdev, blk, n, buf) ? REALFS_ERR_IO : REALFS_OK;
}
static int bdev_sync(struct realfs *fs)
{
	if (fs->edev) {
		return (fs->edev->sync && fs->edev->sync(fs->edev)) ? REALFS_ERR_IO : REALFS_OK;
	}
	return fs->bdev->sync ? (fs->bdev->sync(fs->bdev) ? REALFS_ERR_IO : REALFS_OK)
		   : REALFS_OK;
}
/* Best-effort hint to a log-structured backend that `count` blocks from `blk`
 * are now free.  No-op when the backend has no trim hook. */
static void bdev_trim(struct realfs *fs, uint32_t blk, uint32_t count)
{
	if (fs->bdev->trim) {
		fs->bdev->trim(fs->bdev, blk, count);
	}
}

/* ------------------------------------------------------------------------- */
/* Bucket block helpers (operate on a bs-byte buffer)                        */
/* ------------------------------------------------------------------------- */
static uint32_t bk_max_entry(uint32_t bs)
{
	return (bs - 16) / 8;
}

static uint32_t bk_entry_count(const uint8_t *b)
{
	return ld32(b, BK_ENTRY_COUNT);
}
static uint32_t bk_overflow(const uint8_t *b, uint32_t bs)
{
	return ld32(b, bs - 8);
}
static uint32_t bk_entry_hash(const uint8_t *b, uint32_t i)
{
	return ld32(b, BK_ENTRIES + i * 8);
}
static uint32_t bk_entry_inode(const uint8_t *b, uint32_t i)
{
	return ld32(b, BK_ENTRIES + i * 8 + 4);
}

static void bk_set_entry(uint8_t *b, uint32_t i, uint32_t hash, uint32_t inode)
{
	st32(b, BK_ENTRIES + i * 8, hash);
	st32(b, BK_ENTRIES + i * 8 + 4, inode);
}
static void bk_finalize(uint8_t *b, uint32_t bs, uint32_t entry_count,
						uint32_t overflow, uint32_t revision)
{
	st32(b, BK_REVISION, revision);
	st32(b, BK_ENTRY_COUNT, entry_count);
	st32(b, bs - 8, overflow);
	st32(b, bs - 4, realfs_crc32(b, bs - 4));
}
static int bk_valid(const uint8_t *b, uint32_t bs)
{
	return ld32(b, bs - 4) == realfs_crc32(b, bs - 4);
}

/* ------------------------------------------------------------------------- */
/* Inode helpers                                                             */
/* ------------------------------------------------------------------------- */
struct inode_info {
	uint32_t file_size;
	uint16_t name_len;
	uint16_t extent_count;
	uint16_t head_size;
	uint32_t head_off;     /* byte offset of head_data within the block */
	uint32_t extents_off;  /* byte offset of extents[] within the block */
};

static int inode_parse(const uint8_t *b, struct inode_info *ii)
{
	if (memcmp(b + IN_MAGIC, "INOD", 4) != 0) {
		return REALFS_ERR_CORRUPT;
	}
	ii->file_size    = ld32(b, IN_FILE_SIZE);
	ii->name_len     = ld16(b, IN_NAME_LEN);
	ii->extent_count = ld16(b, IN_EXT_COUNT);
	ii->head_size    = ld16(b, IN_HEAD_SIZE);
	ii->extents_off  = IN_HEADER + ii->name_len;
	ii->head_off     = ii->extents_off + (uint32_t)ii->extent_count * 8;
	return REALFS_OK;
}

static int inode_valid(const uint8_t *b, uint32_t bs)
{
	if (memcmp(b + IN_MAGIC, "INOD", 4) != 0) {
		return 0;
	}
	return ld32(b, IN_CRC) == realfs_crc32(b + 8, bs - 8);
}

static int inode_name_eq(const uint8_t *b, const char *path, uint32_t path_len)
{
	if (ld16(b, IN_NAME_LEN) != path_len) {
		return 0;
	}
	return memcmp(b + IN_HEADER, path, path_len) == 0;
}

/* ------------------------------------------------------------------------- */
/* Bitmap (active group) - on-demand cached, scanned for free runs           */
/* ------------------------------------------------------------------------- */
static int bm_load(struct realfs *fs, int32_t bm_idx)
{
	if (fs->bm_cache_idx == bm_idx) {
		return REALFS_OK;
	}
	int rc = rd1(fs, fs->bitmap_start_act + (uint32_t)bm_idx, fs->bm_cache);
	if (rc) {
		fs->bm_cache_idx = -1;
		return rc;
	}
	fs->bm_cache_idx = bm_idx;
	return REALFS_OK;
}

/* returns 1 if `block` is marked used in the active bitmap (0 if free) */
static int bm_test_used(struct realfs *fs, uint32_t block)
{
	uint32_t bpb = fs->bs * 8;
	int32_t  idx = (int32_t)(block / bpb);
	if (bm_load(fs, idx) != REALFS_OK) {
		return 1;    /* fail safe: treat as used */
	}
	uint32_t k = block - (uint32_t)idx * bpb;
	return (fs->bm_cache[k >> 3] >> (k & 7)) & 1;
}

static int in_pending_set(struct realfs *fs, uint32_t block)
{
	for (int i = 0; i < fs->n_set; i++)
		if (block >= fs->set_runs[i].start &&
			block <  fs->set_runs[i].start + fs->set_runs[i].count) {
			return 1;
		}
	return 0;
}

static int block_free(struct realfs *fs, uint32_t block)
{
	return !bm_test_used(fs, block) && !in_pending_set(fs, block);
}

/*
 * Find `need` free blocks as up to max_runs contiguous runs, scanning forward
 * from alloc_hint (wrapping once).  Pure: records nothing, just fills out[].
 * Each run stays within a single bitmap block so commit touches one block.
 */
static int find_runs(struct realfs *fs, uint32_t need, struct realfs_run *out,
					 int max_runs, int *nout)
{
	if (fs->edev) {
		return ea_find_runs(fs, need, out, max_runs, nout);
	}
	*nout = 0;
	if (need == 0) {
		return REALFS_OK;
	}
	if (fs->free_block_count < need) {
		return REALFS_ERR_NO_SPACE;
	}

	uint32_t total = fs->data_end - fs->data_start;
	uint32_t bpb   = fs->bs * 8;
	uint32_t got   = 0;
	uint32_t i     = 0;

	while (got < need && i < total) {
		uint32_t p = fs->data_start + ((fs->alloc_hint - fs->data_start + i) % total);
		if (!block_free(fs, p)) {
			i++;
			continue;
		}

		uint32_t rs = p, rl = 0, rbm = p / bpb;
		while (got + rl < need && i < total) {
			uint32_t q = fs->data_start + ((fs->alloc_hint - fs->data_start + i) % total);
			if (q != rs + rl) {
				break;    /* wrap or discontinuity */
			}
			if (q / bpb != rbm) {
				break;    /* crossed bitmap-block boundary */
			}
			if (!block_free(fs, q)) {
				break;
			}
			rl++;
			i++;
		}
		if (*nout >= max_runs) {
			return REALFS_ERR_FRAGMENTED;
		}
		out[*nout].start = rs;
		out[*nout].count = rl;
		(*nout)++;
		got += rl;
	}
	if (got < need) {
		return REALFS_ERR_FRAGMENTED;
	}
	return REALFS_OK;
}

static int commit_set(struct realfs *fs, const struct realfs_run *runs, int n)
{
	for (int i = 0; i < n; i++) {
		if (fs->n_set >= (int)REALFS_MAX_RUNS) {
			return REALFS_ERR_NO_MEM;
		}
		fs->set_runs[fs->n_set++] = runs[i];
		fs->free_block_count -= runs[i].count;
		uint32_t end = runs[i].start + runs[i].count;
		fs->alloc_hint = (end >= fs->data_end) ? fs->data_start : end;
	}
	return REALFS_OK;
}

static int add_clr(struct realfs *fs, uint32_t start, uint32_t count)
{
	if (count == 0) {
		return REALFS_OK;
	}
	if (fs->edev) {
		/* erase-aware mode: mark vblks dead immediately (no deferred bitmap). */
		for (uint32_t i = 0; i < count; i++) {
			ea_mark_dead(fs, start + i);
		}
		fs->free_block_count += count;
		return REALFS_OK;
	}
	if (fs->n_clr >= (int)REALFS_MAX_RUNS) {
		return REALFS_ERR_NO_MEM;
	}
	fs->clr_runs[fs->n_clr].start = start;
	fs->clr_runs[fs->n_clr].count = count;
	fs->n_clr++;
	fs->free_block_count += count;
	return REALFS_OK;
}

static int alloc_one(struct realfs *fs, uint32_t *blk)
{
	struct realfs_run r;
	int n, rc;
	rc = find_runs(fs, 1, &r, 1, &n);
	if (rc) {
		return rc;
	}
	rc = commit_set(fs, &r, 1);
	if (rc) {
		return rc;
	}
	*blk = r.start;
	return REALFS_OK;
}

/* ------------------------------------------------------------------------- */
/* Transaction commit: write inactive bitmap (full copy + pending changes),  */
/* then inactive superblock, then atomically flip the active group.          */
/* ------------------------------------------------------------------------- */

/* --- dirty-bitmap helpers ------------------------------------------------ */
static void bm_dirty_set(struct realfs *fs, uint32_t bm_idx)
{
	if (bm_idx < REALFS_MAX_BITMAP_BLOCKS) {
		fs->bm_dirty[bm_idx >> 3] |= (uint8_t)(1u << (bm_idx & 7));
	}
}
static int bm_dirty_test(const struct realfs *fs, uint32_t bm_idx)
{
	if (bm_idx >= REALFS_MAX_BITMAP_BLOCKS) {
		return 1;    /* safe: treat as dirty */
	}
	return (fs->bm_dirty[bm_idx >> 3] >> (bm_idx & 7)) & 1;
}
/* Mark the bitmap blocks touched by a set of runs as dirty. */
static void bm_dirty_mark_runs(struct realfs *fs,
							   const struct realfs_run *runs, int n)
{
	uint32_t bpb = fs->bs * 8;
	for (int i = 0; i < n; i++) {
		uint32_t first = runs[i].start / bpb;
		uint32_t last  = (runs[i].start + runs[i].count - 1) / bpb;
		for (uint32_t bm = first; bm <= last; bm++) {
			bm_dirty_set(fs, bm);
		}
	}
}

static void bm_apply_to_block(struct realfs *fs, uint8_t *buf, uint32_t bm_idx)
{
	uint32_t bpb  = fs->bs * 8;
	uint32_t base = bm_idx * bpb;       /* first global block in this bm block */
	for (int i = 0; i < fs->n_set; i++) {
		uint32_t s = fs->set_runs[i].start, c = fs->set_runs[i].count;
		for (uint32_t b = s; b < s + c; b++)
			if (b >= base && b < base + bpb) {
				uint32_t k = b - base;
				buf[k >> 3] |= (uint8_t)(1u << (k & 7));
			}
	}
	for (int i = 0; i < fs->n_clr; i++) {
		uint32_t s = fs->clr_runs[i].start, c = fs->clr_runs[i].count;
		for (uint32_t b = s; b < s + c; b++)
			if (b >= base && b < base + bpb) {
				uint32_t k = b - base;
				buf[k >> 3] &= (uint8_t)~(1u << (k & 7));
			}
	}
}

/* dirty: ceil(M/8)-byte inactive-group dirty bitmap to persist (v3), or NULL to
 * store all-zero (freshly formatted: both bitmap groups are identical). */
static void sb_build(struct realfs *fs, uint8_t *buf, uint32_t seq,
					 uint32_t bitmap_start, const uint8_t *dirty)
{
	memset(buf, 0, fs->bs);
	memcpy(buf + SB_MAGIC, REALFS_MAGIC_STR, 8);
	st32(buf, SB_VERSION,       REALFS_VERSION);
	st32(buf, SB_BLOCK_SIZE,    fs->bs);
	st32(buf, SB_BLOCK_COUNT,   fs->block_count);
	st32(buf, SB_HASH_BUCKETS,  fs->hash_buckets);
	st32(buf, SB_DATA_START,    fs->data_start);
	st32(buf, SB_ALLOC_HINT,    fs->alloc_hint);
	st32(buf, SB_SEQ,           seq);
	st32(buf, SB_BITMAP_START,  bitmap_start);
	st32(buf, SB_BITMAP_BLOCKS, fs->bitmap_blocks);
	st32(buf, SB_FREE_COUNT,    fs->free_block_count);
	st32(buf, SB_DATA_END,      fs->data_end);
	for (uint32_t i = 0; i < fs->hash_buckets; i++) {
		st32(buf, SB_BUCKET_MAP + i * 4, fs->bucket_map[i]);
	}
	if (dirty) {   /* persist dirty-bitmap right after bucket_map (memset already zeroed it) */
		uint32_t off = SB_BUCKET_MAP + fs->hash_buckets * 4;
		memcpy(buf + off, dirty, ceil_div(fs->bitmap_blocks, 8));
	}
	st32(buf, fs->bs - 4, realfs_crc32(buf, fs->bs - 4));
}

static int commit_txn(struct realfs *fs)
{
	if (fs->edev) {
		return ea_commit(fs);
	}

	int rc;

	/* Apply the pending bucket_map change in RAM so the new SB records it. */
	if (fs->pending_bucket_idx >= 0) {
		fs->bucket_map[fs->pending_bucket_idx] = fs->pending_bucket_block;
	}

	/* 1. Write stale inactive bitmap blocks (dirty optimisation).
	 *
	 * bm_dirty tracks which inactive bitmap blocks differ from the active
	 * group.  We only read+apply+write those blocks instead of all M.
	 *
	 * Correctness: bm_dirty is initialised to all-ones on mount (full stale),
	 * giving a full copy on the first commit.  After each flip, bm_dirty is
	 * reset to exactly the blocks changed by THIS transaction — those are the
	 * only blocks the new inactive (= old active) is missing.
	 *
	 * First compute new_dirty = bitmap blocks touched by this transaction. */
	bm_dirty_mark_runs(fs, fs->set_runs, fs->n_set);
	bm_dirty_mark_runs(fs, fs->clr_runs, fs->n_clr);

	/* Write only the dirty blocks to inactive. */
	for (uint32_t bm = 0; bm < fs->bitmap_blocks; bm++) {
		if (!bm_dirty_test(fs, bm)) {
			continue;
		}
		rc = rd1(fs, fs->bitmap_start_act + bm, fs->io_buf);
		if (rc) {
			return rc;
		}
		bm_apply_to_block(fs, fs->io_buf, bm);
		rc = wr1(fs, fs->bitmap_start_ina + bm, fs->io_buf);
		if (rc) {
			return rc;
		}
	}
	rc = bdev_sync(fs);
	if (rc) {
		return rc;
	}

	/* Recompute bm_dirty = ONLY this transaction's blocks.  After the flip the
	 * new inactive (= old active) will differ from the new active in exactly
	 * these blocks, so this is (a) what the next commit must rewrite and (b)
	 * what we persist in the SB so a remount restores it instead of assuming
	 * everything is stale (avoids the full-copy on the first post-mount commit,
	 * §9.5).  The accumulated bm_dirty above (prev-gen | this-txn) was only
	 * needed to bring the inactive group fully current in the loop just done. */
	memset(fs->bm_dirty, 0, sizeof(fs->bm_dirty));
	bm_dirty_mark_runs(fs, fs->set_runs, fs->n_set);
	bm_dirty_mark_runs(fs, fs->clr_runs, fs->n_clr);

	/* 2. Write the inactive superblock - this is the atomic commit point.
	 * io_buf is idle here (the bitmap loop above already synced), so stage the
	 * SB in it rather than keeping a dedicated sb_buf.  Embed the dirty-bitmap. */
	sb_build(fs, fs->io_buf, fs->seq + 1, fs->bitmap_start_ina, fs->bm_dirty);
	rc = wr1(fs, fs->sb_block_ina, fs->io_buf);
	if (rc) {
		return rc;
	}
	rc = bdev_sync(fs);
	if (rc) {
		return rc;
	}

	/* 3. Flip: the group we just wrote becomes active. */
	{
		uint32_t t;
		t = fs->bitmap_start_act;
		fs->bitmap_start_act = fs->bitmap_start_ina;
		fs->bitmap_start_ina = t;
		t = fs->sb_block_act;
		fs->sb_block_act     = fs->sb_block_ina;
		fs->sb_block_ina     = t;
	}
	fs->seq += 1;

	/* 3b. The commit is now durable: release trimmed blocks. */
	for (int i = 0; i < fs->n_clr; i++) {
		bdev_trim(fs, fs->clr_runs[i].start, fs->clr_runs[i].count);
	}

	/* 4. bm_dirty already holds this transaction's blocks (computed above) -
	 * exactly the new inactive group's staleness.  Nothing more to do here. */

	/* 5. Reset transaction state; active bitmap changed, drop the cache. */
	fs->n_set = fs->n_clr = 0;
	fs->pending_bucket_idx = -1;
	fs->bm_cache_idx = -1;
	return REALFS_OK;
}

static void txn_begin(struct realfs *fs)
{
	/* In erase-aware mode, free_block_count can drift below the true value when a
	 * previous transaction called commit_set (decrement) but aborted without
	 * commit (no restore).  Resync from the LBT before each new transaction so
	 * the reserve check in ea_find_runs always sees an accurate count.
	 * Cost: O(block_count) scan per transaction begin -- acceptable since
	 * block_count is bounded by the partition size. */
	if (fs->edev && fs->ea_lbt) {
		uint32_t used = 0;
		for (uint32_t v = 0; v < fs->block_count; v++)
			if (ea_lbt_get(fs, v) != EA_LBT_NONE) {
				used++;
			}
		fs->free_block_count = fs->block_count - used;
	}
	fs->n_set = fs->n_clr = 0;
	fs->pending_bucket_idx = -1;
}

/* ------------------------------------------------------------------------- */
/* Bucket lookup                                                             */
/* ------------------------------------------------------------------------- */
struct bucket_find_res {
	int      found;
	uint32_t inode_block;
	uint32_t chain[REALFS_MAX_CHAIN];   /* head .. block containing the entry */
	int      chain_len;               /* number of blocks in chain[] */
	int      pos;                     /* chain index of the entry's block */
	int      entry_idx;               /* entry slot within that block */
	uint32_t next_after;              /* overflow_next of the entry's block */
};

/* fs->io_buf is used for bucket blocks, fs->io_buf2 for inode verification. */
static int bucket_find(struct realfs *fs, uint32_t idx, const char *path,
					   uint32_t path_len, uint32_t hash,
					   struct bucket_find_res *res)
{
	res->found = 0;
	res->chain_len = 0;
	uint32_t b = fs->bucket_map[idx];
	int pos = 0;
	while (b != REALFS_BLOCK_EMPTY) {
		if (pos >= (int)REALFS_MAX_CHAIN) {
			return REALFS_ERR_NO_MEM;
		}
		int rc = rd1(fs, b, fs->io_buf);
		if (rc) {
			return rc;
		}
		if (!bk_valid(fs->io_buf, fs->bs)) {
			return REALFS_ERR_CORRUPT;
		}

		uint32_t ec = bk_entry_count(fs->io_buf);
		for (uint32_t j = 0; j < ec; j++) {
			if (bk_entry_hash(fs->io_buf, j) != hash) {
				continue;
			}
			uint32_t inb = bk_entry_inode(fs->io_buf, j);
			rc = rd1(fs, inb, fs->io_buf2);
			if (rc) {
				return rc;
			}
			if (!inode_valid(fs->io_buf2, fs->bs)) {
				return REALFS_ERR_CORRUPT;
			}
			if (inode_name_eq(fs->io_buf2, path, path_len)) {
				res->found       = 1;
				res->inode_block = inb;
				res->pos         = pos;
				res->entry_idx   = (int)j;
				res->next_after  = bk_overflow(fs->io_buf, fs->bs);
				res->chain[pos]  = b;
				res->chain_len   = pos + 1;
				return REALFS_OK;
			}
			/* hash collision false positive: keep scanning */
		}
		res->chain[pos] = b;
		b = bk_overflow(fs->io_buf, fs->bs);
		pos++;
	}
	res->chain_len = pos;
	return REALFS_OK;
}

/* ------------------------------------------------------------------------- */
/* Bucket COW: insert / modify (update) / delete an entry                    */
/* Each writes new bucket block(s) and returns the new head; old blocks that  */
/* are replaced are queued for freeing.  None touch bucket_map (commit does). */
/* ------------------------------------------------------------------------- */

/* Insert a brand new (hash, inode_block) entry into bucket idx. */
static int bucket_insert(struct realfs *fs, uint32_t idx, uint32_t hash,
						 uint32_t inode_block, uint32_t *new_head)
{
	uint32_t bs = fs->bs;
	uint32_t b0 = fs->bucket_map[idx];
	uint32_t nb;
	int rc;

	if (b0 == REALFS_BLOCK_EMPTY) {
		rc = alloc_one(fs, &nb);
		if (rc) {
			return rc;
		}
		memset(fs->io_buf, 0, bs);
		bk_set_entry(fs->io_buf, 0, hash, inode_block);
		bk_finalize(fs->io_buf, bs, 1, REALFS_BLOCK_EMPTY, 0);
		rc = wr1(fs, nb, fs->io_buf);
		if (rc) {
			return rc;
		}
		*new_head = nb;
		return REALFS_OK;
	}

	rc = rd1(fs, b0, fs->io_buf);
	if (rc) {
		return rc;
	}
	if (!bk_valid(fs->io_buf, fs->bs)) {
		return REALFS_ERR_CORRUPT;
	}
	uint32_t ec  = bk_entry_count(fs->io_buf);
	uint32_t rev = ld32(fs->io_buf, BK_REVISION);

	if (ec < bk_max_entry(bs)) {
		/* COW the head: append the new entry, keep the existing overflow. */
		uint32_t ovf = bk_overflow(fs->io_buf, bs);
		rc = alloc_one(fs, &nb);
		if (rc) {
			return rc;
		}
		bk_set_entry(fs->io_buf, ec, hash, inode_block);
		bk_finalize(fs->io_buf, bs, ec + 1, ovf, rev + 1);
		rc = wr1(fs, nb, fs->io_buf);
		if (rc) {
			return rc;
		}
		rc = add_clr(fs, b0, 1);
		if (rc) {
			return rc;    /* free old head */
		}
		*new_head = nb;
		return REALFS_OK;
	}

	/* Head full: prepend a fresh head pointing at the old chain (unchanged). */
	rc = alloc_one(fs, &nb);
	if (rc) {
		return rc;
	}
	memset(fs->io_buf, 0, bs);
	bk_set_entry(fs->io_buf, 0, hash, inode_block);
	bk_finalize(fs->io_buf, bs, 1, b0, 0);
	rc = wr1(fs, nb, fs->io_buf);
	if (rc) {
		return rc;
	}
	*new_head = nb;          /* old head NOT freed: still part of the chain */
	return REALFS_OK;
}

/*
 * Rewrite chain prefix [pos..0] bottom-up after the block at `pos` was given
 * new content in `mod_buf` (already finalized except overflow/crc, which we
 * fix here).  Returns the new head block; queues all old prefix blocks for
 * freeing.  Used by update and by delete's "block still non-empty" path.
 */
static int chain_rewrite_modify(struct realfs *fs, struct bucket_find_res *res,
								uint8_t *mod_buf, uint32_t *new_head)
{
	uint32_t bs = fs->bs;
	int rc;
	uint32_t child;
	uint32_t nb;

	/* position pos: content already in mod_buf; overflow points past it. */
	bk_finalize(mod_buf, bs, bk_entry_count(mod_buf), res->next_after,
				ld32(mod_buf, BK_REVISION) + 1);
	rc = alloc_one(fs, &nb);
	if (rc) {
		return rc;
	}
	rc = wr1(fs, nb, mod_buf);
	if (rc) {
		return rc;
	}
	rc = add_clr(fs, res->chain[res->pos], 1);
	if (rc) {
		return rc;
	}
	child = nb;

	for (int i = res->pos - 1; i >= 0; i--) {
		rc = rd1(fs, res->chain[i], fs->io_buf);
		if (rc) {
			return rc;
		}
		if (!bk_valid(fs->io_buf, bs)) {
			return REALFS_ERR_CORRUPT;
		}
		bk_finalize(fs->io_buf, bs, bk_entry_count(fs->io_buf), child,
					ld32(fs->io_buf, BK_REVISION) + 1);
		rc = alloc_one(fs, &nb);
		if (rc) {
			return rc;
		}
		rc = wr1(fs, nb, fs->io_buf);
		if (rc) {
			return rc;
		}
		rc = add_clr(fs, res->chain[i], 1);
		if (rc) {
			return rc;
		}
		child = nb;
	}
	*new_head = child;
	return REALFS_OK;
}

/* Update an existing entry's inode_block (the structure is unchanged). */
static int bucket_update(struct realfs *fs, struct bucket_find_res *res,
						 uint32_t new_inode, uint32_t *new_head)
{
	int rc = rd1(fs, res->chain[res->pos], fs->io_buf);
	if (rc) {
		return rc;
	}
	if (!bk_valid(fs->io_buf, fs->bs)) {
		return REALFS_ERR_CORRUPT;
	}
	bk_set_entry(fs->io_buf, (uint32_t)res->entry_idx,
				 bk_entry_hash(fs->io_buf, (uint32_t)res->entry_idx), new_inode);
	return chain_rewrite_modify(fs, res, fs->io_buf, new_head);
}

/* Remove the located entry; recycle the block if it becomes empty. */
static int bucket_delete(struct realfs *fs, struct bucket_find_res *res,
						 uint32_t *new_head)
{
	uint32_t bs = fs->bs;
	int rc = rd1(fs, res->chain[res->pos], fs->io_buf);
	if (rc) {
		return rc;
	}
	if (!bk_valid(fs->io_buf, bs)) {
		return REALFS_ERR_CORRUPT;
	}

	uint32_t ec = bk_entry_count(fs->io_buf);
	/* compact: move last entry into the removed slot */
	if ((uint32_t)res->entry_idx != ec - 1) {
		bk_set_entry(fs->io_buf, (uint32_t)res->entry_idx,
					 bk_entry_hash(fs->io_buf, ec - 1),
					 bk_entry_inode(fs->io_buf, ec - 1));
	}
	ec--;

	if (ec > 0) {
		st32(fs->io_buf, BK_ENTRY_COUNT, ec);
		return chain_rewrite_modify(fs, res, fs->io_buf, new_head);
	}

	/* Block is now empty: splice it out of the chain. */
	uint32_t next_after = res->next_after;
	if (res->pos == 0) {
		rc = add_clr(fs, res->chain[0], 1);
		if (rc) {
			return rc;
		}
		*new_head = next_after;   /* may be REALFS_BLOCK_EMPTY */
		return REALFS_OK;
	}
	/* Non-head: rewrite predecessors [pos-1..0], last one points to next_after. */
	{
		uint32_t child = next_after;
		uint32_t nb;
		for (int i = res->pos - 1; i >= 0; i--) {
			rc = rd1(fs, res->chain[i], fs->io_buf);
			if (rc) {
				return rc;
			}
			if (!bk_valid(fs->io_buf, bs)) {
				return REALFS_ERR_CORRUPT;
			}
			bk_finalize(fs->io_buf, bs, bk_entry_count(fs->io_buf), child,
						ld32(fs->io_buf, BK_REVISION) + 1);
			rc = alloc_one(fs, &nb);
			if (rc) {
				return rc;
			}
			rc = wr1(fs, nb, fs->io_buf);
			if (rc) {
				return rc;
			}
			rc = add_clr(fs, res->chain[i], 1);
			if (rc) {
				return rc;
			}
			child = nb;
		}
		rc = add_clr(fs, res->chain[res->pos], 1);
		if (rc) {
			return rc;
		}
		*new_head = child;
	}
	return REALFS_OK;
}

/* ------------------------------------------------------------------------- */
/* Free the data blocks an old inode pointed at (for replace/delete).        */
/* ------------------------------------------------------------------------- */
static int free_inode_blocks(struct realfs *fs, uint32_t inode_block)
{
	int rc = rd1(fs, inode_block, fs->io_buf2);
	if (rc) {
		return rc;
	}
	if (!inode_valid(fs->io_buf2, fs->bs)) {
		return REALFS_ERR_CORRUPT;
	}
	struct inode_info ii;
	rc = inode_parse(fs->io_buf2, &ii);
	if (rc) {
		return rc;
	}
	for (uint32_t i = 0; i < ii.extent_count; i++) {
		uint32_t start = ld32(fs->io_buf2, ii.extents_off + i * 8);
		uint32_t cnt   = ld32(fs->io_buf2, ii.extents_off + i * 8 + 4);
		rc = add_clr(fs, start, cnt);
		if (rc) {
			return rc;
		}
	}
	return add_clr(fs, inode_block, 1);
}

/* ------------------------------------------------------------------------- */
/* format                                                                    */
/* ------------------------------------------------------------------------- */
static int is_pow2(uint32_t v)
{
	return v && ((v & (v - 1)) == 0);
}

static void build_bitmap_block(uint8_t *buf, uint32_t bm_idx, uint32_t bs,
							   uint32_t N, uint32_t data_start, uint32_t data_end)
{
	uint32_t bpb  = bs * 8;
	uint32_t base = bm_idx * bpb;
	memset(buf, 0, bs);
	for (uint32_t k = 0; k < bpb; k++) {
		uint32_t b = base + k;
		int used = (b >= N) || !(b >= data_start && b < data_end);
		if (used) {
			buf[k >> 3] |= (uint8_t)(1u << (k & 7));
		}
	}
}

/* Pick a sane hash_bucket count from the partition geometry.
 *
 * One bucket block indexes (bs-16)/8 entries (~510 @ 4 KB), and every file
 * needs at least its own (inode) block, so the number of data blocks is a hard
 * ceiling on the file count.  Using more buckets than needed to index that many
 * files only wastes data blocks (each occupied bucket costs a full block) -- on
 * a small partition that overhead can dominate.  So choose the fewest
 * power-of-two buckets whose combined capacity covers data_blocks files.
 * (Per-op write cost is one bucket-block rewrite regardless of bucket count,
 * and COW relocates that block each time, so few buckets is not a wear hotspot.)
 */
uint32_t realfs_default_buckets(uint32_t bs, uint32_t data_blocks)
{
	uint32_t per_bucket = (bs > 16) ? (bs - 16) / 8 : 1;   /* entries per bucket block */
	uint32_t need = ceil_div(data_blocks, per_bucket ? per_bucket : 1);
	uint32_t b = 1;
	while (b < need && b < REALFS_MAX_BUCKETS) {
		b <<= 1;
	}
	return b;
}

int realfs_format(const struct realfs_bdev *bdev, uint32_t hash_buckets)
{
	if (!bdev || !bdev->read_blocks || !bdev->write_blocks) {
		return REALFS_ERR_INVAL;
	}
	uint32_t bs = bdev->block_size;
	uint32_t N  = bdev->block_count;
	/* erase-aware mode uses block_size=2032 (non-power-of-2); only NOR/SD need pow2. */
	if (bs < 512 || bs > REALFS_MAX_BLOCK_SIZE || (!is_pow2(bs) && bs != REALFS_EA_BLOCK_SIZE)) {
		return REALFS_ERR_INVAL;
	}

	uint32_t M = ceil_div(ceil_div(N, 8), bs);          /* bitmap blocks */
	if (N < 2 * (1 + M) + 1) {
		return REALFS_ERR_INVAL;
	}
	uint32_t data_start = 1 + M;
	uint32_t data_end   = N - 1 - M;
	if (data_start >= data_end) {
		return REALFS_ERR_INVAL;
	}
	uint32_t freec = data_end - data_start;

	/* hash_buckets == 0 => auto-size from the data region (see helper above). */
	if (hash_buckets == 0) {
		hash_buckets = realfs_default_buckets(bs, freec);
	}
	if (!is_pow2(hash_buckets) || hash_buckets > REALFS_MAX_BUCKETS) {
		return REALFS_ERR_INVAL;
	}
	/* SB must hold: header + bucket_map + persisted dirty-bitmap (ceil(M/8)) + CRC. */
	if (SB_BUCKET_MAP + hash_buckets * 4 + ceil_div(M, 8) + 4 > bs) {
		return REALFS_ERR_INVAL;
	}

	/* Use a temporary realfs only as a place to stage buffers / bdev handle. */
	static struct realfs tmp;                /* large; format is rare and serial */
	memset(&tmp, 0, sizeof(tmp));
	tmp.bdev = bdev;
	tmp.bs = bs;
	tmp.block_count = N;

	/* Write both bitmap groups. */
	for (uint32_t bm = 0; bm < M; bm++) {
		build_bitmap_block(tmp.io_buf, bm, bs, N, data_start, data_end);
		if (wr1(&tmp, 1 + bm, tmp.io_buf)) {
			return REALFS_ERR_IO;    /* group A */
		}
		if (wr1(&tmp, (N - 1 - M) + bm, tmp.io_buf)) {
			return REALFS_ERR_IO;    /* group B */
		}
	}

	/* Stage superblock fields. */
	tmp.block_count      = N;
	tmp.hash_buckets     = hash_buckets;
	tmp.data_start       = data_start;
	tmp.data_end         = data_end;
	tmp.alloc_hint       = data_start;
	tmp.free_block_count = freec;
	tmp.bitmap_blocks    = M;
	for (uint32_t i = 0; i < hash_buckets; i++) {
		tmp.bucket_map[i] = REALFS_BLOCK_EMPTY;
	}

	/* SB-A active (seq=1, bitmap at block 1).  io_buf is free here (bitmap
	 * blocks were already written above), so reuse it to stage the SB.
	 * dirty=NULL: both bitmap groups are identical after format, so the
	 * inactive group is not stale (persisted dirty = all-zero). */
	sb_build(&tmp, tmp.io_buf, 1, 1, NULL);
	if (wr1(&tmp, 0, tmp.io_buf)) {
		return REALFS_ERR_IO;
	}
	/* SB-B backup (seq=0, bitmap at block N-1-M). */
	sb_build(&tmp, tmp.io_buf, 0, N - 1 - M, NULL);
	if (wr1(&tmp, N - 1, tmp.io_buf)) {
		return REALFS_ERR_IO;
	}

	return bdev_sync(&tmp);
}

/* ------------------------------------------------------------------------- */
/* mount / unmount                                                           */
/* ------------------------------------------------------------------------- */
static int sb_is_valid(const uint8_t *b, uint32_t bs, uint32_t N)
{
	if (memcmp(b + SB_MAGIC, REALFS_MAGIC_STR, 8) != 0) {
		return 0;
	}
	if (ld32(b, SB_VERSION) > REALFS_VERSION) {
		return 0;    /* accept older (e.g. v2, no dirty) */
	}
	if (ld32(b, SB_BLOCK_SIZE) != bs) {
		return 0;
	}
	if (ld32(b, SB_BLOCK_COUNT) != N) {
		return 0;
	}
	return ld32(b, bs - 4) == realfs_crc32(b, bs - 4);
}

static void load_from_sb(struct realfs *fs, const uint8_t *b, int group_is_A)
{
	uint32_t N = fs->block_count;
	uint32_t M = ld32(b, SB_BITMAP_BLOCKS);
	fs->hash_buckets     = ld32(b, SB_HASH_BUCKETS);
	fs->data_start       = ld32(b, SB_DATA_START);
	fs->data_end         = ld32(b, SB_DATA_END);
	fs->alloc_hint       = ld32(b, SB_ALLOC_HINT);
	fs->free_block_count = ld32(b, SB_FREE_COUNT);
	fs->bitmap_blocks    = M;
	fs->seq              = ld32(b, SB_SEQ);
	for (uint32_t i = 0; i < fs->hash_buckets; i++) {
		fs->bucket_map[i] = ld32(b, SB_BUCKET_MAP + i * 4);
	}

	if (group_is_A) {
		fs->bitmap_start_act = 1;
		fs->sb_block_act = 0;
		fs->bitmap_start_ina = N - 1 - M;
		fs->sb_block_ina = N - 1;
	} else {
		fs->bitmap_start_act = N - 1 - M;
		fs->sb_block_act = N - 1;
		fs->bitmap_start_ina = 1;
		fs->sb_block_ina = 0;
	}

	/* Restore the dirty-bitmap (§9.5).  v3 SBs persist exactly which inactive
	 * bitmap blocks are stale (= the last commit's touched blocks), so the
	 * first post-mount commit rewrites only those instead of all M (no full
	 * copy, avoids the mount iter-0 stall on large media).  Older/without-dirty
	 * SBs fall back to all-stale = the previous conservative behaviour. */
	if (ld32(b, SB_VERSION) >= 3u) {
		uint32_t off = SB_BUCKET_MAP + fs->hash_buckets * 4;
		uint32_t nb  = ceil_div(M, 8);
		if (nb > sizeof(fs->bm_dirty)) {
			nb = sizeof(fs->bm_dirty);    /* clamp (defensive) */
		}
		memset(fs->bm_dirty, 0, sizeof(fs->bm_dirty));
		memcpy(fs->bm_dirty, b + off, nb);
	} else {
		memset(fs->bm_dirty, 0xFF, sizeof(fs->bm_dirty));
	}
}

int realfs_mount(struct realfs *fs, const struct realfs_bdev *bdev)
{
	if (!fs || !bdev) {
		return REALFS_ERR_INVAL;
	}
	uint32_t bs = bdev->block_size;
	uint32_t N  = bdev->block_count;
	if (bs < 512 || bs > REALFS_MAX_BLOCK_SIZE || (!is_pow2(bs) && bs != REALFS_EA_BLOCK_SIZE)) {
		return REALFS_ERR_INVAL;
	}

	memset(fs, 0, sizeof(*fs));
	fs->bdev = bdev;
	fs->bs = bs;
	fs->block_count = N;
	fs->bm_cache_idx = -1;
	fs->pending_bucket_idx = -1;

	if (rd1(fs, 0,     fs->io_buf)) {
		return REALFS_ERR_IO;    /* SB-A */
	}
	if (rd1(fs, N - 1, fs->io_buf2)) {
		return REALFS_ERR_IO;    /* SB-B */
	}

	int validA = sb_is_valid(fs->io_buf,  bs, N);
	int validB = sb_is_valid(fs->io_buf2, bs, N);
	uint32_t seqA = validA ? ld32(fs->io_buf,  SB_SEQ) : 0;
	uint32_t seqB = validB ? ld32(fs->io_buf2, SB_SEQ) : 0;

	int use_A;
	if (validA && validB) {
		use_A = (seqA >= seqB);
	} else if (validA) {
		use_A = 1;
	} else if (validB) {
		use_A = 0;
	} else {
		return REALFS_ERR_CORRUPT;
	}

	const uint8_t *sb = use_A ? fs->io_buf : fs->io_buf2;
	if (ld32(sb, SB_HASH_BUCKETS) > REALFS_MAX_BUCKETS) {
		return REALFS_ERR_NO_MEM;
	}
	load_from_sb(fs, sb, use_A);
	return REALFS_OK;
}

int realfs_unmount(struct realfs *fs)
{
	if (!fs || !fs->bdev) {
		return REALFS_ERR_INVAL;
	}
	int rc = bdev_sync(fs);
	fs->bdev = NULL;
	return rc;
}

/* ------------------------------------------------------------------------- */
/* open / close / stat                                                       */
/* ------------------------------------------------------------------------- */
/* ------------------------------------------------------------------------- */
/* Open-metadata cache: keyed by path, LRU eviction (see realfs.h).           */
/* ------------------------------------------------------------------------- */
#if REALFS_META_CACHE_BYTES > 0
static struct realfs_meta_entry *meta_cache_lookup(struct realfs *fs, const char *path,
		uint32_t plen, uint32_t hash)
{
	if (plen > REALFS_CACHE_PATH_MAX) {
		return NULL;
	}
	for (uint32_t i = 0; i < REALFS_META_CACHE_ENTRIES; i++) {
		struct realfs_meta_entry *e = &fs->meta_cache[i];
		if (e->in_use && e->path_hash == hash && e->path_len == plen &&
			memcmp(e->path, path, plen) == 0) {
			e->lru_seq = ++fs->meta_lru_clock;
			return e;
		}
	}
	return NULL;
}

static void meta_cache_store(struct realfs *fs, const char *path, uint32_t plen,
							 uint32_t hash, const struct realfs_file *fh)
{
	if (plen > REALFS_CACHE_PATH_MAX) {
		return;
	}
	if (fh->extent_count > REALFS_CACHE_EXTENTS) {
		return;    /* too fragmented to cache */
	}

	/* Prefer: existing entry for this path > empty slot > LRU victim. */
	struct realfs_meta_entry *slot = NULL;
	for (uint32_t i = 0; i < REALFS_META_CACHE_ENTRIES; i++) {
		struct realfs_meta_entry *e = &fs->meta_cache[i];
		if (e->in_use && e->path_hash == hash && e->path_len == plen &&
			memcmp(e->path, path, plen) == 0) {
			slot = e;
			break;
		}
		if (!e->in_use && slot == NULL) {
			slot = e;
		}
	}
	if (slot == NULL) {                                   /* no empty: evict LRU */
		uint32_t lru_min = 0xFFFFFFFFu;
		for (uint32_t i = 0; i < REALFS_META_CACHE_ENTRIES; i++) {
			if (fs->meta_cache[i].lru_seq <= lru_min) {
				lru_min = fs->meta_cache[i].lru_seq;
				slot = &fs->meta_cache[i];
			}
		}
	}
	if (slot == NULL) {
		return;
	}

	slot->in_use       = 1;
	slot->path_hash    = hash;
	slot->path_len     = (uint8_t)plen;
	slot->inode_block  = fh->inode_block;
	slot->file_size    = fh->file_size;
	slot->head_size    = fh->head_size;
	slot->extent_count = fh->extent_count;
	slot->lru_seq      = ++fs->meta_lru_clock;
	memcpy(slot->path, path, plen);
	for (uint16_t i = 0; i < fh->extent_count && i < REALFS_CACHE_EXTENTS; i++) {
		slot->extents[i] = fh->extents[i];
	}
}

static void meta_cache_invalidate(struct realfs *fs, const char *path,
								  uint32_t plen, uint32_t hash)
{
	if (plen > REALFS_CACHE_PATH_MAX) {
		return;
	}
	for (uint32_t i = 0; i < REALFS_META_CACHE_ENTRIES; i++) {
		struct realfs_meta_entry *e = &fs->meta_cache[i];
		if (e->in_use && e->path_hash == hash && e->path_len == plen &&
			memcmp(e->path, path, plen) == 0) {
			e->in_use = 0;
			return;
		}
	}
}
#else
static struct realfs_meta_entry *meta_cache_lookup(struct realfs *fs, const char *path,
		uint32_t plen, uint32_t hash)
{
	(void)fs;
	(void)path;
	(void)plen;
	(void)hash;
	return NULL;
}
static void meta_cache_store(struct realfs *fs, const char *path, uint32_t plen,
							 uint32_t hash, const struct realfs_file *fh)
{
	(void)fs;
	(void)path;
	(void)plen;
	(void)hash;
	(void)fh;
}
static void meta_cache_invalidate(struct realfs *fs, const char *path,
								  uint32_t plen, uint32_t hash)
{
	(void)fs;
	(void)path;
	(void)plen;
	(void)hash;
}
#endif

/* Copy the parsed extent list out of a validated inode block into the handle. */
static void fh_load_extents(struct realfs_file *fh, const struct inode_info *ii)
{
	for (uint16_t i = 0; i < ii->extent_count && i < REALFS_MAX_EXTENTS; i++) {
		fh->extents[i].start = ld32(fh->inode_buf, ii->extents_off + i * 8);
		fh->extents[i].count = ld32(fh->inode_buf, ii->extents_off + i * 8 + 4);
	}
}

int realfs_open(struct realfs *fs, const char *path, struct realfs_file *fh)
{
	if (!fs || !path || !fh) {
		return REALFS_ERR_INVAL;
	}
	uint32_t plen = (uint32_t)strlen(path);
	if (plen == 0 || plen > REALFS_MAX_NAME) {
		return REALFS_ERR_NAME;
	}

	uint32_t hash = realfs_fnv1a(path);

	/* Fast path: parsed-metadata cache hit. */
	struct realfs_meta_entry *ce = meta_cache_lookup(fs, path, plen, hash);
	if (ce) {
		fh->fs           = fs;
		fh->inode_block  = ce->inode_block;
		fh->file_size    = ce->file_size;
		fh->name_len     = (uint16_t)plen;
		fh->extent_count = ce->extent_count;
		fh->head_size    = ce->head_size;
		for (uint16_t i = 0; i < ce->extent_count && i < REALFS_MAX_EXTENTS; i++) {
			fh->extents[i] = ce->extents[i];
		}
		if (ce->head_size == 0) {
			fh->from_cache = 1;    /* large file: extents suffice, skip inode read */
			return REALFS_OK;      /* saved the hash-bucket read AND the inode read */
		}
		/* Small file: head data lives in the inode block, so it must be read. */
		int rc = rd1(fs, ce->inode_block, fh->inode_buf);
		if (rc) {
			return rc;
		}
		if (!inode_valid(fh->inode_buf, fs->bs)) {
			meta_cache_invalidate(fs, path, plen, hash);
			return REALFS_ERR_CORRUPT;
		}
		fh->from_cache = 0;
		return REALFS_OK;          /* saved the hash-bucket read */
	}

	uint32_t idx  = hash & (fs->hash_buckets - 1);
	struct bucket_find_res res;
	int rc = bucket_find(fs, idx, path, plen, hash, &res);
	if (rc) {
		return rc;
	}
	if (!res.found) {
		return REALFS_ERR_NOT_FOUND;
	}

	rc = rd1(fs, res.inode_block, fh->inode_buf);
	if (rc) {
		return rc;
	}
	if (!inode_valid(fh->inode_buf, fs->bs)) {
		return REALFS_ERR_CORRUPT;
	}

	struct inode_info ii;
	inode_parse(fh->inode_buf, &ii);
	fh->fs           = fs;
	fh->inode_block  = res.inode_block;
	fh->file_size    = ii.file_size;
	fh->name_len     = ii.name_len;
	fh->extent_count = ii.extent_count;
	fh->head_size    = ii.head_size;
	fh->from_cache   = 0;
	fh_load_extents(fh, &ii);
	meta_cache_store(fs, path, plen, hash, fh);
	return REALFS_OK;
}

int realfs_close(struct realfs_file *fh)
{
	if (!fh) {
		return REALFS_ERR_INVAL;
	}
	fh->fs = NULL;
	return REALFS_OK;
}

int realfs_stat(struct realfs *fs, const char *path, struct realfs_stat *st)
{
	if (!fs || !path || !st) {
		return REALFS_ERR_INVAL;
	}
	uint32_t plen = (uint32_t)strlen(path);
	if (plen == 0 || plen > REALFS_MAX_NAME) {
		return REALFS_ERR_NAME;
	}

	uint32_t hash = realfs_fnv1a(path);
	uint32_t idx  = hash & (fs->hash_buckets - 1);
	struct bucket_find_res res;
	int rc = bucket_find(fs, idx, path, plen, hash, &res);
	if (rc) {
		return rc;
	}
	if (!res.found) {
		return REALFS_ERR_NOT_FOUND;
	}

	rc = rd1(fs, res.inode_block, fs->io_buf2);
	if (rc) {
		return rc;
	}
	struct inode_info ii;
	rc = inode_parse(fs->io_buf2, &ii);
	if (rc) {
		return rc;
	}
	st->size         = ii.file_size;
	st->extent_count = ii.extent_count;
	st->inode_block  = res.inode_block;
	return REALFS_OK;
}

/* ------------------------------------------------------------------------- */
/* read                                                                      */
/* ------------------------------------------------------------------------- */
int realfs_read(struct realfs_file *fh, void *buf, uint32_t offset, uint32_t length)
{
	if (!fh || !fh->fs || !buf) {
		return REALFS_ERR_INVAL;
	}
	struct realfs *fs = fh->fs;
	uint32_t bs = fs->bs;

	if (offset >= fh->file_size || length == 0) {
		return 0;
	}
	uint32_t end = offset + length;
	if (end > fh->file_size) {
		end = fh->file_size;
	}

	uint8_t *out = (uint8_t *)buf;
	uint32_t copied = 0;

	/* Part 1: head_data inside the inode (0 I/O).  Only reachable when
	 * head_size > 0, in which case inode_buf is always loaded (small files are
	 * never served cache-only, so from_cache implies head_size == 0). */
	if (offset < fh->head_size) {
		uint32_t head_off = IN_HEADER + fh->name_len + (uint32_t)fh->extent_count * 8;
		uint32_t hend = (end < fh->head_size) ? end : fh->head_size;
		uint32_t n = hend - offset;
		memcpy(out, fh->inode_buf + head_off + offset, n);
		copied += n;
	}

	/* Part 2: extents (from fh->extents[], valid whether cache- or inode-sourced). */
	uint32_t cur = (offset > fh->head_size) ? offset : fh->head_size;
	while (cur < end) {
		uint32_t ext_off = cur - fh->head_size;

		/* locate the extent covering ext_off */
		uint32_t acc = 0, i = 0, start = 0, count = 0;
		for (; i < fh->extent_count; i++) {
			start = fh->extents[i].start;
			count = fh->extents[i].count;
			uint32_t ebytes = count * bs;
			if (ext_off < acc + ebytes) {
				break;
			}
			acc += ebytes;
		}
		if (i >= fh->extent_count) {
			break;    /* defensive */
		}

		uint32_t within = ext_off - acc;
		uint32_t phys   = start + within / bs;
		uint32_t intra  = within % bs;
		uint32_t ext_remaining = count * bs - within;
		uint32_t chunk = end - cur;
		if (chunk > ext_remaining) {
			chunk = ext_remaining;
		}

		if (intra == 0 && chunk >= bs) {
			uint32_t full = chunk / bs;
			int rc = rdn(fs, phys, full, out + copied);
			if (rc) {
				return rc;
			}
			uint32_t got = full * bs;
			cur += got;
			copied += got;
		} else {
			int rc = rd1(fs, phys, fs->io_buf);
			if (rc) {
				return rc;
			}
			uint32_t n = bs - intra;
			if (n > chunk) {
				n = chunk;
			}
			memcpy(out + copied, fs->io_buf + intra, n);
			cur += n;
			copied += n;
		}
	}
	return (int)copied;
}

/* ------------------------------------------------------------------------- */
/* write (create or atomic replace)                                          */
/* ------------------------------------------------------------------------- */
static int write_data_runs(struct realfs *fs, const struct realfs_run *runs,
						   int nruns, const uint8_t *src, uint32_t nbytes)
{
	uint32_t bs = fs->bs;
	uint32_t remaining = nbytes;
	for (int r = 0; r < nruns && remaining > 0; r++) {
		uint32_t run_bytes = runs[r].count * bs;
		uint32_t bytes = (remaining < run_bytes) ? remaining : run_bytes;
		uint32_t full  = bytes / bs;
		if (full > 0) {
			int rc = wrn(fs, runs[r].start, full, src);
			if (rc) {
				return rc;
			}
			src += full * bs;
			remaining -= full * bs;
		}
		uint32_t tail = bytes - full * bs;
		if (tail > 0) {
			memset(fs->io_buf, 0, bs);
			memcpy(fs->io_buf, src, tail);
			int rc = wr1(fs, runs[r].start + full, fs->io_buf);
			if (rc) {
				return rc;
			}
			src += tail;
			remaining -= tail;
		}
	}
	return REALFS_OK;
}

int realfs_write(struct realfs *fs, const char *path, const void *data, uint32_t size)
{
	if (!fs || !path || (!data && size)) {
		return REALFS_ERR_INVAL;
	}
	uint32_t plen = (uint32_t)strlen(path);
	if (plen == 0 || plen > REALFS_MAX_NAME) {
		return REALFS_ERR_NAME;
	}

	uint32_t bs = fs->bs;
	const uint8_t *src = (const uint8_t *)data;
	uint32_t hash = realfs_fnv1a(path);
	uint32_t idx  = hash & (fs->hash_buckets - 1);

	struct bucket_find_res res;
	int rc = bucket_find(fs, idx, path, plen, hash, &res);
	if (rc) {
		return rc;
	}

	txn_begin(fs);

	/* ---- layout: decide extent_count / head_size, scan free runs ---- */
	uint32_t head_max0 = bs - IN_HEADER - plen;       /* head when 0 extents */
	uint16_t ec;
	uint32_t head_size;
	struct realfs_run runs[REALFS_MAX_EXTENTS];
	int nruns = 0;

	if (size <= head_max0) {
		ec = 0;
		head_size = size;
	} else {
		/* Large file: keep head_size = 0 so ALL data lives in extents starting
		 * at file offset 0 (block-aligned).  Embedding the head in the inode
		 * would shift every extent by head_size, making block-aligned reads
		 * straddle block boundaries (slow rd1+memcpy path).  Cost: at most one
		 * extra data block vs the head-in-inode packing.  Small files (the
		 * branch above) keep head-in-inode -- 0 data blocks, 0 read I/O. */
		if (IN_HEADER + plen + REALFS_MAX_EXTENTS * 8 >= bs) {
			return REALFS_ERR_FRAGMENTED;   /* extent table would not fit inode */
		}
		head_size = 0;
		uint32_t need = ceil_div(size, bs);
		rc = find_runs(fs, need, runs, REALFS_MAX_EXTENTS, &nruns);
		if (rc) {
			return rc;
		}
		ec = (uint16_t)nruns;
		rc = commit_set(fs, runs, nruns);
		if (rc) {
			return rc;
		}
	}

	/* ---- allocate the inode block ---- */
	uint32_t inode_block;
	rc = alloc_one(fs, &inode_block);
	if (rc) {
		return rc;
	}

	/* ---- write file data ---- */
	if (ec > 0) {
		rc = write_data_runs(fs, runs, nruns, src + head_size, size - head_size);
		if (rc) {
			return rc;
		}
	}

	/* ---- build & write the inode ---- */
	memset(fs->io_buf, 0, bs);
	memcpy(fs->io_buf + IN_MAGIC, "INOD", 4);
	st32(fs->io_buf, IN_FILE_SIZE, size);
	st16(fs->io_buf, IN_NAME_LEN, (uint16_t)plen);
	st16(fs->io_buf, IN_EXT_COUNT, ec);
	st16(fs->io_buf, IN_HEAD_SIZE, (uint16_t)head_size);
	memcpy(fs->io_buf + IN_HEADER, path, plen);
	for (uint16_t i = 0; i < ec; i++) {
		st32(fs->io_buf, IN_HEADER + plen + i * 8,     runs[i].start);
		st32(fs->io_buf, IN_HEADER + plen + i * 8 + 4, runs[i].count);
	}
	if (head_size) {
		memcpy(fs->io_buf + IN_HEADER + plen + ec * 8, src, head_size);
	}
	st32(fs->io_buf, IN_CRC, realfs_crc32(fs->io_buf + 8, bs - 8));
	rc = wr1(fs, inode_block, fs->io_buf);
	if (rc) {
		return rc;
	}
	rc = bdev_sync(fs);
	if (rc) {
		return rc;
	}

	/* ---- COW the hash bucket ---- */
	uint32_t new_head;
	if (res.found) {
		rc = bucket_update(fs, &res, inode_block, &new_head);
		if (rc) {
			return rc;
		}
		rc = free_inode_blocks(fs, res.inode_block);   /* free old inode + data */
		if (rc) {
			return rc;
		}
	} else {
		rc = bucket_insert(fs, idx, hash, inode_block, &new_head);
		if (rc) {
			return rc;
		}
	}
	rc = bdev_sync(fs);
	if (rc) {
		return rc;
	}

	fs->pending_bucket_idx   = (int)idx;
	fs->pending_bucket_block = new_head;

	/* ---- atomic commit ---- */
	rc = commit_txn(fs);
	meta_cache_invalidate(fs, path, plen, hash);  /* inode_block changed */
	return rc;
}

/* ------------------------------------------------------------------------- */
/* streaming write                                                           */
/* ------------------------------------------------------------------------- */

/* Merge the last two set_runs entries if contiguous, keeping n_set bounded even
 * when a large file is allocated across many flushes. */
static void compact_last_set_run(struct realfs *fs)
{
	if (fs->n_set < 2) {
		return;
	}
	struct realfs_run *prev = &fs->set_runs[fs->n_set - 2];
	struct realfs_run *last = &fs->set_runs[fs->n_set - 1];
	if (prev->start + prev->count == last->start) {
		prev->count += last->count;
		fs->n_set--;
	}
}

/* Append a freshly-allocated run to the inode extent list, merging with the
 * previous extent when physically contiguous. */
static int ctx_add_run(struct realfs_write_ctx *ctx, struct realfs_run run)
{
	if (ctx->nruns > 0 &&
		ctx->runs[ctx->nruns - 1].start + ctx->runs[ctx->nruns - 1].count == run.start) {
		ctx->runs[ctx->nruns - 1].count += run.count;
		return REALFS_OK;
	}
	if (ctx->nruns >= (int)REALFS_MAX_EXTENTS) {
		return REALFS_ERR_FRAGMENTED;
	}
	ctx->runs[ctx->nruns++] = run;
	return REALFS_OK;
}

/* Allocate `nblocks` free blocks and write the first nblocks*bs bytes of the
 * staging buffer to them, one multi-block wrn per contiguous run.  Records the
 * allocation in the txn (set_runs) and the inode extent list (ctx->runs). */
static int stream_flush(struct realfs_write_ctx *ctx, uint32_t nblocks)
{
	struct realfs *fs = ctx->fs;
	struct realfs_run runs[REALFS_MAX_EXTENTS];
	int nr = 0;
	int rc = find_runs(fs, nblocks, runs, REALFS_MAX_EXTENTS, &nr);
	if (rc) {
		return rc;
	}
	for (int i = 0; i < nr; i++) {
		rc = commit_set(fs, &runs[i], 1);
		if (rc) {
			return rc;
		}
		compact_last_set_run(fs);
		rc = ctx_add_run(ctx, runs[i]);
		if (rc) {
			return rc;
		}
	}
	return write_data_runs(fs, runs, nr, ctx->staging, nblocks * fs->bs);
}

int realfs_write_begin(struct realfs *fs, const char *path,
					   struct realfs_write_ctx *ctx)
{
	if (!fs || !path || !ctx) {
		return REALFS_ERR_INVAL;
	}
	uint32_t plen = (uint32_t)strlen(path);
	if (plen == 0 || plen > REALFS_MAX_NAME) {
		return REALFS_ERR_NAME;
	}

	memset(ctx, 0, sizeof(*ctx));
	ctx->fs   = fs;
	ctx->plen = plen;
	memcpy(ctx->path, path, plen);

	txn_begin(fs);
	ctx->saved_free_count = fs->free_block_count;

	int rc = alloc_one(fs, &ctx->inode_block);
	if (rc) {
		fs->n_set = fs->n_clr = 0;
		return rc;
	}

	ctx->active = 1;
	return REALFS_OK;
}

/* Blocks to buffer before a streaming flush, chosen from the block size:
 * small blocks (NOR@512B map) batch more so one flush fills a whole 4KB erase
 * segment; large blocks (>=2KB: SD/eMMC/NOR-direct/NAND) use fewer, bigger
 * multi-block writes.  Capped so K*bs never exceeds the fixed staging buffer. */
static uint32_t staging_blocks(uint32_t bs)
{
	uint32_t k     = (bs <= 512u) ? REALFS_STAGING_BLOCKS_SMALL : REALFS_STAGING_BLOCKS;
	uint32_t max_k = (REALFS_STAGING_BLOCKS * REALFS_MAX_BLOCK_SIZE) / bs;
	if (k > max_k) {
		k = max_k;
	}
	if (k < 1u) {
		k = 1u;
	}
	return k;
}

int realfs_write_chunk(struct realfs_write_ctx *ctx, const void *buf, uint32_t len)
{
	if (!ctx || !ctx->active) {
		return REALFS_ERR_INVAL;
	}
	if (!buf && len) {
		return REALFS_ERR_INVAL;
	}
	struct realfs *fs = ctx->fs;
	uint32_t bs  = fs->bs;
	uint32_t k   = staging_blocks(bs);           /* flush granularity (blocks) */
	uint32_t cap = k * bs;                        /* staging capacity in bytes */
	const uint8_t *src = (const uint8_t *)buf;
	uint32_t rem = len;

	while (rem > 0) {
		uint32_t space = cap - ctx->staging_used;
		uint32_t copy  = rem < space ? rem : space;
		memcpy(ctx->staging + ctx->staging_used, src, copy);
		ctx->staging_used  += copy;
		ctx->total_written += copy;
		src += copy;
		rem -= copy;

		if (ctx->staging_used < cap) {
			break;
		}

		/* staging full: flush all K blocks in one multi-block write */
		int rc = stream_flush(ctx, k);
		if (rc) {
			ctx->active = 0;
			return rc;
		}
		ctx->staging_used = 0;
	}
	return REALFS_OK;
}

int realfs_write_end(struct realfs_write_ctx *ctx)
{
	if (!ctx || !ctx->active) {
		return REALFS_ERR_INVAL;
	}
	ctx->active = 0;
	struct realfs *fs  = ctx->fs;
	uint32_t bs        = fs->bs;
	uint32_t plen      = ctx->plen;
	uint32_t size      = ctx->total_written;
	uint32_t head_max0 = bs - IN_HEADER - plen;
	uint16_t ec;
	uint32_t head_size;
	int rc;

	uint32_t full = ctx->staging_used / bs;   /* whole blocks still buffered */
	uint32_t tail = ctx->staging_used % bs;    /* trailing partial block */

	/* Flush any whole blocks left in staging (one multi-block write). */
	if (full > 0) {
		rc = stream_flush(ctx, full);
		if (rc) {
			return rc;
		}
	}

	if (tail > 0) {
		if (ctx->nruns == 0 && tail <= head_max0) {
			/* whole file fits in the inode head (no data blocks) */
			ec = 0;
			head_size = tail;
		} else {
			/* pad the trailing partial block and write it as one more block */
			if (ctx->nruns >= (int)REALFS_MAX_EXTENTS) {
				return REALFS_ERR_FRAGMENTED;
			}
			uint8_t *tailbuf = ctx->staging + full * bs;
			memset(tailbuf + tail, 0, bs - tail);
			struct realfs_run run;
			int n;
			rc = find_runs(fs, 1, &run, 1, &n);
			if (rc) {
				return rc;
			}
			rc = commit_set(fs, &run, 1);
			if (rc) {
				return rc;
			}
			compact_last_set_run(fs);
			rc = ctx_add_run(ctx, run);
			if (rc) {
				return rc;
			}
			rc = wr1(fs, run.start, tailbuf);
			if (rc) {
				return rc;
			}
			ec = (uint16_t)ctx->nruns;
			head_size = 0;
		}
	} else {
		ec = (uint16_t)ctx->nruns;
		head_size = 0;
	}

	/* ---- build and write inode ---- */
	memset(fs->io_buf, 0, bs);
	memcpy(fs->io_buf + IN_MAGIC, "INOD", 4);
	st32(fs->io_buf, IN_FILE_SIZE, size);
	st16(fs->io_buf, IN_NAME_LEN, (uint16_t)plen);
	st16(fs->io_buf, IN_EXT_COUNT, ec);
	st16(fs->io_buf, IN_HEAD_SIZE, (uint16_t)head_size);
	memcpy(fs->io_buf + IN_HEADER, ctx->path, plen);
	for (uint16_t i = 0; i < ec; i++) {
		st32(fs->io_buf, IN_HEADER + plen + i * 8,     ctx->runs[i].start);
		st32(fs->io_buf, IN_HEADER + plen + i * 8 + 4, ctx->runs[i].count);
	}
	if (head_size) {
		memcpy(fs->io_buf + IN_HEADER + plen, ctx->staging, head_size);
	}
	st32(fs->io_buf, IN_CRC, realfs_crc32(fs->io_buf + 8, bs - 8));
	rc = wr1(fs, ctx->inode_block, fs->io_buf);
	if (rc) {
		return rc;
	}
	rc = bdev_sync(fs);
	if (rc) {
		return rc;
	}

	/* ---- bucket COW ---- */
	uint32_t hash = realfs_fnv1a(ctx->path);
	uint32_t idx  = hash & (fs->hash_buckets - 1);
	struct bucket_find_res res;
	rc = bucket_find(fs, idx, ctx->path, plen, hash, &res);
	if (rc) {
		return rc;
	}

	uint32_t new_head;
	if (res.found) {
		rc = bucket_update(fs, &res, ctx->inode_block, &new_head);
		if (rc) {
			return rc;
		}
		rc = free_inode_blocks(fs, res.inode_block);
		if (rc) {
			return rc;
		}
	} else {
		rc = bucket_insert(fs, idx, hash, ctx->inode_block, &new_head);
		if (rc) {
			return rc;
		}
	}
	rc = bdev_sync(fs);
	if (rc) {
		return rc;
	}

	fs->pending_bucket_idx   = (int)idx;
	fs->pending_bucket_block = new_head;

	rc = commit_txn(fs);
	meta_cache_invalidate(fs, ctx->path, plen, hash);  /* inode_block changed */
	return rc;
}

void realfs_write_abort(struct realfs_write_ctx *ctx)
{
	if (!ctx || !ctx->active) {
		return;
	}
	ctx->active = 0;
	struct realfs *fs = ctx->fs;
	/* Restore free_block_count; orphaned blocks are still marked free in the
	 * active bitmap (set_runs was never committed) so the count is correct
	 * once we reset n_set. */
	fs->free_block_count = ctx->saved_free_count;
	fs->n_set = fs->n_clr = 0;
	fs->pending_bucket_idx = -1;
	fs->bm_cache_idx = -1;
}

/* ------------------------------------------------------------------------- */
/* delete                                                                    */
/* ------------------------------------------------------------------------- */
int realfs_delete(struct realfs *fs, const char *path)
{
	if (!fs || !path) {
		return REALFS_ERR_INVAL;
	}
	uint32_t plen = (uint32_t)strlen(path);
	if (plen == 0 || plen > REALFS_MAX_NAME) {
		return REALFS_ERR_NAME;
	}

	uint32_t hash = realfs_fnv1a(path);
	uint32_t idx  = hash & (fs->hash_buckets - 1);
	struct bucket_find_res res;
	int rc = bucket_find(fs, idx, path, plen, hash, &res);
	if (rc) {
		return rc;
	}
	if (!res.found) {
		return REALFS_ERR_NOT_FOUND;
	}

	/* Allow delete to use the metadata vblk reserve: delete always frees more
	 * vblks than it allocates (net negative), but the alloc precedes the free,
	 * so it needs the reserve to bridge the gap.  MUST clear on ALL paths. */
	if (fs->edev) {
		fs->ea_meta_op = 1;
	}
	txn_begin(fs);

	uint32_t new_head;
	rc = bucket_delete(fs, &res, &new_head);
	if (rc) {
		if (fs->edev) {
			fs->ea_meta_op = 0;
		}
		return rc;
	}
	rc = free_inode_blocks(fs, res.inode_block);
	if (rc) {
		if (fs->edev) {
			fs->ea_meta_op = 0;
		}
		return rc;
	}
	rc = bdev_sync(fs);
	if (rc) {
		return rc;
	}

	fs->pending_bucket_idx   = (int)idx;
	fs->pending_bucket_block = new_head;

	rc = commit_txn(fs);
	if (fs->edev) {
		fs->ea_meta_op = 0;
	}
	meta_cache_invalidate(fs, path, plen, hash);  /* file gone */
	return rc;
}

/* ========================================================================= */
/* Debug / inspection helpers (compiled only when REALFS_ENABLE_DEBUG).      */
/*                                                                            */
/* All read-only.  They reuse fs->io_buf / io_buf2 / bm_cache as scratch, so */
/* call them only when no realfs operation is in flight (e.g. between writes  */
/* in a long-run test, or from a shell command) -- never concurrently.  The  */
/* printf-style sink keeps this core free of stdio/board headers: PC code    */
/* passes printf, firmware passes a DiagPrintf-compatible wrapper.            */
/* ========================================================================= */
#ifdef REALFS_ENABLE_DEBUG

static void dbg_name(const uint8_t *inode, char *out, uint32_t cap)
{
	uint32_t n = ld16(inode, IN_NAME_LEN);
	if (n > cap - 1) {
		n = cap - 1;
	}
	memcpy(out, inode + IN_HEADER, n);
	out[n] = '\0';
}

void realfs_dump_super(struct realfs *fs, realfs_printf_t pf)
{
	if (!fs || !pf) {
		return;
	}
	uint32_t data_blocks = fs->data_end - fs->data_start;
	uint32_t used = (data_blocks >= fs->free_block_count)
					? data_blocks - fs->free_block_count : 0;
	uint32_t pct  = data_blocks ? (uint32_t)(((uint64_t)used * 100) / data_blocks) : 0;

	pf("=== REALFS superblock / geometry ===\r\n");
	pf("  block_size   : %u\r\n", (unsigned)fs->bs);
	pf("  block_count  : %u  (%u KB)\r\n", (unsigned)fs->block_count,
	   (unsigned)((uint64_t)fs->block_count * fs->bs / 1024));
	pf("  hash_buckets : %u\r\n", (unsigned)fs->hash_buckets);
	pf("  data region  : [%u, %u) = %u blocks (%u KB)\r\n",
	   (unsigned)fs->data_start, (unsigned)fs->data_end, (unsigned)data_blocks,
	   (unsigned)((uint64_t)data_blocks * fs->bs / 1024));
	pf("  bitmap (M)   : %u blocks  act@%u  ina@%u\r\n",
	   (unsigned)fs->bitmap_blocks, (unsigned)fs->bitmap_start_act,
	   (unsigned)fs->bitmap_start_ina);
	pf("  superblock   : act@%u (%s)  ina@%u\r\n", (unsigned)fs->sb_block_act,
	   fs->sb_block_act == 0 ? "group A" : "group B", (unsigned)fs->sb_block_ina);
	pf("  seq          : %u\r\n", (unsigned)fs->seq);
	pf("  alloc_hint   : %u\r\n", (unsigned)fs->alloc_hint);
	pf("  free blocks  : %u / %u   (used %u, %u%%)\r\n",
	   (unsigned)fs->free_block_count, (unsigned)data_blocks, (unsigned)used, (unsigned)pct);
}

void realfs_dump_buckets(struct realfs *fs, realfs_printf_t pf)
{
	if (!fs || !pf) {
		return;
	}
	uint32_t occupied = 0, total_entries = 0, max_chain = 0;
	pf("=== REALFS buckets (%u) ===\r\n", (unsigned)fs->hash_buckets);
	for (uint32_t bi = 0; bi < fs->hash_buckets; bi++) {
		uint32_t blk = fs->bucket_map[bi];
		if (blk == REALFS_BLOCK_EMPTY) {
			continue;
		}
		occupied++;
		uint32_t chain = 0, entries = 0;
		pf("  bucket[%u]:", (unsigned)bi);
		while (blk != REALFS_BLOCK_EMPTY) {
			if (rd1(fs, blk, fs->io_buf)) {
				pf(" blk%u[IO err]", (unsigned)blk);
				break;
			}
			uint32_t ec  = bk_entry_count(fs->io_buf);
			uint32_t rev = ld32(fs->io_buf, BK_REVISION);
			pf(" -> blk%u(rev%u,%ue)", (unsigned)blk, (unsigned)rev, (unsigned)ec);
			entries += ec;
			chain++;
			blk = bk_overflow(fs->io_buf, fs->bs);
		}
		pf("\r\n");
		total_entries += entries;
		if (chain > max_chain) {
			max_chain = chain;
		}
	}
	pf("  occupied %u/%u buckets, %u entries, max chain %u\r\n",
	   (unsigned)occupied, (unsigned)fs->hash_buckets, (unsigned)total_entries,
	   (unsigned)max_chain);
}

void realfs_dump_files(struct realfs *fs, realfs_printf_t pf)
{
	if (!fs || !pf) {
		return;
	}
	char namebuf[REALFS_MAX_NAME + 1];
	uint32_t total = 0;
	pf("=== REALFS files ===\r\n");
	for (uint32_t bi = 0; bi < fs->hash_buckets; bi++) {
		uint32_t blk = fs->bucket_map[bi];
		while (blk != REALFS_BLOCK_EMPTY) {
			if (rd1(fs, blk, fs->io_buf)) {
				pf("  [bucket %u blk %u: IO error]\r\n",
				   (unsigned)bi, (unsigned)blk);
				break;
			}
			uint32_t ec  = bk_entry_count(fs->io_buf);
			uint32_t ovf = bk_overflow(fs->io_buf, fs->bs);
			for (uint32_t e = 0; e < ec; e++) {
				uint32_t hash  = bk_entry_hash(fs->io_buf, e);
				uint32_t inblk = bk_entry_inode(fs->io_buf, e);
				if (rd1(fs, inblk, fs->io_buf2)) {
					pf("  inode@%u: IO error\r\n", (unsigned)inblk);
					continue;
				}
				struct inode_info ii;
				if (inode_parse(fs->io_buf2, &ii)) {
					pf("  inode@%u: bad magic\r\n", (unsigned)inblk);
					continue;
				}
				dbg_name(fs->io_buf2, namebuf, sizeof(namebuf));
				int crc_ok = inode_valid(fs->io_buf2, fs->bs);
				pf("  [%u] \"%s\"\r\n", (unsigned)total, namebuf);
				pf("      size=%u head=%u extents=%u inode@%u bucket=%u hash=0x%x crc=%s\r\n",
				   (unsigned)ii.file_size, (unsigned)ii.head_size, (unsigned)ii.extent_count,
				   (unsigned)inblk, (unsigned)bi, (unsigned)hash, crc_ok ? "ok" : "BAD");
				for (uint32_t x = 0; x < ii.extent_count; x++) {
					uint32_t s = ld32(fs->io_buf2, ii.extents_off + x * 8);
					uint32_t c = ld32(fs->io_buf2, ii.extents_off + x * 8 + 4);
					pf("        ext[%u] start=%u count=%u (blocks %u..%u)\r\n",
					   (unsigned)x, (unsigned)s, (unsigned)c, (unsigned)s,
					   (unsigned)(s + c - 1));
				}
				total++;
			}
			blk = ovf;
		}
	}
	pf("  total files: %u\r\n", (unsigned)total);
}

int realfs_foreach(struct realfs *fs, realfs_iter_cb cb, void *user)
{
	if (!fs || !cb) {
		return REALFS_ERR_INVAL;
	}
	char namebuf[REALFS_MAX_NAME + 1];
	int count = 0;
	for (uint32_t bi = 0; bi < fs->hash_buckets; bi++) {
		uint32_t blk = fs->bucket_map[bi];
		while (blk != REALFS_BLOCK_EMPTY) {
			if (rd1(fs, blk, fs->io_buf)) {
				return REALFS_ERR_IO;
			}
			if (!bk_valid(fs->io_buf, fs->bs)) {
				return REALFS_ERR_CORRUPT;
			}
			uint32_t ec  = bk_entry_count(fs->io_buf);
			uint32_t ovf = bk_overflow(fs->io_buf, fs->bs);
			for (uint32_t e = 0; e < ec; e++) {
				/* Re-read the bucket block: the callback may have used the
				 * shared scratch buffers (e.g. realfs_open) since last entry. */
				if (rd1(fs, blk, fs->io_buf)) {
					return REALFS_ERR_IO;
				}
				uint32_t inblk = bk_entry_inode(fs->io_buf, e);
				if (rd1(fs, inblk, fs->io_buf2)) {
					return REALFS_ERR_IO;
				}
				struct inode_info ii;
				if (inode_parse(fs->io_buf2, &ii)) {
					return REALFS_ERR_CORRUPT;
				}
				dbg_name(fs->io_buf2, namebuf, sizeof(namebuf));
				struct realfs_dirent ent;
				ent.name         = namebuf;
				ent.size         = ii.file_size;
				ent.extent_count = ii.extent_count;
				ent.inode_block  = inblk;
				count++;
				if (cb(&ent, user)) {
					return count;
				}
			}
			blk = ovf;
		}
	}
	return count;
}

void realfs_dump_bitmap(struct realfs *fs, realfs_printf_t pf)
{
	if (!fs || !pf) {
		return;
	}
	uint32_t data_blocks = fs->data_end - fs->data_start;
	pf("=== REALFS bitmap (data [%u,%u), '#'=used '.'=free) ===\r\n",
	   (unsigned)fs->data_start, (unsigned)fs->data_end);

	const uint32_t PER_LINE = 64;
	const uint32_t MAX_SHOW = 4096;          /* cap the visual map */
	uint32_t free_runs = 0, cur_run = 0, max_run = 0, free_total = 0;
	char line[64 + 1];
	uint32_t col = 0, shown = 0;

	for (uint32_t b = fs->data_start; b < fs->data_end; b++) {
		int used = bm_test_used(fs, b);
		if (used) {
			if (cur_run > max_run) {
				max_run = cur_run;
			}
			cur_run = 0;
		} else {
			free_total++;
			if (cur_run == 0) {
				free_runs++;
			}
			cur_run++;
		}
		if (shown < MAX_SHOW) {
			if (col == 0) {
				pf("  %6u: ", (unsigned)b);
			}
			line[col++] = used ? '#' : '.';
			if (col == PER_LINE) {
				line[col] = '\0';
				pf("%s\r\n", line);
				col = 0;
				shown += PER_LINE;
			}
		}
	}
	if (cur_run > max_run) {
		max_run = cur_run;
	}
	if (col) {
		line[col] = '\0';
		pf("%s\r\n", line);
	}
	if (data_blocks > MAX_SHOW) {
		pf("  (... %u more blocks not shown ...)\r\n", (unsigned)(data_blocks - MAX_SHOW));
	}
	pf("  free %u blocks in %u runs, largest run %u blocks\r\n",
	   (unsigned)free_total, (unsigned)free_runs, (unsigned)max_run);
}

int realfs_check(struct realfs *fs, realfs_printf_t pf)
{
	if (!fs || !pf) {
		return -1;
	}
	int problems = 0;
	char namebuf[REALFS_MAX_NAME + 1];
	pf("=== REALFS consistency check ===\r\n");

	/* Shadow bitmap (1 bit per data block) used to cross-check that every
	 * data-region block is referenced by exactly one object and that every
	 * "used" bit corresponds to a real reference.  Covers up to
	 * REALFS_MAX_BLOCK_SIZE*8 data blocks; larger FSes skip the cross-check. */
	static uint8_t dbg_shadow[REALFS_MAX_BLOCK_SIZE];
	uint32_t data_blocks = fs->data_end - fs->data_start;
	int shadow_ok = (data_blocks <= sizeof(dbg_shadow) * 8);
	if (shadow_ok) {
		memset(dbg_shadow, 0, (data_blocks + 7) / 8);
	}
	/* Mark block `b`; flags a double-reference if it was already claimed. */
#define SH_MARK(b, who) do { \
        uint32_t _bb = (uint32_t)(b); \
        if (shadow_ok && _bb >= fs->data_start && _bb < fs->data_end) { \
            uint32_t _o = _bb - fs->data_start; \
            if (dbg_shadow[_o >> 3] & (uint8_t)(1u << (_o & 7))) { \
                pf("  [E] block %u double-referenced (%s)\r\n", (unsigned)_bb, (who)); \
                problems++; \
            } else { \
                dbg_shadow[_o >> 3] |= (uint8_t)(1u << (_o & 7)); \
            } \
        } \
    } while (0)

	/* 1. superblock magic + CRC, both groups. */
	for (int g = 0; g < 2; g++) {
		uint32_t sbblk = (g == 0) ? fs->sb_block_act : fs->sb_block_ina;
		if (rd1(fs, sbblk, fs->io_buf)) {
			pf("  [E] sb@%u IO error\r\n", (unsigned)sbblk);
			problems++;
			continue;
		}
		int magic_ok = (memcmp(fs->io_buf, REALFS_MAGIC_STR, 8) == 0);
		uint32_t want = ld32(fs->io_buf, fs->bs - 4);
		uint32_t got  = realfs_crc32(fs->io_buf, fs->bs - 4);
		if (!magic_ok || want != got) {
			pf("  [E] sb@%u (%s) bad%s%s (crc want 0x%x got 0x%x)\r\n",
			   (unsigned)sbblk, g == 0 ? "active" : "inactive",
			   magic_ok ? "" : " magic", (want == got) ? "" : " crc",
			   (unsigned)want, (unsigned)got);
			problems++;
		}
	}

	/* 2. recompute free count from the active bitmap over the data region. */
	uint32_t free_recount = 0;
	for (uint32_t b = fs->data_start; b < fs->data_end; b++)
		if (!bm_test_used(fs, b)) {
			free_recount++;
		}
	if (free_recount != fs->free_block_count) {
		pf("  [E] free count mismatch: sb=%u  bitmap=%u\r\n",
		   (unsigned)fs->free_block_count, (unsigned)free_recount);
		problems++;
	}

	/* 3. per-file: inode crc, extent bounds, extent/inode blocks marked used,
	 *    and head+extents large enough to hold file_size. */
	uint32_t nfiles = 0;
	for (uint32_t bi = 0; bi < fs->hash_buckets; bi++) {
		uint32_t blk = fs->bucket_map[bi];
		while (blk != REALFS_BLOCK_EMPTY) {
			if (rd1(fs, blk, fs->io_buf)) {
				pf("  [E] bucket %u blk %u IO error\r\n",
				   (unsigned)bi, (unsigned)blk);
				problems++;
				break;
			}
			uint32_t ec  = bk_entry_count(fs->io_buf);
			uint32_t ovf = bk_overflow(fs->io_buf, fs->bs);
			SH_MARK(blk, "bucket block");
			for (uint32_t e = 0; e < ec; e++) {
				uint32_t inblk = bk_entry_inode(fs->io_buf, e);
				if (inblk < fs->data_start || inblk >= fs->data_end) {
					pf("  [E] bucket %u entry %u: inode block %u out of range\r\n",
					   (unsigned)bi, (unsigned)e, (unsigned)inblk);
					problems++;
					continue;
				}
				SH_MARK(inblk, "inode");
				if (rd1(fs, inblk, fs->io_buf2)) {
					problems++;
					continue;
				}
				struct inode_info ii;
				if (inode_parse(fs->io_buf2, &ii)) {
					pf("  [E] inode@%u bad magic\r\n", (unsigned)inblk);
					problems++;
					continue;
				}
				dbg_name(fs->io_buf2, namebuf, sizeof(namebuf));
				if (!inode_valid(fs->io_buf2, fs->bs)) {
					pf("  [E] inode@%u \"%s\" crc BAD\r\n", (unsigned)inblk, namebuf);
					problems++;
				}
				if (!bm_test_used(fs, inblk)) {
					pf("  [E] \"%s\" inode block %u not marked used\r\n", namebuf, (unsigned)inblk);
					problems++;
				}
				uint32_t ext_bytes = 0;
				for (uint32_t x = 0; x < ii.extent_count; x++) {
					uint32_t s = ld32(fs->io_buf2, ii.extents_off + x * 8);
					uint32_t c = ld32(fs->io_buf2, ii.extents_off + x * 8 + 4);
					if (c == 0 || s < fs->data_start || s + c > fs->data_end) {
						pf("  [E] \"%s\" ext[%u] start=%u count=%u out of range\r\n",
						   namebuf, (unsigned)x, (unsigned)s, (unsigned)c);
						problems++;
						continue;
					}
					for (uint32_t k = 0; k < c; k++) {
						SH_MARK(s + k, "extent");
						if (!bm_test_used(fs, s + k)) {
							pf("  [E] \"%s\" ext[%u] block %u not marked used\r\n",
							   namebuf, (unsigned)x, (unsigned)(s + k));
							problems++;
						}
					}
					ext_bytes += c * fs->bs;
				}
				if ((uint32_t)ii.head_size + ext_bytes < ii.file_size) {
					pf("  [E] \"%s\" size=%u > head(%u)+extents(%u)=%u\r\n",
					   namebuf, (unsigned)ii.file_size, (unsigned)ii.head_size,
					   (unsigned)ext_bytes, (unsigned)((uint32_t)ii.head_size + ext_bytes));
					problems++;
				}
				nfiles++;
			}
			blk = ovf;
		}
	}

	/* 4. leak detection: any data block marked used in the bitmap but not
	 *    referenced by an inode / extent / bucket is a leak.  (Blocks
	 *    referenced but NOT marked used are already reported in step 3;
	 *    double-references were reported inline by SH_MARK.) */
	if (shadow_ok) {
		for (uint32_t b = fs->data_start; b < fs->data_end; b++) {
			uint32_t o = b - fs->data_start;
			int ref = (dbg_shadow[o >> 3] >> (o & 7)) & 1;
			if (!ref && bm_test_used(fs, b)) {
				pf("  [E] block %u marked used but unreferenced (leak)\r\n", (unsigned)b);
				problems++;
			}
		}
	} else {
		pf("  [i] %u data blocks > shadow capacity; double-alloc/leak check skipped\r\n",
		   (unsigned)data_blocks);
	}
#undef SH_MARK

	pf("  checked %u files; %d problem(s) found\r\n", (unsigned)nfiles, problems);
	return problems;
}

void realfs_dump_all(struct realfs *fs, realfs_printf_t pf)
{
	realfs_dump_super(fs, pf);
	realfs_dump_buckets(fs, pf);
	realfs_dump_files(fs, pf);
	realfs_dump_bitmap(fs, pf);
	realfs_check(fs, pf);
}

#endif /* REALFS_ENABLE_DEBUG */

/* =========================================================================
 * erase-aware mode
 * =========================================================================
 * All functions here are only reachable when fs->edev != NULL.
 * They operate on the realfs_erasedev interface and manage:
 *   - LBT  (vblk -> physical (seg, page_in_seg))
 *   - Segment state table (free / open / sealed / sb_ring)
 *   - Append allocator (write to open segment, advance cursor)
 *   - Superblock ring (two dedicated segments, append-only commit)
 *   - Mount scan (rebuild LBT + seg state from page headers)
 *   - GC (copy live pages out of a low-live segment, then erase it)
 * ========================================================================= */

/* ---- page header helpers ------------------------------------------------ */
#define EA_PH_SIZE REALFS_EA_PH_SIZE  /* local shorthand */

static void ea_ph_write(uint8_t *ph, uint32_t vblk, uint32_t seq)
{
	ph[0] = 'R';
	ph[1] = 'P';
	ph[2] = 'A';
	ph[3] = 'G';
	st32(ph, EA_PH_VBLK, vblk);
	st32(ph, EA_PH_SEQ,  seq);
	st32(ph, EA_PH_CRC,  realfs_crc32(ph, EA_PH_CRC));
}

static int ea_ph_valid(const uint8_t *ph)
{
	if (ph[0] != 'R' || ph[1] != 'P' || ph[2] != 'A' || ph[3] != 'G') {
		return 0;
	}
	return ld32(ph, EA_PH_CRC) == realfs_crc32(ph, EA_PH_CRC);
}

/* Scratch page buffer: header (16B) + block payload.  Used by single-page ops
 * (GC relocation, SB ring). */
#define EA_PGBUF_SIZE  REALFS_EA_PAGE_SIZE
static uint8_t g_ea_pgbuf[EA_PGBUF_SIZE]; /* single-threaded, no RTOS */

/* Batched-write scratch: assembles up to EA_BATCH_BYTES worth of consecutive
 * physical pages (each = 16B header + payload) so a run of blocks becomes ONE
 * seg_prog instead of one flash transaction per page.  4 KB covers a full NOR
 * segment (8 x 512B pages) and 2 NAND pages (2 x 2048B). */
#define EA_BATCH_BYTES  4096u
static uint8_t g_ea_batch[EA_BATCH_BYTES]; /* single-threaded, no RTOS */

/* ---- LBT helpers ------------------------------------------------------- */
static uint16_t ea_lbt_get(const struct realfs *fs, uint32_t vblk)
{
	return fs->ea_lbt[vblk];
}
static void ea_lbt_set(struct realfs *fs, uint32_t vblk, uint16_t val)
{
	fs->ea_lbt[vblk] = val;
}
static uint16_t ea_lbt_encode(uint32_t seg, uint32_t pg)
{
	return (uint16_t)((seg << 6) | (pg & 0x3Fu));
}
static uint32_t ea_lbt_seg(uint16_t v)
{
	return (uint32_t)(v >> 6);
}
static uint32_t ea_lbt_page(uint16_t v)
{
	return (uint32_t)(v & 0x3Fu);
}

/* ---- physical I/O wrappers --------------------------------------------- */
static int ea_page_read(const struct realfs *fs, uint32_t seg, uint32_t pg,
						void *block_buf /* EA_BLOCK_SIZE bytes */)
{
	if (fs->edev->seg_read(fs->edev, seg,
						   EA_PH_SIZE + pg * fs->edev->page_size,
						   fs->bs, block_buf)) {
		return REALFS_ERR_IO;
	}
	return REALFS_OK;
}

/* Read the full page (header + payload) into g_ea_pgbuf. */
static int ea_page_read_full(const struct realfs *fs, uint32_t seg, uint32_t pg)
{
	if (fs->edev->seg_read(fs->edev, seg,
						   pg * fs->edev->page_size,
						   fs->edev->page_size, g_ea_pgbuf)) {
		return REALFS_ERR_IO;
	}
	return REALFS_OK;
}

/* Program one page: build header + payload into g_ea_pgbuf, then prog. */
static int ea_page_prog(struct realfs *fs, uint32_t seg, uint32_t pg,
						uint32_t vblk, const void *block_buf)
{
	ea_ph_write(g_ea_pgbuf, vblk, fs->ea_seq++);
	/* GC relocation passes block_buf == g_ea_pgbuf + EA_PH_SIZE (payload of
	 * the page just read into g_ea_pgbuf); skip the self-copy in that case. */
	if (block_buf != g_ea_pgbuf + EA_PH_SIZE) {
		memcpy(g_ea_pgbuf + EA_PH_SIZE, block_buf, fs->bs);
	}
	if (fs->edev->seg_prog(fs->edev, seg,
						   pg * fs->edev->page_size,
						   fs->edev->page_size, g_ea_pgbuf)) {
		return REALFS_ERR_IO;
	}
	return REALFS_OK;
}

/* ---- allocator ---------------------------------------------------------- */
/* Segments kept in reserve so GC always has a relocation destination.  User
 * writes trigger GC once free segments drop to this level; GC itself is allowed
 * to consume the reserve (it nets segments back by erasing the victim). */
#define EA_GC_RESERVE 3u

/* Low-level: make sure there is an open segment with room, taking a fresh FREE
 * segment if needed.  DOES NOT run GC (so it never recurses) -- returns
 * REALFS_ERR_NO_SPACE when no free segment is available.  Erasing the segment
 * before opening unmaps any stale/foreign mapping on the thin FTL (program-once
 * safe) and physically clears it on a direct backend. */
static int ea_open_fresh_seg(struct realfs *fs)
{
	const struct realfs_erasedev *e = fs->edev;
	if (fs->ea_open_seg < e->seg_count &&
		fs->ea_sst[fs->ea_open_seg] == EA_SEG_OPEN &&
		fs->ea_page_cur < e->seg_pages) {
		return REALFS_OK;    /* current open segment still has space */
	}

	if (fs->ea_open_seg < e->seg_count &&
		fs->ea_sst[fs->ea_open_seg] == EA_SEG_OPEN) {
		fs->ea_sst[fs->ea_open_seg] = EA_SEG_SEALED;
	}

	for (uint32_t i = 0; i < e->seg_count; i++) {
		if (fs->ea_sst[i] == EA_SEG_FREE) {
			if (e->seg_erase(e, i) != 0) {
				continue;    /* skip a segment that won't erase */
			}
			fs->ea_open_seg = i;
			fs->ea_page_cur = 0;
			fs->ea_sst[i]   = EA_SEG_OPEN;
			if (fs->ea_free_segs > 0) {
				fs->ea_free_segs--;
			}
			return REALFS_OK;
		}
	}
	return REALFS_ERR_NO_SPACE;
}

/* User-write headroom: run GC until at least EA_GC_RESERVE+1 free segments
 * exist, so a subsequent allocation (and the GC that a later write may need)
 * always has room.  Called on the USER write path only -- never from GC, so
 * GC can never re-enter this and recurse.  Returns ENOSPC when GC can make no
 * further progress and the device is genuinely full. */
static int ea_reserve_space(struct realfs *fs)
{
	/* Grind GC until enough free segments exist.  A single GC of a mostly-live
	 * segment may not raise ea_free_segs at all (relocating N live pages
	 * consumes ~N free pages while erasing the victim frees a full segment), yet
	 * it still reclaims that segment's dead pages and consolidates live data --
	 * after enough passes the freed pages coalesce into whole free segments.  So
	 * we keep going as long as GC finds *any* reclaimable garbage; ea_gc_one
	 * returns REALFS_ERR_NO_SPACE only when the best victim has no dead pages at
	 * all (device genuinely full).  Bounded to avoid unbounded worst-case
	 * latency; the bound is generous (every segment can be visited). */
	uint32_t guard = fs->edev->seg_count * 2u + 8u;
	while (fs->ea_free_segs <= EA_GC_RESERVE && guard-- > 0) {
		int rc = ea_gc_one(fs);
		if (rc == REALFS_ERR_NO_SPACE) {
			break;      /* no reclaimable garbage left */
		}
		if (rc) {
			return rc;  /* I/O error */
		}
	}
	return (fs->ea_free_segs > 0) ? REALFS_OK : REALFS_ERR_NO_SPACE;
}

/* Allocate the next page slot in the open segment.  Returns (seg<<6)|page or
 * sets seg_out/pg_out and returns 0; returns error on no space. */
static int ea_alloc_page(struct realfs *fs, uint32_t *seg_out, uint32_t *pg_out)
{
	int rc = ea_open_fresh_seg(fs);   /* no GC here -> never recurses via GC */
	if (rc) {
		return rc;
	}
	*seg_out = fs->ea_open_seg;
	*pg_out  = fs->ea_page_cur++;
	return REALFS_OK;
}

/* ---- rd1 / wr1 replacements for erase-aware mode ------------------------------ */
#define EA_RD_CACHE_NONE  0xFFFFFFFFu

static int ea_read_vblk(struct realfs *fs, uint32_t vblk, void *buf)
{
	/* Single-block read cache.  REALFS block_size (2032) does not divide the
	 * VFS read chunk (4096), so a block straddling two consecutive read calls
	 * is fetched twice: it is the last block of chunk N and the first of chunk
	 * N+1.  Caching just the most-recent block turns that second fetch into a
	 * memcpy, removing the ~1.5x read amplification for sequential reads. */
	if (vblk == fs->ea_rd_cache_vblk && fs->ea_rd_cache) {
		memcpy(buf, fs->ea_rd_cache, fs->bs);
		return REALFS_OK;
	}

	uint16_t e = ea_lbt_get(fs, vblk);
	if (e == EA_LBT_NONE) {
		/* Never written: return all-0xFF (erased page content) */
		memset(buf, 0xFF, fs->bs);
	} else {
		int rc = ea_page_read(fs, ea_lbt_seg(e), ea_lbt_page(e), buf);
		if (rc) {
			return rc;
		}
	}

	if (fs->ea_rd_cache) {
		memcpy(fs->ea_rd_cache, buf, fs->bs);
		fs->ea_rd_cache_vblk = vblk;
	}
	return REALFS_OK;
}

/* Write a run of `n` consecutive vblks (payloads packed at fs->bs stride in
 * `buf`).  Consecutive pages that land in the same open segment are assembled
 * into g_ea_batch and flushed with a SINGLE seg_prog -- this collapses the
 * per-page flash transactions that dominate write cost on sub-erase-unit
 * geometries (NOR@512B: up to 8 pages -> 1 transaction).  The batch is capped
 * by EA_BATCH_BYTES, by the pages remaining in the open segment, and by `n`.
 * User-write path only (never GC), so ea_reserve_space's GC can't recurse. */
static int ea_write_run(struct realfs *fs, uint32_t blk, uint32_t n, const void *buf)
{
	const struct realfs_erasedev *e = fs->edev;
	uint32_t page_size  = e->page_size;
	uint32_t batch_max  = EA_BATCH_BYTES / page_size;   /* 8 (NOR) / 2 (NAND) */
	if (batch_max == 0) {
		batch_max = 1;
	}
	const uint8_t *src = (const uint8_t *)buf;

	uint32_t i = 0;
	while (i < n) {
		/* GC headroom before allocating this batch (proactive, low-water). */
		int rc = ea_reserve_space(fs);
		if (rc) {
			return rc;
		}
		rc = ea_open_fresh_seg(fs);   /* no GC here -> never recurses via GC */
		if (rc) {
			return rc;
		}

		uint32_t seg = fs->ea_open_seg;
		uint32_t pg  = fs->ea_page_cur;
		uint32_t k   = n - i;
		if (k > batch_max) {
			k = batch_max;
		}
		if (k > e->seg_pages - pg) {
			k = e->seg_pages - pg;    /* stay in this seg */
		}

		/* Assemble k physical pages (header + payload) into the batch buffer. */
		for (uint32_t j = 0; j < k; j++) {
			uint8_t *p = g_ea_batch + j * page_size;
			ea_ph_write(p, blk + i + j, fs->ea_seq++);
			memcpy(p + EA_PH_SIZE, src + (i + j) * fs->bs, fs->bs);
		}
		if (e->seg_prog(e, seg, pg * page_size, k * page_size, g_ea_batch)) {
			return REALFS_ERR_IO;
		}

		/* Commit allocation + LBT/live updates for the k pages just written. */
		fs->ea_page_cur += k;
		for (uint32_t j = 0; j < k; j++) {
			uint32_t vblk = blk + i + j;
			if (vblk == fs->ea_rd_cache_vblk) {
				fs->ea_rd_cache_vblk = EA_RD_CACHE_NONE;
			}
			uint16_t old = ea_lbt_get(fs, vblk);   /* old copy becomes dead */
			if (old != EA_LBT_NONE) {
				uint32_t oseg = ea_lbt_seg(old);
				if (fs->ea_live[oseg] > 0) {
					fs->ea_live[oseg]--;
				}
			}
			ea_lbt_set(fs, vblk, ea_lbt_encode(seg, pg + j));
			fs->ea_live[seg]++;
		}
		i += k;
	}
	return REALFS_OK;
}

static int ea_write_vblk(struct realfs *fs, uint32_t vblk, const void *buf)
{
	return ea_write_run(fs, vblk, 1, buf);   /* single-block wrapper */
}

static void ea_mark_dead(struct realfs *fs, uint32_t vblk)
{
	uint16_t old = ea_lbt_get(fs, vblk);
	if (old == EA_LBT_NONE) {
		return;
	}
	uint32_t oseg = ea_lbt_seg(old);
	if (fs->ea_live[oseg] > 0) {
		fs->ea_live[oseg]--;
	}
	ea_lbt_set(fs, vblk, EA_LBT_NONE);
}

/* ---- allocator: NAND replacement for find_runs ------------------------- */
/* In erase-aware mode we just need `need` fresh vblk numbers from a flat pool.
 * vblk numbers are assigned sequentially and freed on delete.  The LBT
 * already tracks which are live; here we find `need` unmapped (free) vblks
 * using a simple linear scan from alloc_hint. */
/* vblks kept in reserve so delete/rename (which alloc before freeing) succeed
 * even when the FS is "user-full".  User writes stop at this threshold; metadata
 * operations (ea_meta_op=1) bypass it and can dip into the reserve. */
#define EA_VBLK_META_RESERVE  16u

static int ea_find_runs(struct realfs *fs, uint32_t need,
						struct realfs_run *out, int max_runs, int *nout)
{
	*nout = 0;
	if (need == 0) {
		return REALFS_OK;
	}
	/* NOTE: do NOT use free_block_count as a hard gate here.  In erase-aware mode,
	 * free_block_count can drift below the true LBT-free count when a
	 * transaction calls commit_set (decrement) but later aborts without
	 * commit (no restore).  The LBT scan below is the authoritative source;
	 * free_block_count is only an optimistic hint.  For user writes, enforce
	 * the metadata reserve via the scan's found count. */
	uint32_t reserve = fs->ea_meta_op ? 0u : EA_VBLK_META_RESERVE;

	uint32_t total = fs->block_count;
	uint32_t got   = 0;
	uint32_t scanned = 0;

	uint32_t pos = fs->alloc_hint;
	while (got < need && scanned < total) {
		if (pos >= total) {
			pos = 0;
		}
		if (ea_lbt_get(fs, pos) == EA_LBT_NONE) {
			/* start a run */
			if (*nout >= max_runs) {
				return REALFS_ERR_FRAGMENTED;
			}
			uint32_t rs = pos;
			uint32_t rl = 0;
			while (got + rl < need && scanned + rl < total) {
				uint32_t q = rs + rl;
				if (q >= total) {
					break;
				}
				if (ea_lbt_get(fs, q) != EA_LBT_NONE) {
					break;
				}
				rl++;
			}
			out[*nout].start = rs;
			out[*nout].count = rl;
			(*nout)++;
			got     += rl;
			scanned += rl;
			pos      = rs + rl;
		} else {
			pos++;
			scanned++;
		}
	}
	if (got < need) {
		return REALFS_ERR_NO_SPACE;
	}
	/* For user writes, refuse if the scan left fewer than reserve free vblks.
	 * Count remaining LBT-NONE entries (fast path: check free_block_count as
	 * lower bound since it may undercount due to abort drift, but if it still
	 * exceeds the reserve we are safe). */
	if (reserve > 0) {
		/* Verify we are not eating into the reserve.  free_block_count may be
		 * stale (undercounts on abort drift), so recompute only when it looks
		 * tight.  The actual remaining = free_block_count + drift; since drift
		 * is non-negative (aborts only decrement), free_block_count <= actual.
		 * Accept if free_block_count > reserve after deducting `need`; reject
		 * if free_block_count - need < reserve (conservative). */
		if (fs->free_block_count < need + reserve) {
			/* Might be a false positive due to drift; do a full LBT scan to
			 * count actual free vblks and decide accurately. */
			uint32_t actual_free = 0;
			for (uint32_t v = 0; v < total; v++)
				if (ea_lbt_get(fs, v) == EA_LBT_NONE) {
					actual_free++;
				}
			if (actual_free < need + reserve) {
				*nout = 0;
				return REALFS_ERR_NO_SPACE;
			}
		}
	}
	fs->alloc_hint = pos;
	return REALFS_OK;
}

/* ---- superblock ring commit -------------------------------------------- */
/* The two SB ring segments (ea_sb_seg[0/1]) live at the end of the seg
 * address space.  Each page in a ring segment stores one superblock record
 * (standard SB_* layout, 2032B payload).  On commit we append to the active
 * ring segment; when it's full we erase the other one and switch to it. */

static int ea_sb_ring_write(struct realfs *fs)
{
	const struct realfs_erasedev *e = fs->edev;
	uint32_t active_seg = fs->ea_sb_seg[fs->ea_sb_active];

	/* If active ring segment is full, flip to the other one */
	if (fs->ea_sb_cursor >= e->seg_pages) {
		uint32_t next = fs->ea_sb_active ^ 1u;
		uint32_t next_seg = fs->ea_sb_seg[next];
		if (e->seg_erase(e, next_seg) != 0) {
			return REALFS_ERR_IO;
		}
		fs->ea_sb_active = next;
		fs->ea_sb_cursor = 0;
		active_seg = next_seg;
	}

	/* Build the SB record in io_buf: reuse sb_build() with NAND adaptations.
	 * In erase-aware mode bitmap fields are zeroed (no bitmap). */
	uint8_t *buf = fs->io_buf;
	memset(buf, 0, fs->bs);
	memcpy(buf + SB_MAGIC, REALFS_MAGIC_STR, 8);
	st32(buf, SB_VERSION,      2);
	st32(buf, SB_BLOCK_SIZE,   fs->bs);
	st32(buf, SB_BLOCK_COUNT,  fs->block_count);
	st32(buf, SB_HASH_BUCKETS, fs->hash_buckets);
	st32(buf, SB_DATA_START,   fs->data_start);
	st32(buf, SB_DATA_END,     fs->data_end);
	st32(buf, SB_ALLOC_HINT,   fs->alloc_hint);
	st32(buf, SB_SEQ,          fs->seq + 1);
	st32(buf, SB_FREE_COUNT,   fs->free_block_count);
	/* bitmap fields left zero in erase-aware mode */
	for (uint32_t i = 0; i < fs->hash_buckets; i++) {
		st32(buf, SB_BUCKET_MAP + i * 4, fs->bucket_map[i]);
	}
	/* CRC over [0, bs-4) */
	st32(buf, fs->bs - 4, realfs_crc32(buf, fs->bs - 4));

	uint32_t pg = fs->ea_sb_cursor;
	ea_ph_write(g_ea_pgbuf, 0xFFFFFFFEu, fs->ea_seq++); /* special vblk = SB */
	memcpy(g_ea_pgbuf + EA_PH_SIZE, buf, fs->bs);
	if (e->seg_prog(e, active_seg,
					pg * e->page_size,
					e->page_size, g_ea_pgbuf)) {
		return REALFS_ERR_IO;
	}
	fs->ea_sb_cursor++;
	if (e->sync) {
		e->sync(e);
	}
	return REALFS_OK;
}

/* ---- NAND commit (replaces commit_txn for erase-aware mode) ------------------- */
static int ea_commit(struct realfs *fs)
{
	/* Apply pending bucket_map change */
	if (fs->pending_bucket_idx >= 0) {
		fs->bucket_map[fs->pending_bucket_idx] = fs->pending_bucket_block;
	}

	/* Mark freed vblks as dead in LBT */
	for (int i = 0; i < fs->n_clr; i++) {
		for (uint32_t j = 0; j < fs->clr_runs[i].count; j++) {
			ea_mark_dead(fs, fs->clr_runs[i].start + j);
		}
	}

	/* Write the new superblock ring entry (atomic commit point) */
	int rc = ea_sb_ring_write(fs);
	if (rc) {
		return rc;
	}

	fs->seq++;
	fs->n_set = fs->n_clr = 0;
	fs->pending_bucket_idx = -1;
	return REALFS_OK;
}

/* ---- GC ----------------------------------------------------------------- */
/* Garbage-collect one segment: copy live pages to the open segment, then
 * erase the victim.  Caller should check free segment count before calling. */
static int ea_gc_one(struct realfs *fs)
{
	const struct realfs_erasedev *e = fs->edev;

	/* Pick victim: lowest live count among SEALED segments */
	uint32_t victim = (uint32_t) -1;
	uint8_t  min_live = 255u;
	for (uint32_t i = 0; i < e->seg_count; i++) {
		if (fs->ea_sst[i] != EA_SEG_SEALED) {
			continue;
		}
		if (fs->ea_live[i] < min_live) {
			min_live = fs->ea_live[i];
			victim   = i;
		}
	}
	if (victim == (uint32_t) -1) {
		return REALFS_ERR_NO_SPACE;
	}
	/* Best victim is fully live -> nothing to reclaim anywhere; report full so
	 * the caller stops grinding instead of relocating a segment for zero gain. */
	if (min_live >= e->seg_pages) {
		return REALFS_ERR_NO_SPACE;
	}

	/* Move every live page out */
	for (uint32_t pg = 0; pg < e->seg_pages; pg++) {
		int rc = ea_page_read_full(fs, victim, pg);
		if (rc) {
			return rc;
		}
		if (!ea_ph_valid(g_ea_pgbuf)) {
			continue;
		}
		uint32_t vblk = ld32(g_ea_pgbuf, EA_PH_VBLK);
		if (vblk == 0xFFFFFFFFu || vblk == 0xFFFFFFFEu) {
			continue;
		}
		if (vblk >= fs->block_count) {
			continue;
		}

		/* Only move if this is still the current physical location */
		uint16_t cur = ea_lbt_get(fs, vblk);
		if (cur == EA_LBT_NONE) {
			continue;
		}
		if (ea_lbt_seg(cur) != victim || ea_lbt_page(cur) != pg) {
			continue;
		}

		/* Allocate new page and write */
		uint32_t nseg, npg;
		rc = ea_alloc_page(fs, &nseg, &npg);
		if (rc) {
			return rc;
		}
		rc = ea_page_prog(fs, nseg, npg, vblk, g_ea_pgbuf + EA_PH_SIZE);
		if (rc) {
			return rc;
		}

		ea_lbt_set(fs, vblk, ea_lbt_encode(nseg, npg));
		fs->ea_live[nseg]++;
		fs->ea_live[victim]--;
	}

	/* Erase and return to free pool */
	if (e->seg_erase(e, victim) != 0) {
		return REALFS_ERR_IO;
	}
	fs->ea_sst[victim]  = EA_SEG_FREE;
	fs->ea_live[victim] = 0;
	fs->ea_free_segs++;
	return REALFS_OK;
}

/* ---- mount scan -------------------------------------------------------- */
/* Scan all data segments, read every page header, rebuild LBT + seg state.
 * The superblock record with the highest seq in the SB ring segments is
 * loaded to restore bucket_map and other FS metadata. */
static int ea_mount_scan(struct realfs *fs)
{
	const struct realfs_erasedev *e = fs->edev;

	/* Clear LBT and segment state */
	for (uint32_t v = 0; v < fs->block_count; v++) {
		ea_lbt_set(fs, v, EA_LBT_NONE);
	}
	memset(fs->ea_live, 0, e->seg_count);
	memset(fs->ea_sst,  EA_SEG_FREE, e->seg_count);

	uint32_t sb_seg_a = fs->ea_sb_seg[0];
	uint32_t sb_seg_b = fs->ea_sb_seg[1];
	fs->ea_sst[sb_seg_a] = EA_SEG_SB_RING;
	fs->ea_sst[sb_seg_b] = EA_SEG_SB_RING;

	/* Pass 1: scan data segments, build per-vblk max-seq table to resolve
	 * duplicates (GC-in-progress or power-loss leftover). */
	/* We store max_seq per vblk temporarily in ea_lbt via (seq<<16) trick —
	 * but seq is 32-bit.  Use a separate temporary scan pass instead:
	 * first collect (vblk, seq, seg, pg) then take max-seq winner. */

	/* Simple approach: two sub-passes.
	 * Sub-pass A: for each data page, if vblk is valid and seq > current
	 *             best, update LBT.  Uses ea_seq as a scratch to track
	 *             best seq per vblk (stored in an auxiliary array — but we
	 *             have no extra RAM).
	 * Compact approach: single pass keeping winner in LBT, using a parallel
	 *             max_seq[] array of uint32_t. */

	/* Allocate a temporary max_seq array on the stack would overflow for
	 * 14k entries.  Use fs->io_buf2 as a rolling window instead, processing
	 * one segment at a time and updating LBT in-place by seq comparison. */

	/* We need to know the seq of the current LBT entry.  Re-read it when a
	 * candidate has the same vblk.  This adds re-reads on collision but keeps
	 * memory usage to zero. */

	for (uint32_t seg = 0; seg < e->seg_count; seg++) {
		if (fs->ea_sst[seg] == EA_SEG_SB_RING) {
			continue;
		}
		uint8_t has_any = 0;
		uint8_t all_ff  = 1;

		for (uint32_t pg = 0; pg < e->seg_pages; pg++) {
			int rc = ea_page_read_full(fs, seg, pg);
			if (rc) {
				continue;    /* I/O error on one page: skip */
			}

			/* Detect erased page */
			int all_erased = 1;
			for (uint32_t b = 0; b < EA_PH_SIZE; b++)
				if (g_ea_pgbuf[b] != 0xFFu) {
					all_erased = 0;
					break;
				}
			if (all_erased) {
				continue;    /* free page slot */
			}

			all_ff = 0;
			if (!ea_ph_valid(g_ea_pgbuf)) {
				continue;    /* corrupt header */
			}

			uint32_t vblk = ld32(g_ea_pgbuf, EA_PH_VBLK);
			uint32_t seq  = ld32(g_ea_pgbuf, EA_PH_SEQ);

			if (vblk == 0xFFFFFFFFu) {
				continue;    /* free vblk marker */
			}
			if (vblk == 0xFFFFFFFEu) {
				continue;    /* SB page in data seg? skip */
			}
			if (vblk >= fs->block_count) {
				continue;    /* out of range */
			}

			/* Update global seq high-water mark */
			if (seq + 1 > fs->ea_seq) {
				fs->ea_seq = seq + 1;
			}

			uint16_t cur = ea_lbt_get(fs, vblk);
			if (cur == EA_LBT_NONE) {
				/* first sighting */
				ea_lbt_set(fs, vblk, ea_lbt_encode(seg, pg));
				fs->ea_live[seg]++;
				has_any = 1;
			} else {
				/* Already have an entry: keep the one with higher seq.
				 * Re-read the current winner's header to get its seq. */
				uint32_t cseg = ea_lbt_seg(cur);
				uint32_t cpg  = ea_lbt_page(cur);
				uint8_t  ph2[EA_PH_SIZE];
				if (e->seg_read(e, cseg, cpg * e->page_size,
								EA_PH_SIZE, ph2) == 0 && ea_ph_valid(ph2)) {
					uint32_t cseq = ld32(ph2, EA_PH_SEQ);
					if (seq > cseq) {
						/* new winner */
						if (fs->ea_live[cseg] > 0) {
							fs->ea_live[cseg]--;
						}
						ea_lbt_set(fs, vblk, ea_lbt_encode(seg, pg));
						fs->ea_live[seg]++;
						has_any = 1;
					}
					/* loser page is dead, don't count it */
				}
			}
		}

		if (all_ff) {
			fs->ea_sst[seg] = EA_SEG_FREE;
		} else if (has_any) {
			/* Determine open vs sealed by whether cursor reached end */
			fs->ea_sst[seg] = EA_SEG_SEALED;
		}
	}

	/* Pass 2: scan SB ring segments to find latest valid SB */
	uint32_t best_seq = 0;
	int      found_sb = 0;
	uint32_t best_sb_seg = 0, best_sb_pg = 0;

	for (uint32_t si = 0; si < 2; si++) {
		uint32_t seg = fs->ea_sb_seg[si];
		for (uint32_t pg = 0; pg < e->seg_pages; pg++) {
			int rc = ea_page_read_full(fs, seg, pg);
			if (rc) {
				continue;
			}
			if (!ea_ph_valid(g_ea_pgbuf)) {
				continue;
			}
			if (ld32(g_ea_pgbuf, EA_PH_VBLK) != 0xFFFFFFFEu) {
				continue;
			}
			uint32_t seq = ld32(g_ea_pgbuf, EA_PH_SEQ);
			if (!found_sb || seq > best_seq) {
				best_seq    = seq;
				found_sb    = 1;
				best_sb_seg = seg;
				best_sb_pg  = pg;
			}
		}
	}

	if (!found_sb) {
		return REALFS_ERR_CORRUPT;
	}

	/* Load the winning SB record */
	if (e->seg_read(e, best_sb_seg,
					best_sb_pg * e->page_size + EA_PH_SIZE,
					fs->bs, fs->io_buf) != 0) {
		return REALFS_ERR_IO;
	}

	uint8_t *sb = fs->io_buf;
	uint32_t bs = ld32(sb, SB_BLOCK_SIZE);
	uint32_t N  = ld32(sb, SB_BLOCK_COUNT);
	if (bs != fs->bs || N != fs->block_count) {
		return REALFS_ERR_CORRUPT;
	}
	if (ld32(sb, bs - 4) != realfs_crc32(sb, bs - 4)) {
		return REALFS_ERR_CORRUPT;
	}

	fs->hash_buckets     = ld32(sb, SB_HASH_BUCKETS);
	fs->data_start       = ld32(sb, SB_DATA_START);
	fs->data_end         = ld32(sb, SB_DATA_END);
	fs->alloc_hint       = ld32(sb, SB_ALLOC_HINT);
	fs->free_block_count = ld32(sb, SB_FREE_COUNT);
	fs->seq              = ld32(sb, SB_SEQ);
	for (uint32_t i = 0; i < fs->hash_buckets; i++) {
		fs->bucket_map[i] = ld32(sb, SB_BUCKET_MAP + i * 4);
	}

	/* Set up SB ring cursor: find next free slot in active ring seg */
	for (uint32_t si = 0; si < 2; si++) {
		uint32_t seg = fs->ea_sb_seg[si];
		if (seg != best_sb_seg) {
			continue;
		}
		fs->ea_sb_active = si;
		/* cursor = first page after last valid SB page */
		fs->ea_sb_cursor = best_sb_pg + 1;
		break;
	}

	/* Identify the one open segment (last one that is not free/sealed) */
	/* After full scan everything is SEALED; find the segment with a page
	 * cursor < seg_pages to mark as OPEN.  We pick the segment whose last
	 * page is the highest seq data page (most recent writer). */
	uint32_t open_seg = (uint32_t) -1;
	uint32_t open_cur = 0;
	uint32_t max_seg_seq = 0;
	for (uint32_t seg = 0; seg < e->seg_count; seg++) {
		if (fs->ea_sst[seg] != EA_SEG_SEALED) {
			continue;
		}
		/* Check last page's seq */
		uint8_t ph[EA_PH_SIZE];
		/* find last written page by scanning backwards */
		for (int pg = (int)e->seg_pages - 1; pg >= 0; pg--) {
			if (e->seg_read(e, seg, (uint32_t)pg * e->page_size,
							EA_PH_SIZE, ph) != 0) {
				break;
			}
			int erased = 1;
			for (int b = 0; b < (int)EA_PH_SIZE; b++)
				if (ph[b] != 0xFFu) {
					erased = 0;
					break;
				}
			if (erased) {
				continue;    /* this page slot empty, keep searching */
			}
			if (!ea_ph_valid(ph)) {
				break;
			}
			uint32_t seq = ld32(ph, EA_PH_SEQ);
			if (seq >= max_seg_seq) {
				max_seg_seq = seq;
				open_seg    = seg;
				open_cur    = (uint32_t)pg + 1;
			}
			break;
		}
	}
	if (open_seg != (uint32_t) -1 && open_cur < e->seg_pages) {
		fs->ea_sst[open_seg] = EA_SEG_OPEN;
		fs->ea_open_seg  = open_seg;
		fs->ea_page_cur  = open_cur;
	} else {
		/* All segments are fully sealed or no data at all — next write will
		 * pick a free segment. */
		fs->ea_open_seg = (uint32_t) -1;
		fs->ea_page_cur = 0;
		/* trigger ensure_open_seg on next write */
		if (open_seg != (uint32_t) -1) {
			fs->ea_sst[open_seg] = EA_SEG_SEALED;
		}
	}

	/* Recompute free_block_count from LBT */
	uint32_t used = 0;
	for (uint32_t v = 0; v < fs->block_count; v++)
		if (ea_lbt_get(fs, v) != EA_LBT_NONE) {
			used++;
		}
	fs->free_block_count = fs->block_count - used;

	/* Count free data segments (for GC headroom accounting). */
	fs->ea_free_segs = 0;
	for (uint32_t s = 0; s < e->seg_count; s++)
		if (fs->ea_sst[s] == EA_SEG_FREE) {
			fs->ea_free_segs++;
		}

	return REALFS_OK;
}

/* ---- public API --------------------------------------------------------- */

/* Validate erase-aware geometry shared by raw NAND (page 2048 B) and NOR@512B
 * map mode (page 512 B), and return block_size = page_size - header.  Returns 0
 * on invalid geometry.  Constraints: block fits the scratch buffer and leaves
 * room for the SB record; the (seg,page) space is encodable in the uint16 LBT
 * entry ((seg<<6)|page, 0xFFFF = unmapped) -> seg_pages<=63, and the highest
 * code stays below the 0xFFFF sentinel. */
static uint32_t ea_geom_bs(const struct realfs_erasedev *edev)
{
	if (!edev || !edev->seg_read || !edev->seg_prog || !edev->seg_erase) {
		return 0;
	}
	uint32_t ps = edev->page_size;
	if (ps <= REALFS_EA_PH_SIZE || ps > EA_PGBUF_SIZE) {
		return 0;
	}
	uint32_t bs = ps - REALFS_EA_PH_SIZE;
	if (bs < 480u) {
		return 0;    /* SB/inode need room */
	}
	if (edev->seg_pages == 0 || edev->seg_pages > 63u) {
		return 0;    /* 6-bit page field */
	}
	if (edev->seg_count < 4) {
		return 0;
	}
	if ((uint32_t)(edev->seg_count - 1) * 64u + (edev->seg_pages - 1) >= EA_LBT_NONE) {
		return 0;
	}
	return bs;
}

int realfs_ea_format(const struct realfs_erasedev *edev, uint32_t hash_buckets)
{
	uint32_t bs = ea_geom_bs(edev);
	if (bs == 0) {
		return REALFS_ERR_INVAL;
	}

	uint32_t data_segs    = edev->seg_count - 2;          /* reserve last 2 for SB ring */
	uint32_t block_count  = data_segs * edev->seg_pages;  /* total vblks */
	uint32_t sb_seg_a     = data_segs;
	uint32_t sb_seg_b     = data_segs + 1;

	if (hash_buckets == 0) {
		hash_buckets = realfs_default_buckets(bs, block_count);
	}
	if (hash_buckets > REALFS_MAX_BUCKETS) {
		hash_buckets = REALFS_MAX_BUCKETS;
	}
	if (!is_pow2(hash_buckets)) {
		return REALFS_ERR_INVAL;
	}
	/* Cap buckets so the SB record (header + bucket_map + CRC) fits one block
	 * (relevant on NOR@496B; NAND@2032B always has room). */
	while (hash_buckets > 1u && SB_BUCKET_MAP + hash_buckets * 4u + 4u > bs) {
		hash_buckets >>= 1;
	}
	if (SB_BUCKET_MAP + hash_buckets * 4u + 4u > bs) {
		return REALFS_ERR_INVAL;
	}

	/* Erase ALL segments.  This is essential for correctness, not just cleanup:
	 * mount scan rebuilds the LBT from any valid page header (RPAG) it finds, so
	 * stale pages left from a PREVIOUS REALFS instance on this partition would be
	 * "resurrected" as live data (segments marked SEALED, old vblks mapped) --
	 * making the freshly-formatted FS appear full and forcing GC on every write.
	 * On a thin FTL (LBM) an unmapped block erases instantly, so this only costs
	 * real NAND erases for blocks that actually hold old data (<1 s, one-time). */
	for (uint32_t s = 0; s < edev->seg_count; s++) {
		if (edev->seg_erase(edev, s) != 0) {
			return REALFS_ERR_IO;
		}
	}

	/* Build initial superblock in a temp struct realfs.  static (not on the
	 * stack): struct realfs is ~25 KB (io_buf/io_buf2/bm_cache/meta_cache);
	 * a stack instance overflows the caller's task stack.  Format is rare and
	 * serial, so a shared static is fine (matches realfs_format for NOR). */
	static struct realfs tmp;
	memset(&tmp, 0, sizeof(tmp));
	tmp.edev        = edev;
	tmp.bs          = bs;
	tmp.block_count = block_count;
	tmp.hash_buckets = hash_buckets;
	tmp.data_start  = 0;
	tmp.data_end    = block_count;
	tmp.alloc_hint  = 0;
	tmp.free_block_count = block_count;
	tmp.seq         = 0;
	tmp.ea_sb_seg[0]  = sb_seg_a;
	tmp.ea_sb_seg[1]  = sb_seg_b;
	tmp.ea_sb_active  = 0;
	tmp.ea_sb_cursor  = 0;
	tmp.ea_seq        = 1;
	for (uint32_t i = 0; i < hash_buckets; i++) {
		tmp.bucket_map[i] = REALFS_BLOCK_EMPTY;
	}
	tmp.pending_bucket_idx = -1;

	/* Write the first SB ring entry (seq=1) */
	int rc = ea_sb_ring_write(&tmp);
	if (rc) {
		return rc;
	}

	return REALFS_OK;
}

int realfs_ea_mount(struct realfs *fs, const struct realfs_erasedev *edev)
{
	if (!fs || !edev) {
		return REALFS_ERR_INVAL;
	}
	uint32_t bs = ea_geom_bs(edev);
	if (bs == 0) {
		return REALFS_ERR_INVAL;
	}

	memset(fs, 0, sizeof(*fs));
	fs->edev        = edev;
	fs->bs          = bs;
	uint32_t data_segs   = edev->seg_count - 2;
	fs->block_count = data_segs * edev->seg_pages;
	fs->data_start  = 0;
	fs->data_end    = fs->block_count;
	fs->ea_sb_seg[0] = data_segs;
	fs->ea_sb_seg[1] = data_segs + 1;
	fs->pending_bucket_idx = -1;
	fs->bm_cache_idx = -1;

	/* Allocate LBT, segment state arrays, and the 1-block read cache. */
	fs->ea_lbt  = (uint16_t *)malloc(fs->block_count * sizeof(uint16_t));
	fs->ea_live = (uint8_t *)malloc(edev->seg_count);
	fs->ea_sst  = (uint8_t *)malloc(edev->seg_count);
	fs->ea_rd_cache = (uint8_t *)malloc(fs->bs);
	fs->ea_rd_cache_vblk = EA_RD_CACHE_NONE;
	if (!fs->ea_lbt || !fs->ea_live || !fs->ea_sst || !fs->ea_rd_cache) {
		free(fs->ea_lbt);
		free(fs->ea_live);
		free(fs->ea_sst);
		free(fs->ea_rd_cache);
		return REALFS_ERR_NO_MEM;
	}

	int rc = ea_mount_scan(fs);
	if (rc) {
		free(fs->ea_lbt);
		free(fs->ea_live);
		free(fs->ea_sst);
		free(fs->ea_rd_cache);
		fs->ea_lbt = NULL;
		return rc;
	}
	return REALFS_OK;
}

void realfs_ea_dump(struct realfs *fs, int (*pf)(const char *fmt, ...))
{
	if (!fs || !fs->edev) {
		pf("realfs_ea_dump: not erase-aware mode\n");
		return;
	}
	const struct realfs_erasedev *e = fs->edev;
	uint32_t nfree = 0, nopen = 0, nsealed = 0, nsb = 0;
	uint32_t total_live = 0, sealed_live = 0;
	uint32_t min_live = 0xFFFFFFFFu, max_live = 0;
	/* Live histogram for sealed segs: count segs per live-page bucket */
	uint32_t hist_0    = 0;                /* 0 live pages (fully dead) */
	uint32_t hist_part = 0;               /* 1..seg_pages-1 (partial) */
	uint32_t hist_full = 0;               /* seg_pages (100% live) */
	for (uint32_t s = 0; s < e->seg_count; s++) {
		total_live += fs->ea_live[s];
		switch (fs->ea_sst[s]) {
		case EA_SEG_FREE:
			nfree++;
			break;
		case EA_SEG_OPEN:
			nopen++;
			break;
		case EA_SEG_SB_RING:
			nsb++;
			break;
		case EA_SEG_SEALED:
			nsealed++;
			sealed_live += fs->ea_live[s];
			if (fs->ea_live[s] < min_live) {
				min_live = fs->ea_live[s];
			}
			if (fs->ea_live[s] > max_live) {
				max_live = fs->ea_live[s];
			}
			if (fs->ea_live[s] == 0) {
				hist_0++;
			} else if (fs->ea_live[s] < e->seg_pages) {
				hist_part++;
			} else {
				hist_full++;
			}
			break;
		default:
			break;
		}
	}
	pf("RFS-EA free_segs(ctr)=%u | scan: free=%u open=%u sealed=%u sb=%u (of %u)\n",
	   (unsigned)fs->ea_free_segs, (unsigned)nfree, (unsigned)nopen,
	   (unsigned)nsealed, (unsigned)nsb, (unsigned)e->seg_count);
	pf("RFS-EA block_count=%u free_blocks=%u | live_pages=%u cap=%u\n",
	   (unsigned)fs->block_count, (unsigned)fs->free_block_count,
	   (unsigned)total_live, (unsigned)(e->seg_count * e->seg_pages));
	pf("RFS-EA sealed_live min=%u max=%u sum=%u (seg_pages=%u) open_seg=%u cur=%u\n",
	   (unsigned)(nsealed ? min_live : 0), (unsigned)max_live, (unsigned)sealed_live,
	   (unsigned)e->seg_pages, (unsigned)fs->ea_open_seg, (unsigned)fs->ea_page_cur);
	pf("RFS-EA sealed hist: full=%u partial=%u empty=%u (GC only picks partial/empty)\n",
	   (unsigned)hist_full, (unsigned)hist_part, (unsigned)hist_0);
	/* LBT consistency: count mapped vblks vs ea_live sum */
	uint32_t lbt_mapped = 0;
	for (uint32_t v = 0; v < fs->block_count; v++)
		if (ea_lbt_get(fs, v) != EA_LBT_NONE) {
			lbt_mapped++;
		}
	pf("RFS-EA LBT_mapped=%u ea_live_sum=%u (should match)\n",
	   (unsigned)lbt_mapped, (unsigned)total_live);
}

int realfs_ea_unmount(struct realfs *fs)
{
	if (!fs || !fs->edev) {
		return REALFS_ERR_INVAL;
	}
	free(fs->ea_lbt);
	free(fs->ea_live);
	free(fs->ea_sst);
	free(fs->ea_rd_cache);
	fs->ea_lbt  = NULL;
	fs->ea_live = NULL;
	fs->ea_sst  = NULL;
	fs->ea_rd_cache = NULL;
	fs->ea_rd_cache_vblk = EA_RD_CACHE_NONE;
	fs->edev      = NULL;
	return REALFS_OK;
}
