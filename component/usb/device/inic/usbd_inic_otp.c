/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba.h"
#include "usbd_inic.h"
#include "usbd.h"

static const char *const TAG = "INIC";
/* Private defines -----------------------------------------------------------*/
#define USB_OTP_START	0x2A0U
#define USB_OTP_END		0x2DFU
#define USB_OTP_LEN		64U // USB_OTP_END - USB_OTP_START, aligned to 4 bytes
#define USB_OTP_STR_START	0x2A5U
#define USB_OTP_OFFSET_VID			(0x2A0U - USB_OTP_START)
#define USB_OTP_OFFSET_PID			(0x2A2U - USB_OTP_START)
#define USB_OTP_OFFSET_STR			(USB_OTP_STR_START - USB_OTP_START)
#define USB_OTP_STR_LEN				(USB_OTP_END - USB_OTP_STR_START + 1U)

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

static u8 usbd_otp_get_strlen(u8 *buf)
{
	u8 len = 0U;

	while (*buf != '\0') {
		len++;
		buf++;
	}

	return len;
}

/**
  * @brief  Get string descriptor from ASCII string buffer
  * @param  desc - String descriptor in UNICODE
  * @param  buf - String buffer in ASCII
  * @param  len - String descriptor length
  * @retval void
  */
static void usbd_otp_get_str_desc(u8 *desc, u8 *buf, u8 *len)
{
	u8 idx = 0U;

	if (buf != NULL) {
		*len = usbd_otp_get_strlen(buf) * 2U + 2U;
		desc[idx++] = *len;
		desc[idx++] = USB_DESC_TYPE_STRING;

		while (*buf != '\0') {
			desc[idx++] = *buf++;
			desc[idx++] =  0U;
		}
	}
}

/* Exported functions --------------------------------------------------------*/

int usbd_otp_init(usbd_otp_t *otp)
{
	int status;
	u32 reg;
	u8 sn_len;
	u8 mfg_len;
	u8 prod_len;
	u8 buf[USBD_OTP_MAX_STR_LEN];

	otp->otp_sn = 0U;
	otp->otp_param = 0U;

	otp->bt_en = ((HAL_READ32(OTPC_REG_BASE, SEC_OTP_SYSCFG0) & SEC_BIT_BT_FUNCTION_EN) == 0U) ? 0U : 1U;

	otp->mfg_str = (u8 *)usb_os_malloc(USBD_INIC_MAX_STR_LEN);
	if (otp->mfg_str == NULL) {
		return HAL_ERR_MEM;
	}

	otp->prod_str = (u8 *)usb_os_malloc(USBD_INIC_MAX_STR_LEN);
	if (otp->prod_str == NULL) {
		usb_os_mfree(otp->mfg_str);
		otp->mfg_str = NULL;
		return HAL_ERR_MEM;
	}

	otp->sn_str = (u8 *)usb_os_malloc(USBD_INIC_MAX_STR_LEN);
	if (otp->sn_str == NULL) {
		usb_os_mfree(otp->mfg_str);
		otp->mfg_str = NULL;
		usb_os_mfree(otp->prod_str);
		otp->prod_str = NULL;
		return HAL_ERR_MEM;
	}

	otp->otp_map = (u8 *)usb_os_malloc(OTP_LMAP_LEN);
	if (otp->otp_map == NULL) {
		usb_os_mfree(otp->mfg_str);
		otp->mfg_str = NULL;
		usb_os_mfree(otp->prod_str);
		otp->prod_str = NULL;
		usb_os_mfree(otp->sn_str);
		otp->sn_str = NULL;
		return HAL_ERR_MEM;
	}

	reg = HAL_READ32(USB_ADDON_REG_AUTOLOAD_CTRL, 0U);
	otp->self_powered = ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SELF_POWER_EN) == 0) ? 0 : 1;
	otp->remote_wakeup_en = ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_REMOTE_WAKEUP) == 0) ? 0 : 1;
	if (((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN) != 0U) ||
		((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM) == 0U)) {
		status = OTP_LogicalMap_Read(otp->otp_map, USB_OTP_START, USB_OTP_LEN);
		if (status != RTK_SUCCESS) {
			RTK_LOGE(TAG, "OTP read fail %d\n", status);
			return HAL_ERR_HW;
		}

		if ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM) == 0U) {
			// Mfg string
			mfg_len = otp->otp_map[USB_OTP_OFFSET_STR];
			if ((mfg_len < 2) || (mfg_len > USB_OTP_STR_LEN)) {
				return HAL_ERR_PARA;
			}
			// Product string
			prod_len = otp->otp_map[USB_OTP_OFFSET_STR + mfg_len];
			if ((prod_len < 2) || ((u16)(mfg_len + prod_len) > USB_OTP_STR_LEN)) {
				return HAL_ERR_PARA;
			}
			// SN string
			sn_len = otp->otp_map[USB_OTP_OFFSET_STR + mfg_len + prod_len];
			if ((sn_len < 2U) || ((u16)(sn_len + mfg_len + prod_len) > (u16)USB_OTP_STR_LEN)) {
				return HAL_ERR_PARA;
			}
			usb_os_memcpy((void *)buf, (void *)&otp->otp_map[USB_OTP_OFFSET_STR + mfg_len + prod_len + 2], sn_len - 2);
			buf[sn_len - 2] = '\0';
			usbd_otp_get_str_desc(otp->sn_str, buf, &otp->sn_str_len);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP SN str:%s\n", buf);
			otp->otp_sn = 1U;
		}

		if ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN) != 0U) {
			// VID
			otp->vid = otp->otp_map[USB_OTP_OFFSET_VID] | ((u16)otp->otp_map[USB_OTP_OFFSET_VID + 1] << 8);
			// PID
			otp->pid = otp->otp_map[USB_OTP_OFFSET_PID] | ((u16)otp->otp_map[USB_OTP_OFFSET_PID + 1] << 8);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP PID-VID: 0x%04x-0x%04x\n", otp->pid, otp->vid);
			// Mfg string
			mfg_len = otp->otp_map[USB_OTP_OFFSET_STR];
			if ((mfg_len < 2) || (mfg_len > USB_OTP_STR_LEN)) {
				return HAL_ERR_PARA;
			}
			usb_os_memcpy((void *)buf, (void *)&otp->otp_map[USB_OTP_OFFSET_STR + 2], mfg_len - 2);
			buf[mfg_len - 2] = '\0';
			usbd_otp_get_str_desc(otp->mfg_str, buf, &otp->mfg_str_len);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP MFG str:%s\n", buf);
			// Product string
			prod_len = otp->otp_map[USB_OTP_OFFSET_STR + mfg_len];
			if ((prod_len < 2) || ((u16)(otp->mfg_str_len + prod_len) > USB_OTP_STR_LEN)) {
				return HAL_ERR_PARA;
			}
			usb_os_memcpy((void *)buf, (void *)&otp->otp_map[USB_OTP_OFFSET_STR + mfg_len + 2], prod_len - 2);
			buf[prod_len - 2] = '\0';
			usbd_otp_get_str_desc(otp->prod_str, buf, &otp->prod_str_len);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP PROD str:%s\n", buf);
			otp->otp_param = 1U;
		}
	}

	return HAL_OK;
}

void usbd_otp_deinit(usbd_otp_t *otp)
{
	if (otp->mfg_str != NULL) {
		usb_os_mfree(otp->mfg_str);
		otp->mfg_str = NULL;
	}
	if (otp->prod_str != NULL) {
		usb_os_mfree(otp->prod_str);
		otp->prod_str = NULL;
	}
	if (otp->sn_str != NULL) {
		usb_os_mfree(otp->sn_str);
		otp->sn_str = NULL;
	}
	if (otp->otp_map != NULL) {
		usb_os_mfree(otp->otp_map);
		otp->otp_map = NULL;
	}
}

