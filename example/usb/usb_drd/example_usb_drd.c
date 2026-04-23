/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "os_wrapper.h"
#include "usbd.h"
#include "usbh.h"
#include "usbd_msc.h"
#include "usbh_msc.h"
#include "vfs_fatfs.h"
#include "ff.h"
#include "atcmd_service.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "DRD";

// USB speed
#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#error	"USB DRD example only supports USB high speed"
#endif

#define USB_DRD_SPEED							USB_SPEED_HIGH

// Thread priorities
#define USBH_MSC_RW_THREAD_PRIORITY				5U
#define USBH_MSC_THREAD_STACK_SIZE				(256*46)

#define USBH_MSC_TEST_BUF_SIZE					4096
#define USBH_MSC_TEST_ROUNDS					20
#define USBH_MSC_CHECK_DATA                     1

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void usbd_msc_cb_status_changed(u8 old_status, u8 status);
static int usbh_msc_cb_attach(void);
static int usbh_msc_cb_setup(void);
static int usbh_msc_cb_process(usb_host_t *host, u8 msg);

static void usbd_msc_cmd_test(u16 argc, char **argv);
static void usbh_msc_cmd_test(u16 argc, char **argv);

/* Private variables ---------------------------------------------------------*/

static usbd_config_t usbd_msc_cfg = {
	.speed = USB_DRD_SPEED,
	.isr_priority = INT_PRI_MIDDLE,
#if defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 708U,
	.ptx_fifo_depth = {16U, 256U, },
#elif defined (CONFIG_AMEBAPRO3)
	/*DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD*/
	.rx_fifo_depth = 1680U,
	.ptx_fifo_depth = {256U, 16U, 16U},
#endif
};

static usbd_msc_cb_t usbd_msc_cb = {
	.status_changed = usbd_msc_cb_status_changed
};

static rtos_sema_t usbh_msc_attach_sema;
static __IO int usbh_msc_is_rdy = 0;
static u8 file_cnt = 5;
static u8 *msc_wt_buf;
static u8 *msc_rd_buf;

static usbh_config_t usbh_cfg = {
	.speed = USB_DRD_SPEED,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_priority = 3U,
	.tick_source = USBH_SOF_TICK,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAPRO3)
	/*FIFO total depth is 2232 DWORD, resv 8 DWORD for DMA addr */
	.rx_fifo_depth = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_msc_cb_t usbh_msc_usr_cb = {
	.attach = usbh_msc_cb_attach,
	.setup = usbh_msc_cb_setup,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = usbh_msc_cb_process
};

/* Private functions ---------------------------------------------------------*/

static void usbd_msc_cb_status_changed(u8 old_status, u8 status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Device status change: %d->%d\n", old_status, status);
}

static int usbh_msc_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Host attach\n");
	rtos_sema_give(usbh_msc_attach_sema);
	return HAL_OK;
}

static int usbh_msc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Host setup\n");
	usbh_msc_is_rdy = 1;
	return HAL_OK;
}

static int usbh_msc_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		usbh_msc_is_rdy = 0;
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}

	return HAL_OK;
}

static void usbd_msc_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+USBDMSC=<command>\r\n");
	RTK_LOGI(NOTAG, "\t<command>:\tinit: init device msc driver\r\n");
	RTK_LOGI(NOTAG, "\t<command>:\tdeinit: deinit device msc driver\r\n");

}

static void usbd_msc_cmd_test(u16 argc, char **argv)
{
	int error_no = HAL_OK;
	int ret = HAL_OK;
	const char *cmd;

	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid USB argument\n");
		error_no = HAL_ERR_PARA;
		goto end;
	}

	cmd = (const char *)argv[1];

	if (_stricmp(cmd, "init") == 0) {
		ret = usbd_msc_disk_init();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init disk: %d\n", ret);
			return;
		}
		ret = usbd_init(&usbd_msc_cfg);
		if (ret != HAL_OK) {
			usbd_msc_disk_deinit();
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBD: %d\n", ret);
			return;
		}
		ret = usbd_msc_init(&usbd_msc_cb);
		if (ret != HAL_OK) {
			usbd_deinit();
			usbd_msc_disk_deinit();
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init MSC\n");
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "MSC device session start\n");
	} else if (_stricmp(cmd, "deinit") == 0) {
		usbd_msc_deinit();
		ret = usbd_deinit();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to deinit USBD\n");
		}
		ret = usbd_msc_disk_deinit();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to deinit disk\n");
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd err\n");
		error_no = HAL_ERR_PARA;
	}

end:
	if (error_no == HAL_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		usbd_msc_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

void usbh_msc_trx_test(void *param)
{
	FATFS fs;
	FIL f;
	int drv_num = 0;
	FRESULT res;
	char logical_drv[4];
	char path[64] = {'0'};
	int ret = 0;
	u32 filenum = 0;
	u32 br;
	u32 bw;
	u32 round = 0;
	u32 start;
	u32 elapse;
	u32 perf = 0;
	u32 test_sizes[] = {512, 1024, 2048, 4096, 8192};
	u32 test_size;

	u32 i;
	u8 data;

	UNUSED(param);

	msc_wt_buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	if (msc_wt_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc test buf\n");
		goto exit;
	}

	msc_rd_buf = (u8 *)rtos_mem_zmalloc(USBH_MSC_TEST_BUF_SIZE);
	if (msc_rd_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to alloc test buf\n");
		goto exit_free;
	}

	// Register USB disk driver to fatfs
	RTK_LOGS(TAG, RTK_LOG_INFO, "Register USB disk\n");
	drv_num = FATFS_RegisterDiskDriver(&USB_disk_Driver);
	if (drv_num < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to register\n");
		goto exit_free;
	}

	logical_drv[0] = drv_num + '0';
	logical_drv[1] = ':';
	logical_drv[2] = '/';
	logical_drv[3] = 0;

	RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R performance test start...\n");

	while (1) {
		if (usbh_msc_is_rdy) {
			rtos_time_delay_ms(10);
			break;
		}
	}

	if (f_mount(&fs, logical_drv, 1) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to mount logical drive\n");
		goto exit_unregister;
	}

	strcpy(path, logical_drv);
	while (1) {
		if (rtos_sema_take(usbh_msc_attach_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to take sema\n");
			goto exit_unmount;
		}

		while (1) {
			if (usbh_msc_is_rdy) {
				rtos_time_delay_ms(10);
				break;
			}
		}

next_file:
		sprintf(&path[3], "TEST%ld.DAT", filenum);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Open file: %s\n", path);
		/* Open test file */
		res = f_open(&f, path, FA_OPEN_ALWAYS | FA_READ | FA_WRITE);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to open file: TEST%d.DAT\n", filenum);
			goto exit_unmount;
		}
#if USBH_MSC_CHECK_DATA
		/* change write data */
		data = _rand() % 0xFF;
		memset(msc_wt_buf, data, USBH_MSC_TEST_BUF_SIZE);
#endif

		for (i = 0; i < sizeof(test_sizes) / sizeof(test_sizes[0]); ++i) {
			test_size = test_sizes[i];
			if (test_size > USBH_MSC_TEST_BUF_SIZE) {
				break;
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "W test: size %d, round %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_write(&f, (void *)msc_wt_buf, test_size, (UINT *)&bw);
				if (res || (bw < test_size)) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "W err bw=%d, rc=%d\n", bw, res);
					ret = 1;
					break;
				}
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, RTK_LOG_INFO, "W rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);

			/* Move the file pointer to the file head */
			res = f_lseek(&f, 0);

			RTK_LOGS(TAG, RTK_LOG_INFO, "R test: size = %d round = %d...\n", test_size, USBH_MSC_TEST_ROUNDS);
			start = SYSTIMER_TickGet();

			for (round = 0; round < USBH_MSC_TEST_ROUNDS; ++round) {
				res = f_read(&f, (void *)msc_rd_buf, test_size, (UINT *)&br);
				if (res || (br < test_size)) {
					f_lseek(&f, 0);
					RTK_LOGS(TAG, RTK_LOG_ERROR, "R err br=%d, rc=%d\n", br, res);
					ret = 1;
					break;
				}
			}

			elapse = SYSTIMER_GetPassTime(start);
			perf = (round * test_size * 10000 / 1024) / elapse;
			RTK_LOGS(TAG, RTK_LOG_INFO, "R rate %d.%d KB/s for %d round @ %d ms\n", perf / 10, perf % 10, round, elapse);

			/* Move the file pointer to the file head */
			res = f_lseek(&f, 0);

#if USBH_MSC_CHECK_DATA
			/* Check TRX data*/
			if (!(memcmp(msc_wt_buf, msc_rd_buf, test_size) == 0)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "WR%d check err: %x-%x-%x-%x vs %x-%x-%x-%x\n", test_size,
						 msc_wt_buf[0], msc_wt_buf[1], msc_wt_buf[test_size - 2], msc_wt_buf[test_size - 1],
						 msc_rd_buf[0], msc_rd_buf[1], msc_rd_buf[test_size - 2], msc_rd_buf[test_size - 1]);
				ret = HAL_ERR_HW;
				break;
			}
#endif
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "FatFS USB W/R performance test %s\n", (ret == 0) ? "done" : "abort");

		/* Close source file */
		res = f_close(&f);
		if (res) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "File close fail\n");
			ret = 1;
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "File close OK\n");
		}

		if ((!ret) && (++filenum < file_cnt)) {
			goto next_file;
		} else {
			break;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Test %d over: %d\n", filenum, ret);
exit_unmount:
	if (f_unmount(logical_drv) != FR_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to unmount logical drive\n");
	}
exit_unregister:
	if (FATFS_UnRegisterDiskDriver(drv_num)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to unregister disk driver from FATFS\n");
	}
exit_free:
	if (msc_rd_buf) {
		rtos_mem_free(msc_rd_buf);
	}
	if (msc_wt_buf) {
		rtos_mem_free(msc_wt_buf);
	}
exit:
	rtos_task_delete(NULL);
}

static void usbh_msc_help(void)
{
	RTK_LOGI(NOTAG, "\r\n");
	RTK_LOGI(NOTAG, "AT+USBHMSC=<command>[,<file_cnt>]\r\n");
	RTK_LOGI(NOTAG, "\t<command>:\tinit: init host msc driver\r\n");
	RTK_LOGI(NOTAG, "\t<command>:\tdeinit: deinit host msc driver\r\n");
	RTK_LOGI(NOTAG, "\t<command>:\trw_test: file read and write test\r\n");
	RTK_LOGI(NOTAG, "\t<file_cnt>:\trw test file cnt, default: 5\r\n");
}

static void usbh_msc_cmd_test(u16 argc, char **argv)
{
	int error_no = HAL_OK;
	int ret = HAL_OK;
	const char *cmd;
	usb_os_task_t task;

	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid USB argument\n");
		error_no = HAL_ERR_PARA;
		goto end;
	}

	cmd = (const char *)argv[1];

	if (_stricmp(cmd, "init") == 0) {
		rtos_sema_create(&usbh_msc_attach_sema, 0U, 1U);
		ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
		if (ret != HAL_OK) {
			rtos_sema_delete(usbh_msc_attach_sema);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBH: %d\n", ret);
			return;
		}
		ret = usbh_msc_init(&usbh_msc_usr_cb);
		if (ret != HAL_OK) {
			usbh_deinit();
			rtos_sema_delete(usbh_msc_attach_sema);
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init MSC: %d\n", ret);
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "MSC host session start\n");
	} else if (_stricmp(cmd, "deinit") == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Deinit MSC host driver\n");
		ret = usbh_msc_deinit();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to deinit MSC: %d\n", ret);
		}
		ret = usbh_deinit();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to deinit USBH: %d\n", ret);
		}
		rtos_sema_delete(usbh_msc_attach_sema);
	} else if (_stricmp(cmd, "rw_test") == 0) {
		if (argv[2]) {
			file_cnt = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}

		RTK_LOGS(TAG, RTK_LOG_INFO, "USB host MSC R&W test started\n");

		ret = rtos_task_create(&task, "usbh_msc_rw_test", usbh_msc_trx_test, NULL, USBH_MSC_THREAD_STACK_SIZE, USBH_MSC_RW_THREAD_PRIORITY);
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create USB host MSC R&W test thread\n");
		}
	} else {
		error_no = HAL_ERR_PARA;
	}
end:
	if (error_no == HAL_OK) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		usbh_msc_help();
		at_printf(ATCMD_ERROR_END_STR, error_no);
	}
}

/*
AT+USBDMSC=init
AT+USBDMSC=deinit
AT+USBHMSC=init
AT+USBHMSC=deinit
AT+USBHMSC=rw_test[, <file_cnt>]
*/
ATCMD_TABLE_DATA_SECTION
const log_item_t usb_drd_cmd_table[] = {
	{"+USBHMSC", usbh_msc_cmd_test},
	{"+USBDMSC", usbd_msc_cmd_test},
};

void example_usb_drd(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USB DRD demo start\n");
}

