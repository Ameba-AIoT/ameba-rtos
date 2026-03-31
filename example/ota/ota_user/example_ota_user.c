#include "os_wrapper.h"
#include "ameba_soc.h"
#include "ota_api.h"
#include "serial_api.h"
#include "serial_ex_api.h"
#include "PinNames.h"
#include "ringbuffer.h"

#define UART_IDX      0

#if defined (CONFIG_AMEBASMART)
#define UART_TX       PA_3 // UART0 TX
#define UART_RX       PA_2 // UART0 RX
#define UART_RTS      PA_5 // UART0 RTS
#define UART_CTS      PA_4 // UART0 CTS

#elif defined (CONFIG_AMEBALITE)
#define UART_TX       PA_28 // UART0 TX
#define UART_RX       PA_29 // UART0 RX
#define UART_RTS      PA_30 // UART0 RTS
#define UART_CTS      PA_31 // UART0 CTS

#elif defined (CONFIG_AMEBADPLUS)
#define UART_TX       PB_17 // UART0 TX
#define UART_RX       PB_18 // UART0 RX
#define UART_RTS      PB_19 // UART0 RTS
#define UART_CTS      PB_20 // UART0 CTS

#elif defined (CONFIG_AMEBAGREEN2)
/* fully programmable zone */
#define UART_TX        PA_26 // UART TX
#define UART_RX        PA_25 // UART RX
#define UART_RTS       PA_24 // UART0 RTS
#define UART_CTS       PA_23 // UART0 CTS

#else
#define UART_TX        PA_26 // UART TX
#define UART_RX        PA_25 // UART RX
#define UART_RTS       PA_24 // UART0 RTS
#define UART_CTS       PA_23 // UART0 CTS

#endif

#define UART_BAUD     1500000
#define READ_TIMEOUT  30000 // ms

extern void sys_reset(void);
serial_t sobj;
static RingBuffer *ota_rx_ring_buf = NULL;

void ota_uart_irq_handler(uint32_t id, SerialIrq event)
{
	serial_t *sobj = (serial_t *)id;
	unsigned char rc = 0;
	u32 space = 0;

	if (event == RxIrq) {
		rc = serial_getc(sobj);
		space = RingBuffer_Space(ota_rx_ring_buf);
		if (space > 0) {
			RingBuffer_Write(ota_rx_ring_buf, &rc, 1);
		} else {
			printf("%s, space %d !!!!\n", __func__, (int)space);
		}
	}
}

int ota_user_read(u8 *buf, int len)
{
	int size = -1;
	int avail_len = 0;
	int timeout = 0;

	do {
		avail_len = RingBuffer_Available(ota_rx_ring_buf);
		// if (avail_len > 0) {
		// 	size = avail_len > len ? len : avail_len;
		// 	RingBuffer_Read(ota_rx_ring_buf, buf, size);
		// 	return size;
		// }
		if (avail_len == 0) {
			rtos_time_delay_ms(10);
			timeout += 10;
		}
	} while (avail_len == 0 && timeout < READ_TIMEOUT);

	if (avail_len > 0) {
		size = avail_len > len ? len : avail_len;
		RingBuffer_Read(ota_rx_ring_buf, buf, size);
		return size;
	}
	return size;
}

int ota_user_open(void)
{
	serial_send_stream(&sobj, "hello", 5);
	return OTA_OK;
}

void ota_task(void *param)
{
	(void)param;
	ota_context_t *ctx = NULL;
	int ret = -1;

	rtos_time_delay_ms(2000);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r\n\r\n\r\n\r<<<<<< OTA USER Example >>>>>>>\n\r\n\r\n\r\n\r");

	sobj.uart_idx = UART_IDX;
	serial_init(&sobj, UART_TX, UART_RX);
	serial_baud(&sobj, UART_BAUD);
	serial_format(&sobj, 8, ParityNone, 1);
	serial_rx_fifo_level(&sobj, FifoLv1Byte);
	serial_set_flow_control(&sobj, FlowControlRTSCTS, UART_RTS, UART_CTS);
	serial_irq_handler(&sobj, ota_uart_irq_handler, (uint32_t)&sobj);
	serial_irq_set(&sobj, RxIrq, ENABLE);

	ota_rx_ring_buf = RingBuffer_Create(NULL, 4 * 1024, LOCAL_RINGBUFF, 1);
	if (!ota_rx_ring_buf) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "ota_rx_ring_buf malloc failed\r\n");
		goto exit;
	}

	ctx = (ota_context_t *)rtos_mem_malloc(sizeof(ota_context_t));
	if (!ctx) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "ctx malloc failed\r\n");
		goto exit;
	}

	memset(ctx, 0, sizeof(ota_context_t));

	ret = ota_init(ctx, NULL, 0, NULL, OTA_USER);
	if (ret != OTA_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "ota_init failed\r\n");
		goto exit;
	}

	ota_register_user_read_func(ctx, ota_user_read);
	ota_register_user_open_func(ctx, ota_user_open);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "ota user download start\r\n");

	ret = ota_start(ctx);

	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%s] OTA task exit\n", __FUNCTION__);
	if (ret == OTA_OK) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[%s] Ready to reboot\n", __FUNCTION__);
		rtos_time_delay_ms(20);
		sys_reset();
	}

exit:
	serial_free(&sobj);
	ota_deinit(ctx);
	if (ctx) {
		rtos_mem_free(ctx);
	}
	if (ota_rx_ring_buf) {
		RingBuffer_Destroy(ota_rx_ring_buf);
	}
	rtos_task_delete(NULL);
}

void example_ota_user(void)
{
	rtos_task_t task;
	if (rtos_task_create(&task, ((const char *)"ota_task"), ota_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r[%s] Create ota_task failed\n", __FUNCTION__);
	}
}
