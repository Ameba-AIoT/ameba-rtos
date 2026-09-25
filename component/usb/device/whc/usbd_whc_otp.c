/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba.h"
#include "usbd_whc.h"
#include "usbd.h"

static const char *const TAG = "WHC";
/* Private defines -----------------------------------------------------------*/
#define USB_OTP_START              0x2A0U
#define USB_OTP_LEN                64U
#define USB_OTP_STR_START          0x2A5U
#define USB_OTP_OFFSET_VID         (0x2A0U - USB_OTP_START)
#define USB_OTP_OFFSET_PID         (0x2A2U - USB_OTP_START)
#define USB_OTP_OFFSET_STR         (USB_OTP_STR_START - USB_OTP_START)

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
	u16 off;
	u8 buf[USBD_OTP_MAX_STR_LEN];

	otp->otp_sn = 0U;
	otp->otp_param = 0U;

	otp->bt_en = ((HAL_READ32(OTPC_REG_BASE, SEC_OTP_SYSCFG0) & SEC_BIT_BT_FUNCTION_EN) == 0U) ? 0U : 1U;

	otp->mfg_str = (u8 *)usb_os_malloc(USBD_WHC_MAX_STR_LEN);
	if (otp->mfg_str == NULL) {
		return HAL_ERR_MEM;
	}

	otp->prod_str = (u8 *)usb_os_malloc(USBD_WHC_MAX_STR_LEN);
	if (otp->prod_str == NULL) {
		usb_os_mfree((void *)otp->mfg_str);
		otp->mfg_str = NULL;
		return HAL_ERR_MEM;
	}

	otp->sn_str = (u8 *)usb_os_malloc(USBD_WHC_MAX_STR_LEN);
	if (otp->sn_str == NULL) {
		usb_os_mfree((void *)otp->mfg_str);
		otp->mfg_str = NULL;
		usb_os_mfree((void *)otp->prod_str);
		otp->prod_str = NULL;
		return HAL_ERR_MEM;
	}

	otp->otp_map = (u8 *)usb_os_malloc(USB_OTP_LEN);
	if (otp->otp_map == NULL) {
		usb_os_mfree((void *)otp->mfg_str);
		otp->mfg_str = NULL;
		usb_os_mfree((void *)otp->prod_str);
		otp->prod_str = NULL;
		usb_os_mfree((void *)otp->sn_str);
		otp->sn_str = NULL;
		return HAL_ERR_MEM;
	}

	reg = HAL_READ32(USB_ADDON_REG_AUTOLOAD_CTRL, 0U);
	otp->self_powered = ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SELF_POWER_EN) == 0) ? 0 : 1;
	otp->remote_wakeup_en = ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_REMOTE_WAKEUP) == 0) ? 0 : 1;
	if (((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN) != 0U) ||
		((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM) == 0U)) {
		status = OTP_LogicalRead(otp->otp_map, USB_OTP_START, USB_OTP_LEN);
		if (status != RTK_SUCCESS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP read fail %d\n", status);
			return HAL_ERR_HW;
		}

		if ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM) == 0U) {
			/* The strings are chained, each one starting with its own length byte. A length byte
			 * that is still to be read has to stay inside the map, hence the >= below. */
			off = USB_OTP_OFFSET_STR;
			// Mfg string
			mfg_len = otp->otp_map[off];
			if ((mfg_len < 2U) || ((off + mfg_len) >= USB_OTP_LEN)) {
				return HAL_ERR_PARA;
			}
			off += mfg_len;
			// Product string
			prod_len = otp->otp_map[off];
			if ((prod_len < 2U) || ((off + prod_len) >= USB_OTP_LEN)) {
				return HAL_ERR_PARA;
			}
			off += prod_len;
			// SN string
			sn_len = otp->otp_map[off];
			if ((sn_len < 2U) || ((off + sn_len) > USB_OTP_LEN)) {
				return HAL_ERR_PARA;
			}
			usb_os_memcpy((void *)buf, (const void *)&otp->otp_map[off + 2U], sn_len - 2U);
			buf[sn_len - 2U] = '\0';
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
			off = USB_OTP_OFFSET_STR;
			// Mfg string
			mfg_len = otp->otp_map[off];
			/* The product length byte follows the mfg string, so it has to stay inside the map */
			if ((mfg_len < 2U) || ((off + mfg_len) >= USB_OTP_LEN)) {
				return HAL_ERR_PARA;
			}
			usb_os_memcpy((void *)buf, (const void *)&otp->otp_map[off + 2U], mfg_len - 2U);
			buf[mfg_len - 2U] = '\0';
			usbd_otp_get_str_desc(otp->mfg_str, buf, &otp->mfg_str_len);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP MFG str:%s\n", buf);
			off = (u16)(off + mfg_len);
			// Product string
			prod_len = otp->otp_map[off];
			/* Bound by the raw ASCII length, mfg_str_len already holds the UNICODE descriptor length */
			if ((prod_len < 2U) || ((off + prod_len) > USB_OTP_LEN)) {
				return HAL_ERR_PARA;
			}
			usb_os_memcpy((void *)buf, (const void *)&otp->otp_map[off + 2U], prod_len - 2U);
			buf[prod_len - 2U] = '\0';
			usbd_otp_get_str_desc(otp->prod_str, buf, &otp->prod_str_len);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP PROD str:%s\n", buf);
			otp->otp_param = 1U;
		}
	}

	return HAL_OK;
}

void usbd_otp_deinit(usbd_otp_t *otp)
{
	usb_os_mfree((void *)otp->mfg_str);
	otp->mfg_str = NULL;
	usb_os_mfree((void *)otp->prod_str);
	otp->prod_str = NULL;
	usb_os_mfree((void *)otp->sn_str);
	otp->sn_str = NULL;
	usb_os_mfree((void *)otp->otp_map);
	otp->otp_map = NULL;
}
