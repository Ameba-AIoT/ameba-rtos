/* SPDX-License-Identifier: Apache-2.0 */
/*
 * REALFS erase-aware device abstraction (NAND mode).
 *
 * Exposes an erase-unit (segment) device: read any offset/length, program
 * page-aligned ranges (program-once within a segment), erase whole segments.
 * Used by REALFS NAND mode; NOR/SD mode keeps using realfs_bdev.h.
 */
#ifndef REALFS_ERASEDEV_H
#define REALFS_ERASEDEV_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct realfs_erasedev {
	/* Read any byte range within a segment.  off+len <= seg_pages*page_size.
	 * Returns 0 on success, <0 on error. */
	int (*seg_read)(const struct realfs_erasedev *dev, uint32_t seg,
					uint32_t off, uint32_t len, void *buf);

	/* Program page-aligned range within a segment.  off and len must both be
	 * multiples of page_size.  Pages must not have been programmed since the
	 * last erase of this segment (program-once).
	 * Returns 0 on success, <0 on error. */
	int (*seg_prog)(const struct realfs_erasedev *dev, uint32_t seg,
					uint32_t off, uint32_t len, const void *buf);

	/* Erase a whole segment (resets all pages to 0xFF, clears program-once
	 * state).  Returns 0 on success, <0 on error. */
	int (*seg_erase)(const struct realfs_erasedev *dev, uint32_t seg);

	/* Flush any device write buffer.  May be NULL if writes are synchronous. */
	int (*sync)(const struct realfs_erasedev *dev);

	uint32_t page_size;   /* bytes per page (program / read granularity) */
	uint32_t seg_pages;   /* data pages per segment (erase unit) */
	uint32_t seg_count;   /* total number of segments */

	void *ctx;
};

#ifdef __cplusplus
}
#endif

#endif /* REALFS_ERASEDEV_H */
