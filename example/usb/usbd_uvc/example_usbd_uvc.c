/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "usbd.h"
#include "usbd_uvc.h"
#include "os_wrapper.h"
#include "sample_h264.h"

/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "UVC";

// Thread priorities
#define CONFIG_USBD_UVC_INIT_THREAD_PRIORITY                5U

// Thread stack sizes
#define CONFIG_USBD_UVC_INIT_THREAD_STACK_SIZE              (256 * 4)

// Video parameters
#define USBD_UVC_VIDEO_BUF_NUM                              1U
#define USBD_UVC_VIDEO_FPS                                  30U
#define USBD_UVC_H264_NAL_SIZE                              4U

/* Private types -------------------------------------------------------------*/

typedef struct video_array_s {
	const unsigned char *data;
	uint32_t data_len;
	uint32_t data_offset;
	uint32_t fps;
	uint32_t h264_nal_size;
	uint32_t size;
} video_array_t;

/* Private macros ------------------------------------------------------------*/

#define USBD_UVC_FRAME_INTERVAL_MS                          (1000U / USBD_UVC_VIDEO_FPS)

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static video_array_t video;
static usbd_uvc_buffer_t uvc_payload[USBD_UVC_VIDEO_BUF_NUM];

static usbd_config_t uvc_cfg = {
	.speed = USB_SPEED_HIGH,
	.isr_priority = INT_PRI_MIDDLE,
#if defined (CONFIG_AMEBAPRO3)
	//DFIFO total 2232 DWORD, resv 8 DWORD for DMA addr and EP0 fixed 256 DWORD
	.rx_fifo_depth = 1168U,
	.ptx_fifo_depth = {256U, 256U, 256U, },
#endif
};

static uint32_t array_get_h264_frame_size(const unsigned char *ptr_start, const unsigned char *ptr_end, uint8_t nal_len)
{
	if ((ptr_start >= ptr_end) || ((nal_len != 3U) && (nal_len != 4U))) {
		return 0;
	}

	int skip_flag = 1;
	const unsigned char *ptr = ptr_start;
	while ((ptr + nal_len) < ptr_end) {
		if ((ptr[0] == 0U) && (ptr[1] == 0U)) {
			if (((nal_len == 4U) && (ptr[2] == 0U) && (ptr[3] == 1U))
				|| ((nal_len == 3U) && (ptr[2] == 1U))) {
				if (((ptr[nal_len] & 0x1fU) != 0x07U) && ((ptr[nal_len] & 0x1fU) != 0x08U)) {
					if (skip_flag == 0) {
						return (uint32_t)(ptr - ptr_start);
					} else {
						skip_flag = 0;
					}
				} else if ((ptr[nal_len] & 0x1fU) == 0x08U) {
					skip_flag = 1;
				}
			}
		}
		ptr++;
	}

	return (uint32_t)(ptr_end - ptr_start);
}

static void example_usbd_uvc_video_thread(void *param)
{
	(void)param;
	video_array_t *video_data = &video;
	usbd_uvc_buffer_t *payload = NULL;

	while (1) {
		rtos_time_delay_ms(USBD_UVC_FRAME_INTERVAL_MS);
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
				rtos_time_delay_ms(1);
			}
		} while (payload == NULL);

		payload->mem = video_data->data + video_data->data_offset;
		payload->bytesused = video_data->size;
		video_data->data_offset += video_data->size;

		usbd_uvc_video_put_in_stream_queue(payload);
		usbd_uvc_wait_frame_down();
	}
}

static void example_usbd_uvc_thread(void *param)
{
	int ret = 0;
	rtos_task_t task;

	UNUSED(param);

	ret = usbd_init(&uvc_cfg);
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBD fail\n");
		goto exit_usbd_init_fail;
	}

	ret = usbd_uvc_init();
	if (ret != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init UVC class fail\n");
		goto exit_usbd_uvc_init_fail;
	}

	video.data = h264_sample;
	video.data_len = (uint32_t)h264_sample_len;
	video.data_offset = 0;
	video.fps = USBD_UVC_VIDEO_FPS;
	video.h264_nal_size = USBD_UVC_H264_NAL_SIZE;

	for (uint32_t i = 0; i < USBD_UVC_VIDEO_BUF_NUM; i++) {
		usbd_uvc_video_put_out_stream_queue(&uvc_payload[i]);
	}

	ret = rtos_task_create(&task, "example_usbd_uvc_video_thread", example_usbd_uvc_video_thread, NULL, CONFIG_USBD_UVC_INIT_THREAD_STACK_SIZE,
						   CONFIG_USBD_UVC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD UVC video thread fail\n");
		goto exit_create_video_task_fail;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD UVC demo start\n");
	rtos_task_delete(NULL);
	return;

exit_create_video_task_fail:
exit_usbd_uvc_init_fail:
	usbd_uvc_deinit();
	usbd_deinit();
exit_usbd_init_fail:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbd_uvc(void)
{
	int ret;
	rtos_task_t task;

	ret = rtos_task_create(&task, "example_usbd_uvc_thread", example_usbd_uvc_thread, NULL, CONFIG_USBD_UVC_INIT_THREAD_STACK_SIZE,
						   CONFIG_USBD_UVC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create USBD UVC thread fail\n");
	}
}
