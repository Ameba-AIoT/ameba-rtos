#include "tsf_time.h"
#include "tsf_latch.h"

#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <sys/mman.h>
#include <sys/stat.h>

static struct tsf_latch *g_latch = NULL;
static pthread_mutex_t g_init_lock = PTHREAD_MUTEX_INITIALIZER;

static inline uint64_t ts_to_ns(const struct timespec *t)
{
	return (uint64_t)t->tv_sec * 1000000000ULL + (uint64_t)t->tv_nsec;
}

int tsf_time_open(void)
{
	struct tsf_latch *p;
	int fd;

	if (__atomic_load_n(&g_latch, __ATOMIC_ACQUIRE)) {
		return 0;
	}

	fd = open(TSF_LATCH_DEV, O_RDONLY | O_CLOEXEC);
	if (fd < 0) {
		return -1;
	}

	p = mmap(NULL, TSF_LATCH_MAP_SZ, PROT_READ, MAP_SHARED, fd, 0);

	close(fd);   /* the VMA keeps its own reference to the struct file */

	if (p == MAP_FAILED) {
		return -2;
	}

	__atomic_store_n(&g_latch, p, __ATOMIC_RELEASE);
	return 0;
}

/* Keeps retrying while the driver is not loaded: the app may start first. */
static struct tsf_latch *ensure_mapped(void)
{
	struct tsf_latch *p = __atomic_load_n(&g_latch, __ATOMIC_ACQUIRE);

	if (p) {
		return p;
	}

	pthread_mutex_lock(&g_init_lock);
	tsf_time_open();
	p = __atomic_load_n(&g_latch, __ATOMIC_ACQUIRE);
	pthread_mutex_unlock(&g_init_lock);

	return p;
}

/* Both seq checks are required, else the snapshot can be torn. */
static int latch_snapshot(struct tsf_latch *snap)
{
	struct tsf_latch *l = ensure_mapped();
	int retry = 0;

	if (!l) {
		return -1;
	}

	for (;;) {
		uint32_t s1 = l->seq;

		if (!(s1 & 1u)) {
			__sync_synchronize();
			*snap = *l;
			__sync_synchronize();
			if (s1 == l->seq) {
				break;
			}
		}
		if (++retry > 1000) {   /* writer died mid-update */
			return -1;
		}
	}

	return snap->valid ? 0 : -1;
}

static int tsf_interp(uint64_t event_mono_ns, const struct tsf_latch *snap, uint64_t *tsf_us)
{
	uint64_t d = (event_mono_ns > snap->update_mono_ns)
				 ? (event_mono_ns - snap->update_mono_ns)
				 : (snap->update_mono_ns - event_mono_ns);
	if (d > TSF_STALE_NS) {
		return -1;
	}

	/* Sign handled explicitly: uint64_t would underflow. */
	if (event_mono_ns >= snap->H_mono_ns) {
		*tsf_us = snap->TSF_latch_us + (event_mono_ns - snap->H_mono_ns) / 1000ULL;
	} else {
		uint64_t back_us = (snap->H_mono_ns - event_mono_ns) / 1000ULL;
		if (back_us > snap->TSF_latch_us) {
			return -1;
		}
		*tsf_us = snap->TSF_latch_us - back_us;
	}
	return 0;
}

int tsf_now_us(uint64_t *tsf_us)
{
	struct tsf_latch snap;
	struct timespec now;

	if (!tsf_us || latch_snapshot(&snap) < 0) {
		return -1;
	}
	clock_gettime(CLOCK_MONOTONIC_RAW, &now);
	return tsf_interp(ts_to_ns(&now), &snap, tsf_us);
}

int tsf_at_mono_us(uint64_t event_mono_ns, uint64_t *tsf_us)
{
	struct tsf_latch snap;

	if (!tsf_us || latch_snapshot(&snap) < 0) {
		return -1;
	}
	return tsf_interp(event_mono_ns, &snap, tsf_us);
}

void tsf_time_close(void)
{
	struct tsf_latch *p;

	pthread_mutex_lock(&g_init_lock);
	p = __atomic_load_n(&g_latch, __ATOMIC_ACQUIRE);
	if (p) {
		__atomic_store_n(&g_latch, NULL, __ATOMIC_RELEASE);
		munmap(p, TSF_LATCH_MAP_SZ);
	}
	pthread_mutex_unlock(&g_init_lock);
}
