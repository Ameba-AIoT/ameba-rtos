/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#ifndef _MOUSE_DES_H_
#define _MOUSE_DES_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

/**
 * @struct    rtk_hid_mouse_data_report_t
 * @brief     hid mouse report struct.
 */
typedef struct {
	uint8_t Button;                                     /*!< button 1-3 -> bit 0-2 */
	int8_t LocationX;                                   /*!< -127 to 127 in X direction */
	int8_t LocationY;                                   /*!< -127 to 127 in Y direction */
	int8_t Wheel;                                       /*!< -127 to 127 of wheel */
} rtk_hid_mouse_data_report_t;

/**
 * @struct    rtk_bt_hid_mouse_report_t
 * @brief     bt hid mouse report struct.
 */
typedef struct {
	uint8_t bd_addr[6];                                 /*!< address */
	uint8_t ReportID;                                   /*!< report id */
	rtk_hid_mouse_data_report_t mse_data;               /*!< mouse data */
} rtk_bt_hid_mouse_report_t;

extern const uint8_t hid_mouse_sdp_record[];
extern uint32_t mouse_sdp_record_size;
extern const uint8_t hid_descriptor_mouse_boot_mode[];
extern uint32_t mouse_hid_descriptor_size;

#ifdef __cplusplus
}
#endif

#endif /* _MOUSE_DES_H_ */