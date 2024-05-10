/**
  ******************************************************************************
  * @file    usbh_cdc_ecm.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB CDC ECM Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbh_cdc_ecm.h"
#include "usb_os.h"
#include "usbh.h"

/* Private defines -----------------------------------------------------------*/
#define USBH_CDC_BUFFER_SIZE             1024
#define ECM_ENABLE_DUMP_DESCRIPYOT_PARSE (0)

/* conventional codes for class-specific descriptors */
#define USB_DT_CS_INTERFACE              0x24

#define USB_INTR_BUSY_MAX_CNT            1000
#define USB_INTR_IDLE_MAX_CNT            8000

/* Private types -------------------------------------------------------------*/
typedef struct {
	u8  bLength;
	u8  bDescriptorType;
} usb_ecm_descriptor_header;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbh_cdc_ecm_attach(usb_host_t *host);
static int usbh_cdc_ecm_detach(usb_host_t *host);
static int usbh_cdc_ecm_process(usb_host_t *host);
static int usbh_cdc_ecm_setup(usb_host_t *host);
static int usbh_cdc_ecm_sof(usb_host_t *host);
static int usbh_cdc_ecm_nak(usb_host_t *host, u8 pipe_num);
static void usbh_cdc_ecm_process_bulk_out(usb_host_t *host);
static void usbh_cdc_ecm_process_bulk_in(usb_host_t *host);
static void usbh_cdc_ecm_process_intr_in(usb_host_t *host);
static int usbh_cdc_acm_process_get_string(usb_host_t *host, u8 *pbuf, u16 buflen, u8 str_id);
static int usbh_cdc_ecm_process_set_alt(usb_host_t *host);
#if ECM_ENABLE_RCR_CONFIGURATION
static int usbh_cdc_ecm_process_get_rcr(usb_host_t *host);
static int usbh_cdc_ecm_process_set_rcr(usb_host_t *host);
#endif
#if ECM_ENABLE_PACKETFILTER
static int usbh_cdc_ecm_process_get_statistic(usb_host_t *host);
static int usbh_cdc_ecm_process_set_packet_filter(usb_host_t *host);
static int usbh_cdc_ecm_process_set_muiticast_filter(usb_host_t *host);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *TAG = "CDC_ECM";

/* USB Standard Device Descriptor */
static usbh_class_driver_t usbh_cdc_ecm_driver = {
	.class_code = CDC_CLASS_CODE,
	.attach  = usbh_cdc_ecm_attach,
	.detach  = usbh_cdc_ecm_detach,
	.setup   = usbh_cdc_ecm_setup,
	.process = usbh_cdc_ecm_process,
	.sof = usbh_cdc_ecm_sof,
	.nak = usbh_cdc_ecm_nak,
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

	RTK_LOGI(TAG, "--------------------Dump Descriptor Start------------------\n");

	//dump ecm control
	RTK_LOGI(TAG, "[INTR]epaddr(0x%x)MPS(%d)interval(%d)\n",
			 pcomm_if->intr_in_ep, pcomm_if->intr_in_packet_size, pcomm_if->intr_ep_interval);
	RTK_LOGI(TAG, "MAC(0x%x)thernet MPS(%d)\n",
			 pfunc_if->iMACAddress, pfunc_if->wMaxSegmentSize);
	//dump ecm data
	RTK_LOGI(TAG, "[BULK][in:epaddr(0x%x)MPS(=%d)][out:epaddr(0x%x)MPS(%d)]\n",
			 pdata_if->bulk_in_ep, pdata_if->bulk_in_packet_size,
			 pdata_if->bulk_out_ep, pdata_if->bulk_out_packet_size);

	RTK_LOGI(TAG, "---------------------Dump End-----------------------------\n");
#endif
}
static int usbh_cdc_ecm_usb_status_check(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	if (cdc->usbh_state < USBH_CDC_ECM_SETUP) {
		RTK_LOGE(TAG, "[ECM]Usb Not Setup success,wait...\n");
		usb_os_sleep_ms(1000);
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

	u8 *desc = pbuf ;
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
	u8 *desc = pbuf ;
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
				RTK_LOGE(TAG, "ECM parse control fail\n");
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
					RTK_LOGE(TAG, "ECM parse data fail\n");
					return ret;
				}

				pbuf = (usbh_if_desc_t *)((u8 *) pbuf + len);
				cfglen -= len;
			}
			break;

		default:
			RTK_LOGE(TAG, "ECM subclass(%d) is not control or data\n", pbuf->bInterfaceClass);
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
	//max_ep_size = max_ep_size > rx_fifo_size ? rx_fifo_size : max_ep_size;

	pdesc = usbh_get_device_descriptor(ecm->host);
	ecm->vid = pdesc->idVendor;
	ecm->pid = pdesc->idProduct;

	status = usbh_cdc_ecm_parse_cfgdesc(host);
	if (status) {
		RTK_LOGE(TAG, "Fail to parse ecm class specific descriptor\n");
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
		RTK_LOGE(TAG, "Fail to allocate INTR IN pipe for CDC ECM class.");
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
		RTK_LOGE(TAG, "Fail to allocate BULK OUT pipe for CDC ECM class.");
		usbh_free_pipe(host, pcomm_if->intr_in_pipe);
		return HAL_ERR_MEM;
	}

	pipe_num = usbh_alloc_pipe(host, pdata_if->bulk_in_ep);
	if (pipe_num != 0xFFU) {
		pdata_if->bulk_in_pipe = pipe_num;
	} else {
		RTK_LOGE(TAG, "Fail to allocate BULK IN pipe for CDC ECM class.");
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
	RTK_LOGI(TAG, "[in:epaddr=0x%x,chan=%d packsize=%d][out:epaddr=0x%x,chan=%d,packsize=%d]\n",
			 pdata_if->bulk_in_ep, pdata_if->bulk_in_pipe, pdata_if->bulk_in_packet_size,
			 pdata_if->bulk_out_ep, pdata_if->bulk_out_pipe, pdata_if->bulk_in_packet_size);

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
  * @brief  Usb Setup callback function.
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_setup(usb_host_t *host)
{
	u8 i = 0;
	int status = HAL_ERR_UNKNOWN ;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	/*Issue the get line coding request*/
	status = usbh_cdc_acm_process_get_string(host, cdc->mac_string, CDC_ECM_MAC_STRING_LEN, cdc->func_if.iMACAddress);
	if (status == HAL_OK) {
		for (i = 0; i < 6; i++) {
			cdc->mac[i] = hex_to_char(cdc->mac_string[2 + 4 * i]) * 16 + hex_to_char(cdc->mac_string[2 + 4 * i + 2]) ;
		}

		cdc->usbh_state = USBH_CDC_ECM_SETUP;
		if ((cdc->cb != NULL) && (cdc->cb->setup != NULL)) {
			cdc->cb->setup();
		}
	}

	return status;
}
/**
  * @brief  Usb Sof callback function.
  * @param  host: Host handle
  * @retval HAL_OK
  */
static int usbh_cdc_ecm_sof(usb_host_t *host)
{
	u8 i = 0;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	for (i = 0U; i < host->config.pipes; i++) {
		if (cdc->host_pipe[i]) {
			cdc->host_pipe[i] = 0;
			usbh_reactivate_pipe(host, i);
		}
	}

	return HAL_OK;
}
/**
  * @brief  Usb nak callback function.
  * @param  host: Host handle
  * @param  pipe_num: Pipe number
  * @retval return OK while success, else return HAL_ERR_UNKNOWN
  */
static int usbh_cdc_ecm_nak(usb_host_t *host, u8 pipe_num)
{
	u8 ep_type ;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	if (pipe_num >= CDC_ECM_HOST_PIPE_COUNT) {
		return HAL_ERR_PARA ;
	}

	ep_type = usbh_get_ep_type(host, pipe_num);
	if (ep_type == USB_CH_EP_TYPE_BULK) {
		//do not retransmit in this loop, it will do in next SOF
		cdc->host_pipe[pipe_num] = 1;
		return HAL_OK;
	} else if (ep_type == USB_CH_EP_TYPE_INTR) {
		cdc->intr_in_busy_tick = host->tick;
		cdc->intr_in_idle_tick = host->tick;
		//INTR did not retrigger the EP, but wait intr_in_busy_tick to retrigger
		cdc->host_pipe[pipe_num] = 0;
		//finish the ep
		return HAL_OK;
	}

	return HAL_ERR_UNKNOWN ;
}

/**
  * @brief  Usb State Machine handling callback
  * @param  host: Host handle
  * @retval Status
  */
static int usbh_cdc_ecm_process(usb_host_t *host)
{
	u8 req_status = HAL_OK;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	switch (cdc->state) {

	case CDC_ECM_STATE_IDLE:
		break;

	case CDC_ECM_STATE_ALT_SETTING:
		req_status = usbh_cdc_ecm_process_set_alt(host);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ECM_STATE_IDLE;
			RTK_LOGD(TAG, "ECM alt setting success\n");
		} else if (req_status != HAL_BUSY) {
			RTK_LOGW(TAG, "ECM alt setting error\n");
			cdc->state = CDC_ECM_STATE_ERROR;
		}
		break;

#if ECM_ENABLE_PACKETFILTER
	case CDC_ECM_STATE_SET_ETHERNET_MULTICAST_FILTER:
		req_status = usbh_cdc_ecm_process_set_muiticast_filter(host);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ECM_STATE_IDLE;
			RTK_LOGD(TAG, "ECM set eth multicast filter success\n");
		} else if (req_status != HAL_BUSY) {
			RTK_LOGW(TAG, "ECM set eth multicast filter error\n");
			cdc->state = CDC_ECM_STATE_ERROR;
		}
		break;

	case CDC_ECM_STATE_SET_ETHERNET_PACKET_FILTER:
		req_status = usbh_cdc_ecm_process_set_packet_filter(host);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ECM_STATE_IDLE;
			RTK_LOGD(TAG, "ECM set eth packet filter success\n");
		} else if (req_status != HAL_BUSY) {
			RTK_LOGW(TAG, "ECM set eth packet filter error\n");
			cdc->state = CDC_ECM_STATE_ERROR;
		}
		break;

	case CDC_ECM_STATE_GET_ETHERNET_STATISTIC:
		req_status = usbh_cdc_ecm_process_get_statistic(host);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ECM_STATE_IDLE;
			RTK_LOGD(TAG, "ECM get eth statistic success\n");
		} else if (req_status != HAL_BUSY) {
			RTK_LOGW(TAG, "ECM get eth statistic error\n");
			cdc->state = CDC_ECM_STATE_ERROR;
		}
		break;
#endif

#if ECM_ENABLE_RCR_CONFIGURATION
	case CDC_ECM_STATE_RCR_GET:
		RTK_LOGI(TAG, "ECM switch to rcr get \n");
		req_status = usbh_cdc_ecm_process_get_rcr(host);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ECM_STATE_IDLE;
			RTK_LOGD(TAG, "ECM get rcr success\n");
		} else if (req_status != HAL_BUSY) {
			RTK_LOGW(TAG, "ECM get rcr error\n");
			cdc->state = CDC_ECM_STATE_ERROR;
		}
		break;

	case CDC_ECM_STATE_RCR_SET:
		RTK_LOGI(TAG, "ECM switch to rcr set \n");
		req_status = usbh_cdc_ecm_process_set_rcr(host);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ECM_STATE_IDLE;
			RTK_LOGD(TAG, "ECM set rcr success\n");
		} else if (req_status != HAL_BUSY) {
			RTK_LOGW(TAG, "ECM set rcr error\n");
			cdc->state = CDC_ECM_STATE_ERROR;
		}
		break;
#endif

	case CDC_ECM_STATE_SWITCH_TO_TRANSFER:
		RTK_LOGI(TAG, "ECM switch to transfer \n");
		req_status = HAL_OK;
		cdc->state = CDC_ECM_STATE_TRANSFER;
		break;

	case CDC_ECM_STATE_TRANSFER:
		usbh_cdc_ecm_process_bulk_in(host);
		usbh_cdc_ecm_process_bulk_out(host);
		usbh_cdc_ecm_process_intr_in(host);
		if (cdc->next_transfor) {
			cdc->next_transfor = 0;
			usbh_notify_class_state_change(host);
		}
		break;

	case CDC_ECM_STATE_ERROR:
		req_status = usbh_ctrl_clear_feature(host, 0x00U);
		if (req_status == HAL_OK) {
			cdc->state = CDC_ECM_STATE_IDLE;
		}
		break;

	default:
		break;

	}

	return req_status;
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
	case CDC_ECM_TRANSFER_STATE_TX:
		usbh_bulk_send_data(host,
							cdc->bulk_data_out_buf,
							cdc->bulk_data_out_len,
							cdc->data_if.bulk_out_pipe);
		cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_TX_BUSY;
		cdc->next_transfor = 1;
		break;

	case CDC_ECM_TRANSFER_STATE_TX_BUSY:
		urb_state = usbh_get_urb_state(host, cdc->data_if.bulk_out_pipe);
		if (urb_state == USBH_URB_DONE) {
			if (cdc->bulk_out_zlp) { //ZLP
				cdc->bulk_out_zlp = 0U;
				cdc->bulk_data_out_len = 0U;
				cdc->bulk_data_out_buf = NULL;
				cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_TX;
			} else {
				cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_IDLE;
				if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
					cdc->cb->bulk_send(urb_state);
				}
			}
			cdc->next_transfor = 1;
		} else if (urb_state == USBH_URB_BUSY) { //retransmit
			cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_TX;
			cdc->next_transfor = 1;
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_IDLE;
			if ((cdc->cb != NULL) && (cdc->cb->bulk_send != NULL)) {
				cdc->cb->bulk_send(urb_state);
			}
			RTK_LOGW(TAG, "BULK out failed %d\n", urb_state);
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
	case CDC_ECM_TRANSFER_STATE_RX:
		usbh_bulk_receive_data(host,
							   cdc->bulk_data_in_buf,
							   USBH_CDC_ECM_BULK_BUF_MAX_SIZE,
							   cdc->data_if.bulk_in_pipe);
		cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_RX_BUSY;
		cdc->next_transfor = 1;
		break;

	case CDC_ECM_TRANSFER_STATE_RX_BUSY:
		urb_state = usbh_get_urb_state(host, cdc->data_if.bulk_in_pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, cdc->data_if.bulk_in_pipe);
			if ((cdc->cb != NULL) && (cdc->cb->bulk_received != NULL)) {
				cdc->cb->bulk_received(cdc->bulk_data_in_buf, len);
			}

			usbh_bulk_receive_data(host,
								   cdc->bulk_data_in_buf,
								   USBH_CDC_ECM_BULK_BUF_MAX_SIZE,
								   cdc->data_if.bulk_in_pipe);

			cdc->next_transfor = 1;
		} else if (urb_state == USBH_URB_BUSY) {
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
			RTK_LOGW(TAG, "BULK in failed %d\n", urb_state);
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
static void usbh_cdc_ecm_process_intr_in(usb_host_t *host)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usbh_urb_state_t urb_state = USBH_URB_IDLE;
	u32 len;

	switch (cdc->intr_in_state) {
	case CDC_ECM_INT_TRANSFER_STATE_RX:
		usbh_intr_receive_data(host,
							   cdc->intr_in_buf,
							   cdc->comm_if.intr_in_packet_size,
							   cdc->comm_if.intr_in_pipe);

		cdc->intr_in_idle_tick = host->tick;
		cdc->intr_in_busy_tick = host->tick;
		cdc->intr_in_state = CDC_ECM_INT_TRANSFER_STATE_RX_BUSY;
		cdc->next_transfor = 1;
		break;

	case CDC_ECM_INT_TRANSFER_STATE_RX_BUSY:
		urb_state = usbh_get_urb_state(host, cdc->comm_if.intr_in_pipe);
		if (urb_state == USBH_URB_DONE) {
			len = usbh_get_last_transfer_size(host, cdc->comm_if.intr_in_pipe);
			if (len && (cdc->cb != NULL) && (cdc->cb->intr_received != NULL)) {
				cdc->cb->intr_received(cdc->intr_in_buf, len);
			}
			if (len > cdc->comm_if.intr_in_packet_size) {
				RTK_LOGD(TAG, "Intr urb_state(%d) done!\n", urb_state);
				cdc->intr_in_state = CDC_ECM_INT_TRANSFER_STATE_RX;
			} else {
				cdc->intr_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
			}
			cdc->next_transfor = 1;
		} else if (urb_state == USBH_URB_BUSY) {
			cdc->next_transfor = 1;
			if (host->tick - cdc->intr_in_busy_tick >= USB_INTR_BUSY_MAX_CNT) {
				RTK_LOGD(TAG, "Intr in busy to long!\n");
				cdc->intr_in_state = CDC_ECM_INT_TRANSFER_STATE_RX;
				cdc->intr_in_busy_tick = host->tick;
			}
		} else if ((urb_state == USBH_URB_ERROR) || (urb_state == USBH_URB_STALL)) {
			cdc->intr_in_state = CDC_ECM_TRANSFER_STATE_IDLE;
			RTK_LOGW(TAG, "INTR in failed %d\n", urb_state);
			cdc->next_transfor = 1;
		} else if (urb_state == USBH_URB_IDLE) {
			//while run usbh_intr_receive_data, but the interrupt not happen
			cdc->next_transfor = 1;
			if (host->tick - cdc->intr_in_idle_tick >= USB_INTR_IDLE_MAX_CNT) { // 1s
				RTK_LOGD(TAG, "Intr in idle to long(%ld-%ld)!\n", host->tick, cdc->intr_in_idle_tick);
				cdc->intr_in_state = CDC_ECM_INT_TRANSFER_STATE_RX;
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

#if ECM_ENABLE_RCR_CONFIGURATION
/**
  * @brief  Get Ethernet Statistic
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

	return usbh_ctrl_request(host, &setup, (u8 *)cdc->rcr);
}

static int usbh_cdc_ecm_process_set_rcr(usb_host_t *host)
{
	usbh_setup_req_t setup;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	setup.b.bmRequestType = USB_H2D | USB_REQ_TYPE_VENDOR | USB_REQ_RECIPIENT_DEVICE;
	setup.b.bRequest = 0x05;
	setup.b.wValue = 0xc010;
	setup.b.wIndex = 0x010F;
	setup.b.wLength = 4;

	return usbh_ctrl_request(host, &setup, (u8 *)cdc->rcr);
}
#endif

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
	usb_os_memset(cdc, 0x00, sizeof(usbh_cdc_ecm_host_t));

	cdc->mac_string = usb_os_malloc(CDC_ECM_MAC_STRING_LEN);
	if (NULL == cdc->mac_string) {
		RTK_LOGE(TAG, "CDC ECM init error, can not get the %d memory!", CDC_ECM_MAC_STRING_LEN);
		return HAL_ERR_MEM;
	}

	cdc->bulk_data_in_buf = usb_os_malloc(USBH_CDC_ECM_BULK_BUF_MAX_SIZE);
	if (NULL == cdc->bulk_data_in_buf) {
		RTK_LOGE(TAG, "CDC ECM init error, can not get the %d memory!", USBH_CDC_ECM_BULK_BUF_MAX_SIZE);
		usb_os_mfree(cdc->mac_string);
		cdc->mac_string = NULL;
		return HAL_ERR_MEM;
	}

#if ECM_ENABLE_RCR_CONFIGURATION
	cdc->rcr = usb_os_malloc(CDC_ECM_MAC_STRING_LEN);
	if (NULL == cdc->rcr) {
		RTK_LOGE(TAG, "CDC ECM init error, can not get the %d memory!", CDC_ECM_MAC_STRING_LEN);
		usb_os_mfree(cdc->mac_string);
		usb_os_mfree(cdc->bulk_data_in_buf);
		return HAL_ERR_MEM;
	}
#endif

	if (cb != NULL) {
		cdc->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				RTK_LOGE(TAG, "CDC ECM user init error: %d", ret);
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

	RTK_LOGD(TAG, "Deinit!\n");
	if ((cdc->cb != NULL) && (cdc->cb->deinit != NULL)) {
		cdc->cb->deinit();
	}

	if ((host != NULL) && (host->state == USBH_CLASS_READY)) {
		cdc->state = CDC_ECM_STATE_IDLE;

		usbh_close_pipe(host, cdc->comm_if.intr_in_pipe);
		usbh_close_pipe(host, cdc->data_if.bulk_in_pipe);
		usbh_close_pipe(host, cdc->data_if.bulk_out_pipe);
	}

#if ECM_ENABLE_RCR_CONFIGURATION
	if (cdc->rcr) {
		usb_os_mfree(cdc->rcr);
		cdc->rcr = NULL;
	}
#endif

	if (cdc->mac_string) {
		usb_os_mfree(cdc->mac_string);
		cdc->mac_string = NULL;
	}

	if (cdc->bulk_data_in_buf) {
		usb_os_mfree(cdc->bulk_data_in_buf);
		cdc->bulk_data_in_buf = NULL;
	}


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
		//RTK_LOGE(TAG, "Usb Not Setup success,just return[%s]\n", __func__);
		return ret;
	}

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_TRANSFER)) {
		if (cdc->bulk_data_out_state == CDC_ECM_TRANSFER_STATE_IDLE) {
			cdc->bulk_data_out_buf = buf;
			cdc->bulk_data_out_len = len;
			if (len % cdc->data_if.bulk_out_packet_size == 0) { //ZLP
				cdc->bulk_out_zlp = 1;
			} else {
				cdc->bulk_out_zlp = 0;
			}
			cdc->bulk_data_out_state = CDC_ECM_TRANSFER_STATE_TX;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host);
			ret = HAL_OK;
		} else {
			RTK_LOGW(TAG, "BULK TX %ld: busy\n", len);
		}
	} else {
		RTK_LOGW(TAG, "BULK TX not ready(%d)\n", cdc->state);
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
	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_TRANSFER)) {
		if (cdc->bulk_data_in_state == CDC_ECM_TRANSFER_STATE_IDLE) {
			cdc->bulk_data_in_state = CDC_ECM_TRANSFER_STATE_RX;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			usbh_notify_class_state_change(host);
			ret = HAL_OK;
		} else {
			RTK_LOGW(TAG, "BULK RX busy %d\n", cdc->bulk_data_in_state);
		}
	} else {
		RTK_LOGW(TAG, "BULK RX not ready(%d)\n", cdc->state);
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
	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_TRANSFER)) {
		if (cdc->intr_in_state == CDC_ECM_TRANSFER_STATE_IDLE) {
			cdc->intr_in_buf = buf;
			cdc->intr_in_state = CDC_ECM_INT_TRANSFER_STATE_RX;
			cdc->state = CDC_ECM_STATE_TRANSFER;
			if (len < pcomm_if->intr_in_packet_size) {
				RTK_LOGW(TAG, "Notice,Please enlarge the inbuf size(%d-%d)\n",
						 pcomm_if->intr_in_packet_size, len);
				pcomm_if->intr_in_packet_size = len;
			}
			usbh_notify_class_state_change(host);
			ret = HAL_OK;
		} else {
			RTK_LOGW(TAG, "INTR IN %ld: busy\n", len);
		}
	} else {
		RTK_LOGW(TAG, "INTR IN not ready(%d)\n", cdc->state);
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
	RTK_LOGD(TAG, "cfg_id =%d\n", cfg_id);
	return usbh_set_configuration(host, cfg_id);
}

#if ECM_ENABLE_PACKETFILTER
/**
  * @brief  get thernet statistic info
  * @param  selector: selectot index
  * @retval Status
  */
int usbh_cdc_ecm_get_ethernet_statistic(u16 selector)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}
	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_GET_ETHERNET_STATISTIC)) {
		cdc->state = CDC_ECM_STATE_GET_ETHERNET_STATISTIC;
		cdc->feature_selector = selector;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  set thernet packet filter
  * @param  filter: filer value,
  * @retval Status
  */
int usbh_cdc_ecm_set_ethernet_packetfilter(u16 filter)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_SET_ETHERNET_PACKET_FILTER)) {
		cdc->state = CDC_ECM_STATE_SET_ETHERNET_PACKET_FILTER;
		cdc->packet_filter = filter;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  set thernet multicast filter array
  * @param  filter: multicast buffer handle
  			len: buffer length
  * @retval Status
  */
int usbh_cdc_ecm_set_ethernet_multicast_filter(u8 *filter, u8 len)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_SET_ETHERNET_MULTICAST_FILTER)) {
		cdc->state = CDC_ECM_STATE_SET_ETHERNET_MULTICAST_FILTER;
		usb_os_memcpy(cdc->muticast_filter, filter, len);
		cdc->muticast_filter_len = len;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}
#endif

#if ECM_ENABLE_RCR_CONFIGURATION
/**
  * @brief  get ECM device rcr(right now,just for 0x8156)
  * @retval Status
  */
int usbh_cdc_ecm_get_rcr(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if ((cdc->pid != 0x8156) || (cdc->vid != 0x0BDA)) {
		return HAL_OK;
	}

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_RCR_GET)) {
		cdc->state = CDC_ECM_STATE_RCR_GET;
		usb_os_memset(cdc->rcr, 0, 4);
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  set ECM device rcr(right now,just for 0x8156)
  * @retval Status
  */
int usbh_cdc_ecm_set_rcr(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if ((cdc->pid != 0x8156) || (cdc->vid != 0x0BDA)) {
		return HAL_OK;
	}

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_RCR_SET)) {
		cdc->state = CDC_ECM_STATE_RCR_SET;
		//set bit 0~3 set 1 will enable PING & UDP transfer
		cdc->rcr[0] = cdc->rcr[0] | 0x0F;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}
#endif

/**
  * @brief  set thernet packet filter
  * @param  NONE
  * @retval Status
  */
int usbh_cdc_ecm_set_ethernet_start_transfer(void)
{
	int ret = HAL_BUSY;
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if ((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->state == CDC_ECM_STATE_SWITCH_TO_TRANSFER)) {
		cdc->state = CDC_ECM_STATE_SWITCH_TO_TRANSFER;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}

/**
  * @brief  set the alternate setting
  * @param  NONE
  * @retval Status
  */
int usbh_cdc_ecm_alt_setting(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	usb_host_t *host = cdc->host;
	int ret = HAL_ERR_UNKNOWN;

	if (usbh_cdc_ecm_usb_status_check() != HAL_OK) {
		return ret;
	}

	if ((host->state == USBH_CLASS_REQUEST) || (host->state == USBH_CLASS_READY)) {
		cdc->state = CDC_ECM_STATE_ALT_SETTING;
		usbh_notify_class_state_change(host);
		ret = HAL_OK;
	}

	return ret;
}
/**
  * @brief  get the bulk in process status
  * @param  NONE
  * @retval Status
  */
int usbh_cdc_ecm_bulk_in_processing(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	return !((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->bulk_data_in_state == CDC_ECM_TRANSFER_STATE_IDLE)) ;
}
/**
  * @brief  get the intr in process status
  * @param  NONE
  * @retval Status
  */
int usbh_cdc_ecm_intr_in_processing(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;

	return !((cdc->state == CDC_ECM_STATE_IDLE) || (cdc->intr_in_state == CDC_ECM_TRANSFER_STATE_IDLE)) ;
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
		RTK_LOGW(TAG, "ECM hostState(%d)want(%d)keep wait ...\n", cdc->usbh_state, USBH_CDC_ECM_SETUP);
	}
	return cdc->data_if.bulk_in_packet_size;
}

/**
  * @brief  return interrupt interval
  * @retval interval
  */
u32 usbh_cdc_ecm_get_intr_interval(void)
{
	usbh_cdc_ecm_host_t *cdc = &usbh_cdc_ecm_host;
	if (cdc->usbh_state < USBH_CDC_ECM_SETUP) {
		return 10;
	} else {
		return cdc->comm_if.intr_ep_interval;
	}
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
	while (cdc->usbh_state < USBH_CDC_ECM_SETUP && i < 10) {
		usb_os_sleep_ms(1000);
		i++;
		RTK_LOGW(TAG, "ECM hostState(%d)want(%d)keep wait get mac string!\n", cdc->usbh_state, USBH_CDC_ECM_SETUP);
	}
	return cdc->mac;
}

