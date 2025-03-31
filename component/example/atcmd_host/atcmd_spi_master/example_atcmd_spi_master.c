#include "serial_api.h"
#include "serial_ex_api.h"
#include "gpio_api.h"
#include "gpio_irq_api.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "example_atcmd_spi_master.h"
#include "atcmd_service.h"
#include "ringbuffer.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"

#define ATCMD_SPI_DMA_SIZE	2048
#define SPI_SCLK_FREQ		20*1000*1000
#define SPI_DATA_FRAME_SIZE	8
#define SPI_MODE			0

#define MAX_CMD_LEN 2000
#define FORMAT_LEN 128

#define CHECKSUM_EN 0
#define TT_MODE_TEST 1

void *master_tx_done_sema;
void *master_rx_done_sema;
void *master_tt_mode_sema;
void *master_gpio_sema;

RingBuffer *atcmd_host_tx_ring_buf = NULL;

gpio_t at_spi_master_to_slave_gpio;
gpio_irq_t at_spi_slave_to_master_irq;

spi_t spi_master;
serial_t sobj;

char uart_format_buffer[FORMAT_LEN];

int current_edge = IRQ_FALL;
char uart_irq_buffer[MAX_CMD_LEN] = {0};
u32 uart_irq_count = 0;
volatile u8 stop_flag = 0;
volatile u8 tt_mode_task_start = 0;
volatile u8 send_flag = 0;

u8 MasterRxBuf[ATCMD_SPI_DMA_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 MasterTxBuf[ATCMD_SPI_DMA_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

void (*spi_recv_data_callback)(u8 *data, u16 len);

#if CHECKSUM_EN
static uint32_t checksum_32_spi(uint32_t start_value, uint8_t *data, int len)
{
	uint32_t checksum32 = start_value;
	uint16_t data16 = 0;
	int i;

	for (i = 0; i < (len / 2 * 2); i += 2) {
		data16 = (data[i] << 8) | data[i + 1];
		checksum32 += data16;
	}

	if (len % 2) {
		data16 = data[len - 1] << 8;
		checksum32 += data16;
	}

	return checksum32;
}
#endif

void spi_m2s_pin_set(u8 val)
{
	gpio_write(&at_spi_master_to_slave_gpio, val);
}

void spi_master_send(u8 *buf, u32 len)
{
	u32 space;

	while (1) {
		space = RingBuffer_Space(atcmd_host_tx_ring_buf);
		if (space > len) {
			RingBuffer_Write(atcmd_host_tx_ring_buf, (u8 *)buf, len);
			spi_m2s_pin_set(0);
			return;
		} else {
			vTaskDelay(1);
		}
	}
}

void uart_send_string(serial_t *sobj, char *pstr, u16 len)
{
	unsigned int i = 0;
	while (*(pstr + i) != 0 && i < len) {
		serial_putc(sobj, *(pstr + i));
		i++;
	}
}

void uart_format_string_output(const char *fmt, ...)
{
	int len_fmt = 0;
	va_list ap;
	va_start(ap, fmt);
	len_fmt = vsnprintf(uart_format_buffer, FORMAT_LEN, fmt, ap);
	va_end(ap);

	if (len_fmt < 0 || len_fmt >= FORMAT_LEN) {
		return;
	}

	uart_send_string(&sobj, (char *)uart_format_buffer, len_fmt);
}


void uart_irq(uint32_t id, SerialIrq event)
{
	serial_t *sobj = (void *)id;

	if (event == RxIrq) {
		while (serial_readable(sobj)) {
			if (uart_irq_count == MAX_CMD_LEN) {
				break;
			}

			uart_irq_buffer[uart_irq_count++] = serial_getc(sobj);
			serial_putc(sobj, uart_irq_buffer[uart_irq_count - 1]);
		}

		if ((uart_irq_count > 1 && uart_irq_buffer[uart_irq_count - 1] == '\n' && uart_irq_buffer[uart_irq_count - 2] == '\r')
			|| (uart_irq_count == MAX_CMD_LEN)) {
			spi_master_send((u8 *)uart_irq_buffer, uart_irq_count);
			memset(uart_irq_buffer, 0, MAX_CMD_LEN);
			uart_irq_count = 0;
		}
	}
}

void Master_tr_done_callback(uint32_t pdata, SpiIrq event)
{
	(void)pdata;

	BaseType_t task_woken = pdFALSE;

	switch (event) {
	case SpiRxIrq:
		xSemaphoreGiveFromISR(master_rx_done_sema, &task_woken);
		portEND_SWITCHING_ISR(task_woken);
		break;
	case SpiTxIrq:
		xSemaphoreGiveFromISR(master_tx_done_sema, &task_woken);
		portEND_SWITCHING_ISR(task_woken);
		break;
	default:
		break;
	}
}

void gpio_edge_irq_handler(uint32_t id, gpio_irq_event event)
{
	(void)event;
	(void)id;
	BaseType_t task_woken = pdFALSE;
	// master recv slave signal, prepare for reading
	xSemaphoreGiveFromISR(master_gpio_sema, &task_woken);
	portEND_SWITCHING_ISR(task_woken);
}

void atcmd_tt_mode_task(void)
{
	// same size as atcmd config size , for example:"AT+HTTPPOST=server_url,/post,1,204800,,2,Connection: close,Transfer-Encoding: chunked" or "AT+TEST=1,2097152"
	u32 tt_len = 2 * 1024 * 1024;
	u32 send_len;
	u8 *tt_tx_buf = pvPortMalloc(ATCMD_SPI_DMA_SIZE - 8);

	while (tt_len > 0) {
		send_len = tt_len > (ATCMD_SPI_DMA_SIZE - 8) ? (ATCMD_SPI_DMA_SIZE - 8) : tt_len;
		spi_master_send((u8 *)tt_tx_buf, send_len);
		xSemaphoreTake(master_tt_mode_sema, 0xFFFFFFFF);
		tt_len -= send_len;
		while (stop_flag) {
			vTaskDelay(1);
		}
	}

	vPortFree(tt_tx_buf);
	tt_mode_task_start = 0;
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "atcmd_tt_mode_task end\r\n");
	vTaskDelete(NULL);
}

void spi_handle_recv_data(u8 *data, u16 len)
{
	static u32 start_time = 0, end_time = 0, ds_count = 0;
	char *str_find = NULL;
#if TT_MODE_TEST
	if (len > 0 && (memcmp(&MasterRxBuf[4], ">>>", 3) == 0)) {
		if (xTaskCreate((void *)atcmd_tt_mode_task, ((const char *)"atcmd_tt_mode_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 4, NULL) != pdPASS) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s rtos_task_create(atcmd_tt_mode_task) failed", __FUNCTION__);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "atcmd_tt_mode_task start\r\n");
			tt_mode_task_start = 1;
		}
		return;
	}

	if (tt_mode_task_start) {
		if ((len > 0) && (memcmp(data, ATCMD_TT_MODE_HIGH_WATERMARK_STR, strlen(ATCMD_TT_MODE_HIGH_WATERMARK_STR)) == 0)) {
			stop_flag = 1;
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_TT_MODE_HIGH_WATERMARK_STR);
		} else if ((len > 0) && (memcmp(data, ATCMD_TT_MODE_LOW_WATERMARK_STR, strlen(ATCMD_TT_MODE_LOW_WATERMARK_STR)) == 0)) {
			stop_flag = 0;
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_TT_MODE_LOW_WATERMARK_STR);
		}

		if (send_flag == 1) {
			xSemaphoreGive(master_tt_mode_sema);
		}

	} else
#endif
	{
		if (len > 0) {
			if (memcmp(data, ATCMD_DOWNSTREAM_TEST_START_STR, strlen(ATCMD_DOWNSTREAM_TEST_START_STR)) == 0) {
				start_time = xTaskGetTickCount();
				RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_DOWNSTREAM_TEST_START_STR);
			}

			if (start_time != 0) {
				str_find = strstr((char *)data, ATCMD_DOWNSTREAM_TEST_END_STR);
				if (str_find != NULL) {
					RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, ATCMD_DOWNSTREAM_TEST_END_STR);
					end_time = xTaskGetTickCount();
					ds_count = ds_count + ((u8 *)str_find - data);
					uart_format_string_output("downstream test: Receive %lu KBytes in %lu ms, %d Kbits/sec\n\r", ds_count / 1024,
											  (end_time - start_time), (int)((ds_count * 8) / (end_time - start_time)));
					start_time = 0;
					end_time = 0;
					ds_count = 0;
				} else {
					ds_count += len;
				}
			} else {
				data[len] = 0;
				uart_send_string(&sobj, (char *)data, len);
			}
		}
	}
}

void atcmd_spi_host_task(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[M] atcmd_spi_host_task start\n");
	vTaskDelay(1000);

	u32 remain_len = 0;
	u32 max_tx_len = ATCMD_SPI_DMA_SIZE - 8;
	u32 tx_len;
	u16 *p_tx_len;
	u16 rx_len;

	/*init M2S pin*/
	gpio_init(&at_spi_master_to_slave_gpio, MASTER_SYNC_PIN);
	gpio_dir(&at_spi_master_to_slave_gpio, PIN_OUTPUT);
	gpio_mode(&at_spi_master_to_slave_gpio, PullUp);
	gpio_write(&at_spi_master_to_slave_gpio, 1);

	/*init S2M pin*/
	gpio_irq_init(&at_spi_slave_to_master_irq, SLAVE_SYNC_PIN, gpio_edge_irq_handler, (uint32_t)(&current_edge));
	gpio_irq_set(&at_spi_slave_to_master_irq, IRQ_RISE, 1);
	gpio_irq_enable(&at_spi_slave_to_master_irq);

	/*init uart*/
	sobj.uart_idx = 0;
	serial_init(&sobj, HOST_UART_TX, HOST_UART_RX);
	serial_baud(&sobj, HOST_UART_BAUDRATE);
	serial_format(&sobj, 8, ParityNone, 1);

	uart_send_string(&sobj, "UART Send Command Demo...\r\n", strlen("UART Send Command Demo...\r\n"));
	serial_irq_handler(&sobj, uart_irq, (uint32_t)&sobj);
	serial_irq_set(&sobj, RxIrq, 1);
	serial_irq_set(&sobj, TxIrq, 1);

	/* SPI1 is as Master */
	spi_master.spi_idx = MBED_SPI1;
	spi_init(&spi_master, SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CS);
	spi_format(&spi_master, SPI_DATA_FRAME_SIZE, SPI_MODE, 0);
	spi_frequency(&spi_master, SPI_SCLK_FREQ);

	spi_irq_hook(&spi_master, (spi_irq_handler) Master_tr_done_callback, (uint32_t)&spi_master);

	while (1) {
		spi_flush_rx_fifo(&spi_master);
		memset(MasterRxBuf, 0, 4);
		xSemaphoreTake(master_gpio_sema, 0xFFFFFFFF);

		while (spi_busy(&spi_master)) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "spi busy, wait...\n");
			vTaskDelay(10);
		}

		remain_len = RingBuffer_Available(atcmd_host_tx_ring_buf);
		if (remain_len > 0) {
			tx_len = remain_len > max_tx_len ? max_tx_len : remain_len;

			MasterTxBuf[0] = 0x41;
			MasterTxBuf[1] = 0x54;

			p_tx_len = (u16 *)&MasterTxBuf[2];
			*p_tx_len = tx_len;

			RingBuffer_Read(atcmd_host_tx_ring_buf, (u8 *)&MasterTxBuf[4], tx_len);

#if CHECKSUM_EN
			// cal checksum
			u32 tx_checksum = checksum_32_spi(0, (u8 *)MasterTxBuf, tx_len + 4);
			u32 *p_tx_checksum = (u32 *)&MasterTxBuf[tx_len + 4];
			*p_tx_checksum = tx_checksum;
#endif
			spi_master_write_read_stream_dma(&spi_master, (char *)MasterTxBuf, (char *)MasterRxBuf, ATCMD_SPI_DMA_SIZE);
			xSemaphoreTake(master_tx_done_sema, 0xFFFFFFFF);
			xSemaphoreTake(master_rx_done_sema, 0xFFFFFFFF);
			send_flag = 1;
		} else {
			spi_master_read_stream_dma(&spi_master, (char *)MasterRxBuf, ATCMD_SPI_DMA_SIZE);
			xSemaphoreTake(master_rx_done_sema, 0xFFFFFFFF);
			send_flag = 0;
		}

		// check recv data
		if (MasterRxBuf[0] != 0x41 || MasterRxBuf[1] != 0x54) {
			rx_len = 0;
			goto NEXT;
		}

		rx_len = MasterRxBuf[2] | (MasterRxBuf[3] << 8);
		if (rx_len > ATCMD_SPI_DMA_SIZE - 8) {
			uart_send_string(&sobj, "[M] recv_len error\n", strlen("[M] recv_len error\n"));
			rx_len = 0;
			goto NEXT;
		}

#if CHECKSUM_EN
		// check checksum
		u32 rx_checksum = checksum_32_spi(0, (u8 *)MasterRxBuf, rx_len + 4);
		u32 *p_rx_checksum = (u32 *)&MasterRxBuf[rx_len + 4];
		if (rx_checksum != *p_rx_checksum) {
			uart_send_string(&sobj, "[M] recv slave data checksum error\n", strlen("[M] recv slave data checksum error\n"));
			rx_len = 0;
			goto NEXT;
		}
#endif


NEXT:
		if (spi_recv_data_callback != NULL) {
			spi_recv_data_callback((u8 *)&MasterRxBuf[4], rx_len);
		}

		spi_m2s_pin_set(1);
	}
}

void example_atcmd_spi_master(void)
{
	master_tx_done_sema = (void *)xSemaphoreCreateCounting(0xFFFF, 0);
	master_rx_done_sema = (void *)xSemaphoreCreateCounting(0xFFFF, 0);
	master_tt_mode_sema = (void *)xSemaphoreCreateCounting(0xFFFF, 0);
	master_gpio_sema = (void *)xSemaphoreCreateCounting(0xFFFF, 0);

	atcmd_host_tx_ring_buf = RingBuffer_Create(NULL, ((ATCMD_SPI_DMA_SIZE - 8) * 7) + 1, LOCAL_RINGBUFF, 1);

	spi_recv_data_callback = spi_handle_recv_data;

	if (xTaskCreate((void *)atcmd_spi_host_task, ((const char *)"atcmd_spi_host_task"), 1024 / sizeof(portSTACK_TYPE), NULL, 5, NULL) != pdPASS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s rtos_task_create(atcmd_spi_host_task) failed", __FUNCTION__);
	}
}