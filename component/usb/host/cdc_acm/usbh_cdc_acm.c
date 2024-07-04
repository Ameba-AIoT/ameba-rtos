/**
  ******************************************************************************
  * @file    usbh_cdc_acm.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB CDC ACM Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbh_cdc_acm.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/

#define DMA_TRANSFER_LARGE_DATA             0

#define USB_BULK_OUT_IDLE_MAX_CNT           8000 //sof
#define USB_BULK_IN_IDLE_MAX_CNT            100  //sof
#define USB_INTR_BUSY_MAX_CNT               1000

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_cdc_acm_attach(usb_host_t *host);
static int usbh_cdc_acm_detach(usb_host_t *host);
static int usbh_cdc_acm_process(usb_host_t *host);
static int usbh_cdc_acm_setup(usb_host_t *host);
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usbh_cdc_acm_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usbh_cdc_acm_line_coding_t *linecoding);
static int usbh_cdc_acm_process_set_control_line_state(usb_host_t *host);
static void usbh_cdc_acm_process_tx(usb_host_t *host);
static void usbh_cdc_acm_process_rx(usb_host_t *host);
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host);
static int usbh_cdc_acm_nak(usb_host_t *host, u8 pipe_num);

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "ACM";

/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_cdc_acm_driver = {
	.class_code = CDC_CLASS_CODE,
	.attach = usbh_cdc_acm_attach,
	.detach = usbh_cdc_acm_detach,
	.setup = usbh_cdc_acm_setup,
	.process = usbh_cdc_acm_process,
	.nak = usbh_cdc_acm_nak,
};

static usbh_cdc_acm_host_t usbh_cdc_acm_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_acm_attach(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	u8 pipe_num;
	u8 interface;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_if_desc_t *comm_if_desc;
	usbh_if_desc_t *data_if_desc;
	usbh_ep_desc_t *ep_desc;

	/* Get interface index as per supported class & protocol */
	interface = usbh_get_interface(host,
									   CDC_COMM_INTERFACE_CLASS_CODE,
									   CDC_ABSTRACT_CONTROL_MODEL,
									   CDC_CTRL_PROTOCOL_COMMON_AT_COMMAND);
	if (interface == 0xFFU) {
		RTK_LOGS(TAG, "[ACM] Get itf fail\n");
		return status;
	}

	cdc->host = host;

	/* Set active interface */
	usbh_set_interface(host, interface);

	/* Get Communication Interface */
	comm_if_desc = usbh_get_interface_descriptor(host, interface, 0);
	if (comm_if_desc == NULL) {
		RTK_LOGS(TAG, "[ACM] Get comm itf fail\n");
		return status;
	}

	/* Set notification endpoint */
	ep_desc = &comm_if_desc->ep_desc_array[0];
	if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
		cdc->comm_if.intr_in_ep = ep_desc->bEndpointAddress;
		cdc->comm_if.intr_in_packet_size  = ep_desc->wMaxPacketSize;
		cdc->comm_if.intr_in_ep_interval = usbh_get_interval(cdc->host, USB_CH_EP_TYPE_INTR, ((usbh_ep_desc_t *)ep_desc)->bInterval);
	}

	pipe_num = usbh_alloc_pipe(host, cdc->comm_if.intr_in_ep);
	if (pipe_num != 0xFFU) {
		cdc->comm_if.intr_in_pipe = pipe_num;
	} else {
		RTK_LOGS(TAG, "[ACM] Alloc INTR in pipe fail\n");
		return HAL_ERR_MEM;
	}

	usbh_open_pipe(host,
				   cdc->comm_if.intr_in_pipe,
				   cdc->comm_if.intr_in_ep,
				   USB_CH_EP_TYPE_INTR,
				   cdc->comm_if.intr_in_packet_size);

	/* Get Data Interface */
	interface = usbh_get_interface(host,
									   CDC_DATA_INTERFACE_CLASS_CODE,
									   CDC_RESERVED,
									   CDC_CTRL_PROTOCOL_NO_CLASS_SPECIFIC);
	if (interface == 0xFFU) {
		RTK_LOGS(TAG, "[ACM] Get data itf fail\n");
		return status;
	}

	data_if_desc = usbh_get_interface_descriptor(host, interface, 0);
	if (data_if_desc == NULL) {
		RTK_LOGS(TAG, "[ACM] Get itf desc fail\n");
		return status;
	}

	/* Set data in/out endpoints */
	ep_desc = &data_if_desc->ep_desc_array[0];
	if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
		cdc->data_if.bulk_in_ep = ep_desc->bEndpointAddress;
		cdc->data_if.bulk_in_packet_size  = ep_desc->wMaxPacketSize;
	} else {
		cdc->data_if.bulk_out_ep = ep_desc->bEndpointAddress;
		cdc->data_if.bulk_out_packet_size  = ep_desc->wMaxPacketSize;
	}

	ep_desc = &data_if_desc->ep_desc_array[1];
	if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
		cdc->data_if.bulk_in_ep = ep_desc->bEndpointAddress;
		cdc->data_if.bulk_in_packet_size = ep_desc->wMaxPacketSize;
	} else {
		cdc->data_if.bulk_out_ep = ep_desc->bEndpointAddress;
		cdc->data_if.bulk_out_packet_size  = ep_desc->wMaxPacketSize;
	}

	pipe_num = usbh_alloc_pipe(host, cdc->data_if.bulk_out_ep);
	if (pipe_num != 0xFFU) {
		cdc->data_if.bulk_out_pipe = pipe_num;
	} else {
		RTK_LOGS(TAG, "[ACM] Alloc BULK out pipe fail\n");
		usbh_free_pipe(host, cdc->comm_if.intr_in_pipe);
		return HAL_ERR_MEM;
	}

	pipe_num = usbh_alloc_pipe(host, cdc->data_if.bulk_in_ep);
	if (pipe_num != 0xFFU) {
		cdc->data_if.bulk_in_pipe = pipe_num;
	} else {
		RTK_LOGS(TAG, "[ACM] Alloc BULK in pipe fail\n");
		usbh_free_pipe(host, cdc->comm_if.intr_in_pipe);
		usbh_free_pipe(host, cdc->data_if.bulk_out_pipe);
		return HAL_ERR_MEM;
	}

	usbh_open_pipe(host,
				   cdc->data_if.bulk_out_pipe,
				   cdc->data_if.bulk_out_ep,
				   USB_CH_EP_TYPE_BULK,
				   cdc->data_if.bulk_out_packet_size);

	usbh_open_pipe(host,
				   cdc->data_if.bulk_in_pipe,
				   cdc->data_if.bulk_in_ep,
				   USB_CH_EP_TYPE_BULK,
				   cdc->data_if.bulk_in_packet_size);

	cdc->state = CDC_ACM_STATE_IDLE;

	if ((cdc->cb != NULL) && (cdc->cb->attach != NULL)) {
		cdc->cb->attach();
	}

	status = HAL_OK;

	return status;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_acm_detach(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	if ((cdc->cb != NULL) && (cdc->cb->detach != NULL)) {
		cdc->cb->detach();
	}

	if (cdc->comm_if.intr_in_pipe) {
		usbh_close_pipe(host, cdc->comm_if.intr_in_pipe);
		usbh_free_pipe(host, cdc->comm_if.intr_in_pipe);
		cdc->comm_if.intr_in_pipe = 0U;
	}

	if (cdc->data_if.bulk_in_pipe) {
		usbh_close_pipe(host, cdc->data_if.bulk_in_pipe);
		usbh_free_pipe(host, cdc->data_if.bulk_in_pipe);
		cdc->data_if.bulk_in_pipe = 0U;
	}

	if (cdc->data_if.bulk_out_pipe) {
		usbh_close_pipe(host, cdc->data_if.bulk_out_pipe);
		usbh_free_pipe(host, cdc->data_if.bulk_out_pipe);
		cdc->data_if.bulk_out_pipe = 0U;
	}

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_acm_setup(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN ;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	/*Issue the get line coding request*/
	status = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
	if (status == HAL_OK) {
		if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
			cdc->cb->setup();
		}
	}
	return status;
}

/**
  * @brief  Usb nak callback function.
  * @param  host: Host handle
  * @param  pipe_num: Pipe number
  * @retval return OK while success, else return HAL_ERR_UNKNOWN
  */
static int usbh_cdc_acm_nak(usb_host_t *host, u8 pipe_num)
{
	u8 ep_type ;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	if (pipe_num >= USB_MAX_PIPES) {
		return HAL_ERR_PARA ;
	}

	ep_type = usbh_get_ep_type(host, pipe_num);
	if (ep_type == USB_CH_EP_TYPE_INTR) {
		cdc->intr_in_busy_tick = host->tick;
		cdc->intr_in_idle_tick = host->tick;
		//INTR ,NAK did not retrigger the EP,wait for next binterval
		return HAL_OK;
	}

	return HAL_ERR_UNKNOWN ;
}

/**
* @brief  State machine handling callback
* @param  host:Host handle
* @retval Status
*/
static int usbh_cdc_acm_process(usb_host_t *host)
{
	int status = HAL_BUSY;
	u8 req_status = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	switch (cdc->state) {

	case CDC_ACM_STATE_IDLE:
		status = HAL_OK;
		break;

	case CDC_ACM_STATE_SET_CONTROL_LINE_STATE:
		req_status = usbh_cdc_acm_process_set_control_line_state(host);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ACM_STATE_IDLE;
		} else if (req_status != HAL_BUSY) {
			cdc->state = CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host);
		break;

	case CDC_ACM_STATE_SET_LINE_CODING:
		req_status = usbh_cdc_acm_process_set_line_coding(host, cdc->user_line_coding);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ACM_STATE_GET_LINE_CODING;
		} else if (req_status != HAL_BUSY) {
			cdc->state = CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host);
		break;

	case CDC_ACM_STATE_GET_LINE_CODING:
		req_status = usbh_cdc_acm_process_get_line_coding(host, cdc->line_coding);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ACM_STATE_IDLE;
			if ((cdc->line_coding->b.bCharFormat == cdc->user_line_coding->b.bCharFormat) &&
				(cdc->line_coding->b.bDataBits == cdc->user_line_coding->b.bDataBits) &&
				(cdc->line_coding->b.bParityType == cdc->user_line_coding->b.bParityType) &&
				(cdc->line_coding->b.dwDteRate == cdc->user_line_coding->b.dwDteRate)) {
				if ((cdc->cb != NULL) && (cdc->cb->line_coding_changed != NULL)) {
					cdc->cb->line_coding_changed(cdc->line_coding);
				}
			}
		} else if (req_status != HAL_BUSY) {
			cdc->state = CDC_ACM_STATE_ERROR;
		}
		usbh_notify_class_state_change(host);
		break;

	case CDC_ACM_STATE_TRANSFER:
		usbh_cdc_acm_process_tx(host);
		usbh_cdc_acm_process_rx(host);
		usbh_cdc_acm_process_intr_rx(host);
		break;

	case CDC_ACM_STATE_ERROR:
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ACM_STATE_IDLE;
		}
		break;

	default:
		break;

	}

	return status;
}

/**
  * @brief  Get line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_cdc_acm_process_get_line_coding(usb_host_t *host, usbh_cdc_acm_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;

	setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = CDC_GET_LINE_CODING;
	setup.b.wValue = 0U;
	setup.b.wIndex = 0U;
	setup.b.wLength = CDC_LINE_CODING_DATA_LEN;

	return usbh_ctrl_request(host, &setup, linecoding->d8);
}

/**
  * @brief  Set line coding
  * @param  host: Host handle
  * @param  linecoding: Line coding data pointer
  * @retval Status
  */
static int usbh_cdc_acm_process_set_line_coding(usb_host_t *host, usbh_cdc_acm_line_coding_t *linecoding)
{
	usbh_setup_req_t setup;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = CDC_SET_LINE_CODING;
	setup.b.wValue = 0U;
	setup.b.wIndex = 0U;
	setup.b.wLength = CDC_LINE_CODING_DATA_LEN;

	return usbh_ctrl_request(host, &setup, linecoding->d8);
}

/**
  * @brief  Set control line
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_acm_process_set_control_line_state(usb_host_t *host)
{
	usbh_setup_req_t setup;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = CDC_SET_CONTROL_LINE_STATE;
	setup.b.wValue = 1U;
	setup.b.wIndex = 0U;
	setup.b.wLength = 0U;
	return usbh_ctrl_request(host, &setup, NULL);
}
/**
  * @brief  TX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_cdc_acm_process_tx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;

	switch (cdc->data_tx_state) {
	case CDC_ACM_TRANSFER_STATE_TX:
#if DMA_TRANSFER_LARGE_DATA
		usbh_bulk_send_data(host,
							cdc->tx_buf,
							(u16)cdc->tx_len,
							cdc->data_if.bulk_out_pipe);
#else
		if (cdc->tx_len > cdc->data_if.bulk_out_packet_size) {
			usbh_bulk_send_data(host,
								cdc->tx_buf,
								cdc->data_if.bulk_out_packet_size,
								cdc->data_if.bulk_out_pipe);
		} else {
			usbh_bulk_send_data(host,
								cdc->tx_buf,
								(u16)cdc->tx_len,
								cdc->data_if.bulk_out_pipe);
		}
#endif
		cdc->data_tx_state = CDC_ACM_TRANSFER_STATE_TX_BUSY;
		cdc->tx_idle_tick = host->tick;
		usbh_notify_class_state_change(host);
		break;

	case CDC_ACM_TRANSFER_STATE_TX_BUSY:
		urb_state = usbh_get_urb_state(host, cdc->data_if.bulk_out_pipe);
		if (urb_state == USBH_URB_DONE) {
#if  DMA_TRANSFER_LARGE_DATA
			if (cdc->tx_zlp) { //ZLP
				cdc->tx_zlp = 0U;
				cdc->tx_len = 0U;
				cdc->tx_buf = NULL;
				cdc->data_tx_state = CDC_ACM_TRANSFER_STATE_TX;
			} else {
				cdc->data_tx_state = CDC_ACM_TRANSFER_STATE_IDLE;
				if ((cdc->cb != NULL) && (cdc->cb->transmit != NULL)) {
					cdc->cb->transmit(urb_state);
				}
			}
#else
			if (cdc->tx_len >= cdc->data_if.bulk_out_packet_size) {
				cdc->tx_len -= cdc->data_if.bulk_out_packet_size;
				cdc->tx_buf += cdc->data_if.bulk_out_packet_size;
				cdc->data_tx_state = CDC_ACM_TRANSFER_STATE_TX;
			} else {
				cdc->tx_len = 0U;
				cdc->data_tx_state = CDC_ACM_TRANSFER_STATE_IDLE;
				if ((cdc->cb != NULL) && (cdc->cb->transmit != NULL)) {
					cdc->cb->transmit(urb_state);
				}
			}
#endif
			usbh_notify_class_state_change(host);
		} else if (urb_state == USBH_URB_BUSY) {
			cdc->data_tx_state = CDC_ACM_TRANSFER_STATE_TX;
			usbh_notify_class_state_change(host);
		} else if (urb_state == USBH_URB_ERROR) {
			cdc->data_tx_state = CDC_ACM_TRANSFER_STATE_IDLE;
			if ((cdc->cb != NULL) && (cdc->cb->transmit != NULL)) {
				cdc->cb->transmit(urb_state);
			}
			usbh_notify_class_state_change(host);
		} else if (urb_state == USBH_URB_IDLE) {
			if (usbh_get_elapsed_ticks(host, cdc->tx_idle_tick) >= USB_BULK_OUT_IDLE_MAX_CNT) {
				cdc->data_tx_state = CDC_ACM_TRANSFER_STATE_TX;
			}
			usbh_notify_class_state_change(host);
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  RX handling
  * @param  host: Host handle
  * @retval None
*/
static void usbh_cdc_acm_process_rx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (cdc->data_rx_state) {
	case CDC_ACM_TRANSFER_STATE_RX:
#if DMA_TRANSFER_LARGE_DATA
		usbh_bulk_receive_data(host,
							   cdc->rx_buf,
							   (u16)cdc->rx_len,
							   cdc->data_if.bulk_in_pipe);
#else
		if (cdc->rx_len > cdc->data_if.bulk_in_packet_size) {
			usbh_bulk_receive_data(host,
								   cdc->rx_buf,
								   cdc->data_if.bulk_in_packet_size, //just read one MPS
								   cdc->data_if.bulk_in_pipe);
		} else {
			usbh_bulk_receive_data(host,
								   cdc->rx_buf,
								   (u16)cdc->rx_len,
								   cdc->data_if.bulk_in_pipe);
		}
#endif

		cdc->data_rx_state = CDC_ACM_TRANSFER_STATE_RX_BUSY;
		cdc->rx_idle_tick = host->tick;
		break;

	case CDC_ACM_TRANSFER_STATE_RX_BUSY:
		urb_state = usbh_get_urb_state(host, cdc->data_if.bulk_in_pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, cdc->data_if.bulk_in_pipe);

			if ((cdc->cb != NULL) && (cdc->cb->receive != NULL)) {
				cdc->cb->receive(cdc->rx_buf, len);
			}

			//should handle the ZLP packet
			if ((cdc->rx_len >= len) && (len >= cdc->data_if.bulk_in_packet_size)) {
				cdc->rx_len -= len ;
				cdc->rx_buf += len;
				cdc->data_rx_state = CDC_ACM_TRANSFER_STATE_RX;
			} else {
				cdc->rx_len = 0;
				cdc->data_rx_state = CDC_ACM_TRANSFER_STATE_IDLE;
			}

			usbh_notify_class_state_change(host);
		} else if (urb_state == USBH_URB_IDLE) {
			if (usbh_get_elapsed_ticks(host, cdc->rx_idle_tick) >= USB_BULK_IN_IDLE_MAX_CNT) {
				cdc->data_rx_state = CDC_ACM_TRANSFER_STATE_RX;
			}
			usbh_notify_class_state_change(host);
		}
		break;

	default:
		break;
	}
}

/**
  * @brief  Intr in function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_acm_process_intr_rx(usb_host_t *host)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (cdc->intr_data_rx_state) {

	case CDC_ACM_TRANSFER_STATE_NOTIFY_RX:
		cdc->intr_data_rx_state = CDC_ACM_TRANSFER_STATE_NOTIFY_RX_BUSY;
		usbh_intr_receive_data(host,
							   cdc->intr_rx_buf,
							   cdc->comm_if.intr_in_packet_size,
							   cdc->comm_if.intr_in_pipe);
		cdc->intr_in_idle_tick = host->tick;
		cdc->intr_in_busy_tick = host->tick;
		usbh_notify_class_state_change(host);
		break;

	case CDC_ACM_TRANSFER_STATE_NOTIFY_RX_BUSY:
		urb_state = usbh_get_urb_state(host, cdc->comm_if.intr_in_pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, cdc->comm_if.intr_in_pipe);
			if ((cdc->cb != NULL) && (cdc->cb->notify != NULL)) {
				cdc->cb->notify(cdc->intr_rx_buf, len);
			}

			cdc->intr_data_rx_state = CDC_ACM_TRANSFER_STATE_IDLE;
		} else if (urb_state == USBH_URB_BUSY) {
			if (usbh_get_elapsed_ticks(host, cdc->intr_in_busy_tick) >= USB_INTR_BUSY_MAX_CNT) {
				cdc->intr_data_rx_state = CDC_ACM_TRANSFER_STATE_NOTIFY_RX;
			}
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			cdc->intr_data_rx_state = CDC_ACM_TRANSFER_STATE_NOTIFY_RX;
			RTK_LOGS(TAG, "[ACM] INTR in failed %d\n", urb_state);
		} else if (urb_state == USBH_URB_IDLE) {
			if (usbh_get_elapsed_ticks(host, cdc->intr_in_idle_tick) >= cdc->comm_if.intr_in_ep_interval) { //
				cdc->intr_data_rx_state = CDC_ACM_TRANSFER_STATE_NOTIFY_RX;
			}
		}
		usbh_notify_class_state_change(host);
		break;

	default:
		break;
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init CDC ACM class
  * @param  cb: User callback
  * @retval Status
  */
int usbh_cdc_acm_init(usbh_cdc_acm_cb_t *cb)
{
	int ret = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;

	if (cb != NULL) {
		cdc->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGS(TAG, "[ACM] User init err %d\n", ret);
				return ret;
			}
		}
	}

	cdc->line_coding = (usbh_cdc_acm_line_coding_t *)usb_os_malloc(sizeof(usbh_cdc_acm_line_coding_t));
	if (cdc->line_coding == NULL) {
		RTK_LOGS(TAG, "[ACM] Malloc line code fail\n");
		return HAL_ERR_MEM;
	}
	cdc->user_line_coding = (usbh_cdc_acm_line_coding_t *)usb_os_malloc(sizeof(usbh_cdc_acm_line_coding_t));
	if (cdc->user_line_coding == NULL) {
		RTK_LOGS(TAG, "[ACM] Malloc user line code fail\n");
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
		return HAL_ERR_MEM;
	}

	usbh_register_class(&usbh_cdc_acm_driver);

	return ret;
}

/**
  * @brief  Deinit CDC ACM class
  * @retval Status
  */
int usbh_cdc_acm_deinit(void)
{
	int ret = HAL_OK;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	if ((host != NULL) && (host->state == USBH_CLASS_READY)) {
		cdc->state = CDC_ACM_STATE_IDLE;

		usbh_close_pipe(host, cdc->comm_if.intr_in_pipe);
		usbh_close_pipe(host, cdc->data_if.bulk_in_pipe);
		usbh_close_pipe(host, cdc->data_if.bulk_out_pipe);
	}

	if (cdc->line_coding != NULL) {
		usb_os_mfree(cdc->line_coding);
		cdc->line_coding = NULL;
	}
	if (cdc->user_line_coding != NULL) {
		usb_os_mfree(cdc->user_line_coding);
		cdc->user_line_coding = NULL;
	}

	usbh_unregister_class(&usbh_cdc_acm_driver);

	return ret;
}

/**
* @brief  Set the control line status
* @retval Status
*/
int usbh_cdc_acm_set_control_line_state(void)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if (host->state == USBH_CLASS_READY) {
		cdc->state = CDC_ACM_STATE_SET_CONTROL_LINE_STATE;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Prepares the state before issuing the class specific commands
* @param  line_coding: Line coding data
* @retval Status
*/
int usbh_cdc_acm_set_line_coding(usbh_cdc_acm_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if (host->state == USBH_CLASS_READY) {
		cdc->state = CDC_ACM_STATE_SET_LINE_CODING;
		cdc->user_line_coding->b.dwDteRate = line_coding->b.dwDteRate;
		cdc->user_line_coding->b.bCharFormat = line_coding->b.bCharFormat;
		cdc->user_line_coding->b.bParityType = line_coding->b.bParityType;
		cdc->user_line_coding->b.bDataBits = line_coding->b.bDataBits;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}

/**
* @brief  Prepares the state before issuing the class specific commands
* @param  line_coding: Line coding data
* @retval Status
*/
int usbh_cdc_acm_get_line_coding(usbh_cdc_acm_line_coding_t *line_coding)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if ((host->state == USBH_CLASS_READY) || (host->state == USBH_CLASS_REQUEST)) {
		line_coding->b.dwDteRate = cdc->line_coding->b.dwDteRate;
		line_coding->b.bCharFormat = cdc->line_coding->b.bCharFormat;
		line_coding->b.bParityType = cdc->line_coding->b.bParityType;
		line_coding->b.bDataBits = cdc->line_coding->b.bDataBits;
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Start to transfer data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_cdc_acm_transmit(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;

	if ((cdc->data_tx_state == CDC_ACM_TRANSFER_STATE_IDLE)
		&& ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER))) {
		cdc->tx_buf = buf;
		cdc->tx_len = len;
		if ((len % cdc->data_if.bulk_out_packet_size) == 0) { //ZLP
			cdc->tx_zlp = 1;
		} else {
			cdc->tx_zlp = 0;
		}
		cdc->state = CDC_ACM_STATE_TRANSFER;
		cdc->data_tx_state = CDC_ACM_TRANSFER_STATE_TX;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Start to receive data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_cdc_acm_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;

	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		if (cdc->data_rx_state == CDC_ACM_TRANSFER_STATE_IDLE) {
			cdc->rx_buf = buf;
			cdc->rx_len = len;
			cdc->state = CDC_ACM_STATE_TRANSFER;
			cdc->data_rx_state = CDC_ACM_TRANSFER_STATE_RX;
			usbh_notify_class_state_change(host);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to receive notify data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_cdc_acm_notify_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	usb_host_t *host = cdc->host;

	if ((cdc->state == CDC_ACM_STATE_IDLE) || (cdc->state == CDC_ACM_STATE_TRANSFER)) {
		cdc->intr_rx_buf = buf;
		cdc->intr_rx_len = len;

		/* the user buf len < MPS, update the rx buf length */
		if (len < cdc->comm_if.intr_in_packet_size) {
			RTK_LOGS(TAG, "[ACM] Pls inc inbuf len %d-%d\n", len, cdc->comm_if.intr_in_packet_size);
			cdc->comm_if.intr_in_packet_size = len;
		}

		cdc->state = CDC_ACM_STATE_TRANSFER;
		cdc->intr_data_rx_state = CDC_ACM_TRANSFER_STATE_NOTIFY_RX;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  Get the BULK IN packet size
  * @retval packet size
  */
u16 usbh_cdc_acm_get_bulk_ep_mps(void)
{
	usbh_cdc_acm_host_t *cdc = &usbh_cdc_acm_host;
	return cdc->data_if.bulk_in_packet_size;
}

