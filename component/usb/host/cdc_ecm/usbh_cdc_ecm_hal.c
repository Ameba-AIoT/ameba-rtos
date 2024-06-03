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
#define CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST   (1)   /* Hot plug test */
#define USBH_ECM_RX_SPEED_CHECK             (0)   /* CDC ECM rx speed test */
#define USBH_ECM_TX_SPEED_CHECK             (0)   /* CDC ECM tx speed test */

#define USBH_CDC_ECM_INTR_BUF_SIZE          256   /* Buffer size for INTR loopback test, which should match with device INTR loopback buffer size */
#define USBH_CDC_ECM_LOOPBACK_CNT           100   /* Loopback test round */

#define USBH_CORE_INIT_FAIL                 (1)
#define USBH_CLASS_INIT_FAIL                (2)


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

static const char *TAG = "CDC_ECM";

static u8 cdc_ecm_intr_rx_buf[USBH_CDC_ECM_INTR_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

static usb_os_sema_t cdc_ecm_detach_sema;
static usb_os_sema_t cdc_ecm_attach_sema;
static usb_os_sema_t cdc_ecm_intr_start_sema;
static usb_os_sema_t cdc_ecm_bulk_start_sema;

static usbh_cdc_ecm_host_hal_t usbh_cdc_ecm_host_user;

static usbh_config_t usbh_ecm_cfg = {
	.pipes = 5U,
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1U,
	.main_task_priority = 4U,
	.isr_task_priority  = 5U,
	.rx_fifo_size   = 0x200U,
	.nptx_fifo_size = 0x100U,
	.ptx_fifo_size  = 0x100U,
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
	RTK_LOGI(TAG, "CDC ECM INIT\n");
	return HAL_OK;
}

static int cdc_ecm_cb_deinit(void)
{
	RTK_LOGI(TAG, "CDC ECM DEINIT\n");
	usbh_cdc_ecm_host_user.ecm_hw_connect = 0;
	return HAL_OK;
}

static int cdc_ecm_cb_attach(void)
{
	RTK_LOGI(TAG, "ATTACH\n");
	usb_os_sema_give(cdc_ecm_attach_sema);
	return HAL_OK;
}

static int cdc_ecm_cb_detach(void)
{
	RTK_LOGI(TAG, "DETACH\n");
	usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 0;
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	usb_os_sema_give(cdc_ecm_detach_sema);
#endif

	return HAL_OK;
}

static int cdc_ecm_cb_setup(void)
{
	RTK_LOGI(TAG, "SETUP\n");
	usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 1;
	usb_os_sema_give(cdc_ecm_intr_start_sema);
	usb_os_sema_give(cdc_ecm_bulk_start_sema);

	return HAL_OK;
}

static int cdc_ecm_cb_bulk_receive(u8 *buf, u32 length)
{
	//RTK_LOGI(TAG, "bulk receive(%d)\n",length);
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
		RTK_LOGI(TAG, "[RX][heap %ld]%lld bytes in %lld ms, %lld Kbits/sec \n",
				 rtos_mem_get_free_heap_size(),
				 usb_rx_total_len, usb_rx_interval_time, (usb_rx_total_len * 8 * 1000) / (usb_rx_interval_time * 1024));
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
	//RTK_LOGI(TAG, "Transmit finish: %d\n", state);
	usb_os_sema_give(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema);
	if (state != USBH_URB_DONE) {
		RTK_LOGE(TAG, "Transmit fail: %d\n", state);
	}

	return HAL_OK;
}

//add parse to get the ethernet status
static int cdc_ecm_cb_intr_receive(u8 *buf, u32 length)
{
	//RTK_LOGD(TAG, "INTR RX len(%d)\n", length);
	if (buf && length >= 8) {
		//RTK_LOGI(TAG, "Data(%02x %02x %02x %02x )\n",buf[0],buf[1],buf[2],buf[3]);
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
	RTK_LOGD(TAG, "Process ,id=%d\n", id);
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
	RTK_LOGI(TAG, "[USBH] INTR test pending, wait for device ready\n");

	if (usb_os_sema_take(cdc_ecm_intr_start_sema, USB_OS_SEMA_TIMEOUT) == HAL_OK) {
		usb_os_sleep_ms(500);
		RTK_LOGI(TAG, "[USBH] intr send task started \n");

		do {
			i++;
			if (!usbh_cdc_ecm_get_usb_status()) { //18s
				if ((i % 10 == 0)) {
					RTK_LOGW(TAG, "[USBH] Device disconnected, intr test aborted[%d]\n", usbh_cdc_ecm_get_usb_status());
				}
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
	RTK_LOGI(TAG, "[USBH] bulk test pending, wait for device ready\n");

	if (usb_os_sema_take(cdc_ecm_bulk_start_sema, USB_OS_SEMA_TIMEOUT) == HAL_OK) {
		usb_os_sleep_ms(500);
		RTK_LOGI(TAG, "[USBH] bulk send task started \n");

		do {
			i++;
			if (!usbh_cdc_ecm_get_usb_status()) {
				if (i % 10 == 0) {
					RTK_LOGW(TAG, "[USBH] Device disconnected, bulk test aborted[%d]\n", usbh_cdc_ecm_get_usb_status());
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
		usb_os_sleep_ms(2000);
		usbh_cdc_ecm_deinit();
		usbh_deinit();
		usb_os_sleep_ms(10);
		RTK_LOGI(TAG, "Free heap size: 0x%08lX\n", usb_os_get_free_heap_size());

		ret = usbh_cdc_ecm_doinit();
		if (ret != HAL_OK) {
			RTK_LOGE(TAG, "Fail to init USB host controller: %d\n", ret);
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
		RTK_LOGE(TAG, "Fail to init USB host controller: %d\n", status);
		return USBH_CORE_INIT_FAIL;
	}

	status = usbh_cdc_ecm_init(&cdc_ecm_usb_cb);  /*0 means use default transfer size, and it can not exceed 65536*/
	if (status != HAL_OK) {
		RTK_LOGE(TAG, "Fail to init USB host cdc_ecm driver: %d\n", status);
		return USBH_CLASS_INIT_FAIL;
	}

	if (usb_os_sema_take(cdc_ecm_attach_sema, USB_OS_SEMA_TIMEOUT) == HAL_OK) {
		usbh_cdc_ecm_state_t step = CDC_ECM_STATE_ALT_SETTING;
		u8 loop = 1;
#if ECM_ENABLE_PACKETFILTER
		u8 mac_str[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif
		do {
			switch (step) {
			case CDC_ECM_STATE_ALT_SETTING: //choose alt
				status = usbh_cdc_ecm_alt_setting();
				if (HAL_OK != status) {
					usb_os_sleep_ms(100);
				} else {
					step++;
				}
				break;

#if ECM_ENABLE_PACKETFILTER
			case CDC_ECM_STATE_SET_ETHERNET_MULTICAST_FILTER://set the multi list
				status = usbh_cdc_ecm_set_ethernet_multicast_filter(mac_str, 6);
				if (HAL_OK != status) {
					usb_os_sleep_ms(100);
				} else {
					step = CDC_ECM_STATE_SET_ETHERNET_PACKET_FILTER;
				}
				break;

			case CDC_ECM_STATE_SET_ETHERNET_PACKET_FILTER://set the filter
				status = usbh_cdc_ecm_set_ethernet_packetfilter(CDC_ECM_ETH_PACKET_TYPE_DIRECTED);
				if (HAL_OK != status) {
					usb_os_sleep_ms(100);
				} else {
					step ++;
				}
				break;
#endif

#if ECM_ENABLE_RCR_CONFIGURATION
			case CDC_ECM_STATE_RCR_GET://get the RCR
				status = usbh_cdc_ecm_get_rcr();
				if (HAL_OK != status) {
					usb_os_sleep_ms(100);
				} else {
					step = CDC_ECM_STATE_RCR_SET;
				}
				break;

			case CDC_ECM_STATE_RCR_SET://set the RCR
				status = usbh_cdc_ecm_set_rcr();
				if (HAL_OK != status) {
					usb_os_sleep_ms(100);
				} else {
					step ++;
				}
				break;
#endif

			case CDC_ECM_STATE_SWITCH_TO_TRANSFER://switch to transfer
			default:
				status = usbh_cdc_ecm_set_ethernet_start_transfer();
				if (HAL_OK != status) {
					usb_os_sleep_ms(100);
				} else {
					loop = 0;
				}
				break;
			}
		} while (loop);
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
	usb_os_sema_create(&cdc_ecm_attach_sema);
	usb_os_sema_create(&cdc_ecm_intr_start_sema);
	usb_os_sema_create(&cdc_ecm_bulk_start_sema);

	status = usbh_cdc_ecm_doinit();
	if (USBH_CORE_INIT_FAIL == status) {
		goto free_sema_exit;
	} else if (USBH_CLASS_INIT_FAIL == status) {
		goto usb_deinit_exit;
	}

#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	status = rtos_task_create(&hotplug_task, "ecm_hotplug_thread", ecm_hotplug_thread, NULL, 1024U * 2, 2);
	if (status != SUCCESS) {
		RTK_LOGE(TAG, "Fail to create USBH cdc_ecm hotplug check thread\n");
		goto usb_ecm_deinit_exit;
	}
#endif

	status = rtos_task_create(&intr_task, "ecm_intr_rx_thread", ecm_intr_rx_thread, NULL, 1024U * 2, 2);
	if (status != SUCCESS) {
		RTK_LOGE(TAG, "[USBH] Fail to create USBH INTR intr_cdc_ecm_test_task thread\n");
		goto usbh_ecm_cdc_deinit_exit;
	}

	status = rtos_task_create(&bulk_task, "ecm_bulk_rx_thread", ecm_bulk_rx_thread, NULL, 1024U * 2, 2);
	if (status != SUCCESS) {
		RTK_LOGE(TAG, "[USBH] Fail to create USBH BULK ecm_bulk_rx_thread thread\n");
		goto delete_intr_task_exit;
	}
	RTK_LOGD(TAG, "USB host init task run finish!\n");
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
	usb_os_sema_delete(cdc_ecm_attach_sema);
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

	RTK_LOGI(TAG, "USB host init(%d)...\n", usbh_cdc_ecm_host_user.ecm_init_success);
	if (0 == usbh_cdc_ecm_host_user.ecm_init_success) {
		status = rtos_task_create(&task, "ecm_init_thread", usbh_cdc_ecm_init_thread, NULL, 1024U * 2, 2);
		if (status != SUCCESS) {
			RTK_LOGE(TAG, "Fail to create USB host cdc_ecm init thread: %d\n", status);
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
			RTK_LOGI(TAG, "TX drop(%ld)\n", len);
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
			RTK_LOGI(TAG, "[TX][heap %ld]%lld bytes in %lld ms, %lld Kbits/sec \n",
					 rtos_mem_get_free_heap_size(),
					 usb_tx_total_len, usb_tx_interval_time, (usb_tx_total_len * 8 * 1000) / (usb_tx_interval_time * 1024));
			usb_tx_start_time = usb_tx_end_time;
			usb_tx_total_len = 0;
		}
#endif

	}
	//RTK_LOGD(TAG, "Transmit start(%d)\n",len);

	return ret;
}

int usbh_cdc_ecm_get_connect_status(void)//1 up
{
	u8 ret1 = usbh_cdc_ecm_host_user.cdc_ecm_is_ready;
	u8 ret2 = usbh_cdc_ecm_host_user.ecm_hw_connect;
	int ret = ret1 & ret2;
	RTK_LOGD(TAG, "CDC ECM Status(usb(%d)-dongle(%d))\n", ret1, ret2);
	return ret;
}

