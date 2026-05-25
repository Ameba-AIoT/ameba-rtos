/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_TIME_H
#define ZEPHYR_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <os_wrapper_time.h>

typedef unsigned int k_ticks_t;

#define K_TICKS_FOREVER ((k_ticks_t) -1)

typedef struct {
	k_ticks_t ticks;
} k_timeout_t;

struct _timeout;
typedef void (*_timeout_func_t)(struct _timeout *t);

struct _timeout {
	void *handle;
	_timeout_func_t fn;
	uint64_t to_ticks;
};

#define MSEC_PER_SEC 1000U

#define Z_TIMEOUT_TICKS(t) ((k_timeout_t) { .ticks = (t) })

#define K_NO_WAIT 	((k_timeout_t){0})
#define K_FOREVER	Z_TIMEOUT_TICKS(0xFFFFFFFF)

#define z_systime_to_ms(_systime)  (_systime)
#define z_ms_to_systime(_ms)  (_ms)

#define K_MSEC(ms)    (Z_TIMEOUT_TICKS(z_ms_to_systime(MAX((ms), 0))))
#define K_SECONDS(s)   K_MSEC((s) * 1000U)
#define K_MINUTES(m)   K_SECONDS((m) * 60)
#define K_HOURS(h)     K_MINUTES((h) * 60)
#define SYS_FOREVER_MS    K_TICKS_FOREVER
#define SYS_TIMEOUT_MS(ms) ((ms) == SYS_FOREVER_MS ? K_FOREVER : K_MSEC(ms))


#define K_TIMEOUT_EQ(a, b) ((a).ticks == (b).ticks)

#define z_init_timeout(__arg)
int64_t sys_clock_tick_get(void);
uint64_t sys_clock_timeout_end_calc(k_timeout_t timeout);
void z_add_timeout(struct _timeout *to, _timeout_func_t fn, k_timeout_t timeout);
int z_abort_timeout(struct _timeout *to);
k_ticks_t z_timeout_remaining(const struct _timeout *timeout);
int32_t k_sleep(k_timeout_t timeout);
int64_t k_uptime_get(void);
uint32_t k_uptime_get_32(void);
int64_t k_uptime_delta(int64_t *reftime);
uint32_t k_ticks_to_ms_floor32(uint32_t t);

#ifdef __cplusplus
}
#endif

#endif
