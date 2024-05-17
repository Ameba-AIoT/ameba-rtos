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

#define USBH_MSC_THREAD_STACK_SIZE  (1024*8)
#define USBH_MSC_TEST_BUF_SIZE      4096
#define USBH_MSC_TEST_ROUNDS        20
#define USBH_MSC_TEST_SEED          0xA5

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int msc_cb_attach(void);
static int msc_cb_setup(void);
static int msc_cb_process(usb_host_t *host, u8 id);

/* Private variables ---------------------------------------------------------*/

static rtos_sema_t msc_attach_sema;
static __IO int msc_is_ready = 0;
static u32 filenum = 0;

static usbh_config_t usbh_cfg = {
	.pipes = 5U,
	.speed = USB_SPEED_HIGH,
	.dma_enable = FALSE,
	.main_task_priority = 3U,
	.isr_task_priority = 4U,
	.rx_fifo_size = 0x200U,
	.nptx_fifo_size = 0x100U,
	.ptx_fifo_size = 0x100U,
};

static usbh_msc_cb_t msc_usr_cb = {
	.attach = msc_cb_attach,
	.setup = msc_cb_setup,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = msc_cb_process
};

/* Private functions ---------------------------------------------------------*/

static int msc_cb_attach(void)
{
	printf("[MSC] ATTACH\n");
	rtos_sema_give(msc_attach_sema);
	return HAL_OK;
}

static int msc_cb_setup(void)
{
	printf("[MSC] SETUP\n");
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

void example_usbh_msc_thread(void *param)
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
	u32 i;
	u8 *buf = NULL;

	UNUSED(param);

	rtos_sema_create(&msc_attach_sema, 0U, 1U);

	buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	if (buf == NULL) {
		printf("[MSC] \nFail to allocate USBH MSC test buffer\n");
		goto exit;
	}

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		printf("[MSC] Fail to init USB\n");
		goto exit_free;
	}

	usbh_msc_init(&msc_usr_cb);

	// Register USB disk driver to fatfs
	printf("\nRegister USB disk driver\n");
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
	if (drv_num < 0) {
		printf("[MSC] Fail to register USB disk driver\n");
		goto exit_deinit;
	}

	logical_drv[0] = drv_num + '0';
	logical_drv[1] = ':';
	logical_drv[2] = '/';
	logical_drv[3] = 0;

	printf("FatFS USB Write/Read performance test started...\n");

	while (1) {
		if (msc_is_ready) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	if (f_mount(&fs, logical_drv, 1) != FR_OK) {
		printf("[MSC] Fail to mount logical drive\n");
		goto exit_unregister;
	}

	strcpy(path, logical_drv);

	while (1) {
		if (rtos_sema_take(msc_attach_sema, RTOS_SEMA_MAX_COUNT) != SUCCESS) {
			printf("[MSC] Fail to take attach sema\n");
			continue;
		}

		while (1) {
			if (msc_is_ready) {
				rtos_time_delay_ms(10);
				break;
			}
		}

		sprintf(&path[3], "TEST%ld.DAT", filenum);
		printf("[MSC] open file path: %s\n", path);
		// open test file
		res = f_open(&f, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			printf("[MSC] Fail to open file: TEST%ld.DAT\n", filenum);
			goto exit_unmount;
		}
		// clean write and read buffer
		memset(buf, USBH_MSC_TEST_SEED, USBH_MSC_TEST_BUF_SIZE);

		for (i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); ++i) {
			test_size = test_sizes[i];
			if (test_size > USBH_MSC_TEST_BUF_SIZE) {
				break;
			}

			printf("[MSC] Write test: size = %ld round = %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_write(&f, (void *)buf, test_size, (UINT *)&bw);
				if (res || (bw < test_size)) {
					f_lseek(&f, 0);
					printf("[MSC] Write error bw=%ld, rc=%d\n", bw, res);
					ret = 1;
					break;
				}
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			printf("[MSC] Write rate %ld.%ld KB/s for %ld round @ %ld ms\n", perf / 10, perf % 10, round, elapse);

			/* move the file pointer to the file head*/
			res = f_lseek(&f, 0);

			printf("[MSC] Read test: size = %ld round = %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_read(&f, (void *)buf, test_size, (UINT *)&br);
				if (res || (br < test_size)) {
					f_lseek(&f, 0);
					printf("[MSC] Read error br=%ld, rc=%d\n", br, res);
					ret = 1;
					break;
				}
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			printf("[MSC] Read rate %ld.%ld KB/s for %ld round @ %ld ms\n", perf / 10, perf % 10, round, elapse);

			/* move the file pointer to the file head*/
			res = f_lseek(&f, 0);
		}

		printf("[MSC] FatFS USB Write/Read performance test %s\n", (ret == 0) ? "done" : "aborted");

		// close source file
		res = f_close(&f);
		if (res) {
			printf("[MSC] File close fail \n");
			ret = 1;
		} else {
			printf("[MSC] File close success \n");
		}

		if (!ret) {
			filenum++;
		}
		ret = 0;
	}
exit_unmount:
	if (f_mount(NULL, logical_drv, 1) != FR_OK) {
		printf("[MSC] Fail to unmount logical drive\n");
	}
exit_unregister:
	if (FATFS_UnRegisterDiskDriver(drv_num)) {
		printf("[MSC] Fail to unregister disk driver from FATFS\n");
	}
exit_deinit:
	usbh_msc_deinit();
	usbh_deinit();
exit_free:
	if (buf) {
		rtos_mem_free(buf);
	}
exit:
	rtos_sema_delete(msc_attach_sema);
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_msc(void)
{
	int ret;
	rtos_task_t task;

	printf("\n[MSC] USB host MSC demo started...\n");

	ret = rtos_task_create(&task, "example_usbh_msc_thread", example_usbh_msc_thread, NULL, USBH_MSC_THREAD_STACK_SIZE, 2);
	if (ret != SUCCESS) {
		printf("\n[MSC] Fail to create USB host MSC thread\n");
	}
}

