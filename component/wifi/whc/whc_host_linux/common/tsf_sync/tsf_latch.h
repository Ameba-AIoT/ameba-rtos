#ifndef TSF_LATCH_H
#define TSF_LATCH_H

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/stddef.h>   /* offsetof */
#define TSF_U32  u32
#define TSF_U64  u64
#define TSF_S32  s32
#else
#include <stdint.h>
#include <stddef.h>
#define TSF_U32  uint32_t
#define TSF_U64  uint64_t
#define TSF_S32  int32_t
#endif

/* The kernel maps by page via remap_pfn_range(), so userspace must map a whole
 * page, not just sizeof(struct tsf_latch). */
#define TSF_LATCH_DEV      "/dev/tsf_latch"
#define TSF_LATCH_MAP_SZ   4096

/* No new sync point for 3s means the latch is stale (device offline, wifi
 * disconnected, or driver not loaded). */
#define TSF_STALE_NS       (3ULL * 1000000000ULL)

/* Latest sync point. seqlock protected: odd seq = write in progress. */
struct tsf_latch {
	volatile TSF_U32 seq;
	TSF_U32 sample_seq;        /* device sample seq, for debug / freshness */
	TSF_U64 H_mono_ns;         /* edge instant, CLOCK_MONOTONIC_RAW (ns) */
	TSF_U64 TSF_latch_us;      /* AP TSF at that instant (us) */
	TSF_U64 update_mono_ns;    /* last update (mono ns), used for stale check */
	TSF_S32 valid;             /* 1 = synced and usable */
};

/* If either side fails to build here, kernel and userspace would disagree on
 * the layout. Stop and investigate. */
_Static_assert(sizeof(struct tsf_latch) == 40, "tsf_latch size changed; kernel and userspace would misparse");
_Static_assert(offsetof(struct tsf_latch, seq) == 0, "tsf_latch.seq offset changed");
_Static_assert(offsetof(struct tsf_latch, sample_seq) == 4, "tsf_latch.sample_seq offset changed");
_Static_assert(offsetof(struct tsf_latch, H_mono_ns) == 8, "tsf_latch.H_mono_ns offset changed");
_Static_assert(offsetof(struct tsf_latch, TSF_latch_us) == 16, "tsf_latch.TSF_latch_us offset changed");
_Static_assert(offsetof(struct tsf_latch, update_mono_ns) == 24, "tsf_latch.update_mono_ns offset changed");
_Static_assert(offsetof(struct tsf_latch, valid) == 32, "tsf_latch.valid offset changed");

#endif /* TSF_LATCH_H */
