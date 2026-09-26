/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <zephyr_sema.h>
#include <zephyr_poll.h>
#include <osif.h>

int k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit)
{
    if (osif_sem_create(&sem->p_handle, initial_count, limit)) {
        sem->count = initial_count;
        sem->limit = limit;
	    sys_dlist_init(&sem->poll_events);
        return 0;
    }
    
    return -1;
}

int k_sem_take(struct k_sem *sem, k_timeout_t timeout)
{
    if (osif_sem_take(sem->p_handle, timeout.ticks)) {
        return 0;
    }
    
    return -1;
}

void k_sem_give(struct k_sem *sem)
{
    osif_sem_give(sem->p_handle);
	z_handle_obj_poll_events(&sem->poll_events, K_POLL_STATE_SEM_AVAILABLE);
}

uint32_t k_sem_count_get(struct k_sem *sem)
{
    return osif_sem_get_count(sem->p_handle);
}

int k_sem_deinit(struct k_sem *sem)
{
    if (osif_sem_delete(sem->p_handle)) {
        return 0;
    }
    
    return -1;
}
