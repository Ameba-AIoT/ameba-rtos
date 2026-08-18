#include <stdio.h>
#include <string.h>
#include "sdio_api_ext.h"
#include "usart.h"

extern unsigned char RxBufferBackup[256];
extern uint32_t Uart1_Rx_Cnt_Backup;

u32 MAX_TX_BUF_SIZE = 0;
volatile u8 tt_mode_task_start = 0;

/* ---------- SPDIO host->device TXBD ring tracking ----------
 * SDIO_REG_FREE_TXBD_NUM (0x20) is deliberately not used: on green2 it can
 * read 0 while BDs are actually free, and no tx-bd-available interrupt follows
 * (JIRA PRINTER-2628). Derive the free count from the device RPTR plus a
 * host-maintained WPTR instead, the same way the Linux host driver does.
 * SPDIO_REG_TXBD_NUM/WPTR/RPTR are SPDIO control registers: CMD52 access only,
 * so they must be read one byte at a time (sdio_read_common with cnt==1 uses
 * CMD52; cnt>1 would fall through to CMD53, which these registers reject). */
static u16 g_txbd_size;   /* device TXBD ring size, read once at init */
static u16 g_txbd_wptr;   /* software WPTR, advanced on every send */
static u16 g_txbd_free;   /* cached free count, spent one per send, refilled by query */

#define NULL 0
#define ATCMD_DOWNSTREAM_TEST_START_STR 	"Downstream Test Start\r\n"
#define ATCMD_DOWNSTREAM_TEST_END_STR 		"Downstream Test End\r\n"

rtw_semaphore_type_t host_irq_sema;
rtw_semaphore_type_t host_recv_sema;
rtw_semaphore_type_t uart_recv_sema;

rtw_result_t sd_init(void)
{
	uint8_t byte_data;
	rtw_result_t result;
	uint32_t loop_count;

	// CMD 0/5/3/7
	RTW_VERIFY_RESULT(stm32_sdio_enumerate());

	/* Setup the backplane*/
	loop_count = 0;
	do {
		/* Enable function 1 */
		RTW_VERIFY_RESULT(rtw_write_register_value(BASIC_FUNC, SDIOD_CCCR_IOEN, (uint8_t)1, SDIO_FUNC_ENABLE_1));

		if (loop_count != 0) {
			(void)rtw_rtos_delay_milliseconds((uint32_t)1); /* Ignore return - nothing can be done if it fails */
		}
		RTW_VERIFY_RESULT(rtw_read_register_value(BASIC_FUNC, SDIOD_CCCR_IOEN, (uint8_t)1, &byte_data));

		loop_count++;
		if (loop_count >= (uint32_t)500) {
			return RTW_TIMEOUT;
		}
	} while (byte_data != (uint8_t)SDIO_FUNC_ENABLE_1);
	printf("%s:%d enable fun1 ok\n", __FUNCTION__, __LINE__);

#ifndef SDIO_1_BIT
	/* Read the bus width and set to 4 bits */
	RTW_VERIFY_RESULT(rtw_read_register_value(BASIC_FUNC, SDIOD_CCCR_BICTRL, (uint8_t)1, &byte_data));
	RTW_VERIFY_RESULT(rtw_write_register_value(BASIC_FUNC, SDIOD_CCCR_BICTRL, (uint8_t)1, (byte_data & (~BUS_SD_DATA_WIDTH_MASK)) | BUS_SD_DATA_WIDTH_4BIT));
	printf("%s:%d set bus width to 4\n", __FUNCTION__, __LINE__);
#endif
	/* NOTE: We don't need to change our local bus settings since we're not sending any data (only using CMD52) until after we change the bus speed further down */

	RTW_VERIFY_RESULT(rtw_write_register_value(BASIC_FUNC, SDIOD_CCCR_BLKSIZE_0, (uint8_t) 1, (uint32_t) SDIO_64B_BLOCK));
	RTW_VERIFY_RESULT(rtw_write_register_value(BASIC_FUNC, SDIOD_CCCR_F1BLKSIZE_0, (uint8_t) 1, (uint32_t)(USER_BLK_SZ & 0xff)));
	RTW_VERIFY_RESULT(rtw_write_register_value(BASIC_FUNC, SDIOD_CCCR_F1BLKSIZE_1, (uint8_t) 1, (uint32_t)((USER_BLK_SZ >> 8) & 0xff)));
	printf("%s:%d set blksz fun0/fun1 64B \n", __FUNCTION__, __LINE__);

	/* Enable Client interrupts */
	RTW_VERIFY_RESULT(rtw_write_register_value(BASIC_FUNC, SDIOD_CCCR_INTEN, (uint8_t)1, INTR_CTL_MASTER_EN | INTR_CTL_FUNC1_EN));
	printf("%s:%d enable fun1\n", __FUNCTION__, __LINE__);

	/* This code is required if we want more than 25 MHz clock */

	/* kw: set EHS if SHS is 1 */

	RTW_VERIFY_RESULT(rtw_read_register_value(BASIC_FUNC, SDIOD_CCCR_SPEED_CONTROL, 1, &byte_data));
	if ((byte_data & 0x1) != 0) {
		RTW_VERIFY_RESULT(rtw_write_register_value(BASIC_FUNC, SDIOD_CCCR_SPEED_CONTROL, 1, byte_data | SDIO_SPEED_EHS));
	} else {
		return RTW_BUS_READ_REGISTER_ERROR;
	}
	printf("%s:%d enable high speed\n", __FUNCTION__, __LINE__);

	/* Switch to high speed mode and change to 4 bit mode */
	RTW_VERIFY_RESULT(stm32_enable_high_speed_sdio());
	printf("%s:%d Switch to high speed mode\n", __FUNCTION__, __LINE__);

	/* Wait till the backplane is ready */
	loop_count = 0;
	while (((result = rtw_read_register_value(BASIC_FUNC, SDIOD_CCCR_IORDY, (uint8_t)1, &byte_data)) == RTW_SUCCESS) &&
		   ((byte_data & SDIO_FUNC_READY_1) == 0) && (loop_count < (uint32_t)500)) {
		(void)rtw_rtos_delay_milliseconds((uint32_t)1); /* Ignore return - nothing can be done if it fails */
		loop_count++;
	}

	if (loop_count >= (uint32_t)500) {
		printf("Timeout while waiting for func1 to be ready\n");
		return RTW_TIMEOUT;
	}
	RTW_VERIFY_RESULT(result);
	printf("%s:%d check fun1 iordy is 1\n", __FUNCTION__, __LINE__);

	return result;
}

rtw_result_t device_is_ready(void)
{
	return (rtw_result_t)stm32_bus_init();
}

rtw_result_t host_init(void)
{
	uint8_t TxUnitCnt = 0;
	uint8_t num_lo = 0, num_hi = 0, wptr = 0;

	rtw_result_t result = rtw_rtos_init_semaphore(&host_irq_sema);
	if (result != RTW_SUCCESS) {
		return result;
	}

	result = rtw_rtos_init_semaphore(&host_recv_sema);
	if (result != RTW_SUCCESS) {
		return result;
	}

	result = rtw_rtos_init_semaphore(&uart_recv_sema);
	if (result != RTW_SUCCESS) {
		return result;
	}

	result = device_is_ready();
	if (result != RTW_SUCCESS) {
		printf(("device_is_ready fail\n"));
		return result;
	}

	result = sd_init();
	if (result != RTW_SUCCESS) {
		printf(("sd_init fail\n"));
		return result;
	}

	sdio_read_common(SPDIO_REG_TXBUF_UNIT_SZ, 1, &TxUnitCnt);

	if (!TxUnitCnt) {
		printf("read txbuf unit fail\r\n");
		return -1;
	} else {
		printf("TxUnitCnt : %d\r\n", TxUnitCnt);
		MAX_TX_BUF_SIZE = TxUnitCnt * 64 - 32;
	}

	/* TXBD ring size is configured by the device and never changes afterwards,
	 * so read it only once here; WPTR is synced with hw only at this point and
	 * then maintained in software. Two 1-byte CMD52 reads for the 16-bit NUM. */
	sdio_read_common(SPDIO_REG_TXBD_NUM, 1, &num_lo);
	sdio_read_common(SPDIO_REG_TXBD_NUM + 1, 1, &num_hi);
	sdio_read_common(SPDIO_REG_TXBD_WPTR, 1, &wptr);
	g_txbd_size = ((u16)num_hi << 8) | num_lo;
	if (!g_txbd_size) {
		printf("read txbd num fail\r\n");
		return -1;
	}
	g_txbd_wptr = wptr;
	printf("TXBD num : %d, wptr : %d\r\n", g_txbd_size, g_txbd_wptr);

	return RTW_SUCCESS;
}

/*
 * Refill g_txbd_free from the device RPTR and the software WPTR.
 * The hw WPTR is only sampled at init: the device updates it on the AHB ack,
 * which can lag behind the packets already pushed into the TX FIFO, so tracking
 * it in software gives a conservative, always-valid value.
 */
static void sdio_query_free_txbd(void)
{
	u8 rptr = 0;

	if (sdio_read_common(SPDIO_REG_TXBD_RPTR, 1, &rptr) != 0) {
		return; /* keep the previous value on a read error */
	}

	if (g_txbd_wptr >= rptr) {
		g_txbd_free = (u16)(g_txbd_size + rptr - g_txbd_wptr - 1);
	} else {
		g_txbd_free = (u16)(rptr - g_txbd_wptr - 1);
	}
}

/* One BD per packet: advance the software WPTR and spend one cached free BD.
 * Called after every sdio_host_send_to_dev() so both the UART and TT paths
 * account for the BD they consume. */
static void sdio_txbd_consume_one(void)
{
	g_txbd_wptr = (u16)((g_txbd_wptr + 1) % g_txbd_size);
	if (g_txbd_free > 0) {
		g_txbd_free--;
	}
}

static void uart_irq_handle_task(void *arg)
{
	while (1) {
		rtw_rtos_get_semaphore(&uart_recv_sema, (uint32_t) 0xFFFFFFFF, RTW_TRUE);
		RxBufferBackup[Uart1_Rx_Cnt_Backup] = '\0';
		sdio_host_send_to_dev(RxBufferBackup, Uart1_Rx_Cnt_Backup);
		sdio_txbd_consume_one();
	}

	vTaskDelete(NULL);
}

static void host_irq_handler_task(void *arg)
{
	u8 sdio_hisr;
	while (1) {
		rtw_rtos_get_semaphore(&host_irq_sema, (uint32_t) 0xFFFFFFFF, RTW_TRUE);

		sdio_read_common(SDIO_REG_HISR, 1, &sdio_hisr);

		if (sdio_hisr & SDIO_HISR_RX_REQUEST) {
			rtw_rtos_set_semaphore(&host_recv_sema, RTW_FALSE);
		}
	}

	vTaskDelete(NULL);
}

static void host_tt_mode_task(void *arg)
{
	u32 tt_len = 100 * 1024 * 1024;
	u32 send_len = 0;
	u32 max_send_len = MAX_TX_BUF_SIZE;
	u8 *tt_tx_buf = pvPortMalloc(max_send_len);

	printf("host_tt_mode_task start \r\n");

	while (tt_len > 0) {

		/* Spend the cached free count first; only re-query the device (RPTR)
		 * once it runs out, so a burst of sends needs no CMD52 per packet. */
		while (g_txbd_free == 0) {
			sdio_query_free_txbd();
			if (g_txbd_free == 0) {
				vTaskDelay(1);
			}
		}

		send_len = tt_len > max_send_len ? max_send_len : tt_len;
		sdio_host_send_to_dev(tt_tx_buf, send_len);
		sdio_txbd_consume_one();
		tt_len -= send_len;
	}

	vPortFree(tt_tx_buf);
	tt_mode_task_start = 0;
	vTaskDelete(NULL);
}

void host_show_recv_data(uint8_t *data)
{
	struct INIC_RX_DESC *rdata = (struct INIC_RX_DESC *)data;
	u16 len = rdata->pkt_len;
	static u32 start_time = 0, end_time = 0, ds_count = 0;
	char *str_find = NULL;
	char *payload = (char *)data + sizeof(struct INIC_RX_DESC);

	if ((len == 5) && (memcmp(payload, ">>>", 3) == 0)) {
		xTaskCreate((TaskFunction_t)host_tt_mode_task,
					(const char *)"host_tt_mode_task",
					(uint16_t)2048 / sizeof(portSTACK_TYPE),
					(void *)NULL,
					(UBaseType_t)1,
					(TaskHandle_t *)NULL);
		tt_mode_task_start = 1;
	}

	if (tt_mode_task_start) {
		Usart_SendString(DEBUG_USART, payload, len);
	} else {
		if (len > 0) {
			if (memcmp(payload, ATCMD_DOWNSTREAM_TEST_START_STR, strlen(ATCMD_DOWNSTREAM_TEST_START_STR)) == 0) {
				start_time = xTaskGetTickCount();
				printf(ATCMD_DOWNSTREAM_TEST_START_STR);
				printf("start_time : %d\r\n", start_time);
			}

			if (start_time != 0) {
				str_find = strstr((char *)payload, ATCMD_DOWNSTREAM_TEST_END_STR);
				if (str_find != NULL) {
					printf(ATCMD_DOWNSTREAM_TEST_END_STR);
					end_time = xTaskGetTickCount();
					ds_count = ds_count + ((char *)str_find - payload);
					printf("downstream test: Receive %u KBytes in %u ms, %d Kbits/sec\n\r", ds_count / 1024,
						   (end_time - start_time), (int)((ds_count * 8) / (end_time - start_time)));
					start_time = 0;
					end_time = 0;
					ds_count = 0;

					if (strstr((char *)payload, "OK")) {
						printf("\r\nOK\r\n");
					}
				} else {
					ds_count += len;
				}
			} else {
				//if (len < 250) {
				Usart_SendString(DEBUG_USART, payload, len);
				//}
			}
		}
	}

	vPortFree(data);
}

static void host_recv_data_process_task(void *arg)
{
	uint8_t tmp[4];
	uint8_t *pbuf;
	uint8_t rx_len_rdy;
	uint32_t himr;
	uint16_t SdioRxFIFOSize;
	uint8_t retry = 0;

	while (1) {
		rtw_rtos_get_semaphore(&host_recv_sema, (uint32_t) 0xFFFFFFFF, RTW_FALSE);

		himr = 0;
		sdio_local_write(SDIO_REG_HIMR, 4, (uint8_t *)&himr);

		do {
			/* validate RX_LEN_RDY before reading RX0_REQ_LEN */
			sdio_read_common(SDIO_REG_RX0_REQ_LEN + 3, 1, &rx_len_rdy);
			rx_len_rdy &= BIT(7);

			if (rx_len_rdy) {
				sdio_read_common(SDIO_REG_RX0_REQ_LEN, 4, tmp);
				SdioRxFIFOSize = (*(u16 *)tmp);
				//printf("host read %d bytes \r\n", SdioRxFIFOSize);

				if (SdioRxFIFOSize == 0) {
					if (retry ++ < 3) {
						continue;
					} else {
						break;
					}
				} else {
					retry = 0;
					pbuf = sdio_read_rxfifo(SdioRxFIFOSize);
					if (pbuf) {
						host_show_recv_data(pbuf);
					}
				}
			} else {
				break;
			}
		} while (1);

		himr = SDIO_HIMR_RX_REQUEST_MSK;
		sdio_local_write(SDIO_REG_HIMR, 4, (u8 *)&himr);
	}

	vTaskDelete(NULL);
}

static void BSP_Init(void)
{
	stm32_init_infrastructure();

	Debug_USART_Config();
}

int main(void)
{
	BSP_Init();

	if (host_init() != RTW_SUCCESS) {
		printf(("sd_init fail\n"));
	}

	BaseType_t xReturn1 = pdPASS, xReturn2 = pdPASS, xReturn3 = pdPASS;
	xReturn1 = xTaskCreate((TaskFunction_t)uart_irq_handle_task,
						   (const char *)"uart_irq_handle_task",
						   (uint16_t)2048 / sizeof(portSTACK_TYPE),
						   (void *)NULL,
						   (UBaseType_t)4,
						   (TaskHandle_t *)NULL);

	xReturn2 = xTaskCreate((TaskFunction_t)host_irq_handler_task,
						   (const char *)"host_irq_handler_task",
						   (uint16_t)1024 / sizeof(portSTACK_TYPE),
						   (void *)NULL,
						   (UBaseType_t)4,
						   (TaskHandle_t *)NULL);

	xReturn3 = xTaskCreate((TaskFunction_t)host_recv_data_process_task,
						   (const char *)"host_recv_data_process_task",
						   (uint16_t)2048 / sizeof(portSTACK_TYPE),
						   (void *)NULL,
						   (UBaseType_t)4,
						   (TaskHandle_t *)NULL);

	if (pdPASS == xReturn1 && pdPASS == xReturn2 && pdPASS == xReturn3) {
		vTaskStartScheduler();
	} else {
		printf("creat task error\r\n");
		return -1;
	}

	printf(("Main() function returned - error"));
	return 0;
}
