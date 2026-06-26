/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "log_forward.h"

/*
 * Architecture: character-level hook on the ROM's DiagFunc pointer.
 *
 * Every console output — RTK_LOGx, RTK_LOGS, printf, and importantly logs from
 * WiFi/BT code that lives in ROM — funnels through the ROM's out_buffer():
 *     if (DiagFunc) DiagFunc(c, idx); else DiagPutChar(c);
 * DiagFunc is a RAM pointer (default NULL). By pointing it at log_diag_hook we
 * intercept *all* output, including ROM code we cannot patch at link time.
 *
 * The hook only receives single characters (formatting already happened inside
 * ROM), so our stack stays tiny — no DiagVSNprintf engine (~304B) runs in the
 * caller context, and no malloc. Characters are accumulated per-context into a
 * line buffer; on '\n' (or when full) the whole line is pushed to the ring
 * buffer, then drained by log_fwd_sender_task and sent over SDIO.
 */

#define LOG_FWD_RING_SIZE   3072
#define LOG_FWD_TASK_STACK  640    /* sender task: read_buf(128) + call chain, used ~280B */
#define LOG_FWD_TASK_PRIO   3

/*
 * Per-context line buffers (accumulate chars until '\n').
 * slot[0] is reserved for interrupt context; slot[1..] are handed to tasks
 * (one per task, matched by task handle). A task holds its slot only until the
 * line ends, so a handful of slots covers all tasks that log concurrently.
 * If every task slot is busy (more concurrent half-lines than slots) the caller
 * falls back to slot[0] — output may interleave, exactly as the original
 * unlocked DiagPutChar would, which is acceptable.
 */
#ifndef LOG_FWD_LINE_SLOTS
#define LOG_FWD_LINE_SLOTS  3      /* 1 interrupt + 2 task slots */
#endif
#if (LOG_FWD_LINE_SLOTS < 2)
#error "LOG_FWD_LINE_SLOTS must be >= 2 (slot0 for ISR, >=1 for tasks)"
#endif
#define LOG_FWD_ISR_SLOT    0

/* IPC_SEM_LOG_FWD_RING guards the ring buffer; IPC_SEM_LOG_FWD_SLOT guards
 * line-slot owner assignment.  Both are defined in ameba_sema_rom.h. */

typedef struct {
	char     buf[LOG_FWD_RING_SIZE];
	volatile u32 write_idx;
	volatile u32 read_idx;
	volatile u32 drop_count;
	rtos_sema_t  data_sem;
} log_fwd_ring_t;

typedef struct {
	log_fwd_ring_t       ring;
	rtk_log_output_fn    output_fn;
	volatile bool        enabled;
} log_fwd_state_t;

static log_fwd_state_t g_fwd;

typedef struct {
	volatile rtos_task_t owner;   /* task that owns this slot; NULL = free (task slots only) */
	u16  len;
	char buf[LOG_FWD_LINE_MAX];
} log_line_t;

static log_line_t g_line[LOG_FWD_LINE_SLOTS];

/* ROM console char sink (RAM pointer, default NULL -> DiagPutChar). */
extern void (*DiagFunc)(char character, size_t idx);

/* ---- ring buffer (single consumer: sender task; multi producer: hook) ---- */

static u32 ring_space(void)
{
	u32 w = g_fwd.ring.write_idx;
	u32 r = g_fwd.ring.read_idx;
	if (w >= r) {
		return (LOG_FWD_RING_SIZE - 1) - (w - r);
	} else {
		return r - w - 1;
	}
}

static u32 ring_avail(void)
{
	u32 w = g_fwd.ring.write_idx;
	u32 r = g_fwd.ring.read_idx;
	if (w >= r) {
		return w - r;
	} else {
		return LOG_FWD_RING_SIZE - r + w;
	}
}

static void ring_write(const char *data, u32 len)
{
	u32 w = g_fwd.ring.write_idx;
	u32 tail = LOG_FWD_RING_SIZE - w;

	if (tail > len) {
		_memcpy(&g_fwd.ring.buf[w], data, len);
		g_fwd.ring.write_idx = w + len;
	} else if (tail == len) {
		_memcpy(&g_fwd.ring.buf[w], data, len);
		g_fwd.ring.write_idx = 0;
	} else {
		_memcpy(&g_fwd.ring.buf[w], data, tail);
		_memcpy(&g_fwd.ring.buf[0], data + tail, len - tail);
		g_fwd.ring.write_idx = len - tail;
	}
}

static u32 ring_read(char *out, u32 size)
{
	u32 avail = ring_avail();
	u32 to_read = (size < avail) ? size : avail;
	if (to_read == 0) {
		return 0;
	}

	u32 r = g_fwd.ring.read_idx;
	u32 tail = LOG_FWD_RING_SIZE - r;

	if (tail >= to_read) {
		_memcpy(out, &g_fwd.ring.buf[r], to_read);
		g_fwd.ring.read_idx = (tail == to_read) ? 0 : r + to_read;
	} else {
		_memcpy(out, &g_fwd.ring.buf[r], tail);
		_memcpy(out + tail, &g_fwd.ring.buf[0], to_read - tail);
		g_fwd.ring.read_idx = to_read - tail;
	}

	return to_read;
}

/*
 * Push one complete line to the ring as [u16 len][data], written atomically
 * under IPC_SEM so the reader always gets a whole line (no partial messages).
 */
static void log_fwd_submit(const char *data, u32 len)
{
	if (len == 0) {
		return;
	}
	if (len > LOG_FWD_LINE_MAX) {
		len = LOG_FWD_LINE_MAX;
	}

	u16 msg_len = (u16)len;
	if (IPC_SEMTake(IPC_SEM_LOG_FWD_RING, 0) == TRUE) {
		if (ring_space() >= sizeof(u16) + len) {
			ring_write((const char *)&msg_len, sizeof(u16));
			ring_write(data, len);
			IPC_SEMFree(IPC_SEM_LOG_FWD_RING);
			rtos_sema_give(g_fwd.ring.data_sem);
		} else {
			g_fwd.ring.drop_count++;   /* ring truly full (back-pressure) */
			IPC_SEMFree(IPC_SEM_LOG_FWD_RING);
		}
	} else {
		g_fwd.ring.drop_count++;
	}
}

/* ---- per-context line accumulation ---- */

/* Pick the line slot for the current context. ISR -> slot0; task -> its own
 * slot (matched/allocated by handle). Falls back to slot0 if all task slots
 * are busy (rare: more concurrent half-lines than slots). */
static log_line_t *line_get(void)
{
	if (CPU_InInterrupt()) {
		return &g_line[LOG_FWD_ISR_SLOT];
	}

	rtos_task_t me = rtos_task_handle_get();
	log_line_t *s = &g_line[LOG_FWD_ISR_SLOT];   /* fallback */

	if (IPC_SEMTake(IPC_SEM_LOG_FWD_SLOT, 0) == TRUE) {
		int i;
		for (i = 1; i < LOG_FWD_LINE_SLOTS; i++) {     /* already mine? */
			if (g_line[i].owner == me) {
				s = &g_line[i];
				goto out;
			}
		}
		for (i = 1; i < LOG_FWD_LINE_SLOTS; i++) {     /* take a free one */
			if (g_line[i].owner == NULL) {
				g_line[i].owner = me;
				g_line[i].len = 0;
				s = &g_line[i];
				goto out;
			}
		}
out:
		IPC_SEMFree(IPC_SEM_LOG_FWD_SLOT);
	}
	return s;
}

/* Release a task slot back to the free set (slot0/ISR stays resident). */
static void line_put(log_line_t *s)
{
	if (s == &g_line[LOG_FWD_ISR_SLOT]) {
		return;
	}
	if (IPC_SEMTake(IPC_SEM_LOG_FWD_SLOT, 0) == TRUE) {
		s->owner = NULL;
		IPC_SEMFree(IPC_SEM_LOG_FWD_SLOT);
	} else {
		s->owner = NULL;   /* single pointer write, atomic enough */
	}
}

/*
 * The DiagFunc hook: called by ROM/RAM for every console character.
 *  - not enabled : pass straight through to UART (original behaviour).
 *  - enabled     : accumulate into the per-context line; flush on '\n' or full.
 *
 * Task-owned slots (1..N) are written only by their owning task, so they
 * need no extra lock beyond the IPC_SEM that guards owner assignment.
 * Slot0 is shared between ISR context and task-fallback (all task slots busy).
 * No lock is taken around slot0 writes: the fallback path is extremely rare,
 * and adding irq_disable would pull log_fwd_submit into the critical window —
 * rtos_sema_give inside log_fwd_submit must not be called with interrupts
 * fully disabled.  Concurrent slot0 writes degrade to interleaved output,
 * the same behaviour as the original unlocked DiagPutChar.
 */
static void log_diag_hook(char c, size_t idx)
{
	(void)idx;

	if (!g_fwd.enabled) {
		DiagPutChar(c);
		return;
	}

	log_line_t *s = line_get();
	if (s->len < LOG_FWD_LINE_MAX) {
		s->buf[s->len++] = c;
	}
	if (c == '\n' || s->len >= LOG_FWD_LINE_MAX) {
		log_fwd_submit(s->buf, s->len);
		s->len = 0;
		line_put(s);   /* no-op for slot0 */
	}
}

/*
 * printf hook (called from __wrap_printf). The C library path
 * printf->vprintf->_write->DiagPutChar bypasses DiagFunc, so redirect printf
 * through DiagVprintf, which goes via out_buffer -> DiagFunc -> our hook.
 * Returning >=0 tells __wrap_printf the line is handled (skip its own UART).
 */
int rtk_printf_hook(const char *fmt, va_list ap)
{
	return DiagVprintf(fmt, ap);
}

/* ---- consumer ---- */

static void log_fwd_sender_task(void *arg)
{
	(void)arg;
	char read_buf[LOG_FWD_LINE_MAX];
	u16 msg_len;

	while (1) {
		rtos_sema_take(g_fwd.ring.data_sem, RTOS_MAX_TIMEOUT);

		/* Drain all complete [u16 len][data] messages */
		while (ring_read((char *)&msg_len, sizeof(u16)) == sizeof(u16)) {
			if (msg_len > sizeof(read_buf)) {
				msg_len = sizeof(read_buf);   /* defensive; should never happen */
			}
			u32 n = ring_read(read_buf, msg_len);
			if (g_fwd.output_fn) {
				g_fwd.output_fn((const u8 *)read_buf, n);
			}
		}
	}
}

void rtk_log_forward_init(rtk_log_output_fn fn)
{
	if (DiagFunc == log_diag_hook) {
		return;   /* already initialised; a second call would leak a task */
	}

	g_fwd.output_fn = fn;
	g_fwd.enabled = false;

	/* Max count reflects realistic ring capacity: each message occupies at
	 * least sizeof(u16)+1 bytes, so the ring can hold at most this many. */
	rtos_sema_create(&g_fwd.ring.data_sem, 0, LOG_FWD_RING_SIZE / (sizeof(u16) + 1));

	if (RTK_SUCCESS != rtos_task_create(NULL, "log_fwd_sender", (rtos_task_function_t)log_fwd_sender_task,
										NULL, LOG_FWD_TASK_STACK, LOG_FWD_TASK_PRIO)) {
		RTK_LOGE(NOTAG, "Create log_fwd_sender Err\n");
		return;
	}

	/* Intercept all console output. While enabled==false the hook just calls
	 * DiagPutChar, so this is transparent until forwarding is turned on. */
	DiagFunc = log_diag_hook;
}

void rtk_log_forward_enable(void)
{
	if (g_fwd.enabled) {
		return;   /* already on: no-op; do NOT reset drop_count mid-session */
	}
	g_fwd.ring.drop_count = 0;   /* fresh session counter */
	g_fwd.enabled = true;
}

void rtk_log_forward_disable(void)
{
	if (!g_fwd.enabled) {
		return;   /* already off: skip flush and drop report to avoid duplicates */
	}

	/* Stop accepting new characters first.  After this point log_diag_hook
	 * will call DiagPutChar instead of accumulating into line slots, so the
	 * slot buffers are stable and can be flushed without racing writers. */
	g_fwd.enabled = false;

	/* Flush any half-lines (no trailing '\n') still in the line slots.
	 * Because enabled is now false no new chars arrive via log_diag_hook.
	 * log_fwd_submit has no enabled check — it always writes to the ring,
	 * so the residual bytes are forwarded to the host over SDIO. */
	int i;
	for (i = 0; i < LOG_FWD_LINE_SLOTS; i++) {
		if (g_line[i].len) {
			if (g_line[i].len < LOG_FWD_LINE_MAX) {
				g_line[i].buf[g_line[i].len++] = '\n';
			}
			log_fwd_submit(g_line[i].buf, g_line[i].len);
			g_line[i].len = 0;
		}
	}

	/* Report the per-session drop count to the host.  Cannot use RTK_LOGS
	 * here: enabled is false so log_diag_hook would call DiagPutChar (UART)
	 * instead of the ring.  Format directly and submit to reach dmesg. */
	if (g_fwd.ring.drop_count) {
		char msg[64];
		int n = DiagSnPrintf(msg, sizeof(msg),
							 "[log_fwd] dropped %u msg(s) this session\n",
							 (unsigned int)g_fwd.ring.drop_count);
		if (n > 0) {
			log_fwd_submit(msg, (u32)n);
		}
	}
}
