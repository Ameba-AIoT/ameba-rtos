/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#ifndef _GAMEPAD_DES_H_
#define _GAMEPAD_DES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

enum GAMEPAD_HID_REPORT_IDS {
	HID_GAMEPAD_KEYSTROKE_REPORT_ID      = 0x01,
	HID_GAMEPAD_SYS_COMMANDS_REPORT_ID   = 0x02,
	HID_GAMEPAD_MOTOR_CTRL_REPORT_ID     = 0x03,
	HID_GAMEPAD_BATTERY_STATE_REPORT_ID  = 0x04,
	// CUSTOM_MODES
	HID_KEYBOARD_REPORT_ID               = 0x05,
	HID_SET_CUSTOM_MODE_REPORT_ID        = 0x06,
	HID_SET_CUSTOM_MODE_RESP_REPORT_ID   = 0x07,
	HID_GET_CUSTOM_MODE_REPORT_ID        = 0x08,
	HID_GET_CUSTOM_MODE_RESP_REPORT_ID   = 0x09,
	HID_EXTGAMEPAD_STATUS_RESP_REPORT_ID   = 0x0A,
	HID_QUERY_EXTGAMEPAD_STATUS_RESP_REPORT_ID   = 0x0B,
};

/**
 * @struct    rtk_hid_digital_buttons_t
 * @brief     hid gamepad digital buttons struct.
 */
typedef union {
	struct {
		uint16_t A: 1;                                  /*!< Bit0: digital button A */
		uint16_t B: 1;                                  /*!< Bit1: digital button B */
		uint16_t X: 1;                                  /*!< Bit2: digital button X */
		uint16_t Y: 1;                                  /*!< Bit3: digital button Y */

		uint16_t LShd: 1;                               /*!< Bit4: digital button LShd(LeftShoulder)/LB(Left bumper) */
		uint16_t RShd: 1;                               /*!< Bit5: digital button RShd(EightShoulder)/RB(Right bumper) */
		uint16_t View: 1;                               /*!< Bit6: digital button View/BACK/APP */
		uint16_t Menu: 1;                               /*!< Bit7: digital button Menu/START */

		uint16_t LThStk: 1;                             /*!< Bit8: digital button Left Thumbstick Pressed */
		uint16_t RThStk: 1;                             /*!< Bit9: digital button Right Thumbstick Pressed */

		//Bit10 --- Bit15: Padding bits.
		uint16_t Pad1: 1;
		uint16_t Pad2: 1;
		uint16_t Pad3: 1;
		uint16_t Pad4: 1;
		uint16_t Pad5: 1;
		uint16_t Pad6: 1;
	} PC;

	struct {

		uint16_t A: 1;                                  /*!< Bit0: digital button A */
		uint16_t B: 1;                                  /*!< Bit1: digital button B */

		uint16_t Padbit1: 1;                            /*!< Bit2: padbit */

		uint16_t X: 1;                                  /*!< Bit3: digital button X */
		uint16_t Y: 1;                                  /*!< Bit4: digital button Y */

		uint16_t Padbit2: 1;                            /*!< Bit5: padbit */

		uint16_t LShd: 1;                               /*!< Bit6: digital button LShd(LeftShoulder)/LB(Left bumper) */
		uint16_t RShd: 1;                               /*!< Bit7: digital button RShd(EightShoulder)/RB(Right bumper) */
		uint16_t Padbit3: 1;                            /*!< Bit8: padbit */
		uint16_t Padbit4: 1;                            /*!< Bit9: padbit */

		uint16_t View: 1;                               /*!< Bit10: padbit */

		uint16_t Menu: 1;                               /*!< Bit11: digital button Menu/START */
		uint16_t Nexus: 1;                              /*!< Bit12: padbit */

		uint16_t LThStk: 1;                             /*!< Bit13: digital button Left Thumbstick Pressed */
		uint16_t RThStk: 1;                             /*!< Bit14: digital button Right Thumbstick Pressed */

		uint16_t Pad7: 1;                               /*!< Bit15: padbit */
	} ANDROID;

	uint16_t Value16;
} rtk_hid_digital_buttons_t;

/**
 * @struct    rtk_hid_gamepad_data_report_t
 * @brief     hid gamepad report struct.
 */
typedef struct {
	int16_t LeftThumbstickX;                            /*!< -32K to +32K Left Thumbstick in X direction */
	int16_t LeftThumbstickY;                            /*!< -32K to +32K Left Thumbstick in Y direction */
	int16_t RightThumbstickX;                           /*!< -32K to +32K Right Thumbstick in X direction */
	int16_t RightThumbstickY;                           /*!< -32K to +32K Right Thumbstick in Y direction */
	uint16_t LeftTrigger;                               /*!< 0 - 1023 Left Trigger */
	uint16_t RightTrigger;                              /*!< 0 - 1023 Right Trigger */
	uint8_t DPadButtons;                                /*!< 8 DPAD in center position. 0 - 7 Conressponding to 0-315 with steps 45 degrees [Angular Position] */
	rtk_hid_digital_buttons_t DigitalButtons;           /*!< DPAD, Y X B A, LShd(LeftShoulder)/LB(Left bumper), RShd/RB, */
	uint8_t ShareButton;                                /*!< Merlin Share button */
} rtk_hid_gamepad_data_report_t;

/**
 * @struct    rtk_bt_hid_gamepad_report_t
 * @brief     bt hid gamepad report struct.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint8_t ReportID;                                   /*!< report id */
	rtk_hid_gamepad_data_report_t gpd_data;             /*!< gamepad data */
} rtk_bt_hid_gamepad_report_t;

extern const uint8_t hid_gamepad_sdp_record[];
extern uint32_t gamepad_sdp_record_size;
extern const uint8_t hid_descriptor_gamepad_boot_mode[];
extern uint32_t gamepad_hid_descriptor_size;

#ifdef __cplusplus
}
#endif

#endif /* _GAMEPAD_DES_H_ */