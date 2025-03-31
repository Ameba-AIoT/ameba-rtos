/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
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
#define USBH_ECM_MAIN_THREAD_PRIORITY         4
#define USBH_ECM_ISR_THREAD_PRIORITY          5
#define USBH_ECM_HOTPLUG_THREAD_PRIORITY      6

#if USBH_ECM_RX_SPEED_CHECK | USBH_ECM_TX_SPEED_CHECK
extern uint32_t xTaskGetTickCount(void);
#endif

/* Private types -------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)
typedef struct {
	usb_report_data             report_data;        //usb rx callback function

	rtos_task_t 	            intr_task;
	rtos_task_t 	            bulk_task;
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	rtos_task_t 	            hotplug_task;
#endif

	usb_os_sema_t               cdc_ecm_tx_sema;    //usb tx send sema
	volatile u8                 cdc_ecm_is_ready;   //ecm attached status
	volatile u8                 ecm_hw_connect;     //ecm ethernet connect status:0 disconnect,1 connect
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

static const char *const TAG = "ECMH";

static u8 cdc_ecm_intr_rx_buf[USBH_CDC_ECM_INTR_BUF_SIZE] __attribute__((aligned(CACHE_LINE_SIZE)));

static usb_os_sema_t cdc_ecm_detach_sema;
static usb_os_sema_t cdc_ecm_intr_start_sema;
static usb_os_sema_t cdc_ecm_bulk_start_sema;

static usbh_cdc_ecm_host_hal_t usbh_cdc_ecm_host_user;

static usbh_config_t usbh_ecm_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1U,
	.ext_intr_en = 0, //USBH_SOF_INTR
	.main_task_priority = USBH_ECM_MAIN_THREAD_PRIORITY,
	.isr_task_priority  = USBH_ECM_ISR_THREAD_PRIORITY,

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
	return usbh_cdc_ecm_host_user.cdc_ecm_is_ready;
}

static int cdc_ecm_cb_init(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "INIT\n");
	return HAL_OK;
}

static int cdc_ecm_cb_deinit(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DEINIT\n");
	usbh_cdc_ecm_host_user.ecm_hw_connect = 0;
	return HAL_OK;
}

static int cdc_ecm_cb_attach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "ATTACH\n");
	return HAL_OK;
}

static int cdc_ecm_cb_detach(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "DETACH\n");
	usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 0;
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	usb_os_sema_give(cdc_ecm_detach_sema);
#endif

	return HAL_OK;
}

static int cdc_ecm_cb_setup(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "SETUP\n");
	usbh_cdc_ecm_host_user.cdc_ecm_is_ready = 1;
	usb_os_sema_give(cdc_ecm_intr_start_sema);
	usb_os_sema_give(cdc_ecm_bulk_start_sema);

	return HAL_OK;
}

static int cdc_ecm_cb_bulk_receive(u8 *buf, u32 length)
{
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d RX %dB in %d ms, %d Kbps\n",
				 rtos_mem_get_free_heap_size(),
				 (u32)usb_rx_total_len, (u32)usb_rx_interval_time, (u32)((usb_rx_total_len * 8 * 1000) / (usb_rx_interval_time * 1024)));
		usb_rx_start_time = usb_rx_end_time;
		usb_rx_total_len = 0;
	}
#endif

	if ((usbh_cdc_ecm_host_user.report_data != NULL) && (length > 0)) {
#if 0
		RTK_LOGS(TAG, RTK_LOG_INFO, "%s(%d) len = %d ", __func__, __LINE__, length);
		for (u32 i = 0; i < length; i++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", (u8)buf[i]);
		}
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
#endif
		usbh_cdc_ecm_host_user.report_data(buf, length);
	}

	return HAL_OK;
}

static int cdc_ecm_cb_bulk_send(usbh_urb_state_t state)
{
	usb_os_sema_give(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema);
	if (state != USBH_URB_DONE) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK TX fail %d\n", state);
	}

	return HAL_OK;
}

//add parse to get the ethernet status
static int cdc_ecm_cb_intr_receive(u8 *buf, u32 length)
{
	if (buf && length >= 8) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Data(%02x %02x %02x %02x )\n",buf[0],buf[1],buf[2],buf[3]);
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
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR test pending, wait device ready\n");

	if (usb_os_sema_take(cdc_ecm_intr_start_sema, USB_OS_SEMA_TIMEOUT) == HAL_OK) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "INTR TX task start\n");

		do {
			i++;
			if (!usbh_cdc_ecm_get_usb_status()) { //18s
				if ((i % 10 == 0)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Device disconnected %d\n", usbh_cdc_ecm_get_usb_status());
				}
				//device disconnect, try again after sleep 1s
				usb_os_sleep_ms(1000);
				continue;
			}

			//work when ECM connect fail
			// if (usbh_cdc_ecm_get_connect_status()) {
			// 	usb_os_sleep_ms(100);
			// 	continue;
			// }

			if (HAL_OK != usbh_cdc_ecm_intr_receive(cdc_ecm_intr_rx_buf, USBH_CDC_ECM_INTR_BUF_SIZE)) {
				usb_os_sleep_ms(usbh_cdc_ecm_get_intr_interval());
			} else {
				//500ms to check the event
				usb_os_sleep_ms(500);
			}
		} while (1);
	}

}

static void ecm_bulk_rx_thread(void *param)
{
	u32 i = 0;
	UNUSED(param);
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK test pending, wait device ready\n");

	if (usb_os_sema_take(cdc_ecm_bulk_start_sema, USB_OS_SEMA_TIMEOUT) == HAL_OK) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "BULK TX task start\n");

		do {
			i++;
			if (!usbh_cdc_ecm_get_usb_status()) {
				if (i % 10 == 0) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Device disconnected %d\n", usbh_cdc_ecm_get_usb_status());
				}
				usb_os_sleep_ms(1000);
				continue;
			}
			//wait ECM connect success
			if (!usbh_cdc_ecm_get_connect_status()) {
				usb_os_sleep_ms(100);
				continue;
			}

			if (HAL_OK != usbh_cdc_ecm_bulk_receive()) {
				usb_os_sleep_ms(1);
			} else {
				usb_os_sleep_ms(10);
			}
		} while (1);
	}

}

#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
static void ecm_hotplug_thread(void *param)
{
	int ret = 0;

	UNUSED(param);

	for (;;) {
		usb_os_sema_take(cdc_ecm_detach_sema, USB_OS_SEMA_TIMEOUT);
		//stop isr
		usbh_cdc_ecm_deinit();
		usbh_deinit();
		RTK_LOGS(TAG, RTK_LOG_INFO, "Free heap size: 0x%08x\n", usb_os_get_free_heap_size());

		ret = usbh_cdc_ecm_doinit();
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail %d\n", ret);
			break;
		}
	}

}
#endif

static int usbh_cdc_ecm_doinit(void)
{
	int status;

	status = usbh_init(&usbh_ecm_cfg, &usbh_ecm_usr_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Host init fail %d\n", status);
		return USBH_CORE_INIT_FAIL;
	}

	status = usbh_cdc_ecm_init(&cdc_ecm_usb_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init driver fail %d\n", status);
		return USBH_CLASS_INIT_FAIL;
	}

	return HAL_OK;
}


/* Exported functions --------------------------------------------------------*/
/*
 * */
int usbh_cdc_ecm_do_deinit(void)
{
	//destory all task
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	if (usbh_cdc_ecm_host_user.hotplug_task != NULL) {
		RTK_LOGI(TAG, "Del Hotplug task\n");
		rtos_task_delete(usbh_cdc_ecm_host_user.hotplug_task);
		usbh_cdc_ecm_host_user.hotplug_task = NULL;
	}
#endif

	if (usbh_cdc_ecm_host_user.intr_task != NULL) {
		RTK_LOGI(TAG, "Del intr_task task\n");
		rtos_task_delete(usbh_cdc_ecm_host_user.intr_task);
		usbh_cdc_ecm_host_user.intr_task = NULL;
	}

	if (usbh_cdc_ecm_host_user.bulk_task != NULL) {
		RTK_LOGI(TAG, "Del bulk_task task\n");
		rtos_task_delete(usbh_cdc_ecm_host_user.bulk_task);
		usbh_cdc_ecm_host_user.bulk_task = NULL;
	}

	usbh_cdc_ecm_deinit();
	usbh_deinit();

	usb_os_sema_delete(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema);
	usb_os_sema_delete(cdc_ecm_detach_sema);
	// usb_os_sema_delete(cdc_ecm_ctrl_start_sema);
	usb_os_sema_delete(cdc_ecm_intr_start_sema);
	usb_os_sema_delete(cdc_ecm_bulk_start_sema);

	return HAL_OK;
}

int usbh_cdc_ecm_do_init(usb_report_data cb_handle, usbh_cdc_ecm_priv_data_t *priv)
{
	int status;

	usbh_cdc_ecm_host_user.report_data = cb_handle;

	if (priv) {
		if (priv->mac_value) {
			usbh_cdc_ecm_set_dongle_mac(priv->mac_value);
		}
		if ((priv->led_array != NULL) && (priv->led_cnt > 0)) {
			usbh_cdc_ecm_set_dongle_led_array(priv->led_array, priv->led_cnt);
		}
	}

	usb_os_sema_create(&(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema));
	usb_os_sema_create(&cdc_ecm_detach_sema);
	usb_os_sema_create(&cdc_ecm_intr_start_sema);
	usb_os_sema_create(&cdc_ecm_bulk_start_sema);

	status = usbh_cdc_ecm_doinit();
	if (USBH_CORE_INIT_FAIL == status) {
		goto free_sema_exit;
	} else if (USBH_CLASS_INIT_FAIL == status) {
		goto usb_deinit_exit;
	}

#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	status = rtos_task_create(&(usbh_cdc_ecm_host_user.hotplug_task), "usbh_ecm_hotplug_thread", ecm_hotplug_thread, NULL, 1024U, USBH_ECM_HOTPLUG_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create hotplug check task fail\n");
		goto usb_ecm_deinit_exit;
	}
#endif

	status = rtos_task_create(&(usbh_cdc_ecm_host_user.intr_task), "usbh_ecm_intr_rx_thread", ecm_intr_rx_thread, NULL, 1024U, USBH_ECM_RX_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create INTR RX task fail\n");
		goto usbh_ecm_cdc_deinit_exit;
	}

	status = rtos_task_create(&(usbh_cdc_ecm_host_user.bulk_task), "usbh_ecm_bulk_rx_thread", ecm_bulk_rx_thread, NULL, 1024U, USBH_ECM_RX_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create BULK RX thread fail\n");
		goto delete_intr_task_exit;
	}

	return HAL_OK;

delete_intr_task_exit:
	rtos_task_delete(usbh_cdc_ecm_host_user.intr_task);
	usbh_cdc_ecm_host_user.intr_task = NULL;

usbh_ecm_cdc_deinit_exit:
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	rtos_task_delete(usbh_cdc_ecm_host_user.hotplug_task);
	usbh_cdc_ecm_host_user.hotplug_task = NULL;
#endif

usb_ecm_deinit_exit:
	usbh_cdc_ecm_deinit();

usb_deinit_exit:
	usbh_deinit();

free_sema_exit:
	usb_os_sema_delete(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema);
	usb_os_sema_delete(cdc_ecm_detach_sema);
	usb_os_sema_delete(cdc_ecm_intr_start_sema);
	usb_os_sema_delete(cdc_ecm_bulk_start_sema);

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
			//RTK_LOGS(TAG, RTK_LOG_ERROR, "TX drop(%d)\n", len);
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
			RTK_LOGS(TAG, RTK_LOG_INFO, "Heap %d TX %dB in %d ms, %d Kbps\n",
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
	//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Status usb(%d)-dongle(%d)\n", ret1, ret2);
	return ret;
}

#if ECM_STATE_DEBUG_ENABLE
int usbh_cdc_ecm_get_hw_statue(void)
{
	return usbh_cdc_ecm_host_user.ecm_hw_connect;
}
#endif