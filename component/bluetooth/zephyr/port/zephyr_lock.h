/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_SPINLOCK_H
#define ZEPHYR_SPINLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <osif.h>
#include <zephyr_port.h>

struct k_spinlock{
	char dummy;
};

struct z_spinlock_key {
	uint32_t key;
};
typedef struct z_spinlock_key k_spinlock_key_t;

static inline k_spinlock_key_t k_spin_lock(struct k_spinlock *l)
{
	ARG_UNUSED(l);
	k_spinlock_key_t k;
	k.key = osif_lock();
	return k;
}

static inline void k_spin_unlock(struct k_spinlock *l,
					k_spinlock_key_t key)
{
	ARG_UNUSED(l);
	osif_unlock(key.key);
}

#define irq_lock() (unsigned int)osif_lock()
#define irq_unlock(__key) osif_unlock((uint32_t)__key)

#define k_sched_lock osif_sched_suspend
#define k_sched_unlock osif_sched_resume

#ifdef __cplusplus
}
#endif

#endif
