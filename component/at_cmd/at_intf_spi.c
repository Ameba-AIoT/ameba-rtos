/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "atcmd_service.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "os_wrapper.h"
#include "gpio_api.h"
#include "gpio_irq_api.h"
#include "at_intf_spi.h"
#include "ringbuffer.h"

static const char *const TAG = "AT_SPI-S";

#define DataFrameSize	8
#define Mode			0
#define ATCMD_SPI_DMA_SIZE	2048 //for dma mode, buffer size should be multiple of CACHE_LINE_SIZE 

#if defined (CONFIG_AMEBASMART)
//both EV30EL0 and EV30EA0 support
u8 SPI0_MOSI = _PA_13;
u8 SPI0_MISO = _PA_14;
u8 SPI0_SCLK = _PA_15;
u8 SPI0_CS = _PA_16;

#elif defined (CONFIG_AMEBALITE)
u8 SPI0_MOSI = _PA_29;
u8 SPI0_MISO = _PA_30;
u8 SPI0_SCLK = _PA_28;
u8 SPI0_CS = _PA_31;

#elif defined (CONFIG_AMEBADPLUS)
// SPI0 FID=8
u8 SPI0_MOSI = _PB_24;
u8 SPI0_MISO = _PB_25;
u8 SPI0_SCLK = _PB_23;
u8 SPI0_CS = _PB_26;
#endif

u8 AT_SYNC_FROM_MASTER_GPIO = PA_26;
u8 AT_SYNC_TO_MASTER_GPIO = PA_27;

/* for dma mode, start address of buffer should be CACHE_LINE_SIZE  aligned*/
u8 SlaveTxBuf[ATCMD_SPI_DMA_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
u8 SlaveRxBuf[ATCMD_SPI_DMA_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

spi_t spi_slave;

gpio_t at_spi_slave_to_master_gpio;
gpio_irq_t at_spi_master_to_slave_irq;

rtos_sema_t slave_tx_sema;
rtos_sema_t slave_rx_sema;
rtos_sema_t atcmd_spi_rx_sema;

rtos_queue_t g_spi_cmd_queue;
RingBuffer *at_spi_rx_ring_buf = NULL;
RingBuffer *at_spi_tx_ring_buf = NULL;

extern char g_tt_mode;
extern volatile UART_LOG_CTL shell_ctl;
extern UART_LOG_BUF shell_rxbuf;
extern RingBuffer *atcmd_tt_mode_rx_ring_buf;
extern rtos_sema_t atcmd_tt_mode_sema;

uint32_t checksum_32_spi(uint32_t start_value, uint8_t *data, int len)
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

void at_spi_master_to_slave_irq_handler(uint32_t id, gpio_irq_event event)
{
	(void)id;
	(void)event;
	struct atcmd_spi_req req = {0};
	req.cmd = SPI_SLAVE_RD_CMD;
	rtos_queue_send(g_spi_cmd_queue, (void *)&req, 0);
}



void Slave_tr_done_callback(uint32_t pdata, SpiIrq event)
{
	(void)pdata;

	switch (event) {
	case SpiRxIrq:
		rtos_sema_give(slave_rx_sema);
		break;
	case SpiTxIrq:
		rtos_sema_give(slave_tx_sema);
		break;
	default:
		break;
	}
}

void atcmd_spi_task(void)
{
	/* wait total cpus enter application to avoid log missing */
	rtos_time_delay_ms(1000);
	Pinmux_Swdoff();

	struct atcmd_spi_req req = {0};

	/* SPI0 is as Slave */
	spi_slave.spi_idx = MBED_SPI0;
	spi_init(&spi_slave,  SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
	spi_format(&spi_slave, DataFrameSize, Mode, 1);

	spi_irq_hook(&spi_slave, (spi_irq_handler) Slave_tr_done_callback, (uint32_t)&spi_slave);

	spi_flush_rx_fifo(&spi_slave);

	while (1) {
		rtos_queue_receive(g_spi_cmd_queue, (void *)&req, 0xFFFFFFFF);

		memset(SlaveTxBuf, 0, ATCMD_SPI_DMA_SIZE);
		memset(SlaveRxBuf, 0, ATCMD_SPI_DMA_SIZE);

		if (req.cmd == SPI_SLAVE_RD_CMD) {
			// check ready tx data
			u32 send_len = 0;
			u32 remain_len = RingBuffer_Available(at_spi_tx_ring_buf);
			if (remain_len > 0) {
				send_len = (remain_len > ATCMD_SPI_DMA_SIZE - 8) ? ATCMD_SPI_DMA_SIZE - 8 : remain_len;
				RingBuffer_Read(at_spi_tx_ring_buf, SlaveTxBuf + 4, send_len);

				//add header magic number
				SlaveTxBuf[0] = 0x54;
				SlaveTxBuf[1] = 0x58;

				//add tx data length
				u16 *p_tx_len = (u16 *)&SlaveTxBuf[2];
				*p_tx_len = send_len;

				//add checksum for tx data
				u32 tx_checksum = checksum_32_spi(0, (u8 *)SlaveTxBuf, send_len + 4);
				u32 *p_tx_checksum = (u32 *)&SlaveTxBuf[send_len + 4];
				*p_tx_checksum = tx_checksum;

				spi_slave_write_stream_dma(&spi_slave, (char *)SlaveTxBuf, ATCMD_SPI_DMA_SIZE);
			}

			gpio_write(&at_spi_slave_to_master_gpio, 0);
			spi_slave_read_stream_dma(&spi_slave, (char *)SlaveRxBuf, ATCMD_SPI_DMA_SIZE);

			while (rtos_sema_take(slave_rx_sema, 0) == SUCCESS) {}

			gpio_write(&at_spi_slave_to_master_gpio, 1);
			rtos_sema_take(slave_rx_sema, 0xFFFFFFFF);

			// check rx dataheader
			if (SlaveRxBuf[0] != 0x54 || SlaveRxBuf[1] != 0x58) {
				RTK_LOGE(TAG, "recv master data magic number error\n");
				continue;
			}

			u16 recv_len = SlaveRxBuf[2] | (SlaveRxBuf[3] << 8);
			if (recv_len > ATCMD_SPI_DMA_SIZE - 8) {
				RTK_LOGE(TAG, "recv_len error\n");
				continue;
			}

			// check rx data checksum
			u32 checksum = checksum_32_spi(0, (u8 *)SlaveRxBuf, recv_len + 4);
			u32 *p_rx_checksum = (u32 *)&SlaveRxBuf[recv_len + 4];
			if (checksum != *p_rx_checksum) {
				RTK_LOGE(TAG, "recv master data checksum error\n");
				continue;
			}

			u32 space = 0;
			if (g_tt_mode) {
				space = RingBuffer_Space(atcmd_tt_mode_rx_ring_buf);
				if (space >= recv_len) {
					RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, SlaveRxBuf + 4, recv_len);
					rtos_sema_give(atcmd_tt_mode_sema);
				} else if (space > 0) {
					RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, SlaveRxBuf + 4, space);
					rtos_sema_give(atcmd_tt_mode_sema);
					RTK_LOGW(TAG, "atcmd_tt_mode_rx_ring_buf is full, drop partial data\n");
				} else {
					RTK_LOGW(TAG, "atcmd_tt_mode_rx_ring_buf is full, drop data\n");
				}
			} else {
				space = RingBuffer_Space(at_spi_rx_ring_buf);
				if (space >= recv_len) {
					RingBuffer_Write(at_spi_rx_ring_buf, SlaveRxBuf + 4, recv_len);
					rtos_sema_give(atcmd_spi_rx_sema);
				} else {
					RTK_LOGW(TAG, "at_spi_rx_ring_buf is full, drop data\n");
				}
			}

		} else if (req.cmd == SPI_SLAVE_WR_CMD) {
			u32 send_len = 0;
			u32 remain_len = RingBuffer_Available(at_spi_tx_ring_buf);
			if (remain_len > 0) {
				send_len = (remain_len > ATCMD_SPI_DMA_SIZE - 8) ? ATCMD_SPI_DMA_SIZE - 8 : remain_len;
			} else {
				// no data to send, wait for next request
				continue;
			}

			RingBuffer_Read(at_spi_tx_ring_buf, SlaveTxBuf + 4, send_len);

			//add header magic number
			SlaveTxBuf[0] = 0x54;
			SlaveTxBuf[1] = 0x58;

			//add tx data length
			u16 *p_tx_len = (u16 *)&SlaveTxBuf[2];
			*p_tx_len = send_len;

			//add checksum for tx data
			u32 tx_checksum = checksum_32_spi(0, (u8 *)SlaveTxBuf, send_len + 4);
			u32 *p_tx_checksum = (u32 *)&SlaveTxBuf[send_len + 4];
			*p_tx_checksum = tx_checksum;

			gpio_write(&at_spi_slave_to_master_gpio, 0);
			spi_slave_read_stream_dma(&spi_slave, (char *)SlaveRxBuf, ATCMD_SPI_DMA_SIZE);
			spi_slave_write_stream_dma(&spi_slave, (char *)SlaveTxBuf, ATCMD_SPI_DMA_SIZE);
			gpio_write(&at_spi_slave_to_master_gpio, 1);
			rtos_sema_take(slave_tx_sema, 0xFFFFFFFF);

			remain_len = RingBuffer_Available(at_spi_tx_ring_buf);
			if (remain_len > 0) {
				req.cmd = 0;
				if (rtos_queue_peek(g_spi_cmd_queue, (void *)&req, 0) == SUCCESS) {
					if (req.cmd != SPI_SLAVE_WR_CMD) {
						req.cmd = SPI_SLAVE_WR_CMD;
						if (rtos_queue_send(g_spi_cmd_queue, (void *)&req, 0) != SUCCESS) {
							RTK_LOGE(TAG, "g_spi_cmd_queue is full\n");
						}
					}
				}
			}

			// check rx header
			if (SlaveRxBuf[0] != 0x54 || SlaveRxBuf[1] != 0x58) {
				continue;
			}

			// check rx data length
			u16 recv_len = SlaveRxBuf[2] | (SlaveRxBuf[3] << 8);
			if (recv_len > ATCMD_SPI_DMA_SIZE - 8) {
				RTK_LOGE(TAG, "recv_len error\n");
				continue;
			}

			// check rx checksum
			u32 checksum = checksum_32_spi(0, (u8 *)SlaveRxBuf, recv_len + 4);
			u32 *p_rx_checksum = (u32 *)&SlaveRxBuf[recv_len + 4];
			if (checksum != *p_rx_checksum) {
				RTK_LOGE(TAG, "recv master data checksum error\n");
				continue;
			}

			u32 space = 0;
			if (g_tt_mode) {
				space = RingBuffer_Space(atcmd_tt_mode_rx_ring_buf);
				if (space >= recv_len) {
					RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, SlaveRxBuf + 4, recv_len);
					rtos_sema_give(atcmd_tt_mode_sema);
				} else if (space > 0) {
					RingBuffer_Write(atcmd_tt_mode_rx_ring_buf, SlaveRxBuf + 4, space);
					rtos_sema_give(atcmd_tt_mode_sema);
					RTK_LOGW(TAG, "atcmd_tt_mode_rx_ring_buf is full, drop partial data\n");
				} else {
					RTK_LOGW(TAG, "atcmd_tt_mode_rx_ring_buf is full, drop data\n");
				}
			} else {
				space = RingBuffer_Space(at_spi_rx_ring_buf);
				if (space >= recv_len) {
					RingBuffer_Write(at_spi_rx_ring_buf, SlaveRxBuf + 4, recv_len);
					rtos_sema_give(atcmd_spi_rx_sema);
				} else {
					RTK_LOGW(TAG, "at_spi_rx_ring_buf is full, drop data\n");
				}
			}
		}
	}

	spi_free(&spi_slave);

	rtos_task_delete(NULL);
}

void atcmd_spi_input_handler_task(void)
{
	PUART_LOG_BUF pShellRxBuf = &shell_rxbuf;
	u32 i = 0, actual_len = 0;
	while (1) {
		pShellRxBuf->BufCount = 0;
		i = 0;

		rtos_sema_take(atcmd_spi_rx_sema, 0xFFFFFFFF);

		actual_len = RingBuffer_Available(at_spi_rx_ring_buf);
		RingBuffer_Read(at_spi_rx_ring_buf, pShellRxBuf->UARTLogBuf, actual_len);

		pShellRxBuf->BufCount = actual_len;

recv_again:
		if (shell_cmd_chk(pShellRxBuf->UARTLogBuf[i++], (UART_LOG_CTL *)&shell_ctl, ENABLE) == 2) {
			if (shell_ctl.pTmpLogBuf != NULL) {
				shell_ctl.ExecuteCmd = TRUE;

				if (shell_ctl.shell_task_rdy) {
					shell_ctl.GiveSema();
				}
			} else {
				shell_array_init((u8 *)shell_ctl.pTmpLogBuf->UARTLogBuf, UART_LOG_CMD_BUFLEN, '\0');
			}
		}

		/* recv all data one time */
		if ((pShellRxBuf->BufCount != i) && (pShellRxBuf->BufCount != 0)) {
			goto recv_again;
		}
	}
}

void atio_spi_output(char *buf, int len)
{
	int space = 0;
	int send_len = len;
	struct atcmd_spi_req req = {0};
	req.cmd = SPI_SLAVE_WR_CMD;

	while (1) {
		space = RingBuffer_Space(at_spi_tx_ring_buf);
		if (space >= send_len) {
			RingBuffer_Write(at_spi_tx_ring_buf, (u8 *)buf, send_len);
			rtos_queue_send(g_spi_cmd_queue, (void *)&req, 0xFFFFFFFF);
			break;
		} else if (space > 0) {
			RingBuffer_Write(at_spi_tx_ring_buf, (u8 *)buf, space);
			rtos_queue_send(g_spi_cmd_queue, (void *)&req, 0xFFFFFFFF);
			send_len -= space;
		}

		rtos_time_delay_ms(10);
	}
}


int atio_spi_init(void)
{
	memset(SlaveTxBuf, 0, ATCMD_SPI_DMA_SIZE);
	memset(SlaveRxBuf, 0, ATCMD_SPI_DMA_SIZE);

	rtos_sema_create(&slave_rx_sema, 0, 0xFFFF);
	rtos_sema_create(&slave_tx_sema, 0, 0xFFFF);
	rtos_sema_create(&atcmd_spi_rx_sema, 0, 0xFFFF);

	rtos_queue_create(&g_spi_cmd_queue, 16, sizeof(struct atcmd_spi_req));
	at_spi_rx_ring_buf = RingBuffer_Create(NULL, 4 * 1024, 1);
	at_spi_tx_ring_buf = RingBuffer_Create(NULL, 4 * 1024, 1);

	gpio_irq_init(&at_spi_master_to_slave_irq, AT_SYNC_FROM_MASTER_GPIO, at_spi_master_to_slave_irq_handler, (uint32_t)(&at_spi_master_to_slave_irq));
	gpio_irq_set(&at_spi_master_to_slave_irq, IRQ_FALL, 1);   // falling Edge Trigger
	gpio_irq_enable(&at_spi_master_to_slave_irq);

	gpio_init(&at_spi_slave_to_master_gpio, AT_SYNC_TO_MASTER_GPIO);
	gpio_dir(&at_spi_slave_to_master_gpio, PIN_OUTPUT);
	gpio_mode(&at_spi_slave_to_master_gpio, PullUp);

	out_buffer = atio_spi_output;

	if (rtos_task_create(NULL, ((const char *)"atcmd_spi_task"), (rtos_task_t)atcmd_spi_task, NULL, 1024, 2) != SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s rtos_task_create(atcmd_spi_task) failed", __FUNCTION__);
	}

	if (rtos_task_create(NULL, ((const char *)"atcmd_spi_input_handler_task"), (rtos_task_t)atcmd_spi_input_handler_task, NULL, 1024, 1) != SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s rtos_task_create(atcmd_spi_input_handler_task) failed", __FUNCTION__);
	}

	return 0;
}

