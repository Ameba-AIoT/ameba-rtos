/* can move to wifi adapter.c */
int rtos_task_create(rtos_task_t *pp_handle, const char *p_name, void (*p_routine)(void *),
					 void *p_param, size_t stack_size_in_byte, uint16_t priority)
{
	struct rt_thread *thread;
	thread = rt_thread_create(p_name, p_routine, p_param, stack_size_in_byte, priority, 10);

	if (thread != RT_NULL) {
		if (pp_handle) {
			*pp_handle = thread;
		}
		rt_thread_startup(thread);
		return 0;
	} else {
		return -1;
	}
}

int rtos_sema_create(rtos_sema_t *pp_handle, uint32_t init_count, uint32_t max_count)
{
	if (pp_handle == NULL || max_count > 65535) {
		return -1;
	}
	*pp_handle = (rtos_sema_t)rt_sem_create("rtos_sema", init_count, RT_IPC_FLAG_PRIO);
	if (*pp_handle != NULL) {
		return 0;
	}
	return -1;
}

int rtos_sema_delete(rtos_sema_t p_handle)
{
	if (p_handle == NULL) {
		return -1;
	}

	rt_err_t ret = rt_sem_delete((rt_sem_t)p_handle);
	return (ret == RT_EOK) ? 0 : -1;
}

int rtos_sema_take(rtos_sema_t p_handle, uint32_t wait_ms)
{
	if (p_handle == NULL) {
		return -1;
	}

	rt_int32_t ticks = (wait_ms == 0xFFFFFFFF) ?
					   RT_WAITING_FOREVER : rt_tick_from_millisecond(wait_ms);

	if (rt_interrupt_get_nest() > 0) {
		rt_err_t ret = rt_sem_trytake((rt_sem_t)p_handle);
		return (ret == RT_EOK) ? 0 : -1;
	} else {
		rt_err_t ret = rt_sem_take((rt_sem_t)p_handle, ticks);
		return (ret == RT_EOK) ? 0 : -1;
	}
}

int rtos_sema_give(rtos_sema_t p_handle)
{
	if (p_handle == NULL) {
		return -1;
	}

	rt_err_t ret = rt_sem_release((rt_sem_t)p_handle);

	return ret;
}

/*********************************************************************************************/
int rtos_mutex_create(rtos_mutex_t *pp_handle)
{
	if (pp_handle == NULL) {
		return -1;
	}

	*pp_handle = (rtos_mutex_t)rt_mutex_create("rtos_mutex", RT_IPC_FLAG_PRIO);
	if (*pp_handle != NULL) {
		return 0;
	}
	return -1;;

}

int rtos_mutex_take(rtos_mutex_t p_handle, uint32_t wait_ms)
{
	if (p_handle == NULL) {
		return -1;
	}

	rt_int32_t ticks = (wait_ms == RT_WAITING_FOREVER) ?
					   RT_WAITING_FOREVER : rt_tick_from_millisecond(wait_ms);

	rt_err_t ret = rt_mutex_take((rt_mutex_t)p_handle, ticks);
	return (ret == RT_EOK) ? 0 : -1;

}

int rtos_mutex_give(rtos_mutex_t p_handle)
{
	if (p_handle == NULL) {
		return -1;
	}

	rt_err_t ret = rt_mutex_release((rt_mutex_t)p_handle);
	return (ret == RT_EOK) ? 0 : -1;

}