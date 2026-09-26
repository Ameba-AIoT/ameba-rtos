/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <zephyr_mutex.h>
#include <osif.h>

int k_mutex_init(struct k_mutex *mutex)
{
    if (osif_recursive_mutex_create(&mutex->p_handle)) {
        return 0;
    }
    
    return -1;
}

int k_mutex_lock(struct k_mutex *mutex, k_timeout_t timeout)
{
    if (osif_recursive_mutex_take(mutex->p_handle, timeout.ticks)) {
        return 0;
    }
    
    return -1;
}

int k_mutex_unlock(struct k_mutex *mutex)
{
    if (osif_recursive_mutex_give(mutex->p_handle)) {
        return 0;
    }
    
    return -1;
}

int k_mutex_deinit(struct k_mutex *mutex)
{
    if (osif_recursive_mutex_delete(mutex->p_handle)) {
        return 0;
    }
    
    return -1;
}
