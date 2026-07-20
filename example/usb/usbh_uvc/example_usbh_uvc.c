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
#include "usbh_uvc.h"
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

/* Supported formats: USBH_UVC_FORMAT_MJPEG, USBH_UVC_FORMAT_YUV,
 *                    USBH_UVC_FORMAT_H264, USBH_UVC_FORMAT_H265
 * Note: Users must verify which formats their specific camera supports and
 * adjust the definition below accordingly. */
#define CONFIG_USBH_UVC_FORMAT_TYPE                USBH_UVC_FORMAT_MJPEG

/* USBH_UVC_APP_SIMPLE supports all formats. HTTPC and VFS both support MJPEG, H264 and H265
 * (H265 reuses the H264 ringbuffer streaming path). YUV is SIMPLE-only; VFS/HTTPC drop YUV frames. */
#if ((CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_YUV) && (CONFIG_USBH_UVC_APP != USBH_UVC_APP_SIMPLE))
#error "USBH_UVC_FORMAT_YUV is only supported by USBH_UVC_APP_SIMPLE. Select MJPEG, H264 or H265 for VFS/HTTPC, or set CONFIG_USBH_UVC_APP to USBH_UVC_APP_SIMPLE."
#endif

/* Target resolution and compression ratio.
 * If the specific camera device does not support
 * these values, the host stack will automatically select the closest match.
 * Always check the logs to confirm the actual parameters applied. */
#define CONFIG_USBH_UVC_WIDTH                      1280
#define CONFIG_USBH_UVC_HEIGHT                     720
#define CONFIG_USBH_UVC_FRAME_RATE                 30

/* Frame buffer size in bytes
 * Size depends on format, resolution, and scene complexity.
 * Formula:
 *   avg_frame_size (KB) = bitrate (kbps) / framerate (fps) / 8
 *   rec = avg x margin
 *
 * Margin by format:
 *   MJPEG: 3 x avg (peak frame fluctuation)
 *   H264:  5 x avg (peak I-frame; increase to 8x for high-motion)
 *   H265:  5 x avg (peak I-frame; increase to 8x for high-motion)
 *   YUY2:  no margin needed, set to width x height x 2 (uncompressed, fixed frame size)
 *
 * Examples:
 *   MJPEG @ 720p  30 fps, 10 Mbps : avg = 41 KB,  rec = 125 KB
 *   MJPEG @ 1080p 30 fps, 12 Mbps : avg = 50 KB,  rec = 150 KB
 *   H264  @ 720p  30 fps, 8 Mbps  : avg = 33 KB,  rec = 166 KB
 *   H264  @ 1080p 30 fps, 12 Mbps : avg = 50 KB,  rec = 250 KB
 *   H265  @ 720p  30 fps, 5 Mbps  : avg = 21 KB,  rec = 105 KB
 *   H265  @ 1080p 30 fps, 8 Mbps  : avg = 33 KB,  rec = 166 KB
 *   YUY2  @ 480p  30 fps : frame = 640 × 480 × 2 = 600 KB    (USB 2.0 bandwidth: 147 Mbps)
 *   YUY2  @ 720p  30 fps : frame = 1280 × 720 × 2 = 1,800 KB (USB 2.0 bandwidth: 442 Mbps)
 *   YUY2  @ 1080p 30 fps : not listed — exceeds USB 2.0 HS bandwidth (~480 Mbps theoretical);
 *                            1080p YUY2 @ 30 fps requires ~950 Mbps; use MJPEG or H264 instead.
 * Note: Increase this value if an oversize error occurs; Ensure the target hardware has
 *       sufficient heap memory for this buffer. */
#define CONFIG_USBH_UVC_FRAME_BUF_SIZE             (170 * 1024)

/* Most cameras have a single video stream interface, so use default 0.
 * If the camera supports dual streams, set this to 0 or 1.
 * Note: Current protocol stack supports a maximum of 2 video stream in HW UVC. */
#define CONFIG_USBH_UVC_STREAM_INDEX               0

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

#define CONFIG_USBH_UVC_INIT_THREAD_PRIORITY           1
#define CONFIG_USBH_UVC_MAIN_THREAD_PRIORITY           3
#define CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY        6
#define CONFIG_USBH_UVC_TEST_THREAD_PRIORITY           4
#define CONFIG_USBH_UVC_VFS_THREAD_PRIORITY            1
#define CONFIG_USBH_UVC_HTTPC_THREAD_PRIORITY          2
// Thread stack sizes
#define CONFIG_USBH_UVC_INIT_THREAD_STACK_SIZE           768U
#define CONFIG_USBH_UVC_MAIN_TASK_STACK_SIZE             1024U
#define CONFIG_USBH_UVC_HOTPLUG_THREAD_STACK_SIZE        768U
#define CONFIG_USBH_UVC_TEST_THREAD_STACK_SIZE           768U
#define CONFIG_USBH_UVC_VFS_THREAD_STACK_SIZE            3584U
#define CONFIG_USBH_UVC_HTTPC_THREAD_STACK_SIZE          5200U

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

/* HTTPC buffer mode: 0 = ringbuffer (per-frame MJPEG / streamed H264/H265), 1 = PSRAM.
 * The PSRAM path accumulates one continuous elementary stream and only handles H264/H265,
 * so mode 1 is used only for H264/H265 under HW UVC. MJPEG is ALWAYS uploaded per-frame via
 * the ringbuffer path (mode 0), even with HW UVC -- the PSRAM path has no per-JPEG framing. */
#if (USBH_UVC_USE_HW && (CONFIG_USBH_UVC_FORMAT_TYPE != USBH_UVC_FORMAT_MJPEG))
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
#define USBH_UVC_HTTPC_CONNECT_RETRY     10                  /* give up the upload after this many failed connect attempts */

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
static u8 uvc_setparam_status;
static __IO u8 uvc_task_exiting = 0;

static u64 rx_total_bytes;	/* total received bytes since rx_start; counted once per frame in the capture loop */
static u32 rx_start;

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
static rtos_sema_t uvc_vfs_save_img_sema = NULL;
static u8 uvc_vfs_is_init = 0;
static u8 uvc_vfs_thread_alive = 0;
static u32 uvc_buf_size = 0;
static RingBuffer *uvc_rb;
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
/* Sequential JPEG file index; only the MJPEG VFS thread writes per-frame files.
 * The H264 VFS thread streams to a single file and never uses this counter. */
static int uvc_vfs_img_file_no = 0;
#endif
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
/* Two independent signals, both matter for hot-plug:
 *
 *   thread_alive - set at thread creation in uvc_httpc_start(), cleared by the worker thread
 *     as the LAST thing before it self-deletes. uvc_httpc_stop() and the post-capture wait in
 *     uvc_test() block on this so deinit / re-init on re-attach never runs while the thread is
 *     still alive (still owning uvc_rb / the http conn). The only safe "thread exited" signal.
 *   connected  - set after the server connect succeeds, cleared as soon as the upload target
 *     is reached (H264/H265) or at exit. Used ONLY as the capture gate and upload-loop
 *     condition; it clears early (before teardown), so it must never be used to decide the
 *     thread has finished. Declared only for the mode0 / MJPEG builds that gate capture. */
static u8 uvc_httpc_thread_alive = 0;
#if ((USBH_UVC_HTTPC_BUFFER_MODE == 0) || (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG))
static u8 uvc_httpc_connected = 0;
#endif
static char uvc_httpc_server_ip[48] = USBH_UVC_HTTPC_SERVER;
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
/* MJPEG: frame-by-frame upload via sema signal */
static rtos_sema_t uvc_httpc_save_img_sema = NULL;
static u8 uvc_httpc_img_file_no = 0;
static u32 uvc_buf_size = 0;
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
static u32 uvc_buf_size = 0;
static rtos_sema_t uvc_httpc_save_img_sema = NULL;
#endif
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
/* PSRAM buffer state — written by usbh_uvc_img_prepare, read by upload thread */
static u8 *uvc_httpc_psramp_write_ptr = NULL;
static u32 uvc_httpc_psramp_total_len = 0;
static u8 *uvc_httpc_psramp_base = NULL;
static u32 uvc_httpc_psramp_frame_cnt = 0;
static u8 uvc_httpc_psramp_full = 0U;
static rtos_sema_t uvc_httpc_done_sema = NULL;
#endif
#endif

/* uvc_buf is referenced whenever frames pass through a local frame buffer:
 *   - VFS: single-image (MJPEG) buffer / H264 ringbuffer backing store
 *   - HTTPC ringbuffer mode (USBH_UVC_HTTPC_BUFFER_MODE == 0): MJPEG buffer / H264 ringbuffer
 *   - HTTPC MJPEG: the MJPEG upload thread copies each frame into uvc_buf and uploads it,
 *     regardless of buffer mode (it has no PSRAM branch).
 * The only case that never touches uvc_buf is HTTPC PSRAM mode (BUFFER_MODE == 1, HW UVC)
 * streaming H264, which writes directly to PSRAM, so it must not be compiled there. */
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS) || \
    ((CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC) && \
     ((USBH_UVC_HTTPC_BUFFER_MODE == 0) || (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)))
static u8 uvc_buf[CONFIG_USBH_UVC_FRAME_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));
#endif

static const usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.ext_intr_enable = USBH_SOF_INTR,
	.isr_priority = INT_PRI_MIDDLE,
	.main_task_stack_size = CONFIG_USBH_UVC_MAIN_TASK_STACK_SIZE,
	.main_task_priority = CONFIG_USBH_UVC_MAIN_THREAD_PRIORITY,
	.tick_source = USBH_SOF_TICK,
#if defined (CONFIG_AMEBAGREEN2)
	/*FIFO total depth is 1024, reserve 12 for DMA addr*/
	.rx_fifo_depth = 500U,
	.nptx_fifo_depth = 256U,
	.ptx_fifo_depth = 256U,
#elif defined (CONFIG_AMEBAL2)
	/*FIFO total depth is 1024 DWORD, reserve 11 DWORD for DMA addr*/
	.rx_fifo_depth = 501U,
	.nptx_fifo_depth = 256U,
	.ptx_fifo_depth = 256U,
#elif defined (CONFIG_AMEBAPRO3)
	/*FIFO total depth is 2232 DWORD, resv 8 DWORD for DMA addr */
	.rx_fifo_depth = 1712U,
	.nptx_fifo_depth = 256U,
	.ptx_fifo_depth = 256U,
#endif
};

static const usbh_uvc_ctx_t uvc_cfg = {
	.frame_buf_size = CONFIG_USBH_UVC_FRAME_BUF_SIZE,
#if USBH_UVC_USE_HW
	.hw_isr_pri = CONFIG_USBH_UVC_HW_IRQ_PRIORITY,
#endif
};

static const usbh_uvc_cb_t uvc_cb = {
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
	return HAL_OK;
}

static int uvc_cb_deinit(void)
{
	return HAL_OK;
}

static int uvc_cb_attach(void)
{
	rtos_sema_give(uvc_attach_sema);
	return HAL_OK;
}

static int uvc_cb_detach(void)
{
#if CONFIG_USBH_UVC_HOT_PLUG
	rtos_sema_give(uvc_detach_sema);
#endif

	return HAL_OK;
}

static int uvc_cb_setup(void)
{
	rtos_sema_give(uvc_start_sema);
	return HAL_OK;
}

static int uvc_cb_setparam(int status)
{
	uvc_setparam_status = status;
	rtos_sema_give(uvc_setparam_sema);
	return HAL_OK;
}

static void uvc_calculate_tp(u32 loop)
{
	u32 rx_elapse;		/* elapsed capture time, ms */
	u32 rx_fps;		/* frames per second */
	u32 rx_kbps;		/* throughput, KB/s */
	u32 rx_mbps_x100;	/* throughput, MB/s scaled x100 (2 decimals) */

	rx_elapse = SYSTIMER_GetPassTime(rx_start);
	if (rx_elapse == 0U) {
		rx_elapse = 1U;	/* guard against divide-by-zero on very fast/empty runs */
	}

	rx_fps = loop * 1000U / rx_elapse;

	/* rx_total_bytes is 64-bit and rx_total_bytes * 100000 overflows 32-bit, so the
	 * intermediates are computed in 64-bit; the final KB/s and MB/s results fit in u32.
	 * This is format/mode independent because rx_total_bytes is accumulated once per
	 * captured frame in the main loop (works for MJPEG/YUV/H264/H265 in every APP mode). */
	rx_kbps = (u32)(rx_total_bytes * 1000U / 1024U / rx_elapse);
	rx_mbps_x100 = (u32)(rx_total_bytes * 100000U / (1024U * 1024U) / rx_elapse);

	RTK_LOGS(TAG, RTK_LOG_INFO, "TP %u KB/s @ %u ms, fps %u/s\n", rx_kbps, rx_elapse, rx_fps);
	RTK_LOGS(TAG, RTK_LOG_INFO, "TP %u.%02u MB/s (%u bytes)\n",
			 rx_mbps_x100 / 100U, rx_mbps_x100 % 100U, (u32)rx_total_bytes);

	rx_total_bytes = 0U;
}

static void usbh_uvc_img_prepare(usbh_uvc_frame_t *frame)
{
	u32 len = frame->byteused;

#if CONFIG_USBH_UVC_CHECK_MJEPG_DATA
	//some camera may pad 0 to the end of image
	if (len == 0U) {
		return;
	}
	while (len > 2U && frame->buf[len - 1U] == 0U) {
		len--;
	}

	/* UVC Host only passes data through. */
	/* Invalid data from camera should be handled by application and must not stopping fetching the next frame. */
	if (frame->buf[0] != 0xffU || frame->buf[1] != 0xd8U || frame->buf[len - 2] != 0xffU || frame->buf[len - 1] != 0xd9U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[mjpeg] image error: %x %x %x %x\n", frame->buf[0], frame->buf[1], frame->buf[2], frame->buf[3]);
		RTK_LOGS(TAG, RTK_LOG_ERROR, "[mjpeg] image error: %x %x %x %x\n", frame->buf[len - 4U], frame->buf[len - 3U], frame->buf[len - 2U], frame->buf[len - 1U]);
		/* should not return */
		/* The application can adopt a drop mechanism here, discarding frames that do not comply with the specification without storing them */
	}
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_SIMPLE)
	/* SIMPLE: no local processing. Throughput is accounted for in the capture loop. */
	(void)len;
#endif

#if ((CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC) || (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS))
	if (rtos_mutex_take(uvc_buf_mutex, 1000U / uvc_s_ctx.frame_rate / 2U + 50U) == RTK_SUCCESS) {
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
		/* PSRAM mode: H264 H265 only — write frames to PSRAM buffer for batch upload */
		if ((uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H264) ||
			(uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H265)) {
			if (uvc_httpc_psramp_total_len + len <= USBH_UVC_HTTPC_PSRAM_BUF_SIZE) {
				memcpy((void *)uvc_httpc_psramp_write_ptr, (void *)(frame->buf), len);
				uvc_httpc_psramp_write_ptr += len;
				uvc_httpc_psramp_total_len += len;
				uvc_httpc_psramp_frame_cnt++;
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "%s: PSRAM full (%u bytes, %u frames), stop capture\n",
						 USBH_UVC_HTTP_TAG, uvc_httpc_psramp_total_len, uvc_httpc_psramp_frame_cnt);
				uvc_httpc_psramp_full = 1U;
			}
		}

		rtos_mutex_give(uvc_buf_mutex);
#else
		/* Ringbuffer mode:
		 * - H264/H265: write to ringbuffer for streaming upload
		 * - MJPEG: copy to uvc_buf + signal httpc thread for per-frame upload
		 * - YUV: not supported in HTTPC, drop frame */
		if ((uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H264) ||
			(uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H265)) {
			if ((uvc_rb != NULL) && ((u32)RingBuffer_Space(uvc_rb) > frame->byteused)) {
				RingBuffer_Write(uvc_rb, frame->buf, frame->byteused);
			}
			rtos_mutex_give(uvc_buf_mutex);
		} else if (uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_MJPEG) {
			memcpy(uvc_buf, (void *)(frame->buf), len);
			uvc_buf_size = len;
			rtos_mutex_give(uvc_buf_mutex);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "give sema %x\n", (u32)uvc_httpc_save_img_sema);
			rtos_sema_give(uvc_httpc_save_img_sema);
		}
		/* YUV not supported in HTTPC ringbuffer mode: drop frame */
#endif
#else
		if ((uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H264) ||
			(uvc_s_ctx.fmt_type == USBH_UVC_FORMAT_H265)) {
			if ((uvc_rb != NULL) && ((u32)RingBuffer_Space(uvc_rb) > frame->byteused)) {
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
	char filename[64] = {0};
	char f_num[15];
	char *prefix;
	FILE *finfo = NULL;
	int res = 0;

	UNUSED(param);

	res = vfs_user_register("sdcard", VFS_FATFS, VFS_INF_SD, VFS_REGION_4, VFS_RW);
	if (res == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "VFS-SDcard Init Success \n");
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "VFS-SDcard Init Fail \n");
		goto exit;
	}

	uvc_vfs_is_init = 1;
	uvc_vfs_thread_alive = 1U;

	prefix = find_vfs_tag(VFS_REGION_4);

	while (uvc_vfs_is_init != 0U) {
		if (rtos_sema_take(uvc_vfs_save_img_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Fail to take img_sema\n");
			continue;
		}
		if ((uvc_task_exiting != 0U) || (uvc_vfs_is_init == 0U)) {
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
			if (res != (int)uvc_buf_size) {
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
	uvc_vfs_is_init = 0U;
	RTK_LOGS(TAG, RTK_LOG_INFO, "VFS MJPEG done, %d files written to SD\n", uvc_vfs_img_file_no);
	uvc_vfs_thread_alive = 0U;
	rtos_task_delete(NULL);
}

#else

static void example_usbh_uvc_vfs_thread(void *param)
{
	char path[128] = {0};
	char filename[64] = {0};
	char *prefix;
	FILE *finfo = NULL;
	int res = 0;
	u32 total_len = 0U;
	u8 *buffer_h264 = NULL;

	UNUSED(param);

	uvc_vfs_is_init = 1U;
	uvc_vfs_thread_alive = 1U;
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
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_H265)
	strcat(filename, ".h265");
#else
	strcat(filename, ".h264");
#endif

	snprintf(path, sizeof(path), "%s:%s", prefix, filename);
	RTK_LOGS(TAG, RTK_LOG_INFO, "Create image file: %s\n", path);

	finfo = fopen(path, "wb+");
	if (finfo == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "fopen image fail\n");
		goto exit;
	}

	/* uvc_task_exiting guard: on a mid-stream detach the ring stops filling, so without this
	 * the loop would spin forever waiting for VFS_VIDEO_SIZE bytes and never reach exit: to
	 * clear uvc_vfs_is_init, close the file and destroy uvc_rb -> uvc_vfs_stop() would hang. */
	while ((total_len < USBH_UVC_VFS_VIDEO_SIZE) && (uvc_task_exiting == 0U)) {
		if (RingBuffer_Available(uvc_rb) >= USBH_UVC_VFS_WRITE_SIZE) {
			if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
				RingBuffer_Read(uvc_rb, buffer_h264, USBH_UVC_VFS_WRITE_SIZE);
				rtos_mutex_give(uvc_buf_mutex);
			}
			res = fwrite(buffer_h264, USBH_UVC_VFS_WRITE_SIZE, 1, finfo);
			if (res != (int)USBH_UVC_VFS_WRITE_SIZE) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "buf fwrite fail: %d\n", res);
				goto exit;
			}
			total_len += USBH_UVC_VFS_WRITE_SIZE;
		} else {
			rtos_time_delay_ms(2U);
		}
	}

	if (total_len >= USBH_UVC_VFS_VIDEO_SIZE) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "VFS %s done, %u bytes\n", filename, (unsigned int)total_len);
	}

exit:
	uvc_vfs_is_init = 0U;
	if (finfo != NULL) {
		fclose(finfo);
	}
	if (buffer_h264 != NULL) {
		rtos_mem_free(buffer_h264);
	}
	if (uvc_rb != NULL) {
		RingBuffer_Destroy(uvc_rb);
		uvc_rb = NULL;
	}
	uvc_vfs_thread_alive = 0U;
	rtos_task_delete(NULL);
}

#endif

static int uvc_vfs_start(void)
{
	int ret = RTK_SUCCESS;
	rtos_task_t task;

#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
	/* Restart JPEG file numbering each run (img0, img1, ...); overwrites the
	 * previous run's files on the SD card. */
	uvc_vfs_img_file_no = 0;
#endif
	rtos_sema_create(&uvc_vfs_save_img_sema, 0U, 1U);
	ret = rtos_task_create(&task, "example_usbh_uvc_vfs_thread", example_usbh_uvc_vfs_thread, NULL,
						   CONFIG_USBH_UVC_VFS_THREAD_STACK_SIZE, CONFIG_USBH_UVC_VFS_THREAD_PRIORITY);

	if (ret != RTK_SUCCESS) {
		rtos_sema_delete(uvc_vfs_save_img_sema);
		uvc_vfs_save_img_sema = NULL;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create vfs thread fail\n");
	}

	return ret;
}

static void uvc_vfs_stop(void)
{
	while (uvc_vfs_thread_alive == 1U) {
		rtos_time_delay_ms(100U);
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
	static u8 resp_drain[512];
	char post_end[256] = {0};
	char type[128] = {0};
	static char _boundary[64] = {0};
	char img_file[32];
	char post_end1[32] = {0};
	int ret = 0;
	struct httpc_conn *conn = NULL;
	u32 content_length;
	u32 post_end1_length;
	u32 ticknow = 0;
	u32 reconnect_cnt = 0;
	int drain_len;
	size_t drain_remain;
	size_t drain_want;

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
		rtos_time_delay_ms(1000U);
		if (uvc_task_exiting != 0U) {
			goto exit;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Start httpc\n");

	rtos_time_delay_ms(2000U);

	uvc_httpc_connected = 1U;

	while (1) {
		rtos_sema_take(uvc_httpc_save_img_sema, RTOS_MAX_TIMEOUT);
		if (uvc_task_exiting != 0U) {
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
			reconnect_cnt++;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Send %s request fail: %d reconn=%d\n", USBH_UVC_HTTP_TAG, ret, (int)reconnect_cnt);
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

		// receive response header, then drain body to keep the keep-alive stream byte-aligned
		if (httpc_response_read_header(conn) == 0) {
			if (conn->response.trans_enc != NULL) {
				do {
					drain_len = httpc_response_read_data(conn, resp_drain, sizeof(resp_drain));
				} while (drain_len > 0);
			} else {
				drain_remain = conn->response.content_len;
				while (drain_remain > 0U) {
					drain_want = (drain_remain < sizeof(resp_drain)) ? drain_remain : sizeof(resp_drain);
					drain_len = httpc_response_read_data(conn, resp_drain, drain_want);
					if (drain_len <= 0) {
						break;
					}
					drain_remain -= (size_t)drain_len;
				}
			}
		}

		uvc_httpc_img_file_no ++;
	}

exit:
	if (conn != NULL) {
		httpc_conn_close(conn);
	}
	uvc_httpc_connected = 0U;
	uvc_httpc_thread_alive = 0U;
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
	char post_end[256] = {0};
	char type[128] = {0};
	static char _boundary[64] = {0};
	char post_end1[32] = {0};
	int ret = 0;
	struct httpc_conn *conn = NULL;
	u32 content_length;
	u32 post_end1_length;
	u32 ticknow = 0U;
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_H265)
	char filename[32] = "stream.h265";
#else
	char filename[32] = "stream.h264";
#endif
	u32 send_len = 0U;
	u32 send_offset;
	u8 *send_buf = NULL;
	u32 send_frames = 0U;
	u32 retry;
	u32 chunk;
#else
	u32 total_len = 0;
	u8 *buffer_h264 = NULL;
#endif

	UNUSED(param);

#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
	/* PSRAM buffer mode: wait for capture to finish, then upload.
	 * PSRAM buffer state and the is_init "thread alive" flag are both set by
	 * uvc_httpc_start() before/at thread creation, because uvc_test (priority 4) may
	 * reach img_prepare and the post-capture wait before this thread (priority 2) runs. */

	/* Wait for uvc_test() to finish writing all frames to PSRAM */
	rtos_sema_take(uvc_httpc_done_sema, RTOS_SEMA_MAX_COUNT);
	if (uvc_task_exiting != 0U) {
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

	if (send_len == 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: Nothing to upload\n", USBH_UVC_HTTP_TAG);
		goto exit;
	}

	conn = httpc_conn_new(USBH_UVC_HTTPC_SECURE, NULL, NULL, NULL);
	if (conn == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: httpc_conn_new failed\n", USBH_UVC_HTTP_TAG);
		goto exit;
	}

	for (retry = 0U; retry < USBH_UVC_HTTPC_CONNECT_RETRY; retry++) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Try to connect %s server\n", USBH_UVC_HTTP_TAG);
		ret = httpc_conn_connect(conn, uvc_httpc_server_ip, USBH_UVC_HTTPC_PORT, 0);
		if (ret == 0) {
			break;
		}
		if (uvc_task_exiting != 0U) {
			goto exit;
		}
		rtos_time_delay_ms(1000U);
	}
	if (ret != 0) {
		/* Server unreachable: report the failure honestly and abort instead of retrying
		 * forever. The thread still exits cleanly (is_init cleared at exit:). */
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s: connect failed after %d attempts, upload aborted\n",
				 USBH_UVC_HTTP_TAG, USBH_UVC_HTTPC_CONNECT_RETRY);
		goto exit;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Start %s\n", USBH_UVC_HTTP_TAG);

	ticknow = rtos_time_get_current_system_time_ms();
	sprintf(_boundary, "rtkBoundary%d", (int)ticknow);
	sprintf(type, "multipart/form-data; boundary=%s", _boundary);
	memset(post_end1, 0x0U, sizeof(post_end1));
	post_end1_length = content_length = snprintf(post_end1, sizeof(post_end1), body_end, _boundary);
	memset(post_end, 0x0U, sizeof(post_end));
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
		if (uvc_task_exiting != 0U) {
			goto exit;
		}
		chunk = send_len - send_offset;
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
	if (conn != NULL) {
		httpc_conn_close(conn);
		conn = NULL;
	}
	if (send_buf != NULL) {
		rtos_mem_free(send_buf);
	}
	uvc_httpc_thread_alive = 0;
	rtos_task_delete(NULL);

#else
	/* Ringbuffer mode: stream upload frame by frame (H264) or image by image (non-H264) */
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
		if (uvc_task_exiting != 0U) {
			goto exit;
		}
		rtos_time_delay_ms(1000);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "Start httpc\n");
	rtos_time_delay_ms(2000);
	uvc_httpc_connected = 1;

	/* Wait until capture has produced the first chunk before sending the HTTP header.
	 * The header declares a fixed 2 MB Content-Length; if it is sent while the ring is
	 * still empty (capture only starts after this thread sets connected=1, then runs
	 * usbh_uvc_set_param/usbh_uvc_start), the header/body idle gap can make a strict server reset
	 * the connection -- after which every write below fails and nothing is received. */
	while (uvc_httpc_connected && (RingBuffer_Available(uvc_rb) < USBH_UVC_HTTPC_WRITE_SIZE)) {
		if (uvc_task_exiting != 0U) {
			goto exit;
		}
		rtos_time_delay_ms(2);
	}

	ticknow = rtos_time_get_current_system_time_ms();
	sprintf(_boundary, "rtkBoundary%d", (int)ticknow);
	sprintf(type, "multipart/form-data; boundary=%s", _boundary);
	memset(post_end1, 0x0, sizeof(post_end1));
	post_end1_length = content_length = snprintf(post_end1, sizeof(post_end1), body_end, _boundary);
	memset(post_end, 0x0, sizeof(post_end));
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_H265)
	content_length += snprintf(post_end, sizeof(post_end), upload_request, _boundary, "stream.h265");
#else
	content_length += snprintf(post_end, sizeof(post_end), upload_request, _boundary, "stream.h264");
#endif

	post_end1_length = strlen(post_end1);
	// start a header and add Host (added automatically), Content-Type and Content-Length (added by input param)
	httpc_request_write_header_start(conn, "POST", "/cgi-bin/submit.py", type, USBH_UVC_HTTPC_VIDEO_SIZE + content_length);
	// add other header fields if necessary
	httpc_request_write_header(conn, (char *)"Connection", (char *)"keep-alive");
	// finish and send header
	httpc_request_write_header_finish(conn);

	ret = httpc_request_write_data(conn, (u8 *)post_end, content_length - post_end1_length);

	while (uvc_httpc_connected && total_len < USBH_UVC_HTTPC_VIDEO_SIZE) {

		if (RingBuffer_Available(uvc_rb) >= USBH_UVC_HTTPC_WRITE_SIZE) {
			if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
				RingBuffer_Read(uvc_rb, buffer_h264, USBH_UVC_HTTPC_WRITE_SIZE);
				rtos_mutex_give(uvc_buf_mutex);
			}
			ret = httpc_request_write_data(conn, buffer_h264, USBH_UVC_HTTPC_WRITE_SIZE);
			if (ret < 0) {
				/* H264 is a continuous stream under a fixed 2 MB Content-Length: reconnecting
				 * mid-stream (as the MJPEG per-frame path does) would break that byte contract,
				 * so abort honestly instead of counting failed writes as progress. */
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Send %s data fail: %d, abort upload at %d bytes\n",
						 USBH_UVC_HTTP_TAG, ret, (int)total_len);
				goto exit;
			}
			total_len += USBH_UVC_HTTPC_WRITE_SIZE;
		} else {
			if (uvc_task_exiting != 0U) {
				break;
			}
			rtos_time_delay_ms(2);
		}
	}

	/* Upload target reached (or detached): stop the capture loop now, before finalizing and
	 * logging, so no further "Captured frame" is logged or counted after "Upload complete".
	 * uvc_rb is torn down later under uvc_buf_mutex in the exit path. */
	uvc_httpc_connected = 0;

	/* Only finalize on a real 2 MB completion. On a detach (uvc_task_exiting) the fixed 2 MB
	 * Content-Length promised in the header can no longer be met, so skip post_end1 and the
	 * response read entirely: sending a short body and then blocking in read_header would
	 * stall teardown until the server times out, delaying release of uvc_rb / the conn. */
	if (uvc_task_exiting == 0U) {
		ret = httpc_request_write_data(conn, (u8 *)post_end1, post_end1_length);
		httpc_response_read_header(conn);
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s: Upload complete: %d bytes\n", USBH_UVC_HTTP_TAG, (int)total_len);
	}
	goto exit;

exit:
	if (buffer_h264 != NULL) {
		rtos_mem_free(buffer_h264);
		buffer_h264 = NULL;
	}
	/* Stop the capture loop from fetching more frames before the ring is torn down,
	 * then destroy uvc_rb under uvc_buf_mutex so it cannot race with usbh_uvc_img_prepare,
	 * which also touches uvc_rb only while holding that mutex. Without this, capture could
	 * call RingBuffer_Space/Write on a just-freed (NULL) uvc_rb and take a data abort. */
	uvc_httpc_connected = 0;
	if (rtos_mutex_take(uvc_buf_mutex, RTOS_MAX_TIMEOUT) == RTK_SUCCESS) {
		if (uvc_rb != NULL) {
			RingBuffer_Destroy(uvc_rb);
			uvc_rb = NULL;
		}
		rtos_mutex_give(uvc_buf_mutex);
	}
	if (conn != NULL) {
		httpc_conn_close(conn);
	}
	/* Cleared LAST -- after uvc_rb is destroyed and the conn closed -- so uvc_httpc_stop()'s
	 * "while thread_alive" only returns once every resource this thread owned is released. */
	uvc_httpc_thread_alive = 0U;
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
	uvc_httpc_psramp_full = 0U;
#else
	rtos_sema_create(&uvc_httpc_save_img_sema, 0, 1);
#endif
	ret = rtos_task_create(&task, "example_usbh_uvc_httpc_thread", example_usbh_uvc_httpc_thread, NULL,
						   CONFIG_USBH_UVC_HTTPC_THREAD_STACK_SIZE, CONFIG_USBH_UVC_HTTPC_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
		rtos_sema_delete(uvc_httpc_done_sema);
		uvc_httpc_done_sema = NULL;
#else
		rtos_sema_delete(uvc_httpc_save_img_sema);
		uvc_httpc_save_img_sema = NULL;
#endif
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create %s client thread fail\n", USBH_UVC_HTTP_TAG);
	} else {
		/* Mark the worker thread alive from the creating (higher-priority) thread, before
		 * capture starts, so uvc_test()'s post-capture wait and uvc_httpc_stop() reliably
		 * block until the thread exits. The thread clears it at its exit: path. Set here
		 * (not after connect) so a never-reachable server no longer makes those waits fall
		 * through. Applies to every mode: the mode0 (ringbuffer/MJPEG) worker and the mode1
		 * (PSRAM) worker all clear thread_alive as their last act before self-deleting. */
		uvc_httpc_thread_alive = 1U;
	}

	return ret;
}


#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
static void uvc_httpc_stop(void)
{
	while (uvc_httpc_thread_alive != 0U) {
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
	/* Wait on thread_alive, NOT connected: the H264/H265 worker clears connected early (as
	 * soon as 2 MB is sent) while it may still be blocked in httpc_response_read_header and
	 * still owns uvc_rb. Waiting on connected here would return too early and let the caller
	 * deinit / re-init on re-attach while the old thread is alive -> uvc_rb use-after-free. */
	while (uvc_httpc_thread_alive != 0U) {
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
 *    UVC APIs on valid resources — do NOT tear down resources yet.
 * 3) Call usbh_uvc_stop() to halt ISOC transfer and move the stream out of ACTIVE.
 *    stop() also wakes a blocked usbh_uvc_get_frame() (gives the frame/dec sema)
 *    so it returns NULL promptly instead of waiting out its timeout; any later
 *    call returns NULL immediately (stream_state != ACTIVE). Resources are kept.
 * 4) Wait for uvc_test to detect flag/NULL, break out of loop, and self-delete.
 * 5) Deinit UVC (frees the stream resources) and USB host, then re-init for next attach.
 * 6) Thread priority: The hotplug handling thread MUST have higher priority than the UVC
 *    get-frame thread to ensure detach is processed promptly.
*/
static void example_usbh_uvc_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	while (1) {
		if (rtos_sema_take(uvc_detach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {

			uvc_task_exiting = 1;

			/* Let uvc_test finish processing any frame it already retrieved
			 * (img_prepare + put_frame) before we tear down stream resources. */
			rtos_time_delay_ms(200);

			/* Halt ISOC transfer and move the stream out of ACTIVE. stop() also
			 * gives the frame/dec sema, so a blocked usbh_uvc_get_frame() wakes and
			 * returns NULL promptly (no ~1000 ms timeout wait); any subsequent call
			 * returns NULL immediately (stream_state != ACTIVE), so uvc_test observes
			 * uvc_task_exiting and exits. The stream resources themselves are kept and
			 * freed later by usbh_uvc_deinit(). */
			usbh_uvc_stop(CONFIG_USBH_UVC_STREAM_INDEX);

			/* Wake uvc_test if it is waiting for the next attach in the outer loop. */
			rtos_sema_give(uvc_start_sema);
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC) && (USBH_UVC_HTTPC_BUFFER_MODE == 0)
			/* MJPEG upload thread runs while(1); give its sema so it checks uvc_task_exiting and exits. */
			if (uvc_httpc_save_img_sema != NULL) {
				rtos_sema_give(uvc_httpc_save_img_sema);
			}
#endif

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

/*
 * API semantics -- read before use:
 *   usbh_uvc_init  BUFFER ALLOC : malloc frame buffers (cfg.frame_buf_size x USBH_UVC_VIDEO_FRAME_NUMS)
 *                               for every stream and bring them to READY. Called once at startup.
 *   usbh_uvc_set_param NEGOTIATE: send PROBE/COMMIT/SET_INTERFACE to camera (non-blocking).
 *                               Result via cb_setparam; wait uvc_setparam_sema before usbh_uvc_start.
 *   usbh_uvc_start DATA ON      : flush stale frames then begin ISOC transfer -- frames available via get_frame.
 *   usbh_uvc_stop  DATA OFF     : halt ISOC transfer; frame buffer memory stays allocated
 *                               (contents are flushed on the next usbh_uvc_start, not preserved).
 *   usbh_uvc_deinit  BUFFER FREE: free frame buffers for every stream; also unblocks any pending
 *                               get_frame. Called once at shutdown / on detach (hotplug).
 *
 * Typical single session (per attach):
 *   [usbh_uvc_init done at startup] -> usbh_uvc_set_param -> [wait sema] -> usbh_uvc_start ->
 *   [capture] -> usbh_uvc_stop
 *
 * Switch format/resolution mid-session (buffers already allocated at init -- no re-malloc):
 *   usbh_uvc_stop -> update uvc_s_ctx -> usbh_uvc_set_param -> [wait sema] -> usbh_uvc_start
 */
static void example_usbh_uvc_test(void *param)
{
	usbh_uvc_frame_t *buf;
	const char *fmt_name = NULL;
	int ret = 0;
	int img_cnt = 0;
	int fail_cnt = 0;
	u32 len;

	UNUSED(param);

	/* Re-runs each attach; uvc_start_sema is given by cb_attach after device enumeration. */
	while (uvc_task_exiting == 0U) {
		if (rtos_sema_take(uvc_start_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		/* hotplug handler gives this sema to unblock us after normal test completion. */
		if (uvc_task_exiting != 0U) {
			goto exit;
		}

		uvc_s_ctx.fmt_type = CONFIG_USBH_UVC_FORMAT_TYPE;
		uvc_s_ctx.width = CONFIG_USBH_UVC_WIDTH;
		uvc_s_ctx.height = CONFIG_USBH_UVC_HEIGHT;
		uvc_s_ctx.frame_rate = CONFIG_USBH_UVC_FRAME_RATE;
		/* frame_buf_size is configured once via uvc_cfg at usbh_uvc_init(); the
		 * frame buffers for every stream are already allocated by then. */

		/* fmt_name is only a log label.
		* The UVC driver streams whatever fmt_type the camera negotiates -- these names are just
		* the formats this example can save/upload (gated at compile time above).
		* Unknown types still stream in SIMPLE mode and only get a label here. */
		switch (uvc_s_ctx.fmt_type) {
		case USBH_UVC_FORMAT_MJPEG:
			fmt_name = "MJPEG";
			break;
		case USBH_UVC_FORMAT_H264:
			fmt_name = "H264";
			break;
		case USBH_UVC_FORMAT_YUV:
			fmt_name = "YUV";
			break;
		case USBH_UVC_FORMAT_H265:
			/* H.265 is fully supported: SIMPLE stream, VFS save (stream.h265) and HTTPC
			 * upload (mode0 ringbuffer) all reuse the H.264 code path. */
			fmt_name = "H265";
			break;
		default:
			fmt_name = "OTHER";
			RTK_LOGS(TAG, RTK_LOG_WARN, "Unrecognized fmt %d: stream only, no save/check\n", uvc_s_ctx.fmt_type);
			break;
		}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
		RTK_LOGS(TAG, RTK_LOG_INFO, "Start vfs service\n");
		ret = uvc_vfs_start();
		if (ret != 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Start vfs err\n");
			goto exit;
		}

		while (uvc_vfs_is_init == 0U) {
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
		while ((uvc_httpc_connected == 0U) && (uvc_task_exiting == 0U)) {
			rtos_time_delay_ms(500);
		}
		if (uvc_task_exiting != 0U) {
			goto exit;
		}
#endif
#endif
		img_cnt = 0;
		fail_cnt = 0;

		/* Stream resources (frame buffers/semaphores/threads) were allocated for every
		 * stream in usbh_uvc_init(); no per-session open is needed here.
		 * To switch parameters (fmt_type/width/height/frame_rate):
		 * usbh_uvc_stop -> update uvc_s_ctx fields -> usbh_uvc_set_param -> [wait sema] -> usbh_uvc_start */

		/* Trigger the UVC parameter setting process */
		ret = usbh_uvc_set_param(&uvc_s_ctx, CONFIG_USBH_UVC_STREAM_INDEX);
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

		/* Begin ISOC transfer; state: READY -> ACTIVE. */
		usbh_uvc_start(CONFIG_USBH_UVC_STREAM_INDEX);
#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
		rx_start = SYSTIMER_TickGet();
		/* Save 2s stream.h264 to psram */
		while ((img_cnt < (CONFIG_USBH_UVC_FRAME_RATE * 2)) && (uvc_task_exiting == 0U) && (uvc_httpc_psramp_full == 0U)) {
#else
		rx_start = SYSTIMER_TickGet();
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
		/* MJPEG: per-frame upload, stop after CONFIG_USBH_UVC_LOOP frames. */
		while ((uvc_httpc_connected != 0U) && (img_cnt < CONFIG_USBH_UVC_LOOP) && (uvc_task_exiting == 0U)) {
#else
		/* H264/H265: continuous stream with a FIXED 2 MB Content-Length. The upload thread
		 * clears uvc_httpc_connected once 2 MB has been sent, which stops this loop -- no
		 * frame-count cap, so a low-bitrate stream still fills the declared 2 MB body. */
		while ((uvc_httpc_connected != 0U) && (uvc_task_exiting == 0U)) {
#endif
#endif
#elif (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
#if (CONFIG_USBH_UVC_FORMAT_TYPE == USBH_UVC_FORMAT_MJPEG)
		/* MJPEG: per-frame JPEG files, stop after CONFIG_USBH_UVC_LOOP frames. */
		while ((uvc_vfs_is_init != 0U) && (img_cnt < CONFIG_USBH_UVC_LOOP) && (uvc_task_exiting == 0U)) {
#else
		/* H264/H265: single stream file. The writer clears uvc_vfs_is_init once 2 MB has
		 * been written, which stops this loop -- no frame-count cap, so a low-bitrate
		 * stream still reaches the 2 MB target. */
		while ((uvc_vfs_is_init != 0U) && (uvc_task_exiting == 0U)) {
#endif
#else
		rx_start = SYSTIMER_TickGet();
		while ((img_cnt < CONFIG_USBH_UVC_LOOP) && (uvc_task_exiting == 0U)) {
#endif
			buf = usbh_uvc_get_frame(CONFIG_USBH_UVC_STREAM_INDEX);

			if (buf == NULL) {
				fail_cnt++;
				if (fail_cnt >= CONFIG_USBH_UVC_MAX_FAIL_COUNT) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Stop (fail:%d)\n", fail_cnt);
					break;
				}

				rtos_time_delay_ms(1);
				continue;
			}

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC) && (USBH_UVC_HTTPC_BUFFER_MODE == 0)
			/* Upload thread finished 2 MB and cleared uvc_httpc_connected: stop capturing now so
			 * no frame is logged or counted after "Upload complete". Return this frame unused. */
			if (uvc_httpc_connected == 0U) {
				if (usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_STREAM_INDEX) != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Put frame fail\n");
				}
				break;
			}
#endif

			len = buf->byteused;

			/* The host stack clamps byteused to frame_buffer_size (see usbh_uvc_stream.c:
			 * bytes = MIN(maxlen, payload_len)), so len can only ever reach, never exceed,
			 * CONFIG_USBH_UVC_FRAME_BUF_SIZE. Reaching it means the camera frame was larger
			 * than the buffer and the tail was silently truncated -> must report an error. */
			if (len >= CONFIG_USBH_UVC_FRAME_BUF_SIZE) {
				if (usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_STREAM_INDEX) != HAL_OK) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Put frame fail\n");
				}
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Frame %d truncated: len %d reached buf size %d, increase CONFIG_USBH_UVC_FRAME_BUF_SIZE\n", img_cnt, len,
						 CONFIG_USBH_UVC_FRAME_BUF_SIZE);
				goto exit;
			}

			if (len > 0U) {
				/* Account for throughput here, once per captured frame, so the TP figure is
				 * correct for every APP mode and pixel format (MJPEG/YUV/H264/H265). */
				rx_total_bytes += len;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Captured frame %d, len=%d\n", img_cnt, len);
				usbh_uvc_img_prepare(buf);
			}

			if (usbh_uvc_put_frame(buf, CONFIG_USBH_UVC_STREAM_INDEX) != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Put frame fail\n");
			}

			img_cnt ++;
		}

		/* Halt ISOC; state: ACTIVE -> READY. Frame buffers remain (freed only by usbh_uvc_deinit). */
		usbh_uvc_stop(CONFIG_USBH_UVC_STREAM_INDEX);
		uvc_calculate_tp(img_cnt);

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_VFS)
		/* Normal completion (MJPEG reached the frame count; H264/H265 already cleared
		 * uvc_vfs_is_init at 2 MB). Clear it and wake the MJPEG writer so it leaves its
		 * sema-wait and self-deletes -- symmetric with the H264/H265 writer -- so no
		 * writer thread is left hung and uvc_vfs_start() is not double-invoked on the
		 * next attach. No-op for H264/H265 (is_init already 0; sema unused). */
		if (uvc_task_exiting == 0U) {
			uvc_vfs_is_init = 0U;
			if (uvc_vfs_save_img_sema != NULL) {
				rtos_sema_give(uvc_vfs_save_img_sema);
			}
		}
#endif

#if (CONFIG_USBH_UVC_APP == USBH_UVC_APP_HTTPC)
#if (USBH_UVC_HTTPC_BUFFER_MODE == 1)
		/* Signal the HTTP upload thread to start uploading PSRAM data */
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s: %u frames (%u bytes) ready for upload\n",
				 USBH_UVC_HTTP_TAG, uvc_httpc_psramp_frame_cnt, uvc_httpc_psramp_total_len);
		rtos_sema_give(uvc_httpc_done_sema);

		/* Block until the upload thread has fully exited (is_init stays 1 for its whole
		 * lifetime now). The thread logs the real outcome itself: "Upload complete: N bytes"
		 * on success, or a connect/write-fail error otherwise -- so no success message is
		 * printed here, which previously fired even when nothing was ever uploaded. */
		while (uvc_httpc_thread_alive == 1U) {
			rtos_time_delay_ms(100);
		}
#endif
#endif

		if (uvc_task_exiting == 1U) {
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
			/* Do NOT clear uvc_vfs_is_init here -- let the vfs thread clear it at its own exit
			 * so uvc_vfs_stop() blocks until the file is closed and uvc_rb is destroyed (safe
			 * hot-plug). The MJPEG thread exits via its uvc_task_exiting check after this sema
			 * give; the H264/H265 thread exits via the uvc_task_exiting loop guard. */
			if (uvc_vfs_save_img_sema != NULL) {
				rtos_sema_give(uvc_vfs_save_img_sema);
			}
#endif
			goto exit;
		}
		/* Normal completion: halt ISOC before waiting for the next start. Stream
		 * resources stay allocated (freed only by usbh_uvc_deinit). */
		usbh_uvc_stop(CONFIG_USBH_UVC_STREAM_INDEX);
	}

exit:
	/* Halt ISOC on every terminal exit path (detach, frame truncation, init
	 * failure). Stream resources (frame buffers/semaphores/threads) are NOT freed
	 * here -- they were allocated once in usbh_uvc_init() and are released by
	 * usbh_uvc_deinit() (called by the hotplug handler on detach, or by the main
	 * task on shutdown). usbh_uvc_stop is idempotent (no-op once not ACTIVE). */
	usbh_uvc_stop(CONFIG_USBH_UVC_STREAM_INDEX);

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

static void example_usbh_uvc_task(void *param) {
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
	ret = rtos_task_create(&hotplug_task, "example_usbh_uvc_hotplug_thread",
						   example_usbh_uvc_hotplug_thread, NULL,
						   CONFIG_USBH_UVC_HOTPLUG_THREAD_STACK_SIZE, CONFIG_USBH_UVC_HOTPLUG_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto usbh_uvc_deinit_exit;
	}
#endif

	while (1) {
		if (rtos_sema_take(uvc_attach_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			ret = rtos_task_create(&uvc_task, "example_usbh_uvc_test", example_usbh_uvc_test, NULL,
								   CONFIG_USBH_UVC_TEST_THREAD_STACK_SIZE, CONFIG_USBH_UVC_TEST_THREAD_PRIORITY);
			if (ret != RTK_SUCCESS) {
				goto delete_hotplug_task_exit;
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
static u32 uvch_set_ip(u16 argc, u8 * argv[]) {
	if (argc == 0 || argv[0] == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Usage: uvch_set_ip <ip>\n");
		return HAL_ERR_PARA;
	}

	snprintf(uvc_httpc_server_ip, sizeof(uvc_httpc_server_ip), "%s", (const char *)argv[0]);
	RTK_LOGS(TAG, RTK_LOG_INFO, "HTTPC server IP set to: %s\n", uvc_httpc_server_ip);

	return HAL_OK;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE uvc_httpc_test_cmd_table[] = {
	{"uvch_set_ip", uvch_set_ip},
};
#endif

/* Exported functions --------------------------------------------------------*/

void example_usbh_uvc(void) {
	int ret;
	rtos_task_t task;

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBH UVC demo start\n");

	ret = rtos_task_create(&task, "example_usbh_uvc_thread", example_usbh_uvc_task, NULL,
						   CONFIG_USBH_UVC_INIT_THREAD_STACK_SIZE, CONFIG_USBH_UVC_INIT_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create thread fail\n");
	}
}
