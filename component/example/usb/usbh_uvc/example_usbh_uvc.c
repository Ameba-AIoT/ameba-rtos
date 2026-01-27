/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include <platform_autoconf.h>
#include "example_usbh_uvc.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"
#include "usbh_uvc_intf.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
/*Just capture and abandon frame*/
#define USBH_UVC_APP_SIMPLE                        1

/*Capture frame and write it to SD card through vfs*/
#define USBH_UVC_APP_VFS                           2

/*Capture frame and write it to http server*/
#define USBH_UVC_APP_HTTPC                         3

/* User Configurations */

/* Supported application example: USBH_UVC_APP_SIMPLE, USBH_UVC_APP_VFS, USBH_UVC_APP_HTTPC */
#define CONFIG_USBH_UVC_APP                        USBH_UVC_APP_SIMPLE

/* Supported formats: USBH_UVC_FORMAT_MJPEG, USBH_UVC_FORMAT_YUV, USBH_UVC_FORMAT_H264
 * Note: Users must verify which formats their specific camera supports and
 * adjust the definition below accordingly. */
#define CONFIG_USBH_UVC_FORMAT_TYPE                USBH_UVC_FORMAT_MJPEG

/* Target resolution and compression ratio.
 * If the specific camera device does not support
 * these values, the host stack will automatically select the closest match.
 * Always check the logs to confirm the actual parameters applied. */
#define CONFIG_USBH_UVC_WIDTH                      1280
#define CONFIG_USBH_UVC_HEIGHT                     720
#define CONFIG_USBH_UVC_FRAME_RATE                 30

/* Frame buffer size in bytes
 * Size depends on format, resolution, and scene complexity.
 * Please increase this value if an oversize error occurs. */
#define CONFIG_USBH_UVC_FRAME_BUF_SIZE             (150 * 1024)

/* Most cameras have a single video stream interface, so use default 0.
 * If the camera supports dual streams, set this to 1.
 * Note: Current protocol stack supports a maximum of 2 video stream interfaces. */
#define CONFIG_USBH_UVC_IF_NUM_0                   0

/* Hot plug / memory leak test */
#define CONFIG_USBH_UVC_HOT_PLUG                   1

/* Check image data validity (0: Disable, 1: Enable) */
#define CONFIG_USBH_UVC_CHECK_MJEPG_DATA           0

/* Number of frames to capture in the loop */
#define CONFIG_USBH_UVC_LOOP                       200

/* Maximum continuous error count before stopping */
#define CONFIG_USBH_UVC_MAX_FAIL_COUNT             5

/* Priority of the main UVC thread */
#define CONFIG_USBH_UVC_MAIN_THREAD_PRIORITY       3

/* Priority of the UVC test thread */
#define CONFIG_USBH_UVC_TEST_THREAD_PRIORITY       4

/* Priority of the USB hotplug detection thread */
#define CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY    6

#if USBH_UVC_USE_HW
/* HW UVC IRQ PRIORITY*/
#define CONFIG_USBH_UVC_HW_IRQ_PRIORITY            INT_PRI_LOWEST
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
#include "vfs.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "ringbuffer.h"
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#include "httpc/httpc.h"
#include "lwipconf.h"
#include "lwip_netconf.h"
#include "ringbuffer.h"
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
#define USBH_UVC_VFS_WRITE_SIZE          (16 * 1024)
#define USBH_UVC_VFS_VIDEO_SIZE          (2 * 1024 * 1024)
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#define USBH_UVC_HTTPC_WRITE_SIZE        (4 * 1024)
#define USBH_UVC_HTTPC_VIDEO_SIZE        (2 * 1024 * 1024)
#define USBH_UVC_HTTPC_SERVER            "192.168.1.100"
#define USBH_UVC_HTTPC_SECURE            HTTPC_SECURE_NONE
#define USBH_UVC_HTTPC_PORT              5090
#define USBH_UVC_HTTP_TAG                "HTTP"
#endif
/* User Configurations */

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int uvc_cb_init(void);
static int uvc_cb_deinit(void);
static int uvc_cb_attach(void);
static int uvc_cb_detach(void);
static int uvc_cb_setup(void);
static int uvc_cb_setparam(void);
static int uvc_cb_process(usb_host_t *host, u8 msg);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UVC";

static rtos_sema_t uvc_attach_sema;
static rtos_sema_t uvc_detach_sema;
static rtos_sema_t uvc_start_sema;
static rtos_sema_t uvc_setparam_sema;
static rtos_mutex_t uvc_buf_mutex = NULL;
static usbh_uvc_config_t uvc_ctx;
static __IO int usbh_uvc_is_ready = 0;

static u32 rx_total_H;
static u32 rx_total_L;
static u32 rx_start;

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
static rtos_sema_t uvc_vfs_save_img_sema = NULL;
static int uvc_vfs_is_init = 0;
static int uvc_vfs_img_file_no = 0;
static int uvc_buf_size = 0;
static RingBuffer *uvc_rb;
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
static rtos_sema_t uvc_httpc_save_img_sema = NULL;
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
static int uvc_httpc_img_file_no = 0;
#endif
static int uvc_buf_size = 0;
static u32 uvc_httpc_is_init = 0;
static RingBuffer *uvc_rb;
#endif

u8 uvc_buf[CONFIG_USBH_UVC_FRAME_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_priority = CONFIG_USBH_UVC_MAIN_THREAD_PRIORITY,
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
#endif
};

static usbh_uvc_cb_t uvc_cb = {
	.init = uvc_cb_init,
	.deinit = uvc_cb_deinit,
	.attach = uvc_cb_attach,
	.detach = uvc_cb_detach,
	.setup = uvc_cb_setup,
	.setparam = uvc_cb_setparam,
};

static usbh_user_cb_t usbh_usr_cb = {
	.process = uvc_cb_process,
};

/* Private functions ---------------------------------------------------------*/

static int uvc_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return HAL_OK;
}

static int uvc_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	return HAL_OK;
}

static int uvc_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	rtos_sema_give(uvc_attach_sema);
	return HAL_OK;
}

static int uvc_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
#if CONFIG_USBH_UVC_HOT_PLUG
	rtos_sema_give(uvc_detach_sema);
#endif
	usbh_uvc_is_ready = 0;
	return HAL_OK;
}

static int uvc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	usbh_uvc_is_ready = 1;
	rtos_sema_give(uvc_start_sema);
	return HAL_OK;
}

static int uvc_cb_setparam(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETPARAM\n");
	rtos_sema_give(uvc_setparam_sema);
	return HAL_OK;
}

static int uvc_cb_process(usb_host_t *host, u8 msg)
{
	UNUSED(host);

	switch (msg) {
	case USBH_MSG_DISCONNECTED:
		usbh_uvc_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

static void uvc_calculate_tp(u32 loop)
{
	u32 rx_elapse;
	u32 rx_perf;
	u32 rx_fps;
	u32 rx_perf_total;

	rx_elapse = SYSTIMER_GetPassTime(rx_start);

	rx_fps = loop * 1000 / rx_elapse;
	rx_perf = rx_total_L / 1024 * 1000 / rx_elapse;//KB/S

	RTK_LOGS(TAG, RTK_LOG_INFO, "TP %d KB/s @ %d ms, fps %d/s\n", rx_perf, rx_elapse, rx_fps);

	rx_perf = rx_perf * 10 / 1024;
	rx_perf_total = rx_perf + ((rx_total_H * 10000 << 12) / rx_elapse);
	RTK_LOGS(TAG, RTK_LOG_INFO, "TP %d.%d MB/s-%d (%d_%d/%d)\n", rx_perf_total / 10, rx_perf_total % 10, rx_perf, rx_total_H, rx_total_L, loop);

	rx_total_L = 0;
	rx_total_H = 0;
}

static void usbh_uvc_img_prepare(usbh_uvc_frame_t *frame)
{
	u32 len = frame->byteused;

#if CONFIG_USBH_UVC_CHECK_MJEPG_DATA
	//some camera may pad 0 to the end of image
	while (1) {
		if (frame->buf[len - 1] == 0) {
			len--;
		} else {
			break;
		}
	}

	/* UVC Host only passes data through. */
	/* Invalid data from camera should be handled by application and must not stopping fetching the next frame. */
	if (frame->buf[0] != 0xff || frame->buf[1] != 0xd8 || frame->buf[len - 2] != 0xff || frame->buf[len - 1] != 0xd9) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[mjpeg] image error: %x %x %x %x\n", frame->buf[0], frame->buf[1], frame->buf[2], frame->buf[3]);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[mjpeg] image error: %x %x %x %x\n", frame->buf[len - 4], frame->buf[len - 3], frame->buf[len - 2], frame->buf[len - 1]);
		/* should not return */
		/* The application can adopt a drop mechanism here, discarding frames that do not comply with the specification without storing them */
	}
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_SIMPLE)
	/* just copy data without any further processing */
	//memcpy(uvc_buf, (void *)(frame->buf), len);
	if (rx_total_L > (rx_total_L + len)) {
		rx_total_H ++;
	}
	rx_total_L += len;
#endif

#if ((CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC) || (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS))
	if (rtos_mutex_take(uvc_buf_mutex, 1000 / uvc_ctx.frame_rate / 2) == RTK_SUCCESS) {
		if (uvc_ctx.fmt_type == USBH_UVC_FORMAT_H264) {
			if ((u32)RingBuffer_Space(uvc_rb) > frame->byteused) {
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

#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
static void uvc_vfs_thread(void *param)
{
	char path[128] = {0};
	char *prefix;
	FILE *finfo;
	int res = 0;
	char filename[64] = {0};
	char f_num[15];
	UNUSED(param);

	rtos_sema_create(&uvc_vfs_save_img_sema, 0U, 1U);

	res = vfs_user_register("sdcard", VFS_FATFS, VFS_INF_SD, VFS_REGION_4, VFS_RW);
	if (res == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "VFS-SDcard Init Success \n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VFS-SDcard Init Fail \n");
		goto exit;
	}

	uvc_vfs_is_init = 1;

	prefix = find_vfs_tag(VFS_REGION_4);

	while (uvc_vfs_is_init) {
		if (rtos_sema_take(uvc_vfs_save_img_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Fail to take img_sema\n");
			continue;
		}
		memset(filename, 0, 64);
		sprintf(filename, "img");
		sprintf(f_num, "%d", uvc_vfs_img_file_no);
		strcat(filename, f_num);
		strcat(filename, ".jpeg");
		snprintf(path, sizeof(path), "%s:%s", prefix, filename);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Create image file: %s\n", path);

		finfo = fopen(path, "wb+");
		if (finfo == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "fopen image fail\n");
			goto exit;
		}

		if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
			res = fwrite(uvc_buf, uvc_buf_size, 1, finfo);
			if (res != uvc_buf_size) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "fwrite() failed, err: %d\n", res);
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "fwrite() ok, w %d\n", uvc_buf_size);
			}

			rtos_mutex_give(uvc_buf_mutex);
		}

		fclose(finfo);

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
	FILE *finfo;
	int res = 0;
	char filename[64] = {0};
	u32 total_len = 0;
	UNUSED(param);
	u8 *buffer_h264 = rtos_mem_malloc(USBH_UVC_VFS_WRITE_SIZE);
	rtos_sema_create(&uvc_vfs_save_img_sema, 0U, 1U);
	uvc_vfs_is_init = 1;

	res = vfs_user_register("sdcard", VFS_FATFS, VFS_INF_SD, VFS_REGION_4, VFS_RW);
	if (res == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "VFS-SDcard Init Success \n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VFS-SDcard Init Fail \n");
		goto exit;
	}

	prefix = find_vfs_tag(VFS_REGION_4);

	memset(filename, 0, 64);
	sprintf(filename, "stream");
	strcat(filename, ".h264");

	snprintf(path, sizeof(path), "%s:%s", prefix, filename);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Create image file: %s\n", path);

	finfo = fopen(path, "wb+");
	if (finfo == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "fopen image fail\n");
		goto exit;
	}

	while (total_len < USBH_UVC_VFS_VIDEO_SIZE) {
		if (RingBuffer_Available(uvc_rb) >= USBH_UVC_VFS_WRITE_SIZE) {
			rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT);
			RingBuffer_Read(uvc_rb, buffer_h264, USBH_UVC_VFS_WRITE_SIZE);
			rtos_mutex_give(uvc_buf_mutex);
			res = fwrite(buffer_h264, USBH_UVC_VFS_WRITE_SIZE, 1, finfo);
			if (res != 1) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "buf fwrite fail: %d\n", res);
				goto exit;
			}
			total_len += USBH_UVC_VFS_WRITE_SIZE;
		} else {
			rtos_time_delay_ms(2);
		}
	}

exit:
	uvc_vfs_is_init = 0;
	fclose(finfo);
	rtos_mem_free(buffer_h264);
	RingBuffer_Destroy(uvc_rb);
	rtos_task_delete(NULL);
}

#endif

static int uvc_vfs_start(void)
{
	int ret;
	rtos_task_t task;

	uvc_rb = RingBuffer_Create(uvc_buf, CONFIG_USBH_UVC_FRAME_BUF_SIZE, LOCAL_RINGBUFF, 0);

	ret = rtos_task_create(&task, "uvc_vfs_thread", uvc_vfs_thread, NULL, 1024U * 8, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create vfs thread fail\n");
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

#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)

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
		RTK_LOGS(TAG, RTK_LOG_INFO, "Try to conn\n");
		ret = httpc_conn_connect(conn, USBH_UVC_HTTPC_SERVER, USBH_UVC_HTTPC_PORT, 0);
		if (ret == 0) {
			break;
		}
		rtos_time_delay_ms(1000);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Start httpc\n");

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
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Send %s request fail: %d\n", USBH_UVC_HTTP_TAG, ret);
			httpc_conn_close(conn);
			rtos_time_delay_ms(2);
			httpc_conn_connect(conn, USBH_UVC_HTTPC_SERVER, USBH_UVC_HTTPC_PORT, 0);
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Send image%d request ok\n", uvc_httpc_img_file_no);
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "Try to connect http server\n");
		ret = httpc_conn_connect(conn, USBH_UVC_HTTPC_SERVER, USBH_UVC_HTTPC_PORT, 0);
		if (ret == 0) {
			break;
		}
		rtos_time_delay_ms(1000);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Start httpc\n");
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
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Send %s request fail: %d\n", USBH_UVC_HTTP_TAG, ret);
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

	// Delay to check successful WiFi connection and obtain of an IP address
	while (LwIP_Check_Connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}

	uvc_rb = RingBuffer_Create(uvc_buf, CONFIG_USBH_UVC_FRAME_BUF_SIZE, LOCAL_RINGBUFF, 0);

	rtos_sema_create(&uvc_httpc_save_img_sema, 0, 1);

	ret = rtos_task_create(&task, "uvc_httpc_thread", uvc_httpc_thread, NULL, 1024 * 8, 2);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create %s client thread fail\n", USBH_UVC_HTTP_TAG);
		rtos_sema_delete(&uvc_httpc_save_img_sema);
	}

	return ret;
}

#endif

#if CONFIG_USBH_UVC_HOT_PLUG
static void uvc_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(uvc_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			rtos_time_delay_ms(100);
			usbh_uvc_deinit();
			usbh_deinit();
			rtos_time_delay_ms(10);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
				break;
			}

			ret = usbh_uvc_init(&uvc_cb);
			if (ret < 0) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init UVC fail\n");
				usbh_deinit();
				break;
			}
		}
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Hotplug thread fail\n");
	rtos_task_delete(NULL);
}
#endif

static void uvc_test(void *param)
{
	usbh_uvc_frame_t *buf;
	const char *fmt_name = NULL;
	int ret = 0;
	int img_cnt = 0;
	int fail_cnt = 0;
	u32 len;

	UNUSED(param);

	if (rtos_sema_take(uvc_start_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {

		uvc_ctx.fmt_type = CONFIG_USBH_UVC_FORMAT_TYPE;
		uvc_ctx.width = CONFIG_USBH_UVC_WIDTH;
		uvc_ctx.height = CONFIG_USBH_UVC_HEIGHT;
		uvc_ctx.frame_rate = CONFIG_USBH_UVC_FRAME_RATE;
		uvc_ctx.frame_buf_size = CONFIG_USBH_UVC_FRAME_BUF_SIZE;

#if USBH_UVC_USE_HW
		uvc_ctx.isr_priority = CONFIG_USBH_UVC_HW_IRQ_PRIORITY;
#endif

		if (uvc_ctx.fmt_type == USBH_UVC_FORMAT_MJPEG) {
			fmt_name = "MJPEG";
		} else if (uvc_ctx.fmt_type == USBH_UVC_FORMAT_H264) {
			fmt_name = "H264";
		} else if (uvc_ctx.fmt_type == USBH_UVC_FORMAT_YUV) {
			fmt_name = "YUV";
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Unsupport type %d\n", uvc_ctx.fmt_type);
			rtos_task_delete(NULL);
			return;
		}

		/* Trigger the UVC parameter setting process */
		ret = usbh_uvc_set_param(&uvc_ctx, CONFIG_USBH_UVC_IF_NUM_0);
		/* Check if the request itself failed immediately */
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set param req: %d\n", ret);
			rtos_task_delete(NULL);
			return;
		}

		/* Wait for the semaphore indicating the setting is actually completed */
		if (rtos_sema_take(uvc_setparam_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Set paras ok: %s %d*%d@%dfps\n",
					 fmt_name, uvc_ctx.width, uvc_ctx.height, uvc_ctx.frame_rate);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set paras fail: %s %d*%d@%dfps\n",
					 fmt_name, uvc_ctx.width, uvc_ctx.height, uvc_ctx.frame_rate);
			rtos_task_delete(NULL);
			return;
		}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
		RTK_LOGS(TAG, RTK_LOG_INFO, "Start vfs service\n");
		ret = uvc_vfs_start();
		if (ret != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Start vfs err\n");
			rtos_task_delete(NULL);
			return;
		}

		while (uvc_vfs_is_init == 0) {
			rtos_time_delay_ms(500);
		}
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
		RTK_LOGS(TAG, RTK_LOG_INFO, "Start %s client\n", USBH_UVC_HTTP_TAG);
		ret = uvc_httpc_start();
		if (ret != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Start httpc err\n");
			rtos_task_delete(NULL);
			return;
		}

		while (uvc_httpc_is_init == 0) {
			rtos_time_delay_ms(500);
		}
#endif

		img_cnt = 0;
		fail_cnt = 0;

		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream on\n");
		ret = usbh_uvc_stream_on(&uvc_ctx, CONFIG_USBH_UVC_IF_NUM_0);

		if (ret) {
			rtos_task_delete(NULL);
			return;
		}
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
		while (uvc_httpc_is_init) {
#elif (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
		while (uvc_vfs_is_init) {
#else
		rx_start = SYSTIMER_TickGet();
		while (img_cnt < CONFIG_USBH_UVC_LOOP) {
#endif

			if (!usbh_uvc_is_ready) {
				rtos_time_delay_ms(500);
				RTK_LOGS(TAG, RTK_LOG_WARN, "Device disconnect, test abort\n");
				break;
			}

			buf = usbh_uvc_get_frame(CONFIG_USBH_UVC_IF_NUM_0);

			if (buf == NULL) {
				if (usbh_uvc_is_ready) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Capture frame %d fail\n", img_cnt);
					fail_cnt++;
					if (fail_cnt >= CONFIG_USBH_UVC_MAX_FAIL_COUNT) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "Stop Capture (fail:%d)\n", fail_cnt);
						break;
					}
				}

				rtos_time_delay_ms(10);
				break;
			}

			len = buf->byteused;
			if (len > CONFIG_USBH_UVC_FRAME_BUF_SIZE) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Frame %d overflow %d > %d\n", img_cnt, len, CONFIG_USBH_UVC_FRAME_BUF_SIZE);
				usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_IF_NUM_0);
				return;
			}

			if (len > 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Captured frame %d, len=%d\n", img_cnt, len);
				usbh_uvc_img_prepare(buf);
			}

			usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_IF_NUM_0);

			img_cnt ++;
		}

		if (usbh_uvc_is_ready) {
			uvc_calculate_tp(img_cnt);
			usbh_uvc_stream_off(CONFIG_USBH_UVC_IF_NUM_0);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Stream off\n");
		}
	}

	rtos_time_delay_ms(1000);
	rtos_task_delete(NULL);

}

static void example_usbh_uvc_task(void *param)
{
	rtos_task_t uvc_task;
#if CONFIG_USBH_UVC_HOT_PLUG
	rtos_task_t hotplug_task;
#endif
	int ret = 0;

	UNUSED(param);

	rtos_sema_create(&uvc_attach_sema, 0U, 1U);
	rtos_sema_create(&uvc_detach_sema, 0U, 1U);
	rtos_sema_create(&uvc_start_sema, 0U, 1U);
	rtos_sema_create(&uvc_setparam_sema, 0U, 1U);
	rtos_mutex_create(&uvc_buf_mutex);

	ret = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (ret != HAL_OK) {
		goto free_sema_exit;
	}

	ret = usbh_uvc_init(&uvc_cb);
	if (ret != HAL_OK) {
		usbh_deinit();
		goto usb_deinit_exit;
	}

#if CONFIG_USBH_UVC_HOT_PLUG
	ret = rtos_task_create(&hotplug_task, "uvc_hotplug_thread", uvc_hotplug_thread, NULL, 1024U, CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto usbh_uvc_deinit_exit;
	}
#endif

	if (rtos_sema_take(uvc_attach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
		ret = rtos_task_create(&uvc_task, "uvc_test", uvc_test, NULL, 1024U * 2, CONFIG_USBH_UVC_TEST_THREAD_PRIORITY);
		if (ret != RTK_SUCCESS) {
			goto delete_hotplug_task_exit;
		}
	}

	goto example_exit;

delete_hotplug_task_exit:
#if CONFIG_USBH_UVC_HOT_PLUG
	rtos_task_delete(hotplug_task);
#endif

#if CONFIG_USBH_UVC_HOT_PLUG
usbh_uvc_deinit_exit:
#endif
	usbh_uvc_deinit();

usb_deinit_exit:
	usbh_deinit();

free_sema_exit:
	rtos_mutex_delete(uvc_buf_mutex);
	rtos_sema_delete(uvc_attach_sema);
	rtos_sema_delete(uvc_detach_sema);
	rtos_sema_delete(uvc_start_sema);
	rtos_sema_delete(uvc_setparam_sema);
example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/

void example_usbh_uvc(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UVC demo start\n");

	status = rtos_task_create(&task, "example_usbh_uvc_thread", example_usbh_uvc_task, NULL, 1024 * 16U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}

