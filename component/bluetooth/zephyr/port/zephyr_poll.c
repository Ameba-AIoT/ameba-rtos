/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <zephyr_poll.h>
#include <zephyr/sys/__assert.h>
#include <zephyr_lock.h>
#include <common/assert.h>

static struct k_spinlock lock;

enum POLL_MODE { MODE_NONE, MODE_POLL, MODE_TRIGGERED };

#define POLL_SEM_NUM_MAX 5

struct poll_static_sem {
	void *sem;
	bool available;
};

static struct poll_static_sem poll_sems[POLL_SEM_NUM_MAX];

void poll_static_sem_init(void)
{
	int i = 0;

	for (i = 0; i < POLL_SEM_NUM_MAX; i++) {
		osif_sem_create(&poll_sems[i].sem, 0, 1);
		poll_sems[i].available = true;
	}
}

void poll_static_sem_deinit(void)
{
	int i = 0;

	for (i = 0; i < POLL_SEM_NUM_MAX; i++) {
		osif_sem_delete(poll_sems[i].sem);
	}
}

void k_poll_event_init(struct k_poll_event *event, uint32_t type, int mode, void *obj)
{
	/* event->tag is left uninitialized: the user will set it if needed */
	event->type = type;
	event->state = K_POLL_STATE_NOT_READY;
	event->mode = mode;
	event->unused = 0U;
	event->obj = obj;
}

void k_poll_signal_init(struct k_poll_signal *sig)
{
	sys_dlist_init(&sig->poll_events);
	sig->signaled = 0U;
	/* signal->result is left uninitialized */
}

/* must be called with interrupts locked */
static inline void clear_event_registration(struct k_poll_event *event)
{
	bool remove_event = false;

	switch (event->type) {
	case K_POLL_TYPE_SEM_AVAILABLE:
		__ASSERT(event->sem != NULL, "invalid semaphore\n");
		remove_event = true;
		break;
	case K_POLL_TYPE_DATA_AVAILABLE:
		__ASSERT(event->queue != NULL, "invalid queue\n");
		remove_event = true;
		break;
	case K_POLL_TYPE_SIGNAL:
		__ASSERT(event->signal != NULL, "invalid poll signal\n");
		remove_event = true;
		break;
// 	case K_POLL_TYPE_MSGQ_DATA_AVAILABLE:
// 		__ASSERT(event->msgq != NULL, "invalid message queue\n");
// 		remove_event = true;
// 		break;
// #ifdef CONFIG_PIPES
// 	case K_POLL_TYPE_PIPE_DATA_AVAILABLE:
// 		__ASSERT(event->pipe != NULL, "invalid pipe\n");
// 		remove_event = true;
// 		break;
// #endif
	case K_POLL_TYPE_IGNORE:
		/* nothing to do */
		break;
	default:
		__ASSERT(false, "invalid event type\n");
		break;
	}

	if (remove_event && sys_dnode_is_linked(&event->_node)) {
		sys_dlist_remove(&event->_node);
	}

	event->poll_sem = NULL;
}

/* must be called with interrupts locked */
static inline void clear_event_registrations(struct k_poll_event *events,
					      int num_events)
{
	while (num_events--) {
		k_spinlock_key_t key;
		key = k_spin_lock(&lock);
		clear_event_registration(&events[num_events]);
		k_spin_unlock(&lock, key);
	}
}

/* must be called with interrupts locked */
static inline bool is_condition_met(struct k_poll_event *event, uint32_t *state)
{
	switch (event->type) {
	case K_POLL_TYPE_SEM_AVAILABLE:
		if (k_sem_count_get(event->sem) > 0U) {
			*state = K_POLL_STATE_SEM_AVAILABLE;
			return true;
		}
		break;
	case K_POLL_TYPE_DATA_AVAILABLE:
		if (!k_queue_is_empty(event->queue)) {
			*state = K_POLL_STATE_FIFO_DATA_AVAILABLE;
			return true;
		}
		break;
	case K_POLL_TYPE_SIGNAL:
		if (event->signal->signaled != 0U) {
			*state = K_POLL_STATE_SIGNALED;
			return true;
		}
		break;
// 	case K_POLL_TYPE_MSGQ_DATA_AVAILABLE:
// 		if (event->msgq->used_msgs > 0) {
// 			*state = K_POLL_STATE_MSGQ_DATA_AVAILABLE;
// 			return true;
// 		}
// 		break;
// #ifdef CONFIG_PIPES
// 	case K_POLL_TYPE_PIPE_DATA_AVAILABLE:
// 		if (k_pipe_read_avail(event->pipe)) {
// 			*state = K_POLL_STATE_PIPE_DATA_AVAILABLE;
// 			return true;
// 		}
// #endif
	case K_POLL_TYPE_IGNORE:
		break;
	default:
		__ASSERT(false, "invalid event type (0x%x)\n", event->type);
		break;
	}

	return false;
}

static inline void add_event(sys_dlist_t *events, struct k_poll_event *event)
{
	// struct k_poll_event *pending;

	// pending = (struct k_poll_event *)sys_dlist_peek_tail(events);
	// if (pending == NULL) {
	// 	sys_dlist_append(events, &event->_node);
	// } else {
	// 	__ASSERT(0, "duplicate polling\n");
	// }
	sys_dlist_append(events, &event->_node);
}

/* must be called with interrupts locked */
static inline void register_event(struct k_poll_event *event)
{
	switch (event->type) {
	case K_POLL_TYPE_SEM_AVAILABLE:
		__ASSERT(event->sem != NULL, "invalid semaphore\n");
		add_event(&event->sem->poll_events, event);
		break;
	case K_POLL_TYPE_DATA_AVAILABLE:
		__ASSERT(event->queue != NULL, "invalid queue\n");
		add_event(&event->queue->poll_events, event);
		break;
	case K_POLL_TYPE_SIGNAL:
		__ASSERT(event->signal != NULL, "invalid poll signal\n");
		add_event(&event->signal->poll_events, event);
		break;
// 	case K_POLL_TYPE_MSGQ_DATA_AVAILABLE:
// 		__ASSERT(event->msgq != NULL, "invalid message queue\n");
// 		add_event(&event->msgq->poll_events, event, poller);
// 		break;
// #ifdef CONFIG_PIPES
// 	case K_POLL_TYPE_PIPE_DATA_AVAILABLE:
// 		__ASSERT(event->pipe != NULL, "invalid pipe\n");
// 		add_event(&event->pipe->poll_events, event, poller);
// 		break;
// #endif
	case K_POLL_TYPE_IGNORE:
		/* nothing to do */
		break;
	default:
		__ASSERT(false, "invalid event type\n");
		break;
	}
}

static inline void set_event_ready(struct k_poll_event *event, uint32_t state)
{
	event->state |= state;
}

static int register_events(struct k_poll_event *events,
				  int num_events,
				  bool just_check,
				  bool *condition_met)
{
	int events_registered = 0;

	*condition_met = false;

	for (int ii = 0; ii < num_events; ii++) {
		k_spinlock_key_t key;
		uint32_t state;

		key = k_spin_lock(&lock);
		if (is_condition_met(&events[ii], &state)) {
			set_event_ready(&events[ii], state);
			*condition_met = true;
		} else if (!just_check) {
			register_event(&events[ii]);
			events_registered += 1;
		} else {
			/* Event is not one of those identified in is_condition_met()
			 * catching non-polling events, or is marked for just check,
			 * or not marked for polling. No action needed.
			 */
			;
		}
		k_spin_unlock(&lock, key);
	}

	return events_registered;
}

static void *_k_poll_sem_get(void)
{
	void *sem = NULL;
	k_spinlock_key_t key;
	int idx = 0;

	key = k_spin_lock(&lock);
	for (idx = 0; idx < POLL_SEM_NUM_MAX; idx ++) {
		if (poll_sems[idx].available) {
			poll_sems[idx].available = false;
			sem = poll_sems[idx].sem;
			break;
		}
	}
	k_spin_unlock(&lock, key);

	BT_ASSERT(sem);
	return sem;
}

static void _k_poll_sem_put(void *sem)
{
	k_spinlock_key_t key;
	int idx = 0;

	if (!sem)
		return;

	key = k_spin_lock(&lock);
	for (idx = 0; idx < POLL_SEM_NUM_MAX; idx ++) {
		if (poll_sems[idx].sem == sem) {
			poll_sems[idx].available = true;
			break;
		}
	}
	k_spin_unlock(&lock, key);
}

int k_poll(struct k_poll_event *events, int num_events, k_timeout_t timeout)
{
	int events_registered;
	bool condition_met, just_check;
	int rc = 0;
	void *sem = NULL;
	k_spinlock_key_t key;

	just_check = K_TIMEOUT_EQ(timeout, K_NO_WAIT);

	if (!just_check) {
		int idx_num = num_events;

		sem = _k_poll_sem_get();

		if (!sem) {
			return -ENOMEM;
		}

		while (idx_num--) {
			key = k_spin_lock(&lock);
			clear_event_registration(&events[idx_num]);
			events[idx_num].poll_sem = sem;
			k_spin_unlock(&lock, key);
		}
	}

	events_registered = register_events(events, num_events,
					    just_check, &condition_met);

	if (just_check) { /* No events will be registered */
		if (!condition_met)
			rc = -EAGAIN;
	} else { /* Events may be registered */
		if (!condition_met && events_registered) {
			if (!osif_sem_take(sem, timeout.ticks)) /* wait for condition met */
				rc = -1;
		}
		/* Clear all event registrations when condition is met or timeout. */
		clear_event_registrations(events, num_events);
	}

	if (!just_check) {
		_k_poll_sem_put(sem);
	}

	return rc;
}

static void signal_poll_event(sys_dlist_t *events, uint32_t state, k_spinlock_key_t key)
{
	struct k_poll_event *event;
	void *sem;
	
	event = (struct k_poll_event *)sys_dlist_get(events);
	if (!event) {
		k_spin_unlock(&lock, key);
		return;
	}

	set_event_ready(event, state);
	sem = event->poll_sem;
	event->poll_sem = NULL;
	k_spin_unlock(&lock, key);
	if (sem) {
		osif_sem_give(sem);
	}
}

int k_poll_signal_raise(struct k_poll_signal *sig, int result)
{
	k_spinlock_key_t key = k_spin_lock(&lock);

	sig->result = result;
	sig->signaled = 1U;

	signal_poll_event(&sig->poll_events, K_POLL_STATE_SIGNALED, key);
	return 0;
}

void z_handle_obj_poll_events(sys_dlist_t *events, uint32_t state)
{
	k_spinlock_key_t key = k_spin_lock(&lock);
	signal_poll_event(events, state, key);
}