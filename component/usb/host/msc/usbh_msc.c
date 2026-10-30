/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_msc.h"
#include "usbh_msc_scsi.h"

/* Private defines -----------------------------------------------------------*/

#define USBH_MSC_DEBUG 0
#define MSC_XFER_MAX_TIMEOUT_TICK            100  //sof
#define MSC_CMD_TIMEOUT_PER_BLOCK_MS         10000U  /**< READ/WRITE budget per block before the caller abandons it */
#define MSC_INIT_STALL_DIAG_MS               5000U   /**< LUN init without progress for this long gets one diagnostic line */
#define MSC_LUN_INIT_TIMEOUT_MS              10000U  /**< Bound of one LUN's INQUIRY / TEST UNIT READY / READ CAPACITY sequence */

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbh_msc_attach(usb_host_t *host);
static void usbh_msc_detach(usb_host_t *host);
static void usbh_msc_process(usb_host_t *host, usbh_drv_msg_t *msg);
static int usbh_msc_setup(usb_host_t *host);
static void usbh_msc_sof(usb_host_t *host);
static void usbh_msc_recover_init(usb_host_t *host, usbh_msc_host_t *msc, usbh_msc_lun_t *unit);
static void usbh_msc_init_failed(usb_host_t *host, usbh_msc_host_t *msc, usbh_msc_lun_t *unit);
static int usbh_msc_process_rw(u8 lun);
static u32 usbh_msc_chunk_blocks(const usbh_msc_host_t *msc, u8 lun);
static void usbh_msc_reopen_pipe(usb_host_t *host, usbh_pipe_t *pipe);
static void usbh_msc_finish_cmd(usbh_msc_host_t *msc, int result);
static u32 usbh_msc_fsm_snapshot(const usbh_msc_host_t *msc);
static void usbh_msc_process_abort(usb_host_t *host, usbh_msc_host_t *msc);
static int usbh_msc_clear_halt_failed(usbh_msc_host_t *msc);
static void usbh_msc_start_failed(usb_host_t *host, usbh_msc_host_t *msc, const usbh_pipe_t *pipe);
static int usbh_msc_xfer(u8 lun, u32 address, u8 *rx_buf, const u8 *tx_buf, u32 length, u8 cmd);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "MSC";

/* USB MSC device identification */
static const usbh_dev_id_t msc_devs[] = {
	{
		.mMatchFlags = USBH_DEV_ID_MATCH_ITF_CLASS,
		.bInterfaceClass = MSC_CLASS_CODE,
	},
	{
	},
};

/* USB Host MSC class driver */
static const usbh_class_driver_t usbh_msc_driver = {
	.id_table = msc_devs,
	.attach = usbh_msc_attach,
	.detach = usbh_msc_detach,
	.setup = usbh_msc_setup,
	.process = usbh_msc_process,
	.sof = usbh_msc_sof,
};

static usbh_msc_host_t usbh_msc_host;

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Attach callback.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_msc_attach(usb_host_t *host)
{
	int status = RTK_FAIL;
	usbh_itf_data_t *itf_data;
	usbh_ep_desc_t *ep_desc;
	usbh_msc_host_t *msc = &usbh_msc_host;
	usb_msc_bot_cbw_t *cbw = msc->hbot.cbw;
	usbh_itf_desc_t *msc_itf_desc;
	usbh_pipe_t *bulk_out = &msc->bulk_out;
	usbh_pipe_t *bulk_in = &msc->bulk_in;
	usbh_dev_id_t dev_id = {0,};

	dev_id.bInterfaceClass = MSC_CLASS_CODE;
	dev_id.bInterfaceSubClass = USBH_MSC_TRANSPARENT;
	dev_id.bInterfaceProtocol = USBH_MSC_BOT;
	dev_id.mMatchFlags = USBH_DEV_ID_MATCH_ITF_INFO;
	itf_data = usbh_get_interface_descriptor(host, &dev_id);

	if (itf_data == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Get itf desc fail\n");
	} else {
		msc_itf_desc = itf_data->itf_desc_array;
		msc->host = host;

		/* Set data in/out endpoints. ep_desc_array holds exactly bNumEndpoints
		   entries, so bound the loop by bNumEndpoints to avoid an out-of-bounds read. */
		for (int i = 0; i < msc_itf_desc->bNumEndpoints && i < 2; i++) {
			ep_desc = &msc_itf_desc->ep_desc_array[i];
			if ((ep_desc->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) {
				if (usbh_open_pipe(host, bulk_in, ep_desc, &usbh_msc_driver) != RTK_SUCCESS) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk in pipe fail\n");
					goto open_fail;
				}
				bulk_in->max_timeout_tick = MSC_XFER_MAX_TIMEOUT_TICK;
			} else {
				if (usbh_open_pipe(host, bulk_out, ep_desc, &usbh_msc_driver) != RTK_SUCCESS) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Open bulk out pipe fail\n");
					goto open_fail;
				}
				bulk_out->max_timeout_tick = MSC_XFER_MAX_TIMEOUT_TICK;
			}
		}

		/* BOT mandates both a bulk-in and a bulk-out endpoint */
		if ((bulk_in->pipe_num == 0) || (bulk_out->pipe_num == 0)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Missing bulk pipe\n");
			goto open_fail;
		}

		msc->itf_num = msc_itf_desc->bInterfaceNumber;
		msc->current_lun = 0U;
		msc->state = MSC_INIT;
		msc->sof_tick = usbh_get_tick(host);
		msc->lun_ms = usb_os_get_timestamp_ms();
		msc->init_ms = msc->lun_ms;
		msc->init_diag_done = 0U;
		msc->init_recovery = 0U;
		msc->req_state = MSC_REQ_IDLE;

		cbw->field.dCBWSignature = USB_MSC_CBW_SIGN;
		msc->hbot.tag_counter = USB_BOT_CBW_TAG;
		cbw->field.dCBWTag = msc->hbot.tag_counter;
		msc->hbot.state = BOT_SEND_CBW;
		msc->bulk_out.xfer_state = USBH_EP_XFER_START;
		msc->hbot.cmd_state = BOT_CMD_SEND;
		msc->hbot.reset_recovery = 0U;

		/* De-Initialize LUNs information */
		usb_os_memset((void *)msc->unit, 0, sizeof(msc->unit));

		if ((msc->cb != NULL) && (msc->cb->attached != NULL)) {
			msc->cb->attached();
		}

		status = RTK_SUCCESS;
	}
	return status;

open_fail:
	/* Roll back any pipe already opened (guard with pipe_num: only close open pipes) */
	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}
	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}
	return status;
}

/**
  * @brief  Detach callback.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_msc_detach(usb_host_t *host)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	usbh_pipe_t *bulk_out = &msc->bulk_out;
	usbh_pipe_t *bulk_in = &msc->bulk_in;

	if ((msc->cb != NULL) && (msc->cb->detached != NULL)) {
		msc->cb->detached();
	}

	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}

	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}

	/* The pipes are halted above, so no DMA still targets the bounce buffer, which is kept
	   for the next attach. Dropping the host handle in the critical section that also guards
	   publishing a READ/WRITE, see usbh_msc_xfer(), orders the two: a command published
	   before is ended just below, a later one is refused, so none is left with no process()
	   to run it. */
	usb_os_enter_critical();
	msc->host = NULL;
	usb_os_exit_critical();

	/* A READ/WRITE in flight will never complete now: end it so its caller returns at once
	   instead of waiting out its whole timeout. detach() runs in the host task, the same
	   context as process(), so it cannot race the command it ends. */
	usbh_msc_finish_cmd(msc, -RTK_ERR_IO);
}

/**
  * @brief  Standard control requests handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_msc_setup(usb_host_t *host)
{
	int status = -RTK_ERR_BUSY;
	usbh_setup_req_t setup;
	usbh_msc_host_t *msc = &usbh_msc_host;
	u8 i;

	/* Switch MSC REQ state machine */
	switch (msc->req_state) {
	case MSC_REQ_IDLE:
	case MSC_REQ_GET_MAX_LUN:
		/* Issue GetMaxLUN request */
		setup.req.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS
								  | USB_REQ_RECIPIENT_INTERFACE;
		setup.req.bRequest = USB_MSC_REQUEST_GET_MAX_LUN;
		setup.req.wValue = 0U;
		setup.req.wIndex = msc->itf_num;
		setup.req.wLength = 1U;
		status = usbh_ctrl_request(host, &setup, msc->max_lun_buf);
		/* When devices do not support the GetMaxLun request, this should
		   be considered as only one logical unit is supported */
		if (status == -RTK_ERR_BADARG) {
			msc->max_lun = 0U;
			status = RTK_SUCCESS;
		}

		if (status == RTK_SUCCESS) {
			msc->max_lun = *(msc->max_lun_buf);
			/* GetMaxLUN returns the highest 0-based LUN index, so the LUN count is
			   bMaxLUN + 1. Clamp the count to the unit[] capacity. Using >= is
			   required: at bMaxLUN == USBH_MSC_MAX_LUN, bMaxLUN + 1 would overflow
			   the array. This evaluates to min(bMaxLUN + 1, USBH_MSC_MAX_LUN). */
			msc->max_lun = (msc->max_lun >= USBH_MSC_MAX_LUN) ? USBH_MSC_MAX_LUN : (msc->max_lun + 1U);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Max lun %d\n", msc->max_lun);

			for (i = 0U; i < msc->max_lun; i++) {
				msc->unit[i].prev_ready = 0U;
				msc->unit[i].state_changed = 0U;
			}
		}
		break;

	case MSC_REQ_ERROR :
		/* a Clear Feature should be issued here */
		if (usbh_ctrl_clear_feature(host, 0x00U) != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TX clear feature fail\n");
		}
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  State machine handling callback
  * @param  host: Host handle
  * @param  msg: USB host driver message
  * @retval None
  */
static void usbh_msc_process(usb_host_t *host, usbh_drv_msg_t *msg)
{
	UNUSED(msg);

	usbh_msc_host_t *msc = &usbh_msc_host;
	int scsi_status = -RTK_ERR_BUSY;
	u32 snap;

	switch (msc->state) {
	case MSC_INIT:
		if (msc->current_lun < msc->max_lun) {
			usbh_msc_lun_t *unit = &msc->unit[msc->current_lun];

			/* Diagnostic only: name the BOT step a stuck LUN init is parked in, once per init. */
			if ((msc->init_diag_done == 0U) && ((u32)(usb_os_get_timestamp_ms() - msc->init_ms) > MSC_INIT_STALL_DIAG_MS)) {
				msc->init_diag_done = 1U;
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Lun init stall, unit %d, bot %d/%d, in %d/%d, out %d/%d\n", unit->state, msc->hbot.state,
						 msc->hbot.cmd_state, msc->bulk_in.xfer_state, usbh_get_urb_state(host, &msc->bulk_in),
						 msc->bulk_out.xfer_state, usbh_get_urb_state(host, &msc->bulk_out));
			}

			/* A device that keeps NAKing or failing one of these commands must not hold the
			   attach forever: escalate the recovery, see usbh_msc_recover_init(). */
			if ((u32)(usb_os_get_timestamp_ms() - msc->lun_ms) > MSC_LUN_INIT_TIMEOUT_MS) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Lun %d init timeout, unit %d\n", msc->current_lun, unit->state);
				usbh_msc_recover_init(host, msc, unit);
				break;
			}

			snap = usbh_msc_fsm_snapshot(msc);
			unit->error = MSC_NOT_READY;
			/* Switch MSC REQ state machine */
			switch (unit->state) {
			case MSC_INIT:
				RTK_LOGS(TAG, RTK_LOG_INFO, "Lun %d\n", msc->current_lun);
				unit->state = MSC_READ_INQUIRY;
				break;

			case MSC_READ_INQUIRY:
				scsi_status = usbh_scsi_inquiry(msc, (u8)msc->current_lun, &unit->inquiry);

				if (scsi_status == RTK_SUCCESS) {
#if USBH_MSC_DEBUG
					RTK_LOGS(TAG, RTK_LOG_INFO, "Inq ven %s\n", unit->inquiry.vendor_id);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Inq prod %s\n", unit->inquiry.product_id);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Inq ver %s\n", unit->inquiry.revision_id);
#endif
					unit->state = MSC_TEST_UNIT_READY;
				}
				if (scsi_status == RTK_FAIL) {
					unit->state = MSC_REQUEST_SENSE;
				} else {
					if (scsi_status == -RTK_ERR_IO) {
						usbh_msc_init_failed(host, msc, unit);
					}
				}
				break;

			case MSC_TEST_UNIT_READY:
				scsi_status = usbh_scsi_test_unit_ready(msc, (u8)msc->current_lun);

				if (scsi_status == RTK_SUCCESS) {
					if (unit->prev_ready == 0U) {
						unit->state_changed = 1U;
						//RTK_LOGS(TAG, RTK_LOG_INFO, "Device ready\n");
					} else {
						unit->state_changed = 0U;
					}
					unit->state = MSC_READ_CAPACITY10;
					unit->error = MSC_OK;
					unit->prev_ready = 1U;
				}
				if (scsi_status == RTK_FAIL) {
					/* Media not ready, so try to check again during 10s */
					if (unit->prev_ready != 0U) {
						unit->state_changed = 1U;
						RTK_LOGS(TAG, RTK_LOG_WARN, "Device not ready\n");
					} else {
						unit->state_changed = 0U;
					}
					unit->state = MSC_REQUEST_SENSE;
					unit->error = MSC_NOT_READY;
					unit->prev_ready = 0U;
				} else {
					if (scsi_status == -RTK_ERR_IO) {
						usbh_msc_init_failed(host, msc, unit);
					}
				}
				break;

			case MSC_READ_CAPACITY10:
				scsi_status = usbh_scsi_read_capacity(msc, (u8)msc->current_lun, &unit->capacity);

				if (scsi_status == RTK_SUCCESS) {
					if (unit->state_changed == 1U) {
#if USBH_MSC_DEBUG
						RTK_LOGS(TAG, RTK_LOG_INFO, "Capacity %dB\n",
								 (unit->capacity.block_nbr * unit->capacity.block_size));
						RTK_LOGS(TAG, RTK_LOG_INFO, "Block num %d\n", unit->capacity.block_nbr);
						RTK_LOGS(TAG, RTK_LOG_INFO, "Block size %d\n", unit->capacity.block_size);
#endif
					}
					unit->state = MSC_IDLE;
					unit->error = MSC_OK;
					msc->current_lun++;
				} else if (scsi_status == RTK_FAIL) {
					unit->state = MSC_REQUEST_SENSE;
				} else if (scsi_status == -RTK_ERR_BADARG) {
					/* The medium geometry is unusable, so no legal READ10/WRITE10 CBW can ever
					   be built for this LUN. Retire it here, which keeps it out of
					   usbh_msc_unit_is_ready(), instead of re-issuing the same command. */
					unit->error = MSC_ERROR;
					unit->state = MSC_UNRECOVERED_ERROR;
				} else {
					if (scsi_status == -RTK_ERR_IO) {
						usbh_msc_init_failed(host, msc, unit);
					}
				}
				break;

			case MSC_REQUEST_SENSE:
				scsi_status = usbh_scsi_request_sense(msc, (u8)msc->current_lun, &unit->sense);

				if (scsi_status == RTK_SUCCESS) {
					if ((unit->sense.key == SCSI_SENSE_KEY_UNIT_ATTENTION) ||
						(unit->sense.key == SCSI_SENSE_KEY_NOT_READY)) {

						/* Keep polling a becoming-ready medium; the LUN budget above ends it. */
						if ((u32)(usb_os_get_timestamp_ms() - msc->lun_ms) <= MSC_LUN_INIT_TIMEOUT_MS) {
							unit->state = MSC_TEST_UNIT_READY;
							break;
						}
					}

#if USBH_MSC_DEBUG
					RTK_LOGS(TAG, RTK_LOG_INFO, "Sense key %x\n", unit->sense.key);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Sense code %x\n", unit->sense.asc);
					RTK_LOGS(TAG, RTK_LOG_INFO, "Sense code qua %x\n", unit->sense.ascq);
#endif
					unit->state = MSC_IDLE;
					msc->current_lun++;
				}
				if (scsi_status == RTK_FAIL) {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Device not ready\n");
					unit->state = MSC_UNRECOVERED_ERROR;
				} else {
					if (scsi_status == -RTK_ERR_IO) {
						usbh_msc_init_failed(host, msc, unit);
					}
				}
				break;

			case MSC_UNRECOVERED_ERROR:
				msc->current_lun++;
				break;

			default:
				break;
			}

			/* As for READ/WRITE, re-run only after a step moved the FSM: a BUSY step waiting on
			   a transfer is woken by its completion or by the SOF tick, not by spinning here. */
			if ((scsi_status != -RTK_ERR_BUSY) || (usbh_msc_fsm_snapshot(msc) != snap) || (msc->current_lun >= msc->max_lun)) {
				if (msc->current_lun < msc->max_lun) {
					/* A new LUN starts its own budget. */
					msc->lun_ms = usb_os_get_timestamp_ms();
				}
				usbh_notify(host, 0, &usbh_msc_driver);
			}
		} else {
			msc->current_lun = 0U;
			msc->state = MSC_IDLE;

			usbh_notify(host, 0, &usbh_msc_driver);
			if ((msc->cb != NULL) && (msc->cb->setup != NULL)) {
				msc->cb->setup();
			}
		}
		break;

	case MSC_IDLE:
		break;

	case MSC_READ:
	case MSC_WRITE:
		/* The BOT FSM is advanced only here, in the host task: the bulk URB completions and
		   the EP0 completions of the recovery requests are routed back to this driver, and
		   only this context may issue an EP0 request once the class is ready. */
		if ((msc->bulk_in.pipe_num == 0U) || (msc->bulk_out.pipe_num == 0U)) {
			/* A failed reopen left no bulk pipe: pipe 0 is EP0, so nothing may be sent. */
			usbh_msc_finish_cmd(msc, -RTK_ERR_IO);
			break;
		}
		snap = usbh_msc_fsm_snapshot(msc);
		scsi_status = usbh_msc_process_rw(msc->current_lun);
		if (scsi_status != -RTK_ERR_BUSY) {
			usbh_msc_finish_cmd(msc, scsi_status);
		} else if (usbh_msc_fsm_snapshot(msc) != snap) {
			/* Re-run only after a step really moved the FSM: an unchanged BUSY step is waiting
			   for a completion, which notifies on its own, or for the SOF timeout tick. */
			usbh_notify(host, 0, &usbh_msc_driver);
		} else {
			/* waiting for an event */
		}
		break;

	case MSC_ABORT:
		usbh_msc_process_abort(host, msc);
		break;

	default:
		break;
	}
}

/**
  * @brief  Escalate the recovery of a LUN init that ran out of its budget.
  * @note   Ref BOT 5.3.4: first a Bulk-Only Mass Storage Reset with both halts cleared, after
  *         which the SCSI layer re-issues the very command that stalled. If that does not help
  *         either, the device is reset and enumerated again (usbh_reenumerate()), which brings
  *         this driver back through detach() and attach(). Only when the core refuses that
  *         too is the LUN retired, so usbh_msc_unit_is_ready() reports it unusable and setup()
  *         still runs.
  * @param  host: Host handle
  * @param  msc: Msc host handle
  * @param  unit: LUN whose init timed out
  * @retval None
  */
static void usbh_msc_recover_init(usb_host_t *host, usbh_msc_host_t *msc, usbh_msc_lun_t *unit)
{
	int ret;

	/* Halt both bulk channels first, so no stale transfer of the stalled command completes
	   into what follows. */
	usbh_msc_reopen_pipe(host, &msc->bulk_in);
	usbh_msc_reopen_pipe(host, &msc->bulk_out);
	msc->lun_ms = usb_os_get_timestamp_ms();

	if (msc->init_recovery == 0U) {
		msc->init_recovery = 1U;
		/* The SCSI step stays at BOT_CMD_BUSY, so it resumes from BOT_SEND_CBW with the
		   same CBW once the recovery is over. */
		msc->hbot.reset_recovery = 0U;
		msc->hbot.state = BOT_UNRECOVERED_ERROR;
		msc->hbot.cmd_state = BOT_CMD_BUSY;
		usbh_notify(host, 0, &usbh_msc_driver);
		return;
	}

	msc->hbot.reset_recovery = 0U;
	msc->hbot.state = BOT_SEND_CBW;
	msc->hbot.cmd_state = BOT_CMD_SEND;
	msc->bulk_out.xfer_state = USBH_EP_XFER_START;

	ret = usbh_reenumerate();
	if ((ret == RTK_SUCCESS) || (ret == -RTK_ERR_BUSY)) {
		/* detach() comes next from the host task; nothing is driven until then. */
		msc->state = MSC_IDLE;
		return;
	}

	unit->error = MSC_ERROR;
	unit->retired = 1U;
	unit->state = MSC_IDLE;
	msc->current_lun++;
	usbh_notify(host, 0, &usbh_msc_driver);
}

/**
  * @brief  Handle a LUN init command that failed for good.
  * @note   A SCSI step reports -RTK_ERR_IO only when BOT Reset Recovery itself failed, so that
  *         step of usbh_msc_recover_init() is spent: go on to the next one at once instead of
  *         parking the unit until the LUN budget runs out.
  * @param  host: Host handle
  * @param  msc: Msc host handle
  * @param  unit: LUN whose init failed
  * @retval None
  */
static void usbh_msc_init_failed(usb_host_t *host, usbh_msc_host_t *msc, usbh_msc_lun_t *unit)
{
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Lun %d init fail, unit %d\n", msc->current_lun, unit->state);
	msc->init_recovery = 1U;
	usbh_msc_recover_init(host, msc, unit);
}

/**
  * @brief  Pack the FSM fields one READ/WRITE step can change, to tell progress from waiting.
  * @param  msc: Msc host handle
  * @retval Snapshot value
  */
static u32 usbh_msc_fsm_snapshot(const usbh_msc_host_t *msc)
{
	return ((u32)msc->hbot.state) | ((u32)msc->hbot.cmd_state << 8U) | ((u32)msc->unit[msc->current_lun].state << 16U) |
		   ((u32)msc->bulk_in.xfer_state << 24U) | ((u32)msc->bulk_out.xfer_state << 28U);
}

/**
  * @brief  Abandon the in-flight READ/WRITE through BOT Reset Recovery.
  * @note   Ref BOT 5.3.4: Bulk-Only Mass Storage Reset, then CLEAR_FEATURE(ENDPOINT_HALT) on
  *         Bulk-In and then on Bulk-Out. The first pass halts both bulk channels, so no DMA is
  *         left on the bounce buffer, and releases the caller at once: the recovery runs on EP0 only, which has no
  *         timeout in the host core, so a device that keeps NAKing it must not keep the caller
  *         waiting. The recovery then goes on in the background, with req_state = MSC_REQ_RESET
  *         marking the caller as released and msc->state left at MSC_ABORT, so a new command is
  *         refused until the device is back in a known state.
  *         An EP0 request of the abandoned command may still be in flight, and EP0 carries one
  *         request at a time, so that request is driven to its end first.
  * @param  host: Host handle
  * @param  msc: Msc host handle
  * @retval None
  */
static void usbh_msc_process_abort(usb_host_t *host, usbh_msc_host_t *msc)
{
	u8 bot_state = msc->hbot.state;
	u8 release = 0U;
	int status;

	if (msc->req_state != MSC_REQ_RESET) {
		/* Halt both bulk channels: from here on no DMA can touch the bounce buffer, and no READ
		   data is copied to the caller, so it may return. Their toggles are reset as well, which
		   matches the device once the recovery below clears both halts. */
		usbh_msc_reopen_pipe(host, &msc->bulk_in);
		usbh_msc_reopen_pipe(host, &msc->bulk_out);

		usb_os_enter_critical();
		if (msc->state == MSC_ABORT) {
			msc->hbot.cmd_result = -RTK_ERR_TIMEOUT;
			msc->req_state = MSC_REQ_RESET;
			release = 1U;
		}
		usb_os_exit_critical();

		if (release != 0U) {
			usb_os_sema_give(msc->hbot.cmd_done_sema);
		}

		/* No EP0 request of the abandoned command pending: start the full recovery now. */
		if ((bot_state != BOT_ERROR_IN) && (bot_state != BOT_ERROR_OUT) && (bot_state != BOT_UNRECOVERED_ERROR)) {
			msc->hbot.reset_recovery = 0U;
			msc->hbot.state = BOT_UNRECOVERED_ERROR;
		}
		usbh_notify(host, 0, &usbh_msc_driver);
		return;
	}

	status = usbh_msc_bot_process(host, msc->current_lun);

	if (msc->hbot.state == BOT_SEND_CBW) {
		/* Full recovery over: either both halts cleared, or the device refused it for good. */
		RTK_LOGS(TAG, RTK_LOG_WARN, "Abort end %d\n", status);
		msc->hbot.reset_recovery = 0U;
		msc->hbot.cmd_state = BOT_CMD_SEND;
		msc->bulk_out.xfer_state = USBH_EP_XFER_START;
		usbh_msc_finish_cmd(msc, -RTK_ERR_TIMEOUT);
		if (status == -RTK_ERR_IO) {
			/* BOT 5.3.4 leaves no further recovery than a port reset: have the core reset and
			   re-enumerate the device. Only if it refuses is the LUN retired, so every later
			   command is refused rather than sent to a device in an unknown state. Re-attach
			   clears it. */
			status = usbh_reenumerate();
			if ((status != RTK_SUCCESS) && (status != -RTK_ERR_BUSY)) {
				msc->unit[msc->current_lun].error = MSC_ERROR;
				msc->unit[msc->current_lun].retired = 1U;
			}
		}
	} else if ((msc->hbot.state != BOT_ERROR_IN) && (msc->hbot.state != BOT_ERROR_OUT) &&
			   (msc->hbot.state != BOT_UNRECOVERED_ERROR)) {
		/* A partial recovery of the abandoned command ended (it reached the CSW phase): this
		   abort still needs the full one. */
		msc->hbot.reset_recovery = 0U;
		msc->hbot.state = BOT_UNRECOVERED_ERROR;
		usbh_notify(host, 0, &usbh_msc_driver);
	} else if (msc->hbot.state != bot_state) {
		usbh_notify(host, 0, &usbh_msc_driver);
	} else {
		/* EP0 request in flight: its completion, or the SOF tick, runs this again */
	}
}

/**
  * @brief  SOF callback, the timeout source of an in-flight READ/WRITE.
  * @note   Runs in ISR context. A device that keeps NAKing produces no completion event, and
  *         the core watchdog does not reach class drivers, so without this wakeup nothing would
  *         re-run the BOT FSM to retire the stuck transfer. It only ticks while a command is in
  *         flight, once per bulk timeout period.
  * @param  host: Host handle
  * @retval None
  */
static void usbh_msc_sof(usb_host_t *host)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	u8 state = msc->state;

	/* MSC_INIT too: LUN init now waits for events as READ/WRITE do, and needs the same tick
	   to retire a NAK run and to reach its own timeout. sof_tick is private to this wakeup,
	   so the LUN budget kept in msc->lun_ms is not moved by it. */
	if ((state == MSC_INIT) || (state == MSC_READ) || (state == MSC_WRITE) || (state == MSC_ABORT)) {
		if (usbh_get_elapsed_ticks(host, msc->sof_tick) >= MSC_XFER_MAX_TIMEOUT_TICK) {
			msc->sof_tick = usbh_get_tick(host);
			usbh_notify(host, 0, &usbh_msc_driver);
		}
	}
}

/**
  * @brief  End the in-flight READ/WRITE and wake its caller.
  * @note   Runs in the host task. Only a command still owned by the BOT FSM is ended, so the
  *         completion semaphore is given exactly once per command: not again for an abort whose
  *         caller was already released (req_state == MSC_REQ_RESET). A command that completed
  *         while its caller was abandoning it reports its real result, since its data was moved.
  * @param  msc: Msc host handle
  * @param  result: Final RTK status of the command
  * @retval None
  */
static void usbh_msc_finish_cmd(usbh_msc_host_t *msc, int result)
{
	u8 pending = 0U;

	usb_os_enter_critical();
	if ((msc->state == MSC_READ) || (msc->state == MSC_WRITE) || (msc->state == MSC_ABORT)) {
		if ((msc->state != MSC_ABORT) || (msc->req_state != MSC_REQ_RESET)) {
			msc->hbot.cmd_result = result;
			pending = 1U;
		}
		/* The per-unit FSM may have parked in an error state; the next command starts afresh. */
		msc->unit[msc->current_lun].state = MSC_IDLE;
		msc->req_state = MSC_REQ_IDLE;
		msc->state = MSC_IDLE;
	}
	usb_os_exit_critical();

	if (pending != 0U) {
		usb_os_sema_give(msc->hbot.cmd_done_sema);
	}
}

/**
  * @brief  Close and reopen one bulk pipe.
  * @note   Closing halts the channel, so no DMA is left targeting the bounce buffer. Reopening
  *         resets the host-side data toggle to DATA0, which has to follow every
  *         CLEAR_FEATURE(ENDPOINT_HALT) of that endpoint, since the device resets its own toggle
  *         then (Ref USB 2.0 9.4.5). An endpoint that was not cleared must keep its toggle.
  *         If the reopen fails, pipe_num stays 0 and the next command is refused.
  * @param  host: Host handle
  * @param  pipe: Bulk pipe to reopen
  * @retval None
  */
static void usbh_msc_reopen_pipe(usb_host_t *host, usbh_pipe_t *pipe)
{
	usbh_ep_desc_t ep_desc = {0};
	u8 old_pipe = pipe->pipe_num;

	if (pipe->pipe_num == 0U) {
		return;
	}

	ep_desc.bLength = USB_LEN_EP_DESC;
	ep_desc.bDescriptorType = USB_DESC_TYPE_ENDPOINT;
	ep_desc.bEndpointAddress = pipe->ep_addr;
	ep_desc.bmAttributes = USB_CH_EP_TYPE_BULK;
	ep_desc.wMaxPacketSize = pipe->ep_mps;

	usbh_close_pipe(host, pipe);
	if (usbh_open_pipe(host, pipe, &ep_desc, &usbh_msc_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Reopen EP%02x ch%d fail\n", ep_desc.bEndpointAddress, old_pipe);
	} else {
		pipe->max_timeout_tick = MSC_XFER_MAX_TIMEOUT_TICK;
		RTK_LOGS(TAG, RTK_LOG_WARN, "Reopen EP%02x ch%d->%d\n", ep_desc.bEndpointAddress, old_pipe, pipe->pipe_num);
	}
}

/**
  * @brief  Size of the next chunk of the in-flight READ/WRITE.
  * @note   A READ/WRITE is moved as consecutive READ(10)/WRITE(10) commands of at most one
  *         bounce buffer each, which also keeps the 16-bit TRANSFER LENGTH field and
  *         dCBWDataTransferLength exact. A Reset Recovery retry re-issues the same chunk.
  * @param  msc: Msc host handle
  * @param  lun: logical Unit Number
  * @retval Blocks in the chunk, 0 for an unusable block size, which the SCSI layer refuses
  */
static u32 usbh_msc_chunk_blocks(const usbh_msc_host_t *msc, u8 lun)
{
	u32 block_size = msc->unit[lun].capacity.block_size;
	u32 remain = msc->hbot.cmd_block_count - msc->hbot.cmd_done_count;
	u32 max_blocks;

	if (block_size == 0U) {
		return 0U;
	}

	max_blocks = msc->hbot.bounce_len / block_size;
	return (remain < max_blocks) ? remain : max_blocks;
}

/**
  * @brief  Transfer state machine handling
  * @param  lun: logical Unit Number
  * @retval Status
*/
static int usbh_msc_process_rw(u8 lun)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	usbh_msc_lun_t *unit = &msc->unit[lun];
	int status = -RTK_ERR_BUSY;
	int scsi_status = -RTK_ERR_BUSY;
	u32 chunk = usbh_msc_chunk_blocks(msc, lun);
	/* Below cmd_block_count * block_size, which usbh_msc_xfer() keeps in u32 */
	u32 offset = msc->hbot.cmd_done_count * unit->capacity.block_size;

	/* Switch MSC REQ state machine */
	switch (unit->state) {

	case MSC_READ:
		/* BOT §6.3.3: Reset Recovery rewinds the BOT FSM to BOT_CMD_SEND, so this call may have
		   to rebuild the very same CBW. Always pass the parameters of the current chunk so it is
		   re-issued unchanged instead of degenerating into a zero-length one. */
		scsi_status = usbh_scsi_read(msc, lun, msc->hbot.cmd_address + msc->hbot.cmd_done_count, &msc->hbot.cmd_rx_buf[offset], chunk);

		if (scsi_status == RTK_SUCCESS) {
			msc->hbot.cmd_done_count += chunk;
			if (msc->hbot.cmd_done_count >= msc->hbot.cmd_block_count) {
				unit->state = MSC_IDLE;
				unit->error = MSC_OK;
				status = RTK_SUCCESS;
			}
			/* else the BOT FSM is back at BOT_CMD_SEND and the next call sends the next chunk */
		} else if (scsi_status == RTK_FAIL) {
			unit->state = MSC_REQUEST_SENSE;
		} else if (scsi_status == -RTK_ERR_BADARG) {
			/* Unusable block size, so dCBWDataTransferLength cannot be computed and no
			   CBW was ever sent. BOT 6.7 has no case for a zero-length READ10, hence the
			   host must reject it locally. Report the error instead of returning -RTK_ERR_BUSY
			   forever, which would only stall the caller until its own timeout. */
			unit->state = MSC_IDLE;
			unit->error = MSC_ERROR;
			status = RTK_FAIL;
		} else {
			if (scsi_status == -RTK_ERR_IO) {
				unit->state = MSC_UNRECOVERED_ERROR;
				status = RTK_FAIL;
			}
		}

		break;

	case MSC_WRITE:
		/* Same chunking and Reset Recovery retry path as MSC_READ, see the comment above. */
		scsi_status = usbh_scsi_write(msc, lun, msc->hbot.cmd_address + msc->hbot.cmd_done_count, &msc->hbot.cmd_tx_buf[offset], chunk);

		if (scsi_status == RTK_SUCCESS) {
			msc->hbot.cmd_done_count += chunk;
			if (msc->hbot.cmd_done_count >= msc->hbot.cmd_block_count) {
				unit->state = MSC_IDLE;
				unit->error = MSC_OK;
				status = RTK_SUCCESS;
			}
		} else if (scsi_status == RTK_FAIL) {
			unit->state = MSC_REQUEST_SENSE;
		} else if (scsi_status == -RTK_ERR_BADARG) {
			/* Unusable block size, so dCBWDataTransferLength cannot be computed and no
			   CBW was ever sent. BOT 6.7 has no case for a zero-length WRITE10, hence the
			   host must reject it locally. Report the error instead of returning -RTK_ERR_BUSY
			   forever, which would only stall the caller until its own timeout. */
			unit->state = MSC_IDLE;
			unit->error = MSC_ERROR;
			status = RTK_FAIL;
		} else {
			if (scsi_status == -RTK_ERR_IO) {
				unit->state = MSC_UNRECOVERED_ERROR;
				status = RTK_FAIL;
			}
		}

		break;

	case MSC_REQUEST_SENSE:
		scsi_status = usbh_scsi_request_sense(msc, lun, &unit->sense);

		if (scsi_status == RTK_SUCCESS) {
#if USBH_MSC_DEBUG
			RTK_LOGS(TAG, RTK_LOG_INFO, "Sense key: %x\n", unit->sense.key);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Sense code: %x\n", unit->sense.asc);
			RTK_LOGS(TAG, RTK_LOG_INFO, "Sense code qua: %x\n", unit->sense.ascq);
#endif
			unit->state = MSC_IDLE;
			unit->error = MSC_ERROR;

			status = RTK_FAIL;
		}
		if (scsi_status == RTK_FAIL) {
			RTK_LOGS(TAG, RTK_LOG_WARN, "Device not ready\n");
		} else {
			if (scsi_status == -RTK_ERR_IO) {
				unit->state = MSC_UNRECOVERED_ERROR;
				status = RTK_FAIL;
			}
		}

		break;

	default:
		break;

	}
	return status;
}

/**
  * @brief  Escalate a failed CLEAR_FEATURE(ENDPOINT_HALT).
  * @note   The control engine reports a STALL or a spent retry budget, so re-issuing the same
  *         request would not succeed. Outside Reset Recovery, escalate to it (BOT 5.3.4); inside
  *         it, the device cannot be recovered, so end the command instead of looping.
  * @param  msc: Msc host handle
  * @retval -RTK_ERR_BUSY when Reset Recovery starts, -RTK_ERR_IO when the command is given up
  */
static int usbh_msc_clear_halt_failed(usbh_msc_host_t *msc)
{
	if (msc->hbot.reset_recovery == 0U) {
		msc->hbot.state = BOT_UNRECOVERED_ERROR;
		return -RTK_ERR_BUSY;
	}

	RTK_LOGS(TAG, RTK_LOG_ERROR, "Reset recovery fail\n");
	msc->hbot.reset_recovery = 0U;
	msc->hbot.state = BOT_SEND_CBW;
	msc->hbot.cmd_state = BOT_CMD_SEND;
	return -RTK_ERR_IO;
}

/**
  * @brief  Handle a bulk transfer of the BOT FSM that could not be started.
  * @note   The core refuses it before any token is sent (e.g. buffer or channel unusable), so
  *         waiting for its completion would only park the command until the SOF timeout. The
  *         device may already hold the CBW, so the phases are resynchronized through Reset
  *         Recovery (BOT 5.3.4), whose CLEAR_FEATURE steps also reopen both pipes.
  * @param  host: Host handle
  * @param  msc: Msc host handle
  * @param  pipe: Bulk pipe whose transfer was refused
  * @retval None
  */
static void usbh_msc_start_failed(usb_host_t *host, usbh_msc_host_t *msc, const usbh_pipe_t *pipe)
{
	RTK_LOGS(TAG, RTK_LOG_ERROR, "EP%02x start fail, bot %d\n", pipe->ep_addr, msc->hbot.state);
	msc->hbot.reset_recovery = 0U;
	msc->hbot.state = BOT_UNRECOVERED_ERROR;
	usbh_notify(host, 0, &usbh_msc_driver);
}

/**
  * @brief  Decodes the CSW received by the device and updates the
  *         same to upper layer.
  * @param  host: Host handle
  * @retval Status
  * @notes
  *     Refer to USB Mass-Storage Class : BOT (www.usb.org)
  *    6.3.1 Valid CSW Conditions :
  *     The host shall consider the CSW valid when:
  *     1. dCSWSignature is equal to 53425355h
  *     2. the CSW is 13 (Dh) bytes in length,
  *     3. dCSWTag matches the dCBWTag from the corresponding CBW.
  */

static usb_msc_bot_csw_state_t usbh_msc_decode_csw(usb_host_t *host)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	usb_msc_bot_csw_t *csw = msc->hbot.csw;
	usb_msc_bot_cbw_t *cbw = msc->hbot.cbw;
	usb_msc_bot_csw_state_t status = BOT_CSW_CMD_FAILED;

	/*Checking if the transfer length is different than 13*/
	if (usbh_get_last_transfer_size(host, &msc->bulk_in) != USB_MSC_CSW_LEN) {
		/*(4) Hi > Dn (Host expects to receive data from the device,
		Device intends to transfer no data)
		(5) Hi > Di (Host expects to receive data from the device,
		Device intends to send data to the host)
		(9) Ho > Dn (Host expects to send data to the device,
		Device intends to transfer no data)
		(11) Ho > Do  (Host expects to send data to the device,
		Device intends to receive data from the host)*/


		status = BOT_CSW_PHASE_ERROR;
	} else {
		/* CSW length is Correct */

		/* Check validity of the CSW Signature and CSWStatus */
		if (csw->field.dCSWSignature == USB_MSC_CSW_SIGN) {
			/* Check Condition 1. dCSWSignature is equal to 53425355h */

			if (csw->field.dCSWTag == cbw->field.dCBWTag) {
				/* Check Condition 3. dCSWTag matches the dCBWTag from the
				corresponding CBW */

				/* dCSWDataResidue must not exceed dCBWDataTransferLength (BOT §6.5.1) */
				if (csw->field.dCSWDataResidue > cbw->field.dCBWDataTransferLength) {
					status = BOT_CSW_PHASE_ERROR;
				} else if (csw->field.bCSWStatus == 0U) {
					/* Refer to USB Mass-Storage Class : BOT (www.usb.org)

					Hn Host expects no data transfers
					Hi Host expects to receive data from the device
					Ho Host expects to send data to the device

					Dn Device intends to transfer no data
					Di Device intends to send data to the host
					Do Device intends to receive data from the host

					Section 6.7
					(1) Hn = Dn (Host expects no data transfers,
					Device intends to transfer no data)
					(6) Hi = Di (Host expects to receive data from the device,
					Device intends to send data to the host)
					(12) Ho = Do (Host expects to send data to the device,
					Device intends to receive data from the host)

					*/

					status = BOT_CSW_CMD_PASSED;
				} else if (csw->field.bCSWStatus == 1U) {
					status = BOT_CSW_CMD_FAILED;
				}

				else if (csw->field.bCSWStatus == 2U) {
					/* Refer to USB Mass-Storage Class : BOT (www.usb.org)
					Section 6.7
					(2) Hn < Di ( Host expects no data transfers,
					Device intends to send data to the host)
					(3) Hn < Do ( Host expects no data transfers,
					Device intends to receive data from the host)
					(7) Hi < Di ( Host expects to receive data from the device,
					Device intends to send data to the host)
					(8) Hi <> Do ( Host expects to receive data from the device,
					Device intends to receive data from the host)
					(10) Ho <> Di (Host expects to send data to the device,
					Di Device intends to send data to the host)
					(13) Ho < Do (Host expects to send data to the device,
					Device intends to receive data from the host)
					*/

					status = BOT_CSW_PHASE_ERROR;
				} else {
				}
			} else {
				/* BOT §6.3.1: tag mismatch means invalid CSW, must go through Reset Recovery */
				status = BOT_CSW_PHASE_ERROR;
			} /* CSW Tag Matching is Checked  */
		} /* CSW Signature Correct Checking */
		else {
			/* If the CSW Signature is not valid, We shall return the Phase Error to
			Upper Layers for Reset Recovery */

			status = BOT_CSW_PHASE_ERROR;
		}
	} /* CSW Length Check*/

	return status;
}

/**
  * @brief  Hand one READ10/WRITE10 to the host task and wait for its end.
  * @note   The BOT FSM runs in the host task, which alone may issue the EP0 requests of
  *         Reset Recovery, so this thread only publishes the command and blocks. On timeout
  *         it marks the command abandoned and waits again without a limit: returning earlier
  *         could let the host task copy READ data into a buffer the caller has already reused.
  *         The wait stays short, since the host task releases the caller on its first pass over
  *         the abort, before any device request (see usbh_msc_process_abort()), and the SOF tick
  *         re-posts that pass if its message is lost.
  * @param  lun: logical Unit Number
  * @param  address: sector address
  * @param  rx_buf: user buffer receiving the data of a READ, NULL for a WRITE
  * @param  tx_buf: user buffer holding the data of a WRITE, NULL for a READ
  * @param  length: number of sectors
  * @param  cmd: MSC_READ or MSC_WRITE
  * @retval Status
  */
static int usbh_msc_xfer(u8 lun, u32 address, u8 *rx_buf, const u8 *tx_buf, u32 length, u8 cmd)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	usb_host_t *host = msc->host;
	u32 timeout_ms;
	int ret = RTK_SUCCESS;
	u8 abandoned = 0U;

	if ((lun >= USBH_MSC_MAX_LUN) || ((rx_buf == NULL) && (tx_buf == NULL)) || (length == 0U)) {
		return -RTK_ERR_BADARG;
	}

	/* The last block must have a 32-bit LBA (READ10/WRITE10), and the byte offset of the last
	   chunk in the user buffer must fit in u32. */
	if (((length - 1U) > (0xFFFFFFFFU - address)) ||
		((msc->unit[lun].capacity.block_size != 0U) && (length > (0xFFFFFFFFU / msc->unit[lun].capacity.block_size)))) {
		return -RTK_ERR_BADARG;
	}

	/* A LUN retired by a failed init or a failed Reset Recovery stays out until re-attach. */
	if (msc->unit[lun].retired != 0U) {
		return -RTK_ERR_IO;
	}

	if ((host == NULL) || (msc->hbot.cmd_done_sema == NULL)) {
		return RTK_FAIL;
	}

	if ((msc->bulk_in.pipe_num == 0U) || (msc->bulk_out.pipe_num == 0U)) {
		return -RTK_ERR_IO;
	}

	if (host->connect_state != USBH_STATE_SETUP) {
		return RTK_FAIL;
	}

	/* Saturate instead of wrapping: an oversized request then waits "forever", which the
	   host task still bounds by ending the command. */
	timeout_ms = (length >= (0xFFFFFFFFU / MSC_CMD_TIMEOUT_PER_BLOCK_MS)) ? 0xFFFFFFFFU : (length * MSC_CMD_TIMEOUT_PER_BLOCK_MS);

	usb_os_enter_critical();
	if (msc->host == NULL) {
		/* detach() ran since the checks above: no process() would run this command */
		ret = RTK_FAIL;
	} else if ((msc->state != MSC_IDLE) || (msc->unit[lun].state != MSC_IDLE)) {
		ret = -RTK_ERR_BUSY;
	} else {
		/* Remember the command parameters so a Reset Recovery retry re-issues it unchanged (BOT §6.3.3). */
		msc->hbot.cmd_address = address;
		msc->hbot.cmd_rx_buf = rx_buf;
		msc->hbot.cmd_tx_buf = tx_buf;
		msc->hbot.cmd_block_count = length;
		msc->hbot.cmd_done_count = 0U;
		/* Only a REQUEST SENSE of this command may fill it, so the caller never reads stale sense */
		usb_os_memset((void *)&msc->unit[lun].sense, 0, sizeof(msc->unit[lun].sense));
		msc->hbot.cmd_result = RTK_FAIL;
		msc->current_lun = lun;
		msc->unit[lun].state = cmd;
		msc->sof_tick = usbh_get_tick(host);
		msc->state = cmd;
	}
	usb_os_exit_critical();

	if (ret != RTK_SUCCESS) {
		return ret;
	}

	usbh_notify(host, 0, &usbh_msc_driver);

	if (usb_os_sema_take(msc->hbot.cmd_done_sema, timeout_ms) != RTK_SUCCESS) {
		usb_os_enter_critical();
		if ((msc->state == MSC_READ) || (msc->state == MSC_WRITE)) {
			msc->state = MSC_ABORT;
			abandoned = 1U;
		}
		usb_os_exit_critical();

		if (abandoned != 0U) {
			/* One line per abandoned command, enough to tell a device that keeps NAKing a bulk
			   phase (urb 2 = BUSY) from a phase that ended in error without being retired. */
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Cmd %d timeout, bot %d/%d, in %d/%d, out %d/%d\n", cmd, msc->hbot.state, msc->hbot.cmd_state,
					 msc->bulk_in.xfer_state, usbh_get_urb_state(host, &msc->bulk_in),
					 msc->bulk_out.xfer_state, usbh_get_urb_state(host, &msc->bulk_out));
			usbh_notify(host, 0, &usbh_msc_driver);
		}
		/* Already ended otherwise: the give is pending, so this take returns at once. */
		usb_os_sema_take(msc->hbot.cmd_done_sema, USB_OS_SEMA_TIMEOUT);
	}

	return msc->hbot.cmd_result;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Handle the BOT protocol.
  * @param  host: Host handle
  * @param  lun: Logical Unit Number
  * @retval Status
  */
int usbh_msc_bot_process(usb_host_t *host, u8 lun)
{
	int status = -RTK_ERR_BUSY;
	int error  = -RTK_ERR_BUSY;
	usb_msc_bot_csw_state_t CSW_Status = BOT_CSW_CMD_FAILED;
	usbh_msc_host_t *msc = &usbh_msc_host;
	usbh_setup_req_t setup;
	usb_msc_bot_csw_t *csw = msc->hbot.csw;
	usb_msc_bot_cbw_t *cbw = msc->hbot.cbw;
	usbh_pipe_t *bulk_out = &msc->bulk_out;
	usbh_pipe_t *bulk_in = &msc->bulk_in;
	int ret;

	switch (msc->hbot.state) {
	case BOT_SEND_CBW:

		if (bulk_out->xfer_state == USBH_EP_XFER_START) {
			cbw->field.bCBWLUN = lun;
			cbw->field.dCBWTag = ++msc->hbot.tag_counter;
			msc->hbot.csw_retried = 0U;
			/* Only a Data-In phase of this command sets it, so a STALLed or skipped one reads 0 */
			msc->hbot.rx_data_len = 0U;
			bulk_out->xfer_buf = cbw->data;
			bulk_out->xfer_len = USB_MSC_CBW_LEN;
			if (usbh_transfer_process(host, bulk_out) != RTK_SUCCESS) {
				usbh_msc_start_failed(host, msc, bulk_out);
			}
		} else {
			ret = usbh_transfer_process(host, bulk_out);

			if ((ret == RTK_SUCCESS) && (bulk_out->xfer_state == USBH_EP_XFER_IDLE)) {
				if (cbw->field.dCBWDataTransferLength != 0U) {
					/* If there is Data Transfer Stage */
					if (((cbw->field.bmCBWFlags) & USB_REQ_DIR_MASK) == USB_D2H) {
						/* Data Direction is IN */
						msc->hbot.state = BOT_DATA_IN;
						bulk_in->xfer_state = USBH_EP_XFER_START;
					} else {
						/* Data Direction is OUT */
						msc->hbot.state = BOT_DATA_OUT;
						bulk_out->xfer_state = USBH_EP_XFER_START;
					}
				} else {
					/* If there is NO Data Transfer Stage */
					msc->hbot.state = BOT_RECEIVE_CSW;
					bulk_in->xfer_state = USBH_EP_XFER_START;
				}
				usbh_notify(host, 0, &usbh_msc_driver);
			} else if (bulk_out->xfer_state == USBH_EP_XFER_ERROR) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "TX CBW err: %d\n", usbh_get_urb_state(host, bulk_out));
				msc->hbot.state  = BOT_ERROR_OUT;
				usbh_notify(host, 0, &usbh_msc_driver);
			}
		}
		break;
	case BOT_DATA_IN:

		if (bulk_in->xfer_state == USBH_EP_XFER_START) {
			bulk_in->xfer_buf = msc->hbot.pbuf;
			bulk_in->xfer_len = cbw->field.dCBWDataTransferLength;
			/* hbot.data serves the small commands; a READ(10) chunk is a whole number of blocks,
			   so of packets, inside the bounce buffer */
			bulk_in->xfer_buf_len = (msc->hbot.pbuf == msc->hbot.data) ? USBH_BOT_DATA_LENGTH : msc->hbot.bounce_len;
			if (usbh_transfer_process(host, bulk_in) != RTK_SUCCESS) {
				usbh_msc_start_failed(host, msc, bulk_in);
			}
		} else {
			ret = usbh_transfer_process(host, bulk_in);

			if ((ret == RTK_SUCCESS) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
				/* Capture the real Data-In size here: the CSW transfer reuses this pipe and
				   overwrites the channel RX length. BOT §6.7 case 5 (Hi > Di) is legal, so the
				   device may have sent less than dCBWDataTransferLength. */
				msc->hbot.rx_data_len = usbh_get_last_transfer_size(host, bulk_in);
				if (msc->hbot.rx_data_len > cbw->field.dCBWDataTransferLength) {
					/* BOT 6.7.2 case 7 (Hi < Di): the device sent more than the host asked for,
					   a phase error resolved by Reset Recovery. The data are not passed on. */
					RTK_LOGS(TAG, RTK_LOG_ERROR, "RX data %d > %d\n", msc->hbot.rx_data_len, cbw->field.dCBWDataTransferLength);
					msc->hbot.rx_data_len = 0U;
					msc->hbot.state = BOT_UNRECOVERED_ERROR;
				} else {
					msc->hbot.state  = BOT_RECEIVE_CSW;
					bulk_in->xfer_state = USBH_EP_XFER_START;
				}
				usbh_notify(host, 0, &usbh_msc_driver);
			} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "RX data err: %d\n", usbh_get_urb_state(host, bulk_in));
				msc->hbot.state  = BOT_ERROR_IN;
				usbh_notify(host, 0, &usbh_msc_driver);
			}
		}
		break;
	case BOT_DATA_OUT:

		if (bulk_out->xfer_state == USBH_EP_XFER_START) {
			bulk_out->xfer_buf = msc->hbot.pbuf;
			bulk_out->xfer_len = cbw->field.dCBWDataTransferLength;
			if (usbh_transfer_process(host, bulk_out) != RTK_SUCCESS) {
				usbh_msc_start_failed(host, msc, bulk_out);
			}
		} else {
			ret = usbh_transfer_process(host, bulk_out);

			if ((ret == RTK_SUCCESS) && (bulk_out->xfer_state == USBH_EP_XFER_IDLE)) {
				msc->hbot.state = BOT_RECEIVE_CSW;
				bulk_in->xfer_state = USBH_EP_XFER_START;
				usbh_notify(host, 0, &usbh_msc_driver);
			} else if (bulk_out->xfer_state == USBH_EP_XFER_ERROR) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "TX data err: %d\n", usbh_get_urb_state(host, bulk_out));
				msc->hbot.state  = BOT_ERROR_OUT;
				usbh_notify(host, 0, &usbh_msc_driver);
			}
		}
		break;
	case BOT_RECEIVE_CSW:

		if (bulk_in->xfer_state == USBH_EP_XFER_START) {
			bulk_in->xfer_buf = csw->data;
			bulk_in->xfer_len = USB_MSC_CSW_LEN;
			bulk_in->xfer_buf_len = USBH_BOT_CSW_BUF_LENGTH;
			if (usbh_transfer_process(host, bulk_in) != RTK_SUCCESS) {
				usbh_msc_start_failed(host, msc, bulk_in);
			}
		} else {
			/* Drive the CSW like every other bulk phase: usbh_transfer_process() bounds a NAK
			   run and retries transaction errors, so neither can park this state forever. */
			ret = usbh_transfer_process(host, bulk_in);

			if ((ret == RTK_SUCCESS) && (bulk_in->xfer_state == USBH_EP_XFER_IDLE)) {
				CSW_Status = usbh_msc_decode_csw(host);

				if (CSW_Status == BOT_CSW_PHASE_ERROR) {
					/* BOT §6.3.3: PHASE_ERROR requires Reset Recovery */
					msc->hbot.state = BOT_UNRECOVERED_ERROR;
					msc->hbot.cmd_state = BOT_CMD_BUSY;
					status = -RTK_ERR_BUSY;
				} else {
					msc->hbot.state = BOT_SEND_CBW;
					msc->hbot.cmd_state = BOT_CMD_SEND;
					bulk_out->xfer_state = USBH_EP_XFER_START;
					if (CSW_Status == BOT_CSW_CMD_PASSED) {
						status = RTK_SUCCESS;
					} else {
						status = RTK_FAIL;
					}
				}
				usbh_notify(host, 0, &usbh_msc_driver);
			} else if (bulk_in->xfer_state == USBH_EP_XFER_ERROR) {
				/* BOT 6.7.2 / 5.3.3: a failed CSW read gets one ClearFeature(HALT) on Bulk-In and
				   one more read; a second failure is resolved by Reset Recovery. */
				RTK_LOGS(TAG, RTK_LOG_ERROR, "RX CSW err: %d\n", usbh_get_urb_state(host, bulk_in));
				if (msc->hbot.csw_retried == 0U) {
					msc->hbot.csw_retried = 1U;
					msc->hbot.state = BOT_ERROR_IN;
				} else {
					msc->hbot.state = BOT_UNRECOVERED_ERROR;
				}
				usbh_notify(host, 0, &usbh_msc_driver);
			} else {
				/* transfer in flight */
			}
		}
		break;
	case BOT_ERROR_IN:
		error = usbh_ctrl_clear_feature(host, bulk_in->ep_addr);
		if (error == RTK_SUCCESS) {
			usbh_msc_reopen_pipe(host, bulk_in);
			if (msc->hbot.reset_recovery) {
				/* BOT 5.3.4: Reset Recovery clears Bulk-In first, then Bulk-Out. */
				msc->hbot.state = BOT_ERROR_OUT;
			} else {
				/* BOT 6.7.2: after a Data-In STALL the CSW is read from the cleared Bulk-In. */
				msc->hbot.state = BOT_RECEIVE_CSW;
				bulk_in->xfer_state = USBH_EP_XFER_START;
			}
		} else if (error != -RTK_ERR_BUSY) {
			status = usbh_msc_clear_halt_failed(msc);
		} else {
			/* request in flight */
		}
		break;

	case BOT_ERROR_OUT:
		error = usbh_ctrl_clear_feature(host, bulk_out->ep_addr);
		if (error == RTK_SUCCESS) {
			usbh_msc_reopen_pipe(host, bulk_out);
			if (msc->hbot.reset_recovery) {
				/* BOT 5.3.4: both halts cleared, Reset Recovery is complete. */
				msc->hbot.reset_recovery = 0U;
				msc->hbot.state = BOT_SEND_CBW;
				msc->hbot.cmd_state = BOT_CMD_SEND;
				bulk_out->xfer_state = USBH_EP_XFER_START;
			} else {
				/* BOT 6.7.3: after a Data-Out STALL only Bulk-Out is cleared, then the CSW is read. */
				msc->hbot.state = BOT_RECEIVE_CSW;
				bulk_in->xfer_state = USBH_EP_XFER_START;
			}
		} else if (error != -RTK_ERR_BUSY) {
			status = usbh_msc_clear_halt_failed(msc);
		} else {
			/* request in flight */
		}
		break;

	case BOT_UNRECOVERED_ERROR:
		setup.req.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
		setup.req.bRequest = USB_MSC_REQUEST_BOT_RESET;
		setup.req.wValue = 0U;
		setup.req.wIndex = msc->itf_num;
		setup.req.wLength = 0U;

		status = usbh_ctrl_request(host, &setup, NULL);
		if (status == RTK_SUCCESS) {
			/* BOT 5.3.4: after the reset, clear ENDPOINT_HALT on Bulk-In and then Bulk-Out.
			   The command is not done yet, so report BUSY: RTK_SUCCESS here would make the SCSI
			   layer treat the aborted command as passed. */
			msc->hbot.reset_recovery = 1U;
			msc->hbot.state = BOT_ERROR_IN;
			status = -RTK_ERR_BUSY;
		} else if (status != -RTK_ERR_BUSY) {
			/* BOT Reset failed permanently; give up and force a command retry */
			msc->hbot.state = BOT_SEND_CBW;
			msc->hbot.cmd_state = BOT_CMD_SEND;
			status = -RTK_ERR_IO;
		}
		break;

	default:
		break;
	}

	return status;
}

/**
  * @brief  Check if the MSC function is ready
  * @retval Status
  */
int usbh_msc_is_ready(void)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	int res;

	if (msc->host == NULL) {
		return 0;
	}

	if ((msc->host->connect_state == USBH_STATE_SETUP) && (msc->state == MSC_IDLE)) {
		res = 1;
	} else {
		res = 0;
	}

	return res;
}

/**
  * @brief  Get the Max LUN supported
  * @retval logical Unit Number supported
  */
u32 usbh_msc_get_max_lun(void)
{
	usbh_msc_host_t *msc = &usbh_msc_host;

	if (msc->host == NULL) {
		return 0;
	}

	if ((msc->host->connect_state == USBH_STATE_SETUP) && (msc->state == MSC_IDLE)) {
		return msc->max_lun;
	}

	return 0xFFFFFFFFU;
}

/**
  * @brief  Check whether a LUN is ready
  * @param  lun: logical Unit Number
  * @retval Lun status (0: not ready / 1: ready)
  */
int usbh_msc_unit_is_ready(u8 lun)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	int res;

	if (lun >= USBH_MSC_MAX_LUN) {
		return 0;
	}

	if (msc->host == NULL) {
		return 0;
	}

	if ((msc->host->connect_state == USBH_STATE_SETUP) && (msc->unit[lun].error == MSC_OK)) {
		res = 1;
	} else {
		res = 0;
	}

	return res;
}

/**
  * @brief  Get a LUN information
  * @param  lun: logical Unit Number
  * @param  info: logical Unit information
  * @retval Status
  */
int usbh_msc_get_lun_info(u8 lun, usbh_msc_lun_t *info)
{
	usbh_msc_host_t *msc = &usbh_msc_host;

	if (info == NULL) {
		return -RTK_ERR_BADARG;
	}

	if (msc->host == NULL) {
		return RTK_FAIL;
	}

	if ((msc->host->connect_state == USBH_STATE_SETUP) && (USBH_MSC_MAX_LUN > lun)) {
		usb_os_memcpy((void *)info, (const void *)&msc->unit[lun], sizeof(usbh_msc_lun_t));
		return RTK_SUCCESS;
	} else {
		return RTK_FAIL;
	}
}

/**
  * @brief  Performs a Read operation
  * @param  lun: logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to read
  * @retval Status
  */
int usbh_msc_read(u8 lun, u32 address, u8 *pbuf, u32 length)
{
	if (pbuf == NULL) {
		return -RTK_ERR_BADARG;
	}

	return usbh_msc_xfer(lun, address, pbuf, NULL, length, MSC_READ);
}

/**
  * @brief  Performs a Write operation.
  * @param  lun: logical Unit Number
  * @param  address: sector address
  * @param  pbuf: pointer to data
  * @param  length: number of sector to write
  * @retval Status
  */
int usbh_msc_write(u8 lun, u32 address, const u8 *pbuf, u32 length)
{
	if (pbuf == NULL) {
		return -RTK_ERR_BADARG;
	}

	return usbh_msc_xfer(lun, address, NULL, pbuf, length, MSC_WRITE);
}

/**
  * @brief  Init MSC class
  * @param  cb: User callback
  * @param  cfg: Class configuration
  * @retval Status
  */
int usbh_msc_init(const usbh_msc_cb_t *cb, const usbh_msc_config_t *cfg)
{
	int ret = RTK_SUCCESS;
	usbh_msc_host_t *msc = &usbh_msc_host;
	u32 buf_size;

	if ((cb == NULL) || (cfg == NULL)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return -RTK_ERR_BADARG;
	}

	/* Whole blocks of any accepted block size, and one channel transfer at full speed */
	buf_size = (cfg->buf_size != 0U) ? cfg->buf_size : USBH_MSC_DEFAULT_BUF_SIZE;
	if ((buf_size == 0U) || (buf_size > USBH_MSC_MAX_BUF_SIZE) || ((buf_size % USBH_MSC_MAX_BLOCK_SIZE) != 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Bad buf size %u\n", buf_size);
		return -RTK_ERR_BADARG;
	}

	msc->cb = cb;

	msc->hbot.cbw = usb_os_malloc(USB_MSC_CBW_LEN);
	if (msc->hbot.cbw == NULL) {
		return -RTK_ERR_NOMEM;
	}

	msc->hbot.csw = usb_os_malloc(USBH_BOT_CSW_BUF_LENGTH);
	if (msc->hbot.csw == NULL) {
		ret = -RTK_ERR_NOMEM;
		goto exit_free;
	}

	msc->hbot.data = usb_os_malloc(USBH_BOT_DATA_LENGTH);
	if (msc->hbot.data == NULL) {
		ret = -RTK_ERR_NOMEM;
		goto exit_free;
	}

	/* GetMaxLUN returns one byte, but its data stage reserves a whole EP0 packet */
	msc->max_lun_buf = usb_os_malloc(USB_MAX_EP0_SIZE);
	if (msc->max_lun_buf == NULL) {
		ret = -RTK_ERR_NOMEM;
		goto exit_free;
	}

	msc->hbot.bounce_buf = usb_os_malloc(buf_size);
	if (msc->hbot.bounce_buf == NULL) {
		ret = -RTK_ERR_NOMEM;
		goto exit_free;
	}
	msc->hbot.bounce_len = buf_size;

	ret = usb_os_sema_create(&(msc->hbot.cmd_done_sema));
	if (ret != RTK_SUCCESS) {
		goto exit_free;
	}

	ret = usbh_register_class(&usbh_msc_driver);
	if (ret != RTK_SUCCESS) {
		goto exit_free;
	}
	return ret;

exit_free:
	if (msc->hbot.cmd_done_sema != NULL) {
		usb_os_sema_delete(msc->hbot.cmd_done_sema);
		msc->hbot.cmd_done_sema = NULL;
	}
	usb_os_mfree((void *)msc->hbot.bounce_buf);
	msc->hbot.bounce_buf = NULL;
	msc->hbot.bounce_len = 0U;
	usb_os_mfree((void *)msc->max_lun_buf);
	msc->max_lun_buf = NULL;
	usb_os_mfree((void *)msc->hbot.data);
	msc->hbot.data = NULL;
	usb_os_mfree((void *)msc->hbot.csw);
	msc->hbot.csw = NULL;
	usb_os_mfree((void *)msc->hbot.cbw);
	msc->hbot.cbw = NULL;
	return ret;
}

/**
  * @brief  Deinit MSC class
  */
void usbh_msc_deinit(void)
{
	usbh_msc_host_t *msc = &usbh_msc_host;
	usb_host_t *host = msc->host;
	usbh_pipe_t *bulk_out = &msc->bulk_out;
	usbh_pipe_t *bulk_in = &msc->bulk_in;

	/* First, before anything is freed: on RTK_SUCCESS no attach/setup/process/detach of this
	 * class runs any more, an EP0 request it left in flight is halted and a sof() still
	 * running on another core is waited out. Anything else means nothing was released. */
	if (usbh_unregister_class(&usbh_msc_driver) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Deinit refused\n");
		return;
	}

	if (bulk_in->pipe_num) {
		usbh_close_pipe(host, bulk_in);
	}

	if (bulk_out->pipe_num) {
		usbh_close_pipe(host, bulk_out);
	}

	usb_os_mfree((void *)msc->max_lun_buf);
	msc->max_lun_buf = NULL;

	/* hbot.pbuf only aliases hbot.data or hbot.bounce_buf, it owns no memory */
	msc->hbot.pbuf = NULL;

	usb_os_mfree((void *)msc->hbot.bounce_buf);
	msc->hbot.bounce_buf = NULL;
	msc->hbot.bounce_len = 0U;

	usb_os_mfree((void *)msc->hbot.data);
	msc->hbot.data = NULL;

	usb_os_mfree((void *)msc->hbot.csw);
	msc->hbot.csw = NULL;

	usb_os_mfree((void *)msc->hbot.cbw);
	msc->hbot.cbw = NULL;

	/* No caller can be waiting on it: usbh_msc_deinit() is serialized against read/write. */
	if (msc->hbot.cmd_done_sema != NULL) {
		usb_os_sema_delete(msc->hbot.cmd_done_sema);
		msc->hbot.cmd_done_sema = NULL;
	}

	/* Cover the deinit-while-connected path (no detach ran): drop the host handle and the
	   bring up state, so a later API call is refused instead of reading the host the core
	   is about to clear, as in usbh_cdc_acm_deinit(). */
	msc->host = NULL;
	msc->state = MSC_INIT;
}
