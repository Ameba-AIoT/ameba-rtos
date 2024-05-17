/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <mouse_des.h>
#include <rtk_bt_common.h>
#include <rtk_bt_sdp.h>

const uint8_t hid_mouse_sdp_record[] = {
	//Total length
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR_2BYTE,
	0x01,
	0x00,

	//Attribute SDP_ATTR_SRV_CLASS_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_SRV_CLASS_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
	(uint8_t)(RTK_BT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE),

	//Attribute SDP_ATTR_PROTO_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROTO_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_PROTO_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x0D,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_L2CAP >> 8),
	(uint8_t)(RTK_BT_UUID_L2CAP),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_PSM_HID_CONTROL >> 8),
	(uint8_t)(RTK_BT_PSM_HID_CONTROL),
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_HIDP >> 8),
	(uint8_t)(RTK_BT_UUID_HIDP),

	//Attribute SDP_ATTR_LANG_BASE_ATTR_ID_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_LANG_BASE_ATTR_ID_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x09,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_LANG_ENGLISH >> 8),
	(uint8_t)(RTK_BT_SDP_LANG_ENGLISH),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_CHARACTER_UTF8 >> 8),
	(uint8_t)(RTK_BT_SDP_CHARACTER_UTF8),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_BASE_LANG_OFFSET >> 8),
	(uint8_t)(RTK_BT_SDP_BASE_LANG_OFFSET),

	//Attribute SDP_ATTR_PROFILE_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_PROFILE_DESC_LIST >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_PROFILE_DESC_LIST),
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x08,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE >> 8),
	(uint8_t)(RTK_BT_UUID_HUMAN_INTERFACE_DEVICE_SERVICE),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0101 >> 8),
	(uint8_t)(0x0101),

	//Attribute SDP_ATTR_ADD_PROTO_DESC_LIST
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_SDP_ATTR_ADD_PROTO_DESC_LIST >> 8),
	(uint8_t)RTK_BT_SDP_ATTR_ADD_PROTO_DESC_LIST,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x0F,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x0D,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(0x0100 >> 8),
	(uint8_t)(0x0100),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(RTK_BT_PSM_HID_INTERRUPT >> 8),
	(uint8_t)(RTK_BT_PSM_HID_INTERRUPT),
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x03,
	RTK_BT_SDP_UUID16_HDR,
	(uint8_t)(RTK_BT_UUID_HIDP >> 8),
	(uint8_t)(RTK_BT_UUID_HIDP),

	//Attribute SDP_ATTR_SRV_NAME
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	RTK_BT_SDP_STRING_HDR,
	0x0B,
	'R', 'e', 'a', 'l', 't', 'e', 'k', ' ', 'H', 'I', 'D',

	//Attribute SDP_ATTR_SRV_DESC
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SRV_DESC + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_DESC + RTK_BT_SDP_BASE_LANG_OFFSET),
	RTK_BT_SDP_STRING_HDR,
	0x10,
	'(', '3', ')', ' ', 'B', 'u', 't', 't', 'o', 'n', ' ', 'M', 'o', 'u', 's', 'e',

	//Attribute SDP_ATTR_PROVIDER_NAME
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_PROVIDER_NAME + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_PROVIDER_NAME + RTK_BT_SDP_BASE_LANG_OFFSET),
	RTK_BT_SDP_STRING_HDR,
	0x07,
	'R', 'e', 'a', 'l', 't', 'e', 'k',

	//Attribute HIDParserVersion
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0201 >> 8),
	(uint8_t)(0x0201),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0111 >> 8),
	(uint8_t)(0x0111),

	//Attribute HIDDeviceSubclass
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0202 >> 8),
	(uint8_t)(0x0202),
	RTK_BT_SDP_UNSIGNED_ONE_BYTE,
	0x80,

	//Attribute HIDCountryCode
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0203 >> 8),
	(uint8_t)(0x0203),
	RTK_BT_SDP_UNSIGNED_ONE_BYTE,
	0x21,

	//Attribute HIDVirtualCable
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0204 >> 8),
	(uint8_t)(0x0204),
	RTK_BT_SDP_BOOL_ONE_BYTE,
	0x01,

	//Attribute HIDReconnectInitiate
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0205 >> 8),
	(uint8_t)(0x0205),
	RTK_BT_SDP_BOOL_ONE_BYTE,
	0x01,

	//Attribute HIDDescriptorList
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0206 >> 8),
	(uint8_t)(0x0206),
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	//0x38,
	0x46,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	//0x36,
	0x44,
	RTK_BT_SDP_UNSIGNED_ONE_BYTE,
	0x22,                   /* Type = Report Descriptor */
	RTK_BT_SDP_STRING_HDR,
	//0x32,
	0x40,
	0x05, 0x01,             /* USAGE_PAGE (Generic Desktop) */
	0x09, 0x02,             /* USAGE (Mouse) */
	0xA1, 0x01,             /* COLLECTION (Application) */
	0x85, 0x02,             /* REPORT_ID         (2)           */
	0x09, 0x01,             /*   USAGE (Pointer) */
	0xA1, 0x00,             /*   COLLECTION (Physical) */
	0x05, 0x09,             /*     USAGE_PAGE (Button) */
	0x19, 0x01,             /*     USAGE_MINIMUM (Button 1) */
	0x29, 0x03,             /*     USAGE_MAXIMUM (Button 3) */
	0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
	0x25, 0x01,             /*     LOGICAL_MAXIMUM (1) */
	0x95, 0x03,             /*     REPORT_COUNT (3) */
	0x75, 0x01,             /*     REPORT_SIZE (1) */
	0x81, 0x02,             /*     INPUT (Data,Var,Abs) */
	0x95, 0x01,             /*     REPORT_COUNT (1) */
	0x75, 0x05,             /*     REPORT_SIZE (5) */
	0x81, 0x03,             /*     INPUT (Cnst,Var,Abs) */
	0x05, 0x01,             /*     USAGE_PAGE (Generic Desktop) */
	0x09, 0x30,             /*     USAGE (X) */
	0x09, 0x31,             /*     USAGE (Y) */
	0x15, 0x81,             /*     LOGICAL_MINIMUM (-127) */
	0x25, 0x7f,             /*     LOGICAL_MAXIMUM (127) */
	0x75, 0x08,             /*     REPORT_SIZE (8) */
	0x95, 0x02,             /*     REPORT_COUNT (2) */
	0x81, 0x06,             /*     INPUT (Data,Var,Rel) */
	0x09, 0x38,             /*     USAGE (Wheel) */
	0x15, 0x81,             /*     LOGICAL_MINIMUM (-127) */
	0x25, 0x7f,             /*     LOGICAL_MAXIMUM (127) */
	0x75, 0x08,             /*     REPORT_SIZE (8) */
	0x95, 0x01,             /*     REPORT_COUNT (1) */
	0x81, 0x06,             /*     INPUT (Data,Var,Rel) */
	0xC0,                   /*   END_COLLECTION */
	0xC0,                   /* END_COLLECTION */

	//Attribute HIDLANGIDBaseList
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0207 >> 8),
	(uint8_t)(0x0207),
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x08,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR,
	0x06,
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0409 >> 8),
	(uint8_t)(0x0409),
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0100 >> 8),
	(uint8_t)(0x0100),

	//Attribute HIDBatteryPower
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x0209 >> 8),
	(uint8_t)(0x0209),
	RTK_BT_SDP_BOOL_ONE_BYTE,
	0x01,

	//Attribute HIDRemoteWake
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x020A >> 8),
	(uint8_t)(0x020A),
	RTK_BT_SDP_BOOL_ONE_BYTE,
	0x01,

	//Attribute HIDNormallyConnectable
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x020D >> 8),
	(uint8_t)(0x020D),
	RTK_BT_SDP_BOOL_ONE_BYTE,
	0x01,

	//Attribute HIDBootDevice
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)(0x020E >> 8),
	(uint8_t)(0x020E),
	RTK_BT_SDP_BOOL_ONE_BYTE,
	0x01,
};

const uint8_t hid_descriptor_mouse_boot_mode[] = {
	0x05, 0x01,             /* USAGE_PAGE (Generic Desktop) */
	0x09, 0x02,             /* USAGE (Mouse) */
	0xA1, 0x01,             /* COLLECTION (Application) */

	0x85, 0x02,             /* REPORT_ID         (2)           */
	0x09, 0x01,             /*   USAGE (Pointer) */
	0xA1, 0x00,             /*   COLLECTION (Physical) */

	0x05, 0x09,             /*     USAGE_PAGE (Button) */
	0x19, 0x01,             /*     USAGE_MINIMUM (Button 1) */
	0x29, 0x03,             /*     USAGE_MAXIMUM (Button 3) */
	0x15, 0x00,             /*     LOGICAL_MINIMUM (0) */
	0x25, 0x01,             /*     LOGICAL_MAXIMUM (1) */
	0x95, 0x03,             /*     REPORT_COUNT (3) */
	0x75, 0x01,             /*     REPORT_SIZE (1) */
	0x81, 0x02,             /*     INPUT (Data,Var,Abs) */
	0x95, 0x01,             /*     REPORT_COUNT (1) */
	0x75, 0x05,             /*     REPORT_SIZE (5) */
	0x81, 0x03,             /*     INPUT (Cnst,Var,Abs) */

	0x05, 0x01,             /*     USAGE_PAGE (Generic Desktop) */
	0x09, 0x30,             /*     USAGE (X) */
	0x09, 0x31,             /*     USAGE (Y) */
	0x15, 0x81,             /*     LOGICAL_MINIMUM (-127) */
	0x25, 0x7f,             /*     LOGICAL_MAXIMUM (127) */
	0x75, 0x08,             /*     REPORT_SIZE (8) */
	0x95, 0x02,             /*     REPORT_COUNT (2) */
	0x81, 0x06,             /*     INPUT (Data,Var,Rel) */
	0x09, 0x38,             /*     USAGE (Wheel) */
	0x15, 0x81,             /*     LOGICAL_MINIMUM (-127) */
	0x25, 0x7f,             /*     LOGICAL_MAXIMUM (127) */
	0x75, 0x08,             /*     REPORT_SIZE (8) */
	0x95, 0x01,             /*     REPORT_COUNT (1) */
	0x81, 0x06,             /*     INPUT (Data,Var,Rel) */

	0xC0,                   /*   END_COLLECTION */
	0xC0                    /* END_COLLECTION */
};

uint32_t mouse_sdp_record_size = sizeof(hid_mouse_sdp_record);
uint32_t mouse_hid_descriptor_size = sizeof(hid_descriptor_mouse_boot_mode);
