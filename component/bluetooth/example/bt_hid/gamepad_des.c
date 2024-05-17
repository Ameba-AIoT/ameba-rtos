/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <gamepad_des.h>
#include <rtk_bt_common.h>
#include <rtk_bt_sdp.h>

const uint8_t hid_gamepad_sdp_record[] = {
	//Total length
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR_2BYTE,
	0x01,
	0xDE,

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
	0x13,
	'R', 'T', 'K', ' ', 'G', 'a', 'm', 'e', ' ', 'C', 'o', 'n', 't', 'r', 'o', 'l', 'l', 'e', 'r',

	//Attribute SDP_ATTR_SRV_DESC
	RTK_BT_SDP_UNSIGNED_TWO_BYTE,
	(uint8_t)((RTK_BT_SDP_ATTR_SRV_DESC + RTK_BT_SDP_BASE_LANG_OFFSET) >> 8),
	(uint8_t)(RTK_BT_SDP_ATTR_SRV_DESC + RTK_BT_SDP_BASE_LANG_OFFSET),
	RTK_BT_SDP_STRING_HDR,
	0x0F,
	'G', 'a', 'm', 'e', ' ', 'C', 'o', 'n', 't', 'r', 'o', 'l', 'l', 'e', 'r',

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
	0x08,

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
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR_2BYTE,
	//0x38,
	0x01,
	0x23,
	RTK_BT_SDP_DATA_ELEM_SEQ_HDR_2BYTE,
	//0x36,
	0x01,
	0x20,
	RTK_BT_SDP_UNSIGNED_ONE_BYTE,
	0x22,                   /* Type = Report Descriptor */
	0x26,
	0x01,
	0x1B,
	0x05, 0x01,       /* Usage Page (Generic Desktop) */ \
	0x09, 0x05,       /* Usage (Gamepad) */\
	0xA1, 0x01,       /* Collection (Application) */ \
	0x85, HID_GAMEPAD_KEYSTROKE_REPORT_ID, /*  Report ID (1) */
	/* Left Thumbstick */\
	0x09, 0x01,       /* Usage (Pointer) */ \
	0xA1, 0x00,       /* Collection (Physical) */ \
	0x09, 0x30,       /* Usage (X) */ \
	0x09, 0x31,       /* Usage (Y) */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x27, 0xFF, 0xFF, 0x00, 0x00, /* Logical Max (0xFFFF) */ \
	0x95, 0x02,       /* Report Count (2) */ \
	0x75, 0x10,       /* Report Size (16) */ \
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	0xC0, /* End Collection (Thumbstick) */ \
	\
	/* Right Thumbstick */ \
	0x09, 0x01,       /* Usage (Pointer) */ \
	0xA1, 0x00,       /* Collection (Physical) */ \
	0x09, 0x32,       /* Usage (Z)  X and Y for Right thumbstick (16-bit) */ \
	0x09, 0x35,       /* Usage (Rz) */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x27, 0xFF, 0xFF, 0x00, 0x00,  /* Logical Max (0xFFFF) */\
	0x95, 0x02,       /* Report Count (2) */ \
	0x75, 0x10,       /* Report Size (16) */\
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	0xC0,             /* End Collection (Thumbstick) */ \
	\
	/* Left Trigger */ \
	0x05, 0x02,       /* Usage page (Simulation Controls) */ \
	0x09, 0xC5,       /* Usage (Brake) */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x26, 0xFF, 0x03, /* Logical Max (0x3FF) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x75, 0x0A,       /* Report Size (10) */ \
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	/* Padding 6 bits */\
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x75, 0x06,       /* Report Size (6) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x81, 0x03,       /* Input (constant) */ \
	\
	/* Right Trigger */ \
	0x05, 0x02,       /* Usage page (Simulation Controls) */ \
	0x09, 0xC4,       /* Usage (Accelerator)  */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x26, 0xFF, 0x03, /* Logical Max (0x03FF) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x75, 0x0A,       /* Report Size (10) */ \
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	/* Padding 6 bits */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x75, 0x06,       /* Report Size (6) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x81, 0x03,       /* Input (constant) */ \
	\
	/* Hat Switch(DPAD)  */ \
	0x05, 0x01,       /* Usage Page (Generic Desktop) */ \
	0x09, 0x39,       /* Usage, Hat Switch (DPAD)  */ \
	0x15, 0x01,       /* Logical Min (1)   Windows XInput requires 0 for center position. */ \
	0x25, 0x08,       /* Logical Max (8)   4 bits for 8 pressed position in order of "Right  Left  Down  Up" */ \
	/* Logical Min (0)   ipega and NyKo use 8 for center position.Android accepts logical value with unit of Angular.  */ \
	/* Logical Max (7)   0 - 7 for 8 pressed position based on angular 45 degree for each step.  */ \
	/* NOTE: The format used in here works for both Android and Windows. */ \
	0x35, 0x00,       /* Physical Min (0)  logical values from 0 to 10    ===  bit0   bit2  bit1  bit0   */ \
	0x46, 0x3B, 0x01, /* Physical Max (315) */ \
	0x66, 0x14, 0x00, /* Unit (English Rotation: Angular Position [degrees]) */ \
	0x75, 0x04,       /* Report Size (4)  --- 4 buttons */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x81, 0x42,       /* Input (Variable, Null) (DPAD) */ \
	/* Padding 4 bits  */ \
	0x75, 0x04,       /* Report Size (4) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x35, 0x00,       /* Undefine Physical Min/Max (0) */ \
	0x45, 0x00,       /* Undefine Physical Min/Max (0) */ \
	0x65, 0x00,       /* Undefine Unit (none) */ \
	0x81, 0x03,       /* Input (constant) */ \
	\
	/* 15 Digital Buttons: A, B, X, Y, LShd(LeftShoulder)/LB(Left bumper), RShd/RB, View/BACK, Menu/START, LSB(Left stick button)/(Thumbstick Pressed), RSB, LThStk,  RThStk*/ \
	0x05, 0x09,       /* Usage_Page (button) */ \
	0x19, 0x01,       /* Usage Min (Button 1)  */ \
	0x29, 0x0F,       /* Usage Max (Button 15) */ \
	0x15, 0x00,       /* LOGICAL_MINIMUM (0) */ \
	0x25, 0x01,       /* LOGICAL_MAXIMUM (1) */ \
	0x75, 0x01,       /* Report Size (1) */ \
	0x95, 0x0F,       /* Report Count (15) */ \
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	/* Padding 1 bits for D-Pad, Leftshoulder, RightShoulder */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x75, 0x01,       /* Report Size (1) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x81, 0x03,       /* Input (constant) */ \
	/* share button */ \
	0x05, 0x0C,       /* usage_page(Consumer) */ \
	0x0A, 0xB2, 0x00, /* usage(Record) -- share button */ \
	0x15, 0x00,       /* logical min (0) */ \
	0x25, 0x01,       /* logical max (1) */ \
	0x95, 0x01,       /* report count (1) */ \
	0x75, 0x01,       /* report size (1) */ \
	0x81, 0x02,       /* input (data,var,abs) */ \
	/* padding 7 bits */ \
	0x15, 0x00,       /* logical min (0) */ \
	0x25, 0x00,       /* logical max (0) */ \
	0x75, 0x07,       /* report size (7) */ \
	0x95, 0x01,       /* report count (1) */ \
	0x81, 0x03,       /* input (constant) */ \
	0x05, 0x0F,  /* Usage Page (Physical Interface) */ \
	0x09, 0x21,  /* Usage (Set Effect Report) */ \
	0x85, HID_GAMEPAD_MOTOR_CTRL_REPORT_ID,    /*Report ID (3) */ \
	\
	0xA1, 0x02,  /* Collection (Logical) */ \
	0x09, 0x97,  /* Usage (DC enable actuators) */ \
	0x15, 0x00,  /* Logical Minimum (0) */ \
	0x25, 0x01,  /* Logical Maximum (1) */ \
	0x75, 0x04,  /* Report Size (4) */ \
	0x95, 0x01,  /* Report Count(1) */ \
	0x91, 0x02,  /* Output (Data, Variable, Absolute) */ \
	/* Padding 4 bits */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x75, 0x04,       /* Report Size (4) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x91, 0x03,       /* Output (constant) */ \
	\
	0x09, 0x70,       /* Usage (Magnitude in percentage) // left impulse, right impulse, left rumble, right rumble */ \
	0x15, 0x00,       /* Logical Minimum (0) */ \
	0x25, 0x64,       /* Logical Maximum (100) */ \
	0x75, 0x08,       /* Report Size (8) */ \
	0x95, 0x04,       /* Report Count (4) */ \
	0x91, 0x02,       /* Output (Data, Variable, Absolute) */ \
	\
	0x09, 0x50,       /* Usage (Duration) On time duration, in steps of 10ms. */ \
	0x66, 0x01, 0x10, /* Unit time seconds */ \
	0x55, 0x0E,       /* Unit Exponent -2 for 10mS */ \
	0x15, 0x00,       /* Logical Minimum (0) */ \
	0x26, 0xFF, 0x00, /* Logical Maximum (255)  */ \
	0x75, 0x08,       /* Report Size (8) */ \
	0x95, 0x01,       /* Report Count (1)  */ \
	0x91, 0x02,       /* Output (Data, Variable, Absolute) */ \
	\
	0x09, 0xA7,       /* Usage (Start Delay) On time duration, in steps of 10ms. */ \
	0x15, 0x00,       /* Logical Minimum (0) */ \
	0x26, 0xFF, 0x00, /* Logical Maximum (255)   */ \
	0x75, 0x08,       /* Report Size (8) */ \
	0x95, 0x01,       /* Report Count (1)  */ \
	0x91, 0x02,       /* Output (Data, Variable, Absolute)  */ \
	0x65, 0x00,       /* undefine Unit (none) */ \
	0x55, 0x00,       /* undefine Exponent (none) */ \
	\
	0x09, 0x7C,       /* Usage (Loop Count)  Number of times to repeat.  0 = run once. */ \
	0x15, 0x00,       /* Logical Minimum (0) */ \
	0x26, 0xFF, 0x00, /* Logical Maximum (255)  */ \
	0x75, 0x08,       /* Report Size (8) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x91, 0x02,       /* Output (Data, Variable, Absolute)  */ \
	0xC0, /* End collection (Rumble)  */
	0xC0,

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

const uint8_t hid_descriptor_gamepad_boot_mode[] = {
	0x05, 0x01,       /* Usage Page (Generic Desktop) */ \
	0x09, 0x05,       /* Usage (Gamepad) */\
	0xA1, 0x01,       /* Collection (Application) */ \
	0x85, HID_GAMEPAD_KEYSTROKE_REPORT_ID, /*  Report ID (1) */
	/* Left Thumbstick */\
	0x09, 0x01,       /* Usage (Pointer) */ \
	0xA1, 0x00,       /* Collection (Physical) */ \
	0x09, 0x30,       /* Usage (X) */ \
	0x09, 0x31,       /* Usage (Y) */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x27, 0xFF, 0xFF, 0x00, 0x00, /* Logical Max (0xFFFF) */ \
	0x95, 0x02,       /* Report Count (2) */ \
	0x75, 0x10,       /* Report Size (16) */ \
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	0xC0, /* End Collection (Thumbstick) */ \
	\
	/* Right Thumbstick */ \
	0x09, 0x01,       /* Usage (Pointer) */ \
	0xA1, 0x00,       /* Collection (Physical) */ \
	0x09, 0x32,       /* Usage (Z)  X and Y for Right thumbstick (16-bit) */ \
	0x09, 0x35,       /* Usage (Rz) */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x27, 0xFF, 0xFF, 0x00, 0x00,  /* Logical Max (0xFFFF) */\
	0x95, 0x02,       /* Report Count (2) */ \
	0x75, 0x10,       /* Report Size (16) */\
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	0xC0,             /* End Collection (Thumbstick) */ \
	\
	/* Left Trigger */ \
	0x05, 0x02,       /* Usage page (Simulation Controls) */ \
	0x09, 0xC5,       /* Usage (Brake) */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x26, 0xFF, 0x03, /* Logical Max (0x3FF) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x75, 0x0A,       /* Report Size (10) */ \
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	/* Padding 6 bits */\
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x75, 0x06,       /* Report Size (6) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x81, 0x03,       /* Input (constant) */ \
	\
	/* Right Trigger */ \
	0x05, 0x02,       /* Usage page (Simulation Controls) */ \
	0x09, 0xC4,       /* Usage (Accelerator)  */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x26, 0xFF, 0x03, /* Logical Max (0x03FF) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x75, 0x0A,       /* Report Size (10) */ \
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	/* Padding 6 bits */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x75, 0x06,       /* Report Size (6) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x81, 0x03,       /* Input (constant) */ \
	\
	/* Hat Switch(DPAD)  */ \
	0x05, 0x01,       /* Usage Page (Generic Desktop) */ \
	0x09, 0x39,       /* Usage, Hat Switch (DPAD)  */ \
	0x15, 0x01,       /* Logical Min (1)   Windows XInput requires 0 for center position. */ \
	0x25, 0x08,       /* Logical Max (8)   4 bits for 8 pressed position in order of "Right  Left  Down  Up" */ \
	/* Logical Min (0)   ipega and NyKo use 8 for center position.Android accepts logical value with unit of Angular.  */ \
	/* Logical Max (7)   0 - 7 for 8 pressed position based on angular 45 degree for each step.  */ \
	/* NOTE: The format used in here works for both Android and Windows. */ \
	0x35, 0x00,       /* Physical Min (0)  logical values from 0 to 10    ===  bit0   bit2  bit1  bit0   */ \
	0x46, 0x3B, 0x01, /* Physical Max (315) */ \
	0x66, 0x14, 0x00, /* Unit (English Rotation: Angular Position [degrees]) */ \
	0x75, 0x04,       /* Report Size (4)  --- 4 buttons */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x81, 0x42,       /* Input (Variable, Null) (DPAD) */ \
	/* Padding 4 bits  */ \
	0x75, 0x04,       /* Report Size (4) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x35, 0x00,       /* Undefine Physical Min/Max (0) */ \
	0x45, 0x00,       /* Undefine Physical Min/Max (0) */ \
	0x65, 0x00,       /* Undefine Unit (none) */ \
	0x81, 0x03,       /* Input (constant) */ \
	\
	/* 15 Digital Buttons: A, B, X, Y, LShd(LeftShoulder)/LB(Left bumper), RShd/RB, View/BACK, Menu/START, LSB(Left stick button)/(Thumbstick Pressed), RSB, LThStk,  RThStk*/ \
	0x05, 0x09,       /* Usage_Page (button) */ \
	0x19, 0x01,       /* Usage Min (Button 1)  */ \
	0x29, 0x0F,       /* Usage Max (Button 15) */ \
	0x15, 0x00,       /* LOGICAL_MINIMUM (0) */ \
	0x25, 0x01,       /* LOGICAL_MAXIMUM (1) */ \
	0x75, 0x01,       /* Report Size (1) */ \
	0x95, 0x0F,       /* Report Count (15) */ \
	0x81, 0x02,       /* Input (Data,Var,Abs) */ \
	/* Padding 1 bits for D-Pad, Leftshoulder, RightShoulder */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x75, 0x01,       /* Report Size (1) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x81, 0x03,       /* Input (constant) */ \
	/* share button */ \
	0x05, 0x0C,       /* usage_page(Consumer) */ \
	0x0A, 0xB2, 0x00, /* usage(Record) -- share button */ \
	0x15, 0x00,       /* logical min (0) */ \
	0x25, 0x01,       /* logical max (1) */ \
	0x95, 0x01,       /* report count (1) */ \
	0x75, 0x01,       /* report size (1) */ \
	0x81, 0x02,       /* input (data,var,abs) */ \
	/* padding 7 bits */ \
	0x15, 0x00,       /* logical min (0) */ \
	0x25, 0x00,       /* logical max (0) */ \
	0x75, 0x07,       /* report size (7) */ \
	0x95, 0x01,       /* report count (1) */ \
	0x81, 0x03,       /* input (constant) */ \
	0x05, 0x0F,  /* Usage Page (Physical Interface) */ \
	0x09, 0x21,  /* Usage (Set Effect Report) */ \
	0x85, HID_GAMEPAD_MOTOR_CTRL_REPORT_ID,    /*Report ID (3) */ \
	\
	0xA1, 0x02,  /* Collection (Logical) */ \
	0x09, 0x97,  /* Usage (DC enable actuators) */ \
	0x15, 0x00,  /* Logical Minimum (0) */ \
	0x25, 0x01,  /* Logical Maximum (1) */ \
	0x75, 0x04,  /* Report Size (4) */ \
	0x95, 0x01,  /* Report Count(1) */ \
	0x91, 0x02,  /* Output (Data, Variable, Absolute) */ \
	/* Padding 4 bits */ \
	0x15, 0x00,       /* Logical Min (0) */ \
	0x25, 0x00,       /* Logical Max (0) */ \
	0x75, 0x04,       /* Report Size (4) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x91, 0x03,       /* Output (constant) */ \
	\
	0x09, 0x70,       /* Usage (Magnitude in percentage) // left impulse, right impulse, left rumble, right rumble */ \
	0x15, 0x00,       /* Logical Minimum (0) */ \
	0x25, 0x64,       /* Logical Maximum (100) */ \
	0x75, 0x08,       /* Report Size (8) */ \
	0x95, 0x04,       /* Report Count (4) */ \
	0x91, 0x02,       /* Output (Data, Variable, Absolute) */ \
	\
	0x09, 0x50,       /* Usage (Duration) On time duration, in steps of 10ms. */ \
	0x66, 0x01, 0x10, /* Unit time seconds */ \
	0x55, 0x0E,       /* Unit Exponent -2 for 10mS */ \
	0x15, 0x00,       /* Logical Minimum (0) */ \
	0x26, 0xFF, 0x00, /* Logical Maximum (255)  */ \
	0x75, 0x08,       /* Report Size (8) */ \
	0x95, 0x01,       /* Report Count (1)  */ \
	0x91, 0x02,       /* Output (Data, Variable, Absolute) */ \
	\
	0x09, 0xA7,       /* Usage (Start Delay) On time duration, in steps of 10ms. */ \
	0x15, 0x00,       /* Logical Minimum (0) */ \
	0x26, 0xFF, 0x00, /* Logical Maximum (255)   */ \
	0x75, 0x08,       /* Report Size (8) */ \
	0x95, 0x01,       /* Report Count (1)  */ \
	0x91, 0x02,       /* Output (Data, Variable, Absolute)  */ \
	0x65, 0x00,       /* undefine Unit (none) */ \
	0x55, 0x00,       /* undefine Exponent (none) */ \
	\
	0x09, 0x7C,       /* Usage (Loop Count)  Number of times to repeat.  0 = run once. */ \
	0x15, 0x00,       /* Logical Minimum (0) */ \
	0x26, 0xFF, 0x00, /* Logical Maximum (255)  */ \
	0x75, 0x08,       /* Report Size (8) */ \
	0x95, 0x01,       /* Report Count (1) */ \
	0x91, 0x02,       /* Output (Data, Variable, Absolute)  */ \
	0xC0, /* End collection (Rumble)  */
	0xC0,
};

uint32_t gamepad_sdp_record_size = sizeof(hid_gamepad_sdp_record);
uint32_t gamepad_hid_descriptor_size = sizeof(hid_descriptor_gamepad_boot_mode);
