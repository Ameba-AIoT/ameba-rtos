/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbh_cdc_ecm.h"
#include "usb_os.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_CDC_BUFFER_SIZE             1024
#define ECM_ENABLE_DUMP_DESCRIPYOT_PARSE 0

#define ECM_RX_IN_HIGH_TP                1
#define ECM_DMA_LARGE_BUFFER             1

/* conventional codes for class-specific descriptors */
#define USB_DT_CS_INTERFACE              0x24

#define USB_BULK_OUT_IDLE_MAX_CNT        80U
#define USB_BULK_IN_IDLE_MAX_CNT         40U
#define USB_BULK_IN_BUSY_MAX_CNT         10U

#define  USBH_ECM_FREE_MEM(x)  if(x){ usb_os_mfree(x); x = NULL;}

/* Private types -------------------------------------------------------------*/
typedef struct {
	u8  bLength;
	u8  bDescriptorType;
} usb_ecm_descriptor_header;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_cdc_ecm_attach(usb_host_t *host);
static int usbh_cdc_ecm_detach(usb_host_t *host);
static int usbh_cdc_ecm_process(usb_host_t *host, u32 msg);
static int usbh_cdc_ecm_setup(usb_host_t *host);
static void usbh_cdc_ecm_process_bulk_out(usb_host_t *host);
static void usbh_cdc_ecm_process_bulk_in(usb_host_t *host);
static void usbh_cdc_ecm_process_intr_in(usb_host_t *host);
static int usbh_cdc_acm_process_get_string(usb_host_t *host, u8 *pbuf, u16 buflen, u8 str_id);
static int usbh_cdc_ecm_process_set_alt(usb_host_t *host);
static int usbh_cdc_ecm_ctrl_setting(usb_host_t *host);
#if ECM_ENABLE_RCR_CONFIGURATION  //for 8156
static int usbh_cdc_ecm_process_get_rcr(usb_host_t *host);
static int usbh_cdc_ecm_process_set_rcr(usb_host_t *host);
#endif
#if ECM_ENABLE_FIFO_FLOW_CTRL  //for 8152
static int usbh_cdc_ecm_process_set_flow_ctrl1(usb_host_t *host);
static int usbh_cdc_ecm_process_set_flow_ctrl2(usb_host_t *host);
#endif
static int usbh_cdc_ecm_process_mac_get_lock(usb_host_t *host);
static int usbh_cdc_ecm_process_mac_set_dis_lock(usb_host_t *host);
static int usbh_cdc_ecm_process_mac_set_mac1(usb_host_t *host);
static int usbh_cdc_ecm_process_mac_set_mac2(usb_host_t *host);
static int usbh_cdc_ecm_process_mac_en_lock(usb_host_t *host);
static int usbh_cdc_ecm_process_led_set_ctrl(usb_host_t *host);

#if ECM_ENABLE_PACKETFILTER
static int usbh_cdc_ecm_process_get_statistic(usb_host_t *host);
static int usbh_cdc_ecm_process_set_packet_filter(usb_host_t *host);
static int usbh_cdc_ecm_process_set_muiticast_filter(usb_host_t *host);
#endif


/* Private variables ---------------------------------------------------------*/

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
static u8 hex_to_char(u8 hex_num)
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

static void dump_ecm_cfgdesc(void)
{
#if ECM_ENABLE_DUMP_DESCRIPYOT_PARSE
	usbh_cdc_ecm_host_t *ecm = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_comm_if_t        *pcomm_if = &(ecm->comm_if);
	usbh_cdc_ecm_data_if_t        *pdata_if = &(ecm->data_if);
	usbh_cdc_ecm_network_func_t   *pfunc_if = &(ecm->func_if);

	RTK_LOGS(TAG, RTK_LOG_INFO, "--------------------Dump Descriptor Start------------------\n");

	//dump ecm control
	RTK_LOGS(TAG, RTK_LOG_INFO, "INTR epaddr(0x%x) MPS(%d) interval(%d)\n",
			 pcomm_if->intr_in_ep, pcomm_if->intr_in_packet_size, pcomm_if->intr_ep_interval);
	RTK_LOGS(TAG, RTK_LOG_INFO, "MAC(0x%x) ethernet MPS(%d)\n",
			 pfunc_if->iMACAddress, pfunc_if->wMaxSegmentSize);
	//dump ecm data
	RTK_LOGS(TAG, RTK_LOG_INFO, "BULK in:epaddr(0x%x) MPS(=%d) out:epaddr(0x%x) MPS(%d)\n",
			 pdata_if->bulk_in_ep, pdata_if->bulk_in_packet_size,
			 pdata_if->bulk_out_ep, pdata_if->bulk_out_packet_size);

	RTK_LOGS(TAG, RTK_LOG_INFO, "---------------------Dump End-----------------------------\n");
#endif
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

/**
  * @brief	Find next standard descriptor interface
  * @param	pbuf: given buffer to find
  			len: given buffer length
  * @retval Pointer of next standard descriptor
  */
static u8 *usbh_cdc_ecm_find_next_stdesc(u8 *pbuf, u32 *len)
{
	u8 *desc = pbuf;

	while (*len > 0) {
		if (((usb_ecm_descriptor_header *)desc)->bDescriptorType == USB_DESC_TYPE_INTERFACE) {
			return (u8 *) desc;
		} else {
			*len -= ((usb_ecm_descriptor_header *)desc)->bLength;
			desc += ((usb_ecm_descriptor_header *)desc)->bLength;
		}
	}

	return NULL;
}


/**
  * @brief	Parse ecm control interface
  * @param	desc: given descriptor buffer
  			length: lenghth of given buffer
  * @retval Status
  */
static int usbh_cdc_ecm_parse_control(u8 *pbuf, u32 *length)
{
	usbh_cdc_ecm_host_t *ecm = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_comm_if_t *pcomm_if = &(ecm->comm_if);
	usbh_cdc_ecm_network_func_t *pfunc_if = &(ecm->func_if);

	u8 *desc = pbuf;
	while (1) {
		if (USB_DESC_TYPE_ENDPOINT == ((usb_ecm_descriptor_header *) desc)->bDescriptorType) {
			pcomm_if->intr_in_packet_size = ((usbh_ep_desc_t *)desc)->wMaxPacketSize;
			pcomm_if->intr_in_ep = ((usbh_ep_desc_t *)desc)->bEndpointAddress;
			pcomm_if->intr_ep_interval = usbh_get_interval(ecm->host, USB_CH_EP_TYPE_INTR, ((usbh_ep_desc_t *)desc)->bInterval);
			*length += ((usb_ecm_descriptor_header *) desc)->bLength;
			return HAL_OK;
		} else if (USB_DT_CS_INTERFACE == ((usbh_cdc_ecm_network_func_t *)desc)->bDescriptorType
				   && CDC_ECM_NETWORK_FUNC_DESCRIPTOR == ((usbh_cdc_ecm_network_func_t *)desc)->bDescriptorSubtype) {
			usb_os_memcpy(pfunc_if, desc, sizeof(usbh_cdc_ecm_network_func_t)); //Ethernet Networking Functional Descriptor
			*length += ((usb_ecm_descriptor_header *) desc)->bLength;
			desc = desc + ((usb_ecm_descriptor_header *) desc)->bLength;
		} else {
			*length += ((usb_ecm_descriptor_header *) desc)->bLength;
			desc = desc + ((usb_ecm_descriptor_header *) desc)->bLength;
		}
	}

	return HAL_ERR_PARA;
}


/**
  * @brief	Parse ecm data interface
  * @param	pbuf: given descriptor buffer
  			length: lenghth of given buffer
  			leftlen: left buffer length of the given buffer
  * @retval Status
  */
static int usbh_cdc_ecm_parse_data(u8 *pbuf, u32 *length, u32 leftlen)
{
	usbh_cdc_ecm_host_t *ecm = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_data_if_t *pdata_if = &(ecm->data_if);
	u8 *desc = pbuf;
	while (1) {
		if (USB_DESC_TYPE_INTERFACE == ((usb_ecm_descriptor_header *) desc)->bDescriptorType) {
			*length += ((usb_ecm_descriptor_header *) desc)->bLength;
			desc = desc + ((usb_ecm_descriptor_header *) desc)->bLength;
		} else if (USB_DESC_TYPE_ENDPOINT == ((usb_ecm_descriptor_header *) desc)->bDescriptorType) {
			if ((((usbh_ep_desc_t *) desc)->bEndpointAddress & USB_REQ_DIR_MASK) == USB_D2H) { //in
				pdata_if->bulk_in_ep = ((usbh_ep_desc_t *) desc)->bEndpointAddress;
				pdata_if->bulk_in_packet_size  = ((usbh_ep_desc_t *) desc)->wMaxPacketSize;
			} else { //out
				pdata_if->bulk_out_ep = ((usbh_ep_desc_t *) desc)->bEndpointAddress;
				pdata_if->bulk_out_packet_size  = ((usbh_ep_desc_t *) desc)->wMaxPacketSize;
			}

			*length += ((usb_ecm_descriptor_header *) desc)->bLength;
			desc = desc + ((usb_ecm_descriptor_header *) desc)->bLength;

			if (*length >= leftlen) {
				return HAL_OK;
			}
		} else {
			return HAL_OK;
		}
	}
	return HAL_OK;
}

static void usbh_cdc_ecm_config_dongle_mac(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	u8 req_status = HAL_OK;

	switch (cdc->sub_state) {
	case CDC_ECM_STATE_CTRL_MAC_GET_LOCK: //8152 mac
		if ((cdc->pid != 0x8152) || (cdc->vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_state++;
		} else {
			usb_os_memset(cdc->mac_ctrl_lock, 0, 4);
			req_status = usbh_cdc_ecm_process_mac_get_lock(host);
			if (req_status == HAL_OK) {
				usb_os_memcpy(cdc->mac_ctrl_lock, cdc->dongle_ctrl_buf, CDC_ECM_MAC_CTRL_REG_LEN);
				cdc->sub_state++;
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get MAC lock err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK: //8152 mac
		if ((cdc->pid != 0x8152) || (cdc->vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_state++;
		} else {
			cdc->mac_ctrl_lock[0] = 0xD0;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->mac_ctrl_lock, CDC_ECM_MAC_CTRL_REG_LEN);
			req_status = usbh_cdc_ecm_process_mac_set_dis_lock(host);
			if (req_status == HAL_OK) {
				cdc->sub_state++;
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Dis MAC lock err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC1: //8152 mac
		if ((cdc->pid != 0x8152) || (cdc->vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_state++;
		} else {
			usb_os_memcpy(cdc->dongle_ctrl_buf, &(cdc->mac[0]), CDC_ECM_MAC_CTRL_REG_LEN);
			req_status = usbh_cdc_ecm_process_mac_set_mac1(host);
			if (req_status == HAL_OK) {
				cdc->sub_state++;
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC1 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_SET_MAC2: //8152 mac
		if ((cdc->pid != 0x8152) || (cdc->vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_state++;
		} else {
			usb_os_memcpy(cdc->dongle_ctrl_buf, &(cdc->mac[4]), CDC_ECM_MAC_CTRL_REG_LEN);
			cdc->dongle_ctrl_buf[2] = cdc->dongle_ctrl_buf[3] = 0xFF;
			req_status = usbh_cdc_ecm_process_mac_set_mac2(host);
			if (req_status == HAL_OK) {
				cdc->sub_state++;
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set MAC2 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK: //8152 mac
		if ((cdc->pid != 0x8152) || (cdc->vid != 0x0BDA) || (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY)) {
			cdc->sub_state++;
		} else {
			cdc->mac_ctrl_lock[0] = 0x10;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->mac_ctrl_lock, CDC_ECM_MAC_CTRL_REG_LEN);
			req_status = usbh_cdc_ecm_process_mac_en_lock(host);
			if (req_status == HAL_OK) {
				cdc->sub_state++;
				cdc->mac_valid = 1;
				RTK_LOGS(TAG, RTK_LOG_INFO, "Mac set success \n");
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "En MAC lock err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	default:
		cdc->sub_state++;
		break;
	}
}

/**
  * @brief	Parse configuration descriptor
  * @param	host: usb host structure
  * @retval Status
  */
int usbh_cdc_ecm_parse_cfgdesc(usb_host_t *host)
{
	int ret = HAL_OK;
	usbh_cfg_desc_t *desc = (usbh_cfg_desc_t *)usbh_get_raw_configuration_descriptor(host);
	u32 cfglen = (u32)desc->wTotalLength;
	usbh_if_desc_t *pbuf = (usbh_if_desc_t *) desc;
	u32 len = 0;

	while (1) {
		pbuf = (usbh_if_desc_t *)usbh_cdc_ecm_find_next_stdesc((u8 *)pbuf, &cfglen);
		if (pbuf == NULL) {
			break;
		}

		switch (pbuf->bInterfaceClass) {
		case CDC_COMM_INTERFACE_CLASS_CODE: //cdc control
			len = 0;
			ret = usbh_cdc_ecm_parse_control((u8 *)pbuf, &len);
			if (HAL_OK != ret) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse comm desc fail\n");
				return ret;
			}
			pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
			cfglen -= len;
			break;

		case CDC_DATA_INTERFACE_CLASS_CODE: //cdc data
			if (pbuf->bAlternateSetting == 0) { //alt setting
				len = ((usb_ecm_descriptor_header *) pbuf)->bLength;
				pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
				cfglen -= len;
			} else {
				len = 0;
				ret = usbh_cdc_ecm_parse_data((u8 *)pbuf, &len, cfglen);
				if (HAL_OK != ret) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse data fail\n");
					return ret;
				}

				pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
				cfglen -= len;
			}
			break;

		default:
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Subclass(%d) err\n", pbuf->bInterfaceClass);
			return HAL_ERR_PARA;
		}
	}

	return ret;
}


/**
  * @brief  Usb Attach callback function
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_attach(usb_host_t *host)
{
	int status = HAL_ERR_UNKNOWN;
	usbh_dev_desc_t *pdesc;
	u8 pipe_num;
	u32 max_ep_size;

	usbh_cdc_ecm_host_t 		  *ecm = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_comm_if_t        *pcomm_if = &(ecm->comm_if);
	usbh_cdc_ecm_data_if_t        *pdata_if = &(ecm->data_if);
	ecm->usbh_state = USBH_CDC_ECM_ATTACH;
	ecm->host = host;

	if (host->config.speed == USB_SPEED_HIGH) {
		max_ep_size = USB_HS_MAX_PACKET_SIZE;
	} else {
		max_ep_size = USB_FS_MAX_PACKET_SIZE;
	}

	pdesc = usbh_get_device_descriptor(ecm->host);
	ecm->vid = pdesc->idVendor;
	ecm->pid = pdesc->idProduct;

	status = usbh_cdc_ecm_parse_cfgdesc(host);
	if (status) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Parse cfg desc fail\n");
		return status;
	}
	dump_ecm_cfgdesc();

	//limited the max ep size
	if (pdata_if->bulk_in_packet_size >= max_ep_size) {
		pdata_if->bulk_in_packet_size = max_ep_size;
	}
	if (pdata_if->bulk_out_packet_size >= max_ep_size) {
		pdata_if->bulk_out_packet_size = max_ep_size;
	}
	if (pcomm_if->intr_in_packet_size >= max_ep_size) {
		pcomm_if->intr_in_packet_size = max_ep_size;
	}

	//control
	pipe_num = usbh_alloc_pipe(host, pcomm_if->intr_in_ep);
	if (pipe_num != 0xFFU) {
		pcomm_if->intr_in_pipe = pipe_num;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc INTR in pipe fail");
		return HAL_ERR_MEM;
	}
	usbh_open_pipe(host,
				   pcomm_if->intr_in_pipe,
				   pcomm_if->intr_in_ep,
				   USB_CH_EP_TYPE_INTR,
				   pcomm_if->intr_in_packet_size);

	//data
	pipe_num = usbh_alloc_pipe(host, pdata_if->bulk_out_ep);
	if (pipe_num != 0xFFU) {
		pdata_if->bulk_out_pipe = pipe_num;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc BULK out pipe fail");
		usbh_free_pipe(host, pcomm_if->intr_in_pipe);
		return HAL_ERR_MEM;
	}

	pipe_num = usbh_alloc_pipe(host, pdata_if->bulk_in_ep);
	if (pipe_num != 0xFFU) {
		pdata_if->bulk_in_pipe = pipe_num;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc BULK in pipe fail");
		usbh_free_pipe(host, pcomm_if->intr_in_pipe);
		usbh_free_pipe(host, pdata_if->bulk_out_pipe);
		return HAL_ERR_MEM;
	}

	usbh_open_pipe(host,
				   pdata_if->bulk_out_pipe,
				   pdata_if->bulk_out_ep,
				   USB_CH_EP_TYPE_BULK,
				   pdata_if->bulk_out_packet_size);

	usbh_open_pipe(host,
				   pdata_if->bulk_in_pipe,
				   pdata_if->bulk_in_ep,
				   USB_CH_EP_TYPE_BULK,
				   pdata_if->bulk_in_packet_size);

	//dump ecm data
	//RTK_LOGS(TAG, RTK_LOG_INFO, "[in:epaddr=0x%x,chan=%d,packsize=%d] [out:epaddr=0x%x,chan=%d,packsize=%d]\n",
	//		pdata_if->bulk_in_ep, pdata_if->bulk_in_pipe, pdata_if->bulk_in_packet_size,
	//		pdata_if->bulk_out_ep, pdata_if->bulk_out_pipe, pdata_if->bulk_in_packet_size);

	ecm->state = CDC_ECM_STATE_IDLE;

	if ((ecm->cb != NULL) && (ecm->cb->attach != NULL)) {
		ecm->cb->attach();
	}
	status = HAL_OK;
	return status;
}


/**
  * @brief  Usb Detach callback function.
  * @param  host: Host handle
  * @retval HAL_OK
  */
static int usbh_cdc_ecm_detach(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	cdc->state = CDC_ECM_STATE_IDLE;
	cdc->usbh_state = USBH_CDC_ECM_DETACHED;

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
	u8 i = 0;
	u8 mac_is_valid = 0;
	int status = HAL_ERR_UNKNOWN;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/*Issue the get line coding request*/
	if (cdc->mac_src_type == CDC_ECM_MAC_UPPER_LAYER_SET) {
		status = HAL_OK;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Upper set mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4], cdc->mac[5]);
	} else {
		status = usbh_cdc_acm_process_get_string(host, cdc->dongle_ctrl_buf, CDC_ECM_MAC_STRING_LEN, cdc->func_if.iMACAddress);
		cdc->mac_src_type = CDC_ECM_MAC_DONGLE_SUPPLY;
	}

	if (status == HAL_OK) {
		if (cdc->mac_src_type == CDC_ECM_MAC_DONGLE_SUPPLY) {
			for (i = 0; i < 6; i++) {
				cdc->mac[i] = hex_to_char(cdc->dongle_ctrl_buf[2 + 4 * i]) * 16 + hex_to_char(cdc->dongle_ctrl_buf[2 + 4 * i + 2]);
				if (cdc->mac[i]) {
					mac_is_valid = 1;
				}
			}
			if (mac_is_valid == 0) {
				TRNG_get_random_bytes(cdc->mac, CDC_ECM_MAC_STR_LEN);
				RTK_LOGS(TAG, RTK_LOG_INFO, "Random mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4], cdc->mac[5]);
				cdc->mac_src_type = CDC_ECM_MAC_RANDOM_SET;
			} else {
				RTK_LOGS(TAG, RTK_LOG_INFO, "Dongle mac[%02x %02x %02x %02x %02x %02x]\n", cdc->mac[0], cdc->mac[1], cdc->mac[2], cdc->mac[3], cdc->mac[4], cdc->mac[5]);
				cdc->mac_valid = 1;
			}
		}

		cdc->usbh_state = USBH_CDC_ECM_SETUP;

		/* post to loop main process */
		cdc->state = CDC_ECM_STATE_PRE_SETTING;
		cdc->sub_state = CDC_ECM_STATE_CTRL_ALT_SETTING;
		usbh_notify_class_state_change(host, 0);

		if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
			cdc->cb->setup();
		}
	}

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
		req_status = usbh_cdc_ecm_ctrl_setting(host);
		if (req_status == HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "ECM alt setting finish\n");
			cdc->state = CDC_ECM_STATE_TRANSFER;
		}
		usbh_notify_class_state_change(host, cdc->comm_if.intr_in_pipe);
		break;

	case CDC_ECM_STATE_TRANSFER:
#if ECM_STATE_DEBUG_ENABLE
		cdc->loop_cnt++;
#endif
		if (msg == cdc->data_if.bulk_in_pipe) {
#if ECM_STATE_DEBUG_ENABLE
			cdc->bulk_in++;
#endif
			cdc->next_transfor = 0;
			usbh_cdc_ecm_process_bulk_in(host);
			if (cdc->next_transfor) {
				usbh_notify_class_state_change(host, cdc->data_if.bulk_in_pipe);
			}
		}

		if (msg == cdc->data_if.bulk_out_pipe) {
#if ECM_STATE_DEBUG_ENABLE
			cdc->bulk_out++;
#endif
			cdc->next_transfor = 0;
			usbh_cdc_ecm_process_bulk_out(host);
			if (cdc->next_transfor) {
				usbh_notify_class_state_change(host, cdc->data_if.bulk_out_pipe);
			}
		}

		if (msg == cdc->comm_if.intr_in_pipe) {
#if ECM_STATE_DEBUG_ENABLE
			cdc->intr_in++;
#endif
			cdc->next_transfor = 0;
			usbh_cdc_ecm_process_intr_in(host);
			if (cdc->next_transfor) {
				usbh_notify_class_state_change(host, cdc->comm_if.intr_in_pipe);
			}
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

static int usbh_cdc_ecm_ctrl_setting(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	u8 req_status = HAL_OK;
	u8 ret = HAL_BUSY;
#if ECM_ENABLE_PACKETFILTER
	u8 mac_str[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

	switch (cdc->sub_state) {
	case CDC_ECM_STATE_CTRL_IDLE:
		usb_os_sleep_ms(1000);
		break;
	case CDC_ECM_STATE_CTRL_ALT_SETTING:
		req_status = usbh_cdc_ecm_process_set_alt(host);
		if (req_status == HAL_OK) {
			cdc->sub_state++;
		} else if (req_status != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM alt setting err\n");
			usb_os_sleep_ms(100);
		}
		break;

#if ECM_ENABLE_PACKETFILTER
	case CDC_ECM_STATE_CTRL_SET_ETHERNET_MULTICAST_FILTER:
		usb_os_memcpy(cdc->muticast_filter, mac_str, 6);
		cdc->muticast_filter_len = 6;
		req_status = usbh_cdc_ecm_process_set_muiticast_filter(host);
		if (req_status == HAL_OK) {
			cdc->sub_state++;
		} else if (req_status != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM set eth multicast filter err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_SET_ETHERNET_PACKET_FILTER:
		cdc->packet_filter = CDC_ECM_ETH_PACKET_TYPE_DIRECTED;
		req_status = usbh_cdc_ecm_process_set_packet_filter(host);
		if (req_status == HAL_OK) {
			cdc->sub_state++;
		} else if (req_status != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM set eth packet filter err\n");
			usb_os_sleep_ms(100);
		}
		break;

	case CDC_ECM_STATE_CTRL_GET_ETHERNET_STATISTIC:
		req_status = usbh_cdc_ecm_process_get_statistic(host);
		if (req_status == HAL_OK) {
			cdc->sub_state++;
		} else if (req_status != HAL_BUSY) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "ECM get eth statistic err\n");
			usb_os_sleep_ms(100);
		}
		break;
#endif

#if ECM_ENABLE_RCR_CONFIGURATION   ///just for 8156
	case CDC_ECM_STATE_CTRL_RCR_GET:
		if ((cdc->pid != 0x8156) || (cdc->vid != 0x0BDA)) {
			cdc->sub_state++;
		} else {
			usb_os_memset(cdc->rcr, 0, 4);
			req_status = usbh_cdc_ecm_process_get_rcr(host);
			if (req_status == HAL_OK) {
				usb_os_memcpy(cdc->rcr, cdc->dongle_ctrl_buf, CDC_ECM_MAC_CTRL_REG_LEN);
				cdc->sub_state++;
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Get RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_CTRL_RCR_SET:
		if ((cdc->pid != 0x8156) || (cdc->vid != 0x0BDA)) {
			cdc->sub_state++;
		} else {
			//set bit 0~3 set 1 will enable PING & UDP transfer
			cdc->rcr[0] = cdc->rcr[0] | 0x0F;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->rcr, CDC_ECM_MAC_CTRL_REG_LEN);
			req_status = usbh_cdc_ecm_process_set_rcr(host);
			if (req_status == HAL_OK) {
				cdc->sub_state++;
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set RCR err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;
#endif

#if ECM_ENABLE_FIFO_FLOW_CTRL   ///just for 8152
	case CDC_ECM_STATE_FLOW_CTRL1:
		if ((cdc->pid != 0x8152) || (cdc->vid != 0x0BDA)) {
			cdc->sub_state++;
		} else {
			usb_os_memset(cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0x60;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->flow_ctrl, CDC_ECM_MAC_CTRL_REG_LEN);
			req_status = usbh_cdc_ecm_process_set_flow_ctrl1(host);
			if (req_status == HAL_OK) {
				cdc->sub_state++;
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set flow ctrl1 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	case CDC_ECM_STATE_FLOW_CTRL2:
		if ((cdc->pid != 0x8152) || (cdc->vid != 0x0BDA)) {
			cdc->sub_state++;
		} else {
			usb_os_memset(cdc->flow_ctrl, 0, 4);
			cdc->flow_ctrl[0] = 0xa0;
			usb_os_memcpy(cdc->dongle_ctrl_buf, cdc->flow_ctrl, CDC_ECM_MAC_CTRL_REG_LEN);
			req_status = usbh_cdc_ecm_process_set_flow_ctrl2(host);
			if (req_status == HAL_OK) {
				cdc->sub_state++;
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set flow ctrl1 err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;
#endif

	case CDC_ECM_STATE_CTRL_MAC_GET_LOCK:
	case CDC_ECM_STATE_CTRL_MAC_DISABLE_LOCK:
	case CDC_ECM_STATE_CTRL_MAC_SET_MAC1:
	case CDC_ECM_STATE_CTRL_MAC_SET_MAC2:
	case CDC_ECM_STATE_CTRL_MAC_ENABLE_LOCK:
		usbh_cdc_ecm_config_dongle_mac(host);
		break;

	case CDC_ECM_STATE_CTRL_LED_COLOR_SET: //8152 led ctrl
		if ((cdc->pid != 0x8152) || (cdc->vid != 0x0BDA) || (cdc->led_cnt == 0) || (cdc->led_array == NULL)) {
			cdc->sub_state++;
		} else {
			usb_os_memset(cdc->dongle_ctrl_buf, 0xFF, 4);
			usb_os_memcpy(cdc->dongle_ctrl_buf, (u8 *) & (cdc->led_array[0]), 2);
			req_status = usbh_cdc_ecm_process_led_set_ctrl(host);
			if (req_status == HAL_OK) {
				cdc->sub_state++;
			} else if (req_status != HAL_BUSY) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Set led color err\n");
				usb_os_sleep_ms(100);
			}
		}
		break;

	default:
		ret = HAL_OK;
		break;
	}

	return ret;
}

/**
  * @brief  Usb alt setting function
  * @param  host: Host handle
  * @retval Status
*/
static int usbh_cdc_ecm_process_set_alt(usb_host_t *host)
{
	usbh_setup_req_t setup;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_STANDARD | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = USB_REQ_SET_INTERFACE;
	setup.b.wValue = 1;
	setup.b.wIndex = 1;
	setup.b.wLength = 0U;

	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  Bulk Out handling function
  * @param  host: Host handle
  * @retval Status
*/
static void usbh_cdc_ecm_process_bulk_out(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;

	switch (cdc->bulk_data_out_state) {
	case CDC_ECM_TRANSFER_STATE_XFER:
#if ECM_DMA_LARGE_BUFFER
		usbh_bulk_send_data(host,
							cdc->bulk_data_out_buf,
							cdc->bulk_data_out_len,
							cdc->data_if.bulk_out_pipe);
#else
		//mps
		if (cdc->bulk_data_out_len > cdc->data_if.bulk_out_packet_size) {
			usbh_bulk_send_data(host,
								cdc->bulk_data_out_buf,
								cdc->data_if.bulk_out_packet_size,
								cdc->data_if.bulk_out_pipe);
		} else {
			usbh_bulk_send_data(host,
								cdc->bulk_data_out_buf,
								(u16)cdc->bulk_data_out_len,
								cdc->data_if.bulk_out_pipe);
		}
#endif
		cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_BUSY;
		cdc->bulk_out_idle_tick = usbh_get_current_tick(host);
		cdc->next_transfor = 1;
		break;

	case CDC_ECM_TRANSFER_STATE_BUSY:
		urb_state = usbh_get_urb_state(host, cdc->data_if.bulk_out_pipe);
		if (urb_state == USBH_URB_DONE) {
#if ECM_DMA_LARGE_BUFFER
			if (cdc->bulk_out_zlp) { //ZLP
				cdc->bulk_out_zlp = 0U;
				cdc->bulk_data_out_len = 0U;
				cdc->bulk_data_out_buf = NULL;
				cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_XFER;
				cdc->next_transfor = 1;
			} else {
				cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_IDLE;
				if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
					cdc->cb->bulk_send(urb_state);
				}
			}
#else
			if (cdc->bulk_data_out_len >= cdc->data_if.bulk_out_packet_size) {
				cdc->bulk_data_out_len -= cdc->data_if.bulk_out_packet_size;
				cdc->bulk_data_out_buf += cdc->data_if.bulk_out_packet_size;
				cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_XFER;
				cdc->next_transfor = 1;
			} else {
				cdc->bulk_data_out_len = 0U;
				cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_IDLE;
				if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
					cdc->cb->bulk_send(urb_state);
				}
			}
#endif
		} else if (urb_state == USBH_URB_BUSY) { //retransmit
			// cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->next_transfor = 1;
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_IDLE;
			if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
				cdc->cb->bulk_send(urb_state);
			}
			RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK out failed %d\n", urb_state);
		} else if (urb_state == USBH_URB_IDLE) {
			if (usbh_get_elapsed_ticks(host, cdc->bulk_out_idle_tick) >= (USB_BULK_OUT_IDLE_MAX_CNT)) {
				cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_XFER;
				cdc->next_transfor = 1;
			}
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
static void usbh_cdc_ecm_process_bulk_in(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (cdc->bulk_data_in_state) {
	case CDC_ECM_TRANSFER_STATE_XFER:
		usbh_bulk_receive_data(host,
							   cdc->bulk_data_in_buf,
							   USBH_CDC_ECM_BULK_BUF_MAX_SIZE,
							   cdc->data_if.bulk_in_pipe);
		cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_BUSY;
		cdc->next_transfor = 1;
		cdc->bulk_in_idle_tick = usbh_get_current_tick(host);
		cdc->bulk_in_busy_tick = usbh_get_current_tick(host);
		break;

	case CDC_ECM_TRANSFER_STATE_BUSY:
		urb_state = usbh_get_urb_state(host, cdc->data_if.bulk_in_pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, cdc->data_if.bulk_in_pipe);
			if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL)) {
				cdc->cb->bulk_received(cdc->bulk_data_in_buf, len);
			}

#if ECM_RX_IN_HIGH_TP
			usbh_bulk_receive_data(host,
								   cdc->bulk_data_in_buf,
								   USBH_CDC_ECM_BULK_BUF_MAX_SIZE,
								   cdc->data_if.bulk_in_pipe);
			cdc->bulk_in_idle_tick = usbh_get_current_tick(host);
			cdc->bulk_in_busy_tick = usbh_get_current_tick(host);
			cdc->next_transfor = 1;
#else
			cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
#endif
		} else if (urb_state == USBH_URB_BUSY) {
			if (usbh_get_elapsed_ticks(host, cdc->bulk_in_busy_tick) >= USB_BULK_IN_BUSY_MAX_CNT) {
				cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
				cdc->next_transfor = 1;
			}
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
#if ECM_RX_IN_HIGH_TP
			cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->next_transfor = 1;
#else
			cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
#endif
			RTK_LOGS(TAG, RTK_LOG_ERROR, "BULK in failed %d\n", urb_state);
		} else if (urb_state == USBH_URB_IDLE) {
			if (usbh_get_elapsed_ticks(host, cdc->bulk_in_idle_tick) >= (USB_BULK_IN_IDLE_MAX_CNT)) {
				cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_XFER;
				cdc->next_transfor = 1;
			}
		}
		break;

	default:
		cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_XFER;
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
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (cdc->intr_in_state) {
	case CDC_ECM_TRANSFER_STATE_XFER:
		usbh_intr_receive_data(host,
							   cdc->intr_in_buf,
							   cdc->comm_if.intr_in_packet_size,
							   cdc->comm_if.intr_in_pipe);

		cdc->intr_in_idle_tick = usbh_get_current_tick(host);
		cdc->intr_in_busy_tick = usbh_get_current_tick(host);
		cdc->intr_in_state = CDC_ECM_TRANSFER_STATE_BUSY;
		cdc->next_transfor = 1;
		break;

	case CDC_ECM_TRANSFER_STATE_BUSY:
		urb_state = usbh_get_urb_state(host, cdc->comm_if.intr_in_pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, cdc->comm_if.intr_in_pipe);
			if (len && (cdc->cb != NULL) && (cdc->cb->intr_received != NULL)) {
				cdc->cb->intr_received(cdc->intr_in_buf, len);
			}
			if (len > cdc->comm_if.intr_in_packet_size) {
				cdc->intr_in_state = CDC_ECM_TRANSFER_STATE_XFER;
				cdc->next_transfor = 1;
			} else {
				cdc->intr_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
			}
		} else if (urb_state == USBH_URB_BUSY) {
			cdc->next_transfor = 1;
			if (usbh_get_elapsed_ticks(host, cdc->intr_in_busy_tick) >= cdc->comm_if.intr_ep_interval) {
				//RTK_LOGS(TAG, RTK_LOG_DEBUG, "INTR in busy too long %d-%d\n", usbh_get_current_tick(host), cdc->intr_in_busy_tick);
				cdc->intr_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
				cdc->next_transfor = 1;
			}
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			cdc->intr_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "INTR in failed %d\n", urb_state);
		} else if (urb_state == USBH_URB_IDLE) {
			cdc->next_transfor = 1; //maybe long time in this issue
			if (usbh_get_elapsed_ticks(host, cdc->intr_in_idle_tick) >= cdc->comm_if.intr_ep_interval) { // 1s
				cdc->intr_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
				cdc->next_transfor = 1;
			}
		}
		break;

	default:
		break;
	}
}
/**
  * @brief  Get string descriptor by str id
  * @param  host: Host handle
  * @param  pbuf: buffer handle which will save the string value
  * @param  buflen: buffer length
  * @param  str_id: string index
  * @retval Status
  */
static int usbh_cdc_acm_process_get_string(usb_host_t *host, u8 *pbuf, u16 buflen, u8 str_id)
{
	usbh_setup_req_t setup;

	setup.b.bmRequestType = USB_D2H | USB_REQ_RECIPIENT_DEVICE | USB_REQ_TYPE_STANDARD;
	setup.b.bRequest = USB_REQ_GET_DESCRIPTOR;
	setup.b.wValue = USB_DESC_STRING | str_id;
	setup.b.wIndex = 1U;
	setup.b.wLength = buflen;

	return usbh_ctrl_request(host, &setup, pbuf);
}

#if ECM_ENABLE_PACKETFILTER
/**
  * @brief  Set Ethernet Packet Filter
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_packet_filter(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = CDC_ECM_SET_ETHERNET_PACKET_FILTER;
	setup.b.wValue = cdc->packet_filter;
	setup.b.wIndex = 1U;
	setup.b.wLength = 0;

	return usbh_ctrl_request(host, &setup, NULL);
}

/**
  * @brief  Set Ethernet Muticast Filter
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_muiticast_filter(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = CDC_ECM_SET_ETHERNET_MULTICAST_FILTERS;
	setup.b.wValue = 1;
	setup.b.wIndex = 1U;
	setup.b.wLength = cdc->muticast_filter_len;

	return usbh_ctrl_request(host, &setup, cdc->muticast_filter);
}

/**
  * @brief  Get Ethernet Statistic
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_get_statistic(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_CLASS | USB_REQ_RECIPIENT_INTERFACE;
	setup.b.bRequest = CDC_ECM_GET_ETHERNET_STATISTIC;
	setup.b.wValue = cdc->feature_selector;
	setup.b.wIndex = 1U;
	setup.b.wLength = 4;

	return usbh_ctrl_request(host, &setup, (u8 *)&cdc->eth_statistic_count);
}
#endif

#if ECM_ENABLE_RCR_CONFIGURATION  //for 8156
/**
  * @brief  Get 8156 rcr params
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_get_rcr(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xc010;
	setup.b.wIndex = 0x0100;
	setup.b.wLength = 4;

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

/**
  * @brief  Set 8156 rcr params
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_rcr(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xc010;
	setup.b.wIndex = 0x010F;
	setup.b.wLength = 4;

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}
#endif

#if ECM_ENABLE_FIFO_FLOW_CTRL  //for 8152
/**
  * @brief  Set 8152 flow ctrl params1
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_flow_ctrl1(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = 0x40;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xC0A4;
	setup.b.wIndex = 0x0103;
	setup.b.wLength = 4;
	return usbh_ctrl_request(host, &setup, (u8 *)cdc->dongle_ctrl_buf);
}
/**
  * @brief  Set 8152 flow ctrl params2
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_set_flow_ctrl2(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = 0x40;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xC0A8;
	setup.b.wIndex = 0x0103;
	setup.b.wLength = 4;
	return usbh_ctrl_request(host, &setup, (u8 *)cdc->dongle_ctrl_buf);
}
#endif

/******************Set MAC for 8152 *****************************/
/**
  * @brief  Set 8152 mac flow ctrl
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_mac_get_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = USB_D2H | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xE81C;
	setup.b.wIndex = 0x010F;
	setup.b.wLength = 4;

	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_set_dis_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = 0x40;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xE81C;
	setup.b.wIndex = 0x010F;
	setup.b.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_set_mac1(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = 0x40;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xC000;
	setup.b.wIndex = 0x010F;
	setup.b.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_set_mac2(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = 0x40;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xC004;
	setup.b.wIndex = 0x0103;
	setup.b.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

static int usbh_cdc_ecm_process_mac_en_lock(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = 0x40;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xE81C;
	setup.b.wIndex = 0x010F;
	setup.b.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

/**
  * @brief  Set led color for 8152
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process_led_set_ctrl(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = 0x40;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xDD90;
	setup.b.wIndex = 0x0103;
	setup.b.wLength = 4;
	return usbh_ctrl_request(host, &setup, cdc->dongle_ctrl_buf);
}

/**
  * @brief Check ecm bulk rx status, whether should trigger
  * @retval Status
  */
static int usbh_cdc_ecm_bulk_rx_time_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;
	usbh_urb_state_t urb_state = usbh_get_urb_state(host, cdc->data_if.bulk_in_pipe);

	if (usbh_get_elapsed_ticks(host, cdc->bulk_in_idle_tick) >= (USB_BULK_IN_IDLE_MAX_CNT)) {
		return HAL_OK;
	}

	if (usbh_get_elapsed_ticks(host, cdc->bulk_in_busy_tick) >= (USB_BULK_IN_BUSY_MAX_CNT)) {
		return HAL_OK;
	}

	if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
		return HAL_OK;
	}

	return HAL_BUSY;
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

	cdc->dongle_ctrl_buf = (u8 *)usb_os_malloc(CDC_ECM_MAC_STRING_LEN);
	if (NULL == cdc->dongle_ctrl_buf) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc mem %d fail\n", CDC_ECM_MAC_STRING_LEN);
		return HAL_ERR_MEM;
	}

	cdc->bulk_data_in_buf = (u8 *)usb_os_malloc(USBH_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (NULL == cdc->bulk_data_in_buf) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Alloc mem %d fail\n", USBH_CDC_ECM_BULK_BUF_MAX_SIZE);
		USBH_ECM_FREE_MEM(cdc->dongle_ctrl_buf);
		return HAL_ERR_MEM;
	}

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
	usb_host_t *host = cdc->host;

	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	if ((host != NULL) && (host->state == USBH_CLASS_READY)) {
		cdc->state = CDC_ECM_STATE_IDLE;

		usbh_close_pipe(host, cdc->comm_if.intr_in_pipe);
		usbh_close_pipe(host, cdc->data_if.bulk_in_pipe);
		usbh_close_pipe(host, cdc->data_if.bulk_out_pipe);
	}

	USBH_ECM_FREE_MEM(cdc->dongle_ctrl_buf);
	USBH_ECM_FREE_MEM(cdc->bulk_data_in_buf);
	USBH_ECM_FREE_MEM(cdc->led_array);

	usbh_unregister_class(&usbh_cdc_ecm_driver);

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
	usb_host_t *host = cdc->host;
	if (cdc->usbh_state < USBH_CDC_ECM_SETUP) {
		//RTK_LOGS(TAG, RTK_LOG_ERROR, "Usb Not Setup success,just return[%s]\n", __func__);
		return ret;
	}

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_TRANSFER)) {
		if (cdc->bulk_data_out_state == CDC_ECM_TRANSFER_STATE_IDLE) {
			cdc->bulk_data_out_buf = buf;
			cdc->bulk_data_out_len = len;
			if ((len % cdc->data_if.bulk_out_packet_size) == 0) { //ZLP
				cdc->bulk_out_zlp = 1;
			} else {
				cdc->bulk_out_zlp = 0;
			}
			cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, cdc->data_if.bulk_out_pipe);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to receive bulk in data
  * @param  none
  * @retval Status
  */
int usbh_cdc_ecm_bulk_receive(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}
	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if (cdc->bulk_data_in_state == CDC_ECM_TRANSFER_STATE_IDLE || (usbh_cdc_ecm_bulk_rx_time_check() == HAL_OK)) {
			if (cdc->bulk_data_in_state == CDC_ECM_TRANSFER_STATE_IDLE) {
				cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_XFER;
			}
			// cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host, cdc->data_if.bulk_in_pipe);
			ret = HAL_OK;
		}
	}

	return ret;
}

/**
  * @brief  Start to receive INTR data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbh_cdc_ecm_intr_receive(u8 *buf, u32 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_cdc_ecm_comm_if_t *pcomm_if = &(cdc->comm_if);
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}
	if (cdc->state == CDC_ECM_STATE_TRANSFER) {
		if (cdc->intr_in_state == CDC_ECM_TRANSFER_STATE_IDLE) {
			cdc->intr_in_buf = buf;
			cdc->intr_in_state = CDC_ECM_TRANSFER_STATE_XFER;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			if (len < pcomm_if->intr_in_packet_size) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Notice, Pls enlarge the inbuf size(%d-%d)\n",
						 pcomm_if->intr_in_packet_size, len);
				pcomm_if->intr_in_packet_size = len;
			}
			usbh_notify_class_state_change(host, cdc->comm_if.intr_in_pipe);
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
	return cdc->data_if.bulk_in_packet_size;
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
			ret = cdc->comm_if.intr_ep_interval / 8;
		} else {
			ret = cdc->comm_if.intr_ep_interval;
		}

		if (ret == 0) {
			ret = 1;
		}
	}

	return ret;
}

/* Exported functions --------------------------------------------------------*/
/*
	below is the public apis that called by lwip
*/
/**
  * @brief  get the mac str
  * @retval mac string,the length is 6 Bytes
  */
const u8 *usbh_cdc_ecm_process_mac_str(void)
{
	u32 i = 0;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	while ((cdc->usbh_state < USBH_CDC_ECM_SETUP) && (i < 10) && (cdc->mac_valid == 0))  {
		usb_os_sleep_ms(1000);
		i++;
		RTK_LOGS(TAG, RTK_LOG_INFO, "State(%d) want(%d) keep wait\n", cdc->usbh_state, USBH_CDC_ECM_SETUP);
	}
	return cdc->mac;
}

void usbh_cdc_ecm_set_dongle_mac(u8 *mac)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	if (NULL == mac) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param error,mac 0x%08x\n", mac);
		return ;
	}

	memcpy((void *) & (cdc->mac[0]), (void *)mac, 6);
	cdc->mac_src_type = CDC_ECM_MAC_UPPER_LAYER_SET;
}

void usbh_cdc_ecm_set_dongle_led_array(u16 *led, u8 len)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (led == NULL || len == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param error,led 0x%08x, len %d\n", led, len);
		return ;
	}

	USBH_ECM_FREE_MEM(cdc->led_array);

	cdc->led_array = (u16 *)usb_os_malloc(len * sizeof(u16));
	memcpy((void *)cdc->led_array, (void *)led, len * sizeof(u16));

	cdc->led_cnt = len;
}

#if ECM_STATE_DEBUG_ENABLE
void ecm_debug_thread(void *param)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	UNUSED(param);

	while (1) {
		if (cdc->host) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB Dump %d-%d-%d-%d/out %d-%d/in %d-%d/intr %d-%d/%d-%d-%d-%d\n",
					 cdc->next_transfor, cdc->state, usbh_cdc_ecm_get_hw_statue(), usbh_cdc_ecm_get_connect_status(),
					 cdc->bulk_data_out_state, usbh_get_urb_state(cdc->host, cdc->data_if.bulk_out_pipe),
					 cdc->bulk_data_in_state, usbh_get_urb_state(cdc->host, cdc->data_if.bulk_in_pipe),
					 cdc->intr_in_state, usbh_get_urb_state(cdc->host, cdc->comm_if.intr_in_pipe),
					 cdc->loop_cnt, cdc->bulk_in, cdc->bulk_out, cdc->intr_in
					);
		}
		usb_os_sleep_ms(1000);
	}
}
#endif
