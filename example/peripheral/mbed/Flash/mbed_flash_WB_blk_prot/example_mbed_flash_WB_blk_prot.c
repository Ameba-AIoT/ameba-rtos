#include "diag.h"
#include "main.h"
#include "objects.h"
#include "os_wrapper.h"
#include "flash_api.h"
#include <stdio.h>

// Decide starting flash address for storing application data
// User should pick address carefully to avoid corrupting image section

#define FLASH_APP_BASE  0x180000
#define length 8

#ifdef CONFIG_AMEBASMART
#define PROTECTION_CMD 0x38
#else
#define PROTECTION_CMD 0x1C
#endif

static void flash_test_task(void *param)
{
	flash_t         flash;
	uint32_t        address = FLASH_APP_BASE;

	int result1 = 1, result2 = 1, result3 = 1;
	int i = 0;
	int loop = 0;

	char write_data1[length];
	char write_data2[length];
	char buff[length];
	char flash_info[3];

	UNUSED(param);

	// check Menufacture ID. If not Winbond, return
	flash_read_id(&flash, (u8 *)&flash_info, 3);
	RTK_LOGI(NOTAG, "flash ID is: %02x-%02x-%02x \n", flash_info[0], flash_info[1], flash_info[2]);
	if (flash_info[0] != 0xef) {
		RTK_LOGI(NOTAG, "Not Winbond flash. Please modify the setting of the status register in this example according to the actual Flash data sheet \n");
		while (1);
	}

	for (i = 0; i < length; i++) {
		write_data1[i] = i;
		write_data2[i] = ~i;
	}

	for (loop = 0; loop < 8; loop++) {
		RTK_LOGI(NOTAG, "Test Address = %lx\n", address);
		flash_erase_sector(&flash, address);
		flash_burst_write(&flash, address, length, (u8 *) &write_data1[0]);
		flash_stream_read(&flash, address, length, (u8 *) &buff[0]);

		RTK_LOGI(NOTAG, "Before Lock\n");
		for (i = 0; i < length; i++) {
			if (write_data1[i] != buff[i]) {
				RTK_LOGI(NOTAG, "Error : Write Data is = %x, Read Data is %x\n", write_data1[i], buff[i]);
				result1 = 0;
			}
		}
		if (result1 == 1) {
			RTK_LOGI(NOTAG, "Success 1\n");
		}

		RTK_LOGI(NOTAG, "Lock\n");
		result2 = 1;
		RTK_LOGI(NOTAG, "Status Register Before Setting: 0x%02x\n", 0xff & flash_get_status(&flash));

		flash_set_status(&flash, PROTECTION_CMD);
		flash_erase_sector(&flash, address);
		flash_burst_write(&flash, address, length, (u8 *)&write_data2[0]);
		flash_stream_read(&flash, address, length, (u8 *)&buff[0]);

		for (i = 0; i < length; i++) {
			if (write_data1[i] != buff[i]) {
				RTK_LOGI(NOTAG, "Error : Write Data1 is = %x, Write Data2 is = %x, Read Data is %x\n", write_data1[i], write_data2[i], buff[i]);
				result2 = 0;
			}
		}
		if (result2 == 1) {
			RTK_LOGI(NOTAG, "Success 2\n");
		}

		RTK_LOGI(NOTAG, "Unlock\n");
		result3 = 1;
		RTK_LOGI(NOTAG, "Status Register After Setting: 0x%02x\n", 0xff & flash_get_status(&flash));

		flash_set_status(&flash, flash_get_status(&flash) & 0x0);
		flash_erase_sector(&flash, address);
		flash_burst_write(&flash, address, length, (u8 *)&write_data2[0]);
		flash_stream_read(&flash, address, length, (u8 *)&buff[0]);

		for (i = 0; i < length; i++) {
			if (write_data2[i] != buff[i]) {
				RTK_LOGI(NOTAG, "Error : Write Data is = %x, Read Data is %x\n", write_data2[i], buff[i]);
				result3 = 0;
			}
		}
		if (result3 == 1) {
			RTK_LOGI(NOTAG, "Success 3\n");
		}

		flash_reset_status(&flash);//make sure the status register is reset if users would like to reload code
		RTK_LOGI(NOTAG, "Status Register After Reset: 0x%02x\n", 0xff & flash_get_status(&flash));
		RTK_LOGI(NOTAG, "Result is %s\r\n\n", (result1 && result2 && result3) ? "success" : "fail");
		address += 0x10000;
	}
	RTK_LOGI(NOTAG, "Test Done\n");
	rtos_task_delete(NULL);
}

int example_mbed_flash_winbond_block_protect(void)
{
	if (rtos_task_create(NULL, ((const char *)"flash_test_task"), flash_test_task, NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGI(NOTAG, "\n\r%s rtos_task_create(flash_test_task) failed", __FUNCTION__);
	}

	rtos_sched_start();
	while (1) {
		rtos_time_delay_ms(1000);
	}

	return 0;
}
