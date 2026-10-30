/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba.h"
#include "usbd_whc.h"
#include "usbd.h"

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

static int usbd_otp_parse_map(usbd_otp_t *otp, u8 *map, u32 reg);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "WHC";

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Read the raw OTP map and extract the identity parameters from it
  * @param  otp: OTP parameter block, whose string buffers are already allocated
  * @param  map: Scratch buffer of USB_OTP_LEN bytes
  * @param  reg: Value of the USB autoload control register
  * @retval Status
  */
static int usbd_otp_parse_map(usbd_otp_t *otp, u8 *map, u32 reg)
{
	int status;
	u8 sn_len;
	u8 mfg_len;
	u8 prod_len;
	u16 off;
	u8 buf[USBD_OTP_MAX_STR_LEN];

	status = OTP_LogicalRead(map, USB_OTP_START, USB_OTP_LEN);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP read fail %d\n", status);
		return -RTK_ERR_IO;
	}

	if ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM) == 0U) {
		/* The strings are chained, each one starting with its own length byte. A length byte
		 * that is still to be read has to stay inside the map, hence the >= below. */
		off = USB_OTP_OFFSET_STR;
		// Mfg string
		mfg_len = map[off];
		if ((mfg_len < 2U) || ((off + mfg_len) >= USB_OTP_LEN)) {
			return -RTK_ERR_BADARG;
		}
		off += mfg_len;
		// Product string
		prod_len = map[off];
		if ((prod_len < 2U) || ((off + prod_len) >= USB_OTP_LEN)) {
			return -RTK_ERR_BADARG;
		}
		off += prod_len;
		// SN string
		sn_len = map[off];
		if ((sn_len < 2U) || ((off + sn_len) > USB_OTP_LEN)) {
			return -RTK_ERR_BADARG;
		}
		usb_os_memcpy((void *)buf, (const void *)&map[off + 2U], sn_len - 2U);
		buf[sn_len - 2U] = '\0';
		usb_os_memcpy((void *)otp->sn_str, (const void *)buf, (size_t)(sn_len - 1U));
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP SN str:%s\n", buf);
		otp->otp_sn = 1U;
	}

	if ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN) != 0U) {
		// VID
		otp->vid = (u16)((u16)map[USB_OTP_OFFSET_VID] | (u16)((u16)map[USB_OTP_OFFSET_VID + 1U] << 8));
		// PID
		otp->pid = (u16)((u16)map[USB_OTP_OFFSET_PID] | (u16)((u16)map[USB_OTP_OFFSET_PID + 1U] << 8));
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP PID-VID: 0x%04x-0x%04x\n", otp->pid, otp->vid);
		off = USB_OTP_OFFSET_STR;
		// Mfg string
		mfg_len = map[off];
		/* The product length byte follows the mfg string, so it has to stay inside the map */
		if ((mfg_len < 2U) || ((off + mfg_len) >= USB_OTP_LEN)) {
			return -RTK_ERR_BADARG;
		}
		usb_os_memcpy((void *)buf, (const void *)&map[off + 2U], mfg_len - 2U);
		buf[mfg_len - 2U] = '\0';
		usb_os_memcpy((void *)otp->mfg_str, (const void *)buf, (size_t)(mfg_len - 1U));
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP MFG str:%s\n", buf);
		off = (u16)(off + mfg_len);
		// Product string
		prod_len = map[off];
		/* Bound by the raw ASCII length */
		if ((prod_len < 2U) || ((off + prod_len) > USB_OTP_LEN)) {
			return -RTK_ERR_BADARG;
		}
		usb_os_memcpy((void *)buf, (const void *)&map[off + 2U], prod_len - 2U);
		buf[prod_len - 2U] = '\0';
		usb_os_memcpy((void *)otp->prod_str, (const void *)buf, (size_t)(prod_len - 1U));
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP PROD str:%s\n", buf);
		otp->otp_param = 1U;
	}

	return RTK_SUCCESS;
}

/* Exported functions --------------------------------------------------------*/

int usbd_otp_init(usbd_otp_t *otp)
{
	int ret = RTK_SUCCESS;
	u32 reg;
	u8 *map;

	otp->otp_sn = 0U;
	otp->otp_param = 0U;
#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB)
	/* The BT interfaces are exposed only when the firmware also runs the BT stack, so gate the
	 * OTP capability bit on the SW configuration too: otherwise the descriptors advertise BT
	 * while USB HCI vendor requests have no handler and STALL. */
	otp->bt_en = ((HAL_READ32(OTPC_REG_BASE, SEC_OTP_SYSCFG0) & SEC_BIT_BT_FUNCTION_EN) == 0U) ? 0U : 1U;
#endif /* CONFIG_BT && CONFIG_BT_INIC_USB */

	otp->mfg_str = (u8 *)usb_os_malloc(USBD_OTP_MAX_STR_LEN + 1U);
	if (otp->mfg_str == NULL) {
		return -RTK_ERR_NOMEM;
	}

	otp->prod_str = (u8 *)usb_os_malloc(USBD_OTP_MAX_STR_LEN + 1U);
	if (otp->prod_str == NULL) {
		usb_os_mfree((void *)otp->mfg_str);
		otp->mfg_str = NULL;
		return -RTK_ERR_NOMEM;
	}

	otp->sn_str = (u8 *)usb_os_malloc(USBD_OTP_MAX_STR_LEN + 1U);
	if (otp->sn_str == NULL) {
		usb_os_mfree((void *)otp->mfg_str);
		otp->mfg_str = NULL;
		usb_os_mfree((void *)otp->prod_str);
		otp->prod_str = NULL;
		return -RTK_ERR_NOMEM;
	}

	reg = HAL_READ32(USB_ADDON_REG_AUTOLOAD_CTRL, 0U);
	otp->self_powered = ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SELF_POWER_EN) == 0U) ? 0U : 1U;
	otp->remote_wakeup_en = ((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_REMOTE_WAKEUP) == 0U) ? 0U : 1U;
	if (((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN) != 0U) ||
		((reg & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SQNUM_ROM) == 0U)) {
		/* The raw map is a scratch buffer: everything kept afterwards is copied out of it */
		map = (u8 *)usb_os_malloc(USB_OTP_LEN);
		if (map == NULL) {
			usb_os_mfree((void *)otp->mfg_str);
			otp->mfg_str = NULL;
			usb_os_mfree((void *)otp->prod_str);
			otp->prod_str = NULL;
			usb_os_mfree((void *)otp->sn_str);
			otp->sn_str = NULL;
			return -RTK_ERR_NOMEM;
		}

		ret = usbd_otp_parse_map(otp, map, reg);
		usb_os_mfree((void *)map);
	}

	return ret;
}

void usbd_otp_deinit(usbd_otp_t *otp)
{
	usb_os_mfree((void *)otp->mfg_str);
	otp->mfg_str = NULL;
	usb_os_mfree((void *)otp->prod_str);
	otp->prod_str = NULL;
	usb_os_mfree((void *)otp->sn_str);
	otp->sn_str = NULL;
}
