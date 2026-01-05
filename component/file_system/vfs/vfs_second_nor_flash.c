#include "ameba_soc.h"
#include "os_wrapper.h"
#include <string.h>
#include "gpio_api.h"
#include "spi_api.h"
#include "spi_ex_api.h"
#include "vfs_second_nor_flash.h"

static const char *TAG = "SECOND-FLASH";

static rtos_sema_t xTxSemaphore = NULL;
static rtos_sema_t xRxSemaphore = NULL;
static rtos_mutex_t device_lock = NULL;
static spi_t spi_master;

static volatile int MasterTxDone;
static volatile int MasterRxDone;

volatile int second_flash_init_flag = 0;
flash_model_t current_flash_model = {0};

void second_flash_tr_done_callback(u32 id, SpiIrq event)
{
	(void) id;
	switch (event) {
	case SpiRxIrq:
		rtos_sema_give(xRxSemaphore);
		break;
	case SpiTxIrq:
		rtos_sema_give(xTxSemaphore);
		break;
	default:
		RTK_LOGI(TAG, "unknown interrput evnent! \r\n");
	}
}

void second_flash_wait_idle(void)
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

/* Master mode read a sequence of data
** address: start address of reading
** length：length of data to be read
** rx_buffer：buffer for reading data
			  Notes: The first four bytes are the dummy bytes of the command, and the data starts from the fifth byte
*/
int32_t second_flash_read_stream(uint32_t address, uint32_t length, char *rx_buffer)
{
	int32_t ret = 0;
	char *cmd = NULL;
	char *temp = NULL;
	uint32_t total_len = length + CMD_LENGTH_FOUR;

	second_flash_wait_idle();
	rtos_mutex_take(device_lock, RTOS_SEMA_MAX_COUNT);

	spi_flush_rx_fifo(&spi_master);

	//RTK_LOGI(TAG, "%s, start, address: 0x%08x, length: %d\n", __func__, address, length);
	// CACHE_LINE_SIZE align
	total_len += (CACHE_LINE_SIZE - (total_len % CACHE_LINE_SIZE));

	cmd = (char *)rtos_mem_zmalloc(total_len);
	if (!cmd) {
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		rtos_mutex_give(device_lock);
		return -1;
	}

	temp = (char *)rtos_mem_zmalloc(total_len);
	if (!temp) {
		rtos_mem_free(cmd);
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		rtos_mutex_give(device_lock);
		return -1;
	}

	/* transmit read cmd */
	cmd[0] = CMD_READ_DATA;
	cmd[1] = address >> 16;
	cmd[2] = (address >> 8) & 0xFF;
	cmd[3] = address & 0xFF;

	spi_master_write_read_stream_dma(&spi_master, cmd, temp, total_len);

	rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
	rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);

	_memcpy(rx_buffer, temp + CMD_LENGTH_FOUR, length);

	rtos_mem_free(temp);
	rtos_mem_free(cmd);
	rtos_mutex_give(device_lock);
	//RTK_LOGI(TAG, "%s, finish, address: 0x%08x, length: %d\n", __func__, address, length);
	return ret;
}

/* Master mode fast read a sequence of data
** address: start address of reading
** length：length of data to be read
** rx_buffer：buffer for reading data
			  Notes: The first five bytes are the dummy bytes of the command, and the data starts from the sixth byte
*/
int32_t second_flash_fast_read_stream(uint32_t address, uint32_t length, char *rx_buffer)
{
	int32_t ret = 0;
	char *cmd = NULL;
	char *temp = NULL;
	uint32_t total_len = length + CMD_LENGTH_FIVE;

	second_flash_wait_idle();
	rtos_mutex_take(device_lock, RTOS_SEMA_MAX_COUNT);

	// CACHE_LINE_SIZE align
	total_len += (CACHE_LINE_SIZE - (total_len % CACHE_LINE_SIZE));

	cmd = (char *)rtos_mem_zmalloc(total_len);
	if (!cmd) {
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		rtos_mutex_give(device_lock);
		return -1;
	}

	temp = (char *)rtos_mem_zmalloc(total_len);
	if (!temp) {
		rtos_mem_free(cmd);
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		rtos_mutex_give(device_lock);
		return -1;
	}

	// RTK_LOGI(TAG, "%s, start, address: 0x%08x, length: %d\n", __func__, address, length);

	spi_flush_rx_fifo(&spi_master);

	/* transmit read cmd */
	cmd[0] = CMD_FAST_READ_DATA;
	cmd[1] = address >> 16;
	cmd[2] = (address >> 8) & 0xFF;
	cmd[3] = address & 0xFF;

	spi_master_write_read_stream_dma(&spi_master, cmd, temp, total_len);
	rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
	rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);

	_memcpy(rx_buffer, temp + CMD_LENGTH_FIVE, length);

	rtos_mem_free(temp);
	rtos_mem_free(cmd);
	rtos_mutex_give(device_lock);
	// RTK_LOGI(TAG, "%s, finish, address: 0x%08x, length: %d\n", __func__, address, length);
	return ret;
}

/* Master mode erase sector
** address: start address of erasing
*/
int32_t second_flash_erase_sector(uint32_t address)
{
	int32_t ret = 0;
	char *status = NULL;
	char *cmd = NULL;
	int count = 0;

	second_flash_wait_idle();
	rtos_mutex_take(device_lock, RTOS_SEMA_MAX_COUNT);

	cmd = (char *)rtos_mem_zmalloc(CACHE_LINE_SIZE);
	if (!cmd) {
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		rtos_mutex_give(device_lock);
		return -1;
	}

	status = (char *)rtos_mem_zmalloc(CACHE_LINE_SIZE);
	if (!status) {
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		rtos_mem_free(cmd);
		rtos_mutex_give(device_lock);
		return -1;
	}
	spi_flush_rx_fifo(&spi_master);

	/* transmit write enable cmd */
	cmd[0] = CMD_WRITE_ENABLE;
	spi_master_write_stream_dma(&spi_master, cmd, CMD_LENGTH_ONE);

	rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
	/* check WIP and WEL*/
	cmd[0] = CMD_READ_STATUS;
	do {
		spi_flush_rx_fifo(&spi_master);
		second_flash_wait_idle();
		spi_master_write_read_stream_dma(&spi_master, cmd, status, CACHE_LINE_SIZE);

		rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
		rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);
		count++;
		if (count % 20 == 0) {
			RTK_LOGI(TAG, "%s, wren, count: %d\n", __func__, count);
		}
	} while (status[1] & WIP_BIT || !(status[1] & WEL_BIT));
	/* transmit erase cmd */
	cmd[0] = CMD_SECTOR_ERASE;
	cmd[1] = address >> 16;
	cmd[2] = (address >> 8) & 0xFF;
	cmd[3] = 0x00;

	spi_flush_rx_fifo(&spi_master);
	spi_master_write_stream_dma(&spi_master, cmd, CMD_LENGTH_FOUR);

	rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);

	/* check WIP */
	_memset(cmd, 0x0, CACHE_LINE_SIZE);
	cmd[0] = CMD_READ_STATUS;
	count = 0;
	do {
		spi_flush_rx_fifo(&spi_master);
		second_flash_wait_idle();
		spi_master_write_read_stream_dma(&spi_master, cmd, status, CACHE_LINE_SIZE);

		rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
		rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);
		count++;
		if (count % 200 == 0) {
			RTK_LOGI(TAG, "%s, erase, count: %d\n", __func__, count);
		}
	} while (status[1] & WIP_BIT);

	rtos_mem_free(cmd);
	rtos_mem_free(status);
	rtos_mutex_give(device_lock);
	return ret;
}

/* Master mode write a sequence of data
** address: start address of writting
** length：length of data
** tx_buffer：data to be written
*/
int32_t second_flash_write_stream(uint32_t address, uint32_t length, char *tx_buffer)
{
	int32_t ret = 0;
	char *status = NULL;
	uint32_t position = address;
	/*cmd_length = 4, page_size = 256*/
	char *cmd = NULL;
	int count = 0;

	second_flash_wait_idle();
	rtos_mutex_take(device_lock, RTOS_SEMA_MAX_COUNT);

	cmd = (char *)rtos_mem_zmalloc(CMD_LENGTH_FOUR + FLASH_PAGE_SIZE);
	if (!cmd) {
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		rtos_mutex_give(device_lock);
		return -1;
	}

	status = (char *)rtos_mem_zmalloc(CACHE_LINE_SIZE);
	if (!status) {
		RTK_LOGE(TAG, "%s, malloc failed\n", __func__);
		rtos_mem_free(cmd);
		rtos_mutex_give(device_lock);
		return -1;
	}

	//RTK_LOGI(TAG, "%s, start, address: 0x%08x, length: %d\n", __func__, address, length);

	while (position < address + length) {
		spi_flush_rx_fifo(&spi_master);
		_memset(cmd, 0x0, CACHE_LINE_SIZE);
		/* transmit write enable cmd */
		cmd[0] = CMD_WRITE_ENABLE;
		spi_master_write_stream_dma(&spi_master, cmd, CMD_LENGTH_ONE);

		rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
		spi_flush_rx_fifo(&spi_master);
		/* check WIP and WEL */
		cmd[0] = CMD_READ_STATUS;
		do {
			second_flash_wait_idle();
			spi_master_write_read_stream_dma(&spi_master, cmd, status, CACHE_LINE_SIZE);

			rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
			rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);
			count++;
			if (count % 20 == 0) {
				RTK_LOGI(TAG, "%s, wren, count: %d\n", __func__, count);
			}
		} while (status[1] & WIP_BIT || !(status[1] & WEL_BIT));

		/* transmit page program cmd */
		cmd[0] = CMD_PAGE_PROGRAM;
		cmd[1] = position >> 16;
		cmd[2] = (position >> 8) & 0xFF;
		cmd[3] = position & 0xFF;

		spi_flush_rx_fifo(&spi_master);
		if (position % FLASH_PAGE_SIZE == 0) {						/* programm from the start address of a page*/
			_memcpy((u8 *)cmd + CMD_LENGTH_FOUR, tx_buffer, FLASH_PAGE_SIZE);
			/* transmit data */
			ret = spi_master_write_stream_dma(&spi_master, cmd, CMD_LENGTH_FOUR + FLASH_PAGE_SIZE);
			tx_buffer = (char *)tx_buffer + FLASH_PAGE_SIZE;
			position += FLASH_PAGE_SIZE;
		} else {											/* programm not from the start address of a page*/
			_memcpy((u8 *)cmd + CMD_LENGTH_FOUR, tx_buffer, FLASH_PAGE_SIZE - position % FLASH_PAGE_SIZE);
			/* transmit data */
			ret = spi_master_write_stream_dma(&spi_master, cmd, CMD_LENGTH_FOUR + FLASH_PAGE_SIZE - position % FLASH_PAGE_SIZE);
			tx_buffer = (char *)tx_buffer + (FLASH_PAGE_SIZE - position % FLASH_PAGE_SIZE);
			position += (FLASH_PAGE_SIZE - position % FLASH_PAGE_SIZE);
		}

		rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
		spi_flush_rx_fifo(&spi_master);
		_memset(cmd, 0x0, CACHE_LINE_SIZE);
		/* check WIP */
		cmd[0] = CMD_READ_STATUS;
		count = 0;
		do {
			second_flash_wait_idle();
			ret = spi_master_write_read_stream_dma(&spi_master, cmd, status, CACHE_LINE_SIZE);

			rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
			rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);
			count++;
			if (count % 20 == 0) {
				RTK_LOGI(TAG, "%s, write, count: %d\n", __func__, count);
			}
		} while (status[1] & WIP_BIT);
	}
	rtos_mem_free(cmd);
	rtos_mem_free(status);
	rtos_mutex_give(device_lock);
	//RTK_LOGI(TAG, "%s, finish, address: 0x%08x, length: %d\n", __func__, address, length);
	return ret;
}

void second_flash_get_id(void)
{
	int32_t ret;
	char device_id[CMD_LENGTH_FIVE] = {0};
	char device_cmd[CMD_LENGTH_FIVE] = {0};
	int retry_count = 0;

	/* retry max 3 times */
	while (retry_count < 3) {
		rtos_mutex_take(device_lock, RTOS_SEMA_MAX_COUNT);

		/* read flash ID */
		device_cmd[0] = 0xAB;

		ret = spi_master_write_read_stream(&spi_master, device_cmd, device_id, CMD_LENGTH_FIVE);
		if (ret != HAL_OK) {
			RTK_LOGW(TAG, "%s: state(0x%lx) is not ready (retry %d)\r\n", __func__, ret, retry_count);
			rtos_mutex_give(device_lock);
			retry_count++;
			continue;
		}

		rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
		rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);

		// RTK_LOGI(TAG, "Device ID: 0x%02X\r\n", device_id[4]);
		rtos_mutex_give(device_lock);

		rtos_mutex_take(device_lock, RTOS_SEMA_MAX_COUNT);

		/* read flash ID */
		device_cmd[0] = CMD_READ_DEVICE_ID;

		ret = spi_master_write_read_stream(&spi_master, device_cmd, device_id, CMD_LENGTH_FOUR);
		if (ret != HAL_OK) {
			RTK_LOGW(TAG, "%s: state(0x%lx) is not ready (retry %d)\r\n", __func__, ret, retry_count);
			rtos_mutex_give(device_lock);
			retry_count++;
			continue;
		}

		rtos_sema_take(xTxSemaphore, RTOS_SEMA_MAX_COUNT);
		rtos_sema_take(xRxSemaphore, RTOS_SEMA_MAX_COUNT);

		/* check id vaild */
		if (device_id[1] == 0 || device_id[1] == 0xff || device_id[2] == 0 || device_id[2] == 0xff || device_id[3] == 0 || device_id[3] == 0xff) {
			RTK_LOGW(TAG, "%s: Invalid ID, retry %d\r\n", __func__, retry_count);
			rtos_mutex_give(device_lock);
			retry_count++;
			continue;
		}

		/* match Flash model */
		for (u32 i = 0; i < sizeof(flash_models) / sizeof(flash_models[0]); i++) {
			if (device_id[1] == flash_models[i].manufacturer_id &&
				device_id[2] == flash_models[i].memory_type_id &&
				device_id[3] == flash_models[i].capacity_id) {
				current_flash_model = flash_models[i];
				break;
			}
		}

		RTK_LOGI(TAG, "Manufacturer ID: 0x%02X\r\n", device_id[1]);
		RTK_LOGI(TAG, "Memory Type ID : 0x%02X\r\n", device_id[2]);
		RTK_LOGI(TAG, "Capacity ID    : 0x%02X\r\n", device_id[3]);
		if (current_flash_model.manufacturer_id == 0) {
			RTK_LOGI(TAG, "Secdonary Flash type is not in the flash list. Please add it in vfs_second_nor_flash.h.\r\n");
		} else {
			RTK_LOGI(TAG, "Detected Flash: %s\r\n", current_flash_model.model_name);
		}
		rtos_mutex_give(device_lock);
		second_flash_init_flag = 1;
		return;
	}

	/* Read Flash ID Fail */
	RTK_LOGE(TAG, "%s: Failed to read Flash ID after %d retries!\r\n", __func__, retry_count);
	second_flash_init_flag = -1;
}

void second_flash_spi_init(void)
{
	spi_master.spi_idx = MBED_SPI0;
	RTK_LOGI(TAG, "%s, spi idx: %X, freq: %d Hz\n", __func__, spi_master.spi_idx & 0x01, SCLK_FREQ);
	spi_init(&spi_master, SPI0_MOSI, SPI0_MISO, SPI0_SCLK, SPI0_CS);
	spi_format(&spi_master, 8, 0, 0);
	spi_frequency(&spi_master, SCLK_FREQ);
	spi_irq_hook(&spi_master, (spi_irq_handler)second_flash_tr_done_callback, (uint32_t)&spi_master);

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

void second_flash_spi_free(void)
{
	spi_free(&spi_master);
}

void second_flash_flush_rx_fifo(void)
{
	spi_flush_rx_fifo(&spi_master);
}
