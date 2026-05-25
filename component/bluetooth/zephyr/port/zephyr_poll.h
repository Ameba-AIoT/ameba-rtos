/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_POLL_H
#define ZEPHYR_POLL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr/sys/dlist.h>
#include <zephyr_sema.h>
#include <zephyr_queue.h>
#include <zephyr_lock.h>
#include <zephyr_time.h>

#define Z_POLL_TYPE_BIT(type) (1U << ((type) - 1U))
/* private - types bit positions */
enum _poll_types_bits {
	/* can be used to ignore an event */
	_POLL_TYPE_IGNORE,

	/* to be signaled by k_poll_signal_raise() */
	_POLL_TYPE_SIGNAL,

	/* semaphore availability */
	_POLL_TYPE_SEM_AVAILABLE,

	/* queue/FIFO/LIFO data availability */
	_POLL_TYPE_DATA_AVAILABLE,

	/* msgq data availability */
	_POLL_TYPE_MSGQ_DATA_AVAILABLE,

	/* pipe data availability */
	_POLL_TYPE_PIPE_DATA_AVAILABLE,

	_POLL_NUM_TYPES
};

#define Z_POLL_STATE_BIT(state) (1U << ((state) - 1U))

/* private - states bit positions */
enum _poll_states_bits {
	/* default state when creating event */
	_POLL_STATE_NOT_READY,

	/* signaled by k_poll_signal_raise() */
	_POLL_STATE_SIGNALED,

	/* semaphore is available */
	_POLL_STATE_SEM_AVAILABLE,

	/* data is available to read on queue/FIFO/LIFO */
	_POLL_STATE_DATA_AVAILABLE,

	/* queue/FIFO/LIFO wait was cancelled */
	_POLL_STATE_CANCELLED,

	/* data is available to read on a message queue */
	_POLL_STATE_MSGQ_DATA_AVAILABLE,

	/* data is available to read from a pipe */
	_POLL_STATE_PIPE_DATA_AVAILABLE,

	_POLL_NUM_STATES
};

/* public - polling modes */
enum k_poll_modes {
	/* polling thread does not take ownership of objects when available */
	K_POLL_MODE_NOTIFY_ONLY = 0,

	K_POLL_NUM_MODES
};


/* public - values for k_poll_event.state bitfield */
#define K_POLL_STATE_NOT_READY 0
#define K_POLL_STATE_SIGNALED Z_POLL_STATE_BIT(_POLL_STATE_SIGNALED)
#define K_POLL_STATE_SEM_AVAILABLE Z_POLL_STATE_BIT(_POLL_STATE_SEM_AVAILABLE)
#define K_POLL_STATE_DATA_AVAILABLE Z_POLL_STATE_BIT(_POLL_STATE_DATA_AVAILABLE)
#define K_POLL_STATE_FIFO_DATA_AVAILABLE K_POLL_STATE_DATA_AVAILABLE
#define K_POLL_STATE_MSGQ_DATA_AVAILABLE Z_POLL_STATE_BIT(_POLL_STATE_MSGQ_DATA_AVAILABLE)
#define K_POLL_STATE_PIPE_DATA_AVAILABLE Z_POLL_STATE_BIT(_POLL_STATE_PIPE_DATA_AVAILABLE)
#define K_POLL_STATE_CANCELLED Z_POLL_STATE_BIT(_POLL_STATE_CANCELLED)

/* public - values for k_poll_event.type bitfield */
#define K_POLL_TYPE_IGNORE 0
#define K_POLL_TYPE_SIGNAL Z_POLL_TYPE_BIT(_POLL_TYPE_SIGNAL)
#define K_POLL_TYPE_SEM_AVAILABLE Z_POLL_TYPE_BIT(_POLL_TYPE_SEM_AVAILABLE)
#define K_POLL_TYPE_DATA_AVAILABLE Z_POLL_TYPE_BIT(_POLL_TYPE_DATA_AVAILABLE)
#define K_POLL_TYPE_FIFO_DATA_AVAILABLE K_POLL_TYPE_DATA_AVAILABLE
#define K_POLL_TYPE_MSGQ_DATA_AVAILABLE Z_POLL_TYPE_BIT(_POLL_TYPE_MSGQ_DATA_AVAILABLE)
#define K_POLL_TYPE_PIPE_DATA_AVAILABLE Z_POLL_TYPE_BIT(_POLL_TYPE_PIPE_DATA_AVAILABLE)

#define _POLL_EVENT_NUM_UNUSED_BITS \
	(32 - (0 \
	       + 8 /* tag */ \
	       + _POLL_NUM_TYPES \
	       + _POLL_NUM_STATES \
	       + 1 /* modes */ \
	      ))



#define K_POLL_EVENT_STATIC_INITIALIZER(_event_type, _event_mode, _event_obj, \
					event_tag) \
	{ \
	.tag = event_tag, \
	.type = _event_type, \
	.state = K_POLL_STATE_NOT_READY, \
	.mode = _event_mode, \
	.unused = 0, \
	{ \
		.obj = _event_obj, \
	}, \
	}


/* public - poll signal object */
struct k_poll_signal {
	/** PRIVATE - DO NOT TOUCH */
	sys_dlist_t poll_events;

	/**
	 * 1 if the event has been signaled, 0 otherwise. Stays set to 1 until
	 * user resets it to 0.
	 */
	uint32_t signaled;

	/** custom result value passed to k_poll_signal_raise() if needed */
	int32_t result;
};

struct z_poller {
	bool is_polling;
	uint8_t mode;
};


struct k_poll_event {
	/** PRIVATE - DO NOT TOUCH */
	sys_dnode_t _node;

	/** PRIVATE - DO NOT TOUCH */
	void *poll_sem;

	/** optional user-specified tag, opaque, untouched by the API */
	uint32_t tag:8;

	/** bitfield of event types (bitwise-ORed K_POLL_TYPE_xxx values) */
	uint32_t type:_POLL_NUM_TYPES;

	/** bitfield of event states (bitwise-ORed K_POLL_STATE_xxx values) */
	uint32_t state:_POLL_NUM_STATES;

	/** mode of operation, from enum k_poll_modes */
	uint32_t mode:1;

	/** unused bits in 32-bit word */
	uint32_t unused:_POLL_EVENT_NUM_UNUSED_BITS;

	/** per-type data */
	union {
		void *obj;
		struct k_poll_signal *signal;
		struct k_sem *sem;
		struct k_fifo *fifo;
		struct k_queue *queue;
		// struct k_msgq *msgq;
	};
};

#define K_POLL_SIGNAL_INITIALIZER(obj) \
	{ \
	.poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events), \
	.signaled = 0, \
	.result = 0, \
	}

void poll_static_sem_init(void);
void poll_static_sem_deinit(void);
void k_poll_event_init(struct k_poll_event *event, uint32_t type, int mode, void *obj);
void k_poll_signal_init(struct k_poll_signal *sig);
int k_poll(struct k_poll_event *events, int num_events, k_timeout_t timeout);
int k_poll_signal_raise(struct k_poll_signal *sig, int result);
void z_handle_obj_poll_events(sys_dlist_t *events, uint32_t state);

#ifdef __cplusplus
}
#endif

#endif
