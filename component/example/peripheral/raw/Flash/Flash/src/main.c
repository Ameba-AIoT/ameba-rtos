
#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"
#include <stdio.h>

#define FLASH_APP_BASE  0x1EF000 //the last sector of IMG2

static void flash_test_task(void *param)
{
	uint32_t address = FLASH_APP_BASE;

	uint32_t val32_to_write = 0x13572468;
	uint32_t val32_to_read;
	int loop = 0;
	int result = 0;

	UNUSED(param);

	for (loop = 0; loop < 10; loop++) {
		val32_to_read = HAL_READ32(SPI_FLASH_BASE, address);
		printf("\r\nRead Data 0x%lx\n", val32_to_read);

		FLASH_Write_Lock();
		FLASH_Erase(EraseSector, address);
		FLASH_TxData(address, 4, (u8 *)&val32_to_write);
		FLASH_Write_Unlock();

		DCache_Invalidate(SPI_FLASH_BASE + address, 4);

		val32_to_read = HAL_READ32(SPI_FLASH_BASE, address);
		printf("Read Data 0x%lx\n", val32_to_read);

		// verify result
		result = (val32_to_write == val32_to_read) ? 1 : 0;
		printf("Result is %s\r\n", (result) ? "success" : "fail");
		result = 0;
	}

	rtos_task_delete(NULL);
}

int main(void)
{
	if (rtos_task_create(NULL, ((const char *)"flash_test_task"), flash_test_task, NULL, 1024 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create(flash_test_task) failed", __FUNCTION__);
	}

	rtos_sched_start();
	while (1) {
		rtos_time_delay_ms(1000);
	}

	return 0;
}
