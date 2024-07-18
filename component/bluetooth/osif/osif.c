/**
 * Copyright (c) 2015, Realsil Semiconductor Corporation. All rights reserved.
 */
#include <stdio.h>
#include <string.h>
#include <basic_types.h>
#include <os_wrapper.h>
#include <osif.h>
#include <bt_debug.h>

/****************************************************************************/
/* Check if in task context (true), or isr context (false)                  */
/****************************************************************************/
bool osif_task_context_check(void)
{
	return !rtos_critical_is_in_interrupt();
}

/****************************************************************************/
/* Delay current task in a given milliseconds                               */
/****************************************************************************/
void osif_delay(uint32_t ms)
{
	rtos_time_delay_ms(ms);
}

/****************************************************************************/
/* Get system time in milliseconds                                          */
/****************************************************************************/
uint32_t osif_sys_time_get(void)
{
	return rtos_time_get_current_system_time_ms();
}

/****************************************************************************/
/* Start os kernel scheduler                                                */
/****************************************************************************/
bool osif_sched_start(void)
{
	return (rtos_sched_start() == SUCCESS);
}

/****************************************************************************/
/* Stop os kernel scheduler                                                 */
/****************************************************************************/
bool osif_sched_stop(void)
{
	return (rtos_sched_stop() == SUCCESS);
}

/****************************************************************************/
/* Suspend os kernel scheduler                                              */
/****************************************************************************/
bool osif_sched_suspend(void)
{
	return (rtos_sched_suspend() == SUCCESS);
}

/****************************************************************************/
/* Resume os kernel scheduler                                               */
/****************************************************************************/
bool osif_sched_resume(void)
{
	return (rtos_sched_resume() == SUCCESS);
}

/****************************************************************************/
/* Check if os kernel scheduler suspended                                   */
/****************************************************************************/
bool osif_sched_is_suspended(void)
{
	return (rtos_sched_get_state() == RTOS_SCHED_SUSPENDED);
}

/****************************************************************************/
/* Create os level task routine                                             */
/****************************************************************************/
bool osif_task_create(void **pp_handle, const char *p_name, void (*p_routine)(void *),
					  void *p_param, uint16_t stack_size, uint16_t priority)
{
	return (rtos_task_create((rtos_task_t *)pp_handle, p_name, p_routine, p_param, stack_size, priority) == SUCCESS);
}

/****************************************************************************/
/* Delete os level task routine                                             */
/****************************************************************************/
bool osif_task_delete(void *p_handle)
{
	return (rtos_task_delete((rtos_task_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Suspend os level task routine                                            */
/****************************************************************************/
bool osif_task_suspend(void *p_handle)
{
	return (rtos_task_suspend((rtos_task_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Resume os level task routine                                             */
/****************************************************************************/
bool osif_task_resume(void *p_handle)
{
	return (rtos_task_resume((rtos_task_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Yield current os level task routine                                      */
/****************************************************************************/
bool osif_task_yield(void)
{
	return (rtos_task_yield() == SUCCESS);
}

/****************************************************************************/
/* Get current os level task routine handle                                 */
/****************************************************************************/
bool osif_task_handle_get(void **pp_handle)
{
	if (pp_handle == NULL) {
		BT_LOGE("%s: pp_handle is null\r\n", __func__);
		return false;
	}

	*pp_handle = (void *)rtos_task_handle_get();

	return true;
}

/****************************************************************************/
/* Get os level task routine priority                                       */
/****************************************************************************/
bool osif_task_priority_get(void *p_handle, uint16_t *p_priority)
{
	if (p_priority == NULL) {
		BT_LOGE("%s: p_priority is null\r\n", __func__);
		return false;
	}

	*p_priority = (uint16_t)rtos_task_priority_get((rtos_task_t)p_handle);

	return true;
}

/****************************************************************************/
/* Set os level task routine priority                                       */
/****************************************************************************/
bool osif_task_priority_set(void *p_handle, uint16_t priority)
{
	return (rtos_task_priority_set((rtos_task_t)p_handle, priority) == SUCCESS);
}

static rtos_sema_t sig_handle;
/****************************************************************************/
/* Create signal                                                            */
/****************************************************************************/
bool osif_signal_init(void)
{
	return (rtos_sema_create(&sig_handle, 0, 1) == SUCCESS);
}

/****************************************************************************/
/* Delete signal                                                            */
/****************************************************************************/
void osif_signal_deinit(void)
{
	if (sig_handle == NULL) {
		BT_LOGE("%s: sig_handle is null\r\n", __func__);
		return;
	}

	rtos_sema_delete(sig_handle);
	sig_handle = NULL;
}

/****************************************************************************/
/* Send signal to target task                                               */
/****************************************************************************/
bool osif_task_signal_send(void *p_handle, uint32_t signal)
{
	(void)p_handle;
	(void)signal;

	if (sig_handle == NULL) {
		BT_LOGE("%s: sig_handle is null\r\n", __func__);
		return false;
	}

	return (rtos_sema_give(sig_handle) == SUCCESS);
}

/****************************************************************************/
/* Receive signal in target task                                            */
/****************************************************************************/
bool osif_task_signal_recv(uint32_t *p_handle, uint32_t wait_ms)
{
	(void)p_handle;

	if (sig_handle == NULL) {
		BT_LOGE("%s: sig_handle is null\r\n", __func__);
		return false;
	}

	return (rtos_sema_take(sig_handle, wait_ms) == SUCCESS);
}

/****************************************************************************/
/* Clear signal in target task                                              */
/****************************************************************************/
bool osif_task_signal_clear(void *p_handle)
{
	(void)p_handle;

	return true;
}

/****************************************************************************/
/* Lock critical section                                                    */
/****************************************************************************/
uint32_t osif_lock(void)
{
	rtos_critical_enter();

	return 0;
}

/****************************************************************************/
/* Unlock critical section                                                  */
/****************************************************************************/
void osif_unlock(uint32_t flags)
{
	(void)flags;

	rtos_critical_exit();
}

/****************************************************************************/
/* Create counting semaphore                                                */
/****************************************************************************/
bool osif_sem_create(void **pp_handle, uint32_t init_count, uint32_t max_count)
{
	return (rtos_sema_create((rtos_sema_t *)pp_handle, init_count, max_count) == SUCCESS);
}

/****************************************************************************/
/* Delete counting semaphore                                                */
/****************************************************************************/
bool osif_sem_delete(void *p_handle)
{
	return (rtos_sema_delete((rtos_sema_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Take counting semaphore                                                  */
/****************************************************************************/
bool osif_sem_take(void *p_handle, uint32_t wait_ms)
{
	return (rtos_sema_take((rtos_sema_t)p_handle, wait_ms) == SUCCESS);
}

/****************************************************************************/
/* Give counting semaphore                                                  */
/****************************************************************************/
bool osif_sem_give(void *p_handle)
{
	return (rtos_sema_give((rtos_sema_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Get semaphore count                                                      */
/****************************************************************************/
uint32_t osif_sem_get_count(void *p_handle)
{
	return rtos_sema_get_count((rtos_sema_t)p_handle);
}

/****************************************************************************/
/* Create mutex                                                             */
/****************************************************************************/
bool osif_mutex_create(void **pp_handle)
{
	return (rtos_mutex_create((rtos_mutex_t *)pp_handle) == SUCCESS);
}

/****************************************************************************/
/* Delete mutex                                                             */
/****************************************************************************/
bool osif_mutex_delete(void *p_handle)
{
	return (rtos_mutex_delete((rtos_mutex_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Take mutex                                                               */
/****************************************************************************/
bool osif_mutex_take(void *p_handle, uint32_t wait_ms)
{
	return (rtos_mutex_take((rtos_mutex_t)p_handle, wait_ms) == SUCCESS);
}

/****************************************************************************/
/* Give mutex                                                               */
/****************************************************************************/
bool osif_mutex_give(void *p_handle)
{
	return (rtos_mutex_give((rtos_mutex_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Create recursive mutex                                                    */
/****************************************************************************/
bool osif_recursive_mutex_create(void **pp_handle)
{
	return (rtos_mutex_recursive_create((rtos_mutex_t *)pp_handle) == SUCCESS);
}

/****************************************************************************/
/* Delete recursive mutex                                                             */
/****************************************************************************/
bool osif_recursive_mutex_delete(void *p_handle)
{
	return (rtos_mutex_recursive_delete((rtos_mutex_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Take recursive mutex                                                               */
/****************************************************************************/
bool osif_recursive_mutex_take(void *p_handle, uint32_t wait_ms)
{
	return (rtos_mutex_recursive_take((rtos_mutex_t)p_handle, wait_ms) == SUCCESS);
}

/****************************************************************************/
/* Give recursive mutex                                                               */
/****************************************************************************/
bool osif_recursive_mutex_give(void *p_handle)
{
	return (rtos_mutex_recursive_give((rtos_mutex_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Create inter-thread message queue                                        */
/****************************************************************************/
bool osif_msg_queue_create(void **pp_handle, uint32_t msg_num, uint32_t msg_size)
{
	return (rtos_queue_create((rtos_queue_t *)pp_handle, msg_num, msg_size) == SUCCESS);
}

/****************************************************************************/
/* Delete inter-thread message queue                                        */
/****************************************************************************/
bool osif_msg_queue_delete(void *p_handle)
{
	return (rtos_queue_delete((rtos_queue_t)p_handle) == SUCCESS);
}

/****************************************************************************/
/* Peek inter-thread message queue's pending but not received msg number    */
/****************************************************************************/
bool osif_msg_queue_peek(void *p_handle, uint32_t *p_msg_num)
{
	if (p_msg_num == NULL) {
		BT_LOGE("%s: p_msg_num is null\r\n", __func__);
		return false;
	}

	*p_msg_num = rtos_queue_massage_waiting((rtos_queue_t)p_handle);

	return true;
}

/****************************************************************************/
/* Send inter-thread message                                                */
/****************************************************************************/
bool osif_msg_send(void *p_handle, void *p_msg, uint32_t wait_ms)
{
	return (rtos_queue_send((rtos_queue_t)p_handle, p_msg, wait_ms) == SUCCESS);
}

/****************************************************************************/
/* Receive inter-thread message                                             */
/****************************************************************************/
bool osif_msg_recv(void *p_handle, void *p_msg, uint32_t wait_ms)
{
	return (rtos_queue_receive((rtos_queue_t)p_handle, p_msg, wait_ms) == SUCCESS);
}

/****************************************************************************/
/* Peek inter-thread message                                                */
/****************************************************************************/
bool osif_msg_peek(void *p_handle, void *p_msg, uint32_t wait_ms)
{
	return (rtos_queue_peek((rtos_queue_t)p_handle, p_msg, wait_ms) == SUCCESS);
}

/****************************************************************************/
/* Allocate memory                                                          */
/****************************************************************************/
void *osif_mem_alloc(RAM_TYPE ram_type, size_t size)
{
	(void)ram_type;

	return rtos_mem_malloc(size);
}

/****************************************************************************/
/* Allocate aligned memory                                                  */
/****************************************************************************/
void *osif_mem_aligned_alloc(RAM_TYPE ram_type, size_t size, uint8_t alignment)
{
	(void)ram_type;
	void *p;
	void *p_aligned;

	if (alignment == 0) {
		alignment = 16;
	}

	p = rtos_mem_malloc(size + sizeof(void *) + alignment);
	if (p == NULL) {
		return p;
	}

	p_aligned = (void *)(((size_t)p + sizeof(void *) + alignment) & ~(alignment - 1));

	memcpy((uint8_t *)p_aligned - sizeof(void *), &p, sizeof(void *));

	return p_aligned;
}

/****************************************************************************/
/* Free memory                                                              */
/****************************************************************************/
void osif_mem_free(void *p_block)
{
	rtos_mem_free(p_block);
}

/****************************************************************************/
/* Free aligned memory                                                      */
/****************************************************************************/
void osif_mem_aligned_free(void *p_block)
{
	void *p;

	if (p_block == NULL) {
		return;
	}

	memcpy(&p, (uint8_t *)p_block - sizeof(void *), sizeof(void *));

	rtos_mem_free(p);
}

/****************************************************************************/
/* Peek unused (available) memory size                                      */
/****************************************************************************/
size_t osif_mem_peek(RAM_TYPE ram_type)
{
	(void)ram_type;

	return (size_t)rtos_mem_get_free_heap_size();
}

/****************************************************************************/
/* Get software timer ID                                                    */
/****************************************************************************/
bool osif_timer_id_get(void **pp_handle, uint32_t *p_timer_id)
{
	if (pp_handle == NULL || p_timer_id == NULL) {
		BT_LOGE("%s: pp_handle or p_timer_id is null\r\n", __func__);
		return false;
	}

	*p_timer_id = rtos_timer_get_id((rtos_timer_t)(*pp_handle));

	return true;
}

/****************************************************************************/
/* Create software timer                                                    */
/****************************************************************************/
bool osif_timer_create(void **pp_handle, const char *p_timer_name, uint32_t timer_id,
					   uint32_t interval_ms, bool reload, void (*p_timer_callback)(void *))
{
	return (rtos_timer_create((rtos_timer_t *)pp_handle, p_timer_name, timer_id, interval_ms, reload, p_timer_callback) == SUCCESS);
}

/****************************************************************************/
/* Start software timer                                                     */
/****************************************************************************/
bool osif_timer_start(void **pp_handle)
{
	if (pp_handle == NULL) {
		BT_LOGE("%s: pp_handle is null\r\n", __func__);
		return false;
	}

	return (rtos_timer_start((rtos_timer_t)(*pp_handle), 0) == SUCCESS);
}

/****************************************************************************/
/* Restart software timer                                                   */
/****************************************************************************/
bool osif_timer_restart(void **pp_handle, uint32_t interval_ms)
{
	if (pp_handle == NULL) {
		BT_LOGE("%s: pp_handle is null\r\n", __func__);
		return false;
	}

	return (rtos_timer_change_period((rtos_timer_t)(*pp_handle), interval_ms, 0) == SUCCESS);
}

/****************************************************************************/
/* Stop software timer                                                      */
/****************************************************************************/
bool osif_timer_stop(void **pp_handle)
{
	if (pp_handle == NULL) {
		BT_LOGE("%s: pp_handle is null\r\n", __func__);
		return false;
	}

	return (rtos_timer_stop((rtos_timer_t)(*pp_handle), 0) == SUCCESS);
}

/****************************************************************************/
/* Delete software timer                                                    */
/****************************************************************************/
bool osif_timer_delete(void **pp_handle)
{
	if (pp_handle == NULL) {
		BT_LOGE("%s: pp_handle is null\r\n", __func__);
		return false;
	}

	if (rtos_timer_delete((rtos_timer_t)(*pp_handle), 0) == SUCCESS) {
		*pp_handle = NULL;
		return true;
	}

	return false;
}

/****************************************************************************/
/* Get software timer state                                                 */
/****************************************************************************/
bool osif_timer_state_get(void **pp_handle, uint32_t *p_timer_state)
{
	if (pp_handle == NULL || p_timer_state == NULL) {
		BT_LOGE("%s: pp_handle or p_timer_state is null\r\n", __func__);
		return false;
	}

	*p_timer_state = rtos_timer_is_timer_active((rtos_timer_t)(*pp_handle));

	return true;
}

/****************************************************************************/
/* Dump software timer                                                      */
/****************************************************************************/
bool osif_timer_dump(void)
{
	return true;
}

/****************************************************************************/
/* Create secure context                                                    */
/****************************************************************************/
void osif_create_secure_context(uint32_t size)
{
	rtos_create_secure_context(size);
}
