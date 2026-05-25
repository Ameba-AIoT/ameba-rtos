#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "ameba_soc.h"
#include "osif.h"
#include "serial_api.h"
#include "serial_ex_api.h"
#include <bt_debug.h>
#include <zephyr_log.h>

void z_printf(const char *format, ...)
{
    va_list ap;

    va_start(ap, format);
	DiagVprintf(format, ap);
    va_end(ap);
}

#if (ZLOG_SNOOP_ON || (ZLOG_STACK_MODE == ZLOG_STACK_MODE_ARGS))

#if defined(CONFIG_AMEBAD)
#define ZLOG_TO_LOGUART 0
#else
#define ZLOG_TO_LOGUART 1
#endif
#define ZLOG_BY_THREAD 	1

/*
 * If thread is not used, it is suggested to set ZLOG_MAX_LEVEL less than LOG_LEVEL_DBG.
 * Otherwise uart tx conflict may occurr because of too many logs.
 * Uart tx conflict brings disorder of some logs.
 */

#define BTLOG_HDR_SIZE 3
#define BIN_MAX_LEN 0xFD
#define BTLOG_MAX_LEN (BIN_MAX_LEN - BTLOG_HDR_SIZE)

#define SNOOP_HDR_SIZE 2

/* Do not modify, this is consistent with trace tool */
#define LOG_MESSAGE_SYNC_CODE 0x7E
#define LOG_MESSAGE_MORE_DATA 0x80

#define BTLOG_TYPE_SNOOP            0
#define BTLOG_TYPE_STACK_ARGS       2
#define BTLOG_TYPE_COEX			    3
#define BTLOG_TYPE_EXIT             0xFF

#if ZLOG_BY_THREAD
#define BTLOG_TASK_RUNNING  1
#define BTLOG_TASK_CLOSING  2
#define BTLOG_TASK_CLOSED   3

#if (ZLOG_STACK_MODE == ZLOG_STACK_MODE_ARGS)
#define Z_LOG_QUEUE_SIZE 1000
#else
#define Z_LOG_QUEUE_SIZE 20
#endif

#define BTLOG_STACK_SIZE 1024
#define BTLOG_PRIORITY 5

void *btlog_q;
void *btlog_task_hdl;

static uint8_t btlog_task_status = 0;
static uint32_t btlog_task_msg_num = 0;
#else
static void *btlog_mutex = NULL;
#endif /* #if ZLOG_BY_THREAD */

typedef struct {
	uint8_t type;
	uint8_t *data;
	uint16_t len;
} btlog_message;


#if !ZLOG_TO_LOGUART

#if defined(CONFIG_AMEBASMART)
#define BTLOG_UART_PIN_TX  _PA_3
#define BTLOG_UART_PIN_RX  _PA_2
#elif defined(CONFIG_AMEBALITE) || defined(CONFIG_AMEBADPLUS)
#define BTLOG_UART_PIN_TX  _PA_28
#elif defined(CONFIG_AMEBADPLUS)
#define BTLOG_UART_PIN_TX  _PA_26
#elif defined(CONFIG_AMEBAD)
#define BTLOG_UART_PIN_TX  _PA_26
#define BTLOG_UART_PIN_RX  _PA_25
#endif

static serial_t sobj;

static void btlog_uart_init(void)
{
	serial_init(&sobj, BTLOG_UART_PIN_TX, BTLOG_UART_PIN_RX);
	serial_baud(&sobj, 1500000);
	serial_format(&sobj, 8, ParityNone, 1);
}

static void btlog_uart_deinit(void)
{
	serial_free(&sobj);
}

static void btlog_uart_tx(uint8_t *data, uint16_t len)
{
	serial_send_blocked(&sobj, (char *)data, len, len);
}
#endif

static void btlog_send_packet(uint8_t *hdr, uint8_t hdrlen, uint8_t *data, uint8_t len)
{
#if ZLOG_TO_LOGUART

#if defined(CONFIG_ARM_CORE_CM4) && CONFIG_ARM_CORE_CM4
	/* Judge if log uart is ready to send, if not ready, yield task to avoid
	LOGUART_BT_SendData to block wait, ref RSWLAND-1587 Q7.1. This can decrease
	CPU use rate of btlog thread and remove music lag in AmebaLite KM4 when
	btsnoop log open, but it may also lead to some btsnoop packet missing. */
	while (!(LOGUART_GetStatus(LOGUART_DEV) & LOGUART_BIT_TP3F_NOT_FULL)) {
		osif_delay(1);
	}
	LOGUART_BT_SendData(hdr, hdrlen);
	while (!(LOGUART_GetStatus(LOGUART_DEV) & LOGUART_BIT_TP3F_NOT_FULL)) {
		osif_delay(1);
	}
	LOGUART_BT_SendData(data, len);
#else /* CONFIG_ARM_CORE_CM4 */
	LOGUART_BT_SendData(hdr, hdrlen);
	LOGUART_BT_SendData(data, len);
#endif /* CONFIG_ARM_CORE_CM4 */

#else
	btlog_uart_tx(hdr, hdrlen);
	btlog_uart_tx(data, len);
#endif
}

static void btlog_write_hdr(uint8_t type, uint8_t *hdr, uint16_t datalen)
{
	uint16_t len = datalen;

	/* Fist byte is Marker.
	 * Second byte is length.
	 *   Trace tool depends on this format to append timestamp to every message.
	 *   Trace tool considers 0xFE & 0xFF as invalid length.
	 * Third byte is packet type, MSB is more data bit.
	 */

	hdr[0] = LOG_MESSAGE_SYNC_CODE;
	hdr[2] = type;
	if (len > BTLOG_MAX_LEN) { /* fragment */
		len = BTLOG_MAX_LEN;
		hdr[2] |= LOG_MESSAGE_MORE_DATA;
	}
	hdr[1] = len + BTLOG_HDR_SIZE; /* Compression length to 1 byte to fit trace tool format */
}

static void btlog_send_data(uint8_t type, uint8_t *data, uint16_t datalen)
{
	uint16_t index = 0;
	uint8_t hdr[BTLOG_HDR_SIZE];

	while (index < datalen) {
		btlog_write_hdr(type, hdr, datalen - index);
		btlog_send_packet(hdr, BTLOG_HDR_SIZE, data + index, hdr[1] - BTLOG_HDR_SIZE);
		index += (hdr[1] - BTLOG_HDR_SIZE);
	}
}

#if ZLOG_BY_THREAD
static void btlog_taskentry(void *arg)
{
	(void)arg;
	bool running = true;
	btlog_message *msg;

	while (running) {
		if (!osif_msg_recv(btlog_q, &msg, BT_TIMEOUT_FOREVER)) {
			continue;
		}

		if (msg->type == BTLOG_TYPE_EXIT) {
			running = false;
		} else {
			btlog_send_data(msg->type, msg->data, msg->len);
		}
		osif_mem_free(msg);
	}

	btlog_task_status = BTLOG_TASK_CLOSED;
	osif_task_delete(NULL);
}
#endif

static void btlog_send(btlog_message *msg)
{
#if ZLOG_BY_THREAD
	uint32_t flags = 0;

	flags = osif_lock();
	btlog_task_msg_num++;
	osif_unlock(flags);

	if (btlog_task_status != BTLOG_TASK_RUNNING) {
		osif_mem_free(msg);
		goto end;
	}

	if (false == osif_msg_send(btlog_q, &msg, BT_TIMEOUT_NONE)) {
		osif_mem_free(msg);
	}

end:
	flags = osif_lock();
	btlog_task_msg_num--;
	osif_unlock(flags);

#else

	if (!osif_sched_is_suspended()) { /* fix hang when print log during k_sched_lock/k_sched_unlock */
		osif_mutex_take(btlog_mutex, BT_TIMEOUT_FOREVER);
	}

	btlog_send_data(msg->type, msg->data, msg->len);

	if (!osif_sched_is_suspended()) {
		osif_mutex_give(btlog_mutex);
	}

	osif_mem_free(msg);

#endif
}

#endif  /* #if (ZLOG_SNOOP_ON || (ZLOG_STACK_MODE == ZLOG_STACK_MODE_ARGS)) */

void zephyr_btlog_init(void)
{
#if (ZLOG_SNOOP_ON || (ZLOG_STACK_MODE == ZLOG_STACK_MODE_ARGS))

#if ZLOG_TO_LOGUART
	rtk_bt_trace_log_open();
#else
	btlog_uart_init();
#endif

#if ZLOG_BY_THREAD
	osif_msg_queue_create(&btlog_q, Z_LOG_QUEUE_SIZE, sizeof(void *));
	osif_task_create(&btlog_task_hdl, "btlog", btlog_taskentry, NULL, BTLOG_STACK_SIZE, BTLOG_PRIORITY);

	btlog_task_status = BTLOG_TASK_RUNNING;
#else
	osif_mutex_create(&btlog_mutex);
#endif /* #if ZLOG_BY_THREAD */

#endif  /* #if (ZLOG_SNOOP_ON || (ZLOG_STACK_MODE == ZLOG_STACK_MODE_ARGS)) */
}

void zephyr_btlog_deinit(void)
{
#if (ZLOG_SNOOP_ON || (ZLOG_STACK_MODE == ZLOG_STACK_MODE_ARGS))

#if ZLOG_BY_THREAD
	btlog_message *msg = (btlog_message *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(btlog_message));

	msg->type = BTLOG_TYPE_EXIT;
	btlog_task_status = BTLOG_TASK_CLOSING;
	/* Waiting btlog_send() on other tasks interrupted by deinit task to complete */
	while (btlog_task_msg_num) {
		osif_delay(5);
	}

	osif_msg_send(btlog_q, &msg, BT_TIMEOUT_NONE);

	while (btlog_task_status != BTLOG_TASK_CLOSED) {
		osif_delay(5);
	}

	osif_msg_queue_delete(btlog_q);
#else
	osif_mutex_delete(btlog_mutex);
#endif /* #if ZLOG_BY_THREAD */

#if ZLOG_TO_LOGUART
	/* Keep loguart channel always on for coex log. */
	// rtk_bt_trace_log_close();
#else
	btlog_uart_deinit();
#endif

#endif  /* #if (ZLOG_SNOOP_ON || (ZLOG_STACK_MODE == ZLOG_STACK_MODE_ARGS)) */
}


void btsnoop_send(uint8_t type, uint8_t *data, uint32_t len, bool c2h)
{
#if ZLOG_SNOOP_ON
	btlog_message *msg;

	msg = (btlog_message *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(btlog_message) + len + SNOOP_HDR_SIZE);
	if (!msg) {
		return;
	}

	msg->type = BTLOG_TYPE_SNOOP;

	msg->data = (uint8_t *)msg + sizeof(btlog_message);
	msg->len = len + SNOOP_HDR_SIZE;
	/* Fist byte is Marker, second & third bytes are length. */
	msg->data[0] = c2h ? 1 : 0;
	msg->data[1] = type;
	memcpy(msg->data + SNOOP_HDR_SIZE, data, len);
	btlog_send(msg);
#else
	(void)type;
	(void)data;
	(void)len;
	(void)c2h;
#endif
}

/* stacklog args format
* | 0x7E | len | type ||| level | log_idx | param_num |   params    |
* |   1  |  1  |  1   |||   1   |   4     |   1       | param_num*4 |
*/
void zephyr_stacklog_args_send(char level, int log_idx, char num, ...)
{
#if (ZLOG_STACK_MODE == ZLOG_STACK_MODE_ARGS)
	btlog_message *msg;
	int idx = 0, len = 2;
	bool hexdump = true;

	if (log_idx) {
		hexdump = false;
		len += ((num + 1) * sizeof(int));
	} else {
		len += (sizeof(int) + num * sizeof(char));
	}

	msg = (btlog_message *)osif_mem_alloc(RAM_TYPE_DATA_ON, len + sizeof(btlog_message));
	if (!msg) {
		return;
	}

	msg->type = BTLOG_TYPE_STACK_ARGS;

	msg->data = (uint8_t *)msg + sizeof(btlog_message);
	msg->len = len;
	idx = 0;
	msg->data[idx++] = level;
	msg->data[idx++] = (uint8_t)log_idx;
	msg->data[idx++] = (uint8_t)(log_idx >> 8);
	msg->data[idx++] = (uint8_t)(log_idx >> 16);
	msg->data[idx++] = (uint8_t)(log_idx >> 24);
	msg->data[idx++] = num;
	if (num) {
		va_list  arg;
		va_start(arg, num);
		if (!hexdump) {
			int i;
			for (i = 0; i < num; i++) {
				int param = va_arg(arg, int);
				msg->data[idx++] = (uint8_t)param;
				msg->data[idx++] = (uint8_t)(param >> 8);
				msg->data[idx++] = (uint8_t)(param >> 16);
				msg->data[idx++] = (uint8_t)(param >> 24);
			}
		} else {
			uint8_t *hex = va_arg(arg, uint8_t *);
			memcpy(&msg->data[idx], hex, num);
		}
		va_end(arg);
	}

	btlog_send(msg);
#else
	(void)level;
	(void)log_idx;
	(void)num;
#endif
}

uint32_t zlog_level_get(void)
{
	return ZLOG_MAX_LEVEL;
}