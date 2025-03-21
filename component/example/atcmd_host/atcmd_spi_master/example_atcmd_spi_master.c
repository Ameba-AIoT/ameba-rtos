#include "os_wrapper.h"
#include "serial_api.h"
#include "serial_ex_api.h"
#include "gpio_api.h"
#include "gpio_irq_api.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "example_atcmd_spi_master.h"

#define ATCMD_SPI_DMA_SIZE	2048
#define SPI_SCLK_FREQ		20*1000*1000
#define SPI_DATA_FRAME_SIZE	8
#define SPI_MODE			0

#define CHECKSUM_EN 0

rtos_sema_t master_tx_sema;
rtos_sema_t master_rx_sema;
rtos_sema_t master_gpio_sema;

gpio_t at_spi_master_to_slave_gpio;
gpio_irq_t at_spi_slave_to_master_irq;

spi_t spi_master;

int current_edge = IRQ_FALL;
u32 send_count = 0;
char uart_irq_buffer[128] = {0};
u32 uart_irq_count = 0;

u8 MasterRxBuf[ATCMD_SPI_DMA_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 MasterTxBuf[ATCMD_SPI_DMA_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

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

void uart_send_string(serial_t *sobj, char *pstr, u16 len)
{
	unsigned int i = 0;
	while (*(pstr + i) != 0 && i < len) {
		serial_putc(sobj, *(pstr + i));
		i++;
	}
}


void uart_irq(uint32_t id, SerialIrq event)
{
	serial_t *sobj = (void *)id;

	if (event == RxIrq) {
		while (serial_readable(sobj)) {
			uart_irq_buffer[uart_irq_count++] = serial_getc(sobj);
			serial_putc(sobj, uart_irq_buffer[uart_irq_count - 1]);
		}
		if (uart_irq_buffer[uart_irq_count - 1] == '\n') {
			send_count++;
			gpio_write(&at_spi_master_to_slave_gpio, 0);
		}
	}
}

void Master_tr_done_callback(uint32_t pdata, SpiIrq event)
{
	(void)pdata;

	switch (event) {
	case SpiRxIrq:
		rtos_sema_give(master_rx_sema);
		break;
	case SpiTxIrq:
		rtos_sema_give(master_tx_sema);
		break;
	default:
		break;
	}
}

void gpio_edge_irq_handler(uint32_t id, gpio_irq_event event)
{
	(void)event;
	(void)id;
	// master recv slave signal, prepare for reading
	rtos_sema_give(master_gpio_sema);
}

void atcmd_spi_master_demo_task(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "[M] atcmd_spi_master_demo_task start\n");
	rtos_time_delay_ms(1000);
	serial_t sobj;
	sobj.uart_idx = 0;

	serial_init(&sobj, HOST_UART_TX, HOST_UART_RX);
	serial_baud(&sobj, HOST_UART_BAUDRATE);
	serial_format(&sobj, 8, ParityNone, 1);

	uart_send_string(&sobj, "UART Send Command Demo...\r\n", strlen("UART Send Command Demo...\r\n"));
	serial_irq_handler(&sobj, uart_irq, (uint32_t)&sobj);
	serial_irq_set(&sobj, RxIrq, 1);
	serial_irq_set(&sobj, TxIrq, 1);

	gpio_irq_init(&at_spi_slave_to_master_irq, SLAVE_SYNC_PIN, gpio_edge_irq_handler, (uint32_t)(&current_edge));
	gpio_irq_set(&at_spi_slave_to_master_irq, IRQ_RISE, 1);
	gpio_irq_enable(&at_spi_slave_to_master_irq);

	gpio_init(&at_spi_master_to_slave_gpio, MASTER_SYNC_PIN);
	gpio_dir(&at_spi_master_to_slave_gpio, PIN_OUTPUT);
	gpio_mode(&at_spi_master_to_slave_gpio, PullUp);

	/* SPI1 is as Master */
	spi_master.spi_idx = MBED_SPI1;
	spi_init(&spi_master, SPI1_MOSI, SPI1_MISO, SPI1_SCLK, SPI1_CS);
	spi_format(&spi_master, SPI_DATA_FRAME_SIZE, SPI_MODE, 0);
	spi_frequency(&spi_master, SPI_SCLK_FREQ);

	spi_irq_hook(&spi_master, (spi_irq_handler) Master_tr_done_callback, (uint32_t)&spi_master);

	gpio_write(&at_spi_master_to_slave_gpio, 1);

	while (1) {
		memset(MasterRxBuf, 0, ATCMD_SPI_DMA_SIZE);
		memset(MasterTxBuf, 0, ATCMD_SPI_DMA_SIZE);

		rtos_sema_take(master_gpio_sema, 0xFFFFFFFF);

		while (spi_busy(&spi_master)) {
			RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "spi busy, wait...\n");
			rtos_time_delay_ms(10);
		}

		//prepare tx data
		if (uart_irq_count > 0) {
			MasterTxBuf[0] = 0x41;
			MasterTxBuf[1] = 0x54;

			//add data length
			u32 send_len = uart_irq_count;
			u16 *p_tx_len = (u16 *)&MasterTxBuf[2];
			*p_tx_len = send_len;

			// uart test add
			uart_irq_count = 0;

			memcpy(&MasterTxBuf[4], uart_irq_buffer, send_len);

#if CHECKSUM_EN
			// cal checksum
			u32 tx_checksum = checksum_32_spi(0, (u8 *)MasterTxBuf, send_len + 4);
			u32 *p_tx_checksum = (u32 *)&MasterTxBuf[send_len + 4];
			*p_tx_checksum = tx_checksum;
#endif
		}

		spi_flush_rx_fifo(&spi_master);

		// send tx data
		spi_master_write_read_stream_dma(&spi_master, (char *)MasterTxBuf, (char *)MasterRxBuf, ATCMD_SPI_DMA_SIZE);
		rtos_sema_take(master_tx_sema, 0xFFFFFFFF);

		// check recv data
		if (MasterRxBuf[0] != 0x41 || MasterRxBuf[1] != 0x54) {
			goto NEXT;
		}

		u16 rx_len = MasterRxBuf[2] | (MasterRxBuf[3] << 8);
		if (rx_len > ATCMD_SPI_DMA_SIZE - 8) {
			uart_send_string(&sobj, "[M] recv_len error\n", strlen("[M] recv_len error\n"));
			goto NEXT;
		}
#if CHECKSUM_EN
		// check checksum
		u32 rx_checksum = checksum_32_spi(0, (u8 *)MasterRxBuf, rx_len + 4);
		u32 *p_rx_checksum = (u32 *)&MasterRxBuf[rx_len + 4];
		if (rx_checksum != *p_rx_checksum) {
			uart_send_string(&sobj, "[M] recv slave data checksum error\n", strlen("[M] recv slave data checksum error\n"));
			goto NEXT;
		}
#endif

		// show recv data
		MasterRxBuf[rx_len + 4] = 0;
		uart_send_string(&sobj, (char *)&MasterRxBuf[4], rx_len);

NEXT:
		gpio_write(&at_spi_master_to_slave_gpio, 1);
	}
}

void example_atcmd_spi_master(void)
{
	rtos_sema_create(&master_tx_sema, 0, 0xFFFF);
	rtos_sema_create(&master_rx_sema, 0, 0xFFFF);
	rtos_sema_create(&master_gpio_sema, 0, 0xFFFF);

	if (rtos_task_create(NULL, ((const char *)"atcmd_spi_master_demo_task"), (rtos_task_t)atcmd_spi_master_demo_task, NULL, 1024, 5) != SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\n\r%s rtos_task_create(atcmd_spi_master_demo_task) failed", __FUNCTION__);
	}
}