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
/* A chained string entry starts with bLength + bDescriptorType */
#define USB_OTP_STR_HDR_LEN        2U

#ifdef CONFIG_RLE1509
#define USB_OTP_START              USB_OTP_INFO_OFFSET
#define USB_OTP_LEN                USB_OTP_INFO_LEN
/* Chars of an entry filling the whole string area and the terminator */
#define USB_OTP_STR_BUF_LEN        (USB_OTP_STR_LEN - USB_OTP_STR_HDR_LEN + 1U)
#else
#define USB_OTP_START              0x2A0U
#define USB_OTP_LEN                64U
#define USB_OTP_STR_START          0x2A5U
#define USB_OTP_OFFSET_VID         (0x2A0U - USB_OTP_START)
#define USB_OTP_OFFSET_PID         (0x2A2U - USB_OTP_START)
#define USB_OTP_OFFSET_STR         (USB_OTP_STR_START - USB_OTP_START)
#define USB_OTP_STR_BUF_LEN        (USBD_OTP_MAX_STR_LEN + 1U)
#endif

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int usbd_whc_otp_get_ctrl(u8 *ctrl);
static int usbd_whc_otp_parse_map(usbd_whc_otp_t *otp, u8 *map, u8 ctrl);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "WHC";

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

#ifdef CONFIG_RLE1509

/**
  * @brief  Read the identity control byte from the OTP info block
  * @param  ctrl: Control byte, 0 when the read fails or the byte is unprogrammed
  * @retval Status; -RTK_ERR_IO when the OTP cannot be read
  */
static int usbd_whc_otp_get_ctrl(u8 *ctrl)
{
	u8 ctrl00 = 0x00U;
	u8 ctrl_ff = 0xFFU;

	if ((OTP_LogicalReadRaw(&ctrl00, USB_OTP_INFO_CTRL_OFFSET, 1U) != RTK_SUCCESS) ||
		(OTP_LogicalReadRaw(&ctrl_ff, USB_OTP_INFO_CTRL_OFFSET, 1U) != RTK_SUCCESS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP ctrl read fail\n");
		*ctrl = 0x00U;
		return -RTK_ERR_IO;
	}

	/* A blank byte keeps its prefill, so the two reads differ; that alone decides it, no 0xFF test */
	*ctrl = (ctrl00 == ctrl_ff) ? ctrl00 : 0x00U;

	return RTK_SUCCESS;
}

/**
  * @brief  Get the length of a string entry of the OTP info block
  * @param  map: OTP info block
  * @param  off: Index of the entry in the block
  * @retval Entry length including its header, 0 for a blank or malformed entry
  * @note   A malformed entry ends the chain just like a blank one does, matching the loader and ROM:
  *         nothing past it can be located, so the entries behind it are simply not adopted.
  */
static u8 usbd_whc_otp_get_str_entry_len(const u8 *map, u8 off)
{
	u8 len = map[off];

	/* A blank (0xFF) or zero length byte only means no entry here, so stay quiet; a malformed entry is reported
	 * but ends the chain the same way, which is what the loader and ROM do */
	if ((len == 0xFFU) || (len == 0U)) {
		return 0U;
	}

	/* Any length is fine as long as the entry stays inside the string area */
	if ((len < USB_OTP_STR_HDR_LEN) || ((u16)(off + len) > (USB_OTP_STR_IDX + USB_OTP_STR_LEN)) || (map[off + 1U] != USB_DESC_TYPE_STRING)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid OTP str len %d at %d\n", len, off);
		len = 0U;
	}

	return len;
}

/* Copy the chars of an entry into dst; NULL for an absent entry gives the empty string */
static void usbd_whc_otp_copy_str(u8 *dst, const u8 *entry)
{
	u8 len = (entry != NULL) ? (u8)(entry[0] - USB_OTP_STR_HDR_LEN) : 0U;

	if (len != 0U) {
		usb_os_memcpy((void *)dst, (const void *)&entry[USB_OTP_STR_HDR_LEN], len);
	}
	dst[len] = '\0';
}

/* Fixed-offset info block: VID/PID at the head, then the chained MFG, PROD and SN entries.
 * VID_PID_EN, STRING_EN and SN_EN are three independent gates, and a programmed gate always takes its item over
 * from the built-in default. An item holding no valid value (blank, malformed, header-only, or starting with a NUL)
 * is logged and left as the empty string, which the core reports as no string (index 0); so is one behind a
 * broken chain, as nothing past a bad entry can be located. The other gates' items are unaffected. */
static int usbd_whc_otp_parse_info_block(usbd_whc_otp_t *otp, const u8 *map, u8 ctrl)
{
	u8 mfg_len;
	u8 prod_len;
	u8 sn_len;
	u8 off;

	if ((ctrl & USB_OTP_CTRL_BIT_VID_PID_EN) != 0U) {
		otp->vid = (u16)((u16)map[USB_OTP_VID_IDX] | (u16)((u16)map[USB_OTP_VID_IDX + 1U] << 8));
		otp->pid = (u16)((u16)map[USB_OTP_PID_IDX] | (u16)((u16)map[USB_OTP_PID_IDX + 1U] << 8));
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP PID-VID: 0x%04x-0x%04x\n", otp->pid, otp->vid);
		/* 0xFFFF is a valid value too: VID and PID are programmed together */
		otp->vid_pid = 1U;
	}

	if ((ctrl & (USB_OTP_CTRL_BIT_STRING_EN | USB_OTP_CTRL_BIT_SN_EN)) == 0U) {
		return RTK_SUCCESS;
	}

	/* An unprogrammed MFG/PROD is a header-only 02 03 placeholder (length 2), so the chain still walks on */
	mfg_len = usbd_whc_otp_get_str_entry_len(map, USB_OTP_STR_IDX);
	prod_len = (mfg_len != 0U) ? usbd_whc_otp_get_str_entry_len(map, (u8)(USB_OTP_STR_IDX + mfg_len)) : 0U;

	if ((ctrl & USB_OTP_CTRL_BIT_STRING_EN) != 0U) {
		usbd_whc_otp_copy_str(otp->mfg_str, (mfg_len != 0U) ? &map[USB_OTP_STR_IDX] : NULL);
		usbd_whc_otp_copy_str(otp->prod_str, (prod_len != 0U) ? &map[USB_OTP_STR_IDX + mfg_len] : NULL);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP MFG str:%s\n", otp->mfg_str);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP PROD str:%s\n", otp->prod_str);
		if ((otp->mfg_str[0] == '\0') || (otp->prod_str[0] == '\0')) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP MFG/PROD empty\n");
		}
		otp->mfg_prod = 1U;
	}

	if ((ctrl & USB_OTP_CTRL_BIT_SN_EN) != 0U) {
		/* Behind a broken MFG/PROD the SN cannot be located */
		off = (u8)(USB_OTP_STR_IDX + mfg_len + prod_len);
		sn_len = (prod_len != 0U) ? usbd_whc_otp_get_str_entry_len(map, off) : 0U;
		usbd_whc_otp_copy_str(otp->sn_str, (sn_len != 0U) ? &map[off] : NULL);
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP SN str:%s\n", otp->sn_str);
		if (otp->sn_str[0] == '\0') {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP SN empty\n");
		}
		otp->sn = 1U;
	}

	return RTK_SUCCESS;
}

#else

/**
  * @brief  Get the identity control byte: the autoload shadow, or the OTP flag bits when the shadow lost its content
  * @param  ctrl: Control byte: the shadow value, replaced by the OTP byte when their defined bits differ
  * @retval Status; -RTK_ERR_IO when the OTP cannot be read
  * @note   OTP byte 0x2A4 is the AUTOLOAD_CTRL value the shadow was loaded from, so the two are compared over
  *         the defined bits (the reserved [7:6] are ignored): equal means the shadow still holds it, any
  *         difference means the shadow lost its content (PG resume resets it to DESC_EN=0, SN_FROM_ROM=1) and
  *         the OTP byte replaces it outright. A blank byte is unprogrammed and never replaces anything.
  *         A failed OTP read fails the OTP tier only: the power bits stay as the shadow reports them, while the
  *         caller skips the identity items. The UPHY path in amebagreen2_usb.c reads the same byte with the
  *         same programming test.
  */
static int usbd_whc_otp_get_ctrl(u8 *ctrl)
{
	int ret;
	u8 flag;

	*ctrl = HAL_READ8(USB_ADDON_REG_AUTOLOAD_CTRL, 0U);

	ret = OTP_LogicalRead(&flag, USB_OTP_UPHY_FLAG_OFFSET, 1U);
	if (ret != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP flag read fail %d\n", ret);
		return -RTK_ERR_IO;
	}

	/* The byte at 0x2A4 is the AUTOLOAD_CTRL value the shadow was loaded from: a difference over the defined
	 * bits means the shadow lost its content, so the OTP wins.
	 * A blank flag is unprogrammed, not a value: the efuse skips a 0xFF byte, so a VID/PID-only burn leaves it
	 * at 0xFF, whose DESC_EN and SN_FROM_ROM would send the caller down a chain that was never burned. */
	if ((flag != USB_OTP_UPHY_FLAG_BLANK) && (((*ctrl ^ flag) & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_MASK) != 0U)) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "OTP flag 0x%x overrides shadow 0x%x\n", flag, *ctrl);
		*ctrl = flag;
	}

	return RTK_SUCCESS;
}

/* Advance *off past the entry it points at, checking that the entry stays inside the map.
 * last=0: the next entry's length byte must stay inside the map, hence the strict bound.
 * last=1: the entry may end on the map's last byte. */
static int usbd_whc_otp_skip_str(const u8 *map, u16 *off, u8 last)
{
	u8 len = map[*off];

	if ((len < USB_OTP_STR_HDR_LEN) || ((u16)(*off + len) > (u16)(last ? USB_OTP_LEN : (USB_OTP_LEN - 1U)))) {
		return -RTK_ERR_BADARG;
	}

	*off = (u16)(*off + len);

	return RTK_SUCCESS;
}

/* Copy the chars of the entry at *off into dst and advance *off past it */
static int usbd_whc_otp_take_str(const u8 *map, u16 *off, u8 *dst, u8 last)
{
	u16 start = *off;
	u8 len = map[start];
	int ret;

	ret = usbd_whc_otp_skip_str(map, off, last);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	usb_os_memcpy((void *)dst, (const void *)&map[start + USB_OTP_STR_HDR_LEN], (size_t)(len - USB_OTP_STR_HDR_LEN));
	dst[len - USB_OTP_STR_HDR_LEN] = '\0';

	return RTK_SUCCESS;
}

/* Autoload shadow layout: the DESC_EN gated VID/PID plus MFG/PROD entries, and the SN_FROM_ROM gated SN entry
 * chained behind them. Two groups are adopted on their own, as the Green2 bootloader does: VID/PID/MFG/PROD
 * together, and the SN. An enabled group that holds no valid value (an empty MFG/PROD or a blank 0xFFFF ID for the
 * first, an SN entry outside the map for the second) is logged and not adopted, so the caller keeps its own
 * default for it, and the other group is unaffected. A MFG/PROD entry that cannot be walked ends the chain, so
 * the SN behind it is lost as well. There is no double read to tell a programmed 0xFFFF from a blank one here,
 * so 0xFFFF counts as blank. A header-only SN is a valid empty SN. */
static int usbd_whc_otp_parse_autoload(usbd_whc_otp_t *otp, const u8 *map, u8 ctrl)
{
	u8 sn_en = ((ctrl & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SN_FROM_ROM) == 0U) ? 1U : 0U;
	u16 off = USB_OTP_OFFSET_STR;
	int ret = RTK_SUCCESS;

	/* The strings are chained, each one starting with its own length byte, and the SN is last: MFG/PROD are walked
	 * even when only the SN is enabled, as the SN cannot be located without them */
	if ((usbd_whc_otp_take_str(map, &off, otp->mfg_str, 0U) != RTK_SUCCESS) ||
		(usbd_whc_otp_take_str(map, &off, otp->prod_str, 1U) != RTK_SUCCESS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid OTP str chain\n");
		return -RTK_ERR_BADARG;
	}

	if (sn_en != 0U) {
		/* PROD may end on the map's last byte, which leaves no length byte for the SN */
		if ((off >= USB_OTP_LEN) || (usbd_whc_otp_take_str(map, &off, otp->sn_str, 1U) != RTK_SUCCESS)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid OTP SN\n");
			ret = -RTK_ERR_BADARG;
		} else {
			if (otp->sn_str[0] == '\0') {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP SN empty\n");
			}
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP SN str:%s\n", otp->sn_str);
			otp->sn = 1U;
		}
	}

	if ((ctrl & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN) != 0U) {
		u16 vid = (u16)((u16)map[USB_OTP_OFFSET_VID] | (u16)((u16)map[USB_OTP_OFFSET_VID + 1U] << 8));
		u16 pid = (u16)((u16)map[USB_OTP_OFFSET_PID] | (u16)((u16)map[USB_OTP_OFFSET_PID + 1U] << 8));

		/* A half-programmed pair or an empty MFG/PROD is unprogrammed, and the ID is then not stored: otp->vid/pid stay 0 */
		if ((vid == 0xFFFFU) || (pid == 0xFFFFU) || (otp->mfg_str[0] == '\0') || (otp->prod_str[0] == '\0')) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid OTP desc\n");
			ret = -RTK_ERR_BADARG;
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP PID-VID: 0x%04x-0x%04x\n", pid, vid);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP MFG str:%s\n", otp->mfg_str);
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Get OTP PROD str:%s\n", otp->prod_str);
			otp->vid = vid;
			otp->pid = pid;
			otp->vid_pid = 1U;
			otp->mfg_prod = 1U;
		}
	}

	return ret;
}

#endif

/**
  * @brief  Read the raw OTP map and extract the identity parameters from it
  * @param  otp: OTP parameter block, whose string buffers are already allocated
  * @param  map: Scratch buffer of USB_OTP_LEN bytes
  * @param  ctrl: Identity control byte from usbd_whc_otp_get_ctrl()
  * @retval Status; -RTK_ERR_IO when the map cannot be read and nothing is adopted, -RTK_ERR_BADARG when an enabled
  *         autoload group is broken or empty: that group is not adopted and the caller keeps its own default for it
  */
static int usbd_whc_otp_parse_map(usbd_whc_otp_t *otp, u8 *map, u8 ctrl)
{
	int status;

	status = OTP_LogicalRead(map, USB_OTP_START, USB_OTP_LEN);
	if (status != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "OTP read fail %d\n", status);
		return -RTK_ERR_IO;
	}

#ifdef CONFIG_RLE1509
	return usbd_whc_otp_parse_info_block(otp, map, ctrl);
#else
	return usbd_whc_otp_parse_autoload(otp, map, ctrl);
#endif
}

/* Whether the control byte asks for any OTP-supplied identity item */
static u8 usbd_whc_otp_info_requested(u8 ctrl)
{
#ifdef CONFIG_RLE1509
	return ((ctrl & (USB_OTP_CTRL_BIT_VID_PID_EN | USB_OTP_CTRL_BIT_STRING_EN | USB_OTP_CTRL_BIT_SN_EN)) != 0U) ? 1U : 0U;
#else
	return ((((ctrl & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_AUTOLOAD_DESC_EN) != 0U) ||
			 (((ctrl & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SN_FROM_ROM) == 0U))) ? 1U : 0U);
#endif
}

/* Map the control byte onto the power attributes, whose bit positions differ per IC */
static void usbd_whc_otp_apply_power(usbd_whc_otp_t *otp, u8 ctrl)
{
#ifdef CONFIG_RLE1509
	otp->self_powered = ((ctrl & USB_OTP_CTRL_BIT_SELF_POWERED) == 0U) ? 0U : 1U;
	otp->remote_wakeup_en = ((ctrl & USB_OTP_CTRL_BIT_REMOTE_WAKEUP_DIS) == 0U) ? 1U : 0U;
#else
	otp->self_powered = ((ctrl & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_SELF_POWER_EN) == 0U) ? 0U : 1U;
	otp->remote_wakeup_en = ((ctrl & USB_ADDON_REG_AUTOLOAD_CTRL_BIT_REMOTE_WAKEUP) == 0U) ? 0U : 1U;
#endif
}

/* Exported functions --------------------------------------------------------*/

int usbd_whc_otp_init(usbd_whc_otp_t *otp)
{
	int status;
	int ret = RTK_SUCCESS;
	u8 *map;
	u8 ctrl;

	otp->sn = 0U;
	otp->mfg_prod = 0U;
	otp->vid_pid = 0U;
	otp->vid = 0U;
	otp->pid = 0U;
#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC_USB)
	/* The BT interfaces are exposed only when the firmware also runs the BT stack, so gate the
	 * OTP capability bit on the SW configuration too: otherwise the descriptors advertise BT
	 * while USB HCI vendor commands have no handler and STALL. */
	otp->bt_en = ((HAL_READ32(OTPC_REG_BASE, SEC_OTP_SYSCFG0) & SEC_BIT_BT_FUNCTION_EN) == 0U) ? 0U : 1U;
#endif /* CONFIG_BT && CONFIG_BT_INIC_USB */

	/* Before the allocations: the caller applies the power attributes even on -RTK_ERR_NOMEM.
	 * ctrl always holds the shadow byte, so its power bits apply even when the OTP tier failed. */
	ret = usbd_whc_otp_get_ctrl(&ctrl);
	usbd_whc_otp_apply_power(otp, ctrl);

	otp->mfg_str = (u8 *)usb_os_malloc(USB_OTP_STR_BUF_LEN);
	otp->prod_str = (u8 *)usb_os_malloc(USB_OTP_STR_BUF_LEN);
	otp->sn_str = (u8 *)usb_os_malloc(USB_OTP_STR_BUF_LEN);
	if ((otp->mfg_str == NULL) || (otp->prod_str == NULL) || (otp->sn_str == NULL)) {
		/* deinit frees whichever allocations did succeed and clears them */
		usbd_whc_otp_deinit(otp);
		return -RTK_ERR_NOMEM;
	}

	/* The OTP tier already failed: its map is what could not be read, so no item is taken from it */
	if ((ret == RTK_SUCCESS) && usbd_whc_otp_info_requested(ctrl)) {
		/* The raw map is a scratch buffer: everything kept afterwards is copied out of it */
		map = (u8 *)usb_os_malloc(USB_OTP_LEN);
		if (map == NULL) {
			usbd_whc_otp_deinit(otp);
			return -RTK_ERR_NOMEM;
		}

		status = usbd_whc_otp_parse_map(otp, map, ctrl);
		if (status != RTK_SUCCESS) {
			/* Only the failed group is missing: the flags of the items that were adopted stay set */
			ret = status;
		}
		usb_os_mfree((void *)map);
	}

	return ret;
}

void usbd_whc_otp_deinit(usbd_whc_otp_t *otp)
{
	usb_os_mfree((void *)otp->mfg_str);
	otp->mfg_str = NULL;
	usb_os_mfree((void *)otp->prod_str);
	otp->prod_str = NULL;
	usb_os_mfree((void *)otp->sn_str);
	otp->sn_str = NULL;
}
