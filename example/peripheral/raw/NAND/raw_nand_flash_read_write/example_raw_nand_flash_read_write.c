#include "ameba_soc.h"
#include "os_wrapper.h"

#define FLASH_TEST_BASE_BLOCK    0x100

static void flash_test_task(void *param)
{
	UNUSED(param);

	DiagPrintf("\r\n   NAND FLASH DEMO   \r\n");

	extern FLASH_InitTypeDef flash_init_para;

	uint8_t data[2112] __attribute__((aligned(32)));
	uint8_t data_w[2112] __attribute__((aligned(32)));

	NAND_SPIC_DPHY_CTRL(&flash_init_para, 0);

	NAND_StructInit(&flash_init_para);
	if (!NAND_Init(Spic4IOBitMode)) { // was Spic1IOBitMode
		DiagPrintf("SPI Init Fail!!!!!!\n");
	} else {
		DiagPrintf("SPI Init SUCCESS\n");
	}

	NAND_SPIC_DPHY_CTRL(&flash_init_para, 1);

	NAND_RxCmd(flash_init_para.FLASH_cmd_rd_id, 0, NULL, 3, data);
	DiagPrintf("MID:0x%x, DID:0x%x, DID:0x%x \n\r", data[0], data[1], data[2]);

	u8 feature_id = 0xA0;
	DiagPrintf("0x%x:0x%02x\n", feature_id, NAND_GetStatus(feature_id));
	feature_id = 0xB0;
	DiagPrintf("0x%x:0x%02x\n", feature_id, NAND_GetStatus(feature_id));
	feature_id = 0xC0;
	DiagPrintf("0x%x:0x%02x\n", feature_id, NAND_GetStatus(feature_id));

	DiagPrintf("Bad block scan!\n\r");
	for (int i = 0; i < 1024; i++) {
		NAND_Page_Read(i * 64, 0, 2048 + 32, &data[0]);
		if (data[2048] != 0xff) {
			DiagPrintf("Block %d is bad block!\n\r", i);
		}
	}
	DiagPrintf("Bad block scan done!\n\r");
	NAND_Page_Read(FLASH_TEST_BASE_BLOCK * 64, 0, 2048 + 32, &data[0]);

	if (data[2048] == 0xff) {
		NAND_Erase(FLASH_TEST_BASE_BLOCK * 64);
		NAND_Page_Read(FLASH_TEST_BASE_BLOCK * 64, 0, 2048 + 32, &data[0]);
		for (int i = 0; i < 2112; i++) {
			data_w[i] = i & 0xff ;
		}

		NAND_Page_Write(FLASH_TEST_BASE_BLOCK * 64, 0, 2048, &data_w[0]);

		//read & check data
		NAND_Page_Read(FLASH_TEST_BASE_BLOCK * 64, 0, 2048 + 32, &data[0]);
		for (int i = 0; i < 2048; i++) {
			if (data[i] != data_w[i]) {
				DiagPrintf("data[%d]=%d is not data_w[%d]=%d !\n\r", i, data[i], i, data_w[i]);
			}
		}
		DiagPrintf("NAND FLASH DEMO Done!\n\r");

	} else {
		DiagPrintf("Block %d is Bad Block!\n\r", FLASH_TEST_BASE_BLOCK);

	}

	rtos_task_delete(NULL);
}

int example_raw_nand_flash_read_write(void)
{
	if (rtos_task_create(NULL, ((const char *)"flash_test_task"), flash_test_task, NULL, 1024 * 16, 1) != RTK_SUCCESS) {
		RTK_LOGI(NOTAG, "\n\r%s rtos_task_create(flash_test_task) failed", __FUNCTION__);
	}

	rtos_sched_start();
	while (1) {
		rtos_time_delay_ms(1000);
	}

	return 0;
}
