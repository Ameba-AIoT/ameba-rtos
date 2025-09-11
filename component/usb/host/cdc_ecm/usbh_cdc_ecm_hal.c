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
#include "usbh_cdc_ecm_appx.h"
#include "usbh_cdc_ecm_hal.h"


/* Private defines -----------------------------------------------------------*/
#define CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST   1     /* Hot plug test */
#define USBH_ECM_RX_SPEED_CHECK             0     /* CDC ECM rx speed test */
#define USBH_ECM_TX_SPEED_CHECK             0     /* CDC ECM tx speed test */

#define USBH_CDC_ECM_LOOPBACK_CNT           100   /* Loopback test round */

#define USBH_CORE_INIT_FAIL                 (1)
#define USBH_CLASS_INIT_FAIL                (2)

#define USBH_ECM_MONITOR_THREAD_PRIORITY      3
#define USBH_ECM_RX_THREAD_PRIORITY           4
#define USBH_ECM_MAIN_THREAD_PRIORITY         5
#define USBH_ECM_ISR_THREAD_PRIORITY          6
#define USBH_ECM_HOTPLUG_THREAD_PRIORITY      7
#define USBH_ECM_ISR_PRIORITY                 INT_PRI_MIDDLE

/* Private types -------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)
typedef struct {
	usbh_cdc_ecm_priv_data_t   *params;
	usb_report_data             report_data;        //usb rx callback function

	rtos_task_t 	            monitor_task;
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	rtos_task_t 	            hotplug_task;
#endif
	usb_os_sema_t               cdc_ecm_tx_sema;    //usb tx send sema
	volatile u8                 cdc_ecm_tx_block;   //ecm attached status
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
static int cdc_ecm_cb_device_check(usb_host_t *host, u8 cfg_max);
static int usbh_cdc_ecm_doinit(void);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "ECMH";

static usb_os_sema_t cdc_ecm_detach_sema;
static usb_os_sema_t cdc_ecm_monitor_start_sema;

static usbh_cdc_ecm_host_hal_t usbh_cdc_ecm_host_user;

static usbh_cdc_ecm_time_t usbh_cdc_ecm_time_array[USBH_CDC_ECM_TYPE_MAX];

static usbh_config_t usbh_ecm_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1U,
	.ext_intr_enable = 0, //USBH_SOF_INTR
	.isr_priority = USBH_ECM_ISR_PRIORITY,
	.isr_task_priority  = USBH_ECM_ISR_THREAD_PRIORITY,
	.main_task_priority = USBH_ECM_MAIN_THREAD_PRIORITY,
	.hub_enable = 1U,

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
	.process = cdc_ecm_cb_process,
	.validate = cdc_ecm_cb_device_check,
};

/* Private functions ---------------------------------------------------------*/
static int usbh_ecm_timer_unregister(void)
{
	u8 idx = 0;
	usbh_cdc_ecm_time_t *phandle = NULL;

	for (idx = 0; idx < USBH_CDC_ECM_TYPE_MAX; idx++) {
		phandle = &usbh_cdc_ecm_time_array[idx];
		if (phandle->pipe_id == 0) {
			continue;
		}

		phandle->pipe_id = 0;
		phandle->type = 0;
		phandle->check_interval = 0;
		phandle->last_check_time = 0;
		phandle->func = NULL;
	}

	return HAL_OK;
}

static int cdc_ecm_cb_device_check(usb_host_t *host, u8 cfg_max)
{
	return usbh_cdc_ecm_check_config_desc(host, cfg_max);
}

static int usbh_cdc_ecm_tx_status_check(void)
{
	if (usbh_cdc_ecm_host_user.cdc_ecm_tx_block) {
		//release the sema to return usbh_cdc_ecm_send_data
		do {
			usb_os_sema_give(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema);
			usb_os_sleep_ms(1U);
		} while (usbh_cdc_ecm_host_user.cdc_ecm_tx_block);
	}

	return HAL_OK;
}

static u32 usbh_cdc_ecm_get_elapsed_ticks(u32 new_tick, u32 old_tick)
{
	if (new_tick >= old_tick) {
		return new_tick - old_tick;
	} else {
		return (0xFFFFFFFF - old_tick + 1 + new_tick);
	}
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
	usb_os_sema_give(cdc_ecm_monitor_start_sema);

	return HAL_OK;
}

static int cdc_ecm_cb_bulk_receive(u8 *buf, u32 length)
{
#if USBH_ECM_RX_SPEED_CHECK
	static u64 usb_rx_start_time = 0, usb_rx_end_time, usb_rx_interval_time; //ms
	static u64 usb_rx_total_len = 0;

	usb_rx_total_len += length;
	if (usb_rx_start_time == 0) {
		usb_rx_start_time = usbh_get_timestamp(NULL);
	}
	usb_rx_end_time = usbh_get_timestamp(NULL);
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
		// RTK_LOGS(TAG, RTK_LOG_INFO, "Len = %d\n", length);
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

static void usbh_ecm_monitor_thread(void *param)
{
	UNUSED(param);
	usbh_cdc_ecm_time_t *phandle = NULL;
	u32 next_sleep = -1;  //init
	int eth_state;
	int i = 0;

	if (usb_os_sema_take(cdc_ecm_monitor_start_sema, USB_OS_SEMA_TIMEOUT) == HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Monitor task start\n");

		do {
			i++;
			if (!usbh_cdc_ecm_host_user.cdc_ecm_is_ready) { //18s
				if ((i % 10 == 0)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Device disconnected %d\n", usbh_cdc_ecm_host_user.cdc_ecm_is_ready);
				}
				//device disconnect, try again after sleep 1s
				usb_os_sleep_ms(1000);
				continue;
			}

			eth_state = usbh_cdc_ecm_get_connect_status();
			u32 now = usbh_get_timestamp(NULL); //ms
			next_sleep = -1;  //init

			for (u8 i = 0; i < USBH_CDC_ECM_TYPE_MAX; i++) {
				phandle = &usbh_cdc_ecm_time_array[i];
				if (phandle == NULL) {
					continue;
				}

				if (eth_state == 0) {
					if ((phandle->type == USBH_CDC_ECM_TYPE_BULK_IN) || (phandle->type == USBH_CDC_ECM_TYPE_BULK_OUT)) { //skip ecm trx
						continue;
					} else if ((phandle->type == USBH_CDC_ECM_TYPE_INTR) && (phandle->check_interval != usbh_cdc_ecm_get_intr_interval())) {
						RTK_LOGS(TAG, RTK_LOG_INFO, "[ECMH] Intr switch to %d/%d\n", phandle->check_interval, usbh_cdc_ecm_get_intr_interval());
						phandle->check_interval = usbh_cdc_ecm_get_intr_interval();
					}
				} else {
					if ((phandle->type == USBH_CDC_ECM_TYPE_INTR) && (phandle->check_interval != USBH_ECM_ETH_STATUS_CHECK)) { //skip the ecm trx
						RTK_LOGS(TAG, RTK_LOG_INFO, "[ECMH] Intr switch to %d-%d\n", phandle->check_interval, USBH_ECM_ETH_STATUS_CHECK);
						phandle->check_interval = USBH_ECM_ETH_STATUS_CHECK;
					}
				}

				if (usbh_cdc_ecm_get_elapsed_ticks(now, phandle->last_check_time) >= phandle->check_interval) {
					phandle->last_check_time = now; //update the time
					if (phandle->func) {
						phandle->func();
					}
				}

				//cal the main time
				u32 remaining = phandle->check_interval - usbh_cdc_ecm_get_elapsed_ticks(now, phandle->last_check_time);
				if (next_sleep == (u32) - 1 || remaining < next_sleep) {
					next_sleep = remaining; //get the min time to sleep
				}
			}

			if (next_sleep > 0) {
				if (next_sleep > 1000) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "Sleep %d seconds \n", next_sleep);
				}
				usb_os_sleep_ms(next_sleep);  //sleep the min time
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
		RTK_LOGS(TAG, RTK_LOG_INFO, "Hot plug\n");
		//stop isr
		usbh_cdc_ecm_deinit();
		usbh_deinit();
		usbh_cdc_ecm_tx_status_check();
		usbh_ecm_timer_unregister();

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

	/* init appx */
	usbh_cdc_ecm_appx_doinit(usbh_cdc_ecm_host_user.params);

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

/* Exported functions  used in ECM class--------------------------------------------------------*/
/**
  * @brief  register EP info to the array for the monitor task
  * @param  pipe: User Callback
  * @param  priv: private params
  * @retval Status
  */
int usbh_ecm_timer_register(u8 pipe, u32 time_value, usb_timer_func fn, u8 type)
{
	int ret = -1;
	u8 idx = 0;
	usbh_cdc_ecm_time_t *phandle = NULL;
	//RTK_LOGS(TAG,"USB %d-%d-%d\n", pipe ,type, time_value);

	for (idx = 0; idx < USBH_CDC_ECM_TYPE_MAX; idx++) {
		phandle = &usbh_cdc_ecm_time_array[idx];
		if (phandle->pipe_id != 0) {
			continue;
		}

		phandle->pipe_id = pipe;
		phandle->type = type;
		phandle->check_interval = time_value;
		phandle->last_check_time = 0;
		phandle->func = fn;

		ret = HAL_OK;
		break;
	}

	if (idx == USBH_CDC_ECM_TYPE_MAX) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "USB array is full\n");
	}

	// for (u8 i = 0; i < USBH_CDC_ECM_TYPE_MAX; i++) {
	// 	phandle = &usbh_cdc_ecm_time_array[i];
	// 	RTK_LOGS(TAG, RTK_LOG_ERROR, "USB idx=%d %d-%d-%d-%d\n",i,phandle->pipe_id,phandle->type,phandle->check_interval,phandle->last_check_time);
	// }

	return ret;
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Deinitialize the USB ECM Class
  * @retval Status
  */
int usbh_cdc_ecm_do_deinit(void)
{
#if CONFIG_USBH_CDC_ECM_HOT_PLUG_TEST
	if (usbh_cdc_ecm_host_user.hotplug_task != NULL) {
		RTK_LOGI(TAG, "Del Hotplug task\n");
		rtos_task_delete(usbh_cdc_ecm_host_user.hotplug_task);
		usbh_cdc_ecm_host_user.hotplug_task = NULL;
	}
#endif

	usbh_cdc_ecm_deinit();
	usbh_deinit();
	usbh_cdc_ecm_tx_status_check();
	usbh_ecm_timer_unregister();

	//destory all task
	RTK_LOGS(TAG, RTK_LOG_INFO, "[USBD]%s Line %d\n", __func__, __LINE__);
	if (usbh_cdc_ecm_host_user.monitor_task != NULL) {
		RTK_LOGI(TAG, "Del monitor_task task\n");
		rtos_task_delete(usbh_cdc_ecm_host_user.monitor_task);
		usbh_cdc_ecm_host_user.monitor_task = NULL;
	}

	usb_os_sema_delete(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema);
	usb_os_sema_delete(cdc_ecm_detach_sema);
	usb_os_sema_delete(cdc_ecm_monitor_start_sema);

	return HAL_OK;
}

/**
  * @brief  Initialize the USB ECM Class
  * @param  cb_handle: User Callback
  * @param  priv: private params
  * @retval Status
  */
int usbh_cdc_ecm_do_init(usb_report_data cb_handle, usbh_cdc_ecm_priv_data_t *priv)
{
	int status;

	usbh_cdc_ecm_host_user.report_data = cb_handle;
	usbh_cdc_ecm_host_user.params = priv;

	usb_os_sema_create(&(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema));
	usb_os_sema_create(&cdc_ecm_detach_sema);
	usb_os_sema_create(&cdc_ecm_monitor_start_sema);

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

	status = rtos_task_create(&(usbh_cdc_ecm_host_user.monitor_task), "usbh_ecm_monitor_thread", usbh_ecm_monitor_thread, NULL, 1024U,
							  USBH_ECM_MONITOR_THREAD_PRIORITY);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create INTR RX task fail\n");
		goto usbh_ecm_cdc_deinit_exit;
	}

	return HAL_OK;

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
	usb_os_sema_delete(cdc_ecm_monitor_start_sema);

	return HAL_OK;
}

/**
  * @brief  usb enum success
  * @retval status
  */
u8 usbh_cdc_ecm_usb_is_ready(void)
{
	return (usbh_cdc_ecm_check_enum_status() == HAL_OK);
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
		usb_tx_start_time = usbh_get_timestamp(NULL);
	}
#endif

	while (1) {
		ret = usbh_cdc_ecm_bulk_send(buf, len);
		if (ret == HAL_OK) {
			//success
			break;
		}
		if (++retry_cnt > 100) { //100ms
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX drop(%d)\n", len);
			ret = HAL_ERR_UNKNOWN;
			break;
		} else {
			usb_os_delay_us(1);
		}
	}

	//wait cdc_ecm_cb_bulk_send to give the sema
	if (ret == HAL_OK) {
		usbh_cdc_ecm_host_user.cdc_ecm_tx_block = 1;
		usb_os_sema_take(usbh_cdc_ecm_host_user.cdc_ecm_tx_sema, USB_OS_SEMA_TIMEOUT);
#if USBH_ECM_TX_SPEED_CHECK
		usb_tx_end_time = usbh_get_timestamp(NULL);
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

	usbh_cdc_ecm_host_user.cdc_ecm_tx_block = 0;
	// RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM TX (%d) Done\n", len);
	return ret;
}

/**
  * @brief  ecm device is ready
  * @retval
  */
int usbh_cdc_ecm_get_connect_status(void)//1 up
{
	u8 ret1 = usbh_cdc_ecm_host_user.cdc_ecm_is_ready;
	u8 ret2 = usbh_cdc_ecm_host_user.ecm_hw_connect;
	int ret = ret1 & ret2;
	if (usbh_cdc_ecm_get_device_vid_info() == USB_DEFAULT_VID) {
		return ret;
	}
	return usbh_cdc_ecm_host_user.cdc_ecm_is_ready ;
}

u16 usbh_cdc_ecm_get_receive_mps(void)
{
	return usbh_cdc_ecm_get_usbin_mps();
}

u16 usbh_cdc_ecm_get_device_vid(void)
{
	return usbh_cdc_ecm_get_device_vid_info();
}

u16 usbh_cdc_ecm_get_device_pid(void)
{
	return usbh_cdc_ecm_get_device_pid_info();
}

/**
  * @brief  hex to char
  * @retval
  */
u8 usbh_cdc_ecm_hex_to_char(u8 hex_num)
{
	if ((hex_num >= 0x30) && (hex_num <= 0x39)) {
		/* code */
		hex_num = 0 + hex_num - 0x30;
	} else if ((hex_num >= 0x41) && (hex_num <= 0x5A)) {
		/* code */
		hex_num = 0xA + (hex_num - 0x41);
	} else if ((hex_num >= 0x61) && (hex_num <= 0x7A)) {
		/* code */
		hex_num = 0xA + (hex_num - 0x61);
	} else {
		hex_num = 0x0;
	}

	return hex_num;
}

u8 usbh_cdc_ecm_prepare_done(void)
{
	usbh_cdc_ecm_trx_prepare_done();

	return 0;
}

#if ECM_STATE_DEBUG_ENABLE
int usbh_cdc_ecm_get_hw_statue(void)
{
	return usbh_cdc_ecm_host_user.ecm_hw_connect;
}
int usbh_cdc_ecm_get_usb_statue(void)
{
	return usbh_cdc_ecm_host_user.cdc_ecm_is_ready;
}
#endif