/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/
#include "usbh.h"
#include "usbh_core.h"
#include "usbh_hal.h"
#include "usbh_hcd.h"


/* Private defines -----------------------------------------------------------*/
static const char *const TAG = "CTS";

/* Exported defines ----------------------------------------------------------*/

/* Class Codes */
#define XFER_TEST_CLASS_CODE                              0x02U
#define XFER_TEST_INTERFACE_CLASS_CODE                    0x02U
#define XFER_TEST_ABSTRACT_CONTROL_MODEL                  0x02U
#define XFER_TEST_CTRL_PROTOCOL_COMMON_AT_COMMAND         0x01U

/* Exported types ------------------------------------------------------------*/

/* States for State Machine */
typedef enum {
	CTS_XFER_TEST_STATE_IDLE = 0U,
	CTS_XFER_TEST_STATE_RX_START,
	CTS_XFER_TEST_STATE_RX_CONTINUE,
	CTS_XFER_TEST_STATE_RX_DONE,
	CTS_XFER_TEST_STATE_ERROR,
} usbh_cts_test_transfer_state_t;

/* transfer state */
typedef enum {
	XFER_TEST_STATE_IDLE = 0U,
	XFER_TEST_STATE_TRANSFER,
	XFER_TEST_STATE_ERROR,
} usbh_cts_xfer_test_state_t;

/* xfer test host */
typedef struct {
	u32                                 rx_len;
	u8                                  *rx_buf;
	usbh_cts_test_transfer_state_t      data_rx_state;
	usbh_cts_xfer_test_state_t          state;

	usb_host_t                          *host;
} usbh_cts_xfer_test_host_t;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_cts_xfer_test_attach(usb_host_t *host);
static int usbh_cts_xfer_test_detach(usb_host_t *host);
static int usbh_cts_xfer_test_setup(usb_host_t *host);
static int usbh_cts_xfer_test_process(usb_host_t *host, u32 msg);
static int usbh_cts_xfer_test_process_rx(usb_host_t *host);

/* Private variables ---------------------------------------------------------*/

/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_driver = {
	.class_code = XFER_TEST_CLASS_CODE,
	.attach = usbh_cts_xfer_test_attach,
	.detach = usbh_cts_xfer_test_detach,
	.setup = usbh_cts_xfer_test_setup,
	.process = usbh_cts_xfer_test_process,
};

static __IO u8 xfer_test_is_ready = 0;
static __IO u8 cts_xfer_test_flag = 0;
static usbh_cts_xfer_test_host_t usbh_cts_xfer_test_host;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cts_xfer_test_attach(usb_host_t *host)
{
	u8 interface;
	usbh_cts_xfer_test_host_t *xfer_test = &usbh_cts_xfer_test_host;

	/* Get interface index as per supported class & protocol */
	interface = usbh_get_interface(host,
									   XFER_TEST_INTERFACE_CLASS_CODE,
									   XFER_TEST_ABSTRACT_CONTROL_MODEL,
									   XFER_TEST_CTRL_PROTOCOL_COMMON_AT_COMMAND);
	if (interface == 0xFFU) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to get itf\n");
		return HAL_ERR_UNKNOWN;
	}

	xfer_test->host = host;
	/* Set active interface */
	usbh_set_interface(host, interface);

	xfer_test->state = XFER_TEST_STATE_IDLE;

	return HAL_OK;
}
static int usbh_cts_xfer_test_detach(usb_host_t *host)
{
	UNUSED(host);
	xfer_test_is_ready = 0;

	return HAL_OK;
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cts_xfer_test_setup(usb_host_t *host)
{
	UNUSED(host);
	xfer_test_is_ready = 1;

	return HAL_OK;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cts_xfer_test_process(usb_host_t *host, u32 msg)
{
	UNUSED(msg);
	int status = HAL_BUSY;
	int req_status = HAL_OK;
	usbh_cts_xfer_test_host_t *xfer_test = &usbh_cts_xfer_test_host;

	switch (xfer_test->state) {

	case XFER_TEST_STATE_IDLE:
		status = HAL_OK;
		break;

	case XFER_TEST_STATE_TRANSFER:
		usbh_cts_xfer_test_process_rx(host); ///while success,states will switch to idle
		usbh_notify_class_state_change(host, 0);
		break;

	case XFER_TEST_STATE_ERROR:
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			xfer_test->state = XFER_TEST_STATE_IDLE;
			usbh_notify_class_state_change(host, 0);
		}
		break;

	default:
		break;

	}

	return status;
}



/**
  * @brief  RX handling
  * @param  host: Host handle
  * @retval None
*/
static int usbh_cts_ctrl_process(usbh_core_t *core)
{
	u8 direction;
	u8 do_ping;
	u32 host_msg = USBH_CONTROL_EVENT;
	int status = HAL_BUSY;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	usb_host_t *host = core->host;

	switch (core->ctrl_req.state) {
	case USBH_CTRL_SETUP:
		/* send a SETUP packet */
		usbh_hcd_hc_submit_request((usbh_hcd_t *)core->hcd,
								   core->ctrl_req.pipe_out,				/* Pipe index */
								   0U, 					                /* Direction : OUT */
								   USB_CH_EP_TYPE_CTRL,		           /* EP type */
								   USBH_TOKEN_SETUP,			       /* Type Data */
								   (u8 *)(void *)core->ctrl_req.setup->d32,	 /* Data buffer */
								   USBH_SETUP_PKT_SIZE,	               /* Data length */
								   0U);

		core->ctrl_req.state = USBH_CTRL_SETUP_BUSY;
		break;

	case USBH_CTRL_SETUP_BUSY:
		urb_state = usbh_core_get_urb_state(core, core->ctrl_req.pipe_out);
		/* case SETUP packet sent successfully */
		if (urb_state == USBH_URB_DONE) {
			direction = (core->ctrl_req.setup->b.bmRequestType & USB_REQ_DIR_MASK);

			/* check if there is a data stage */
			if (core->ctrl_req.setup->b.wLength != 0U) {
				if (direction == USB_D2H) {
					/* Data Direction is IN */
					//check whether cts cmd allow
					if (cts_xfer_test_flag) {
						usb_os_queue_send(core->msg_queue, &host_msg, 0U);
						break;
					} else {
						core->ctrl_req.state = USBH_CTRL_DATA_IN;
					}
				}
			}
			usb_os_queue_send(core->msg_queue, &host_msg, 0U);
		} else {
			if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_BUSY)) {
				core->ctrl_req.state = USBH_CTRL_ERROR;
				usb_os_queue_send(core->msg_queue, &host_msg, 0U);
			}
		}
		break;

	case USBH_CTRL_DATA_IN:
		/* Issue an IN token */
		usbh_hcd_hc_submit_request((usbh_hcd_t *)core->hcd,
								   core->ctrl_req.pipe_in,			/* Pipe index */
								   1U, 				                /* Direction : IN */
								   USB_CH_EP_TYPE_CTRL,	            /* EP type */
								   USBH_TOKEN_DATA,		            /* Type Data */
								   core->ctrl_req.buf,				/* Data buffer */
								   core->ctrl_req.length,			/* Data length */
								   0U);

		core->ctrl_req.state = USBH_CTRL_DATA_IN_BUSY;
		break;

	case USBH_CTRL_DATA_IN_BUSY:

		urb_state = usbh_core_get_urb_state(core, core->ctrl_req.pipe_in);
		/* check is DATA packet transferred successfully */
		if (urb_state == USBH_URB_DONE) {
			core->ctrl_req.state = USBH_CTRL_STATUS_OUT;
			usb_os_queue_send(core->msg_queue, &host_msg, 0U);
		}

		/* manage error cases*/
		if (urb_state == USBH_URB_STALL) {
			/* In stall case, return to previous machine state*/
			status = HAL_ERR_PARA;
			usb_os_queue_send(core->msg_queue, &host_msg, 0U);
		} else {
			if (urb_state == USBH_URB_ERROR) {
				/* Device error */
				core->ctrl_req.state = USBH_CTRL_ERROR;
				usb_os_queue_send(core->msg_queue, &host_msg, 0U);
			}
		}
		break;

	case USBH_CTRL_STATUS_OUT:
		if (core->device.speed != USB_SPEED_HIGH) {
			do_ping = 0U;
		} else {
			do_ping = 1U;
		}

		usbh_hcd_hc_submit_request((usbh_hcd_t *)core->hcd,
								   core->ctrl_req.pipe_out,			/* Pipe index */
								   0U, 				                /* Direction : OUT */
								   USB_CH_EP_TYPE_CTRL,	            /* EP type */
								   USBH_TOKEN_DATA,		            /* Type Data */
								   0,				                /* Data buffer */
								   0,				                /* Data length */
								   do_ping);

		core->ctrl_req.state = USBH_CTRL_STATUS_OUT_BUSY;
		break;

	case USBH_CTRL_STATUS_OUT_BUSY:

		urb_state = usbh_core_get_urb_state(core, core->ctrl_req.pipe_out);
		if (urb_state == USBH_URB_DONE) {
			status = HAL_OK;
			core->ctrl_req.state = USBH_CTRL_COMPLETE;
			usb_os_queue_send(core->msg_queue, &host_msg, 0U);
		} else if (urb_state == USBH_URB_BUSY) {
			core->ctrl_req.state = USBH_CTRL_STATUS_OUT;
			usb_os_queue_send(core->msg_queue, &host_msg, 0U);
		} else {
			if (urb_state == USBH_URB_ERROR) {
				core->ctrl_req.state = USBH_CTRL_ERROR;
				usb_os_queue_send(core->msg_queue, &host_msg, 0U);
			}

		}
		break;

	case USBH_CTRL_ERROR:
		if (++core->ctrl_req.error_cnt <= USBH_MAX_ERROR_COUNT) {
			usbh_hcd_stop((usbh_hcd_t *)core->hcd);
			core->ctrl_req.state = USBH_CTRL_SETUP;
			core->ctrl_req_state = USBH_CTRL_REQ_SEND;
		} else {
			if ((host->cb != NULL) && (host->cb->process != NULL)) {
				host->cb->process(host, USBH_MSG_ERROR);
			}
			core->ctrl_req.error_cnt = 0U;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "CTRL req err\n");
			status = HAL_ERR_UNKNOWN;
		}
		break;

	default:
		break;
	}
	return status;
}
static int usbh_cts_xfer_test_process_rx(usb_host_t *host)
{
	usbh_cts_xfer_test_host_t *xfer_test = &usbh_cts_xfer_test_host;
	usbh_core_t *core = (usbh_core_t *)host->private;
	u32 host_msg = USBH_CONTROL_EVENT;
	usbh_setup_req_t setup;
	int ret;

	switch (xfer_test->data_rx_state) {
	case CTS_XFER_TEST_STATE_RX_START:

		setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_DEVICE;
		setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
		setup.b.wValue = 0x0100;
		setup.b.wIndex = 0U;
		setup.b.wLength = xfer_test->rx_len;
		core->ctrl_req.setup->d32[0] = setup.d32[0];
		core->ctrl_req.setup->d32[1] = setup.d32[1];

		/* Start a SETUP transfer */
		core->ctrl_req.buf = xfer_test->rx_buf;
		core->ctrl_req.length = setup.b.wLength;
		core->ctrl_req.state = USBH_CTRL_SETUP;
		core->ctrl_req_state = USBH_CTRL_REQ_BUSY;
		ret = HAL_BUSY;
		usb_os_queue_send(core->msg_queue, &host_msg, 0U);

		xfer_test->data_rx_state = CTS_XFER_TEST_STATE_RX_CONTINUE;
		break;

	case CTS_XFER_TEST_STATE_RX_CONTINUE:
		ret = usbh_cts_ctrl_process(core);
		if (ret == HAL_OK) {
			/* Commands successfully sent and Response Received  */
			core->ctrl_req_state = USBH_CTRL_REQ_SEND;
			core->ctrl_req.state = USBH_CTRL_IDLE;
			ret = HAL_OK;
		} else if (ret == HAL_ERR_PARA) {
			/* Commands successfully sent and Response Received  */
			core->ctrl_req_state = USBH_CTRL_REQ_SEND;
			core->ctrl_req.state = USBH_CTRL_IDLE;
			ret = HAL_ERR_PARA;
		} else {
			if (ret == HAL_ERR_UNKNOWN) {
				/* Failure Mode */
				core->ctrl_req_state = USBH_CTRL_REQ_SEND;
				ret = HAL_ERR_UNKNOWN;
			}
		}
		break;
	case CTS_XFER_TEST_STATE_ERROR:
		break;

	default:
		break;
	}

	return ret;
}
/**
  * @brief  wait usbh init success
  * @retval Status
  */
static int usbh_cts_xfer_test_enum_success(void)
{
	do {
		usb_os_sleep_ms(1000);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Wait USB enum finish\n");
	} while (0 == xfer_test_is_ready);

	return HAL_OK;
}
static int usbh_cts_xfer_test_receive(u8 *buf, u32 len)
{
	u8 ret = HAL_BUSY;
	usbh_cts_xfer_test_host_t *xfer_test = &usbh_cts_xfer_test_host;
	usb_host_t *host = xfer_test->host;

	//wait check enum
	usbh_cts_xfer_test_enum_success();

	if ((xfer_test->state == XFER_TEST_STATE_IDLE) || (xfer_test->state == XFER_TEST_STATE_TRANSFER)) {
		xfer_test->rx_buf = buf;
		xfer_test->rx_len = len;
		xfer_test->state = XFER_TEST_STATE_TRANSFER;
		xfer_test->data_rx_state = CTS_XFER_TEST_STATE_RX_START;
		usbh_notify_class_state_change(host, 0);
		ret = HAL_OK;
	}

	return ret;
}

static usbh_config_t usbh_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = FALSE,
	.main_task_priority = 8U,
	.isr_task_priority = 9U,
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
static int msc_cb_process(usb_host_t *host, u8 id)
{
	UNUSED(host);

	switch (id) {
	case USBH_MSG_DISCONNECTED:
		xfer_test_is_ready = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Host disconnected\n");
		break;
	case USBH_MSG_CONNECTED:
		RTK_LOGS(TAG, RTK_LOG_INFO, "Host connected\n");
		break;
	default:
		break;
	}

	return HAL_OK;
}

static usbh_user_cb_t usbh_usr_cb = {
	.process = msc_cb_process
};

static int usbh_cts_xfer_test_init(void)
{
	int status;

	status = usbh_init(&usbh_cfg, &usbh_usr_cb);
	if (status != HAL_OK) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Fail to init USB host controller: %d\n", status);
		return HAL_ERR_PARA;
	}

	usbh_register_class(&usbh_driver);

	return HAL_OK;
}

/**
  * @brief  Deinit Port Test class
  * @retval Status
  */
static int usbh_cts_xfer_test_deinit(void)
{
	usbh_unregister_class(&usbh_driver);
	usbh_deinit();

	return HAL_OK;
}
static void usbh_cts_xfer_test_start(u8 flag)
{
	cts_xfer_test_flag = flag;
}


/* Exported functions --------------------------------------------------------*/
/*
	usbh xfer step1
	usbh xfer step2
	usbh xfer deinit
*/
int cmd_usbh_xfer_test(u16 argc, u8  *argv[])
{
	u32 len = 8;
	u8 pbuf[10];
	const char *sub_cmd;

	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid arguments, usage:\n");
		RTK_LOGS(TAG, RTK_LOG_ERROR, "  usbh cts cmd\n");
		return HAL_ERR_PARA;
	}

	sub_cmd = (const char *)argv[1];

	/*
		1.init a class[prot test]，start a process
		2.start cmd 1 to enable cts_xfer_test_flag flag
		  start get description
		3.start cmd 2 to disable the flag
		4.deinit the class
	*/
	if (_stricmp(sub_cmd, "step1") == 0) {
		usbh_cts_xfer_test_init();
		usbh_cts_xfer_test_enum_success();

		usbh_cts_xfer_test_start(1);
		usbh_cts_xfer_test_receive(pbuf, len);
	} else if (_stricmp(sub_cmd, "step2") == 0) {
		usbh_cts_xfer_test_start(0);
	} else if (_stricmp(sub_cmd, "deinit") == 0) {
		usbh_cts_xfer_test_deinit();
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid test cmd\n");
	}

	return HAL_OK;
}

