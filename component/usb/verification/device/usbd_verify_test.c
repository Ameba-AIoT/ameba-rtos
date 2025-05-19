/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "usbd.h"
#include "usbd_verify.h"
#include "usbd_chip_info.h"

static const char *const TAG = "USBD";
static volatile u8 usbd_verify_ep_status_flag = 0;

/* Echo asynchronously, for transfer size larger than packet size. While fpr
 * transfer size less than packet size, the synchronous way is preferred.
 */
#define CONFIG_USBD_VERIFY_BULK_ASYNC_XFER		0

#if CONFIG_USBD_VERIFY_BULK_ASYNC_XFER
#define CONFIG_VERIFY_BULK_XFER_THREAD_PRIORITY			5U
#define CONFIG_VERIFY_BULK_ASYNC_FIFO_CNT               3U
static volatile u8 usbd_verify_bulk_async_flag = 0;
static u8 *verify_bulk_async_xfer_fifo[CONFIG_VERIFY_BULK_ASYNC_FIFO_CNT];
static u32 async_bulk_fifo_write;
static u32 async_bulk_fifo_read;
#define IS_BULK_ASYNC_AVAIL (((async_bulk_fifo_write+1) % CONFIG_VERIFY_BULK_ASYNC_FIFO_CNT) != async_bulk_fifo_read)
static u32 verify_bulk_xfer_idx = 0;
static rtos_sema_t verify_bulk_async_xfer_sema;
#endif

/* Both direction xfer:
*   loopback = 1: IN only = 0 && OUT only = 0
*   loopback = 0: IN only = 1
* Single direction xfer: OUT only = 1 or IN only = 1
*   IN only  = 1: prepare IN xfer, and restart IN once IN xfer complete
*   OUT only = 1: no loopback. another OUT/IN start when this OUT/IN done.
*/
static u8 usbd_intr_out_only;
static u8 usbd_isoc_out_only;
static u8 usbd_bulk_out_only;
static u8 usbd_intr_in_only;
static u8 usbd_isoc_in_only;
static u8 usbd_bulk_in_only;
static u32 usbd_intr_in_len = 63;
static u32 usbd_isoc_in_len = 63;
static u32 usbd_bulk_in_len = 63;

/*
	allow to set ep infor by cmd
*/
#define USBD_EP_COUNT_MAX (10)
typedef struct {
	usbd_verify_ep_t  ep_array[USBD_EP_COUNT_MAX];
	__IO u32     ctrl_out_done_count;
	__IO u32     ctrl_in_done_count;
	u8 		  *description_buf;
	u8   	  ep_count;
	u8		  description_buf_len;
} cmd_usbd_verify_ep_t;

/* Private function prototypes -----------------------------------------------*/
static int cmd_usbd_verify_init(void);
static int cmd_usbd_verify_deinit(void);
static u8 *cmd_usbd_verify_get_config_desc(u16 *len);
static int cmd_usbd_verify_set_config(usb_dev_t *dev);
static int cmd_usbd_verify_clear_config(usb_dev_t *dev);
static int cmd_usbd_verify_setup(u8 cmd, u8 *buf, u16 len, u16 value);
static int cmd_usbd_verify_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int cmd_usbd_verify_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static int cmd_usbd_verify_handle_ep0_data_in(usb_dev_t *dev, u8 status);
static int cmd_usbd_verify_handle_ep0_data_out(usb_dev_t *dev);
static int cmd_usbd_verify_cb_received(u8 *buf, u32 len, void *ep);

/*
	usb class define
*/
/* Verify Device */
static cmd_usbd_verify_ep_t cmd_usbd_verify_ep;

static usbd_verify_cb_t cmd_usbd_verify_cb = {
	.init = cmd_usbd_verify_init,
	.deinit = cmd_usbd_verify_deinit,
	.get_config_desc = cmd_usbd_verify_get_config_desc,

	.set_config = cmd_usbd_verify_set_config,
	.clear_config = cmd_usbd_verify_clear_config,
	.setup = cmd_usbd_verify_setup,

	.ep0_data_in = cmd_usbd_verify_handle_ep0_data_in,
	.ep0_data_out = cmd_usbd_verify_handle_ep0_data_out,
	.ep_data_in = cmd_usbd_verify_handle_ep_data_in,
	.ep_data_out = cmd_usbd_verify_handle_ep_data_out,
};

static usbd_config_t cmd_usbd_verify_cfg = {
	.speed = USB_SPEED_HIGH,
	.dma_enable = 1U,
	.isr_priority = INT_PRI_MIDDLE,
#if defined(CONFIG_AMEBASMART)
	/* EOPF for ISOC OUT */
	.ext_intr_en = USBD_EOPF_INTR | USBD_EPMIS_INTR,
	/*DFIFO total 1024 DWORD, resv 8 DWORD for DMA addr*/
#elif defined (CONFIG_AMEBAGREEN2)
	/*DFIFO total 1024 DWORD, resv 12 DWORD for DMA addr and EP0 fixed 32 DWORD*/
	.rx_fifo_depth = 292U,
	.ptx_fifo_depth = {16U, 256U, 32U, 256U, 128U, },
	.ext_intr_en = USBD_EOPF_INTR,//for ISOC OUT
#elif defined (CONFIG_AMEBASMARTPLUS)
	/*DFIFO total 1280 DWORD, resv 14 DWORD for DMA addr and EP0 fixed 32 DWORD*/
	.rx_fifo_depth = 402U,
	.ptx_fifo_depth = {256U, 256U, 32U, 256U, 32U},
	//.ext_intr_en =  USBD_EOPF_INTR,
#elif defined (CONFIG_AMEBAL2)
	/*DFIFO total 1024 DWORD, resv 11 DWORD for DMA addr and EP0 fixed 32 DWORD*/
	.rx_fifo_depth = 405U,
	.ptx_fifo_depth = {256U, 256U, 32U, 32U},
#endif
	.intr_use_ptx_fifo = 0U,
};

/**
  * @}
  */
/* Private functions ---------------------------------------------------------*/
static void cmd_usbd_verify_dump_ep_info(void)
{
	u8 ep_index;
	usbd_verify_ep_t *ep;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		ep = cdev->ep_array + ep_index;
		usbd_verify_dump_ep(ep);
	}
}
static int cmd_usbd_verify_update_ep_description(u8 *buf, usbd_verify_ep_t *ep)
{
	usbd_verify_ep_basic_t *ep_infor = &(ep->ep_infor);
	buf[USBD_VERIFY_EP_ADDR_OFFSET]     = ep_infor->ep_addr;
	buf[USBD_VERIFY_EP_TYPE_OFFSET]     = ep_infor->ep_type;
	buf[USBD_VERIFY_EP_MPS_OFFSET + 0]  = USB_LOW_BYTE(ep_infor->mps);
	buf[USBD_VERIFY_EP_MPS_OFFSET + 1]  = USB_HIGH_BYTE(ep_infor->mps);
	buf[USBD_VERIFY_EP_INTERVAL_OFFSET] = ep_infor->ep_interval;

	return HAL_OK;
}
static int cmd_usbd_verify_update_description(void)
{
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	u16 buf_offset;
	u8 ep_index;

	/* Configuration Descriptor */
	buf_offset = 0;
	usb_os_memcpy((void *)(cdev->description_buf + buf_offset), (void *)usbd_verify_cfg_desc, USB_LEN_CFG_DESC);
	cdev->description_buf[USBD_VERIFY_CFG_LEN_OFFSET + 0] = USB_LOW_BYTE(cdev->description_buf_len);
	cdev->description_buf[USBD_VERIFY_CFG_LEN_OFFSET + 1] = USB_HIGH_BYTE(cdev->description_buf_len);

	/* interface description */
	buf_offset += USB_LEN_CFG_DESC;
	usb_os_memcpy((void *)(cdev->description_buf + buf_offset), (void *)usbd_verify_if_desc, USB_LEN_IF_DESC);
	cdev->description_buf[buf_offset + USBD_VERIFY_IF_EPNUM_OFFSET] = cdev->ep_count;

	/* ep description */
	buf_offset += USB_LEN_IF_DESC;
	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		usb_os_memcpy((void *)(cdev->description_buf + buf_offset),
					  (void *)usbd_verify_ep_desc, USB_LEN_EP_DESC);

		//usbd_verify_dump_ep(cdev->ep_array + ep_index);
		cmd_usbd_verify_update_ep_description(cdev->description_buf + buf_offset, cdev->ep_array + ep_index);

		buf_offset += USB_LEN_EP_DESC;
	}

	return HAL_OK;
}

static int cmd_usbd_verify_set_config(usb_dev_t *pdev)
{
	u8 ep_index;
	usbd_verify_ep_t *ep;
	usbd_verify_ep_basic_t *ep_infor;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;

	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		ep = cdev->ep_array + ep_index;
		usbd_verify_ep_enable(pdev, ep);
		ep_infor = &(ep->ep_infor);
		if (USB_EP_IS_IN(ep_infor->ep_addr)) {
			/* Init IN EP */
			ep->state = VERIFY_TRANSFER_STATE_IDLE;
			usb_os_memset(ep->buf, 0x00, ep_infor->trans_len);
		} else {
			/* Init OUT EP */
			usbd_verify_receive_data(ep);
		}
	}

	return HAL_OK;
}

static int cmd_usbd_verify_clear_config(usb_dev_t *pdev)
{
	u8 ep_index;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	/* DeInit EP */
	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		usbd_verify_ep_disable(pdev, cdev->ep_array + ep_index);
	}

	return HAL_OK;
}

static int cmd_usbd_verify_setup(u8 cmd, u8 *buf, u16 len, u16 value)
{
	UNUSED(cmd);
	UNUSED(value);
	// Echo back the ctrl request
	usbd_verify_transmit_ctrl_data(buf, len);

	return HAL_OK;
}

/*
	soc information
*/
static u8 *cmd_usbd_verify_get_config_desc(u16 *len)
{
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	cmd_usbd_verify_update_description();

	*len = cdev->description_buf_len;
	return cdev->description_buf;
}

static int cmd_usbd_verify_handle_ep0_data_in(usb_dev_t *dev, u8 status)
{
	UNUSED(dev);
	UNUSED(status);

	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	cdev->ctrl_in_done_count++;
	return HAL_OK;
}

static int cmd_usbd_verify_handle_ep0_data_out(usb_dev_t *dev)
{
	UNUSED(dev);
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	cdev->ctrl_out_done_count++;
	//RTK_LOGD(TAG, "EP0 data out, %d %d\n", dev->ep0_data_len, dev->ep0_xfer_total_len);
	return HAL_OK;
}

//ep_num from 1~15
//update usb in status
static int cmd_usbd_verify_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	UNUSED(dev);
	u8 ep_index;
	usbd_verify_ep_t *ep;
	usbd_verify_ep_basic_t *ep_infor;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;

	/* Loop All EP */
	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		ep = cdev->ep_array + ep_index;
		ep_infor = &(ep->ep_infor);
		if (USB_EP_IS_IN(ep_infor->ep_addr)
			&& (ep_addr == ep_infor->ep_addr)) {
			if (status == HAL_OK) {
				if (ep->zlp) {
					ep->zlp = 0;
					//RTK_LOGS(TAG, RTK_LOG_DEBUG, "%s TX ZLP\n", usbd_verify_get_xfer_type_text(ep_infor->ep_type));
					usbd_verify_transmit_zlp(ep_infor->ep_addr);
				} else {
					/*TX done*/
					ep->done_count ++;
					ep->state = VERIFY_TRANSFER_STATE_IDLE;
					if ((ep_infor->ep_type == USB_CH_EP_TYPE_BULK) && (usbd_bulk_in_only)) {
						usb_os_memset(ep->buf, (u8)(ep->buf[0] + 1), usbd_bulk_in_len);
						ep->ep_infor.trans_len = usbd_bulk_in_len;
						usbd_verify_transmit_data(ep);
					} else if ((ep_infor->ep_type == USB_CH_EP_TYPE_INTR) && (usbd_intr_in_only)) {
						usb_os_memset(ep->buf, (u8)(ep->buf[0] + 1), usbd_intr_in_len);
						ep->ep_infor.trans_len = usbd_intr_in_len;
						usbd_verify_transmit_data(ep);
					} else if ((ep_infor->ep_type == USB_CH_EP_TYPE_ISOC) && (usbd_isoc_in_only)) {
						usb_os_memset(ep->buf, (u8)(ep->buf[0] + 1), usbd_isoc_in_len);
						ep->ep_infor.trans_len = usbd_isoc_in_len;
						usbd_verify_transmit_data(ep);
					}
				}
			} else {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "%s TX err: %d\n", usbd_verify_get_xfer_type_text(ep_infor->ep_type), status);
				ep->state = VERIFY_TRANSFER_STATE_IDLE;
			}
		}
	}

	return HAL_OK;
}

static int cmd_usbd_verify_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	u8 ep_index;
	usbd_verify_ep_t *ep;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	usbd_verify_ep_basic_t *ep_infor;

	UNUSED(dev);

	/* Loop All EP */
	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		ep = cdev->ep_array + ep_index;
		ep_infor = &(ep->ep_infor);
		if (USB_EP_IS_OUT(ep_infor->ep_addr)
			&& (ep_addr == ep_infor->ep_addr)) {
			switch (ep_infor->ep_type) {
			case USB_CH_EP_TYPE_ISOC:
			case USB_CH_EP_TYPE_BULK:
			case USB_CH_EP_TYPE_INTR:
				if (len > 0) {
					cmd_usbd_verify_cb_received(ep->buf, len, ep);
				}
				break;
			default://ctrl,error happen
				RTK_LOGS(TAG, RTK_LOG_WARN, "Unknown EP 0x%02x\n", ep_addr);
				break;
			}
			usbd_verify_receive_data(ep);
		}
	}

	return HAL_OK;
}

/*
	usb example define
*/
static int cmd_usbd_verify_init(void)
{
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	cdev->description_buf_len = USBD_EP_DESCRIPTION_SIZE(cdev->ep_count);
	cdev->description_buf = (u8 *)usb_os_malloc(cdev->description_buf_len);
	if (cdev->description_buf == NULL) {
		cdev->description_buf_len = 0;
		return HAL_ERR_MEM;
	}
#if CONFIG_USBD_VERIFY_BULK_ASYNC_XFER
	async_bulk_fifo_write = 0;
	async_bulk_fifo_read = 0;
	for (u8 i = 0; i < CONFIG_VERIFY_BULK_ASYNC_FIFO_CNT; i++) {
		verify_bulk_async_xfer_fifo[i] = (u8 *)usb_os_malloc(VERIFY_BULK_BUF_SIZE);
		if (verify_bulk_async_xfer_fifo[i] == NULL) {
			return HAL_ERR_MEM;
		}
	}
#endif

	return HAL_OK;
}

static int cmd_usbd_verify_deinit(void)
{
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	if (cdev->description_buf) {
		usb_os_mfree(cdev->description_buf);
		cdev->description_buf = NULL;
	}
	cdev->description_buf_len = 0;
#if CONFIG_USBD_VERIFY_BULK_ASYNC_XFER
	async_bulk_fifo_write = 0;
	async_bulk_fifo_read = 0;
	for (u8 i = 0; i < CONFIG_VERIFY_BULK_ASYNC_FIFO_CNT; i++) {
		usb_os_mfree(verify_bulk_async_xfer_fifo[i]);
		verify_bulk_async_xfer_fifo[i] = NULL;
	}
#endif

	return HAL_OK;
}

static void cmd_usbd_verify_loopback(u8 pair_addr, u8 *buf, u32 len)
{
	u8 ep_index;
	usbd_verify_ep_t *tx_ep = NULL;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	if (pair_addr != USBD_VERIFY_MATCH_EP_ADDR) {
		//find the match tx ep
		for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
			if (cdev->ep_array[ep_index].ep_infor.ep_addr == pair_addr) {
				tx_ep = cdev->ep_array + ep_index;
				break;
			}
		}

		if (tx_ep) {
#if CONFIG_USBD_VERIFY_BULK_ASYNC_XFER
			if (tx_ep->ep_infor.ep_type == USB_CH_EP_TYPE_BULK) {
				if (IS_BULK_ASYNC_AVAIL) {
					usb_os_memcpy((void *)verify_bulk_async_xfer_fifo[async_bulk_fifo_write], (void *)buf, len);
					async_bulk_fifo_write = (async_bulk_fifo_write + 1) % CONFIG_VERIFY_BULK_ASYNC_FIFO_CNT;
					tx_ep->ep_infor.trans_len = len;
					rtos_sema_give(verify_bulk_async_xfer_sema);
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK aync fifo full\n");
				}
				return;
			}
#endif
			usb_os_memcpy((void *)tx_ep->buf, (void *)buf, len);
			tx_ep->ep_infor.trans_len = len;
			usbd_verify_transmit_data(tx_ep);
		}
	} else {
		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Pair EP 0x%02x\n", pair_addr);
	}
}

//packet receive
static int cmd_usbd_verify_cb_received(u8 *buf, u32 len, void *ep)
{
	usbd_verify_ep_t *handle = (usbd_verify_ep_t *)ep;
	usbd_verify_ep_basic_t *ep_infor = &(handle->ep_infor);
	handle->done_count ++;

	usbd_verify_dump_buf((u8 *)"bulk RX", buf, len, handle->done_count);

	//loopback
	if (USB_EP_IS_OUT(ep_infor->ep_addr)) {
		if (((ep_infor->ep_type == USB_CH_EP_TYPE_BULK) && (usbd_bulk_out_only == 0) && (usbd_bulk_in_only == 0)) ||
			((ep_infor->ep_type == USB_CH_EP_TYPE_INTR) && (usbd_intr_out_only == 0) && (usbd_intr_in_only == 0)) ||
			((ep_infor->ep_type == USB_CH_EP_TYPE_ISOC) && (usbd_isoc_out_only == 0) && (usbd_isoc_in_only == 0))) {
			cmd_usbd_verify_loopback(ep_infor->match_addr, buf, len);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "Loopback: RX unknown EP %02x\n", ep_infor->ep_addr);
	}
	return HAL_OK;
}

/*
	bind the two eps, one is in, and another one is out
	the device will do the transaction to the IN EP while getting the data from OUT EP
*/
static int cmd_usbd_verify_add_ep_bind(u8 ep1, u8 ep2)
{
	int ret = HAL_ERR_PARA;
	u8 ep_index;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	usbd_verify_ep_t *p_ep1 = NULL, *p_ep2 = NULL;

	/* Loop All EP */
	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		if (ep1 == cdev->ep_array[ep_index].ep_infor.ep_addr) {
			p_ep1 = cdev->ep_array + ep_index;
		}
		if (ep2 == cdev->ep_array[ep_index].ep_infor.ep_addr) {
			p_ep2 = cdev->ep_array + ep_index;
		}
		if (p_ep1 && p_ep2) {
			break;
		}
	}

	if (p_ep1 && p_ep2) {
		p_ep1->ep_infor.match_addr = ep2;
		p_ep2->ep_infor.match_addr = ep1;
		ret = HAL_OK;
	}
	return ret;
}

static void cmd_usbd_verify_in_xfer(u8 ep_type, u8 in_only, u32 in_len)
{
	u8 ep_index;
	usbd_verify_ep_t *ep;
	usbd_verify_ep_basic_t *ep_infor;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	/* Loop All EP */
	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		ep = cdev->ep_array + ep_index;
		ep_infor = &(ep->ep_infor);
		if (USB_EP_IS_IN(ep_infor->ep_addr) && (ep_infor->ep_type == ep_type) && (in_only)) {
			ep->ep_infor.trans_len = in_len;/*len cant be larger than in_buf size*/
			usbd_verify_transmit_data(ep);
		}
	}
}

static int cmd_usbd_verify_add_ep(u8 epaddr, u8 type, u8 intervalue, u16 mps, u16 transsize, u8 matchep)
{
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;

	if (USBD_EP_COUNT_MAX <= 1 + cdev->ep_count) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "More than support %d EPs\n", USBD_EP_COUNT_MAX);
		return HAL_ERR_MEM;
	}

	if (HAL_OK != usbd_verify_ep_init(&(cdev->ep_array[cdev->ep_count]), type,
									  epaddr, intervalue, mps, transsize, matchep)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Init EP%x fail\n", epaddr);
		return HAL_ERR_MEM;
	}
	cdev->ep_count ++;
	return HAL_OK;
}

static int cmd_usbd_verify_eparray_deinit(void)
{
	u8 ep_index;
	usbd_verify_ep_t *ep;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		ep = cdev->ep_array + ep_index;
		usbd_verify_ep_deinit(ep);
	}
	cdev->ep_count = 0;

	return HAL_OK;
}

//for different chip,load the default config
static int cmd_usbd_verify_default_ep_init(const char *chip)
{
	int status = HAL_ERR_PARA;
	usbd_verify_ep_basic_t *p_ep = NULL;

	if ((chip && _stricmp(chip, "smart") == 0) || (chip && _stricmp(chip, "green2") == 0)) {
		if (cmd_usbd_verify_cfg.speed == USB_SPEED_HIGH) {
			p_ep = usbd_ep_hs;
		} else {
			p_ep = usbd_ep_fs;
		}
	}
	if (NULL == p_ep) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid Chip\n");
		return status;
	}
	if (chip) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Chip(%s) test\n", chip);
	}
	for (; (p_ep != NULL) && (p_ep->ep_addr != 0); p_ep++) {
		status = cmd_usbd_verify_add_ep(p_ep->ep_addr, p_ep->ep_type, p_ep->ep_interval, p_ep->mps, p_ep->trans_len, p_ep->match_addr);
		if (HAL_OK != status) {
			break;
		}
	}
	if (HAL_OK != status) {
		cmd_usbd_verify_eparray_deinit();
	} else {
		cmd_usbd_verify_dump_ep_info();
	}

	return status;
}

static void cmd_usbd_verify_ep_status_thread(void *param)
{
	UNUSED(param);
	u16 loop = 0;
	u8 ep_index;
	usbd_verify_ep_t *ep;
	usbd_verify_ep_basic_t *ep_infor;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	usbd_verify_ep_status_flag = 1;

	do {
		loop ++;
		if (loop >= 1) {
			loop = 0;
			RTK_LOGS(TAG, RTK_LOG_INFO, "OUT/IN complete issue: \n");

			/* Loop All EP */
			RTK_LOGS(TAG, RTK_LOG_INFO, "\tCTRL OUT(done%d)\n", cdev->ctrl_out_done_count);
			RTK_LOGS(TAG, RTK_LOG_INFO, "\tCTRL  IN(done%d)\n", cdev->ctrl_in_done_count);
			for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
				ep = cdev->ep_array + ep_index;
				ep_infor = &(ep->ep_infor);
				if (USB_EP_IS_IN(ep_infor->ep_addr)) {
					RTK_LOGS(TAG, RTK_LOG_INFO, "\t%s  IN(send%d/done%d/drop%d)\n", usbd_verify_get_xfer_type_text(ep_infor->ep_type),
							 ep->send_count, ep->done_count, ep->drop_count);
				} else {
					RTK_LOGS(TAG, RTK_LOG_INFO, "\t%s OUT(done%d)\n", usbd_verify_get_xfer_type_text(ep_infor->ep_type), ep->done_count);
				}
			}

			RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
		}
		rtos_time_delay_ms(10 * 1000); //1s*n
	} while (usbd_verify_ep_status_flag);

	usbd_verify_ep_status_flag = 0;
	rtos_task_delete(NULL);
}

static int cmd_usbd_verify_dump_thread(void)
{
	int status;
	void *task;

	if (usbd_verify_ep_status_flag) {
		return HAL_OK;
	}

	RTK_LOGS(TAG, RTK_LOG_INFO, "USBD EP status dump test\n");
	status = rtos_task_create(&task, "usbd_ep_status_thread", cmd_usbd_verify_ep_status_thread, NULL, 1024, 2);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create status thread fail\n");
		usbd_verify_ep_status_flag = 0;
		return status;
	}

	return HAL_OK;
}

#if CONFIG_USBD_VERIFY_BULK_ASYNC_XFER
static void verify_bulk_tx_thread(void *param)
{
	UNUSED(param);
	int ret;
	usbd_verify_bulk_async_flag = 1;
	u8 ep_index;
	usbd_verify_ep_t *tx_ep = NULL;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;

	/* Find the match tx ep */
	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		if (cdev->ep_array[ep_index].ep_infor.ep_addr == VERIFY_BULK_IN_EP) {
			tx_ep = cdev->ep_array + ep_index;
			break;
		}
	}

	if (tx_ep == NULL) {
		goto exit;
	}

	for (;;) {
		if (rtos_sema_take(verify_bulk_async_xfer_sema, RTOS_SEMA_MAX_COUNT) == RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "BULK thread xfer %d, idx (%d)\n", tx_ep->ep_infor.trans_len, verify_bulk_xfer_idx);
			if (async_bulk_fifo_write != async_bulk_fifo_read) {
				usb_os_memcpy((void *)tx_ep->buf, (void *)verify_bulk_async_xfer_fifo[async_bulk_fifo_read], tx_ep->ep_infor.trans_len);
				ret = usbd_verify_transmit_data(tx_ep);
				async_bulk_fifo_read = (async_bulk_fifo_read + 1) % CONFIG_VERIFY_BULK_ASYNC_FIFO_CNT;
				if (ret == HAL_OK) {
					verify_bulk_xfer_idx++;
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "BULK thread xfer busy, drop (%d)\n", verify_bulk_xfer_idx);
				}
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "BULK async fifo empty\n");
			}
		}
	}

exit:
	usbd_verify_bulk_async_flag = 0;
	rtos_task_delete(NULL);
}

static int cmd_usbd_verify_bulk_tx_thread(void)
{
	int ret;
	rtos_task_t xfer_task;

	if (usbd_verify_bulk_async_flag) {
		return HAL_OK;
	}

	rtos_sema_create(&verify_bulk_async_xfer_sema, 0, 1);

	ret = rtos_task_create(&xfer_task, "verify_bulk_tx_thread", verify_bulk_tx_thread, NULL, 1024, CONFIG_VERIFY_BULK_XFER_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create bulk async tx thread fail\n");
		rtos_sema_delete(verify_bulk_async_xfer_sema);
		usbd_verify_bulk_async_flag = 0;
		return ret;
	}

	return HAL_OK;
}
#endif

void cmd_usbd_verify_ep_debug_dump(void)
{
	u8 ep_index;
	usbd_verify_ep_t *ep;
	usbd_verify_ep_basic_t *ep_infor;
	cmd_usbd_verify_ep_t *cdev = &cmd_usbd_verify_ep;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Dump EP cnt=%d\n", cdev->ep_count);
	RTK_LOGS(TAG, RTK_LOG_INFO, "\tCTRL OUT(done%d)\n", cdev->ctrl_out_done_count);
	RTK_LOGS(TAG, RTK_LOG_INFO, "\tCTRL  IN(done%d)\n", cdev->ctrl_in_done_count);

	for (ep_index = 0; ep_index < cdev->ep_count; ep_index++) {
		ep = cdev->ep_array + ep_index;
		ep_infor = &(ep->ep_infor);

		if (USB_EP_IS_IN(ep_infor->ep_addr)) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\t%s IN(send%d/done%d/drop%d)", usbd_verify_get_xfer_type_text(ep_infor->ep_type),
					 ep->send_count, ep->done_count, ep->drop_count);
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "\t%s OUT(done%d)", usbd_verify_get_xfer_type_text(ep_infor->ep_type), ep->done_count);
		}

		RTK_LOGS(NOTAG, RTK_LOG_INFO, "EP%02x/%02x:mps(%d)/xlen%d/state(%d)zlp(%d)\n",
				 ep_infor->ep_addr, ep_infor->match_addr,
				 ep_infor->mps, ep_infor->trans_len, ep->state, ep->zlp);
	}

}

static void usbd_verify_xfer_dir(u8 *argv[])
{
	const char *cmd = (const char *)argv[2];

	if (_stricmp(cmd, "bulk_out") == 0) {
		usbd_bulk_out_only = 1;
		if (argv[3]) {
			usbd_bulk_out_only = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
	} else if (_stricmp(cmd, "intr_out") == 0) {
		usbd_intr_out_only = 1;
		if (argv[3]) {
			usbd_intr_out_only = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
	} else if (_stricmp(cmd, "isoc_out") == 0) {
		usbd_intr_out_only = 1;
		if (argv[3]) {
			usbd_intr_out_only = (u16)_strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
	} else if (_stricmp(cmd, "isoc_in_len") == 0) {
		if (argv[3]) {
			usbd_isoc_in_len = _strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbd_isoc_in_len) {
			usbd_isoc_in_only = 1;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "IsocInLen: %d\n", usbd_isoc_in_len);
	} else if (_stricmp(cmd, "intr_in_len") == 0) {
		if (argv[3]) {
			usbd_intr_in_len = _strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbd_intr_in_len) {
			usbd_intr_in_only = 1;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "IntrInLen: %d\n", usbd_intr_in_len);

	} else if (_stricmp(cmd, "bulk_in_len") == 0) {
		if (argv[3]) {
			usbd_bulk_in_len = _strtoul((const char *)(argv[3]), (char **)NULL, 10);
		}
		if (usbd_bulk_in_len) {
			usbd_bulk_in_only = 1;
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "BulkInLen: %d\n", usbd_bulk_in_len);
	} else if (_stricmp(cmd, "bulk_in_start") == 0) {
		cmd_usbd_verify_in_xfer(USB_CH_EP_TYPE_BULK, usbd_bulk_in_only, usbd_bulk_in_len);
	} else if (_stricmp(cmd, "intr_in_start") == 0) {
		cmd_usbd_verify_in_xfer(USB_CH_EP_TYPE_INTR, usbd_intr_in_only, usbd_intr_in_len);
	} else if (_stricmp(cmd, "isoc_in_start") == 0) {
		cmd_usbd_verify_in_xfer(USB_CH_EP_TYPE_ISOC, usbd_isoc_in_only, usbd_isoc_in_len);
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "BulkInLen: %d(%d-%d)\n", usbd_bulk_in_len, usbd_bulk_in_only, usbd_bulk_out_only);
		RTK_LOGS(TAG, RTK_LOG_INFO, "IntrInLen: %d(%d-%d)\n", usbd_intr_in_len, usbd_intr_in_only, usbd_intr_out_only);
		RTK_LOGS(TAG, RTK_LOG_INFO, "IsocInLen: %d(%d-%d)\n", usbd_isoc_in_len, usbd_isoc_in_only, usbd_isoc_out_only);
	}
}

/*
	usbd verify test entry:

	usbd verify dma dis/en(default)
	usbd verify speed full/high_in_full/high(default)
	usbd verify ep_default <chiptype> 	//default ep config group
	usbd verify set_ep ......        	//manually set ep config
	usbd verify loopback ep1 ep2

	usbd verify start
	usbd verify stop
	usbd verify dump

	usbd verify xfer bulk_out/intr_out/isoc_out <0/1> // OUT only xfer
	usbd verify xfer bulk_in_len/intr_in_len/isoc_in_len <len>
	usbd verify xfer bulk_in_start/intr_in_start/isoc_in_start //prepare IN only xfer
*/
int cmd_usbd_verify_test_entry(
	IN  u16 argc,
	IN  u8  *argv[])
{
	int status = HAL_OK;
	const char *sub_cmd;

	if (argc < 2) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid arguments, usage:\n");
		RTK_LOGS(TAG, RTK_LOG_ERROR, " usbd verify subcmd xxxx\n");
		return HAL_ERR_PARA;
	}

	//usbd_verify_enable_dump_debug(1);

	sub_cmd = (const char *)argv[1];
	RTK_LOGS(TAG, RTK_LOG_INFO, "Enter subcmd=%s\n", sub_cmd);
	if (_stricmp(sub_cmd, "dump") == 0) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "DMA: %d\n", cmd_usbd_verify_cfg.dma_enable);
		RTK_LOGS(TAG, RTK_LOG_INFO, "Speed: %d\n", cmd_usbd_verify_cfg.speed);
		RTK_LOGS(TAG, RTK_LOG_INFO, "ExtIntr: %x\n", cmd_usbd_verify_cfg.ext_intr_en);
		RTK_LOGS(TAG, RTK_LOG_INFO, "BulkInLen: %d(%d-%d)\n", usbd_bulk_in_len, usbd_bulk_in_only, usbd_bulk_out_only);
		RTK_LOGS(TAG, RTK_LOG_INFO, "IntrInLen: %d(%d-%d)\n", usbd_intr_in_len, usbd_intr_in_only, usbd_intr_out_only);
		RTK_LOGS(TAG, RTK_LOG_INFO, "IsocInLen: %d(%d-%d)\n", usbd_isoc_in_len, usbd_isoc_in_only, usbd_isoc_out_only);
		cmd_usbd_verify_ep_debug_dump();
	} else if (_stricmp(sub_cmd, "dma") == 0) {
		u8 dma_en = 0;
		if (argv[2] && (_stricmp((const char *)argv[2], (const char *)"en") == 0)) {
			dma_en = 1;
		} else if (argv[2] && (_stricmp((const char *)argv[2], (const char *)"dis") == 0)) {
			dma_en = 0;
		}
		cmd_usbd_verify_cfg.dma_enable = dma_en;
	} else if (_stricmp(sub_cmd, "speed") == 0) {
		u8 speed = (u8)USB_SPEED_HIGH;
		if (argv[2] && (_stricmp((const char *)argv[2], (const char *)"full") == 0)) {
			speed = (u8)USB_SPEED_FULL;
		} else if (argv[2] && (_stricmp((const char *)argv[2], (const char *)"high_in_full") == 0)) {
			speed = (u8)USB_SPEED_HIGH_IN_FULL;
		}
		cmd_usbd_verify_cfg.speed = speed;
	} else if (_stricmp(sub_cmd, "ep_default") == 0) {
		cmd_usbd_verify_default_ep_init((const char *)argv[2]);
	} else if (_stricmp(sub_cmd, "set_ep") == 0) {
		u8 type, epaddr, intervalue;
		u16 mps, transsize;
		if (argc < 7) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid arguments, usage:\n");
			RTK_LOGS(TAG, RTK_LOG_ERROR, " usbd verify set_ep addr type interval mps transsize\n");
			status = HAL_ERR_PARA;
		} else {
			epaddr = _strtoul((const char *)(argv[2]), (char **)NULL, 10);
			type = _strtoul((const char *)(argv[3]), (char **)NULL, 10);
			intervalue = _strtoul((const char *)(argv[4]), (char **)NULL, 10);
			mps = _strtoul((const char *)(argv[5]), (char **)NULL, 10);
			transsize = _strtoul((const char *)(argv[6]), (char **)NULL, 10);

			cmd_usbd_verify_add_ep(epaddr, type, intervalue, mps, transsize, USBD_VERIFY_MATCH_EP_ADDR);
		}
	} else if (_stricmp(sub_cmd, "loopback") == 0) {
		u8 ep1 = -1, ep2 = -1;
		if (argv[2] && argv[3]) {
			ep1 = _strtoul((const char *)(argv[2]), (char **)NULL, 16);
			ep2 = _strtoul((const char *)(argv[3]), (char **)NULL, 16);
		}
		cmd_usbd_verify_add_ep_bind(ep1, ep2);
	} else if (_stricmp(sub_cmd, "start") == 0) {
		status = usbd_init(&cmd_usbd_verify_cfg);
		if (status != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init USBD fail: %d\n", status);
			return HAL_ERR_PARA;
		}
		status = usbd_verify_init(&cmd_usbd_verify_cb);
		if (status != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Init verify fail: %d\n", status);
			status = HAL_ERR_PARA;
		} else {
			cmd_usbd_verify_dump_thread();
#if CONFIG_USBD_VERIFY_BULK_ASYNC_XFER
			cmd_usbd_verify_bulk_tx_thread();
#endif
		}
	} else if (_stricmp(sub_cmd, "stop") == 0) {
		usbd_verify_ep_status_flag = 0;
#if CONFIG_USBD_VERIFY_BULK_ASYNC_XFER
		usbd_verify_bulk_async_flag = 0;
#endif
		usbd_verify_deinit();
		cmd_usbd_verify_eparray_deinit();
	} else if (_stricmp(sub_cmd, "xfer") == 0) {
		if (argv[2]) {
			usbd_verify_xfer_dir(argv);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cmd %s\n", sub_cmd);
		status = HAL_ERR_PARA;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Exit cmd\n");
	return status;
}

