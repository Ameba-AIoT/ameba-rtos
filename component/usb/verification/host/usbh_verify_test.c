/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */
#include "ameba_soc.h"
#include "usbh.h"
#include "usbh_verify.h"

/* Private defines -----------------------------------------------------------*/
#define CONFIG_USBH_VERIFY_HOT_PLUG_TEST 0     /* Hot plug / memory leak test */

/* Private types -------------------------------------------------------------*/
static const char *const TAG = "USBH";

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int cmd_usbh_verify_cb_detach(void);
static int cmd_usbh_verify_cb_setup(void);
static int cmd_usbh_verify_cb_process(usb_host_t *host, u8 id);
/* Private variables ---------------------------------------------------------*/

static usb_os_sema_t usbh_verify_detach_sema;
static __IO u8 usbh_verify_ready = 0;

static usbh_config_t cmd_usbh_verify_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1,
	.ext_intr_en = USBH_SOF_INTR,
	.sof_tick_en = 1,
	.main_task_priority = 5U,
	.isr_task_priority  = 6U,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBASMARTPLUS)
	/*FIFO total depth is 1280 DWORD, reserve 14 DWORD for DMA addr*/
	.rx_fifo_depth = 754,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_verify_cb_t cmd_usbh_verify_cb = {
	.detach = cmd_usbh_verify_cb_detach,
	.setup  = cmd_usbh_verify_cb_setup,
};

static usbh_user_cb_t cmd_usbh_usr_cb = {
	.process = cmd_usbh_verify_cb_process
};

usbh_verify_xfer_dir_t usbh_verify_xfer;
u8 usbh_fpga_no_sof;
/* Private functions ---------------------------------------------------------*/
static int cmd_usbh_verify_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_VERIFY_HOT_PLUG_TEST
	usb_os_sema_give(usbh_verify_detach_sema);
#endif
	return HAL_OK;
}
static int cmd_usbh_verify_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	usbh_verify_ready = 1;
	return HAL_OK;
}
static int cmd_usbh_verify_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		usbh_verify_ready = 0;
		break;
	case USBH_MSG_CONNECTED:
		break;
	default:
		break;
	}

	return HAL_OK;
}

#if CONFIG_USBH_VERIFY_HOT_PLUG_TEST
static void cmd_usbh_verify_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		usb_os_sema_take(usbh_verify_detach_sema, USB_OS_SEMA_TIMEOUT);
		usbh_verify_deinit();
		usbh_deinit();

		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap size 0x%x\n", rtos_mem_get_free_heap_size());

		ret = usbh_init(&cmd_usbh_verify_cfg, &cmd_usbh_usr_cb);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBH controller %d\n", ret);
			break;
		}

		ret = usbh_verify_init(&cmd_usbh_verify_cb);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBH verify driver %d\n", ret);
			usbh_deinit();
			break;
		}
	}

	rtos_task_delete(NULL);
}
#endif

static int usbh_init_flag = 0;
static void cmd_usbh_verify_init(void)
{
	int status;
#if CONFIG_USBH_VERIFY_HOT_PLUG_TEST
	usb_os_task_t task;
#endif
	if (usbh_init_flag) {
		return;
	}
	usbh_init_flag = 1;
	usb_os_sema_create(&usbh_verify_detach_sema);
	status = usbh_init(&cmd_usbh_verify_cfg, &cmd_usbh_usr_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBH controller %d\n", status);
		goto error_exit;
	}

	status = usbh_verify_init(&cmd_usbh_verify_cb);
	if (status < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USBH verify driver %d\n", status);
		usbh_deinit();
		goto error_exit;
	}

#if CONFIG_USBH_VERIFY_HOT_PLUG_TEST
	status = rtos_task_create(&task, "usbh_verify_hotplug_thread", cmd_usbh_verify_hotplug_thread, NULL, 1024U * 2, 7U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to create hotplug thread\n");
		usbh_verify_deinit();
		usbh_deinit();
		goto error_exit;
	}
#endif

	goto example_exit;

error_exit:
	usb_os_sema_delete(usbh_verify_detach_sema);
example_exit:
	return;
}
static void cmd_usbh_verify_deinit(void)
{
	if (usbh_init_flag == 0) {
		return;
	}
	usbh_verify_deinit();
	usbh_deinit();

	usbh_init_flag = 0;
}

static u8 usbh_verify_get_xfer_type(char *text)
{
	u8 type;
	if (_stricmp(text, "ctrl") == 0) {
		type = USB_CH_EP_TYPE_CTRL;
	} else if (_stricmp(text, "bulk") == 0) {
		type = USB_CH_EP_TYPE_BULK;
	} else if (_stricmp(text, "intr") == 0) {
		type = USB_CH_EP_TYPE_INTR;
	} else if (_stricmp(text, "isoc") == 0) {
		type = USB_CH_EP_TYPE_ISOC;
	} else {
		type = 0xF;
	}

	return type;
}

static u16 usbh_verify_get_xfer_mps_size(u8 ep_type, u16 buf_size)
{
	u16 mps_size;
	switch (ep_type) {
	case USB_CH_EP_TYPE_CTRL:
		if ((buf_size) && (cmd_usbh_verify_cfg.speed == (u8)USB_SPEED_HIGH)) {
			//Len cann't larger than usbd buffer size defined in usbd_chip_info.h
			mps_size = MIN(buf_size, 512); //usbd ctrl buffer size is 512
		} else if ((buf_size) && (cmd_usbh_verify_cfg.speed != (u8)USB_SPEED_HIGH)) {
			mps_size = MIN(buf_size, 256);
		} else {
			mps_size = USB_CTRL_MAX_MPS;
		}
		break;
	case USB_CH_EP_TYPE_BULK:
		if ((buf_size) && (cmd_usbh_verify_cfg.speed == (u8)USB_SPEED_HIGH))  {
			mps_size = MIN(buf_size, 2048); //usbd bulk buffer size is 2048
		} else if ((buf_size) && (cmd_usbh_verify_cfg.speed != (u8)USB_SPEED_HIGH)) {
			mps_size = MIN(buf_size, 512);
		} else {
			mps_size = (cmd_usbh_verify_cfg.speed == (u8)USB_SPEED_HIGH) ? USB_BULK_HS_MAX_MPS : USB_BULK_FS_MAX_MPS;
		}
		break;
	case USB_CH_EP_TYPE_INTR:
		if ((buf_size) && (cmd_usbh_verify_cfg.speed == (u8)USB_SPEED_HIGH))  {
			mps_size = MIN(buf_size, USB_INTR_HS_MAX_MPS);
		} else if ((buf_size) && (cmd_usbh_verify_cfg.speed != (u8)USB_SPEED_HIGH)) {
			mps_size = MIN(buf_size, USB_INTR_FS_MAX_MPS);
		} else {
			mps_size = (cmd_usbh_verify_cfg.speed == (u8)USB_SPEED_HIGH) ? USB_INTR_HS_MAX_MPS : USB_INTR_FS_MAX_MPS;
		}
		break;
	case USB_CH_EP_TYPE_ISOC:
		if ((buf_size) && (cmd_usbh_verify_cfg.speed == (u8)USB_SPEED_HIGH))  {
			mps_size = MIN(buf_size, USB_ISOC_HS_MAX_MPS);
		} else if ((buf_size) && (cmd_usbh_verify_cfg.speed != (u8)USB_SPEED_HIGH)) {
			mps_size = MIN(buf_size, USB_ISOC_FS_MAX_MPS);
		} else {
			mps_size = (cmd_usbh_verify_cfg.speed == (u8)USB_SPEED_HIGH) ? USB_ISOC_HS_MAX_MPS : USB_ISOC_FS_MAX_MPS;
		}
		break;
	default:
		mps_size = 0;
		break;
	}

	return mps_size;
}
static void  usbh_verify_mix_xfer_test(u16 argc, u8 *argv[], u16 size)
{
	UNUSED(argc);
	char *type1, *type2, *type3, *term;
	u16 len1, len2, len3, len4, pktcnt;
	u8 ep_type1, ep_type2, ep_type3;
	u8 mix = 2;
	if (argv[4]) {
		mix = (u16)_strtoul((const char *)(argv[4]), (char **)NULL, 10);
	}

	switch (mix) {
	case 4:
		if (argv[5]) {
			len1 = len2 = len3 = len4 = size;
			term = (char *)argv[5];//usbh verify start mix <size> 4 <short/mps>
			if (_stricmp(term, "short") == 0) {
				len1 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_CTRL, 0) - 1;
				len2 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_BULK, 0) - 1;
				len3 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_INTR, 0) - 1;
				len4 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_ISOC, 0) - 1;
			} else if (_stricmp(term, "mps") == 0) {
				len1 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_CTRL, 0);
				len2 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_BULK, 0);
				len3 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_INTR, 0);
				len4 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_ISOC, 0);
			}
		} else {
			len1 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_CTRL, size);
			len2 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_BULK, size);
			len3 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_INTR, size);
			len4 = usbh_verify_get_xfer_mps_size(USB_CH_EP_TYPE_ISOC, size);
		}
		usbh_verify_transfer_by_eptype(len1, USB_CH_EP_TYPE_CTRL);
		usbh_verify_transfer_by_eptype(len2, USB_CH_EP_TYPE_BULK);
		usbh_verify_transfer_by_eptype(len3, USB_CH_EP_TYPE_INTR);
		usbh_verify_transfer_by_eptype(len4, USB_CH_EP_TYPE_ISOC);
		break;
	case 3:
		if (argv[5] && argv[6] && argv[7]) {
			type1 = (char *)argv[5];
			type2 = (char *)argv[6];
			type3 = (char *)argv[7];
			ep_type1 = usbh_verify_get_xfer_type(type1);
			ep_type2 = usbh_verify_get_xfer_type(type2);
			ep_type3 = usbh_verify_get_xfer_type(type3);
			len1 = len2 = len3 = size;

			if (argv[8]) {//usbh verify start mix <size> 3 type1 type2 type3 <short/mps>
				term = (char *)argv[8];
				if (_stricmp(term, "short") == 0) {
					len1 = usbh_verify_get_xfer_mps_size(ep_type1, 0) - 1;
					len2 = usbh_verify_get_xfer_mps_size(ep_type2, 0) - 1;
					len3 = usbh_verify_get_xfer_mps_size(ep_type3, 0) - 1;
				} else if (_stricmp(term, "mps") == 0) {
					len1 = usbh_verify_get_xfer_mps_size(ep_type1, 0);
					len2 = usbh_verify_get_xfer_mps_size(ep_type2, 0);
					len3 = usbh_verify_get_xfer_mps_size(ep_type3, 0);
				}
			} else { //usbh verify start mix <size> 3 type1 type2 type3
				len1 = usbh_verify_get_xfer_mps_size(ep_type1, size);
				len2 = usbh_verify_get_xfer_mps_size(ep_type2, size);
				len3 = usbh_verify_get_xfer_mps_size(ep_type3, size);
			}
			usbh_verify_transfer_by_eptype(len1, ep_type1);
			usbh_verify_transfer_by_eptype(len2, ep_type2);
			usbh_verify_transfer_by_eptype(len3, ep_type3);
		}
		break;
	case 2:
		if (argv[5] && argv[6]) {
			type1 = (char *)argv[5];
			type2 = (char *)argv[6];
			ep_type1 = usbh_verify_get_xfer_type(type1);
			ep_type2 = usbh_verify_get_xfer_type(type2);
			len1 = len2 = size;

			if (argv[7]) {//usbh verify start mix <size> 2 type1 type2 <short/mps> <pktcnt>
				term = (char *)argv[7];
				pktcnt = 1;
				if (argv[8]) {//Len cann't larger than usbd buffer size defined in usbd_chip_info.h
					pktcnt = (u16)_strtoul((const char *)(argv[8]), (char **)NULL, 10);
				}
				if (_stricmp(term, "short") == 0) {
					len1 = pktcnt * usbh_verify_get_xfer_mps_size(ep_type1, 0) - 1;
					len2 = pktcnt * usbh_verify_get_xfer_mps_size(ep_type2, 0) - 1;
				} else if (_stricmp(term, "mps") == 0) {
					len1 = pktcnt * usbh_verify_get_xfer_mps_size(ep_type1, 0);
					len2 = pktcnt * usbh_verify_get_xfer_mps_size(ep_type2, 0);
				}
			} else {//usbh verify start mix <size> 2 type1 type2
				len1 = usbh_verify_get_xfer_mps_size(ep_type1, size);
				len2 = usbh_verify_get_xfer_mps_size(ep_type2, size);
			}

			usbh_verify_transfer_by_eptype(len1, ep_type1);
			usbh_verify_transfer_by_eptype(len2, ep_type2);
		}
		break;
	default:
		break;
	}
}

/* Exported functions --------------------------------------------------------*/
static void usbh_verify_xfer_dir(u8 *argv[])
{
	const char *cmd = (const char *)argv[2];

	if (_stricmp(cmd, "p_loop") == 0) {
		usbh_verify_xfer.p_loopback = 1;
		if (argv[3]) {
			usbh_verify_xfer.p_loopback = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbh_verify_xfer.p_loopback) {
			usbh_verify_xfer.p_in_only = 0;
			usbh_verify_xfer.p_out_only = 0;
		}
	} else if (_stricmp(cmd, "p_in") == 0) {
		usbh_verify_xfer.p_in_only = 1;
		if (argv[3]) {
			usbh_verify_xfer.p_in_only = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbh_verify_xfer.p_in_only) {
			usbh_verify_xfer.p_loopback = 0;
			usbh_verify_xfer.p_out_only = 0;
		}
	} else if (_stricmp(cmd, "p_out") == 0) {
		usbh_verify_xfer.p_out_only = 1;
		if (argv[3]) {
			usbh_verify_xfer.p_out_only = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbh_verify_xfer.p_out_only) {
			usbh_verify_xfer.p_loopback = 0;
			usbh_verify_xfer.p_in_only = 0;
		}
	} else if (_stricmp(cmd, "bulk_loop") == 0) {
		usbh_verify_xfer.bulk_loopback = 1;
		if (argv[3]) {
			usbh_verify_xfer.bulk_loopback = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbh_verify_xfer.bulk_loopback) {
			usbh_verify_xfer.bulk_in_only = 0;
			usbh_verify_xfer.bulk_out_only = 0;
		}
	} else if (_stricmp(cmd, "bulk_in") == 0) {
		usbh_verify_xfer.bulk_in_only = 1;
		if (argv[3]) {
			usbh_verify_xfer.bulk_in_only = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbh_verify_xfer.bulk_in_only) {
			usbh_verify_xfer.bulk_loopback = 0;
			usbh_verify_xfer.bulk_out_only = 0;
		}
	} else if (_stricmp(cmd, "bulk_out") == 0) {
		usbh_verify_xfer.bulk_out_only = 1;
		if (argv[3]) {
			usbh_verify_xfer.bulk_out_only = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbh_verify_xfer.bulk_out_only) {
			usbh_verify_xfer.bulk_loopback = 0;
			usbh_verify_xfer.bulk_in_only = 0;
		}
	} else if (_stricmp(cmd, "ctrl_in") == 0) {
		usbh_verify_xfer.ctrl_in_only = 1;
		if (argv[3]) {
			usbh_verify_xfer.ctrl_in_only = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbh_verify_xfer.ctrl_in_only) {
			usbh_verify_xfer.ctrl_out_only = 0;
		}
	} else if (_stricmp(cmd, "ctrl_out") == 0) {
		usbh_verify_xfer.ctrl_out_only = 1;
		if (argv[3]) {
			usbh_verify_xfer.ctrl_out_only = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbh_verify_xfer.ctrl_out_only) {
			usbh_verify_xfer.ctrl_in_only = 0;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong cmd %s\n", cmd);
		RTK_LOGS(TAG, RTK_LOG_INFO, "P_loopback: %d(%d-%d)\n",
				 usbh_verify_xfer.p_loopback, usbh_verify_xfer.p_in_only, usbh_verify_xfer.p_out_only);
		RTK_LOGS(TAG, RTK_LOG_INFO, "BULK_loopback: %d(%d-%d)\n",
				 usbh_verify_xfer.bulk_loopback, usbh_verify_xfer.bulk_in_only, usbh_verify_xfer.bulk_out_only);
		RTK_LOGS(TAG, RTK_LOG_INFO, "CTRL: %d-%d\n", usbh_verify_xfer.ctrl_in_only, usbh_verify_xfer.ctrl_out_only);
	}
}

/*
	  usbh verify cmd:

	  usbh verify loopcount <times>
	  usbh verify dma dis/en(default)
	  usbh verify speed full/high(default)
	  usbh verify init

	  usbh verify xfer p_loop/bulk_loop <0/1> //loopback or not, default loopback
	  usbh verify xfer ctrl_out/bulk_out/p_out <0/1> //OUT only
	  usbh verify xfer ctrl_in/bulk_in/p_in   <0/1> //IN only

	  usbh verify start all/bulk/isoc/... size
	  usbh verify start mix size 2 type1 type2 <short/mps> <pktcnt>
	  usbh verify start mix size 3 type1 type2 type3 <short/mps>
	  usbh verify start mix size 4 <short/mps>
	  usbh verify stop
	  usbh verify dump
*/
int cmd_usbh_verify_test_entry(u16 argc, u8 *argv[])
{
	(void) argc;	/* To avoid gcc warnings */
	int ret = TRUE;
	const char *sub_cmd = (const char *)argv[1];
	if (sub_cmd) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Enter usbh cmd \"%s\"\n", sub_cmd);
	}
	/*select interface test mode*/
	if (_stricmp((const char *)sub_cmd, "init") == 0) {
		cmd_usbh_verify_init();
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBH init test finish\n");
		usbh_verify_ep_status_start();
	} else if (_stricmp((const char *)sub_cmd, "loopcount") == 0) {
		u32 count = 100;
		if (argv[2]) {
			count = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}
		usbh_verify_set_test_count_max(count);
		RTK_LOGS(TAG, RTK_LOG_INFO, "USBH init test count set to %d\n", count);
	} else if (_stricmp(sub_cmd, "dma") == 0) {
		u8 dma_enable = 0;
		if (argv[2] && (_stricmp((const char *)argv[2], (const char *)"en") == 0)) {
			dma_enable = 1;
		} else if (argv[2] && (_stricmp((const char *)argv[2], (const char *)"dis") == 0)) {
			dma_enable = 0;
		}
		cmd_usbh_verify_cfg.dma_enable = dma_enable;
	} else if (_stricmp(sub_cmd, "speed") == 0) {
		u8 speed = (u8)USB_SPEED_HIGH;
		if (argv[2] && (_stricmp((const char *)argv[2], (const char *)"full") == 0)) {
			speed = (u8)USB_SPEED_HIGH_IN_FULL;
		}
		cmd_usbh_verify_cfg.speed = speed;
	} else if (_stricmp(sub_cmd, "sof_tick") == 0) {
		cmd_usbh_verify_cfg.sof_tick_en  = 1;
		if (argv[2]) {
			cmd_usbh_verify_cfg.sof_tick_en = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
		}
	} else if (_stricmp(sub_cmd, "xfer") == 0) {
		usbh_verify_xfer_dir(argv);
	} else if (_stricmp(sub_cmd, "start") == 0) {
		const char *type = (const char *)argv[2];
		u16 size = 10;
		if (argv[3]) {
			size = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "Test ep packet size %d\n", size);
		if (type == NULL || (_stricmp(type, "all") == 0)) {
			usbh_verify_all_ep_transfer(size);
		} else if (_stricmp(type, "ctrl") == 0) {
			usbh_verify_transfer_by_eptype(size, USB_CH_EP_TYPE_CTRL);
		} else if (_stricmp(type, "bulk") == 0) {
			usbh_verify_transfer_by_eptype(size, USB_CH_EP_TYPE_BULK);
		} else if (_stricmp(type, "isoc") == 0) {
			usbh_verify_transfer_by_eptype(size, USB_CH_EP_TYPE_ISOC);
		} else if (_stricmp(type, "intr") == 0) {
			usbh_verify_transfer_by_eptype(size, USB_CH_EP_TYPE_INTR);
		} else if (_stricmp(type, "mix") == 0) {
			usbh_verify_mix_xfer_test(argc, argv, size);
		} else if (_stricmp(type, "looprun") == 0) {
			usbh_verify_looprun_test(size);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "USBH start enter wrong cmd %s\n", type);
			ret = FALSE;
		}
		usbh_verify_start_process();
	} else if (_stricmp(sub_cmd, "stop") == 0) {
		usbh_verify_longrun_thread_stop();
		usbh_verify_ep_status_stop();
		cmd_usbh_verify_deinit();
	} else if (_stricmp(sub_cmd, "dump") == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "DMA: %d\n", cmd_usbh_verify_cfg.dma_enable);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Speed: %d\n", cmd_usbh_verify_cfg.speed);
		RTK_LOGS(TAG, RTK_LOG_INFO, "ExtIntr: %d\n", cmd_usbh_verify_cfg.ext_intr_en);
		RTK_LOGS(TAG, RTK_LOG_INFO, "SofTick: %d\n", cmd_usbh_verify_cfg.sof_tick_en);
		RTK_LOGS(TAG, RTK_LOG_INFO, "P_loopback: %d(%d-%d)\n",
				 usbh_verify_xfer.p_loopback, usbh_verify_xfer.p_in_only, usbh_verify_xfer.p_out_only);
		RTK_LOGS(TAG, RTK_LOG_INFO, "BULK_loopback: %d(%d-%d)\n",
				 usbh_verify_xfer.bulk_loopback, usbh_verify_xfer.bulk_in_only, usbh_verify_xfer.bulk_out_only);
		RTK_LOGS(TAG, RTK_LOG_INFO, "CTRL: %d-%d\n", usbh_verify_xfer.ctrl_in_only, usbh_verify_xfer.ctrl_out_only);
		usbh_verify_ep_debug_dump();
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Wrong cmd %s\n", sub_cmd);
		ret = FALSE;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Exit usbh cmd\n");
	return ret;
}

