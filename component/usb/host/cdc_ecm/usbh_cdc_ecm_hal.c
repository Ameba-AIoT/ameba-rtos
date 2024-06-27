/**
  ******************************************************************************
  * @file    usbh_cdc_ecm_hal.c
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2023-01-13
  * @brief   This file provides the apis for USB CDC ECM host class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------ */
#include <platform_autoconf.h>
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include "usbh_cdc_ecm.h"
#include "usbh_cdc_ecm_hal.h"


/* Private defines -----------------------------------------------------------*/
#define CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST   1     /* Hot plug test */
#define USBH_ECM_RX_SPEED_CHECK             0     /* CDC ECM rx speed test */
#define USBH_ECM_TX_SPEED_CHECK             0     /* CDC ECM tx speed test */

#define USBH_CDC_ECM_INTR_BUF_SIZE          256   /* Buffer size for INTR loopback test, which should match with device INTR loopback buffer size */
#define USBH_CDC_ECM_LOOPBACK_CNT           100   /* Loopback test round */

#define USBH_CORE_INIT_FAIL                 (1)
#define USBH_CLASS_INIT_FAIL                (2)

#define USBH_ECM_RX_THREAD_PRIORITY           3
#define USBH_ECM_INIT_THREAD_PRIORITY         4
#define USBH_ECM_MAIN_THREAD_PRIORITY         4
#define USBH_ECM_ISR_THREAD_PRIORITY          5
#define USBH_ECM_HOTPLUG_THREAD_PRIORITY      8

#if USBH_ECM_RX_SPEED_CHECK | USBH_ECM_TX_SPEED_CHECK
extern uint32_t xTaskGetTickCount(void);
#endif

/* Private types -------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)
typedef struct {
	usb_report_data             report_data;        //usb rx callback function

	usb_os_sema_t               cdc_ecm_tx_sema;    //usb tx send sema
	volatile u8                 cdc_ecm_is_ready;   //ecm attached status
	volatile u8                 ecm_hw_connect;     //ecm ethernet connect status:0 disconnect,1 connect
	volatile u8                 ecm_init_success;   //usb init success
} usbh_cdc_ecm_host_hal_t;
#pragma pack(pop)

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int cdc_ecm_cb_init(void);
static int cdc_ecm_cb_deinit(void);
static int cdc_ecm_cb_attach(void);
static int cdc_ecm_cb_detach(void);
static int cdc_ecm_cb_setup(void);
static int cdc_ecm_cb_process(usb_host_t *host, u8 id);
static int cdc_ecm_cb_bulk_send(usbh_urb_state_t state);
static int cdc_ecm_cb_bulk_receive(u8 *pbuf, u32 Len);
static int cdc_ecm_cb_intr_receive(u8 *buf, u32 length);
static int usbh_cdc_ecm_doinit(void);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "ECMH";

static u8 cdc_ecm_intr_rx_buf[USBH_CDC_ECM_INTR_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

static usb_os_sema_t cdc_ecm_detach_sema;
static usb_os_sema_t cdc_ecm_ctrl_start_sema;
static usb_os_sema_t cdc_ecm_intr_start_sema;
static usb_os_sema_t cdc_ecm_bulk_start_sema;

static usbh_cdc_ecm_host_hal_t usbh_cdc_ecm_host_user;

static usbh_config_t usbh_ecm_cfg = {
	.pipes = 5U,
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1U,
	.main_task_priority = USBH_ECM_MAIN_THREAD_PRIORITY,
	.isr_task_priority  = USBH_ECM_ISR_THREAD_PRIORITY,
	.ptx_fifo_first     = 0U,
};

static usbh_cdc_ecm_state_cb_t cdc_ecm_usb_cb = {
	.init   = cdc_ecm_cb_init,
	.deinit = cdc_ecm_cb_deinit,
	.attach = cdc_ecm_cb_attach,
	.detach = cdc_ecm_cb_detach,
	.setup  = cdc_ecm_cb_setup,
	.bulk_send     = cdc_ecm_cb_bulk_send,
	.bulk_received = cdc_ecm_cb_bulk_receive,
	.intr_received = cdc_ecm_cb_intr_receive,
};

static usbh_user_cb_t usbh_ecm_usr_cb = {
	.process = cdc_ecm_cb_process
};

/* Private functions ---------------------------------------------------------*/
static int usbh_cdc_ecm_get_usb_status(void)//
{
	return usbh_cdc_ecm_host_user.cdc_ecm_is_ready ;
}

static int cdc_ecm_cb_init(void)
{
	RTK_LOGS(TAG, "[ECMH] INIT\n");
	return HAL_OK;
}

static int cdc_ecm_cb_deinit(void)
{
	RTK_LOGS(TAG, "[ECMH] DEINIT\n");
	usbh_cdc_ecm_host_user.ecm_hw_connect = 0;
	return HAL_OK;
}

static int cdc_ecm_cb_attach(void)
{
	RTK_LOGS(TAG, "[ECMH] ATTACH\n");
	return HAL_OK;
}

static int cdc_ecm_cb_detach(void)
{
	RTK_LOGS(TAG, "[ECMH] DETACH\n");
	usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 0;
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	usb_os_sema_give(cdc_ecm_detach_sema);
#endif

	return HAL_OK;
}

static int cdc_ecm_cb_setup(void)
{
	RTK_LOGS(TAG, "[ECMH] SETUP\n");
	usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 1;
	usb_os_sema_give(cdc_ecm_intr_start_sema);
	usb_os_sema_give(cdc_ecm_bulk_start_sema);
	usb_os_sema_give(cdc_ecm_ctrl_start_sema);

	return HAL_OK;
}

static int cdc_ecm_cb_bulk_receive(u8 *buf, u32 length)
{
	//RTK_LOGS(TAG, "[ECMH] bulk receive(%d)\n",length);
#if USBH_ECM_RX_SPEED_CHECK
	static u64 usb_rx_start_time = 0, usb_rx_end_time, usb_rx_interval_time;
	static u64 usb_rx_total_len = 0;

	usb_rx_total_len += length;
	if (usb_rx_start_time == 0) {
		usb_rx_start_time = xTaskGetTickCount();
	}
	usb_rx_end_time = xTaskGetTickCount();
	usb_rx_interval_time = (usb_rx_end_time - usb_rx_start_time) * RTOS_TICK_RATE_MS;

	if (usb_rx_interval_time >= 2000) {
		RTK_LOGS(TAG, "[ECMH] Heap %d RX %dB in %d ms, %d Kbps\n",
				 rtos_mem_get_free_heap_size(),
				 (u32)usb_rx_total_len, (u32)usb_rx_interval_time, (u32)((usb_rx_total_len * 8 * 1000) / (usb_rx_interval_time * 1024)));
		usb_rx_start_time = usb_rx_end_time;
		usb_rx_total_len = 0;
	}
#endif

	if (usbh_cdc_ecm_host_user.report_data != NULL) {
		usbh_cdc_ecm_host_user.report_data(buf, length);
	}

	return HAL_OK;
}

static int cdc_ecm_cb_bulk_send(usbh_urb_state_t state)
{
	usb_os_sema_give(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema);
	if (state != USBH_URB_DONE) {
		RTK_LOGS(TAG, "[ECMH] BULK TX fail %d\n", state);
	}

	return HAL_OK;
}

//add parse to get the ethernet status
static int cdc_ecm_cb_intr_receive(u8 *buf, u32 length)
{
	if (buf && length >= 8) {
		//RTK_LOGS(TAG, "[ECMH] Data(%02x %02x %02x %02x )\n",buf[0],buf[1],buf[2],buf[3]);
		/*A1 00 00 00 01 00 00 00 */
		if (length == 8 && buf[0] == 0xA1 && buf[1] == CDC_ECM_NOTIFY_NETWORK_CONNECTION) {
			usbh_cdc_ecm_host_user.ecm_hw_connect = buf[2];
		}
		/*A1 2A 00 00 01 00 00 08 	00 00 00 00 00 00 00 00 */
		else if (length == 16 && buf[0] == 0xA1 && buf[1] == CDC_ECM_NOTIFY_CONNECTION_SPEED_CHANGE) {
			//usbh_cdc_ecm_speed_change_t
		}
	}

	return HAL_OK;
}

static int cdc_ecm_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);
	switch (id) {
	case USBH_MSG_USER_SET_CONFIG:
		usbh_cdc_ecm_choose_config(host);	//choose ecm config
		break;
	case USBH_MSG_DISCONNECTED:
		usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 0;
		break;

	case USBH_MSG_CONNECTED:
		break;

	default:
		break;
	}

	return HAL_OK;
}

static void ecm_intr_rx_thread(void *param)
{
	int i = 0;
	UNUSED(param);
	RTK_LOGS(TAG, "[ECMH] INTR test pending, wait device ready\n");

	if (usb_os_sema_take(cdc_ecm_intr_start_sema, USB_OS_SEMA_TIMEOUT) == HAL_OK) {
		//RTK_LOGS(TAG, "[ECMH] INTR TX task start\n");

		do {
			i++;
			if (!usbh_cdc_ecm_get_usb_status()) { //18s
				if ((i % 10 == 0)) {
					RTK_LOGS(TAG, "[USBH] Device disconnected %d\n", usbh_cdc_ecm_get_usb_status());
				}
				//device disconnect, try again after sleep 1s
				usb_os_sleep_ms(1000);
				continue;
			}
			if (HAL_OK != usbh_cdc_ecm_intr_receive(cdc_ecm_intr_rx_buf, USBH_CDC_ECM_INTR_BUF_SIZE)) {
				usb_os_sleep_ms(usbh_cdc_ecm_get_intr_interval());
			}
		} while (1);
	}

	rtos_task_delete(NULL);
}

static void ecm_bulk_rx_thread(void *param)
{
	u32 i = 0;
	UNUSED(param);
	RTK_LOGS(TAG, "[ECMH] BULK test pending, wait device ready\n");

	if (usb_os_sema_take(cdc_ecm_bulk_start_sema, USB_OS_SEMA_TIMEOUT) == HAL_OK) {
		//RTK_LOGS(TAG, "[ECMH] BULK TX task start\n");

		do {
			i++;
			if (!usbh_cdc_ecm_get_usb_status()) {
				if (i % 10 == 0) {
					RTK_LOGS(TAG, "[USBH] Device disconnected %d\n", usbh_cdc_ecm_get_usb_status());
				}
				usb_os_sleep_ms(1000);
				continue;
			}
			//wait ECM connect success
			if (!usbh_cdc_ecm_get_connect_status()) {
				usb_os_sleep_ms(2000);
				continue;
			}
			if (HAL_OK != usbh_cdc_ecm_bulk_receive()) {
				usb_os_sleep_ms(500);
			}
		} while (1);
	}

	rtos_task_delete(NULL);
}

#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
static void ecm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		usb_os_sema_take(cdc_ecm_detach_sema, USB_OS_SEMA_TIMEOUT);
		usbh_cdc_ecm_deinit();
		usbh_deinit();
		RTK_LOGS(TAG, "[ECMH] Free heap size: 0x%08x\n", usb_os_get_free_heap_size());

		ret = usbh_cdc_ecm_doinit();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, "[ECMH] Init fail %d\n", ret);
			break;
		}
	}

	rtos_task_delete(NULL);
}
#endif

static int usbh_cdc_ecm_doinit(void)
{
	int status;

	status = usbh_init(&usbh_ecm_cfg, &usbh_ecm_usr_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, "[ECMH] Host init fail %d\n", status);
		return USBH_CORE_INIT_FAIL;
	}

	status = usbh_cdc_ecm_init(&cdc_ecm_usb_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, "[ECMH] Init driver fail %d\n", status);
		return USBH_CLASS_INIT_FAIL;
	}

	if (usb_os_sema_take(cdc_ecm_ctrl_start_sema, USB_OS_SEMA_TIMEOUT) == HAL_OK) {
		do {
			status = usbh_cdc_ecm_pre_ctrl_set();
			if (HAL_OK == status) {
				break;
			}
		} while (1);
	}

	return HAL_OK;
}

static void usbh_cdc_ecm_init_thread(void *param)
{
	int status;
	usb_os_task_t intr_task;
	usb_os_task_t bulk_task;
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	usb_os_task_t hotplug_task;
#endif

	UNUSED(param);
	usb_os_sema_create(&(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema));
	usb_os_sema_create(&cdc_ecm_detach_sema);
	usb_os_sema_create(&cdc_ecm_ctrl_start_sema);
	usb_os_sema_create(&cdc_ecm_intr_start_sema);
	usb_os_sema_create(&cdc_ecm_bulk_start_sema);

	status = usbh_cdc_ecm_doinit();
	if (USBH_CORE_INIT_FAIL == status) {
		goto free_sema_exit;
	} else if (USBH_CLASS_INIT_FAIL == status) {
		goto usb_deinit_exit;
	}

#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	status = rtos_task_create(&hotplug_task, "ecm_hotplug_thread", ecm_hotplug_thread, NULL, 1024U, USBH_ECM_HOTPLUG_THREAD_PRIORITY);
	if (status != SUCCESS) {
		RTK_LOGS(TAG, "[ECMH] Create hotplug check task fail\n");
		goto usb_ecm_deinit_exit;
	}
#endif

	status = rtos_task_create(&intr_task, "ecm_intr_rx_thread", ecm_intr_rx_thread, NULL, 1024U, USBH_ECM_RX_THREAD_PRIORITY);
	if (status != SUCCESS) {
		RTK_LOGS(TAG, "[ECMH] Create INTR RX task fail\n");
		goto usbh_ecm_cdc_deinit_exit;
	}

	status = rtos_task_create(&bulk_task, "ecm_bulk_rx_thread", ecm_bulk_rx_thread, NULL, 1024U, USBH_ECM_RX_THREAD_PRIORITY);
	if (status != SUCCESS) {
		RTK_LOGS(TAG, "[ECMH] Create BULK RX thread fail\n");
		goto delete_intr_task_exit;
	}

	goto example_exit;

delete_intr_task_exit:
	rtos_task_delete(intr_task);

usbh_ecm_cdc_deinit_exit:
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	rtos_task_delete(hotplug_task);
#endif

usb_ecm_deinit_exit:
	usbh_cdc_ecm_deinit();

usb_deinit_exit:
	usbh_deinit();

free_sema_exit:
	usb_os_sema_delete(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema);
	usb_os_sema_delete(cdc_ecm_detach_sema);
	usb_os_sema_delete(cdc_ecm_ctrl_start_sema);
	usb_os_sema_delete(cdc_ecm_intr_start_sema);
	usb_os_sema_delete(cdc_ecm_bulk_start_sema);

example_exit:
	rtos_task_delete(NULL);
}

/* Exported functions --------------------------------------------------------*/
/*
 * */
int usbh_cdc_ecm_do_deinit(void)
{
	return HAL_OK;
}

int usbh_cdc_ecm_do_init(usb_report_data cb_handle)
{
	int status;
	usb_os_task_t task;

	usbh_cdc_ecm_host_user.report_data = cb_handle ;

	if (0 == usbh_cdc_ecm_host_user.ecm_init_success) {
		status = rtos_task_create(&task, "ecm_init_thread", usbh_cdc_ecm_init_thread, NULL, 1024U, USBH_ECM_INIT_THREAD_PRIORITY);
		if (status != SUCCESS) {
			RTK_LOGS(TAG, "[ECMH] Create init task fail %d\n", status);
			return HAL_ERR_MEM;
		}
		usbh_cdc_ecm_host_user.ecm_init_success = 1;
	}

	return HAL_OK;
}

u16 usbh_cdc_ecm_get_receive_mps(void)
{
	return usbh_cdc_ecm_get_usbin_mps();
}

int usbh_cdc_ecm_send_data(u8 *buf, u32 len)
{
	int ret;
	u8 retry_cnt = 0;

#if USBH_ECM_TX_SPEED_CHECK
	static u64 usb_tx_start_time = 0, usb_tx_end_time, usb_tx_interval_time;
	static u64 usb_tx_total_len = 0;

	usb_tx_total_len += len;
	if (usb_tx_start_time == 0) {
		usb_tx_start_time = xTaskGetTickCount();
	}
#endif

	while (1) {
		ret = usbh_cdc_ecm_bulk_send(buf, len);
		if (ret == HAL_OK) {
			//success
			break;
		}
		if (++retry_cnt > 3) {
			//RTK_LOGS(TAG, "[ECMH] TX drop(%d)\n", len);
			ret = HAL_ERR_UNKNOWN;
			break;
		} else {
			usb_os_delay_us(1);
		}
	}

	//wait cdc_ecm_cb_bulk_send to give the sema
	if (ret == HAL_OK) {
		usb_os_sema_take(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema, USB_OS_SEMA_TIMEOUT);
#if USBH_ECM_TX_SPEED_CHECK
		usb_tx_end_time = xTaskGetTickCount();
		usb_tx_interval_time = (usb_tx_end_time - usb_tx_start_time) * RTOS_TICK_RATE_MS;

		if (usb_tx_interval_time >= 3000) {
			RTK_LOGS(TAG, "[ECMH] Heap %d TX %dB in %d ms, %d Kbps\n",
					 rtos_mem_get_free_heap_size(),
					 (u32)usb_tx_total_len, (u32)usb_tx_interval_time, (u32)((usb_tx_total_len * 8 * 1000) / (usb_tx_interval_time * 1024)));
			usb_tx_start_time = usb_tx_end_time;
			usb_tx_total_len = 0;
		}
#endif

	}

	return ret;
}

int usbh_cdc_ecm_get_connect_status(void)//1 up
{
	u8 ret1 = usbh_cdc_ecm_host_user.cdc_ecm_is_ready;
	u8 ret2 = usbh_cdc_ecm_host_user.ecm_hw_connect;
	int ret = ret1 & ret2;
	//RTK_LOGS(TAG, "[ECMH] Status usb(%d)-dongle(%d)\n", ret1, ret2);
	return ret;
}

