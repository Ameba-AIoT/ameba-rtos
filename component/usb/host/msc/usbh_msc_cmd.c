/**
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "platform_stdlib.h"
#include "basic_types.h"
#include "usbh.h"
#include "os_wrapper.h"
#include "ff.h"
#include "vfs_fatfs.h"
#include "usbh_msc.h"


/* Private defines -----------------------------------------------------------*/
static const char *TAG = "MSCH";
#define USBH_MSC_TEST_BUF_SIZE      4096
#define USBH_MSC_TEST_ROUNDS        20
#define USBH_MSC_TEST_SEED          0xA5

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int msc_cb_attach(void);
static int msc_cb_setup(void);
static int msc_cb_process(usb_host_t *host, u8 id);

static u32 usbh_msc_cmd_test(u16 argc, u8 *argv[]);

/* Public variables ---------------------------------------------------------*/
static rtos_sema_t msc_attach_sema;
static __IO int msc_is_ready = 0;
static u32 filenum = 0;

static usbh_config_t usbh_cfg = {
	.pipes = 5U,
	.speed = USB_SPEED_HIGH,
	.dma_enable = FALSE,
	.main_task_priority = 3U,
	.isr_task_priority  = 4U,
};

static usbh_msc_cb_t msc_usr_cb = {
	.attach = msc_cb_attach,
	.setup = msc_cb_setup,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = msc_cb_process
};


CMD_TABLE_DATA_SECTION
const COMMAND_TABLE usbh_msc_cmd_table[] = {
	{
		(const u8 *)"usbh_msc", 8, usbh_msc_cmd_test, (const u8 *)"\tUSBH MSC cmd:\n"
		"\t\t usbh_msc init\n"
		"\t\t usbh_msc deinit\n"
		"\t\t usbh_msc rw_test <looptime>\n"
	}
};

/* Private functions ---------------------------------------------------------*/
static int msc_cb_attach(void)
{
	RTK_LOGS(TAG, "ATTACH\n");
	rtos_sema_give(msc_attach_sema);
	return HAL_OK;
}

static int msc_cb_setup(void)
{
	RTK_LOGS(TAG, "SETUP\n");
	msc_is_ready = 1;
	return HAL_OK;
}

static int msc_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		msc_is_ready = 0;
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}

	return HAL_OK;
}

static int msc_trx_test(u8 looptime)
{
	FATFS fs;
	FIL f;
	int drv_num = 0;
	FRESULT res;
	char logical_drv[4];
	char path[64];
	int ret = 0;
	u32 br;
	u32 bw;
	u32 round = 0;
	u32 start;
	u32 elapse;
	u32 perf = 0;
	u32 test_sizes[] = {512, 1024, 2048, 4096, 8192};
	u32 test_size;
	u32 loop;
	u32 i;
	u8 *buf = NULL;

	buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	if (buf == NULL) {
		RTK_LOGS(TAG, "Fail to alloc test buf\n");
		goto exit;
	}

	// Register USB disk driver to fatfs
	RTK_LOGS(TAG, "Register USB disk\n");
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
	if (drv_num < 0) {
		RTK_LOGS(TAG, "Fail to register\n");
		goto exit_free;
	}

	logical_drv[0] = drv_num + '0';
	logical_drv[1] = ':';
	logical_drv[2] = '/';
	logical_drv[3] = 0;

	RTK_LOGS(TAG, "FatFS USB W/R performance test start...\n");

	while (1) {
		if (msc_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	if (f_mount(&fs, logical_drv, 1) != FR_OK) {
		RTK_LOGS(TAG, "Fail to mount logical drive\n");
		goto exit_unregister;
	}

	strcpy(path, logical_drv);

	if (rtos_sema_take(msc_attach_sema, RTOS_SEMA_MAX_COUNT) != SUCCESS) {
		RTK_LOGS(TAG, "Fail to take sema\n");
		goto exit_unmount;
	}

	for (loop = 0; loop < looptime; ++loop) {
		while (1) {
			if (msc_is_ready) {
				rtos_time_delay_ms(10);
				break;
			}
		}

		sprintf(&path[3], "TEST%ld.DAT", filenum);
		RTK_LOGS(TAG, "Open file: %s\n", path);
		/* Open test file */
		res = f_open(&f, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			RTK_LOGS(TAG, "Fail to open file: TEST%d.DAT\n", filenum);
			goto exit_unmount;
		}
		/* Clean write and read buffer */
		memset(buf, USBH_MSC_TEST_SEED, USBH_MSC_TEST_BUF_SIZE);

		for (i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); ++i) {
			test_size = test_sizes[i];
			if (test_size > USBH_MSC_TEST_BUF_SIZE) {
				break;
			}

			RTK_LOGS(TAG, "W test: size %d, round %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_write(&f, (void *)buf, test_size, (UINT *)&bw);
				if (res || (bw < test_size)) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, "W err bw=%d, rc=%d\n", bw, res);
					ret = 1;
					break;
				}
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, "W rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);

			/* Move the file pointer to the file head */
			res = f_lseek(&f, 0);

			RTK_LOGS(TAG, "R test: size = %d round = %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_read(&f, (void *)buf, test_size, (UINT *)&br);
				if (res || (br < test_size)) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, "R err br=%d, rc=%d\n", br, res);
					ret = 1;
					break;
				}
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, "R rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);

			/* Move the file pointer to the file head */
			res = f_lseek(&f, 0);
		}

		RTK_LOGS(TAG, "FatFS USB W/R performance test %s\n", (ret == 0) ? "done" : "abort");

		/* Close source file */
		res = f_close(&f);
		if (res) {
			RTK_LOGS(TAG, "File close fail\n");
			ret = 1;
		} else {
			RTK_LOGS(TAG, "File close OK\n");
		}

		if (!ret) {
			filenum++;
		} else {
			break;
		}
	}

exit_unmount:
	if (f_mount(NULL, logical_drv, 1) != FR_OK) {
		RTK_LOGS(TAG, "Fail to unmount logical drive\n");
	}
exit_unregister:
	if (FATFS_UnRegisterDiskDriver(drv_num)) {
		RTK_LOGS(TAG, "Fail to unregister disk driver from FATFS\n");
	}
exit_free:
	if (buf) {
		rtos_mem_free(buf);
	}
exit:
	return  ret;
}

static u32 usbh_msc_cmd_test(u16 argc, u8 *argv[])
{
	int ret = HAL_OK;
	const char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, "Invalid USB argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (const char *)argv[0];

	if (_stricmp(cmd, "init") == 0) {
		rtos_sema_create(&msc_attach_sema, 0U, 1U);
		ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
		if (ret != HAL_OK) {
			rtos_sema_delete(msc_attach_sema);
			RTK_LOGS(TAG, "Fail to init USBH: %d\n", ret);
			return ret;
		}

		ret = usbh_msc_init(&msc_usr_cb);
		if (ret != HAL_OK) {
			usbh_deinit();
			rtos_sema_delete(msc_attach_sema);
			RTK_LOGS(TAG, "Fail to init MSC: %d\n", ret);
		}
	} else if (_stricmp(cmd, "deinit") == 0) {
		ret = usbh_deinit();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, "Fail to deinit USBH: %d\n", ret);
		}

		ret = usbh_msc_deinit();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, "Fail to deinit MSC: %d\n", ret);
		}
		rtos_sema_delete(msc_attach_sema);
	} else if (_stricmp(cmd, "rw_test") == 0) {
		u8 loop = 5;
		if (argv[1]) {
			loop = _strtoul((const char *)(argv[1]), (char **)NULL, 10);
		}
		ret = msc_trx_test(loop);
		if (ret == HAL_OK) {
			RTK_LOGS(TAG, "MSC test OK\n");
		} else {
			RTK_LOGS(TAG, "MSC test fail: %d\n", ret);
		}
	} else {
		RTK_LOGS(TAG, "Input cmd err\n");
		RTK_LOGS(TAG, "%s\n", usbh_msc_cmd_table[0].msg);
	}

	return ret;
}
