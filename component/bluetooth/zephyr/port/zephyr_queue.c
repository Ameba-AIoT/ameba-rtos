/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <zephyr_queue.h>
#include <zephyr_poll.h>
#include <os_wrapper_time.h>
#include <common/assert.h>

void k_queue_init(struct k_queue *queue)
{
	sys_sflist_init(&queue->data_q);
	sys_dlist_init(&queue->poll_events);
}

static void queue_insert(struct k_queue *queue, void *data, bool is_append)
{
    uint32_t flags;
    
    flags = osif_lock();
    if (is_append)
        sys_sflist_append(&queue->data_q, (sys_sfnode_t *)data);
    else
        sys_sflist_prepend(&queue->data_q, (sys_sfnode_t *)data);
    osif_unlock(flags);

	z_handle_obj_poll_events(&queue->poll_events, K_POLL_STATE_DATA_AVAILABLE);
}

void k_queue_insert(struct k_queue *queue, void *data)
{
    (void)queue_insert(queue, data, true);
}

void k_queue_append(struct k_queue *queue, void *data)
{
    (void)queue_insert(queue, data, true);
}

void k_queue_prepend(struct k_queue *queue, void *data)
{
    (void)queue_insert(queue, data, false);
}

void *k_queue_get(struct k_queue *queue, k_timeout_t timeout)
{
    uint32_t flags;
    sys_sfnode_t *node;
    
    if (timeout.ticks != 0) {
        struct k_poll_event events[1] = {
            K_POLL_EVENT_STATIC_INITIALIZER(K_POLL_TYPE_FIFO_DATA_AVAILABLE,
                            K_POLL_MODE_NOTIFY_ONLY,
                            queue,
                            0),
        };
        k_poll(events, 1, timeout);
    }

    flags = osif_lock();
    node = sys_sflist_get(&queue->data_q);
    osif_unlock(flags);
    
    return (void *)node;
}

void k_queue_cancel_wait(struct k_queue *queue)
{
    z_handle_obj_poll_events(&queue->poll_events, K_POLL_STATE_CANCELLED);
}

void *k_queue_peek_head(struct k_queue *queue)
{
    uint32_t flags;
    sys_sfnode_t *node = NULL;
    
    flags = osif_lock();
    node = sys_sflist_peek_head(&queue->data_q);
    osif_unlock(flags);

    return node;
}

int k_queue_is_empty(struct k_queue *queue)
{
    uint32_t flags;
    bool empty;
    
    flags = osif_lock();
    empty = sys_sflist_is_empty(&queue->data_q);
    osif_unlock(flags);
    return empty ? 1 : 0;
}