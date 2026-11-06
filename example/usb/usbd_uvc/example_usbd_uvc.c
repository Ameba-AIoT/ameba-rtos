/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_uvc.h"
#include "os_wrapper.h"
#include "usb_app.h"
#include "example_usbd_uvc.h"
#include "example_usbd_uvc_sample_h264.h"

/* Private defines -----------------------------------------------------------*/

// Video parameters
#define USBD_UVC_VIDEO_BUF_NUM                   1U
#define USBD_UVC_VIDEO_FPS                       30U
#define USBD_UVC_H264_NAL_SIZE                   4U

// Thread priorities
#define UVC_INIT_THREAD_PRIORITY                 5

// Thread stack sizes
#define UVC_INIT_THREAD_STACK_SIZE               (256 * 4)

#define USBD_UVC_FRAME_INTERVAL_MS               (1000U / USBD_UVC_VIDEO_FPS)

/* Private types -------------------------------------------------------------*/

typedef struct video_array_s {
	const u8 *data;
	u32 data_len;
	u32 data_offset;
	u32 fps;
	u32 h264_nal_size;
	u32 size;
} video_array_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UVC";

static video_array_t video;
static usbd_uvc_buffer_t uvc_payload[USBD_UVC_VIDEO_BUF_NUM];

static const usbd_uvc_ep_cfg_t uvc_ep = {
	.iso_in_addr = USBD_UVC_ISO_IN_EP,
	.iso_in_xfer_size = USBD_UVC_IN_BUF_SIZE,
};

/* Start/stop state: the video thread streams on the running stack, example_usbd_uvc_stop() asks
   it to leave through quit and joins it */
static usb_app_ctx_t uvc_app;

static usbd_config_t uvc_cfg = {
	.info = {
		.prod_str = "Realtek UVC Device",
	},
	.isr_priority = INT_PRI_MIDDLE,
	/* Enable SOF interrupt: drives ISOC IN arming/recovery and the SCR SOF counter. */
	.ext_intr_enable = USBD_SOF_INTR,
#if defined (CONFIG_AMEBAPRO3)
	//DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD
	.rx_fifo_depth = 1168U,
	.ptx_fifo_depth = {16U, 768U, 16U, },
#endif
#if defined (CONFIG_AMEBAGREEN2)
	.rx_fifo_depth = 420U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, },
	/* RTL8721F TxFIFO is not deep enough to hold a whole HB ISOC IN packet; enable the
	   Periodic IN TX threshold so the core streams it out in chunks. */
	.isoc_use_ptx_threshold = 1U,
#endif
};

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static u32 array_get_h264_frame_size(const u8 *ptr_start, const u8 *ptr_end, u8 nal_len)
{
	const u8 *ptr = ptr_start;
	u8 skip_flag = 1U;

	if ((ptr_start >= ptr_end) || ((nal_len != 3U) && (nal_len != 4U))) {
		return 0U;
	}

	while ((ptr + nal_len) < ptr_end) {
		if ((ptr[0] == 0U) && (ptr[1] == 0U)) {
			if (((nal_len == 4U) && (ptr[2] == 0U) && (ptr[3] == 1U))
				|| ((nal_len == 3U) && (ptr[2] == 1U))) {
				if (((ptr[nal_len] & 0x1fU) != 0x07U) && ((ptr[nal_len] & 0x1fU) != 0x08U)) {
					if (skip_flag == 0U) {
						return (u32)(ptr - ptr_start);
					} else {
						skip_flag = 0U;
					}
				} else if ((ptr[nal_len] & 0x1fU) == 0x08U) {
					skip_flag = 1U;
				}
			}
		}
		ptr++;
	}

	return (u32)(ptr_end - ptr_start);
}

static void example_usbd_uvc_video_thread(void *param)
{
	(void)param;
	video_array_t *video_data = &video;
	usbd_uvc_buffer_t *payload = NULL;

	while (1) {
		rtos_time_delay_ms(USBD_UVC_FRAME_INTERVAL_MS);
		if (uvc_app.quit != 0U) {
			break;
		}
		if (usbd_uvc_get_status() == 0) {
			continue;
		}

		if (video_data->data_offset >= video_data->data_len) {
			video_data->data_offset = 0;
		}

		video_data->size = array_get_h264_frame_size(video_data->data + video_data->data_offset,
						   video_data->data + video_data->data_len, video_data->h264_nal_size);
		if (video_data->size == 0U) {
			video_data->data_offset = 0;
			continue;
		}

		do {
			payload = usbd_uvc_video_out_stream_queue();
			if (payload == NULL) {
				if (uvc_app.quit != 0U) {
					break;
				}
				rtos_time_delay_ms(1);
			}
		} while (payload == NULL);
		if (payload == NULL) {
			/* Stop requested while waiting for a free buffer */
			break;
		}

		payload->mem = video_data->data + video_data->data_offset;
		payload->bytesused = video_data->size;
		video_data->data_offset += video_data->size;

		usbd_uvc_video_put_in_stream_queue(payload);
		/* Woken by the frame task once the frame is queued, or by example_usbd_uvc_stop() */
		usbd_uvc_wait_frame_down();
		if (uvc_app.quit != 0U) {
			break;
		}
	}

	/* Stop requested: the stack is still up, example_usbd_uvc_stop() tears it down */
	usb_app_ctx_leave(&uvc_app);
	rtos_task_delete(NULL);
}

/**
  * @brief  Bring the stack and the video thread up
  * @retval RTK_SUCCESS, or the first error, with everything already created rolled back
  */
static int uvc_start(void)
{
	int ret = RTK_SUCCESS;
	rtos_task_t task;
	u32 i = 0U;

	ret = usb_app_ctx_begin(&uvc_app);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	ret = usbd_init(&uvc_cfg);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBD fail\n");
		goto exit_usbd_init_fail;
	}

	ret = usbd_uvc_init(&uvc_ep);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init UVC class fail\n");
		goto exit_usbd_uvc_init_fail;
	}

	video.data = h264_sample;
	video.data_len = (u32)h264_sample_len;
	video.data_offset = 0;
	video.fps = USBD_UVC_VIDEO_FPS;
	video.h264_nal_size = USBD_UVC_H264_NAL_SIZE;

	for (i = 0U; i < USBD_UVC_VIDEO_BUF_NUM; i++) {
		/* After a stop the buffer may still link a queue of the previous run, which the class
		   reset: detach it, or put_out_stream_queue() skips it and no buffer is ever free */
		INIT_LIST_HEAD(&uvc_payload[i].buffer_list);
		usbd_uvc_video_put_out_stream_queue(&uvc_payload[i]);
	}

	ret = rtos_task_create(&task, "usbd_uvc_video_thread", example_usbd_uvc_video_thread, NULL, UVC_INIT_THREAD_STACK_SIZE,
						   UVC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD UVC video thread fail\n");
		goto exit_create_video_task_fail;
	}

	return RTK_SUCCESS;

exit_create_video_task_fail:
exit_usbd_uvc_init_fail:
	usbd_uvc_deinit();
	usbd_deinit();
exit_usbd_init_fail:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "USBD UVC demo aborted\n");
	usb_app_ctx_end(&uvc_app);

	return ret;
}

static void example_usbd_uvc_thread(void *param)
{
	UNUSED(param);

	if (uvc_start() != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start fail\n");
	}
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Start the UVC device, used by the USB app manager
  * @note   Task context only
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if already running, or the init error
  */
int example_usbd_uvc_start(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD UVC demo start\n");

	return uvc_start();
}

/**
  * @brief  Stop the UVC device and release everything it owns
  * @note   Task context only. Safe to call when not running.
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if the video thread did not leave in time
  */
int example_usbd_uvc_stop(void)
{
	if (usb_app_ctx_claim_stop(&uvc_app) == 0U) {
		/* Not running */
		return usb_app_ctx_wait_idle(&uvc_app);
	}

	/* The video thread may wait in usbd_uvc_wait_frame_down() for a frame the host no longer
	   pulls: wake it while the semaphore still exists. The count left behind when it is not
	   waiting there makes its next wait return at once, it then observes quit and leaves. */
	usb_os_sema_give(get_private_usbd_uvcd()->video.output_frame_sema);
	if (usb_app_ctx_join(&uvc_app, 1U) != RTK_SUCCESS) {
		/* The video thread is still on the stack: keep everything, the app stays STOPPING */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop timeout\n");
		return -RTK_ERR_TIMEOUT;
	}

	/* The video thread is gone: this caller is the only one left on the stack */
	usbd_uvc_deinit();
	usbd_deinit();
	usb_app_ctx_end(&uvc_app);
	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD UVC demo stop\n");

	return RTK_SUCCESS;
}

/**
  * @brief  Example entry: start the UVC device from its own thread
  * @retval None
  */
void example_usbd_uvc(void)
{
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD UVC demo start\n");

	ret = rtos_task_create(&task, "usbd_uvc_thread", example_usbd_uvc_thread, NULL, UVC_INIT_THREAD_STACK_SIZE,
						   UVC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD UVC thread fail\n");
	}
}
