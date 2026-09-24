/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <zephyr_time.h>
#include <zephyr_lock.h>
#include <osif.h>
#include <zephyr/sys/util.h>

static struct k_spinlock lock;

#define TIMER_ARRAY_MAX 20
struct _timeout *timer_array[TIMER_ARRAY_MAX] = {0};

int64_t sys_clock_tick_get(void)
{
    return (int64_t)rtos_time_get_current_system_time_ms();
}

uint64_t sys_clock_timeout_end_calc(k_timeout_t timeout)
{
	k_ticks_t dt;

	if (K_TIMEOUT_EQ(timeout, K_FOREVER)) {
		return UINT64_MAX;
	} else if (K_TIMEOUT_EQ(timeout, K_NO_WAIT)) {
		return sys_clock_tick_get();
	} else {

		dt = timeout.ticks;

		return sys_clock_tick_get() + MAX(1, dt);
	}
}

static void _timeout_fn(void *arg)
{
	struct _timeout *to = NULL;
	k_spinlock_key_t key;
	int idx;

	key = k_spin_lock(&lock);
	for (idx = 0; idx < TIMER_ARRAY_MAX; idx++) {
		if (timer_array[idx] && (timer_array[idx]->handle == arg)) {
			to = timer_array[idx];
			break;
		}
	}
	k_spin_unlock(&lock, key);

	if (to) {
		z_abort_timeout(to);
		to->fn(to);
	}
}

void z_add_timeout(struct _timeout *to, _timeout_func_t fn,
		   k_timeout_t timeout)
{
	k_spinlock_key_t key;
	int idx;

    to->fn = fn;

	if (osif_timer_create(&to->handle, "", 0, z_systime_to_ms(timeout.ticks), false, _timeout_fn)) {
		to->to_ticks = sys_clock_timeout_end_calc(timeout);
		key = k_spin_lock(&lock);
		for (idx = 0; idx < TIMER_ARRAY_MAX; idx++) {
			if (timer_array[idx] == NULL) {
				timer_array[idx] = to;
				break;
			}
		}
		k_spin_unlock(&lock, key);
		osif_timer_start(&to->handle);
	}
}

int z_abort_timeout(struct _timeout *to)
{
	k_spinlock_key_t key;
    int idx;

	to->to_ticks = 0;
    osif_timer_stop(&to->handle);
    osif_timer_delete(&to->handle);

    key = k_spin_lock(&lock);
    for (idx = 0; idx < TIMER_ARRAY_MAX; idx++) {
        if (timer_array[idx] == to) {
            timer_array[idx] = NULL;
			break;
		}
    }
    k_spin_unlock(&lock, key);

	return 0;
}

static bool z_is_inactive_timeout(const struct _timeout *timeout)
{
	k_spinlock_key_t key;
    int idx;
	bool inactive = true;

    key = k_spin_lock(&lock);
    for (idx = 0; idx < TIMER_ARRAY_MAX; idx++) {
        if (timer_array[idx] == timeout) {
            inactive = false;
			break;
		}
    }
    k_spin_unlock(&lock, key);

	return inactive;
}

k_ticks_t z_timeout_remaining(const struct _timeout *timeout)
{
	if (z_is_inactive_timeout(timeout)) {
		return 0;
	}

	return (k_ticks_t)(timeout->to_ticks - sys_clock_tick_get());
}

int32_t k_sleep(k_timeout_t timeout)
{
	osif_delay(z_systime_to_ms(timeout.ticks));

	return 0;
}

int64_t k_uptime_get(void)
{
	return (int64_t)(z_systime_to_ms(rtos_time_get_current_system_time_ms()));
}

uint32_t k_uptime_get_32(void)
{
	return (uint32_t)k_uptime_get();
}

int64_t k_uptime_delta(int64_t *reftime)
{
	int64_t uptime, delta;

	uptime = k_uptime_get();
	delta = uptime - *reftime;
	*reftime = uptime;

	return delta;
}

/** @brief Convert ticks to milliseconds
 *
 * Converts time values in ticks to milliseconds.
 * Computes result in 32 bit precision.
 * Truncates to the next lowest output unit.
 *
 * @return The converted time value
 */
uint32_t k_ticks_to_ms_floor32(uint32_t t)
{
	return z_systime_to_ms(t);
}
