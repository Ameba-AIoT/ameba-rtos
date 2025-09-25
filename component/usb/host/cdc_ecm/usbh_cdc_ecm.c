/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_cdc_ecm.h"
#include "usbh_cdc_ecm_hal.h"
#include "usbh_cdc_ecm_appx.h"
#include "usb_os.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_CDC_BUFFER_SIZE             1024

#define ECM_RX_IN_HIGH_TP                1

/* tick count */
/* in ms*/
#define USB_BULK_OUT_IDLE_MAX_CNT        8U * 50 //send total 16k data
#define USB_BULK_OUT_BUSY_MAX_CNT        8U * 10
#define USB_BULK_OUT_BUSY_CHECK_CNT      5

#define USB_BULK_IN_IDLE_MAX_CNT         8U * 10  ///idle
#define USB_BULK_IN_NAK_CHECK_MAX_CNT    8U * 5   /// 4*5 busy nak drop
#define USB_BULK_IN_NAK_CHECK_CNT        4

#define USB_INTR_IN_IDLE_MAX_CNT         80U
#define USB_INTR_IN_BUSY_MAX_CNT         10U

/* monitor task */
#define USB_INTR_IN_CHECK_TIME_CNT       20U
#define USB_BULK_IN_CHECK_TIME_CNT       10U
#define USB_BULK_OUT_CHECK_TIME_CNT      10U


/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_cdc_ecm_attach(usb_host_t *host);
static int usbh_cdc_ecm_detach(usb_host_t *host);
static int usbh_cdc_ecm_process(usb_host_t *host, u32 msg);
static int usbh_cdc_ecm_setup(usb_host_t *host);
static void usbh_cdc_ecm_process_bulk_out(usb_host_t *host);
static void usbh_cdc_ecm_process_bulk_in(usb_host_t *host);
static void usbh_cdc_ecm_process_intr_in(usb_host_t *host);
static int usbh_cdc_ecm_bulk_receive(void);
static int usbh_cdc_ecm_bulk_tx(void);
static int usbh_cdc_ecm_intr_receive(void);

/* Private variables ---------------------------------------------------------*/

#if ECM_STATE_DEBUG_ENABLE
static rtos_task_t 	            ecm_debug_task;
static u8                       ecm_debug_task_init = 0;
static void usbh_cdc_ecm_debug_task_init(void);
static void usbh_cdc_ecm_debug_task_deinit(void);
#endif

static const char *const TAG = "ECMH";

/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_cdc_ecm_driver = {
	.class_code = CDC_CLASS_CODE,
	.attach  = usbh_cdc_ecm_attach,
	.detach  = usbh_cdc_ecm_detach,
	.setup   = usbh_cdc_ecm_setup,
	.process = usbh_cdc_ecm_process,
};

static usbh_cdc_ecm_host_t usbh_cdc_ecm_host;

/* Private functions ---------------------------------------------------------*/
static void usbh_cdc_ecm_dump_ecm_cfgdesc(void)
{
#if ECM_ENABLE_DUMP_DESCRIPYOT_PARSE
	usbh_cdc_ecm_host_t      *ecm = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t        *pcomm_if = &(ecm->ecm_report_ep);
	usbh_cdc_ecm_ep_t        *pdata_tx = &(ecm->ecm_tx_ep);
	usbh_cdc_ecm_ep_t        *pdata_rx = &(ecm->ecm_rx_ep);

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Descriptor Start------------------\n");

	//dump ecm control
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR epaddr(0x%x) MPS(%d) interval(%d)\n",
			 pcomm_if->ep_addr, pcomm_if->ep_mps, pcomm_if->ep_interval);

	//dump ecm data
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK in:epaddr(0x%x) MPS(=%d) out:epaddr(0x%x) MPS(%d)\n",
			 pdata_tx->ep_addr, pdata_tx->ep_mps,
			 pdata_rx->ep_addr, pdata_rx->ep_mps);

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
}

static void usbh_cdc_ecm_deinit_ep_buf(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	USBH_ECM_FREE_MEM(cdc->ecm_rx_ep.xfer_buf);
	USBH_ECM_FREE_MEM(cdc->ecm_report_ep.xfer_buf);
}

static void usbh_cdc_ecm_deinit_all_pipe(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (cdc->ecm_report_ep.pipe_id) {
		usbh_close_pipe(host, cdc->ecm_report_ep.pipe_id);
		usbh_free_pipe(host, cdc->ecm_report_ep.pipe_id);
		cdc->ecm_report_ep.pipe_id = 0U;
	}

	if (cdc->ecm_rx_ep.pipe_id) {
		usbh_close_pipe(host, cdc->ecm_rx_ep.pipe_id);
		usbh_free_pipe(host, cdc->ecm_rx_ep.pipe_id);
		cdc->ecm_rx_ep.pipe_id = 0U;
	}

	if (cdc->ecm_tx_ep.pipe_id) {
		usbh_close_pipe(host, cdc->ecm_tx_ep.pipe_id);
		usbh_free_pipe(host, cdc->ecm_tx_ep.pipe_id);
		cdc->ecm_tx_ep.pipe_id = 0U;
	}
}

static int usbh_cdc_ecm_usb_status_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (cdc->usbh_state < USBH_CDC_ECM_SETUP) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Device disconnected,wait\n");
		return HAL_BUSY;
	}

	return HAL_OK;
}

static int usbh_cdc_ecm_transfer_enable(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (cdc->allow_ecm_xfer) {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "wait prepare\n");
		return HAL_OK;
	} else {
		return HAL_BUSY;
	}
}

static int usbh_cdc_ecm_get_epinfo(usb_host_t *host)
{
	usbh_cdc_ecm_host_t 	     *ecm = &usbh_cdc_ecm_host;
	usbh_if_desc_t *comm_if_desc;
	usbh_ep_desc_t *ep_desc;
	usbh_dev_desc_t *pdesc;
	u8 status = HAL_ERR_UNKNOWN;
	u8 ep_id;
	u8 interface;

	pdesc = usbh_get_device_descriptor(ecm->host);
	ecm->vid = pdesc->idVendor;
	ecm->pid = pdesc->idProduct;

	/* Get interface index as per supported class & protocol */

	/*
		cdc ecm ctrl
	*/
	interface = usbh_get_interface(host,
									   CDC_IF_CDC_CTRL_CODE,
									   CDC_IF_CDC_CTRL_SUB_CLASS_ECM_CODE,
									   0x00);
	if (interface == 0xFFU) {
		RTK_LOGE(TAG,  "Fail to get the ecm ctrl interface.\n");
		return status;
	}

	comm_if_desc = usbh_get_interface_descriptor(host, interface, 0);
	if (comm_if_desc == NULL) {
		RTK_LOGE(TAG,  "Fail to get the ecm ctrl common interface.\n");
		return status;
	}

	/* get notification endpoint */
	ep_desc = &comm_if_desc->ep_desc_array[0];
	if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
		ecm->ecm_report_ep.ep_addr = ep_desc->bEndpointAddress;
		ecm->ecm_report_ep.ep_mps = ep_desc->wMaxPacketSize;
		ecm->ecm_report_ep.ep_interval  = ep_desc->bInterval;
	}

	/*
		cdc ecm data
	*/
	interface = usbh_get_interface(host,
									   CDC_IF_CDC_DATA_CODE,
									   CDC_IF_CDC_DATA_SUB_CLASS_DATA_CODE,
									   0x00);
	if (interface == 0xFFU) {
		RTK_LOGW(TAG,  "Fail to get the ecm data interface[0].");
		return status;
	}

	comm_if_desc = usbh_get_interface_descriptor(host, interface, 1);
	if (comm_if_desc == NULL) {
		RTK_LOGE(TAG,  "Fail to get the ecm data interface[1].");
		return status;
	}

	ecm->ecm_if = comm_if_desc->bInterfaceNumber;
	ecm->ecm_alt = comm_if_desc->bAlternateSetting;
	RTK_LOGS(TAG, RTK_LOG_INFO,  "Get ECM if(%d)alt(%d)\n", ecm->ecm_if, ecm->ecm_alt);

	/*
		get transfor endpoint
		ecm data need 2 eps
	*/
	for (ep_id = 0; ep_id < 2; ep_id++) {
		ep_desc = &comm_if_desc->ep_desc_array[ep_id];
		if (USB_EP_IS_IN(ep_desc->bEndpointAddress)) {
			ecm->ecm_rx_ep.ep_addr = ep_desc->bEndpointAddress;
			ecm->ecm_rx_ep.ep_mps = ep_desc->wMaxPacketSize;
			ecm->ecm_rx_ep.ep_interval  = ep_desc->bInterval;
		} else {
			ecm->ecm_tx_ep.ep_addr = ep_desc->bEndpointAddress;
			ecm->ecm_tx_ep.ep_mps = ep_desc->wMaxPacketSize;
			ecm->ecm_tx_ep.ep_interval  = ep_desc->bInterval;
		}
	}

	return HAL_OK;
}

/**
  * @brief  Usb Attach callback function
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_attach(usb_host_t *host) //parse all ep info
{
	int status = HAL_ERR_UNKNOWN;
	u8 pipe_num;
	u32 max_ep_size;

	usbh_cdc_ecm_host_t 		  *ecm = &usbh_cdc_ecm_host;
	ecm->host = host;

	if (host->config.speed == USB_SPEED_HIGH) {
		max_ep_size = USB_HS_MAX_PACKET_SIZE;
	} else {
		max_ep_size = USB_FS_MAX_PACKET_SIZE;
	}

	status = usbh_cdc_ecm_get_epinfo(host); //get ecm ep info
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse ECM desc fail\n");
		return status;
	}

	status = usbh_cdc_ecm_parse_at_cfgdesc(host, ecm->vid, ecm->pid); //get ecm appx ep info
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse ECM Appx desc fail\n");
		return status;
	}

	usbh_cdc_ecm_dump_ecm_cfgdesc();

	//limited the max ep size
	if (ecm->ecm_report_ep.ep_mps >= max_ep_size) {
		ecm->ecm_report_ep.ep_mps = max_ep_size;
	}
	if (ecm->ecm_tx_ep.ep_mps >= max_ep_size) {
		ecm->ecm_tx_ep.ep_mps = max_ep_size;
	}
	if (ecm->ecm_rx_ep.ep_mps >= max_ep_size) {
		ecm->ecm_rx_ep.ep_mps = max_ep_size;
	}

	//control
	pipe_num = usbh_alloc_pipe(host, ecm->ecm_report_ep.ep_addr);
	if (pipe_num != 0xFFU) {
		ecm->ecm_report_ep.pipe_id = pipe_num;
		usbh_ecm_timer_register(pipe_num, USB_INTR_IN_CHECK_TIME_CNT, usbh_cdc_ecm_intr_receive, USBH_CDC_ECM_TYPE_INTR);
		RTK_LOGS(TAG, RTK_LOG_INFO, "ECM Intr in ep%02x/pipe %d\n", ecm->ecm_report_ep.ep_addr, pipe_num);
	} else {
		RTK_LOGE(TAG,  "Fail to allocate INTR IN pipe for CDC ECM class.");
		return HAL_ERR_MEM;
	}
	usbh_open_pipe(host,
				   ecm->ecm_report_ep.pipe_id,
				   ecm->ecm_report_ep.ep_addr,
				   USB_CH_EP_TYPE_INTR,
				   ecm->ecm_report_ep.ep_mps);

	//bulk out
	pipe_num = usbh_alloc_pipe(host, ecm->ecm_tx_ep.ep_addr);
	if (pipe_num != 0xFFU) {
		ecm->ecm_tx_ep.pipe_id = pipe_num;
		usbh_ecm_timer_register(pipe_num, USB_BULK_OUT_CHECK_TIME_CNT, usbh_cdc_ecm_bulk_tx, USBH_CDC_ECM_TYPE_BULK_OUT);
		RTK_LOGS(TAG, RTK_LOG_INFO, "ECM Bulk Out ep%02x/pipe %d\n", ecm->ecm_tx_ep.ep_addr, pipe_num);
	} else {
		RTK_LOGE(TAG,  "Fail to allocate BULK OUT pipe for CDC ECM class.");
		usbh_free_pipe(host, ecm->ecm_report_ep.pipe_id);
		return HAL_ERR_MEM;
	}

	//bulk in
	pipe_num = usbh_alloc_pipe(host, ecm->ecm_rx_ep.ep_addr);
	if (pipe_num != 0xFFU) {
		ecm->ecm_rx_ep.pipe_id = pipe_num;
		usbh_ecm_timer_register(pipe_num, USB_BULK_IN_CHECK_TIME_CNT, usbh_cdc_ecm_bulk_receive, USBH_CDC_ECM_TYPE_BULK_IN);
		RTK_LOGS(TAG, RTK_LOG_INFO, "ECM BUlk In ep%02x/pipe %d\n", ecm->ecm_rx_ep.ep_addr, pipe_num);
	} else {
		RTK_LOGE(TAG, "Fail to allocate BULK IN pipe for CDC ECM class.");
		usbh_free_pipe(host, ecm->ecm_report_ep.pipe_id);
		usbh_free_pipe(host, ecm->ecm_tx_ep.pipe_id);
		return HAL_ERR_MEM;
	}

	usbh_open_pipe(host,
				   ecm->ecm_tx_ep.pipe_id,
				   ecm->ecm_tx_ep.ep_addr,
				   USB_CH_EP_TYPE_BULK,
				   ecm->ecm_tx_ep.ep_mps);

	usbh_open_pipe(host,
				   ecm->ecm_rx_ep.pipe_id,
				   ecm->ecm_rx_ep.ep_addr,
				   USB_CH_EP_TYPE_BULK,
				   ecm->ecm_rx_ep.ep_mps);

	/* ecm use bulk, the max ethernet packet size is 1542, malloc (512*3) to rx a whole ethernet packet*/
	ecm->ecm_rx_ep.xfer_len = USBH_CDC_ECM_BULK_BUF_MAX_SIZE;
	ecm->ecm_rx_ep.xfer_buf = (u8 *)usb_os_malloc(ecm->ecm_rx_ep.xfer_len);
	ecm->ecm_report_ep.xfer_buf = (u8 *)usb_os_malloc(ecm->ecm_report_ep.ep_mps);

	ecm->state = CDC_ECM_STATE_IDLE;
	ecm->usbh_state = USBH_CDC_ECM_ATTACH;

	if ((ecm->cb != NULL) && (ecm->cb->attach != NULL)) {
		ecm->cb->attach();
	}

	return HAL_OK;
}


/**
  * @brief  Usb Detach callback function.
  * @param  host: Host handle
  * @retval HAL_OK
  */
static int usbh_cdc_ecm_detach(usb_host_t *host)
{
	UNUSED(host);
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	cdc->state = CDC_ECM_STATE_IDLE;
	cdc->usbh_state = USBH_CDC_ECM_DETACHED;

	usbh_cdc_ecm_deinit_all_pipe();

	if ((cdc->cb != NULL) && (cdc->cb->detach != NULL)) {
		cdc->cb->detach();
	}

	return HAL_OK;
}

/**
  * @brief  Usb Setup callback function.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_setup(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	cdc->usbh_state = USBH_CDC_ECM_SETUP;
	cdc->state = CDC_ECM_STATE_PRE_SETTING;

	if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
		cdc->cb->setup();
	}

	status = HAL_OK;

	usbh_notify_class_state_change(host, 0);

	return status;
}


/**
  * @brief  Usb State Machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process(usb_host_t *host, u32 msg)
{
	u8 req_status = HAL_OK;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	switch (cdc->state) {

	case CDC_ECM_STATE_PRE_SETTING:
		req_status = usbh_cdc_ecm_appx_pre_setting(host);
		if (req_status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "ECM alt setting finish\n");
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, cdc->ecm_report_ep.pipe_id);
		} else {
			usbh_notify_class_state_change(host, 0);
		}
		break;

	case CDC_ECM_STATE_TRANSFER:
		if (msg == cdc->ecm_tx_ep.pipe_id) {
			cdc->next_transfer = 0;
			usbh_cdc_ecm_process_bulk_out(host);
			if (cdc->next_transfer) {
				usbh_notify_class_state_change(host, cdc->ecm_tx_ep.pipe_id);
			}
		} else if (msg == cdc->ecm_rx_ep.pipe_id) {
			cdc->next_transfer = 0;
			usbh_cdc_ecm_process_bulk_in(host);
			if (cdc->next_transfer) {
				usbh_notify_class_state_change(host, cdc->ecm_rx_ep.pipe_id);
			}
		} else if (msg == cdc->ecm_report_ep.pipe_id) {
			cdc->next_transfer = 0;
			usbh_cdc_ecm_process_intr_in(host);
			if (cdc->next_transfer) {
				usbh_notify_class_state_change(host, cdc->ecm_report_ep.pipe_id);
			}
		} else {
			usbh_cdc_ecm_appx_transfer(host, msg);
		}
		break;

	case CDC_ECM_STATE_ERROR:
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ECM_STATE_IDLE;
		}
		break;

	case CDC_ECM_STATE_IDLE:
	default:
		/* main task in idle/default status, sleep to release CPU */
		usb_os_sleep_ms(1);
		break;
	}

	return req_status;
}

/**
  * @brief  Bulk Out handling function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_ecm_process_bulk_out(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_tx_ep;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;

#if ECM_STATE_DEBUG_ENABLE
	ep->trigger_cnt++;
#endif

	switch (ep->xfer_state) {
	case CDC_ECM_TRANSFER_STATE_XFER:
		ep->xfer_state = CDC_ECM_TRANSFER_STATE_BUSY;
		usbh_bulk_send_data(host,
							ep->xfer_buf,
							ep->xfer_len,
							ep->pipe_id);

		ep->idle_tick = usbh_get_tick(host);
		ep->busy_tick = usbh_get_tick(host);
		cdc->next_transfer = 1;
		break;

	case CDC_ECM_TRANSFER_STATE_BUSY:
		urb_state = usbh_get_urb_state(host, ep->pipe_id);
		switch (urb_state) {
		case USBH_URB_DONE:
			if (ep->ep_out_zlp) { //ZLP
				ep->ep_out_zlp = 0U;
				ep->xfer_len = 0U;
				ep->xfer_buf = NULL;
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
				cdc->next_transfer = 1;
			} else {
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
				if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
					cdc->cb->bulk_send(urb_state);
				}
			}
			break;

		case USBH_URB_BUSY:
			if (usbh_get_elapsed_ticks(host, ep->busy_tick) >= USB_BULK_OUT_BUSY_MAX_CNT) { ///timeout 5ms
				if (usbh_check_nak_timeout(host, ep->pipe_id, USB_BULK_OUT_BUSY_CHECK_CNT) == HAL_OK) { // loong time not get the nak
					// RTK_LOGS(TAG, "BULK Out Busy TO %d\n",cdc->bulk_data_out_len);
					usbh_prepare_retransfer(host, ep->pipe_id);
					ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
					cdc->next_transfer = 1;
				} else {
					usbh_increase_busy_cnt(host, ep->pipe_id, 1);
					usbh_enable_nak_interrupt(host, ep->pipe_id);
					ep->busy_tick = usbh_get_tick(host);
				}
			}
			break;

		case USBH_URB_ERROR:
			// RTK_LOGD(TAG, "BULK out restart\n");
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->next_transfer = 1;
			break;

		case USBH_URB_STALL:
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
			if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
				cdc->cb->bulk_send(urb_state);
			}
			//RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK out failed %d\n", urb_state);
			break;

		case USBH_URB_IDLE:
			if (usbh_get_elapsed_ticks(host, ep->idle_tick) >= (USB_BULK_OUT_IDLE_MAX_CNT)) {
				usbh_prepare_retransfer(host, ep->pipe_id);
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
				cdc->next_transfer = 1;
			}
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}
}


/**
  * @brief  Bulk In handling  function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_ecm_process_bulk_in_req(usb_host_t *host, usbh_cdc_ecm_ep_t  *ep)
{
	u8 *buf_raw;
	u32 tick = 0;
	buf_raw = ep->xfer_buf;

	usbh_bulk_receive_data(host,
						   buf_raw,
						   ep->xfer_len,
						   ep->pipe_id);
	ep->xfer_state = CDC_ECM_TRANSFER_STATE_BUSY;
	// cdc->next_transfer = 1; //maybe can skip this
	tick = usbh_get_tick(host);
	ep->idle_tick = tick;
	ep->busy_tick = tick;
}

static void usbh_cdc_ecm_process_bulk_in(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_rx_ep;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

#if ECM_STATE_DEBUG_ENABLE
	ep->trigger_cnt++;
#endif

	switch (ep->xfer_state) {
	case CDC_ECM_TRANSFER_STATE_XFER:
		usbh_cdc_ecm_process_bulk_in_req(host, ep);
		break;

	case CDC_ECM_TRANSFER_STATE_BUSY:
		urb_state = usbh_get_urb_state(host, ep->pipe_id);
		switch (urb_state) {
		case USBH_URB_DONE: { //done
			len = usbh_get_last_transfer_size(host, cdc->ecm_rx_ep.pipe_id);
			if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL)) {
				cdc->cb->bulk_received(ep->xfer_buf, len);
			}
#if ECM_RX_IN_HIGH_TP
			usbh_cdc_ecm_process_bulk_in_req(host, ep);
#else
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
			cdc->next_transfer = 1;
#endif
		}
		break;

		case USBH_URB_BUSY:
			if (usbh_get_elapsed_ticks(host, ep->busy_tick) >= USB_BULK_IN_NAK_CHECK_MAX_CNT) {
				if (usbh_check_nak_timeout(host, ep->pipe_id, USB_BULK_IN_NAK_CHECK_CNT) == HAL_OK) {
					usbh_prepare_retransfer(host, ep->pipe_id);
					//RTK_LOGS(TAG, RTK_LOG_INFO, "BULK In Busy TO\n");
					ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
					cdc->next_transfer = 1;
				} else {
					usbh_increase_busy_cnt(host, ep->pipe_id, 1);
					usbh_enable_nak_interrupt(host, ep->pipe_id);
					ep->busy_tick = usbh_get_tick(host);
				}
			}
			break;

		case USBH_URB_ERROR: {
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->next_transfer = 1;

			ep->busy_tick = usbh_get_tick(host);;
			ep->idle_tick = usbh_get_tick(host);
		}
		break;

		case USBH_URB_STALL: {
#if ECM_RX_IN_HIGH_TP
			usbh_cdc_ecm_process_bulk_in_req(host, ep);
#else
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
#endif
#if ECM_STATE_DEBUG_ENABLE
			RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK in failed %d\n", urb_state);
#endif
		}
		break;

		case USBH_URB_IDLE: {
			if (usbh_get_elapsed_ticks(host, ep->idle_tick) >= (USB_BULK_IN_IDLE_MAX_CNT)) {
				usbh_prepare_retransfer(host, ep->pipe_id);
				//RTK_LOGS(TAG, RTK_LOG_INFO, "BULK In IDLE TO tick %d-%d/\n",ep->idle_tick, usbh_get_tick(host));
				usbh_cdc_ecm_process_bulk_in_req(host, ep);
			}
		}
		break;

		default:
			break;
		}
		break;

	default:
		cdc->next_transfer = 1;
		ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
		break;
	}
}
/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_ecm_process_intr_in(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_report_ep;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

#if ECM_STATE_DEBUG_ENABLE
	ep->trigger_cnt++;
#endif

	switch (ep->xfer_state) {
	case CDC_ECM_TRANSFER_STATE_XFER:
		usbh_intr_receive_data(host,
							   ep->xfer_buf,
							   ep->ep_mps,
							   ep->pipe_id);

		ep->idle_tick = usbh_get_tick(host);
		ep->busy_tick = usbh_get_tick(host);
		ep->xfer_state = CDC_ECM_TRANSFER_STATE_BUSY;
		// cdc->next_transfer = 1;
		break;

	case CDC_ECM_TRANSFER_STATE_BUSY:
		urb_state = usbh_get_urb_state(host, ep->pipe_id);
		switch (urb_state) {
		case USBH_URB_DONE:
			len = usbh_get_last_transfer_size(host, ep->pipe_id);
			if (len && (cdc->cb != NULL) && (cdc->cb->intr_received != NULL)) {
				cdc->cb->intr_received(ep->xfer_buf, len);
			}
			if (len > ep->ep_mps) {
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
				cdc->next_transfer = 1;
			} else {
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
			}
			break;

		case USBH_URB_BUSY:
			if (usbh_get_elapsed_ticks(host, ep->busy_tick) >= ep->ep_interval) {
				//RTK_LOGS(TAG, RTK_LOG_DEBUG, "INTR in busy too long %d-%d\n", usbh_get_tick(host), ep->busy_tick);
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
				cdc->next_transfer = 1;
			}
			break;

		case USBH_URB_ERROR:
		case USBH_URB_STALL:
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR in failed %d\n", urb_state);
			break;

		case USBH_URB_IDLE:
			if (usbh_get_elapsed_ticks(host, ep->idle_tick) >= ep->ep_interval) {
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_IDLE;
				cdc->next_transfer = 1;
			}
			break;

		default:
			break;
		}
		break;

	default:
		break;
	}
}

/**
  * @brief Check ecm bulk rx status and tiemout, whether should trigger
  * @retval Status
  */
static int usbh_cdc_ecm_bulk_rx_time_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_rx_ep;
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep->pipe_id);

	if ((USBH_URB_IDLE == urb_state) && (usbh_get_elapsed_ticks(host, ep->idle_tick) >= (USB_BULK_IN_IDLE_MAX_CNT))) {
		return HAL_OK;
	}

	if ((USBH_URB_BUSY == urb_state) && (usbh_get_elapsed_ticks(host, ep->busy_tick) >= (USB_BULK_IN_NAK_CHECK_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) { //if check done, force to trigger a msg
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief Check ecm bulk tx status and timeout, whether should trigger
  * @retval Status
  */
static int usbh_cdc_ecm_bulk_tx_time_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_tx_ep;
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep->pipe_id);

	if ((urb_state == USBH_URB_IDLE) && (usbh_get_elapsed_ticks(host, ep->idle_tick) >= (USB_BULK_OUT_IDLE_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_BUSY) && (usbh_get_elapsed_ticks(host, ep->idle_tick) >= (USB_BULK_OUT_BUSY_MAX_CNT))) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL) || (urb_state == USBH_URB_DONE)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief Check ecm intr rx status and timeout, whether should trigger
  * @retval Status
  */
static int usbh_cdc_ecm_intr_rx_time_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_report_ep;
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, ep->pipe_id);

	if ((USBH_URB_IDLE == urb_state) && (usbh_get_elapsed_ticks(host, ep->idle_tick) >= USB_INTR_IN_IDLE_MAX_CNT)) {
		return HAL_OK;
	}

	if ((USBH_URB_BUSY == urb_state) && (usbh_get_elapsed_ticks(host, ep->busy_tick) >= USB_INTR_IN_BUSY_MAX_CNT)) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
		return HAL_OK;
	}

	return HAL_BUSY;
}

/**
  * @brief  Send event for bulk tx
  * @param  none
  * @retval Status
  */
static int usbh_cdc_ecm_bulk_tx(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_tx_ep;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if (usbh_cdc_ecm_transfer_enable() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if ((ep->xfer_state == CDC_ECM_TRANSFER_STATE_IDLE) || (usbh_cdc_ecm_bulk_tx_time_check() == HAL_OK)) {
			usbh_notify_class_state_change(host, ep->pipe_id);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Send event for bulk rx
  * @param  none
  * @retval Status
  */
static int usbh_cdc_ecm_bulk_receive(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_rx_ep;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if (usbh_cdc_ecm_transfer_enable() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if ((ep->xfer_state == CDC_ECM_TRANSFER_STATE_IDLE) || (usbh_cdc_ecm_bulk_rx_time_check() == HAL_OK)) {
			if (ep->xfer_state == CDC_ECM_TRANSFER_STATE_IDLE) {
				ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			}

			usbh_notify_class_state_change(host, ep->pipe_id);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Send event for intr rx
  * @retval Status
  */
static int usbh_cdc_ecm_intr_receive(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_report_ep;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if (usbh_cdc_ecm_transfer_enable() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if ((ep->xfer_state == CDC_ECM_TRANSFER_STATE_IDLE) || (usbh_cdc_ecm_intr_rx_time_check() == HAL_OK)) {
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host,  ep->pipe_id);
			ret = HAL_OK;
		}
	}

	return ret;
}

int usbh_cdc_ecm_hand_appx_rx(usbh_cdc_ecm_ep_t  *ep)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if (ep->xfer_state == CDC_ECM_TRANSFER_STATE_IDLE) {
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			// RTK_LOGS(TAG,RTK_LOG_INFO,"Appx rx %d-%d/%d\n",cdc->state,ep->xfer_state,ep->pipe_id);
			usbh_notify_class_state_change(host, ep->pipe_id);
			ret = HAL_OK;
		}
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/
/*
	below is the private apis that called by ecm_hal
*/

/**
  * @brief  Init CDC ECM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_cdc_ecm_init(usbh_cdc_ecm_state_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (cb != NULL) {
		cdc->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "User init err %d", ret);
				return ret;
			}
		}
	}
	cdc->usbh_state = USBH_CDC_ECM_IDLE;

	usbh_register_class(&usbh_cdc_ecm_driver);

#if ECM_STATE_DEBUG_ENABLE
	usbh_cdc_ecm_debug_task_init();
#endif

	return ret;
}

/**
  * @brief  Deinit CDC ECM class
  * @retval Status
  */
int usbh_cdc_ecm_deinit(void)
{
	int ret = HAL_OK;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	usbh_unregister_class(&usbh_cdc_ecm_driver);
	cdc->usbh_state = USBH_CDC_ECM_IDLE;

#if ECM_STATE_DEBUG_ENABLE
	usbh_cdc_ecm_debug_task_deinit();
#endif

	usbh_cdc_ecm_appx_deinit(cdc->host);

	usbh_cdc_ecm_deinit_all_pipe();

	usbh_cdc_ecm_deinit_ep_buf();

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	return ret;
}

/**
  * @brief  Start to send bulk out data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_cdc_ecm_bulk_send(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_ep_t  *ep = &cdc->ecm_tx_ep;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if (usbh_cdc_ecm_transfer_enable() != HAL_OK) {
		return ret;
	}

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_TRANSFER)) {
		if (ep->xfer_state == CDC_ECM_TRANSFER_STATE_IDLE) {
			ep->xfer_buf = buf;
			ep->xfer_len = len;
			if ((len % ep->ep_mps) == 0) { //ZLP
				ep->ep_out_zlp = 1;
			} else {
				ep->ep_out_zlp = 0;
			}
			ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, ep->pipe_id);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to send bulk out data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_cdc_ecm_appx_send(u8 *buf, u32 len, usbh_cdc_ecm_ep_t *tx_ep)
{
	UNUSED(buf);
	UNUSED(len);
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (cdc->usbh_state < USBH_CDC_ECM_SETUP) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Appx not setup success,just return\n");
		return ret;
	}

	if (tx_ep == NULL || buf == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Appx param err\n");
		return ret;
	}

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_TRANSFER)) {
		if (tx_ep->xfer_state == CDC_ECM_TRANSFER_STATE_IDLE) {
			tx_ep->xfer_buf = buf;
			tx_ep->xfer_len = len;
			if ((len % tx_ep->ep_mps) == 0) { //ZLP
				tx_ep->ep_out_zlp = 1;
			} else {
				tx_ep->ep_out_zlp = 0;
			}
			tx_ep->xfer_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, tx_ep->pipe_id);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  choose the right config descriptor index while there has more then one config descriptor
  * @param  host: Host handle
  * @param  cfg_num: config index
  * @retval Status
  */
int usbh_cdc_ecm_choose_config(usb_host_t *host)
{
	//choose ecm cfg_num
	u8 cfg_id = usbh_get_configuration(host, CDC_ETHERNET_NETWORK_CONTROL_MODEL);
	return usbh_set_configuration(host, cfg_id);
}

int usbh_cdc_ecm_check_config_desc(usb_host_t *host, u8 cfg_max)
{
	usbh_cfg_desc_t *desc = NULL;
	usbh_if_desc_t *pbuf = NULL;
	u32 cfglen;
	u32 len = 0;
	u8 ecm_ctrl_valid;
	u8 ecm_data_valid;
	u8 idx;

	for (idx = 0 ; idx < cfg_max; idx ++) {
		desc = (usbh_cfg_desc_t *)usbh_get_raw_configuration_descriptor(host, idx);
		if (desc == NULL) {
			continue;
		}
		pbuf = (usbh_if_desc_t *) desc;
		cfglen = (u32)desc->wTotalLength;

		ecm_ctrl_valid = 0;
		ecm_data_valid = 0;

		while (1) {
			pbuf = (usbh_if_desc_t *)usbh_cdc_ecm_find_next_ifdesc((u8 *)pbuf, &cfglen);
			if (pbuf == NULL) {
				break;
			}

			switch (pbuf->bInterfaceClass) {
			case CDC_IF_CDC_CTRL_CODE: //cdc control  0x02
				if (pbuf->bInterfaceSubClass == CDC_IF_CDC_CTRL_SUB_CLASS_ECM_CODE) {
					ecm_ctrl_valid = 1;
				}
				len = ((usb_ecm_descriptor_header *) pbuf)->bLength;
				pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
				cfglen -= len;
				break;

			case CDC_IF_CDC_DATA_CODE: //cdc data  0x0a
				if (pbuf->bAlternateSetting == 0) { //alt setting
					len = ((usb_ecm_descriptor_header *) pbuf)->bLength;
					pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
					cfglen -= len;
				} else {
					ecm_data_valid = 1;
					len = ((usb_ecm_descriptor_header *) pbuf)->bLength;
					pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
					cfglen -= len;
				}
				break;
			default:
				len = ((usb_ecm_descriptor_header *) pbuf)->bLength;
				pbuf = (usbh_if_desc_t *)((u8 *)pbuf + pbuf->bLength);
				cfglen -= len;
				break;
			}
		}

		if (ecm_ctrl_valid && ecm_data_valid) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Check success idx %d max %d\n", idx, cfg_max);
			return HAL_OK;
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Check fail idx %d max %d[%d-%d]\n", idx, cfg_max, ecm_ctrl_valid, ecm_data_valid);
		}
	}

	return HAL_ERR_PARA;
}

/**
  * @brief  check USB device enum status
  * @retval return HAL_OK if enum success, else return HAL_BUSY
  */
int usbh_cdc_ecm_check_enum_status(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (cdc->usbh_state < USBH_CDC_ECM_SETUP) {
		// RTK_LOGS(TAG, RTK_LOG_INFO, "Device not setup success,wait...\n");
		return HAL_BUSY;
	}

	return HAL_OK;
}

/**
  * @brief  return bulkin MPS
  * @retval MPS
  */
u16 usbh_cdc_ecm_get_usbin_mps(void)
{
	u32 i = 0;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	while (cdc->usbh_state < USBH_CDC_ECM_SETUP && i < 10) {
		usb_os_sleep_ms(1000);
		i++;
		RTK_LOGS(TAG, RTK_LOG_INFO, "State(%d) want(%d) keep wait\n", cdc->usbh_state, USBH_CDC_ECM_SETUP);
	}
	return cdc->ecm_rx_ep.ep_mps;
}

/**
  * @brief  return interrupt value in ms
  * @retval interval
  */
u32 usbh_cdc_ecm_get_intr_interval(void)
{
	u32 ret = 1;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	if (cdc->usbh_state >= USBH_CDC_ECM_SETUP) {
		if (usbh_get_device_speed(cdc->host) == USB_SPEED_HIGH) {
			ret = cdc->ecm_report_ep.ep_interval / 8;
		} else {
			ret = cdc->ecm_report_ep.ep_interval;
		}

		if (ret == 0) {
			ret = 1;
		}
	}

	return ret;
}

/**
  * @brief return the device vid
  * @retval
  */
u16 usbh_cdc_ecm_get_device_vid_info(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	return cdc->vid;
}
/**
  * @brief return the device pid
  * @retval
  */
u16 usbh_cdc_ecm_get_device_pid_info(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	return cdc->pid;
}

/**
  * @brief return the ecm interface desc bAlternateSetting
  * @retval
  */
u8 usbh_cdc_ecm_get_ecm_itf_ifnum(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	return cdc->ecm_if;
}
/**
  * @brief return the ecm interface desc bAlternateSetting
  * @retval
  */
u8 usbh_cdc_ecm_get_ecm_itf_alt(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	return cdc->ecm_alt;
}

u8 usbh_cdc_ecm_trx_prepare_done(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	cdc->allow_ecm_xfer = 1;

	return 0;
}

#if ECM_STATE_DEBUG_ENABLE
static void usbh_cdc_ecm_debug_task(void *param)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	UNUSED(param);
	ecm_debug_task_init = 1;
	while (1) {
		if (cdc->host) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Status %d-%d-%d/out %d-%d/in %d-%d/intr %d-%d/%d-%d-%d\n",
					 cdc->state, usbh_cdc_ecm_get_hw_statue(), usbh_cdc_ecm_get_usb_statue(),
					 cdc->ecm_tx_ep.xfer_state, usbh_get_urb_state(cdc->host, cdc->ecm_tx_ep.pipe_id),
					 cdc->ecm_rx_ep.xfer_state, usbh_get_urb_state(cdc->host, cdc->ecm_rx_ep.pipe_id),
					 cdc->ecm_report_ep.xfer_state, usbh_get_urb_state(cdc->host, cdc->ecm_report_ep.pipe_id),
					 cdc->ecm_tx_ep.trigger_cnt, cdc->ecm_rx_ep.trigger_cnt, cdc->ecm_report_ep.trigger_cnt
					);
		}
		usb_os_sleep_ms(1000);
	}
}

void usbh_cdc_ecm_debug_task_init(void)
{
	int status;
	if (ecm_debug_task_init) {
		return;
	}

	status = rtos_task_create(&ecm_debug_task, "Ecm_debug", usbh_cdc_ecm_debug_task, NULL, 1024U * 2, 1U);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create ecm dump task fail\n");
	}
}

void usbh_cdc_ecm_debug_task_deinit(void)
{
	if (ecm_debug_task != NULL) {
		RTK_LOGI(TAG, "Del monitor_task task\n");
		rtos_task_delete(ecm_debug_task);
		ecm_debug_task = NULL;
	}

	ecm_debug_task_init = 0;
}
#endif
