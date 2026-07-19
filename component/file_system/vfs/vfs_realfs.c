/*
 * VFS adapter for REALFS.
 *
 * REALFS stores whole files (create / atomic-replace), so this adapter bridges
 * POSIX-style streaming I/O onto it:
 *   - read-only opens read straight through realfs_read() with a position cursor;
 *   - any write/append/update mode buffers the whole file in RAM and flushes it
 *     with one realfs_write() on fflush/close (matches REALFS's write model and
 *     the KB~hundreds-KB UI-asset size range).
 *
 * Directories are virtual in REALFS (flat hash space), so mkdir/rmdir are
 * no-ops and readdir is unsupported for now (no enumeration API in the core).
 */
#include "platform_autoconf.h"

#if defined(CONFIG_VFS_REALFS_INCLUDED)

#include <string.h>
#include <sys/stat.h>
#include "ameba.h"
#include "vfs.h"
#include "vfs_realfs.h"
#include "os_wrapper.h"
#include "realfs.h"
#include "realfs_bdev_sd.h"
#include "realfs_bdev_flash.h"
#if defined(CONFIG_REALFS_FLASH_NAND_LBM)
#include "realfs_erasedev_lbm.h"
#endif
#if defined(CONFIG_REALFS_FLASH_NOR_MAP)
#include "realfs_erasedev_nor.h"
#endif

/* Erase-aware (route-B) mode is used by raw NAND (via LBM) and NOR@512B map. */
#if defined(CONFIG_REALFS_FLASH_NAND_LBM) || defined(CONFIG_REALFS_FLASH_NOR_MAP)
#define REALFS_EDEV_MODE   1
#endif

#define RFS_BLOCK_SIZE   4096u

/* NOR@512B map mode: physical page = 512 B (block = 496 B after the 16 B page
 * header); each 4 KB NOR sector is one 8-page segment. */
#define RFS_NOR_MAP_PAGE_SIZE   512u

/* REALFS-over-NOR-flash uses the VFS1 flash-layout partition (the same region
 * littlefs region 1 would use). amebalite has only VFS1 allocated; if a board
 * carves out VFS2/USER, point this at the partition you reserved for REALFS. */
#define RFS_FLASH_REGION   VFS1

volatile uint8_t realfs_mount_flag = 0;

static struct realfs      g_rfs;       /* mounted file system state (~17 KB BSS) */
static struct realfs_bdev g_bdev;
#if defined(REALFS_EDEV_MODE)
static struct realfs_erasedev g_edev;  /* erase-device backend (NAND-LBM / NOR-map) */
#endif

/* Per-open handle stored in vfs_file->file.
 * write_mode: 0 = read-only, 1 = streaming write ("w"), 2 = buffered write */
struct rfs_fh {
	int      write_mode;
	int      can_read;
	int      dirty;                    /* buffered write mode only */
	uint32_t pos;
	char     path[REALFS_MAX_NAME + 1];
	struct realfs_file     *rf;        /* read mode only */
	struct realfs_write_ctx *wctx;     /* streaming write mode (write_mode=1) */
	uint8_t *wbuf;                     /* buffered write mode (write_mode=2) */
	uint32_t wsize;
	uint32_t wcap;
};

/* ---- write-buffer helpers ------------------------------------------------- */
static int rfs_ensure(struct rfs_fh *h, uint32_t need)
{
	if (need <= h->wcap) {
		return 0;
	}
	uint32_t ncap = h->wcap ? h->wcap : 256;
	while (ncap < need) {
		ncap <<= 1;
	}
	uint8_t *nb = (uint8_t *)rtos_mem_malloc(ncap);
	if (!nb) {
		return -1;
	}
	if (h->wsize) {
		memcpy(nb, h->wbuf, h->wsize);
	}
	if (h->wbuf) {
		rtos_mem_free(h->wbuf);
	}
	h->wbuf = nb;
	h->wcap = ncap;
	return 0;
}

/* Load the current on-disk contents into the write buffer (for a/a+/r+). */
static int rfs_preload(struct rfs_fh *h, int required)
{
	struct realfs_file rf;
	int rc = realfs_open(&g_rfs, h->path, &rf);
	if (rc) {
		return required ? -1 : 0;      /* not found */
	}
	uint32_t sz = rf.file_size;
	if (sz) {
		h->wbuf = (uint8_t *)rtos_mem_malloc(sz);
		if (!h->wbuf) {
			realfs_close(&rf);
			return -1;
		}
		int n = realfs_read(&rf, h->wbuf, 0, sz);
		realfs_close(&rf);
		if (n != (int)sz) {
			rtos_mem_free(h->wbuf);
			h->wbuf = NULL;
			return -1;
		}
		h->wsize = sz;
		h->wcap  = sz;
	} else {
		realfs_close(&rf);
	}
	return 0;
}

/* ---- file ops ------------------------------------------------------------- */
static int realfs_vfs_open(void *fs, const char *filename, const char *mode, vfs_file *finfo)
{
	(void)fs;
	if (!realfs_mount_flag || !filename || !mode) {
		return -1;
	}
	size_t pl = strlen(filename);
	if (pl == 0 || pl > REALFS_MAX_NAME) {
		return -1;
	}

	struct rfs_fh *h = (struct rfs_fh *)rtos_mem_malloc(sizeof(*h));
	if (!h) {
		return -1;
	}
	memset(h, 0, sizeof(*h));
	memcpy(h->path, filename, pl);
	h->path[pl] = '\0';

	int has_r = (strchr(mode, 'r') != NULL);
	int has_w = (strchr(mode, 'w') != NULL);
	int has_a = (strchr(mode, 'a') != NULL);
	int has_p = (strchr(mode, '+') != NULL);

	if (!has_w && !has_a && !has_p) {
		/* pure read */
		h->rf = (struct realfs_file *)rtos_mem_malloc(sizeof(*h->rf));
		if (!h->rf) {
			rtos_mem_free(h);
			return -1;
		}
		if (realfs_open(&g_rfs, h->path, h->rf)) {
			rtos_mem_free(h->rf);
			rtos_mem_free(h);
			return -1;
		}
		h->write_mode = 0;
		h->can_read   = 1;
	} else {
		h->can_read = (has_r || has_p);

		if (has_w && !has_p) {
			/* pure "w": streaming write — data blocks written eagerly, commit at fclose */
			h->write_mode = 1;
			h->wctx = (struct realfs_write_ctx *)rtos_mem_malloc(sizeof(*h->wctx));
			if (!h->wctx) {
				rtos_mem_free(h);
				return -1;
			}
			if (realfs_write_begin(&g_rfs, filename, h->wctx)) {
				rtos_mem_free(h->wctx);
				rtos_mem_free(h);
				return -1;
			}
		} else {
			/* w+, a, a+, r+: buffered write */
			h->write_mode = 2;
			if (has_w) {
				/* w+ : truncate to empty, nothing to preload */
			} else if (has_a) {
				if (rfs_preload(h, 0)) {   /* a / a+ : keep existing if any */
					rtos_mem_free(h);
					return -1;
				}
				h->pos = h->wsize;         /* append at end */
			} else {
				/* r+ : update in place, file must exist */
				if (rfs_preload(h, 1)) {
					rtos_mem_free(h);
					return -1;
				}
			}
		}
	}

	finfo->file = (void *)h;
	return 0;
}

static int realfs_vfs_read(void *fs, unsigned char *buf, unsigned int size,
						   unsigned int count, vfs_file *finfo)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	if (!h || !h->can_read) {
		return -1;
	}
	uint32_t n = size * count;
	if (n == 0) {
		return 0;
	}

	if (h->write_mode) {
		if (h->pos >= h->wsize) {
			return 0;
		}
		uint32_t avail = h->wsize - h->pos;
		if (n > avail) {
			n = avail;
		}
		memcpy(buf, h->wbuf + h->pos, n);
		h->pos += n;
		return (int)n;
	}

	int rc = realfs_read(h->rf, buf, h->pos, n);
	if (rc < 0) {
		return -1;
	}
	h->pos += (uint32_t)rc;
	return rc;
}

static int realfs_vfs_write(void *fs, unsigned char *buf, unsigned int size,
							unsigned int count, vfs_file *finfo)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	if (!h || !h->write_mode) {
		return -1;
	}
	uint32_t n = size * count;
	if (n == 0) {
		return 0;
	}

	if (h->write_mode == 1) {
		/* streaming: feed directly to write_chunk */
		if (realfs_write_chunk(h->wctx, buf, n)) {
			return -1;
		}
		h->pos += n;
		return (int)n;
	}

	/* buffered write (write_mode == 2) */
	if (rfs_ensure(h, h->pos + n)) {
		return -1;
	}
	if (h->pos > h->wsize) {
		memset(h->wbuf + h->wsize, 0, h->pos - h->wsize);
	}
	memcpy(h->wbuf + h->pos, buf, n);
	h->pos += n;
	if (h->pos > h->wsize) {
		h->wsize = h->pos;
	}
	h->dirty = 1;
	return (int)n;
}

static int realfs_vfs_fflush(void *fs, vfs_file *finfo)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	if (!h) {
		return -1;
	}
	if (h->write_mode == 1) {
		/* streaming: fflush is a no-op (data already on flash, commit at fclose) */
		return 0;
	}
	if (h->write_mode == 2 && h->dirty) {
		int rc = realfs_write(&g_rfs, h->path, h->wsize ? h->wbuf : NULL, h->wsize);
		if (rc == REALFS_ERR_FRAGMENTED) {
			VFS_DBG(VFS_ERROR, "realfs: write failed - filesystem too fragmented, defrag needed");
		}
		if (rc) {
			return -1;
		}
		h->dirty = 0;
	}
	return 0;
}

static int realfs_vfs_close(void *fs, vfs_file *finfo)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	if (!h) {
		return -1;
	}
	int ret = 0;
	if (h->write_mode == 1) {
		if (h->wctx->active) {
			int rc = realfs_write_end(h->wctx);
			if (rc == REALFS_ERR_FRAGMENTED) {
				VFS_DBG(VFS_ERROR, "realfs: write failed - filesystem too fragmented, defrag needed");
			}
			ret = rc ? -1 : 0;
		} else {
			realfs_write_abort(h->wctx);
			ret = -1;
		}
		rtos_mem_free(h->wctx);
	} else if (h->write_mode == 2) {
		ret = realfs_vfs_fflush(fs, finfo);
		if (h->wbuf) {
			rtos_mem_free(h->wbuf);
		}
	} else if (h->rf) {
		realfs_close(h->rf);
		rtos_mem_free(h->rf);
	}
	rtos_mem_free(h);
	finfo->file = NULL;
	return ret;
}

static uint32_t rfs_cur_size(struct rfs_fh *h)
{
	if (h->write_mode == 1) {
		return h->wctx ? h->wctx->total_written : 0;
	}
	if (h->write_mode == 2) {
		return h->wsize;
	}
	return h->rf ? h->rf->file_size : 0;
}

static int realfs_vfs_seek(void *fs, long int offset, int origin, vfs_file *finfo)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	if (!h) {
		return -1;
	}
	long base;
	switch (origin) {
	case SEEK_SET:
		base = 0;
		break;
	case SEEK_CUR:
		base = (long)h->pos;
		break;
	case SEEK_END:
		base = (long)rfs_cur_size(h);
		break;
	default:
		return -1;
	}
	long np = base + offset;
	if (np < 0) {
		return -1;
	}
	/* read mode cannot seek past EOF; write mode may (creates a hole on write) */
	if (!h->write_mode && (uint32_t)np > rfs_cur_size(h)) {
		np = (long)rfs_cur_size(h);
	}
	h->pos = (uint32_t)np;
	return 0;
}

static void realfs_vfs_rewind(void *fs, vfs_file *finfo)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	if (h) {
		h->pos = 0;
	}
}

static int realfs_vfs_tell(void *fs, vfs_file *finfo)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	return h ? (int)h->pos : -1;
}

static int realfs_vfs_fgetpos(void *fs, vfs_file *finfo)
{
	return realfs_vfs_tell(fs, finfo);
}

static int realfs_vfs_fsetpos(void *fs, unsigned int offset, vfs_file *finfo)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	if (!h) {
		return -1;
	}
	h->pos = offset;
	return 0;
}

static int realfs_vfs_eof(void *fs, vfs_file *finfo)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	if (!h) {
		return -1;
	}
	return (h->pos >= rfs_cur_size(h)) ? 1 : 0;
}

static int realfs_vfs_error(vfs_file *finfo)
{
	(void)finfo;
	return 0;
}

static int realfs_vfs_ftruncate(void *fs, vfs_file *finfo, off_t length)
{
	(void)fs;
	struct rfs_fh *h = (struct rfs_fh *)finfo->file;
	if (!h || !h->write_mode || length < 0) {
		return -1;
	}
	uint32_t len = (uint32_t)length;
	if (len > h->wsize) {
		if (rfs_ensure(h, len)) {
			return -1;
		}
		memset(h->wbuf + h->wsize, 0, len - h->wsize);
	}
	h->wsize = len;
	if (h->pos > len) {
		h->pos = len;
	}
	h->dirty = 1;
	return 0;
}

static int realfs_vfs_remove(void *fs, const char *name)
{
	(void)fs;
	if (!realfs_mount_flag) {
		return -1;
	}
	return realfs_delete(&g_rfs, name) ? -1 : 0;
}

static int realfs_vfs_rename(void *fs, const char *old_name, const char *new_name)
{
	(void)fs;
	if (!realfs_mount_flag) {
		return -1;
	}
	struct realfs_file rf;
	if (realfs_open(&g_rfs, old_name, &rf)) {
		return -1;
	}
	uint32_t sz = rf.file_size;
	uint8_t *buf = NULL;
	int ret = -1;
	if (sz) {
		buf = (uint8_t *)rtos_mem_malloc(sz);
		if (!buf) {
			realfs_close(&rf);
			return -1;
		}
		if (realfs_read(&rf, buf, 0, sz) != (int)sz) {
			goto out;
		}
	}
	if (realfs_write(&g_rfs, new_name, sz ? buf : NULL, sz)) {
		goto out;
	}
	realfs_delete(&g_rfs, old_name);
	ret = 0;
out:
	realfs_close(&rf);
	if (buf) {
		rtos_mem_free(buf);
	}
	return ret;
}

static int realfs_vfs_stat(void *fs, char *path, struct stat *buf)
{
	(void)fs;
	if (!realfs_mount_flag) {
		return -1;
	}
	struct realfs_stat s;
	if (realfs_stat(&g_rfs, path, &s)) {
		return -1;
	}
	memset(buf, 0, sizeof(*buf));
	buf->st_mode  = S_IFREG | 0777;
	buf->st_size  = s.size;
	buf->st_nlink = 1;
	return 0;
}

static int realfs_vfs_access(void *fs, const char *pathname, int mode)
{
	(void)fs;
	(void)mode;
	if (!realfs_mount_flag) {
		return -1;
	}
	struct realfs_stat s;
	return realfs_stat(&g_rfs, pathname, &s) ? -1 : 0;
}

/* Directories are virtual (flat hash space): creating/removing them is a no-op
 * success; listing is not supported until the core grows an enumeration API. */
static int realfs_vfs_mkdir(void *fs, const char *pathname)
{
	(void)fs;
	(void)pathname;
	return 0;
}
static int realfs_vfs_rmdir(void *fs, const char *path)
{
	(void)fs;
	(void)path;
	return 0;
}
static int realfs_vfs_opendir(void *fs, const char *name, vfs_file *finfo)
{
	(void)fs;
	(void)name;
	(void)finfo;
	return -1;
}
static struct dirent *realfs_vfs_readdir(void *fs, vfs_file *finfo)
{
	(void)fs;
	(void)finfo;
	return NULL;
}
static int realfs_vfs_closedir(void *fs, vfs_file *finfo)
{
	(void)fs;
	(void)finfo;
	return 0;
}

static int realfs_vfs_get_interface(int interface)
{
	return interface;
}

/* Track which backend the active bdev came from so unmount tears down the
 * right one. -1 = none. */
static int g_bdev_inf = -1;

/* Open a backend for the requested interface.
 * On NAND targets (SHOULD_USE_NAND()) the erase-device path is taken and
 * realfs_ea_mount() is called directly; the bdev pointer is not used.
 * On NOR / SD targets the existing bdev path is used unchanged. */
static int realfs_open_bdev(int interface, struct realfs_bdev *bdev, int for_format)
{
#if defined(CONFIG_REALFS_SD_MODE)
	if (interface == VFS_INF_SD) {
		return realfs_bdev_sd_open(bdev, RFS_BLOCK_SIZE);
	}
#endif
#if defined(CONFIG_REALFS_FLASH)
	if (interface == VFS_INF_FLASH) {
#if defined(CONFIG_REALFS_FLASH_NAND_LBM)
		/* Runtime selection: NAND vs NOR (mirrors littlefs_adapter.c) */
		if (SHOULD_USE_NAND()) {
			int rc = realfs_erasedev_lbm_open(&g_edev);
			if (rc != 0) {
				VFS_DBG(VFS_ERROR, "REALFS: LBM init failed");
				return -1;
			}
			/* Format path: realfs_ea_format; mount path handled in caller */
			(void)bdev;
			(void)for_format;
			return 0;   /* signal: NAND path active, caller uses g_edev */
		}
#endif /* CONFIG_REALFS_FLASH_NAND_LBM */
		u32 start = 0, end = 0;
		flash_get_layout_info(RFS_FLASH_REGION, &start, &end);
		if (start == 0xFFFFFFFF || end == 0xFFFFFFFF || end <= start) {
			VFS_DBG(VFS_ERROR, "REALFS flash region (VFS layout) not allocated");
			return -1;
		}
		uint32_t off  = start - SPI_FLASH_BASE;
		uint32_t size = (end - start) + 1;
#if defined(CONFIG_REALFS_FLASH_NOR_MAP)
		/* Erase-aware (route-B) 512 B map over raw NOR; edev path (uses g_edev). */
		if (realfs_erasedev_nor_open(&g_edev, off, size, RFS_NOR_MAP_PAGE_SIZE) != 0) {
			VFS_DBG(VFS_ERROR, "REALFS: NOR map init failed");
			return -1;
		}
		(void)bdev;
		(void)for_format;
		return 0;
#else
		(void)for_format;
		return realfs_bdev_flash_open(bdev, off, size, RFS_BLOCK_SIZE);
#endif
	}
#endif
	(void)for_format;
	VFS_DBG(VFS_ERROR, "REALFS: unsupported interface %d", interface);
	return -1;
}

/* Returns non-zero if `interface` uses the erase-aware (route-B) edev path
 * (raw NAND via LBM, or NOR@512B map) rather than the rewritable-block bdev. */
static int realfs_is_edev(int interface)
{
	if (interface != VFS_INF_FLASH) {
		return 0;
	}
#if defined(CONFIG_REALFS_FLASH_NAND_LBM)
	if (SHOULD_USE_NAND()) {
		return 1;
	}
#endif
#if defined(CONFIG_REALFS_FLASH_NOR_MAP)
	return 1;   /* NOR-map targets have no NAND; edev is always used on flash */
#endif
	return 0;
}

#if defined(REALFS_EDEV_MODE)
/* Tear down whichever erase-device backend realfs_open_bdev opened. */
static void realfs_edev_close(void)
{
#if defined(CONFIG_REALFS_FLASH_NAND_LBM)
	if (SHOULD_USE_NAND()) {
		realfs_erasedev_lbm_close(&g_edev);
		return;
	}
#endif
#if defined(CONFIG_REALFS_FLASH_NOR_MAP)
	realfs_erasedev_nor_close(&g_edev);
#endif
}
#endif

static void realfs_close_bdev(int interface, struct realfs_bdev *bdev)
{
#if defined(CONFIG_REALFS_SD_MODE)
	if (interface == VFS_INF_SD) {
		realfs_bdev_sd_close(bdev);
		return;
	}
#endif
#if defined(CONFIG_REALFS_FLASH)
	if (interface == VFS_INF_FLASH) {
#if defined(REALFS_EDEV_MODE)
		if (realfs_is_edev(interface)) {
			realfs_edev_close();
			return;
		}
#endif
		realfs_bdev_flash_close(bdev);
	}
#endif
}

static int realfs_vfs_mount(int interface)
{
	if (realfs_mount_flag) {
		return 0;
	}
	if (realfs_open_bdev(interface, &g_bdev, 0) != 0) {
		return -1;
	}

	int rc;
#if defined(REALFS_EDEV_MODE)
	if (realfs_is_edev(interface)) {
		rc = realfs_ea_mount(&g_rfs, &g_edev);
		if (rc) {
			VFS_DBG(VFS_ERROR, "realfs edev mount failed (%d); format first", rc);
			realfs_edev_close();
			return -1;
		}
		g_bdev_inf = interface;
		realfs_mount_flag = 1;
		return 0;
	}
#endif
	rc = realfs_mount(&g_rfs, &g_bdev);
	if (rc) {
		VFS_DBG(VFS_ERROR, "realfs_mount failed (%d); format the device first", rc);
		realfs_close_bdev(interface, &g_bdev);
		return -1;
	}
	g_bdev_inf = interface;
	realfs_mount_flag = 1;
	return 0;
}

static int realfs_vfs_unmount(int interface)
{
	(void)interface;
	if (realfs_mount_flag) {
#if defined(REALFS_EDEV_MODE)
		if (realfs_is_edev(g_bdev_inf)) {
			realfs_ea_unmount(&g_rfs);
			realfs_edev_close();
			g_bdev_inf = -1;
			realfs_mount_flag = 0;
			return 0;
		}
#endif
		realfs_unmount(&g_rfs);
		realfs_close_bdev(g_bdev_inf, &g_bdev);
		g_bdev_inf = -1;
		realfs_mount_flag = 0;
	}
	return 0;
}

/* Format the device behind `interface` (VFS_INF_SD / VFS_INF_FLASH). */
static int realfs_format_inf(int interface, unsigned int hash_buckets)
{
	if (realfs_mount_flag) {
		VFS_DBG(VFS_ERROR, "unmount REALFS before formatting");
		return -1;
	}
	if (realfs_open_bdev(interface, &g_bdev, 1) != 0) {
		return -1;
	}
#if defined(REALFS_EDEV_MODE)
	if (realfs_is_edev(interface)) {
		int rc = realfs_ea_format(&g_edev, (uint32_t)hash_buckets);
		realfs_edev_close();
		return rc ? -1 : 0;
	}
#endif
	int rc = realfs_format(&g_bdev, hash_buckets);
	realfs_close_bdev(interface, &g_bdev);
	return rc ? -1 : 0;
}

#if defined(CONFIG_REALFS_SD_MODE)
int realfs_sd_format(unsigned int hash_buckets)
{
	return realfs_format_inf(VFS_INF_SD, hash_buckets);
}
#endif

int realfs_flash_format(unsigned int hash_buckets)
{
	return realfs_format_inf(VFS_INF_FLASH, hash_buckets);
}

#ifdef REALFS_ENABLE_DEBUG
/* DiagPrintf (declared u32-returning in diag.h via ameba.h) is ABI-compatible
 * with realfs_printf_t (int-returning, same varargs); cast at the call site. */
#define REALFS_DBG_PF  ((realfs_printf_t)DiagPrintf)

void realfs_vfs_dump(void)
{
	if (!realfs_mount_flag) {
		DiagPrintf("REALFS not mounted\r\n");
		return;
	}
	realfs_dump_all(&g_rfs, REALFS_DBG_PF);
}

int realfs_vfs_check(void)
{
	if (!realfs_mount_flag) {
		DiagPrintf("REALFS not mounted\r\n");
		return -1;
	}
	return realfs_check(&g_rfs, REALFS_DBG_PF);
}
#endif /* REALFS_ENABLE_DEBUG */

#if defined(REALFS_EDEV_MODE)
/* Erase-aware segment/GC accounting dump (NAND-LBM or NOR-map) -- available
 * regardless of REALFS_ENABLE_DEBUG so benchmark examples (fs_gc) can print
 * allocator state on ENOSPC. */
void realfs_vfs_ea_dump(void)
{
	if (!realfs_mount_flag) {
		DiagPrintf("REALFS not mounted\r\n");
		return;
	}
	realfs_ea_dump(&g_rfs, (int (*)(const char *, ...))DiagPrintf);
}

uint32_t realfs_vfs_ea_free_segs(void)
{
	if (!realfs_mount_flag || !g_rfs.edev) {
		return 0;
	}
	return g_rfs.ea_free_segs;
}
#endif

const vfs_opt realfs_drv = {
	.open          = realfs_vfs_open,
	.read          = realfs_vfs_read,
	.write         = realfs_vfs_write,
	.close         = realfs_vfs_close,
	.seek          = realfs_vfs_seek,
	.rewind        = realfs_vfs_rewind,
	.fgetpos       = realfs_vfs_fgetpos,
	.fsetpos       = realfs_vfs_fsetpos,
	.fflush        = realfs_vfs_fflush,
	.remove        = realfs_vfs_remove,
	.rename        = realfs_vfs_rename,
	.eof           = realfs_vfs_eof,
	.error         = realfs_vfs_error,
	.tell          = realfs_vfs_tell,
	.ftruncate     = realfs_vfs_ftruncate,
	.opendir       = realfs_vfs_opendir,
	.readdir       = realfs_vfs_readdir,
	.closedir      = realfs_vfs_closedir,
	.rmdir         = realfs_vfs_rmdir,
	.mkdir         = realfs_vfs_mkdir,
	.access        = realfs_vfs_access,
	.stat          = realfs_vfs_stat,
	.mount         = realfs_vfs_mount,
	.unmount       = realfs_vfs_unmount,
	.get_interface = realfs_vfs_get_interface,
	.TAG           = "realfs",
	.vfs_type      = VFS_REALFS,
};

#endif /* CONFIG_VFS_REALFS_INCLUDED */
