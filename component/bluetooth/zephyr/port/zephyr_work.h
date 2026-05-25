/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_WORK_H
#define ZEPHYR_WORK_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/sys/slist.h>
#include <zephyr_thread.h>
#include <zephyr_time.h>
#include <zephyr_sema.h>

enum {
/**
 * @cond INTERNAL_HIDDEN
 */

	/* The atomic API is used for all work and queue flags fields to
	 * enforce sequential consistency in SMP environments.
	 */

	/* Bits that represent the work item states.  At least nine of the
	 * combinations are distinct valid stable states.
	 */
	K_WORK_RUNNING_BIT = 0,
	K_WORK_CANCELING_BIT = 1,
	K_WORK_QUEUED_BIT = 2,
	K_WORK_DELAYED_BIT = 3,
	K_WORK_FLUSHING_BIT = 4,

	K_WORK_MASK = BIT(K_WORK_DELAYED_BIT) | BIT(K_WORK_QUEUED_BIT)
		| BIT(K_WORK_RUNNING_BIT) | BIT(K_WORK_CANCELING_BIT) | BIT(K_WORK_FLUSHING_BIT),

	/* Static work flags */
	K_WORK_DELAYABLE_BIT = 8,
	K_WORK_DELAYABLE = BIT(K_WORK_DELAYABLE_BIT),

	/* Dynamic work queue flags */
	K_WORK_QUEUE_STARTED_BIT = 0,
	K_WORK_QUEUE_STARTED = BIT(K_WORK_QUEUE_STARTED_BIT),
	K_WORK_QUEUE_BUSY_BIT = 1,
	K_WORK_QUEUE_BUSY = BIT(K_WORK_QUEUE_BUSY_BIT),
	K_WORK_QUEUE_DRAIN_BIT = 2,
	K_WORK_QUEUE_DRAIN = BIT(K_WORK_QUEUE_DRAIN_BIT),
	K_WORK_QUEUE_PLUGGED_BIT = 3,
	K_WORK_QUEUE_PLUGGED = BIT(K_WORK_QUEUE_PLUGGED_BIT),

	/* Static work queue flags */
	K_WORK_QUEUE_NO_YIELD_BIT = 8,
	K_WORK_QUEUE_NO_YIELD = BIT(K_WORK_QUEUE_NO_YIELD_BIT),

/**
 * INTERNAL_HIDDEN @endcond
 */
	/* Transient work flags */

	/** @brief Flag indicating a work item that is running under a work
	 * queue thread.
	 *
	 * Accessed via k_work_busy_get().  May co-occur with other flags.
	 */
	K_WORK_RUNNING = BIT(K_WORK_RUNNING_BIT),

	/** @brief Flag indicating a work item that is being canceled.
	 *
	 * Accessed via k_work_busy_get().  May co-occur with other flags.
	 */
	K_WORK_CANCELING = BIT(K_WORK_CANCELING_BIT),

	/** @brief Flag indicating a work item that has been submitted to a
	 * queue but has not started running.
	 *
	 * Accessed via k_work_busy_get().  May co-occur with other flags.
	 */
	K_WORK_QUEUED = BIT(K_WORK_QUEUED_BIT),

	/** @brief Flag indicating a delayed work item that is scheduled for
	 * submission to a queue.
	 *
	 * Accessed via k_work_busy_get().  May co-occur with other flags.
	 */
	K_WORK_DELAYED = BIT(K_WORK_DELAYED_BIT),

	/** @brief Flag indicating a synced work item that is being flushed.
	 *
	 * Accessed via k_work_busy_get().  May co-occur with other flags.
	 */
	K_WORK_FLUSHING = BIT(K_WORK_FLUSHING_BIT),
};

struct k_work_q {
	/* The thread that animates the work. */
	struct k_thread thread;

	/* The queue that contains pending work. */
	void *queue;

	/* Flags describing queue state. */
	uint32_t flags;
};

struct k_work;
typedef void (*k_work_handler_t)(struct k_work *work);

struct k_work {
	/* The function to be invoked by the work queue thread. */
	k_work_handler_t handler;
	
	/* semaphore for waiting flush complete */
	void *sem;

	/* State of the work item.
	 *
	 * The item can be DELAYED, QUEUED, and RUNNING simultaneously.
	 *
	 * It can be RUNNING and CANCELING simultaneously.
	 */
	uint32_t flags;
};

struct k_work_delayable {
	/* The work item. */
	struct k_work work;

	/* Timeout used to submit work after a delay. */
	struct _timeout timeout;

	/* The queue to which the work should be submitted. */
	struct k_work_q *queue;
};

/* Record used to wait for work to flush.
 *
 * The work item is inserted into the queue that will process (or is
 * processing) the item, and will be processed as soon as the item
 * completes.  When the flusher is processed the semaphore will be
 * signaled, releasing the thread waiting for the flush.
 */
struct z_work_flusher {
	struct k_work work;
	struct k_sem sem;
};

/* Record used to wait for work to complete a cancellation.
 *
 * The work item is inserted into a global queue of pending cancels.
 * When a cancelling work item goes idle any matching waiters are
 * removed from pending_cancels and are woken.
 */
struct z_work_canceller {
	sys_snode_t node;
	struct k_work *work;
	struct k_sem sem;
};

struct k_work_sync {
	union {
		struct z_work_flusher flusher;
		struct z_work_canceller canceller;
	};
};

struct k_work_queue_config {
	const char *name;
	bool no_yield;
};

#define Z_WORK_INITIALIZER(work_handler) { \
	.handler = work_handler, \
}

#define Z_WORK_DELAYABLE_INITIALIZER(work_handler) { \
	.work = { \
		.handler = work_handler, \
		.flags = K_WORK_DELAYABLE, \
	}, \
}

#define K_WORK_DEFINE(work, work_handler) \
	struct k_work work = Z_WORK_INITIALIZER(work_handler)

#define K_WORK_DELAYABLE_DEFINE(work, work_handler) \
	struct k_work_delayable work \
	  = Z_WORK_DELAYABLE_INITIALIZER(work_handler)

extern struct k_work_q k_sys_work_q;

static inline struct k_work_delayable *
k_work_delayable_from_work(struct k_work *work)
{
	return CONTAINER_OF(work, struct k_work_delayable, work);
}

static inline k_tid_t k_work_queue_thread_get(struct k_work_q *queue)
{
	return &queue->thread;
}

void k_work_sys_work_queue_start(void);
void k_work_sys_work_queue_delete(void);
int k_work_submit(struct k_work *work);
bool k_work_flush(struct k_work *work, struct k_work_sync *sync);
int k_work_schedule(struct k_work_delayable *dwork,k_timeout_t delay);
int k_work_schedule_for_queue(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay);
int k_work_reschedule_for_queue(struct k_work_q *queue, struct k_work_delayable *dwork, k_timeout_t delay);
int k_work_submit_to_queue(struct k_work_q *queue, struct k_work *work);
void k_work_init_delayable(struct k_work_delayable *dwork,k_work_handler_t handler);
struct k_work_delayable * k_work_delayable_from_work(struct k_work *work);
int k_work_cancel_delayable(struct k_work_delayable *dwork);
int k_work_delayable_busy_get(const struct k_work_delayable *dwork);
bool k_work_delayable_is_pending(const struct k_work_delayable *dwork);
int k_work_reschedule(struct k_work_delayable *dwork,k_timeout_t delay);
k_ticks_t k_work_delayable_remaining_get(const struct k_work_delayable *dwork);
int k_work_cancel(struct k_work *work);
void k_work_queue_init(struct k_work_q *queue);
void k_work_queue_start(struct k_work_q *queue, size_t stack, size_t stack_size,  int prio,  const struct k_work_queue_config *cfg);
void k_work_queue_delete(struct k_work_q *queue);
#ifdef __cplusplus
}
#endif

#endif
