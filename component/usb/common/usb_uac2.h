/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_UAC2_H
#define USB_UAC2_H

/* Includes ------------------------------------------------------------------*/

#include "usb_uac.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_Constants USB Common Constants
 *  @{
 */
/** @defgroup USB_Common_UAC2_Constants USB UAC2 Constants (UAC 2.0)
 *  @brief UAC 2.0 descriptor, request and control definitions shared by the USBD and USBH UAC drivers.
 *  @{
 */

/**
 * Defines UAC 2.0 descriptor lengths.
 */
#define USB_UAC2_LEN_CTRL_IF_HEADER                 0x09U  /**<UAC Audio Control Interface Header Descriptor 2.0. */
#define USB_UAC2_LEN_CLK_SRC_DESC                   0x08U  /**<UAC Audio Control Clock Source Unit Descriptor 2.0. */
#define USB_UAC2_LEN_AC_IN_TTY_DESC                 0x11U  /**<UAC Audio Control Input Terminal Descriptor 2.0. */
#define USB_UAC2_LEN_FUNC_UNIT_DESC                 0x09U  /**<UAC Audio Control Feature Unit Descriptor 2.0. */
#define USB_UAC2_LEN_AC_OUT_TTY_DESC                0x0CU  /**<UAC Audio Control Output Terminal Descriptor 2.0. */
#define USB_UAC2_LEN_AS_IF_ALT_SET_DESC             0x10U  /**<UAC Audio Streaming Interface Descriptor 2.0. */
#define USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC       0x06U  /**<UAC Audio Streaming Format Type Descriptor 2.0. */
#define USB_UAC2_LEN_DATA_EP_DESC                   0x08U  /**<UAC Audio Data Endpoint Descriptor. */

#define USB_UAC2_IF_IDX_AC_HEADSET                  0x00U  /**<UAC Audio Control Interface Index. */
#define USB_UAC2_IF_IDX_AS_HEADSET_HEADPHONES       0x01U  /**<UAC Audio Streaming Headphone Interface Index. */
#define USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE       0x02U  /**<UAC Audio Streaming Microphone Interface Index. */

/* Audio Function Category Codes (UAC2.0 Spec Appendix A.7) */
#define USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_UNDEFINED              0x00U  /**<UAC Audio Function Undefined type. */
#define USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER                0x01U  /**<UAC Audio Function Speaker. */
#define USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_HEADSET                0x04U  /**<UAC Audio Function Headset. */

/* A.9 Audio Class-Specific AC Interface Descriptor Subtype Codes (UAC2-specific) */
#define USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE                   0x0AU  /**<UAC Clock Source Descriptor subtype. */

/* A.17.1 Clock Source Control Selectors */
#define USB_UAC2_CS_CLK_VALID_CONTROL                              0x02U /**< UAC Audio Clock Source Clock Valid Control. */
/* A.17.4 Terminal Control Selectors */
#define USB_UAC2_TE_CONNECTOR_CONTROL                              0x02U /**< UAC Audio Terminal Control Selectors CONNECTOR. */

/** @} End of USB_Common_UAC2_Constants group */
/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_Types USB Common Types
 *  @{
 */
/** @defgroup USB_Common_UAC2_Types USB UAC2 Types (UAC 2.0)
 *  @brief UAC 2.0 descriptor, request and control definitions shared by the USBD and USBH UAC drivers.
 *  @{
 */

/* 5.2.5.4.2 Connector Control */
typedef struct {
	u8  bNrChannels;
	u32 bmChannelConfig;
	u8  iChannelNames;
} __PACKED usb_uac2_ac_connect_ctrl_t;

/* 5.2.3.2 Layout 2 Parameter Block */
/**
 * @brief Structure to define the volume range attribute.
 * @details This structure holds the volume range attribute,
 *          such as max, min and res.
 */
typedef struct {
	u16 wNumSubRanges;
	u16 wMIN;
	u16 wMAX;
	u16 wRES;
} __PACKED usb_uac2_ctrl_range_layout2_struct;

/* 5.2.3.3 Layout 3 Parameter Block */
/**
 * @brief One subrange of a Layout 3 RANGE parameter block.
 * @details A RANGE response is wNumSubRanges followed by wNumSubRanges of these subranges,
 *          e.g. for the Sampling Frequency Control of a Clock Source.
 */
typedef struct {
	u32 dMIN;
	u32 dMAX;
	u32 dRES;
} usb_uac2_freq_t;

/** @} End of USB_Common_UAC2_Types group */
/** @} End of USB_Common_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif /* USB_UAC2_H */
