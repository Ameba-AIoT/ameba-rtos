/*
 *  Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_hal.h"
#include "usbd_core.h"
#include "usbd_otp.h"

/* Private defines -----------------------------------------------------------*/

#define USB_OTP_START		0x2A0U
#define USB_OTP_END			0x2DFU
#define USB_OTP_LEN			64U // USB_OTP_END - USB_OTP_START, aligned to 4 bytes
#define USB_OTP_STR_START	0x2A5U
#define USB_OTP_OFFSET_VID			(0x2A0U - USB_OTP_START)
#define USB_OTP_OFFSET_PID			(0x2A2U - USB_OTP_START)
#define USB_OTP_OFFSET_STR			(USB_OTP_STR_START - USB_OTP_START)
#define USB_OTP_STR_LEN				(USB_OTP_END - USB_OTP_STR_START + 1U)
#define USB_OTP_STR_MFG_LEN			9U
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "USB";

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
u8 USB_OTP_Init(USB_OTP_TypeDef *otp)
{
	int status;
	u32 reg;
	u8 sn_len;
	u8 mfg_len;
	u8 prod_len;
	u8 buf[USB_OTP_STR_LEN];

	otp->otp_sn = 0U;
	otp->otp_param = 0U;

	reg = HAL_READ32(USB_REG_BASE, USB_ADDON_REG_AUTOLOAD_CTRL);
	otp->self_powered = ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SELF_POWER_EN) == 0U) ? 0U : 1U;
	otp->remote_wakeup_en = ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_REMOTE_WAKEUP) == 0U) ? 0U : 1U;
	if (((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN) != 0U) ||
		((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM) == 0U)) {
		status = OTP_LogicalMap_Read(otp->otp_map, USB_OTP_START, USB_OTP_LEN);
		if (status != RTK_SUCCESS) {
			RTK_LOGE(TAG, "OTP read fail %d\n", status);
			return HAL_ERR_HW;
		}

		if ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM) == 0U) {
			RTK_LOGD(TAG, "Get OTP SN str\n");
			// Mfg string
			mfg_len = otp->otp_map[USB_OTP_OFFSET_STR];
			if ((mfg_len < 2) || (mfg_len > USB_OTP_STR_MFG_LEN)) {
				RTK_LOGE(TAG, "Invalid MFG str len %d\n", mfg_len);
				return HAL_ERR_PARA;
			}
			// Product string
			prod_len = otp->otp_map[USB_OTP_OFFSET_STR + mfg_len];
			if ((prod_len < 2) || ((u16)(mfg_len + prod_len) > USB_OTP_STR_LEN)) {
				RTK_LOGE(TAG, "Invalid PROD str len %d\n", prod_len);
				return HAL_ERR_PARA;
			}
			// SN string
			sn_len = otp->otp_map[USB_OTP_OFFSET_STR + mfg_len + prod_len];
			if ((sn_len < 2U) || ((u16)(sn_len + mfg_len + prod_len) > (u16)USB_OTP_STR_LEN)) {
				RTK_LOGE(TAG, "Get OTP SN str fail\n");
				return HAL_ERR_PARA;
			}
			_memcpy((void *)buf, (void *)&otp->otp_map[USB_OTP_OFFSET_STR + mfg_len + prod_len + 2], sn_len - 2);
			buf[sn_len - 2] = '\0';
			USBD_GetString(buf, otp->sn_str, &otp->sn_str_len);
			//RTK_LOGS(TAG, RTK_LOG_INFO, "Get OTP SN str: %s\n", buf);
			otp->otp_sn = 1U;
		}

		if ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN) != 0U) {
			RTK_LOGD(TAG, "Get desc from OTP\n");
			// VID
			otp->vid = otp->otp_map[USB_OTP_OFFSET_VID] | ((u16)otp->otp_map[USB_OTP_OFFSET_VID + 1] << 8);
			// PID
			otp->pid = otp->otp_map[USB_OTP_OFFSET_PID] | ((u16)otp->otp_map[USB_OTP_OFFSET_PID + 1] << 8);
			//RTK_LOGS(TAG, RTK_LOG_INFO, "Get OTP PID-VID 0x%04x-0x%04x\n", otp->pid, otp->vid);
			// Mfg string
			mfg_len = otp->otp_map[USB_OTP_OFFSET_STR];
			if ((mfg_len < 2) || (mfg_len > USB_OTP_STR_MFG_LEN)) {
				RTK_LOGE(TAG, "Get OTP MFG str fail\n");
				return HAL_ERR_PARA;
			}
			_memcpy((void *)buf, (void *)&otp->otp_map[USB_OTP_OFFSET_STR + 2], mfg_len - 2);
			buf[mfg_len - 2] = '\0';
			USBD_GetString(buf, otp->mfg_str, &otp->mfg_str_len);
			//RTK_LOGS(TAG, RTK_LOG_INFO, "Get OTP MFG str: %s\n", buf);
			// Product string
			prod_len = otp->otp_map[USB_OTP_OFFSET_STR + mfg_len];
			if ((prod_len < 2) || ((u16)(otp->mfg_str_len + prod_len) > USB_OTP_STR_LEN)) {
				RTK_LOGE(TAG, "Get OTP PROD str fail\n");
				return HAL_ERR_PARA;
			}
			_memcpy((void *)buf, (void *)&otp->otp_map[USB_OTP_OFFSET_STR + mfg_len + 2], prod_len - 2);
			buf[prod_len - 2] = '\0';
			USBD_GetString(buf, otp->prod_str, &otp->prod_str_len);
			//RTK_LOGS(TAG, RTK_LOG_INFO, "Get OTP PROD str: %s\n", buf);
			otp->otp_param = 1U;
		}
	}

	return HAL_OK;
}

