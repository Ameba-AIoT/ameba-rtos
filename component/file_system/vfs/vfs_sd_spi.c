#include "os_wrapper.h"

#if defined(CONFIG_FATFS_SD_SPI_MODE) && CONFIG_FATFS_SD_SPI_MODE
#include "gpio_api.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "vfs_sd_spi.h"

static const char *TAG = "SD-SPI";

#define SCLK_FREQ		50000000 //50M. Baudrate to be set should be less than or equal to half of the SPI IpClk(100M).

#define SPI0_CS		_PA_31
#define SPI0_SCK		_PA_28
#define SPI0_MOSI		_PA_29
#define SPI0_MISO		_PA_30
#define SDCARD_CS		_PA_16

static char spi_tx_buf[SD_SPI_BLOCK_SIZE];
static char spi_rx_buf[SD_SPI_BLOCK_SIZE];

static rtos_sema_t xTxSemaphore = NULL;
static rtos_sema_t xRxSemaphore = NULL;
rtos_mutex_t device_lock = NULL;
static spi_t spi_master;
static gpio_t gpio_src;

SD_SPI_RESULT sd_status = SD_SPI_NODISK;

static void sd_spi_wait_idle(void)
{
	int count = 0;
	while (spi_busy(&spi_master)) {
		rtos_time_delay_ms(10);
		count++;
		if (count % 100 == 0) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "spi busy, wait...(%d ms)\n", count * 10);
		}
	}

}

static void sd_spi_tr_done_callback(uint32_t id, SpiIrq event)
{
	(void)id;

	switch (event) {
	case SpiRxIrq:
		rtos_sema_give(xRxSemaphore);
		break;
	case SpiTxIrq:
		rtos_sema_give(xTxSemaphore);
		break;
	default:
		RTK_LOGE(TAG, "unknown interrput evnent! \r\n");
	}
}

/* slave select */
static void SELECT(void)
{
	gpio_write(&gpio_src, 0);
}

/* slave deselect */
static void DESELECT(void)
{
	gpio_write(&gpio_src, 1);
}

static uint32_t sd_spi_write_buffer(uint8_t *buf, uint16_t len)
{
	if (buf == NULL) {
		return 1;
	}

	sd_spi_wait_idle();
	rtos_mutex_take(device_lock, RTOS_SEMA_MAX_COUNT);

	spi_flush_rx_fifo(&spi_master);

	_memcpy(spi_tx_buf, buf, len);
	_memset(spi_rx_buf, 0xFF, len);
	spi_master_write_read_stream(&spi_master, (char *)spi_tx_buf, (char *)spi_rx_buf, len);

	rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
	rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);

	rtos_mutex_give(device_lock);

	return 0;
}

static uint32_t sd_spi_read_buffer(uint8_t *buf, uint16_t len)
{
	if (buf == NULL) {
		return 1;
	}

	sd_spi_wait_idle();
	rtos_mutex_take(device_lock, RTOS_SEMA_MAX_COUNT);

	spi_flush_rx_fifo(&spi_master);

	_memset(spi_tx_buf, 0xFF, len);
	_memset(spi_rx_buf, 0x0, len);
	spi_master_write_read_stream(&spi_master, (char *)spi_tx_buf, (char *)spi_rx_buf, len);
	rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
	rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);

	_memcpy(buf, spi_rx_buf, len);

	rtos_mutex_give(device_lock);

	return 0;
}

static uint8_t sd_spi_read_write_byte(uint8_t DataTx)
{
	uint8_t txdata, rxdata = 0;
	txdata = DataTx;

	sd_spi_wait_idle();
	rtos_mutex_take(device_lock, RTOS_SEMA_MAX_COUNT);

	spi_flush_rx_fifo(&spi_master);

	spi_master_write_read_stream(&spi_master, (char *)&txdata, (char *)&rxdata, 1);
	// printf("%s t %x r %x\r\n", __func__, txdata, rxdata);
	rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
	rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);

	rtos_mutex_give(device_lock);

	return rxdata;
}

void sd_spi_spi_init(void)
{
	gpio_init(&gpio_src, SDCARD_CS);
	gpio_dir(&gpio_src, PIN_OUTPUT);
	gpio_mode(&gpio_src, PullNone);

	spi_master.spi_idx = MBED_SPI0;
	RTK_LOGI(TAG, "%s, spi idx: %X, freq: %d Hz\n", __func__, spi_master.spi_idx & 0x01, SCLK_FREQ);
	spi_init(&spi_master, SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_CS);
	spi_format(&spi_master, 8, 0, 0);
	spi_frequency(&spi_master, SCLK_FREQ);
	spi_irq_hook(&spi_master, (spi_irq_handler)sd_spi_tr_done_callback, (uint32_t)&spi_master);

	rtos_sema_create_binary(&xRxSemaphore);
	rtos_sema_create_binary(&xTxSemaphore);
	if (xRxSemaphore == NULL || xTxSemaphore == NULL) {
		RTK_LOGE(TAG, "Create Semaphore failed.\n");
	}
	rtos_mutex_create(&device_lock);
	if (device_lock == NULL) {
		RTK_LOGE(TAG, "Create device_lock failed.\n");
	}

	spi_flush_rx_fifo(&spi_master);
}

uint32_t sd_spi_recv_data(uint8_t *buf, uint16_t len)
{
	uint32_t start_time, end_time;

	/* wait for token */
	start_time = rtos_time_get_current_system_time_ms();
	while (sd_spi_read_write_byte(0xFF) != 0xFE) {
		end_time = rtos_time_get_current_system_time_ms();
		if (end_time - start_time > 200) {
			return 1;
		}
	}

	/* receive data */
	if (sd_spi_read_buffer(buf, len) != 0) {
		return 1;
	}

	/* discard CRC */
	sd_spi_read_write_byte(0xFF);
	sd_spi_read_write_byte(0xFF);
	return 0;
}

uint32_t sd_spi_send_data(uint8_t *buf, uint8_t cmd)
{
	uint32_t start_time, end_time;
	uint8_t response;
	uint8_t retry;

	/* wait SD ready */
	start_time = rtos_time_get_current_system_time_ms();
	while (sd_spi_read_write_byte(0xFF) != 0xFF) {
		end_time = rtos_time_get_current_system_time_ms();
		if (end_time - start_time > 500) {
			return 1;
		}
	}

	/* transmit token */
	sd_spi_read_write_byte(cmd);

	/* if it's not STOP token, transmit data */
	if (cmd != 0xFD) {
		if (sd_spi_write_buffer(buf, SD_SPI_BLOCK_SIZE) != 0) {
			return 1;
		}

		/* discard CRC */
		sd_spi_read_write_byte(0xFF);
		sd_spi_read_write_byte(0xFF);

		/* receive response */
		retry = 64;
		do {
			response = sd_spi_read_write_byte(0xFF);

			/* transmit 0x05 accepted */
			if ((response & 0x1F) == 0x05) {
				return 0;
			}
		} while (--retry > 0);
		return 1;
	}
	return 0;
}

uint8_t sd_spi_send_cmd(uint8_t cmd, uint32_t arg)
{
	uint8_t r1;
	uint8_t crc = 1;
	uint8_t cmddata[6] = {0};
	uint32_t start_time, end_time;
	uint8_t retry;

	DESELECT();
	sd_spi_read_write_byte(0xFF);
	SELECT();

	/* wait SD ready */
	if (cmd != SD_SPI_CMD_GoIdleSte) {
		start_time = rtos_time_get_current_system_time_ms();
		while (sd_spi_read_write_byte(0xFF) != 0xFF) {
			end_time = rtos_time_get_current_system_time_ms();
			if (end_time - start_time > 500) {
				return 0xFF;
			}
		}
	}

	/* prepare CRC */
	if (cmd == SD_SPI_CMD_GoIdleSte) {
		crc = 0x95;
	} else if (cmd == SD_SPI_CMD_SendIfCond) {
		crc = 0x87;
	} else {
		crc = 1;
	}

	/* transmit cmd */
	cmddata[0] = cmd | 0x40;
	cmddata[1] = arg >> 24;
	cmddata[2] = arg >> 16;
	cmddata[3] = arg >> 8;
	cmddata[4] = arg;
	cmddata[5] = crc;
	sd_spi_write_buffer(cmddata, 6);

	/* Skip a stuff byte when stop reading */
	if (cmd == SD_SPI_CMD_StopXsmission) {
		sd_spi_read_write_byte(0xff);
	}

	/* receive response */
	retry = 10;
	do {
		r1 = sd_spi_read_write_byte(0xFF);
	} while ((r1 & 0x80) && --retry);
	return r1;
}

SD_SPI_RESULT sd_spi_get_sector_count(uint32_t *sector_count)
{
	uint32_t ret = SD_SPI_ERROR;
	uint8_t csd[16];
	uint8_t csd_structure;
	uint32_t c_size;
	uint8_t block_size;

	if (sector_count == NULL) {
		return SD_SPI_ERROR;
	}

	/* SEND_CSD */
	if (sd_spi_send_cmd(SD_SPI_CMD_SendCsd, 0) == 0 && sd_spi_recv_data((uint8_t *)csd, 16) == 0) {
		printf("CSD:");
		for (uint32_t i = 0; i < 16; i++) {
			printf("%x ", csd[i]);
		}
		printf("\n");

		csd_structure = (csd[0] & 0xC0) >> 6;
		if (csd_structure == 1) { 					// CSD v2.0 (SDHC/SDXC)
			c_size = ((csd[7] & 0x3F) << 16) | 		// Byte7[5:0]
					 (csd[8] << 8) | 				// Byte8[7:0]
					 csd[9]; 						// Byte9[7:0]
			*sector_count = (c_size + 1) * 1024;
		} else if (csd_structure == 0) { 			// CSD v1.0 (SDSC)
			c_size = ((csd[6] & 0x3F) << 10) | 		// Byte6[5:0]
					 (csd[7] << 2) | 				// Byte7[7:0]
					 ((csd[8] & 0xC0) >> 6); 		// Byte8[7:6]
			block_size = 1 << ((csd[5] & 0x0F)); 	// READ_BL_LEN
			*sector_count = (uint32_t)((c_size + 1) * (1 << (block_size - 9)));
		} else {
			*sector_count = 0;
		}

		if (*sector_count) {
			ret = SD_SPI_OK;
		}
	}

	/* Idle */
	DESELECT();
	sd_spi_read_write_byte(0xFF);

	return ret;
}

SD_SPI_RESULT sd_spi_get_status(void)
{
	return sd_status;
}

SD_SPI_RESULT sd_spi_device_init(void)
{
	RTK_LOGI(TAG, "%s\n", __FUNCTION__);
	char temp[4] = {0};
	uint32_t retry = 0;
	uint32_t start_time, end_time;

	sd_status = SD_SPI_NODISK;

	/* Init SPI */
	sd_spi_spi_init();

	sd_status = SD_SPI_INIT;
	DESELECT();

	/* 74+ cycles */
	for (uint16_t i = 0; i < 10; i++) {
		sd_spi_read_write_byte(0xFF);
	}

	SELECT();

	/* Send CMD0 */
	do {
		if (sd_spi_send_cmd(SD_SPI_CMD_GoIdleSte, 0) == 0x01) {
			break;
		}
		if (retry++ > 10) {
			sd_status = SD_SPI_INITERR;
			RTK_LOGE(TAG, "%s Send CMD0 fail!\n", __FUNCTION__);
			goto exit;
		}
	} while (1);

	/* Ver2.0 */
	if (sd_spi_send_cmd(SD_SPI_CMD_SendIfCond, 0x1AA) == 1) {
		sd_spi_read_buffer((uint8_t *)temp, 4);
		RTK_LOGI(TAG, "%s: IFCond param: %02x %02x %02x %02x\n", __FUNCTION__, temp[0], temp[1], temp[2], temp[3]);

		start_time = rtos_time_get_current_system_time_ms();
		do {
			if (sd_spi_send_cmd(SD_SPI_CMD_AppCmd, 0) <= 1 && sd_spi_send_cmd(SD_SPI_CMD_SdSendOpCond, 0x40000000) == 0) {
				break;
			}
			end_time = rtos_time_get_current_system_time_ms();
			if (end_time - start_time > 1000) {
				sd_status = SD_SPI_INITERR;
				RTK_LOGE(TAG, "%s Send CMD41 fail!\n", __FUNCTION__);
				goto exit;
			}
		} while (1);

		if (sd_spi_send_cmd(SD_SPI_CMD_ReadOcr, 0) == 0) {
			sd_spi_read_buffer((uint8_t *)temp, 4);
			RTK_LOGI(TAG, "%s: OCR: %02x %02x %02x %02x\n", __FUNCTION__, temp[0], temp[1], temp[2], temp[3]);
			if (temp[0] & 0x40) {
				sd_status = SD_SPI_OK;
				RTK_LOGI(TAG, "%s success!\n", __FUNCTION__);
				goto exit;
			}
		}
	} else {
		RTK_LOGE(TAG, "%s: Not SD2.0 card or CMD8 failed!\n", __FUNCTION__);
	}
	sd_status = SD_SPI_INITERR;

exit:
	/* Idle */
	DESELECT();
	sd_spi_read_write_byte(0xFF);
	return sd_status;
}

SD_SPI_RESULT sd_spi_device_deinit(void)
{
	spi_free(&spi_master);

	rtos_sema_delete(xRxSemaphore);
	rtos_sema_delete(xTxSemaphore);
	xTxSemaphore = NULL;
	xRxSemaphore = NULL;

	rtos_mutex_delete(&device_lock);
	device_lock = NULL;

	return SD_SPI_OK;
}

SD_SPI_RESULT sd_spi_read_data(uint8_t *buf, uint32_t sector, uint32_t cnt)
{
	// RTK_LOGI("", "%s sec %d cnt %d\n", __func__, sector, cnt);
	uint32_t ret = SD_SPI_ERROR;
	uint32_t i;

	if (cnt == 1) {
		/* WRITE_BLOCK */
		if (sd_spi_send_cmd(SD_SPI_CMD_RdSingleBlk, sector) == 0) {
			if (sd_spi_recv_data(buf, SD_SPI_BLOCK_SIZE) == 0) {
				ret = SD_SPI_OK;
			}
		}
	} else {
		/* READ_MULTIPLE_BLOCK */
		if (sd_spi_send_cmd(SD_SPI_CMD_RdMulBlk, sector) == 0) {
			ret = SD_SPI_OK;
			for (i = 0; i < cnt; i++) {
				if (sd_spi_recv_data(buf, SD_SPI_BLOCK_SIZE) != 0) {
					ret = SD_SPI_ERROR;
					break;
				}
				buf += SD_SPI_BLOCK_SIZE;
			}
			/* STOP_TRANSMISSION */
			sd_spi_send_cmd(SD_SPI_CMD_StopXsmission, 0);
		}
	}

	/* Idle */
	DESELECT();
	sd_spi_read_write_byte(0xFF);

	return ret;
}

SD_SPI_RESULT sd_spi_write_data(uint8_t *buf, uint32_t sector, uint32_t cnt)
{
	// RTK_LOGI("", "%s sec %d cnt %d\n", __func__, sector, cnt);
	uint32_t ret = SD_SPI_ERROR;
	uint32_t i;

	if (cnt == 1) {
		/* WRITE_BLOCK */
		if (sd_spi_send_cmd(SD_SPI_CMD_WrBlk, sector) == 0) {
			if (sd_spi_send_data(buf, 0xFE) == 0) {
				ret = SD_SPI_OK;
			}
		}
	} else {
		/* WRITE_MULTIPLE_BLOCK */
		if (sd_spi_send_cmd(SD_SPI_CMD_AppCmd, 0) == 0 && sd_spi_send_cmd(SD_SPI_CMD_SetWrBlkEraseCnt, cnt) == 0
			&& sd_spi_send_cmd(SD_SPI_CMD_WrMulBlk, sector) == 0) {
			ret = SD_SPI_OK;
			for (i = 0; i < cnt; i++) {
				if (sd_spi_send_data(buf, 0xFC) != 0) {
					ret = SD_SPI_ERROR;
					break;
				}
				buf += SD_SPI_BLOCK_SIZE;
			}
			/* STOP_TRAN token */
			sd_spi_send_data(0, 0xFD);
		}
	}

	/* Idle */
	DESELECT();
	sd_spi_read_write_byte(0xFF);

	return ret;
}
#endif