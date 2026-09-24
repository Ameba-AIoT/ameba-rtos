/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <zephyr_thread.h>
#include <zephyr_lock.h>
#include <osif.h>
#include <string.h>
#include <ameba_soc.h>

static struct k_spinlock lock;

struct k_thread_array {
	struct k_thread *thread;
	k_thread_entry_t entry;
	void *p1;
	void *p2;
	void *p3;
	bool used;
};

#define THREAD_ARRAY_SIZE 10
static char task_name[THREAD_ARRAY_SIZE][10] = {0};

struct k_thread_array thread_array[THREAD_ARRAY_SIZE] = {0};

static void k_thread_run(void *context)
{
	struct k_thread_array *thread = (struct k_thread_array *)context;
	thread->entry(thread->p1,thread->p2,thread->p3);
}

void k_thread_abort(k_tid_t thread)
{
	k_spinlock_key_t key;
	int idx;

	if (thread == NULL) {
		thread = k_current_get();
	}

	key = k_spin_lock(&lock);
	for(idx = 0; idx < THREAD_ARRAY_SIZE; idx++) {
		if(thread_array[idx].used && thread_array[idx].thread == thread) {
			memset(&thread_array[idx], 0, sizeof(struct k_thread_array));
			break;
		}
	}
	k_spin_unlock(&lock, key);

	osif_task_delete(thread->p_handle);
}

int k_thread_name_set(k_tid_t thread, const char *str)
{
	(void)thread;
	(void)str;
#if 0
	int idx;

	for(idx = 0; idx < THREAD_ARRAY_SIZE; idx++) {
		if(thread_array[idx].used && thread_array[idx].thread == thread) {
			z_printf("zephyr%d, name:%s\r\n", idx, str);
			break;
		}
	}

	if (idx == THREAD_ARRAY_SIZE) {
		z_printf("Task not found\r\n");
		return -1;
	}
#endif
	return 0;
}

k_tid_t k_thread_create(struct k_thread *new_thread,
				  size_t stack,
				  size_t stack_size,
				  k_thread_entry_t entry,
				  void *p1, void *p2, void *p3,
				  int prio, uint32_t options, k_timeout_t delay)
{
	(void)stack;
	(void)delay;
	(void)options;
	k_spinlock_key_t key;
	int idx;

	key = k_spin_lock(&lock);
	for(idx = 0; idx < THREAD_ARRAY_SIZE; idx++) {
		if(!thread_array[idx].used) {
			thread_array[idx].thread = new_thread;
			thread_array[idx].entry = entry;
			thread_array[idx].p1 = p1;
			thread_array[idx].p2 = p2;
			thread_array[idx].p3 = p3;
			thread_array[idx].used = true;
			break;
		}
	}
	k_spin_unlock(&lock, key);

	if(idx == THREAD_ARRAY_SIZE)
		return NULL;

	memset(task_name[idx], 0, 10);
	DiagSnPrintf(task_name[idx], 10, "zephyr%d", idx);
	if (osif_task_create(&(new_thread->p_handle), task_name[idx], k_thread_run, &thread_array[idx], stack_size, prio)) {
		return new_thread;
	} else {
		return NULL;
	}
}

void k_thread_start(struct k_thread *thread)
{
	(void)thread;
	return;
}

k_tid_t k_current_get()
{
	void *pp_handle = NULL;
	k_spinlock_key_t key;
	int idx;
	k_tid_t current = NULL;

	osif_task_handle_get(&pp_handle);

	key = k_spin_lock(&lock);
	for(idx = 0; idx < THREAD_ARRAY_SIZE; idx++) {
		if(thread_array[idx].used && thread_array[idx].thread->p_handle == pp_handle) {
			current = thread_array[idx].thread;
			break;
		}
	}
	k_spin_unlock(&lock, key);

	return current;
}

bool k_is_in_isr(void)
{
	return !osif_task_context_check();
}

bool k_is_user_context(void)
{
	return osif_task_context_check();
}

void k_yield(void)
{
	osif_task_yield();
}

