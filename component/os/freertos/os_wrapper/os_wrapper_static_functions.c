/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>
#include <string.h>
#include "dlist.h"
#include "ameba.h"
#include "FreeRTOS.h"
#include "os_wrapper.h"

#if(configSUPPORT_STATIC_ALLOCATION == 1)

#define COMPONENT_MUTEX 1
#define COMPONENT_SEMA  2
#define COMPONENT_TIMER 3
#define IS_VALID_COMPONENT_TYPE(type) (type == COMPONENT_MUTEX || type == COMPONENT_SEMA || type == COMPONENT_TIMER)

/*
 * Approximate amount consumed after STA connects and pings.
 * If SoftAP is enabled, generally 5 more timers are consumed and 11 more mutexes are consumed.
 * If you need more accurate statistics later, you can enable printf in the
 * __reserved_get_from_poll function to view the number of dynamic components.
 */
#ifdef CONFIG_WLAN
#if defined (CONFIG_AS_INIC_NP) && CONFIG_AS_INIC_NP
#define MAX_MUTEX_BUF_NUM		28
#define MAX_SEMA_BUF_NUM		14
#define MAX_TIMER_BUF_NUM		14
#elif defined (CONFIG_SINGLE_CORE_WIFI) && CONFIG_SINGLE_CORE_WIFI
#define MAX_MUTEX_BUF_NUM		31
#define MAX_SEMA_BUF_NUM		7
#define MAX_TIMER_BUF_NUM		15
#else
#define MAX_MUTEX_BUF_NUM		0
#define MAX_SEMA_BUF_NUM		0
#define MAX_TIMER_BUF_NUM		0
#endif
#else
#define MAX_MUTEX_BUF_NUM		5
#define MAX_SEMA_BUF_NUM		5
#define MAX_TIMER_BUF_NUM		5
#endif

typedef struct mutex_buf {
	struct list_head list;
	StaticSemaphore_t mutex;
} mutex_buf_t;

typedef struct timer_buf {
	struct list_head list;
	StaticTimer_t timerbuf;
} timer_buf_t;

static mutex_buf_t mutex_pool[MAX_MUTEX_BUF_NUM];
static struct list_head wrapper_mutex_buf_list;
static uint32_t mutex_buf_used_num;
static uint32_t mutex_max_buf_used_num;
static uint32_t mutex_dynamic_num;
static uint32_t mutex_pool_init_flag = FALSE;

static mutex_buf_t sema_pool[MAX_SEMA_BUF_NUM];
static struct list_head wrapper_sema_buf_list;
static uint32_t sema_buf_used_num;
static uint32_t sema_max_buf_used_num;
static uint32_t sema_dynamic_num;
static uint32_t sema_pool_init_flag = FALSE;

static timer_buf_t timer_pool[MAX_TIMER_BUF_NUM];
static struct list_head wrapper_timer_buf_list;
static uint32_t timer_buf_used_num;
static uint32_t timer_max_buf_used_num;
static uint32_t timer_dynamic_num;
static uint32_t timer_pool_init_flag = FALSE;

static void __reserved_init_static_pool(int component_type, void *pool_arg, struct list_head *phead, uint32_t max_pool_num,
										uint32_t *poolbuf_used_num, uint32_t *max_poolbuf_used_num,
										uint32_t *pool_dynamic_num, uint32_t *pool_init_flag);

static void *__reserved_get_from_poll(int component_type, struct list_head *phead,
									  uint32_t *pool_init_flag, uint32_t *pool_dynamic_num,
									  uint32_t *pool_used_num, uint32_t *max_pool_buf_used_num)
{
	void *p_component = NULL;
	struct list_head *plist;

	assert_param(IS_VALID_COMPONENT_TYPE(component_type));

	if (*pool_init_flag == FALSE) {
		if (component_type == COMPONENT_MUTEX) {
			__reserved_init_static_pool(COMPONENT_MUTEX, mutex_pool, &wrapper_mutex_buf_list, MAX_MUTEX_BUF_NUM,
										&mutex_buf_used_num, &mutex_max_buf_used_num, &mutex_dynamic_num, &mutex_pool_init_flag);
		} else if (component_type == COMPONENT_SEMA) {
			__reserved_init_static_pool(COMPONENT_SEMA, sema_pool, &wrapper_sema_buf_list, MAX_SEMA_BUF_NUM,
										&sema_buf_used_num, &sema_max_buf_used_num, &sema_dynamic_num, &sema_pool_init_flag);
		} else {
			__reserved_init_static_pool(COMPONENT_TIMER, timer_pool, &wrapper_timer_buf_list, MAX_TIMER_BUF_NUM,
										&timer_buf_used_num, &timer_max_buf_used_num, &timer_dynamic_num, &timer_pool_init_flag);
		}
	}

	// get memory from list
	rtos_critical_enter();
	if (list_empty(phead)) {
		rtos_critical_exit();
		p_component = NULL;
	} else {
		plist = phead->next;
		list_del_init(plist);
		p_component = (void *)((unsigned int)plist + sizeof(struct list_head));
		*pool_used_num = *pool_used_num + 1;
		rtos_critical_exit();
	}

	// set dynamic flags
	if (p_component == NULL) {
		if (*pool_init_flag) {
			(*pool_dynamic_num) ++;
			/*
			printf("function:[%s] static implement is not avaliable, goto dynamic allocation. \ncomponent_type:%s, static_used_cnt:%ld/%d, dynamic allocate times:%d\n",
				   __FUNCTION__,
				   (component_type == COMPONENT_MUTEX ? "mutex" :
					component_type == COMPONENT_SEMA ? "sema" : "timer"),
				   *pool_used_num,
				   (component_type == COMPONENT_MUTEX ? MAX_MUTEX_BUF_NUM :
					component_type == COMPONENT_SEMA ? MAX_SEMA_BUF_NUM : MAX_TIMER_BUF_NUM),
				   (int)*pool_dynamic_num
				  );

			*/
		}
		goto exit;
	} else {
		if (component_type == COMPONENT_MUTEX || component_type == COMPONENT_SEMA) {
			memset(p_component, 0, sizeof(StaticSemaphore_t));
		} else {
			memset(p_component, 0, sizeof(StaticTimer_t));
		}
	}

exit:

	if (*pool_used_num + *pool_dynamic_num > *max_pool_buf_used_num) {
		*max_pool_buf_used_num = *pool_used_num + *pool_dynamic_num;
	}
	return (void *)p_component;
}

static void __reserved_release_to_poll(int component_type, void *p_buf, struct list_head *phead, uint32_t *count, uint32_t *dynamic_count)
{
	struct list_head *plist;
	int is_static = pdFALSE;
	uint32_t buf_addr = (uint32_t)p_buf;
	uint32_t mutex_pool_addr = (uint32_t)&mutex_pool;
	uint32_t sema_pool_addr = (uint32_t)&sema_pool;
	uint32_t timer_pool_addr = (uint32_t)&timer_pool;

	assert_param(IS_VALID_COMPONENT_TYPE(component_type));

	if (component_type == COMPONENT_MUTEX) {
		if (mutex_pool_addr <= buf_addr && buf_addr < (mutex_pool_addr + sizeof(mutex_pool))) {
			is_static = pdTRUE;
		}
	} else if (component_type == COMPONENT_SEMA) {
		if (sema_pool_addr <= buf_addr && buf_addr < (sema_pool_addr + sizeof(sema_pool))) {
			is_static = pdTRUE;
		}
	} else {
		if (timer_pool_addr <= buf_addr && buf_addr < (timer_pool_addr + sizeof(timer_pool))) {
			while (rtos_timer_is_timer_active(p_buf) == pdTRUE) {};
			is_static = pdTRUE;
		}
	}

	rtos_critical_enter();

	if (is_static) {
		plist = (struct list_head *)(((unsigned int)p_buf) - sizeof(struct list_head));
		list_add_tail(plist, phead);
		*count = *count - 1;
	} else {
		*dynamic_count = *dynamic_count - 1;
	}

	rtos_critical_exit();
}

static void __reserved_init_static_pool(int component_type, void *pool_arg, struct list_head *phead, uint32_t max_pool_num,
										uint32_t *poolbuf_used_num, uint32_t *max_poolbuf_used_num,
										uint32_t *pool_dynamic_num, uint32_t *pool_init_flag)
{
	uint32_t i;

	assert_param(IS_VALID_COMPONENT_TYPE(component_type));

	if (*pool_init_flag == TRUE) {
		return;
	}

	if (component_type == COMPONENT_MUTEX || component_type == COMPONENT_SEMA) {
		mutex_buf_t *pool = pool_arg;
		memset(pool, 0, max_pool_num * sizeof(mutex_buf_t));
		INIT_LIST_HEAD(phead);

		for (i = 0; i < max_pool_num; i++) {
			INIT_LIST_HEAD(&pool[i].list);
			list_add_tail(&pool[i].list, phead);
		}
	} else {
		timer_buf_t *pool = pool_arg;
		memset(pool, 0, max_pool_num * sizeof(timer_buf_t));
		INIT_LIST_HEAD(phead);

		for (i = 0; i < max_pool_num; i++) {
			INIT_LIST_HEAD(&pool[i].list);
			list_add_tail(&pool[i].list, phead);
		}
	}

	*poolbuf_used_num = 0;
	*max_poolbuf_used_num = 0;
	*pool_dynamic_num = 0;
	*pool_init_flag = TRUE;
}

StaticSemaphore_t *__reserved_get_mutex_from_poll(void)
{
	return __reserved_get_from_poll(COMPONENT_MUTEX, &wrapper_mutex_buf_list, &mutex_pool_init_flag,
									&mutex_dynamic_num, &mutex_buf_used_num, &mutex_max_buf_used_num);
}

void __reserved_release_mutex_to_poll(void *buf)
{
	__reserved_release_to_poll(COMPONENT_MUTEX, buf, &wrapper_mutex_buf_list, &mutex_buf_used_num, &mutex_dynamic_num);
}

StaticSemaphore_t *__reserved_get_sema_from_poll(void)
{
	return __reserved_get_from_poll(COMPONENT_SEMA, &wrapper_sema_buf_list, &sema_pool_init_flag,
									&sema_dynamic_num, &sema_buf_used_num, &sema_max_buf_used_num);
}

void __reserved_release_sema_to_poll(void *buf)
{
	__reserved_release_to_poll(COMPONENT_SEMA, buf, &wrapper_sema_buf_list, &sema_buf_used_num, &sema_dynamic_num);
}

StaticTimer_t *__reserved_get_timer_from_poll(void)
{
	return __reserved_get_from_poll(COMPONENT_TIMER, &wrapper_timer_buf_list, &timer_pool_init_flag,
									&timer_dynamic_num, &timer_buf_used_num, &timer_max_buf_used_num);
}

void __reserved_release_timer_to_poll(void *buf)
{
	__reserved_release_to_poll(COMPONENT_TIMER, buf, &wrapper_timer_buf_list, &timer_buf_used_num, &timer_dynamic_num);
}

void rtos_static_get_component_status(struct component_status *comp_status)
{
	comp_status->mutex_total_num = MAX_MUTEX_BUF_NUM;
	comp_status->mutex_buf_used_num = mutex_buf_used_num;
	comp_status->mutex_dynamic_num = mutex_dynamic_num;
	comp_status->mutex_max_buf_used_num = mutex_max_buf_used_num;
	comp_status->mutex_poolsize_static = sizeof(mutex_pool);

	comp_status->sema_total_num = MAX_SEMA_BUF_NUM;
	comp_status->sema_buf_used_num = sema_buf_used_num;
	comp_status->sema_dynamic_num = sema_dynamic_num;
	comp_status->sema_max_buf_used_num = sema_max_buf_used_num;
	comp_status->sema_poolsize_static = sizeof(sema_pool);

	comp_status->timer_total_num = MAX_TIMER_BUF_NUM;
	comp_status->timer_buf_used_num = timer_buf_used_num;
	comp_status->timer_dynamic_num = timer_dynamic_num;
	comp_status->timer_max_buf_used_num = timer_max_buf_used_num;
	comp_status->timer_poolsize_static = sizeof(timer_pool);
}

#endif
