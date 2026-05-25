/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_THREAD_H
#define ZEPHYR_THREAD_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/types.h>
#include <zephyr_time.h>
#include <stdbool.h>

#define K_KERNEL_STACK_DEFINE(sym, size) \
		size_t sym = size\

#define K_KERNEL_STACK_SIZEOF(sym) (sym)
#define K_PRIO_COOP(x) (x)

#define K_THREAD_STACK_DEFINE K_KERNEL_STACK_DEFINE
#define K_THREAD_STACK_SIZEOF K_KERNEL_STACK_SIZEOF

typedef void (*k_thread_entry_t)(void *p1, void *p2, void *p3);

struct k_thread{
	void *p_handle;
};

typedef struct k_thread *k_tid_t;


void k_thread_abort(k_tid_t thread);
int k_thread_name_set(k_tid_t thread, const char *str);
k_tid_t k_thread_create(struct k_thread *new_thread,size_t stack,size_t stack_size,k_thread_entry_t entry,void *p1, void *p2, void *p3,int prio, uint32_t options, k_timeout_t delay);
void k_thread_start(struct k_thread *thread);
k_tid_t k_current_get(void);
bool k_is_in_isr(void);
bool k_is_user_context(void);
void k_yield(void);


#ifdef __cplusplus
}
#endif

#endif
