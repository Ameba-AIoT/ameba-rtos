/**
  ******************************************************************************
  * @file    usbd_def_rom.h
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-11-23
  * @brief   This file provides general defines for USB SPEC CH9
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef __USBD_DEF_ROM_H
#define __USBD_DEF_ROM_H

/* Includes ------------------------------------------------------------------*/

#include "basic_types.h"

/* Exported defines ----------------------------------------------------------*/

#define USB_LEN_DEV_QUALIFIER_DESC                     0x0AU
#define USB_LEN_DEV_DESC                               0x12U
#define USB_LEN_CFG_DESC                               0x09U
#define USB_LEN_IF_DESC                                0x09U
#define USB_LEN_EP_DESC                                0x07U
#define USB_LEN_OTG_DESC                               0x03U
#define USB_LEN_LANGID_STR_DESC                        0x04U
#define USB_LEN_OTHER_SPEED_DESC_SIZ                   0x09U

#define USBD_IDX_LANGID_STR                            0x00U
#define USBD_IDX_MFC_STR                               0x01U
#define USBD_IDX_PRODUCT_STR                           0x02U
#define USBD_IDX_SERIAL_STR                            0x03U
#define USBD_IDX_CONFIG_STR                            0x04U
#define USBD_IDX_INTERFACE_STR                         0x05U

#define USB_REQ_TYPE_STANDARD                          0x00U
#define USB_REQ_TYPE_CLASS                             0x20U
#define USB_REQ_TYPE_VENDOR                            0x40U
#define USB_REQ_TYPE_MASK                              0x60U

#define USB_REQ_RECIPIENT_DEVICE                       0x00U
#define USB_REQ_RECIPIENT_INTERFACE                    0x01U
#define USB_REQ_RECIPIENT_ENDPOINT                     0x02U
#define USB_REQ_RECIPIENT_MASK                         0x03U

#define USB_REQ_GET_STATUS                             0x00U
#define USB_REQ_CLEAR_FEATURE                          0x01U
#define USB_REQ_SET_FEATURE                            0x03U
#define USB_REQ_SET_ADDRESS                            0x05U
#define USB_REQ_GET_DESCRIPTOR                         0x06U
#define USB_REQ_SET_DESCRIPTOR                         0x07U
#define USB_REQ_GET_CONFIGURATION                      0x08U
#define USB_REQ_SET_CONFIGURATION                      0x09U
#define USB_REQ_GET_INTERFACE                          0x0AU
#define USB_REQ_SET_INTERFACE                          0x0BU
#define USB_REQ_SYNCH_FRAME                            0x0CU

#define USB_DESC_TYPE_DEVICE                           0x01U
#define USB_DESC_TYPE_CONFIGURATION                    0x02U
#define USB_DESC_TYPE_STRING                           0x03U
#define USB_DESC_TYPE_INTERFACE                        0x04U
#define USB_DESC_TYPE_ENDPOINT                         0x05U
#define USB_DESC_TYPE_DEVICE_QUALIFIER                 0x06U
#define USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION        0x07U
#define USB_DESC_TYPE_BOS                              0x0FU

#define USB_CONFIG_REMOTE_WAKEUP                       0x02U
#define USB_CONFIG_SELF_POWERED                        0x01U

#define USB_FEATURE_EP_HALT                            0x00U
#define USB_FEATURE_REMOTE_WAKEUP                      0x01U
#define USB_FEATURE_TEST_MODE                          0x02U

#define USB_DEVICE_CAPABITY_TYPE                       0x10U

#define USB_HS_MAX_PACKET_SIZE                         512U
#define USB_FS_MAX_PACKET_SIZE                         64U
#define USB_MAX_EP0_SIZE                               64U

/*  Device Status */
#define USBD_STATE_DEFAULT                             0x01U
#define USBD_STATE_ADDRESSED                           0x02U
#define USBD_STATE_CONFIGURED                          0x03U
#define USBD_STATE_SUSPENDED                           0x04U

/*  EP0 State */
#define USBD_EP0_IDLE                                  0x00U
#define USBD_EP0_SETUP                                 0x01U
#define USBD_EP0_DATA_IN                               0x02U
#define USBD_EP0_DATA_OUT                              0x03U
#define USBD_EP0_STATUS_IN                             0x04U
#define USBD_EP0_STATUS_OUT                            0x05U
#define USBD_EP0_STALL                                 0x06U

#define USBD_EP_TYPE_CTRL                              0x00U
#define USBD_EP_TYPE_ISOC                              0x01U
#define USBD_EP_TYPE_BULK                              0x02U
#define USBD_EP_TYPE_INTR                              0x03U

/* Exported types ------------------------------------------------------------*/

typedef struct {

	u8  bmRequest;
	u8  bRequest;
	u16 wValue;
	u16 wIndex;
	u16 wLength;
} USBD_SetupReqTypedef;

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

#endif /* __USBD_DEF_ROM_H */

