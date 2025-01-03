/**
  ******************************************************************************
  * The header file for usbh_cdc_ecm_hal.c
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef USBH_CDC_ECM_HAL_H
#define USBH_CDC_ECM_HAL_H

/* Includes ------------------------------------------------------------------*/


/* Exported defines ----------------------------------------------------------*/

/*  */
typedef struct {
	u8                                  *mac_value;    /* mac value */
	u16                                 *led_array;    /* led array */
	u8                                  led_cnt;       /* led cnt */
} usbh_cdc_ecm_priv_data_t;

/* Exported types ------------------------------------------------------------*/
typedef void (*usb_report_data)(u8 *buf, u32 len);

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
int usbh_cdc_ecm_do_init(usb_report_data cb_handle, usbh_cdc_ecm_priv_data_t *priv);
int usbh_cdc_ecm_do_deinit(void);

const u8 *usbh_cdc_ecm_process_mac_str(void);
int usbh_cdc_ecm_send_data(u8 *buf, u32 len);
int usbh_cdc_ecm_get_connect_status(void);
u16 usbh_cdc_ecm_get_receive_mps(void);

#endif  /* USBH_CDC_ECM_HAL_H */

