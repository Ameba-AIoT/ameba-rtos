/**
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */

#include "example_usbh_uvc.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "usbh_uvc_intf.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
static const char *TAG = "UVC";
/*Just capture and abandon frame*/
#define USBH_UVC_APP_SIMPLE	1

/*Capture frame and write it to SD card through vfs*/
#define USBH_UVC_APP_VFS	2

/*Capture frame and write it to http server*/
#define USBH_UVC_APP_HTTPC	3

/*Choose which application example*/
#define CONFIG_USBH_UVC_APP  USBH_UVC_APP_SIMPLE

#define CONFIG_USBH_UVC_CHECK_IMAGE_DATA  0

/* Private includes -------------------------------------------------------------*/

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
#include "vfs.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#include "httpc/httpc.h"
#include "rtw_wifi_defs.h"
#include "lwipconf.h"
#include "rtw_wifi_defs.h"
#endif

/* Private macros ------------------------------------------------------------*/

#define USBH_UVC_BUF_SIZE       UVC_VIDEO_FRAME_SIZE   // Frame buffer size, resident in PSRAM, depends on format type
//resolution and compression ratio
#define USBH_UVC_FORMAT_TYPE    UVC_FORMAT_MJPEG
#define USBH_UVC_WIDTH          1280
#define USBH_UVC_HEIGHT         720
#define USBH_UVC_FRAME_RATE     30

#define USBH_UVC_IF_NUM_0                0     // most cameras have only one video stream interface 

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
#define USBH_UVC_VFS_WRITE_SIZE          (16 * 1024)
#define USBH_UVC_VFS_VIDEO_SIZE          (2 * 1024 * 1024)
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#define USBH_UVC_HTTPC_WRITE_SIZE      (4 * 1024)
#define USBH_UVC_HTTPC_VIDEO_SIZE      (2 * 1024 * 1024)
#define USBH_UVC_HTTPC_SERVER          "192.168.1.102"
#define USBH_UVC_HTTPC_SECURE           HTTPC_SECURE_NONE
#define USBH_UVC_HTTPC_PORT             5090
#define USBH_UVC_HTTP_TAG              "HTTP"
#endif

/* Private function prototypes -----------------------------------------------*/

static int uvc_cb_init(void);
static int uvc_cb_deinit(void);
static int uvc_cb_attach(void);
static int uvc_cb_detach(void);

/* Private variables ---------------------------------------------------------*/

static rtos_sema_t uvc_conn_sema;
static rtos_sema_t uvc_disconn_sema;
static rtos_mutex_t uvc_buf_mutex = NULL;
static uvc_config_t uvc_ctx;

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
static rtos_sema_t uvc_vfs_save_img_sema = NULL;
static int uvc_vfs_is_init = 0;
static int uvc_vfs_img_file_no = 0;
static int uvc_buf_size = 0;
static RingBuffer *uvc_rb;
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
static rtos_sema_t uvc_httpc_save_img_sema = NULL;
static int uvc_httpc_img_file_no = 0;
static int uvc_buf_size = 0;
static u32 uvc_httpc_is_init = 0;
static RingBuffer *uvc_rb;
#endif

u8 uvc_buf[USBH_UVC_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

static usbh_config_t usbh_cfg = {
	.pipes = 5U,
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1,
	.alt_max = 25,
	.main_task_priority = 3U,
	.isr_task_priority  = 4U,
	.ptx_fifo_first     = 1U,
};

static usbh_uvc_cb_t uvc_cb = {
	.init = uvc_cb_init,
	.deinit = uvc_cb_deinit,
	.attach = uvc_cb_attach,
	.detach = uvc_cb_detach,
};

/* Private functions ---------------------------------------------------------*/

static int uvc_cb_init(void)
{
	RTK_LOGS(TAG, "[UVC] INIT\n");
	return HAL_OK;
}

static int uvc_cb_deinit(void)
{
	RTK_LOGS(TAG, "[UVC] DEINIT\n");
	return HAL_OK;
}

static int uvc_cb_attach(void)
{
	RTK_LOGS(TAG, "[UVC] ATTACH\n");
	rtos_sema_give(uvc_conn_sema);
	return HAL_OK;
}

static int uvc_cb_detach(void)
{
	RTK_LOGS(TAG, "[UVC] DETACH\n");
	rtos_sema_give(uvc_disconn_sema);
	return HAL_OK;
}

static void uvc_img_prepare(uvc_frame_t *frame)
{
	u32 len = 0;

	if (frame == NULL) {
		return;
	}

	len = frame->byteused;
	RTK_LOGS(TAG, "[UVC] Capture len=%d\n", len);
	if (len > USBH_UVC_BUF_SIZE) {
		RTK_LOGS(TAG, "[UVC] Image len overflow!\n");
		return;
	}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_SIMPLE)
	/* just copy data without any further processing */
	memcpy(uvc_buf, (void *)(frame->buf), len);
#endif

#if ((CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC) || (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS))
	if (rtos_mutex_take(uvc_buf_mutex, 1000 / uvc_ctx.frame_rate / 2) == SUCCESS) {
		if (uvc_ctx.fmt_type == UVC_FORMAT_H264) {
			if (RingBuffer_Space(uvc_rb) > frame->byteused) {
				RingBuffer_Write(uvc_rb, frame->buf, frame->byteused);
			}
			rtos_mutex_give(uvc_buf_mutex);

		} else {
			memcpy(uvc_buf, (void *)(frame->buf), len);
			uvc_buf_size = len;
			rtos_mutex_give(uvc_buf_mutex);
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
			rtos_sema_give(uvc_httpc_save_img_sema);
#elif (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
			rtos_sema_give(uvc_vfs_save_img_sema);
#endif
		}
	}
#endif

}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)

#if (USBH_UVC_FORMAT_TYPE == UVC_FORMAT_MJPEG)
static void uvc_vfs_thread(void *param)
{
	char path[128] = {0};
	char *prefix;
	vfs_file *finfo;
	int res = 0;
	char filename[64] = {0};
	char f_num[15];
	UNUSED(param);

	rtos_sema_create(&uvc_vfs_save_img_sema, 0U, 1U);
	uvc_vfs_is_init = 1;

	prefix = find_vfs_tag(VFS_REGION_1);

	while (uvc_vfs_is_init) {
		if (rtos_sema_take(uvc_vfs_save_img_sema, RTOS_SEMA_MAX_COUNT) != SUCCESS) {
			RTK_LOGS(TAG, "[UVC] Fail to take img_sema\n");
			continue;
		}
		memset(filename, 0, 64);
		sprintf(filename, "img");
		sprintf(f_num, "%d", uvc_vfs_img_file_no);
		strcat(filename, f_num);
		strcat(filename, ".jpeg");
		snprintf(path, sizeof(path), "%s:%s", prefix, filename);
		RTK_LOGS(TAG, "[UVC] Create image file: %s\n", path);

		finfo = (vfs_file *)fopen(path, "w");
		if (finfo == NULL) {
			RTK_LOGS(TAG, "[UVC]  fopen image fail\n");
			goto exit;
		}

		if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == SUCCESS) {
			res = fwrite(uvc_buf, uvc_buf_size, 1, (FILE *)finfo);
			if (res != 1) {
				RTK_LOGS(TAG, "[UVC] buf fwrite fail: %d\n", res);
			} else {
				//RTK_LOGS(TAG, "[UVC] buf fwrite succeed\n");
			}

			rtos_mutex_give(uvc_buf_mutex);
		}

		fclose((FILE *)finfo);

		uvc_vfs_img_file_no++;
	}
exit:
	uvc_vfs_is_init = 0;
	rtos_task_delete(NULL);
}

#else

static void uvc_vfs_thread(void *param)
{
	char path[128] = {0};
	char *prefix;
	vfs_file *finfo;
	int res = 0;
	char filename[64] = {0};
	u32 total_len = 0;
	UNUSED(param);
	u8 *buffer_h264 = rtos_mem_malloc(USBH_UVC_VFS_WRITE_SIZE);
	rtos_sema_create(&uvc_vfs_save_img_sema, 0U, 1U);
	uvc_vfs_is_init = 1;

	prefix = find_vfs_tag(VFS_REGION_1);

	memset(filename, 0, 64);
	sprintf(filename, "stream");
	strcat(filename, ".h264");

	snprintf(path, sizeof(path), "%s:%s", prefix, filename);
	RTK_LOGS(TAG, "[UVC] Create image file: %s\n", path);

	finfo = (vfs_file *)fopen(path, "wb+");
	if (finfo == NULL) {
		RTK_LOGS(TAG, "[UVC] fopen image fail\n");
		goto exit;
	}

	while (total_len < USBH_UVC_VFS_VIDEO_SIZE) {
		if (RingBuffer_Available(uvc_rb) >= USBH_UVC_VFS_WRITE_SIZE) {
			rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT);
			RingBuffer_Read(uvc_rb, buffer_h264, USBH_UVC_VFS_WRITE_SIZE);
			rtos_mutex_give(uvc_buf_mutex);
			res = fwrite(buffer_h264, USBH_UVC_VFS_WRITE_SIZE, 1, (FILE *)finfo);
			if (res != 1) {
				RTK_LOGS(TAG, "[UVC] buf fwrite fail: %d\n", res);
				goto exit;
			}
			total_len += USBH_UVC_VFS_WRITE_SIZE;
		} else {
			rtos_time_delay_ms(2);
		}
	}

exit:
	uvc_vfs_is_init = 0;
	fclose((FILE *)finfo);
	rtos_mem_free(buffer_h264);
	RingBuffer_Destroy(uvc_rb);
	rtos_task_delete(NULL);
}

#endif

static int uvc_vfs_start(void)
{
	int ret;
	rtos_task_t task;

	uvc_rb = RingBuffer_Create(uvc_buf, USBH_UVC_BUF_SIZE, 0);

	ret = rtos_task_create(&task, "uvc_vfs_thread", uvc_vfs_thread, NULL, 1024U * 8, 1U);
	if (ret != SUCCESS) {
		RTK_LOGS(TAG, "[UVC] Create vfs thread fail\n");
		ret = 1;
	} else {
		ret = 0;
	}

	return ret;
}

#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)

char upload_request[] =
	"--%s\r\n"
	"Content-Disposition: form-data; name=\"filename\"; filename=\"%s\"\r\n"
	"Content-Type: application/octet-stream\r\n\r\n";

char body_end[] =
	"\r\n--%s--\r\n";

#if (USBH_UVC_FORMAT_TYPE == UVC_FORMAT_MJPEG)

/*
	"POST /cgi-bin/submit.py HTTP/1.1\r\n"
	"Host: 192.168.1.102\r\n"
	"Content-Type: multipart/form-data; boundary=rtkBoundaryXX\r\n"
	"Content-Length: %d\r\n\r\n"
	"Connection: keep-alive\r\n"
	"--rtkBoundaryXX\r\n"
	"Content-Disposition: form-data; name=\"filename\"; filename=\"imgX.jpeg\"\r\n"
	"Content-Type: application/octet-stream\r\n\r\n";
	img.jpeg
	"\r\n--rtkBoundaryXX--\r\n";
*/

static void uvc_httpc_thread(void *param)
{
	int ret;
	char img_file[32];
	struct httpc_conn *conn = NULL;
	char post_end[256] = {0};
	char post_end1[32] = {0};
	u32 content_length;
	u32 post_end1_length;
	u32 ticknow = 0;
	static char _boundary[64] = {0};
	char type[128] = {0};
	UNUSED(param);

	conn = httpc_conn_new(USBH_UVC_HTTPC_SECURE, NULL, NULL, NULL);

	while (1) {
		RTK_LOGS(TAG, "[UVC] Try to conn\n");
		ret = httpc_conn_connect(conn, USBH_UVC_HTTPC_SERVER, USBH_UVC_HTTPC_PORT, 0);
		if (ret == 0) {
			break;
		}
		rtos_time_delay_ms(1000);
	}

	RTK_LOGS(TAG, "[UVC] Start httpc\n");

	rtos_time_delay_ms(2000);

	uvc_httpc_is_init = 1;

	while (1) {
		rtos_sema_take(uvc_httpc_save_img_sema, RTOS_MAX_TIMEOUT);
		ticknow = rtos_time_get_current_system_time_ms();
		sprintf(_boundary, "rtkBoundary%d", (int)ticknow);
		sprintf(img_file, "img%d.jpeg", uvc_httpc_img_file_no);
		sprintf(type, "multipart/form-data; boundary=%s", _boundary);
		memset(post_end1, 0x0, sizeof(post_end1));
		post_end1_length = content_length = snprintf(post_end1, sizeof(post_end1), body_end, _boundary);
		memset(post_end, 0x0, sizeof(post_end));
		content_length += snprintf(post_end, sizeof(post_end), upload_request, _boundary, img_file);

		// start a header and add Host (added automatically), Content-Type and Content-Length (added by input param)
		httpc_request_write_header_start(conn, "POST", "/cgi-bin/submit.py", type, uvc_buf_size + content_length);
		// add other header fields if necessary
		httpc_request_write_header(conn, (char *)"Connection", (char *)"keep-alive");
		// finish and send header
		httpc_request_write_header_finish(conn);

		httpc_request_write_data(conn, (u8 *) post_end, content_length - post_end1_length);

		rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT);

		ret = httpc_request_write_data(conn, uvc_buf, uvc_buf_size);
		if (ret < 0) {
			RTK_LOGS(TAG, "[UVC] Send %s request fail: %d\n", USBH_UVC_HTTP_TAG, ret);
		}

		rtos_mutex_give(uvc_buf_mutex);


		ret = httpc_request_write_data(conn, (u8 *)post_end1, post_end1_length);

		// receive response header
		httpc_response_read_header(conn);

		uvc_httpc_img_file_no ++;
	}

	rtos_task_delete(NULL);
}

#else

/*
	"POST /cgi-bin/submit.py HTTP/1.1\r\n"
	"Host: 192.168.1.102\r\n"
	"Content-Type: multipart/form-data; boundary=rtkBoundaryXX\r\n"
	"Content-Length: %d\r\n\r\n"
	"Connection: keep-alive\r\n"
	"--rtkBoundaryXX\r\n"
	"Content-Disposition: form-data; name=\"filename\"; filename=\"img.h264\"\r\n"
	"Content-Type: application/octet-stream\r\n\r\n";
	img.h264
	"\r\n--rtkBoundaryXX--\r\n";
*/

static void uvc_httpc_thread(void *param)
{
	int ret;
	char img_file[32];
	struct httpc_conn *conn = NULL;
	char post_end[256] = {0};
	char post_end1[32] = {0};
	u32 content_length;
	u32 post_end1_length;
	u32 ticknow = 0;
	static char _boundary[64] = {0};
	char type[128] = {0};
	u32 total_len = 0;
	u8 *buffer_h264 = rtos_mem_malloc(USBH_UVC_HTTPC_WRITE_SIZE);
	UNUSED(param);

	conn = httpc_conn_new(USBH_UVC_HTTPC_SECURE, NULL, NULL, NULL);

	while (1) {
		RTK_LOGS(TAG, "[UVC] Try to connect http server\n");
		ret = httpc_conn_connect(conn, USBH_UVC_HTTPC_SERVER, USBH_UVC_HTTPC_PORT, 0);
		if (ret == 0) {
			break;
		}
		rtos_time_delay_ms(1000);
	}

	RTK_LOGS(TAG, "[UVC] Start httpc\n");
	rtos_time_delay_ms(2000);
	uvc_httpc_is_init = 1;

	ticknow = rtos_time_get_current_system_time_ms();
	sprintf(_boundary, "rtkBoundary%d", (int)ticknow);
	sprintf(img_file, "img.h264");
	sprintf(type, "multipart/form-data; boundary=%s", _boundary);
	memset(post_end1, 0x0, sizeof(post_end1));
	post_end1_length = content_length = snprintf(post_end1, sizeof(post_end1), body_end, _boundary);
	memset(post_end, 0x0, sizeof(post_end));
	content_length += snprintf(post_end, sizeof(post_end), upload_request, _boundary, img_file);

	// start a header and add Host (added automatically), Content-Type and Content-Length (added by input param)
	httpc_request_write_header_start(conn, "POST", "/cgi-bin/submit.py", type, USBH_UVC_HTTPC_VIDEO_SIZE + content_length);
	// add other header fields if necessary
	httpc_request_write_header(conn, (char *)"Connection", (char *)"keep-alive");
	// finish and send header
	httpc_request_write_header_finish(conn);

	ret = httpc_request_write_data(conn, (u8 *)post_end, content_length - post_end1_length);

	while (total_len < USBH_UVC_HTTPC_VIDEO_SIZE) {

		if (RingBuffer_Available(uvc_rb) >= USBH_UVC_HTTPC_WRITE_SIZE) {
			rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT);
			RingBuffer_Read(uvc_rb, buffer_h264, USBH_UVC_HTTPC_WRITE_SIZE);
			rtos_mutex_give(uvc_buf_mutex);
			ret = httpc_request_write_data(conn, buffer_h264, USBH_UVC_HTTPC_WRITE_SIZE);
			if (ret < 0) {
				RTK_LOGS(TAG, "[UVC] Send %s request fail: %d\n", USBH_UVC_HTTP_TAG, ret);
			}
			total_len += USBH_UVC_HTTPC_WRITE_SIZE;
		} else {
			rtos_time_delay_ms(2);
		}
	}

	ret = httpc_request_write_data(conn, (u8 *)post_end1, post_end1_length);

	ret = httpc_response_read_header(conn);

	rtos_mem_free(buffer_h264);
	RingBuffer_Destroy(uvc_rb);

	uvc_httpc_is_init = 0;

	rtos_task_delete(NULL);
}

#endif

static int uvc_httpc_start(void)
{
	int ret = 0;
	rtos_task_t task;

	uvc_rb = RingBuffer_Create(uvc_buf, USBH_UVC_BUF_SIZE, 0);

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS) && (*(u32 *)LwIP_GetIP(0) != 0))) {
		RTK_LOGS(TAG, "[UVC] Wait for WIFI connection ...\n");
		rtos_time_delay_ms(1000);
	}

	rtos_sema_create(&uvc_httpc_save_img_sema, 0, 1);

	ret = rtos_task_create(&task, "uvc_httpc_thread", uvc_httpc_thread, NULL, 1024 * 8, 2);
	if (ret != SUCCESS) {
		RTK_LOGS(TAG, "[UVC] Create %s client thread fail\n", USBH_UVC_HTTP_TAG);
		rtos_sema_delete(&uvc_httpc_save_img_sema);
	}

	return ret;
}

#endif

static void example_usbh_uvc_task(void *param)
{
	int ret = 0;
	uvc_frame_t *buf;
	int img_cnt = 0;
	UNUSED(param);

	ret = usbh_init(&usbh_cfg, NULL);
	if (ret != HAL_OK) {
		goto exit;
	}

	rtos_sema_create(&uvc_conn_sema, 0U, 1U);
	rtos_sema_create(&uvc_disconn_sema, 0U, 1U);
	rtos_mutex_create(&uvc_buf_mutex);

	ret = usbh_uvc_init(&uvc_cb);
	if (ret) {
		goto exit1;
	}

	uvc_ctx.fmt_type = USBH_UVC_FORMAT_TYPE;
	uvc_ctx.width = USBH_UVC_WIDTH;
	uvc_ctx.height = USBH_UVC_HEIGHT;
	uvc_ctx.frame_rate = USBH_UVC_FRAME_RATE;

	rtos_sema_take(uvc_conn_sema, RTOS_SEMA_MAX_COUNT);

	RTK_LOGS(TAG, "[UVC] Set parameters\n");
	ret = usbh_uvc_set_param(&uvc_ctx, USBH_UVC_IF_NUM_0);
	if (ret) {
		goto exit1;
	} else {
		RTK_LOGS(TAG, "[UVC] Para: %d*%d@%dfps\n", uvc_ctx.width, uvc_ctx.height, uvc_ctx.frame_rate);

		if (uvc_ctx.fmt_type == UVC_FORMAT_MJPEG) {
			RTK_LOGS(TAG, "[UVC] MJPEG Stream\n");
		} else if (uvc_ctx.fmt_type == UVC_FORMAT_H264) {
			RTK_LOGS(TAG, "[UVC] H264 Stream\n");
		} else if (uvc_ctx.fmt_type == UVC_FORMAT_YUV) {
			RTK_LOGS(TAG, "[UVC] YUV Stream\n");
		} else {
			RTK_LOGS(TAG, "[UVC] Unsupport Stream\n");
			goto exit1;
		}
	}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
	RTK_LOGS(TAG, "[UVC] Start vfs service\n");
	ret = uvc_vfs_start();
	if (ret != 0) {
		goto exit1;
	}

	while (uvc_vfs_is_init == 0) {
		rtos_time_delay_ms(500);
	}
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
	RTK_LOGS(TAG, "[UVC] Start %s client\n", USBH_UVC_HTTP_TAG);
	ret = uvc_httpc_start();
	if (ret != 0) {
		goto exit1;
	}

	while (uvc_httpc_is_init == 0) {
		rtos_time_delay_ms(500);
	}
#endif

	if (!usbh_get_status()) {
		rtos_sema_give(uvc_disconn_sema);
	}

	img_cnt = 0;
	if (!usbh_get_status()) {
		rtos_sema_give(uvc_disconn_sema);
	}

	RTK_LOGS(TAG, "[UVC] Stream on\n");
	ret = usbh_uvc_stream_on(USBH_UVC_IF_NUM_0);
	if (ret) {
		goto exit2;
	}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
	while (uvc_httpc_is_init) {
#elif (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
	while (uvc_vfs_is_init) {
#else
	while (1) {
#endif
		RTK_LOGS(TAG, "[UVC] Get frame %d\n", img_cnt);
		buf = usbh_uvc_get_frame(USBH_UVC_IF_NUM_0);
		if (buf == NULL) {
			ret = 1;
		} else {
			ret = 0;
		}

		if (ret == 0) {
			uvc_img_prepare(buf);
		}

		RTK_LOGS(TAG, "[UVC] Put frame\n");
		usbh_uvc_put_frame(buf, USBH_UVC_IF_NUM_0);

		if (!usbh_get_status()) {
			rtos_sema_give(uvc_disconn_sema);
			break;
		}

		img_cnt ++;
	}

	RTK_LOGS(TAG, "[UVC] Stop capturing images\n");
	usbh_uvc_stream_off(USBH_UVC_IF_NUM_0);

	//RTK_LOGS(TAG, "[UVC] Free heap 0x%x bytes\n",  rtos_mem_get_free_heap_size());

exit2:
	usbh_uvc_deinit();
exit1:
	rtos_mutex_delete(uvc_buf_mutex);
	rtos_sema_delete(uvc_conn_sema);
	rtos_sema_delete(uvc_disconn_sema);
	usbh_deinit();
exit:
	RTK_LOGS(TAG, "[UVC] USBH UVC demo stop\n");
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_uvc(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, "[UVC] USBH UVC demo start\n");

	status = rtos_task_create(&task, "example_usbh_uvc_thread", example_usbh_uvc_task, NULL, 1024 * 16U, 1U);
	if (status != SUCCESS) {
		RTK_LOGS(TAG, "[UVC] Create thread fail\n");
	}
}

