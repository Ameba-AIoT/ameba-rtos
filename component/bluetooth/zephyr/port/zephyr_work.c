/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <zephyr_work.h>
#include <zephyr_lock.h>
#include <zephyr_thread.h>
#include <osif.h>
#include <string.h>
#include <zephyr/sys/__assert.h>

static struct k_spinlock lock;
struct k_work_q k_sys_work_q;

static K_KERNEL_STACK_DEFINE(sys_work_q_stack,
			     CONFIG_SYSTEM_WORKQUEUE_STACK_SIZE);

void k_work_sys_work_queue_start(void) {
	k_work_queue_start(&k_sys_work_q, sys_work_q_stack,
					K_KERNEL_STACK_SIZEOF(sys_work_q_stack),
					CONFIG_SYSTEM_WORKQUEUE_PRIORITY, NULL);
	k_thread_name_set(&k_sys_work_q.thread, "SYS WQ");
}

void k_work_sys_work_queue_delete(void) {
	k_work_queue_delete(&k_sys_work_q);
}

static inline void flag_clear(uint32_t *flagp,
			      uint32_t bit)
{
	*flagp &= ~BIT(bit);
}

static inline void flag_set(uint32_t *flagp,
			    uint32_t bit)
{
	*flagp |= BIT(bit);
}

static inline bool flag_test(const uint32_t *flagp,
			     uint32_t bit)
{
	return (*flagp & BIT(bit)) != 0U;
}

static inline bool flag_test_and_clear(uint32_t *flagp,
				       int bit)
{
	bool ret = flag_test(flagp, bit);

	flag_clear(flagp, bit);

	return ret;
}

static inline void flags_set(uint32_t *flagp,
			     uint32_t flags)
{
	*flagp = flags;
}

static inline uint32_t flags_get(const uint32_t *flagp)
{
	return *flagp;
}

void k_work_init(struct k_work *work,
		  k_work_handler_t handler)
{
	__ASSERT_NO_MSG(work != NULL);
	__ASSERT_NO_MSG(handler != NULL);

	*work = (struct k_work)Z_WORK_INITIALIZER(handler);
}

int k_work_submit_to_queue(struct k_work_q *queue,
			    struct k_work *work)
{
	k_spinlock_key_t key;
	bool sent;
	int ret = 0;

	key = k_spin_lock(&lock);

	if (!flag_test(&work->flags, K_WORK_QUEUED_BIT)) {
		ret = 2;
		flag_set(&work->flags, K_WORK_QUEUED_BIT);

		k_spin_unlock(&lock, key);
		sent = osif_msg_send(queue->queue, &work, BT_TIMEOUT_NONE);
		key = k_spin_lock(&lock);

		if (!sent) {
			ret = -1;
		}
	}

	k_spin_unlock(&lock, key);

	return ret;
}

int k_work_submit(struct k_work *work)
{
	return k_work_submit_to_queue(&k_sys_work_q, work);
}

/* Wait for last-submitted instance to complete. */
bool k_work_flush(struct k_work *work, struct k_work_sync *sync)
{
	(void)sync;
	__ASSERT_NO_MSG(work != NULL);
	__ASSERT_NO_MSG(!flag_test(&work->flags, K_WORK_DELAYABLE_BIT));
	__ASSERT_NO_MSG(!k_is_in_isr());
	__ASSERT_NO_MSG(sync != NULL);
#ifdef CONFIG_KERNEL_COHERENCE
	__ASSERT_NO_MSG(arch_mem_coherent(sync));
#endif /* CONFIG_KERNEL_COHERENCE */

	void *flush_sem = NULL;
	osif_sem_create(&flush_sem, 0, 1);

	k_spinlock_key_t key = k_spin_lock(&lock);

	bool need_flush = (flags_get(&work->flags)
			   & (K_WORK_QUEUED | K_WORK_RUNNING)) != 0U;

	if (need_flush) {
		work->sem = flush_sem;
		flag_set(&work->flags, K_WORK_FLUSHING_BIT);
	}

	k_spin_unlock(&lock, key);

	/* If necessary wait until the flusher item completes */
	if (need_flush) {
		osif_sem_take(flush_sem, BT_TIMEOUT_FOREVER);
	}
	osif_sem_delete(flush_sem);

	return need_flush;
}

int k_work_busy_get(const struct k_work *work)
{
	int ret;
	k_spinlock_key_t key;

	key = k_spin_lock(&lock);
	ret = flags_get(&work->flags) & K_WORK_MASK;
	k_spin_unlock(&lock, key);

	return ret;
}

bool k_work_is_pending(const struct k_work *work)
{
	return k_work_busy_get(work) != 0;
}

/* Timeout handler for delayable work.
 *
 * Invoked by timeout infrastructure.
 * Takes and releases work lock.
 * Conditionally reschedules.
 */
static void work_timeout(struct _timeout *to)
{
	struct k_work_delayable *dw
		= CONTAINER_OF(to, struct k_work_delayable, timeout);

	flag_clear(&dw->work.flags, K_WORK_DELAYED_BIT);
	k_work_submit_to_queue(dw->queue, &dw->work);
}

static int schedule_for_queue(struct k_work_q *queue,
				     struct k_work_delayable *dwork,
				     k_timeout_t delay)
{
	int ret = 1;
	struct k_work *work = &dwork->work;
	k_spinlock_key_t key;

	if (K_TIMEOUT_EQ(delay, K_NO_WAIT)) {
		return k_work_submit_to_queue(queue, work);
	}

	key = k_spin_lock(&lock);
	/* Ignore this schedule if work was already queued. */
	if ((flags_get(&work->flags) & K_WORK_MASK) & ~K_WORK_RUNNING) {
		k_spin_unlock(&lock, key);
		return 0;
	}
	flag_set(&work->flags, K_WORK_DELAYED_BIT);
	dwork->queue = queue;
	k_spin_unlock(&lock, key);

	/* Add timeout */
	z_add_timeout(&dwork->timeout, work_timeout, delay);

	return ret;
}

int k_work_schedule_for_queue(struct k_work_q *queue,
			       struct k_work_delayable *dwork,
			       k_timeout_t delay)
{
	__ASSERT_NO_MSG(dwork != NULL);

	schedule_for_queue(queue, dwork, delay);

	return 0;
}

int k_work_schedule(struct k_work_delayable *dwork,k_timeout_t delay)
{
	return k_work_schedule_for_queue(&k_sys_work_q, dwork, delay);
}

void k_work_init_delayable(struct k_work_delayable *dwork,
			    k_work_handler_t handler)
{
	__ASSERT_NO_MSG(dwork != NULL);
	__ASSERT_NO_MSG(handler != NULL);

	dwork->work.handler = handler;
}

int k_work_delayable_busy_get(const struct k_work_delayable *dwork)
{
	int ret;
	k_spinlock_key_t key;

	key = k_spin_lock(&lock);
	ret = flags_get(&dwork->work.flags) & K_WORK_MASK;
	k_spin_unlock(&lock, key);

	return ret;
}

bool k_work_delayable_is_pending(
	const struct k_work_delayable *dwork)
{
	return k_work_delayable_busy_get(dwork) != 0;
}

static inline bool unschedule_work(struct k_work_delayable *dwork)
{
	bool ret = false;
	bool is_work_delayed;
	k_spinlock_key_t key;
	struct k_work *work = &dwork->work;

	key = k_spin_lock(&lock);
	is_work_delayed = flag_test_and_clear(&work->flags, K_WORK_DELAYED_BIT);
	k_spin_unlock(&lock, key);

	if (is_work_delayed) {
		ret = (z_abort_timeout(&dwork->timeout) == 0);
	}

	return ret;
}

int k_work_reschedule_for_queue(struct k_work_q *queue,
				 struct k_work_delayable *dwork,
				 k_timeout_t delay)
{
	__ASSERT_NO_MSG(dwork != NULL);

	unschedule_work(dwork);

	/* Schedule the work item with the new parameters. */
	return schedule_for_queue(queue, dwork, delay);
}

int k_work_reschedule(struct k_work_delayable *dwork,
				     k_timeout_t delay)
{
	return k_work_reschedule_for_queue(&k_sys_work_q, dwork, delay);
}

int k_work_cancel(struct k_work *work)
{
	k_spinlock_key_t key;

	key = k_spin_lock(&lock);
	flag_set(&work->flags, K_WORK_CANCELING_BIT);
	k_spin_unlock(&lock, key);

	return 0;
}

int k_work_cancel_delayable(struct k_work_delayable *dwork)
{
	__ASSERT_NO_MSG(dwork != NULL);

	return unschedule_work(dwork);
}

bool k_work_cancel_delayable_sync(struct k_work_delayable *dwork,
				  struct k_work_sync *sync)
{
	(void)sync;

	__ASSERT_NO_MSG(dwork != NULL);
	__ASSERT_NO_MSG(sync != NULL);
	__ASSERT_NO_MSG(!k_is_in_isr());
#ifdef CONFIG_KERNEL_COHERENCE
	__ASSERT_NO_MSG(arch_mem_coherent(sync));
#endif

	return unschedule_work(dwork);
}

k_ticks_t k_work_delayable_remaining_get(const struct k_work_delayable *dwork)
{
	return z_timeout_remaining(&dwork->timeout);
}

/* Loop executed by a work queue thread.
 *
 * @param p1 pointer to the work queue structure
 */
static void work_queue_main(void *p1, void *p2, void *p3)
{
	(void)p2;
	(void)p3;
	struct k_work_q *queue = (struct k_work_q *)p1;
	k_spinlock_key_t key;

	while (true) {
		struct k_work *work = NULL;
		osif_msg_recv(queue->queue, &work, BT_TIMEOUT_FOREVER);

		if (!work)
			continue;

		key = k_spin_lock(&lock);
		flag_clear(&work->flags, K_WORK_QUEUED_BIT);

		if (flag_test(&work->flags, K_WORK_CANCELING_BIT)) {
			flag_clear(&work->flags, K_WORK_CANCELING_BIT);
			k_spin_unlock(&lock, key);
			continue;
		}

		flag_set(&work->flags, K_WORK_RUNNING_BIT);
		k_spin_unlock(&lock, key);

		work->handler(work);

		key = k_spin_lock(&lock);
		flag_clear(&work->flags, K_WORK_RUNNING_BIT);

		if (flag_test(&work->flags, K_WORK_FLUSHING_BIT)) {
			flag_clear(&work->flags, K_WORK_FLUSHING_BIT);
			k_spin_unlock(&lock, key);
			osif_sem_give(work->sem);
			continue;
		}
		k_spin_unlock(&lock, key);
	}
}

void k_work_queue_init(struct k_work_q *queue)
{
	__ASSERT_NO_MSG(queue != NULL);

	*queue = (struct k_work_q) {
		.flags = 0,
	};
}

void k_work_queue_start(struct k_work_q *queue,
			size_t stack,
			size_t stack_size,
			int prio,
			const struct k_work_queue_config *cfg)
{
	(void)stack;
	(void)cfg;
	__ASSERT_NO_MSG(queue);
	__ASSERT_NO_MSG(stack);
	__ASSERT_NO_MSG(!flag_test(&queue->flags, K_WORK_QUEUE_STARTED_BIT));
	uint32_t flags = K_WORK_QUEUE_STARTED;

	flags_set(&queue->flags, flags);

	osif_msg_queue_create(&queue->queue, 128, sizeof(void *));
	/* bt_disable() aborts bt_workq.thread by k_thread_abort(), so Must use k_thread_create() here. */
	k_thread_create(&queue->thread, stack_size, stack_size, work_queue_main, queue, NULL, NULL, prio, 0, K_NO_WAIT);
}

void k_work_queue_delete(struct k_work_q *queue)
{
	__ASSERT_NO_MSG(queue);
	void *msg;

	k_thread_abort(&queue->thread);
	while (osif_msg_recv(queue->queue, &msg, BT_TIMEOUT_NONE));
	osif_msg_queue_delete(queue->queue);

	memset(queue, 0, sizeof(struct k_work_q));
}
