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
 * Note: Please increase this value if an oversize error occurs. */
#define CONFIG_USBH_UVC_FRAME_BUF_SIZE             (150 * 1024)

/* Most cameras have a single video stream interface, so use default 0.
 * If the camera supports dual streams, set this to 1.
 * Note: Current protocol stack supports a maximum of 2 video stream interfaces. */
#define CONFIG_USBH_UVC_IF_NUM_0                   0

/* Hot plug / memory leak test */
#define CONFIG_USBH_UVC_HOT_PLUG                   1

/* Check image data validity (0: Disable, 1: Enable) */
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
#define CONFIG_USBH_UVC_CHECK_MJEPG_DATA           1
#else
#define CONFIG_USBH_UVC_CHECK_MJEPG_DATA           0
#endif

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

#define USBH_UVC_VFS_WRITE_SIZE          (4 * 1024)
#define USBH_UVC_VFS_VIDEO_SIZE          (2 * 1024 * 1024)
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#include "httpc/httpc.h"
#include "lwipconf.h"
#include "lwip_netconf.h"

/* HTTPC buffer mode: 0 = ringbuffer, 1 = PSRAM */
#if USBH_UVC_USE_HW
#define USBH_UVC_HTTPC_BUFFER_MODE       1
#else
#define USBH_UVC_HTTPC_BUFFER_MODE       0
#endif

#if (USBH_UVC_HTTPC_BUFFER_MODE == 0)
#include "ringbuffer.h"
#endif

#define USBH_UVC_HTTPC_WRITE_SIZE        (4 * 1024)
#define USBH_UVC_HTTPC_VIDEO_SIZE        (2 * 1024 * 1024)
#define USBH_UVC_HTTPC_SERVER            "192.168.1.102"
#define USBH_UVC_HTTPC_SECURE            HTTPC_SECURE_NONE
#define USBH_UVC_HTTPC_PORT              5090
#define USBH_UVC_HTTP_TAG                "HTTP"

#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
/* PSRAM buffer: base address and size (2 MB) — HW UVC only */
#define USBH_UVC_HTTPC_PSRAM_BASE_ADDR   (PSRAM_BASE + 0x200000U)
#define USBH_UVC_HTTPC_PSRAM_BUF_SIZE    0x200000U          /* 2 MB */
#endif
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
static int uvc_cb_setparam(int status);

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UVC";

static rtos_sema_t uvc_attach_sema;
static rtos_sema_t uvc_detach_sema;
static rtos_sema_t uvc_start_sema;
static rtos_sema_t uvc_setparam_sema;
static rtos_mutex_t uvc_buf_mutex = NULL;
static usbh_uvc_s_ctx_t uvc_s_ctx;
static rtos_task_t uvc_task;
static int uvc_setparam_status;
static __IO u8 uvc_task_exiting = 0;

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
static u32 uvc_httpc_is_init = 0;
static char uvc_httpc_server_ip[48] = USBH_UVC_HTTPC_SERVER;
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
/* MJPEG: frame-by-frame upload via sema signal */
static rtos_sema_t uvc_httpc_save_img_sema = NULL;
static int uvc_httpc_img_file_no = 0;
static int uvc_buf_size = 0;
#if (USBH_UVC_HTTPC_BUFFER_MODE == 0)
/* Ringbuffer mode: uvc_rb needed by img_prepare ringbuffer branch,
 * which is compiled when BUFFER_MODE==0 regardless of format type. */
static RingBuffer *uvc_rb;
#endif
#elif (USBH_UVC_HTTPC_BUFFER_MODE == 0)
/* Ringbuffer mode: H264 streaming + MJPEG per-frame upload.
 * uvc_buf_size and uvc_httpc_save_img_sema needed by MJPEG path
 * in usbh_uvc_img_prepare when fmt_type != H264. */
static RingBuffer *uvc_rb;
static int uvc_buf_size = 0;
static rtos_sema_t uvc_httpc_save_img_sema = NULL;
#endif
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
/* PSRAM buffer state — written by usbh_uvc_img_prepare, read by upload thread */
static u8 *uvc_httpc_psramp_write_ptr = NULL;
static u32 uvc_httpc_psramp_total_len = 0;
static u8 *uvc_httpc_psramp_base = NULL;
static u32 uvc_httpc_psramp_frame_cnt = 0;
static rtos_sema_t uvc_httpc_done_sema = NULL;
#endif
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS) || \
    ((CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC) && \
     (!defined(USBH_UVC_HTTPC_BUFFER_MODE) || (USBH_UVC_HTTPC_BUFFER_MODE == 0)))
u8 uvc_buf[CONFIG_USBH_UVC_FRAME_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
#endif

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = 768,
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
#elif defined (CONFIG_AMEBAPRO3)
	/*FIFO total depth is 2232 DWORD, resv 8 DWORD for DMA addr */
	.rx_fifo_depth = 1712,
	.nptx_fifo_depth = 256,
	.ptx_fifo_depth = 256,
#endif
};

static usbh_uvc_ctx_t uvc_cfg = {
#if USBH_UVC_USE_HW
	.hw_isr_pri = CONFIG_USBH_UVC_HW_IRQ_PRIORITY,
#endif
};

static usbh_uvc_cb_t uvc_cb = {
	.init = uvc_cb_init,
	.deinit = uvc_cb_deinit,
	.attach = uvc_cb_attach,
	.detach = uvc_cb_detach,
	.setup = uvc_cb_setup,
	.set_param = uvc_cb_setparam,
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

	return HAL_OK;
}

static int uvc_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	rtos_sema_give(uvc_start_sema);
	return HAL_OK;
}

static int uvc_cb_setparam(int status)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETPARAM\n");
	uvc_setparam_status = status;
	rtos_sema_give(uvc_setparam_sema);
	return HAL_OK;
}

static void uvc_calculate_tp(u32 loop)
{
	u32 rx_elapse;
	u32 rx_perf;
	u32 rx_fps;
	u32 rx_perf_total;

	rx_elapse = SYSTIMER_GetPassTime(rx_start);

	rx_fps = loop * 1000U / rx_elapse;
	rx_perf = rx_total_L / 1024U * 1000U / rx_elapse;//KB/S

	RTK_LOGS(TAG, RTK_LOG_INFO, "TP %d KB/s @ %d ms, fps %d/s\n", rx_perf, rx_elapse, rx_fps);

	rx_perf = rx_perf * 10U / 1024U;
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
	if (len == 0) {
		return;
	}
	while (len > 2 && frame->buf[len - 1] == 0) {
		len--;
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
	/* u32 wraparound detection: (rx_total_L + len) wraps when it becomes smaller */
	if (rx_total_L > (rx_total_L + len)) {
		rx_total_H ++;
	}
	rx_total_L += len;
#endif

#if ((CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC) || (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS))
	if (rtos_mutex_take(uvc_buf_mutex, 1000U / uvc_s_ctx.frame_rate / 2U + 50U) == RTK_SUCCESS) {
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
		/* PSRAM mode: H264 only — write frames to PSRAM buffer for batch upload */
		if (uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H264) {
			if (uvc_httpc_psramp_total_len + len <= USBH_UVC_HTTPC_PSRAM_BUF_SIZE) {
				memcpy((void *)uvc_httpc_psramp_write_ptr, (void *)(frame->buf), len);
				uvc_httpc_psramp_write_ptr += len;
				uvc_httpc_psramp_total_len += len;
				uvc_httpc_psramp_frame_cnt++;
			}
			if (rx_total_L > (rx_total_L + len)) {
				rx_total_H++;
			}
			rx_total_L += len;
		}
		/* Non-H264 format with PSRAM mode: not supported, drop frame */
		rtos_mutex_give(uvc_buf_mutex);
#else
		/* Ringbuffer mode:
		 * - H264: write to ringbuffer for streaming upload
		 * - MJPEG: copy to uvc_buf + signal httpc thread for per-frame upload
		 * - YUV: not supported in HTTPC, drop frame */
		if (uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H264) {
			if ((u32)RingBuffer_Space(uvc_rb) > frame->byteused) {
				RingBuffer_Write(uvc_rb, frame->buf, frame->byteused);
			}
			rtos_mutex_give(uvc_buf_mutex);
		} else if (uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_MJPEG) {
			memcpy(uvc_buf, (void *)(frame->buf), len);
			uvc_buf_size = len;
			if (rx_total_L > (rx_total_L + len)) {
				rx_total_H++;
			}
			rx_total_L += len;
			rtos_mutex_give(uvc_buf_mutex);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "give sema %x\n", (u32)uvc_httpc_save_img_sema);
			rtos_sema_give(uvc_httpc_save_img_sema);
		}
		/* YUV not supported in HTTPC ringbuffer mode: drop frame */
#endif
#else
		if (uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H264) {
			if ((u32)RingBuffer_Space(uvc_rb) > frame->byteused) {
				RingBuffer_Write(uvc_rb, frame->buf, frame->byteused);
			}
			rtos_mutex_give(uvc_buf_mutex);

		} else {
			memcpy(uvc_buf, (void *)(frame->buf), len);
			uvc_buf_size = len;
			rtos_mutex_give(uvc_buf_mutex);
			rtos_sema_give(uvc_vfs_save_img_sema);
		}
#endif
	}
#endif

}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)

#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
static void example_usbh_uvc_vfs_thread(void *param)
{
	char path[128] = {0};
	char *prefix;
	FILE *finfo = NULL;
	int res = 0;
	char filename[64] = {0};
	char f_num[15];
	UNUSED(param);

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
		if (uvc_task_exiting) {
			goto exit;
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

static void example_usbh_uvc_vfs_thread(void *param)
{
	char path[128] = {0};
	char *prefix;
	FILE *finfo = NULL;
	int res = 0;
	char filename[64] = {0};
	u32 total_len = 0;
	UNUSED(param);
	u8 *buffer_h264 = NULL;

	uvc_vfs_is_init = 1;
	uvc_rb = RingBuffer_Create(uvc_buf, CONFIG_USBH_UVC_FRAME_BUF_SIZE, LOCAL_RINGBUFF, 0);
	buffer_h264 = rtos_mem_malloc(USBH_UVC_VFS_WRITE_SIZE);

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
			if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
				RingBuffer_Read(uvc_rb, buffer_h264, USBH_UVC_VFS_WRITE_SIZE);
				rtos_mutex_give(uvc_buf_mutex);
			}
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
	if (finfo != NULL) {
		fclose(finfo);
	}
	if (buffer_h264) {
		rtos_mem_free(buffer_h264);
	}
	if (uvc_rb) {
		RingBuffer_Destroy(uvc_rb);
		uvc_rb = NULL;
	}
	rtos_task_delete(NULL);
}

#endif

static int uvc_vfs_start(void)
{
	int ret;
	rtos_task_t task;

	rtos_sema_create(&uvc_vfs_save_img_sema, 0U, 1U);
	ret = rtos_task_create(&task, "example_usbh_uvc_vfs_thread", example_usbh_uvc_vfs_thread, NULL, 3584U, 1U);

	if (ret != RTK_SUCCESS) {
		rtos_sema_delete(uvc_vfs_save_img_sema);
		uvc_vfs_save_img_sema = NULL;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create vfs thread fail\n");
		ret = 1;
	} else {
		ret = 0;
	}

	return ret;
}

static void uvc_vfs_stop(void)
{
	while (uvc_vfs_is_init) {
		rtos_time_delay_ms(100);
	}
	if (uvc_vfs_save_img_sema != NULL) {
		rtos_sema_delete(uvc_vfs_save_img_sema);
		uvc_vfs_save_img_sema = NULL;
	}
}
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)

static char upload_request[] =
	"--%s\r\n"
	"Content-Disposition: form-data; name=\"filename\"; filename=\"%s\"\r\n"
	"Content-Type: application/octet-stream\r\n\r\n";

static char body_end[] =
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

static void example_usbh_uvc_httpc_thread(void *param)
{
	int ret = 0;
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
	if (conn == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "httpc_conn_new failed\n");
		goto exit;
	}

	while (1) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Try to conn\n");
		ret = httpc_conn_connect(conn, uvc_httpc_server_ip, USBH_UVC_HTTPC_PORT, 0);
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
		if (uvc_task_exiting) {
			goto exit;
		}
		ticknow = rtos_time_get_current_system_time_ms();
		sprintf(_boundary, "rtkBoundary%d", (int)ticknow);
		sprintf(img_file, "img%d.jpeg", uvc_httpc_img_file_no);
		sprintf(type, "multipart/form-data; boundary=%s", _boundary);
		memset(post_end1, 0x0, sizeof(post_end1));
		content_length = snprintf(post_end1, sizeof(post_end1), body_end, _boundary);
		post_end1_length = strlen(post_end1);
		memset(post_end, 0x0, sizeof(post_end));
		content_length += snprintf(post_end, sizeof(post_end), upload_request, _boundary, img_file);

		// start a header and add Host (added automatically), Content-Type and Content-Length (added by input param)
		httpc_request_write_header_start(conn, "POST", "/cgi-bin/submit.py", type, uvc_buf_size + content_length);
		// add other header fields if necessary
		httpc_request_write_header(conn, (char *)"Connection", (char *)"keep-alive");
		// finish and send header
		httpc_request_write_header_finish(conn);

		httpc_request_write_data(conn, (u8 *) post_end, content_length - post_end1_length);

		if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
			ret = httpc_request_write_data(conn, uvc_buf, uvc_buf_size);
		} else {
			ret = -1;
		}
		if (ret < 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Send %s request fail: %d\n", USBH_UVC_HTTP_TAG, ret);
			rtos_mutex_give(uvc_buf_mutex);
			httpc_conn_close(conn);
			rtos_time_delay_ms(2);
			/* Reconnect and re-send header before retrying this frame's data */
			ret = httpc_conn_connect(conn, uvc_httpc_server_ip, USBH_UVC_HTTPC_PORT, 0);
			if (ret == 0) {
				httpc_request_write_header_start(conn, "POST", "/cgi-bin/submit.py", type, uvc_buf_size + content_length);
				httpc_request_write_header(conn, (char *)"Connection", (char *)"keep-alive");
				httpc_request_write_header_finish(conn);
				httpc_request_write_data(conn, (u8 *) post_end, content_length - post_end1_length);
				/* Re-take mutex to resend the frame data on the new connection */
				if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
					ret = httpc_request_write_data(conn, uvc_buf, uvc_buf_size);
					rtos_mutex_give(uvc_buf_mutex);
				} else {
					ret = -1;
				}
				if (ret < 0) {
					continue;  /* retry next frame */
				}
			} else {
				continue;  /* reconnect failed, retry next iteration */
			}
		} else {
			rtos_mutex_give(uvc_buf_mutex);
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "Send image%d request ok\n", uvc_httpc_img_file_no);

		ret = httpc_request_write_data(conn, (u8 *)post_end1, post_end1_length);

		// receive response header
		httpc_response_read_header(conn);

		uvc_httpc_img_file_no ++;
	}

exit:
	if (conn) {
		httpc_conn_close(conn);
	}
	uvc_httpc_is_init = 0;
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

static void example_usbh_uvc_httpc_thread(void *param)
{
	int ret = 0;
	struct httpc_conn *conn = NULL;
	char post_end[256] = {0};
	char post_end1[32] = {0};
	u32 content_length;
	u32 post_end1_length;
	u32 ticknow = 0;
	static char _boundary[64] = {0};
	char type[128] = {0};
	UNUSED(param);

#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
	/* PSRAM buffer mode: wait for capture to finish, then upload */
	char filename[32] = "stream.h264";
	u32 send_len = 0;
	u32 send_offset;
	u8 *send_buf = NULL;
	u32 send_frames = 0;
	/* PSRAM buffer state already initialized by uvc_httpc_start() before thread creation,
	 * because uvc_test (priority 4) may reach img_prepare before this thread (priority 2). */

	/* Wait for uvc_test() to finish writing all frames to PSRAM */
	rtos_sema_take(uvc_httpc_done_sema, RTOS_SEMA_MAX_COUNT);
	if (uvc_task_exiting) {
		goto exit;
	}
	/* Acquire mutex to read final total_len safely */
	if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
		send_len = uvc_httpc_psramp_total_len;
		send_frames = uvc_httpc_psramp_frame_cnt;
		rtos_mutex_give(uvc_buf_mutex);
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: mutex take failed\n", USBH_UVC_HTTP_TAG);
		goto exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "%s: PSRAM has %u bytes from %u frames\n",
			 USBH_UVC_HTTP_TAG, send_len, send_frames);

	if (send_len == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: Nothing to upload\n", USBH_UVC_HTTP_TAG);
		goto exit;
	}

	conn = httpc_conn_new(USBH_UVC_HTTPC_SECURE, NULL, NULL, NULL);
	if (conn == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: httpc_conn_new failed\n", USBH_UVC_HTTP_TAG);
		goto exit;
	}

	while (1) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Try to connect %s server\n", USBH_UVC_HTTP_TAG);
		ret = httpc_conn_connect(conn, uvc_httpc_server_ip, USBH_UVC_HTTPC_PORT, 0);
		if (ret == 0) {
			break;
		}
		rtos_time_delay_ms(1000);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Start %s\n", USBH_UVC_HTTP_TAG);
	uvc_httpc_is_init = 1;

	ticknow = rtos_time_get_current_system_time_ms();
	sprintf(_boundary, "rtkBoundary%d", (int)ticknow);
	sprintf(type, "multipart/form-data; boundary=%s", _boundary);
	memset(post_end1, 0x0, sizeof(post_end1));
	post_end1_length = content_length = snprintf(post_end1, sizeof(post_end1), body_end, _boundary);
	memset(post_end, 0x0, sizeof(post_end));
	content_length += snprintf(post_end, sizeof(post_end), upload_request, _boundary, filename);
	post_end1_length = strlen(post_end1);

	httpc_request_write_header_start(conn, "POST", "/cgi-bin/submit.py", type,
									 send_len + content_length);
	httpc_request_write_header(conn, (char *)"Connection", (char *)"keep-alive");
	httpc_request_write_header_finish(conn);

	ret = httpc_request_write_data(conn, (u8 *)post_end, content_length - post_end1_length);
	if (ret < 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: header write fail: %d\n", USBH_UVC_HTTP_TAG, ret);
	}

	send_buf = rtos_mem_malloc(USBH_UVC_HTTPC_WRITE_SIZE);
	if (send_buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: send_buf malloc fail\n", USBH_UVC_HTTP_TAG);
		goto exit;
	}

	/* Send PSRAM data in chunks */
	send_offset = 0;
	while (send_offset < send_len) {
		u32 chunk = send_len - send_offset;
		if (chunk > USBH_UVC_HTTPC_WRITE_SIZE) {
			chunk = USBH_UVC_HTTPC_WRITE_SIZE;
		}

		memcpy(send_buf, uvc_httpc_psramp_base + send_offset, chunk);
		ret = httpc_request_write_data(conn, send_buf, chunk);
		if (ret < 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: data write fail at offset %u: %d\n",
					 USBH_UVC_HTTP_TAG, send_offset, ret);
			goto exit;
		}
		send_offset += chunk;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "%s: Upload complete: %u bytes (%u frames)\n",
			 USBH_UVC_HTTP_TAG, send_len, send_frames);

	ret = httpc_request_write_data(conn, (u8 *)post_end1, post_end1_length);
	ret = httpc_response_read_header(conn);

exit:
	if (conn) {
		httpc_conn_close(conn);
		conn = NULL;
	}
	if (send_buf != NULL) {
		rtos_mem_free(send_buf);
	}
	uvc_httpc_is_init = 0;
	rtos_task_delete(NULL);

#else
	/* Ringbuffer mode: stream upload frame by frame (H264) or image by image (non-H264) */
	u32 total_len = 0;
	u8 *buffer_h264 = NULL;

	uvc_rb = RingBuffer_Create(uvc_buf, CONFIG_USBH_UVC_FRAME_BUF_SIZE, LOCAL_RINGBUFF, 0);
	buffer_h264 = rtos_mem_malloc(USBH_UVC_HTTPC_WRITE_SIZE);

	conn = httpc_conn_new(USBH_UVC_HTTPC_SECURE, NULL, NULL, NULL);
	if (conn == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: httpc_conn_new failed\n", USBH_UVC_HTTP_TAG);
		goto exit;
	}

	while (1) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Try to connect http server\n");
		ret = httpc_conn_connect(conn, uvc_httpc_server_ip, USBH_UVC_HTTPC_PORT, 0);
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
	sprintf(type, "multipart/form-data; boundary=%s", _boundary);
	memset(post_end1, 0x0, sizeof(post_end1));
	post_end1_length = content_length = snprintf(post_end1, sizeof(post_end1), body_end, _boundary);
	memset(post_end, 0x0, sizeof(post_end));
	content_length += snprintf(post_end, sizeof(post_end), upload_request, _boundary, "img.h264");

	post_end1_length = strlen(post_end1);
	// start a header and add Host (added automatically), Content-Type and Content-Length (added by input param)
	httpc_request_write_header_start(conn, "POST", "/cgi-bin/submit.py", type, USBH_UVC_HTTPC_VIDEO_SIZE + content_length);
	// add other header fields if necessary
	httpc_request_write_header(conn, (char *)"Connection", (char *)"keep-alive");
	// finish and send header
	httpc_request_write_header_finish(conn);

	ret = httpc_request_write_data(conn, (u8 *)post_end, content_length - post_end1_length);

	while (uvc_httpc_is_init && total_len < USBH_UVC_HTTPC_VIDEO_SIZE) {

		if (RingBuffer_Available(uvc_rb) >= USBH_UVC_HTTPC_WRITE_SIZE) {
			if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
				RingBuffer_Read(uvc_rb, buffer_h264, USBH_UVC_HTTPC_WRITE_SIZE);
				rtos_mutex_give(uvc_buf_mutex);
			}
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

	httpc_response_read_header(conn);
	goto exit;

exit:
	if (buffer_h264) {
		rtos_mem_free(buffer_h264);
	}
	if (uvc_rb) {
		RingBuffer_Destroy(uvc_rb);
		uvc_rb = NULL;
	}
	if (conn) {
		httpc_conn_close(conn);
	}
	uvc_httpc_is_init = 0;
	rtos_task_delete(NULL);

#endif
}


#endif

static int uvc_httpc_start(void)
{
	int ret = 0;
	rtos_task_t task;

	// Delay to check successful WiFi connection and obtain of an IP address
	while (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}

#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
	rtos_sema_create(&uvc_httpc_done_sema, 0U, 1U);
	/* Initialize PSRAM buffer state before creating the thread,
	 * because uvc_test (priority 4) may reach img_prepare before
	 * the httpc thread (priority 2) gets scheduled. */
	uvc_httpc_psramp_base = (u8 *)USBH_UVC_HTTPC_PSRAM_BASE_ADDR;
	uvc_httpc_psramp_write_ptr = uvc_httpc_psramp_base;
	uvc_httpc_psramp_total_len = 0;
	uvc_httpc_psramp_frame_cnt = 0;
#else
	rtos_sema_create(&uvc_httpc_save_img_sema, 0, 1);
#endif
	ret = rtos_task_create(&task, "example_usbh_uvc_httpc_thread", example_usbh_uvc_httpc_thread, NULL, 5200U, 2);
	if (ret != RTK_SUCCESS) {
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
		rtos_sema_delete(uvc_httpc_done_sema);
		uvc_httpc_done_sema = NULL;
#else
		rtos_sema_delete(uvc_httpc_save_img_sema);
		uvc_httpc_save_img_sema = NULL;
#endif
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create %s client thread fail\n", USBH_UVC_HTTP_TAG);
	}

	return ret;
}


#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
static void uvc_httpc_stop(void)
{
	while (uvc_httpc_is_init) {
		rtos_time_delay_ms(100);
	}
	if (uvc_httpc_done_sema != NULL) {
		rtos_sema_delete(uvc_httpc_done_sema);
		uvc_httpc_done_sema = NULL;
	}
}
#else
static void uvc_httpc_stop(void)
{
	while (uvc_httpc_is_init) {
		rtos_time_delay_ms(100);
	}
	if (uvc_httpc_save_img_sema != NULL) {
		rtos_sema_delete(uvc_httpc_save_img_sema);
		uvc_httpc_save_img_sema = NULL;
	}
}
#endif

#endif


#if CONFIG_USBH_UVC_HOT_PLUG
/**
 * Hotplug handling:
 * 1) Set uvc_task_exiting flag so uvc_test checks it on next loop iteration.
 * 2) Wait a short interval for uvc_test to finish processing the current frame
 *    (img_prepare + put_frame). During this window, uvc_test may still call
 *    UVC APIs on valid resources — do NOT call stream_off yet.
 * 3) Call usbh_uvc_stream_off() to wake up any blocked usbh_uvc_get_frame()
 *    (stream_deinit gives dec_sema/frame_sema, and sets stream_state=OFF).
 *    After this, get_frame returns NULL immediately (stream_state != STREAMING_ON).
 * 4) Wait for uvc_test to detect flag/NULL, break out of loop, and self-delete.
 * 5) Deinit UVC and USB host, then re-init for next attach.
 * 6) Thread priority: The hotplug handling thread MUST have higher priority than the UVC
 *    get-frame thread to ensure detach is processed promptly.
*/
static void example_usbh_uvc_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		if (rtos_sema_take(uvc_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {

			uvc_task_exiting = 1;

			/* Let uvc_test finish processing any frame it already retrieved
			 * (img_prepare + put_frame) before we tear down stream resources. */
			rtos_time_delay_ms(200);

			/* Stop streaming to wake up any blocked get_frame call.
			 * stream_deinit gives dec_sema/frame_sema, causing get_frame
			 * to return NULL (stream_state != STREAMING_ON). */
			usbh_uvc_stream_off(CONFIG_USBH_UVC_IF_NUM_0);

			/* Wait for uvc_test to detect flag, break out of loop, and self-delete.
			 * No timeout: must not deinit while uvc_test is still running,
			 * as it may call usbh_uvc_put_frame() on already-freed memory. */
			while (uvc_task != NULL) {
				rtos_time_delay_ms(100);
			}

			RTK_LOGS(TAG, RTK_LOG_INFO, "Hotplug: uvc_test exited\n");

			usbh_uvc_deinit();
			usbh_deinit();
			rtos_time_delay_ms(10);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap: 0x%x\n", rtos_mem_get_free_heap_size());

			uvc_task_exiting = 0;

			ret = usbh_init(&usbh_cfg, NULL);
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBH fail\n");
				break;
			}

			ret = usbh_uvc_init(&uvc_cfg, &uvc_cb);
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

static void example_usbh_uvc_test(void *param)
{
	usbh_uvc_frame_t *buf;
	const char *fmt_name = NULL;
	int ret = 0;
	int img_cnt = 0;
	int fail_cnt = 0;
	u32 len;

	UNUSED(param);

	while (!uvc_task_exiting) {
		if (rtos_sema_take(uvc_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}

		uvc_s_ctx.fmt_type = CONFIG_USBH_UVC_FORMAT_TYPE;
		uvc_s_ctx.width = CONFIG_USBH_UVC_WIDTH;
		uvc_s_ctx.height = CONFIG_USBH_UVC_HEIGHT;
		uvc_s_ctx.frame_rate = CONFIG_USBH_UVC_FRAME_RATE;
		uvc_s_ctx.frame_buf_size = CONFIG_USBH_UVC_FRAME_BUF_SIZE;

		if (uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_MJPEG) {
			fmt_name = "MJPEG";
		} else if (uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H264) {
			fmt_name = "H264";
		} else if (uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_YUV) {
			fmt_name = "YUV";
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Unsupport type %d\n", uvc_s_ctx.fmt_type);
			goto exit;
		}

		/* Trigger the UVC parameter setting process */
		ret = usbh_uvc_set_param(&uvc_s_ctx, CONFIG_USBH_UVC_IF_NUM_0);
		/* Check if the request itself failed immediately */
		if (ret != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set param req: %d\n", ret);
			goto exit;
		}

		/* Wait for the semaphore indicating the setting is actually completed */
		if (rtos_sema_take(uvc_setparam_sema, 5000) == RTK_SUCCESS) {
			if (uvc_setparam_status != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set paras err: %s %d*%d@%dfps status=%d\n",
						 fmt_name, uvc_s_ctx.width, uvc_s_ctx.height, uvc_s_ctx.frame_rate, uvc_setparam_status);
				goto exit;
			}
			RTK_LOGS(TAG, RTK_LOG_INFO, "Set paras ok: %s %d*%d@%dfps\n",
					 fmt_name, uvc_s_ctx.width, uvc_s_ctx.height, uvc_s_ctx.frame_rate);
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Set paras timeout: %s %d*%d@%dfps\n",
					 fmt_name, uvc_s_ctx.width, uvc_s_ctx.height, uvc_s_ctx.frame_rate);
			goto exit;
		}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
		RTK_LOGS(TAG, RTK_LOG_INFO, "Start vfs service\n");
		ret = uvc_vfs_start();
		if (ret != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Start vfs err\n");
			goto exit;
		}

		while (uvc_vfs_is_init == 0) {
			rtos_time_delay_ms(500);
		}
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
		RTK_LOGS(TAG, RTK_LOG_INFO, "Start %s client (PSRAM buffer mode)\n", USBH_UVC_HTTP_TAG);
#else
		RTK_LOGS(TAG, RTK_LOG_INFO, "Start %s client (ringbuffer mode)\n", USBH_UVC_HTTP_TAG);
#endif
		ret = uvc_httpc_start();
		if (ret != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Start httpc err\n");
			goto exit;
		}

#if (USBH_UVC_HTTPC_BUFFER_MODE == 0)
		while (uvc_httpc_is_init == 0) {
			rtos_time_delay_ms(500);
		}
#endif
#endif
		img_cnt = 0;
		fail_cnt = 0;

		RTK_LOGS(TAG, RTK_LOG_INFO, "Stream on\n");
		ret = usbh_uvc_stream_on(&uvc_s_ctx, CONFIG_USBH_UVC_IF_NUM_0);

		if (ret) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Stream on err\n");
			goto exit;
		}
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
		rx_start = SYSTIMER_TickGet();
		/* Save 2s stream.h264 to psram */
		while (img_cnt < (CONFIG_USBH_UVC_FRAME_RATE * 2) && !uvc_task_exiting) {
#else
		rx_start = SYSTIMER_TickGet();
		while (uvc_httpc_is_init && img_cnt < CONFIG_USBH_UVC_LOOP && !uvc_task_exiting) {
#endif
#elif (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
		while (uvc_vfs_is_init && !uvc_task_exiting) {
#else
		rx_start = SYSTIMER_TickGet();
		while (img_cnt < CONFIG_USBH_UVC_LOOP && !uvc_task_exiting) {
#endif
			buf = usbh_uvc_get_frame(CONFIG_USBH_UVC_IF_NUM_0);

			if (buf == NULL) {
				fail_cnt++;
				if (fail_cnt >= CONFIG_USBH_UVC_MAX_FAIL_COUNT) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Stop (fail:%d)\n", fail_cnt);
					break;
				}

				rtos_time_delay_ms(1);
				continue;
			}

			len = buf->byteused;

			if (len > CONFIG_USBH_UVC_FRAME_BUF_SIZE) {
				if (usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_IF_NUM_0) != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Put frame fail\n");
				}
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Frame %d overflow %d > %d\n", img_cnt, len, CONFIG_USBH_UVC_FRAME_BUF_SIZE);
				goto exit;
			}

			if (len > 0) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Captured frame %d, len=%d\n", img_cnt, len);
				usbh_uvc_img_prepare(buf);
			}

			if (usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_IF_NUM_0) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Put frame fail\n");
			}

			img_cnt ++;
		}

		uvc_calculate_tp(img_cnt);

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
		/* Signal the HTTP upload thread to start uploading PSRAM data */
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s: %u frames (%u bytes) ready for upload\n",
				 USBH_UVC_HTTP_TAG, uvc_httpc_psramp_frame_cnt, uvc_httpc_psramp_total_len);
		rtos_sema_give(uvc_httpc_done_sema);

		/* Wait for upload thread to finish */
		while (uvc_httpc_is_init) {
			rtos_time_delay_ms(500);
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s: Upload complete\n", USBH_UVC_HTTP_TAG);
#endif
#endif

		if (uvc_task_exiting) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "uvc_test: exiting due to detach\n");
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
			/* Wake up httpc thread so it can exit, even if capture was interrupted */
			if (uvc_httpc_done_sema != NULL) {
				rtos_sema_give(uvc_httpc_done_sema);
			}
#else
			/* Wake up httpc thread (MJPEG/ringbuffer) so it can check uvc_task_exiting */
			if (uvc_httpc_save_img_sema != NULL) {
				rtos_sema_give(uvc_httpc_save_img_sema);
			}
#endif
#endif
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
			uvc_vfs_is_init = 0;
			if (uvc_vfs_save_img_sema != NULL) {
				rtos_sema_give(uvc_vfs_save_img_sema);
			}
#endif
			goto exit;
		}
	}

exit:
	/* Clean up worker thread resources after the thread has exited.
	* Worker threads set *_is_init=0 before self-deleting. */
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
	uvc_httpc_stop();
#elif (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
	uvc_vfs_stop();
#endif

	uvc_task = NULL;
	rtos_task_delete(NULL);
}

static void example_usbh_uvc_task(void *param)
{
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

	ret = usbh_init(&usbh_cfg, NULL);
	if (ret != HAL_OK) {
		goto free_sema_exit;
	}

	ret = usbh_uvc_init(&uvc_cfg, &uvc_cb);
	if (ret != HAL_OK) {
		usbh_deinit();
		goto usb_deinit_exit;
	}

#if CONFIG_USBH_UVC_HOT_PLUG
	ret = rtos_task_create(&hotplug_task, "example_usbh_uvc_hotplug_thread", example_usbh_uvc_hotplug_thread, NULL, 768U, CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto usbh_uvc_deinit_exit;
	}
#endif

	while (1) {
		if (rtos_sema_take(uvc_attach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			if (uvc_task == NULL) {
				ret = rtos_task_create(&uvc_task, "example_usbh_uvc_test", example_usbh_uvc_test, NULL,
									   768U, CONFIG_USBH_UVC_TEST_THREAD_PRIORITY);
				if (ret != RTK_SUCCESS) {
					goto delete_hotplug_task_exit;
				}
			}
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

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
static u32 uvch_setip(u16 argc, u8 *argv[])
{
	if (argc == 0 || argv[0] == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: uvch_setip <ip>\n");
		return HAL_ERR_PARA;
	}

	snprintf(uvc_httpc_server_ip, sizeof(uvc_httpc_server_ip), "%s", (const char *)argv[0]);
	RTK_LOGS(TAG, RTK_LOG_INFO, "HTTPC server IP set to: %s\n", uvc_httpc_server_ip);

	return HAL_OK;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE uvc_httpc_test_cmd_table[] = {
	{"uvch_setip", uvch_setip},
};
#endif

/* Exported functions --------------------------------------------------------*/

void example_usbh_uvc(void)
{
	int status;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UVC demo start\n");

	status = rtos_task_create(&task, "example_usbh_uvc_thread", example_usbh_uvc_task, NULL, 768U, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}
