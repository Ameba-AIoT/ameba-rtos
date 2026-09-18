#ifndef TSF_TIME_H
#define TSF_TIME_H

#include <stdint.h>

/* Optional: the first read maps automatically. 0 = ok, -1 = open, -2 = mmap. */
int  tsf_time_open(void);

/* 0 = ok; < 0 = not synced or latch stale, *tsf_us then unusable. */
int  tsf_now_us(uint64_t *tsf_us);
int  tsf_at_mono_us(uint64_t event_mono_ns, uint64_t *tsf_us);

void tsf_time_close(void);

#endif /* TSF_TIME_H */
