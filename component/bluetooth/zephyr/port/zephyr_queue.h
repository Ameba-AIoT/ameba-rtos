/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_QUEUE_H
#define ZEPHYR_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/sys/dlist.h>
#include <zephyr/sys/sflist.h>
#include <zephyr_lock.h>
#include <zephyr_time.h>

struct k_queue {
	sys_sflist_t data_q;
	sys_dlist_t poll_events;
};

struct k_lifo{
	struct k_queue _queue;
};

struct k_fifo {
	struct k_queue _queue;
};

#define Z_QUEUE_INITIALIZER(obj) \
	{ \
	.data_q = SYS_SFLIST_STATIC_INIT(&obj.data_q), \
	.poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events), \
	}

#define Z_LIFO_INITIALIZER(obj) \
	{ \
	._queue = Z_QUEUE_INITIALIZER(obj._queue) \
	}

#define Z_FIFO_INITIALIZER(obj) \
	{ \
	._queue = Z_QUEUE_INITIALIZER(obj._queue) \
	}


#define K_FIFO_DEFINE(name) \
	STRUCT_SECTION_ITERABLE_ALTERNATE(k_queue, k_fifo, name) = \
		Z_FIFO_INITIALIZER(name)


#define k_lifo_put(lifo, data) \
	({ \
	k_queue_prepend(&(lifo)->_queue, data); \
	})

#define k_fifo_init(fifo) \
	({ \
	k_queue_init(&(fifo)->_queue); \
	})

#define k_lifo_init(fifo) \
	({ \
	k_queue_init(&(fifo)->_queue); \
	})

#define k_fifo_peek_head(fifo) \
	({ \
	k_queue_peek_head(&(fifo)->_queue); \
	})
	
#define k_fifo_is_empty(fifo) \
		k_queue_is_empty(&(fifo)->_queue)

	
#define k_lifo_get(lifo, timeout) \
		({ \
		k_queue_get(&(lifo)->_queue, timeout); \
		})

#define k_fifo_put(fifo, data) \
	({ \
	k_queue_append(&(fifo)->_queue, data); \
	})

#define k_fifo_get(fifo, timeout) \
	({ \
	k_queue_get(&(fifo)->_queue, timeout); \
	})

#define k_fifo_cancel_wait(fifo) \
	({ \
	k_queue_cancel_wait(&(fifo)->_queue); \
	})

void k_queue_init(struct k_queue *queue);
void k_queue_insert(struct k_queue *queue, void *data);
void k_queue_append(struct k_queue *queue, void *data);
void k_queue_prepend(struct k_queue *queue, void *data);
void *k_queue_get(struct k_queue *queue, k_timeout_t timeout);
void *k_queue_peek_head(struct k_queue *queue);
int k_queue_is_empty(struct k_queue *queue);
void k_queue_cancel_wait(struct k_queue *queue);

#ifdef __cplusplus
}
#endif

#endif

