/**
  ******************************************************************************
  * @file    usbd_uac.c
  * @author  Realsil WLAN5 Team
  * @brief   This file provides the functionalities of the USB UAC Class
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "usbd_uac.h"
#include "usbd.h"

/* Private defines -----------------------------------------------------------*/
#define USBD_UAC_AUDIO_CTRL_CLASS_SIZE                  (83U)

#define USBD_UAC_ISOC_IN_ENABLE                         1

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/
#define HIBYTE(w)           ((u8)((((u16)(w)) >> 8) & 0xff))

/* Private function prototypes -----------------------------------------------*/

static int usbd_uac_set_config(usb_dev_t *dev, u8 config);
static int usbd_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u8 *usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static void usbd_uac_status_changed(usb_dev_t *dev, u8 status);
static int usbd_uac_receive_isoc_data(void);
/* Private variables ---------------------------------------------------------*/

static const char *TAG = "UAC";

/* USB Standard Device Descriptor */
static u8 usbd_uac_dev_desc[USB_LEN_DEV_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_DESC,                               /* bLength */
	USB_DESC_TYPE_DEVICE,                           /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,                               /* bMaxPacketSize */
	USB_LOW_BYTE(USBD_UAC_VID),                  /* idUAC */
	USB_HIGH_BYTE(USBD_UAC_VID),
	USB_LOW_BYTE(USBD_UAC_PID),                  /* idProduct */
	USB_HIGH_BYTE(USBD_UAC_PID),
	0x00,                                           /* bcdDevice */
	0x02,
	USBD_IDX_MFC_STR,                               /* iManufacturer */
	USBD_IDX_PRODUCT_STR,                           /* iProduct */
	USBD_IDX_SERIAL_STR,                            /* iSerialNumber */
	0x01                                            /* bNumConfigurations */
};  /* usbd_uac_dev_desc */

/* USB Standard String Descriptor 0 */
static u8 usbd_uac_lang_id_desc[USB_LEN_LANGID_STR_DESC] USB_DMA_ALIGNED = {
	USB_LEN_LANGID_STR_DESC,                        /* bLength */
	USB_DESC_TYPE_STRING,                           /* bDescriptorType */
	USB_LOW_BYTE(USBD_UAC_LANGID_STRING),            /* wLANGID */
	USB_HIGH_BYTE(USBD_UAC_LANGID_STRING),
};  /* usbd_uac_lang_id_desc */

/* USB Standard Device Qualifier Descriptor */
static u8 usbd_uac_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] USB_DMA_ALIGNED = {
	USB_LEN_DEV_QUALIFIER_DESC,                     /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,                 /* bDescriptorType */
	0x00,                                           /* bcdUSB */
	0x02,
	0x00,                                           /* bDeviceClass */
	0x00,                                           /* bDeviceSubClass */
	0x00,                                           /* bDeviceProtocol */
	0x40,                                           /* bMaxPacketSize */
	0x01,                                           /* bNumConfigurations */
	0x00,                                           /* Reserved */
};  /* usbd_uac_device_qualifier_desc */

/* USB UAC Device Full Speed Configuration Descriptor */
static u8 usbd_uac_fs_config_desc[USBD_UAC_CONFIG_DESC_SIZE] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	USB_LOW_BYTE(USBD_UAC_CONFIG_DESC_SIZE),        /* wTotalLength */
	USB_HIGH_BYTE(USBD_UAC_CONFIG_DESC_SIZE),
	0x03,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_UAC_SELF_POWERED
	0xC0,                                           /* bmAttributes: self powered */
#else
	0x80,                                           /* bmAttributes: bus powered */
#endif
	0x32,                                           /* bMaxPower */

	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                               /* Size of this descriptor (bytes) */
	USBD_UAC_IA_DESCRIPTOR,                         /* Interface Association Descriptor type */
	USBD_IFACE_IDX_AUDIOCONTROL_HEADSET,            /* First Index: Audio Control Interface Index - Headset (0) */
	0x03,                                           /* 1 Audio Control Interface, 2 Audio Streaming Interfaces (Mic + Headphones) */
	USBD_UAC_IF_CLASS_AUDIO,                        /* Audio Device Class  */
	0x00,                                           /* No subclass */
	USBD_UAC_VERSION_02_00,                         /* Audio Protocol IP version 2.00 */
	0x00,                                           /* Function string descriptor index (0) */

	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* Interface Descriptor type */
	USBD_IFACE_IDX_AUDIOCONTROL_HEADSET,            /* Audio Control Interface Index - Headset (0) */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* No associated endpoints with this interface (uses endpoint 0) */
	USBD_UAC_CLASS_CODE,                            /* Audio Device Class */
	USBD_UAC_SUBCLASS_AUDIOCONTROL,                 /* Audio Control Interface */
	USBD_UAC_VERSION_02_00,                         /* Audio Protocol IP version 2.00 */
	0x00,                                           /* Interface string descriptor index (0) */

	/* Audio microphone */
	/* Microphone Audio Control Interface Header Descriptor 2.0  */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                        /* Class-specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_HEADER,      /* Header Descriptor subtype */
	USB_LOW_BYTE(0x0200),                           /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USB_AUDIO_FUNCTION_CATEGORY_CODE_DESKTOP_HEADSET, /* Headset Category */
	USB_LOW_BYTE(USBD_UAC_AUDIO_CTRL_CLASS_SIZE),     /* Size of the entire Class Specific Audio Control Descriptor */
	USB_HIGH_BYTE(USBD_UAC_AUDIO_CTRL_CLASS_SIZE),
	0x00,                                             /* Control not present (UAC2.0 Section 4.7.2) */

	/* Microphone Audio Control Clock Source Unit Descriptor 2.0 */
	USBD_UAC_LEN_CLK_SRC_DESC,                            /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                              /* Class-specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_CLOCK_SOURCE,      /* Clock Source Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_MICROPHONE,  /* bClockID */
	0x01,                                                 /* 0x00000001 - Internal Fixed Clock */
	0x01,                                                 /* Clock Frequency Control - read only */
	0x00,                                                 /* Not associated with any terminal */
	0x00,                                                 /* Clock source string descriptor index (0) */

	/* Microphone Audio Control Input Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_IN_TTY_DESC,                          /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                              /* Class Specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_INPUT_TERMINAL,    /* Input Terminal Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE,  /* Headset Microphone Input Terminal Entity ID (5) */
	USB_LOW_BYTE(0x0201),                                         /* Microphone terminal type */
	USB_HIGH_BYTE(0x0201),
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, /* Associated with the Headset headphones because they are physically related
                                                                     as a bidirectional terminal pair for the headset device. */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_MICROPHONE,          /* Headset Microphone Clock Entity ID (4) */
	0x01,                                                         /* (1 channel) */
	0x01, 0x00, 0x00, 0x00,                                       /* (Front left) */
	0x00,                                                         /* Channel Name string descriptor index (0) */
	USB_LOW_BYTE(0x0004),                                         /* Connector Control    - read only */
	USB_HIGH_BYTE(0x0004),
	0x00,                                                         /* Input terminal string descriptor index (0) */

	/* Microphone Audio Control Output Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_OUT_TTY_DESC,                                 /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                      /* Class specific Interfae Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_OUTPUT_TERMINAL,           /* Output Terminal Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* Headset Microphone Output Terminal Entity ID (6) */
	USB_LOW_BYTE(0x0101),                                         /* USB Streaming Terminal Type */
	USB_HIGH_BYTE(0x0101),
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES,  /* Associated with the Input USB Streaming Terminal because it is a bidirectional data pair */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE,  /* Headset Microphone Input Terminal Entity ID (5) */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_MICROPHONE,          /* Headset Microphone Clock Entity ID (4) */
	USB_LOW_BYTE(0x0000),                                         /* Control not present (UAC2.0 Section 4.7.2) */
	USB_HIGH_BYTE(0x0000),
	0x00,                                                         /* Output terminal string descriptor index (0) */

	/* Audio headphone */
	/* Headphone Audio Control Clock Source Unit Descriptor 2.0 */
	USBD_UAC_LEN_CLK_SRC_DESC,                                   /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                                     /* Class-specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_CLOCK_SOURCE,             /* Clock Source Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* Headset Headphones Clock Entity ID (7) */
	0x01,                                                        /* 0x00000001 - Internal Fixed Clock */
	0x01,                                                        /* Clock Frequency Control - read only */
	0x00,                                                        /* Not associated with any terminal */
	0x00,                                                        /* Clock source string descriptor index (0) */

	/* Headphone Audio Control Input Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_IN_TTY_DESC,                                 /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                                     /* Class Specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_INPUT_TERMINAL,           /* Input Terminal Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Headset Headphones Input Terminal Entity ID (8) */
	USB_LOW_BYTE(0x0101),                                        /* USB Streaming terminal type */
	USB_HIGH_BYTE(0x0101),
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE,/* Associated with the Output USB Streaming Terminal because it is a bidirectional data pair. */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* Headset Microphone Clock Entity ID (4) */
	0x02,                                                        /* (2 channels) */
	0x03, 0x00, 0x00, 0x00,                                      /* (Front left and Front right) */
	0x00,                                                        /* Channel Name string descriptor index (0) */
	USB_LOW_BYTE(0x0004),                                        /* Connector Control    - read only */
	USB_HIGH_BYTE(0x0004),
	0x00,                                                        /* Input terminal string descriptor index (0) */

	/* Headphone Audio Control Output Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_OUT_TTY_DESC,                                /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                     /* Class specific Interfae Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_OUTPUT_TERMINAL,          /* Output Terminal Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES,/* Headset Headphones Output Terminal Entity ID (9) */
	USB_LOW_BYTE(0x0302),                                        /* Headphones terminal type */
	USB_HIGH_BYTE(0x0302),
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* Associated with the Headset microphone because they are physically related
                                                                    as a bidirectional terminal pair for the headset device. */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Headset Headphones Input Terminal Entity ID (8) */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* Headset Headphones Clock Entity ID (7) */
	USB_LOW_BYTE(0x0000),                                        /* Control not present (UAC2.0 Section 4.7.2) */
	USB_HIGH_BYTE(0x0000),
	0x00,                                                        /* Output terminal string descriptor index (0) */

	/* Interface1 Descriptor alt setting 0 streaming */
	/* Microphone */
	USB_LEN_IF_DESC,                                            /* bLength */
	USB_DESC_TYPE_INTERFACE,                                    /* bDescriptorType */
	USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_MICROPHONE,           /* Headset Microphone Audio Streaming Interface Index (1) */
	0x00,                                                       /* UAC2.0 Requests Alt Setting 0 with no endpoints (zero-bandwidth) */
	0x00,                                                       /* No endpoints for zero-bandwidth interface */
	USBD_UAC_CLASS_CODE,                                        /* Audio Device Class */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                             /* Audio Control Interface */
	USBD_UAC_VERSION_02_00,                                     /* Audio Protocol IP version 2.00 */
	0x00,                                                       /* Interface String Descriptor Index (0) */

	/* Microphone Audio Streaming Interface Alt Setting 0 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                    /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_AS_GENERAL,              /* AS_GENERAL Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* Headset Microphone Output Terminal Entity ID (6) */
	0x00,                                                       /* No controls present */
	0x01,                                                       /* FORMAT_TYPE_I */
	0x01, 0x00, 0x00, 0x00,                                     /* PCM */
	0x01,                                                       /* (1 channels) */
	0x01, 0x00, 0x00, 0x00,                                     /* (Front left) */
	0x00,                                                       /* Channel name string descriptor index (0) */

	/* Microphone Audio Streaming Format Type Alt Setting 0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,              /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                            /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_FORMAT_TYPE,     /* FORMAT_TYPE */
	0x01,                                               /* FORMAT_TYPE_I */
	0x02,                                               /* 2 bytes, which matches bBitResolution of 16 bits per sample */
	0x10,                                               /* 16 bits per sample */

	/* Microphone Audio Streaming Interface Descriptor alt setting 1 */
	USB_LEN_IF_DESC,                                   /* Size of this descriptor (bytes) */
	USB_DESC_TYPE_INTERFACE,                           /* Interface Descriptor type */
	USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_MICROPHONE,  /* Headset Microphone Audio Streaming Interface Index (1) */
	0x01,                                              /* Alt Setting 1 is when this interface is "enabled" */
	0x01,                                              /* One isoch endpoint to deliver audio to host */
	USBD_UAC_CLASS_CODE,                               /* (Audio Device Class) */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                  /* (Audio Streaming Interface) */
	USBD_UAC_VERSION_02_00,                            /* (Audio Protocol IP version 2.00) */
	0x00,                                              /* Interface string descriptor index (0) */

	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                    /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_AS_GENERAL,              /* AS_GENERAL Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE,  /* Headset Microphone Output Terminal Entity ID (6) */
	0x00,                                                       /* No controls present */
	0x01,                                                       /* FORMAT_TYPE_I */
	0x01, 0x00, 0x00, 0x00,                                     /* PCM */
	0x01,                                                       /* (1 channels) */
	0x01, 0x00, 0x00, 0x00,                                     /* (Front left) */
	0x00,                                                       /* Channel name string descriptor index (0) */

	/* Microphone Class Specific Audio Streaming Format Type Alt Setting 1 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,              /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                            /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_FORMAT_TYPE,     /* FORMAT_TYPE */
	0x01,                                               /* FORMAT_TYPE_I */
	0x02,                                               /* 2 bytes, which matches bBitResolution of 16 bits per sample */
	0x10,                                               /* 16 bits per sample */

	/* Microphone Audio Streaming ISO IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_UAC_ISOC_IN_EP,                            /* bEndpointAddress */
	0x0D,                                           /* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_UAC_FS_ISOC_MPS),             /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_FS_ISOC_MPS),
	USBD_UAC_FS_ISOC_IN_INTERVAL,                   /* bInterval */

	/* Microphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_ENDPOINT_DESCRIPTOR,                           /* Class-specific Endpoint Descriptor type */
	USB_CS_AS_ENDPOINT_DESCRIPTOR_SUBTYPE_EP_GENERAL,     /* EP_GENERAL Descriptor subtype */
	0x00,                                                 /* Can handle short packets (D7 = 0) */
	0x00,                                                 /* No controls */
	0x00,                                                 /* Undefined lock delay units */
	USB_LOW_BYTE(0x0000),                                 /* Time it takes this endpoint to lock its */
	USB_HIGH_BYTE(0x0000),                                /* internal clock recovery circuitry */

	/* Interface1 Descriptor alt setting 1 streaming */
	/* Headphone */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_HEADPHONES,  /* Headset Headphones Audio Streaming Interface Index (2) */
	0x00,                                           /* UAC2.0 Requests Alt Setting 0 with no endpoints (zero-bandwidth) */
	0x00,                                           /* No endpoints for zero-bandwidth interface */
	USBD_UAC_CLASS_CODE,                            /* (Audio Device Class) */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,               /* (Audio Streaming Interface) */
	USBD_UAC_VERSION_02_00,                         /* (Audio Protocol IP version 2.00) */
	0x00,                                           /* Interface String Descriptor Index (0) */

	/* Headphone Audio Streaming Interface Descriptor 2.0 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                    /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_AS_GENERAL,              /* AS_GENERAL Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES,/* Headset Headphones Input Terminal Entity ID (8) */
	0x00,                                                       /* No controls present */
	0x01,                                                       /* FORMAT_TYPE_I */
	0x01, 0x00, 0x00, 0x00,                                     /* PCM */
	0x02,                                                       /* (2 channels) */
	0x03, 0x00, 0x00, 0x00,                                     /* (Front left and front right) */
	0x00,                                                       /* Channel name string descriptor index (0) */

	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,              /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                            /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_FORMAT_TYPE,     /* FORMAT_TYPE */
	0x01,                                               /* FORMAT_TYPE_I */
	0x02,                                               /* 2 bytes, which matches bBitResolution of 16 bits per sample */
	0x10,                                               /* 16 bits per sample */

	/* Headphone Interface1 Descriptor alt setting 1 streaming */
	USB_LEN_IF_DESC,                                    /* Size of this descriptor (bytes) */
	USB_DESC_TYPE_INTERFACE,                            /* Interface Descriptor type */
	USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_HEADPHONES,   /* Headset Headphones Audio Streaming Interface Index (5) */
	0x01,                                               /* Alt Setting 1 is when this interface is "enabled" */
	0x01,                                               /* One isoch endpoint to deliver audio to host */
	USBD_UAC_CLASS_CODE,                                /* (Audio Device Class) */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                   /* (Audio Streaming Interface) */
	USBD_UAC_VERSION_02_00,                             /* (Audio Protocol IP version 2.00) */
	0x00,                                               /* Interface string descriptor index (0) */

	/* Headphone Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                    /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_AS_GENERAL,              /* AS_GENERAL Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES,/* Headset Headphones Input Terminal Entity ID (8) */
	0x00,                                                       /* No controls present */
	0x01,                                                       /* FORMAT_TYPE_I */
	0x01, 0x00, 0x00, 0x00,                                     /* PCM */
	0x02,                                                       /* (2 channels) */
	0x03, 0x00, 0x00, 0x00,                                     /* (Front left and front right) */
	0x00,                                                       /* Channel name string descriptor index (0) */

	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,              /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                            /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_FORMAT_TYPE,     /* FORMAT_TYPE */
	0x01,                                               /* FORMAT_TYPE_I */
	0x02,                                               /* 2 bytes, which matches bBitResolution of 16 bits per sample */
	0x10,                                               /* 16 bits per sample */

	/* Headphone ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                    /* bLength */
	USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType */
	USBD_UAC_ISOC_OUT_EP,                               /* bEndpointAddress */
	0x0D,                                               /* Transfer: Isochronous / Synch: Synchronous / Usage: Data */
	USB_LOW_BYTE(USBD_UAC_FS_ISOC_MPS),                 /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_FS_ISOC_MPS),
	USBD_UAC_FS_ISOC_OUT_INTERVAL,                      /* bInterval */

	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                          /* Size of this descriptor (bytes) */
	USB_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USB_CS_AS_ENDPOINT_DESCRIPTOR_SUBTYPE_EP_GENERAL,   /* EP_GENERAL Descriptor subtype */
	0x00,                                               /* Can handle short packets (D7 = 0) */
	0x00,                                               /* No controls */
	0x00,                                               /* Undefined lock delay units */
	USB_LOW_BYTE(0x0000),                               /* Time it takes this endpoint to lock its */
	USB_HIGH_BYTE(0x0000),                              /* internal clock recovery circuitry */
};  /* usbd_uac_fs_config_desc */

/* USB UAC Device High Speed Configuration Descriptor */
static u8 usbd_uac_hs_config_desc[USBD_UAC_CONFIG_DESC_SIZE] USB_DMA_ALIGNED = {
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                               /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                    /* bDescriptorType */
	USB_LOW_BYTE(USBD_UAC_CONFIG_DESC_SIZE),        /* wTotalLength */
	USB_HIGH_BYTE(USBD_UAC_CONFIG_DESC_SIZE),
	0x03,                                           /* bNumInterfaces */
	0x01,                                           /* bConfigurationValue */
	0x00,                                           /* iConfiguration */
#if USBD_UAC_SELF_POWERED
	0xC0,                                           /* bmAttributes: self powered */
#else
	0x80,                                           /* bmAttributes: bus powered */
#endif
	0x32,                                           /* bMaxPower */

	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                               /* Size of this descriptor (bytes) */
	USBD_UAC_IA_DESCRIPTOR,                         /* Interface Association Descriptor type */
	USBD_IFACE_IDX_AUDIOCONTROL_HEADSET,            /* First Index: Audio Control Interface Index - Headset (0) */
	0x03,                                           /* 1 Audio Control Interface, 2 Audio Streaming Interfaces (Mic + Headphones) */
	USBD_UAC_IF_CLASS_AUDIO,                        /* Audio Device Class  */
	0x00,                                           /* No subclass */
	USBD_UAC_VERSION_02_00,                         /* Audio Protocol IP version 2.00 */
	0x00,                                           /* Function string descriptor index (0) */

	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* Interface Descriptor type */
	USBD_IFACE_IDX_AUDIOCONTROL_HEADSET,            /* Audio Control Interface Index - Headset (0) */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* No associated endpoints with this interface (uses endpoint 0) */
	USBD_UAC_CLASS_CODE,                            /* Audio Device Class */
	USBD_UAC_SUBCLASS_AUDIOCONTROL,                 /* Audio Control Interface */
	USBD_UAC_VERSION_02_00,                         /* Audio Protocol IP version 2.00 */
	0x00,                                           /* Interface string descriptor index (0) */

	/* Audio microphone */
	/* Microphone Audio Control Interface Header Descriptor 2.0  */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                        /* Class-specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_HEADER,      /* Header Descriptor subtype */
	USB_LOW_BYTE(0x0200),                           /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USB_AUDIO_FUNCTION_CATEGORY_CODE_DESKTOP_HEADSET, /* Headset Category */
	USB_LOW_BYTE(USBD_UAC_AUDIO_CTRL_CLASS_SIZE),     /* Size of the entire Class Specific Audio Control Descriptor */
	USB_HIGH_BYTE(USBD_UAC_AUDIO_CTRL_CLASS_SIZE),
	0x00,                                             /* Control not present (UAC2.0 Section 4.7.2) */

	/* Microphone Audio Control Clock Source Unit Descriptor 2.0 */
	USBD_UAC_LEN_CLK_SRC_DESC,                            /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                              /* Class-specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_CLOCK_SOURCE,      /* Clock Source Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_MICROPHONE,  /* bClockID */
	0x01,                                                 /* 0x00000001 - Internal Fixed Clock */
	0x01,                                                 /* Clock Frequency Control - read only */
	0x00,                                                 /* Not associated with any terminal */
	0x00,                                                 /* Clock source string descriptor index (0) */

	/* Microphone Audio Control Input Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_IN_TTY_DESC,                          /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                              /* Class Specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_INPUT_TERMINAL,    /* Input Terminal Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE,  /* Headset Microphone Input Terminal Entity ID (5) */
	USB_LOW_BYTE(0x0201),                                         /* Microphone terminal type */
	USB_HIGH_BYTE(0x0201),
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, /* Associated with the Headset headphones because they are physically related
                                                                     as a bidirectional terminal pair for the headset device. */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_MICROPHONE,          /* Headset Microphone Clock Entity ID (4) */
	0x01,                                                         /* (1 channel) */
	0x01, 0x00, 0x00, 0x00,                                       /* (Front left) */
	0x00,                                                         /* Channel Name string descriptor index (0) */
	USB_LOW_BYTE(0x0004),                                         /* Connector Control    - read only */
	USB_HIGH_BYTE(0x0004),
	0x00,                                                         /* Input terminal string descriptor index (0) */

	/* Microphone Audio Control Output Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_OUT_TTY_DESC,                                 /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                      /* Class specific Interfae Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_OUTPUT_TERMINAL,           /* Output Terminal Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* Headset Microphone Output Terminal Entity ID (6) */
	USB_LOW_BYTE(0x0101),                                         /* USB Streaming Terminal Type */
	USB_HIGH_BYTE(0x0101),
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES,  /* Associated with the Input USB Streaming Terminal because it is a bidirectional data pair */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE,  /* Headset Microphone Input Terminal Entity ID (5) */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_MICROPHONE,          /* Headset Microphone Clock Entity ID (4) */
	USB_LOW_BYTE(0x0000),                                         /* Control not present (UAC2.0 Section 4.7.2) */
	USB_HIGH_BYTE(0x0000),
	0x00,                                                         /* Output terminal string descriptor index (0) */

	/* Audio headphone */
	/* Headphone Audio Control Clock Source Unit Descriptor 2.0 */
	USBD_UAC_LEN_CLK_SRC_DESC,                                   /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                                     /* Class-specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_CLOCK_SOURCE,             /* Clock Source Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* Headset Headphones Clock Entity ID (7) */
	0x01,                                                        /* 0x00000001 - Internal Fixed Clock */
	0x01,                                                        /* Clock Frequency Control - read only */
	0x00,                                                        /* Not associated with any terminal */
	0x00,                                                        /* Clock source string descriptor index (0) */

	/* Headphone Audio Control Input Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_IN_TTY_DESC,                                 /* bLength */
	USB_CS_IFACE_DESCRIPTOR,                                     /* Class Specific Interface Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_INPUT_TERMINAL,           /* Input Terminal Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Headset Headphones Input Terminal Entity ID (8) */
	USB_LOW_BYTE(0x0101),                                        /* USB Streaming terminal type */
	USB_HIGH_BYTE(0x0101),
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE,/* Associated with the Output USB Streaming Terminal because it is a bidirectional data pair. */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* Headset Microphone Clock Entity ID (4) */
	0x02,                                                        /* (2 channels) */
	0x03, 0x00, 0x00, 0x00,                                      /* (Front left and Front right) */
	0x00,                                                        /* Channel Name string descriptor index (0) */
	USB_LOW_BYTE(0x0004),                                        /* Connector Control    - read only */
	USB_HIGH_BYTE(0x0004),
	0x00,                                                        /* Input terminal string descriptor index (0) */

	/* Headphone Audio Control Output Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_OUT_TTY_DESC,                                /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                     /* Class specific Interfae Descriptor type */
	USB_CS_AC_IFACE_DESCRIPTOR_SUBTYPE_OUTPUT_TERMINAL,          /* Output Terminal Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES,/* Headset Headphones Output Terminal Entity ID (9) */
	USB_LOW_BYTE(0x0302),                                        /* Headphones terminal type */
	USB_HIGH_BYTE(0x0302),
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* Associated with the Headset microphone because they are physically related
                                                                    as a bidirectional terminal pair for the headset device. */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Headset Headphones Input Terminal Entity ID (8) */
	USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* Headset Headphones Clock Entity ID (7) */
	USB_LOW_BYTE(0x0000),                                        /* Control not present (UAC2.0 Section 4.7.2) */
	USB_HIGH_BYTE(0x0000),
	0x00,                                                        /* Output terminal string descriptor index (0) */

	/* Interface1 Descriptor alt setting 0 streaming */
	/* Microphone */
	USB_LEN_IF_DESC,                                            /* bLength */
	USB_DESC_TYPE_INTERFACE,                                    /* bDescriptorType */
	USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_MICROPHONE,           /* Headset Microphone Audio Streaming Interface Index (1) */
	0x00,                                                       /* UAC2.0 Requests Alt Setting 0 with no endpoints (zero-bandwidth) */
	0x00,                                                       /* No endpoints for zero-bandwidth interface */
	USBD_UAC_CLASS_CODE,                                        /* Audio Device Class */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                             /* Audio Control Interface */
	USBD_UAC_VERSION_02_00,                                     /* Audio Protocol IP version 2.00 */
	0x00,                                                       /* Interface String Descriptor Index (0) */

	/* Microphone Audio Streaming Interface Alt Setting 0 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                    /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_AS_GENERAL,              /* AS_GENERAL Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* Headset Microphone Output Terminal Entity ID (6) */
	0x00,                                                       /* No controls present */
	0x01,                                                       /* FORMAT_TYPE_I */
	0x01, 0x00, 0x00, 0x00,                                     /* PCM */
	0x01,                                                       /* (1 channels) */
	0x01, 0x00, 0x00, 0x00,                                     /* (Front left) */
	0x00,                                                       /* Channel name string descriptor index (0) */

	/* Microphone Audio Streaming Format Type Alt Setting 0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,              /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                            /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_FORMAT_TYPE,     /* FORMAT_TYPE */
	0x01,                                               /* FORMAT_TYPE_I */
	0x02,                                               /* 2 bytes, which matches bBitResolution of 16 bits per sample */
	0x10,                                               /* 16 bits per sample */

	/* Microphone Audio Streaming Interface Descriptor alt setting 1 */
	USB_LEN_IF_DESC,                                   /* Size of this descriptor (bytes) */
	USB_DESC_TYPE_INTERFACE,                           /* Interface Descriptor type */
	USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_MICROPHONE,  /* Headset Microphone Audio Streaming Interface Index (1) */
	0x01,                                              /* Alt Setting 1 is when this interface is "enabled" */
	0x01,                                              /* One isoch endpoint to deliver audio to host */
	USBD_UAC_CLASS_CODE,                               /* (Audio Device Class) */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                  /* (Audio Streaming Interface) */
	USBD_UAC_VERSION_02_00,                            /* (Audio Protocol IP version 2.00) */
	0x00,                                              /* Interface string descriptor index (0) */

	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                    /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_AS_GENERAL,              /* AS_GENERAL Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE,  /* Headset Microphone Output Terminal Entity ID (6) */
	0x00,                                                       /* No controls present */
	0x01,                                                       /* FORMAT_TYPE_I */
	0x01, 0x00, 0x00, 0x00,                                     /* PCM */
	0x01,                                                       /* (1 channels) */
	0x01, 0x00, 0x00, 0x00,                                     /* (Front left) */
	0x00,                                                       /* Channel name string descriptor index (0) */

	/* Microphone Class Specific Audio Streaming Format Type Alt Setting 1 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,              /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                            /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_FORMAT_TYPE,     /* FORMAT_TYPE */
	0x01,                                               /* FORMAT_TYPE_I */
	0x02,                                               /* 2 bytes, which matches bBitResolution of 16 bits per sample */
	0x10,                                               /* 16 bits per sample */

	/* Microphone Audio Streaming ISO IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                /* bLength */
	USB_DESC_TYPE_ENDPOINT,                         /* bDescriptorType */
	USBD_UAC_ISOC_IN_EP,                            /* bEndpointAddress */
	0x0D,                                           /* bmAttributes: ISOC */
	USB_LOW_BYTE(USBD_UAC_HS_ISOC_MPS),             /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_HS_ISOC_MPS),
	USBD_UAC_HS_ISOC_IN_INTERVAL,                   /* bInterval */

	/* Microphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_ENDPOINT_DESCRIPTOR,                           /* Class-specific Endpoint Descriptor type */
	USB_CS_AS_ENDPOINT_DESCRIPTOR_SUBTYPE_EP_GENERAL,     /* EP_GENERAL Descriptor subtype */
	0x00,                                                 /* Can handle short packets (D7 = 0) */
	0x00,                                                 /* No controls */
	0x00,                                                 /* Undefined lock delay units */
	USB_LOW_BYTE(0x0000),                                 /* Time it takes this endpoint to lock its */
	USB_HIGH_BYTE(0x0000),                                /* internal clock recovery circuitry */

	/* Interface1 Descriptor alt setting 1 streaming */
	/* Headphone */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* bDescriptorType */
	USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_HEADPHONES,  /* Headset Headphones Audio Streaming Interface Index (2) */
	0x00,                                           /* UAC2.0 Requests Alt Setting 0 with no endpoints (zero-bandwidth) */
	0x00,                                           /* No endpoints for zero-bandwidth interface */
	USBD_UAC_CLASS_CODE,                            /* (Audio Device Class) */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,               /* (Audio Streaming Interface) */
	USBD_UAC_VERSION_02_00,                         /* (Audio Protocol IP version 2.00) */
	0x00,                                           /* Interface String Descriptor Index (0) */

	/* Headphone Audio Streaming Interface Descriptor 2.0 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                    /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_AS_GENERAL,              /* AS_GENERAL Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES,/* Headset Headphones Input Terminal Entity ID (8) */
	0x00,                                                       /* No controls present */
	0x01,                                                       /* FORMAT_TYPE_I */
	0x01, 0x00, 0x00, 0x00,                                     /* PCM */
	0x02,                                                       /* (2 channels) */
	0x03, 0x00, 0x00, 0x00,                                     /* (Front left and front right) */
	0x00,                                                       /* Channel name string descriptor index (0) */

	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,              /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                            /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_FORMAT_TYPE,     /* FORMAT_TYPE */
	0x01,                                               /* FORMAT_TYPE_I */
	0x02,                                               /* 2 bytes, which matches bBitResolution of 16 bits per sample */
	0x10,                                               /* 16 bits per sample */

	/* Headphone Interface1 Descriptor alt setting 1 streaming */
	USB_LEN_IF_DESC,                                    /* Size of this descriptor (bytes) */
	USB_DESC_TYPE_INTERFACE,                            /* Interface Descriptor type */
	USBD_IFACE_IDX_AUDIOSTREAMING_HEADSET_HEADPHONES,   /* Headset Headphones Audio Streaming Interface Index (5) */
	0x01,                                               /* Alt Setting 1 is when this interface is "enabled" */
	0x01,                                               /* One isoch endpoint to deliver audio to host */
	USBD_UAC_CLASS_CODE,                                /* (Audio Device Class) */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                   /* (Audio Streaming Interface) */
	USBD_UAC_VERSION_02_00,                             /* (Audio Protocol IP version 2.00) */
	0x00,                                               /* Interface string descriptor index (0) */

	/* Headphone Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                            /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                                    /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_AS_GENERAL,              /* AS_GENERAL Descriptor subtype */
	USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES,/* Headset Headphones Input Terminal Entity ID (8) */
	0x00,                                                       /* No controls present */
	0x01,                                                       /* FORMAT_TYPE_I */
	0x01, 0x00, 0x00, 0x00,                                     /* PCM */
	0x02,                                                       /* (2 channels) */
	0x03, 0x00, 0x00, 0x00,                                     /* (Front left and front right) */
	0x00,                                                       /* Channel name string descriptor index (0) */

	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,              /* Size of this descriptor (bytes) */
	USB_CS_IFACE_DESCRIPTOR,                            /* Class-specific Interface Descriptor type */
	USB_CS_AS_IFACE_DESCRIPTOR_SUBTYPE_FORMAT_TYPE,     /* FORMAT_TYPE */
	0x01,                                               /* FORMAT_TYPE_I */
	0x02,                                               /* 2 bytes, which matches bBitResolution of 16 bits per sample */
	0x10,                                               /* 16 bits per sample */

	/* Headphone ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                    /* bLength */
	USB_DESC_TYPE_ENDPOINT,                             /* bDescriptorType */
	USBD_UAC_ISOC_OUT_EP,                               /* bEndpointAddress */
	0x0D,                                               /* Transfer: Isochronous / Synch: Synchronous / Usage: Data */
	USB_LOW_BYTE(USBD_UAC_HS_ISOC_MPS),                 /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_HS_ISOC_MPS),
	USBD_UAC_HS_ISOC_OUT_INTERVAL,                      /* bInterval */

	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                          /* Size of this descriptor (bytes) */
	USB_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */
	USB_CS_AS_ENDPOINT_DESCRIPTOR_SUBTYPE_EP_GENERAL,   /* EP_GENERAL Descriptor subtype */
	0x00,                                               /* Can handle short packets (D7 = 0) */
	0x00,                                               /* No controls */
	0x00,                                               /* Undefined lock delay units */
	USB_LOW_BYTE(0x0000),                               /* Time it takes this endpoint to lock its */
	USB_HIGH_BYTE(0x0000),                              /* internal clock recovery circuitry */
};  /* usbd_uac_hs_config_desc */

/* UAC Class Driver */
static usbd_class_driver_t usbd_uac_driver = {
	.get_descriptor = usbd_uac_get_descriptor,
	.set_config = usbd_uac_set_config,
	.clear_config = usbd_uac_clear_config,
	.setup = usbd_uac_setup,
	.ep_data_in = usbd_uac_handle_ep_data_in,
	.ep_data_out = usbd_uac_handle_ep_data_out,
	.status_changed = usbd_uac_status_changed,
};

/* UAC Device */
static usbd_uac_dev_t usbd_uac_dev;

/* Private functions ---------------------------------------------------------*/
static void usbd_uac_cur_freq_req(usb_dev_t *dev, u32 frequencyHz)
{
	u32 dCur = frequencyHz;

	memcpy(dev->ctrl_buf, &dCur, sizeof(dCur));
	usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(dCur));
}

static void usbd_uac_freq_ctrl_range_req(usb_dev_t *dev, u32 minFrequencyHz, u32 maxFrequencyHz)
{
	usbd_uac_freq_ctrl_range_t response;
	response.wNumSubRanges = 1;
	response.SubRanges[0].bMIN = minFrequencyHz;
	response.SubRanges[0].bMAX = maxFrequencyHz;
	response.SubRanges[0].bRES = 1;

	memcpy(dev->ctrl_buf, &response, sizeof(response));
	usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(response));
}

static void usbd_uac_connect_ctrl_req(usb_dev_t *dev, u8 numChannels, u32 channelConfigMask)
{
	usbd_uac_ac_connect_ctrl_t response = {0, 0, 0};
	response.bNrChannels = numChannels;
	response.bmChannelConfig = channelConfigMask; //endian issue FIXME

	memcpy(dev->ctrl_buf, &response, sizeof(response));
	usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(response));
}

static int usbd_uac_receive_isoc_data(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	usbd_ep_receive(cdev->dev, USBD_UAC_ISOC_OUT_EP, cdev->isoc_out_buf, USBD_UAC_ISOC_OUT_BUF_SIZE);

	return HAL_OK;
}


/**
  * @brief  Set UAC class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_uac_set_config(usb_dev_t *dev, u8 config)
{
	u16 ep_mps;
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	UNUSED(config);

	cdev->dev = dev;
	cdev->alt_setting = 0U;

	/* Init ISO IN EP */
#if USBD_UAC_ISOC_IN_ENABLE
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_UAC_HS_ISOC_MPS : USBD_UAC_FS_ISOC_MPS;
	usbd_ep_init(dev, USBD_UAC_ISOC_IN_EP, USB_CH_EP_TYPE_ISOC, ep_mps);
#endif

	/* Init ISO OUT EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_UAC_HS_ISOC_MPS : USBD_UAC_FS_ISOC_MPS;
	usbd_ep_init(dev, USBD_UAC_ISOC_OUT_EP, USB_CH_EP_TYPE_ISOC, ep_mps);
	usbd_ep_receive(dev, USBD_UAC_ISOC_OUT_EP, cdev->isoc_out_buf, USBD_UAC_ISOC_OUT_BUF_SIZE);

	cdev->is_ready = 1U;

	if (cdev->cb->set_config != NULL) {
		cdev->cb->set_config();
	}

	return ret;
}

/**
  * @brief  Clear UAC configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_uac_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	UNUSED(config);

	cdev->is_ready = 0U;

	/* DeInit ISOC IN EP */
#if USBD_UAC_ISOC_IN_ENABLE
	usbd_ep_deinit(dev, USBD_UAC_ISOC_IN_EP);
#endif

	/* DeInit ISOC OUT EP */
	usbd_ep_deinit(dev, USBD_UAC_ISOC_OUT_EP);

	return ret;
}

/**
  * @brief  Handle UAC specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	u8 entityId, controlSelector;
	int ret = HAL_OK;

	//RTK_LOGS(TAG, "[UAC] SETUP: bmRequestType=0x%02x bRequest=0x%02x wValue=%x wIndex=%x wLength=0x%04x\n",
	//	req->bmRequestType, req->bRequest, req->wValue, req->wIndex, req->wLength);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->alt_setting = (u8)(req->wValue & 0xFF);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = cdev->alt_setting;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 1U);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = 0U;
				cdev->ctrl_buf[1] = 0U;
				usbd_ep0_transmit(dev, cdev->ctrl_buf, 2U);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		default:
			ret = HAL_ERR_HW;
			break;
		}
		break;
	case USB_REQ_TYPE_CLASS :
		entityId = HIBYTE(req->wIndex);
		controlSelector = HIBYTE(req->wValue);

		switch (entityId) {
		case USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_MICROPHONE: {
			if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
				if (req->bRequest == USB_UAC_CLASS_REQ_CODE_CUR) {
					// USBD UAC mic in uses a 24 KHz sampling frequency.
					usbd_uac_cur_freq_req(dev, 24000);
				} else if (req->bRequest == USB_UAC_CLASS_REQ_CODE_RANGE) {
					// USBD UAC mic in uses a 24 KHz sampling frequency.
					usbd_uac_freq_ctrl_range_req(dev, 24000, 24000);
				} else {
					RTK_LOGS(TAG, "[UAC] SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
					ret = HAL_ERR_HW;
				}
			} else {
				RTK_LOGS(TAG, "[UAC] SETUP: wValue err %d-%d\n", entityId, controlSelector);
				ret = HAL_ERR_HW;
			}

			break;
		}

		case USBD_AUDIOCONTROL_ENTITYID_CLOCK_HEADSET_HEADPHONES: {
			if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
				if (req->bRequest == USB_UAC_CLASS_REQ_CODE_CUR) {
					usbd_uac_cur_freq_req(dev, 48000);
				} else if (req->bRequest == USB_UAC_CLASS_REQ_CODE_RANGE) {
					usbd_uac_freq_ctrl_range_req(dev, 48000, 48000);
				} else {
					RTK_LOGS(TAG, "[UAC] SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
					ret = HAL_ERR_HW;
				}
			} else {
				RTK_LOGS(TAG, "[UAC] SETUP: wValue err %d-%d\n", entityId, controlSelector);
				ret = HAL_ERR_HW;
			}

			break;
		}

		case USBD_AUDIOCONTROL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE: {
			if (controlSelector == USB_UAC_TE_CONNECTOR_CONTROL) {
				if (req->bRequest == USB_UAC_CLASS_REQ_CODE_CUR) {
					usbd_uac_connect_ctrl_req(dev, 1, 0x1);
				} else {
					RTK_LOGS(TAG, "[UAC] SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
					ret = HAL_ERR_HW;
				}
			} else {
				RTK_LOGS(TAG, "[UAC] SETUP: wValue err %d-%d\n", entityId, controlSelector);
				ret = HAL_ERR_HW;
			}

			break;
		}
		break;

		default:
			RTK_LOGS(TAG, "[UAC] SETUP: wIndex err %d\n", entityId);
			break;
		}
		break;

	case USB_REQ_TYPE_VENDOR:
		if (req->wLength) {
			if (req->bmRequestType & 0x80U) {
				ret = cdev->cb->setup(req, cdev->ctrl_buf);
				if (ret == HAL_OK) {
					usbd_ep0_transmit(dev, cdev->ctrl_buf, req->wLength);
				}
			} else {
				usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
			}
		} else {
			cdev->cb->setup(req, cdev->ctrl_buf);
		}
		break;

	default:
		RTK_LOGS(TAG, "[UAC] SETUP: bmRequestType err\n");
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_cb_t *cb = cdev->cb;

	UNUSED(dev);
#if USBD_UAC_ISOC_IN_ENABLE
	if (ep_addr == USBD_UAC_ISOC_IN_EP) {
		if (status != HAL_OK) {
			RTK_LOGS(TAG, "[UAC] ISOC TX err: %d\n", status);
		}
		if (cb->isoc_transmitted != NULL) {
			cb->isoc_transmitted(status);
		}
	}
#else
	UNUSED(ep_addr);
	UNUSED(status);
	UNUSED(cdev);
	UNUSED(cb);
#endif
	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	UNUSED(dev);

	if (len == 0) {
		/*RX ZLP*/
		return HAL_OK;
	}

	if (ep_addr == USBD_UAC_ISOC_OUT_EP) {
		cdev->cb->isoc_received(cdev->isoc_out_buf, len);
	}

	return usbd_uac_receive_isoc_data();
}

/**
  * @brief  Get descriptor callback
  * @param  req: setup request
  * @param  speed: device speed
  * @param  len: descriptor length
  * @retval descriptor buffer
  */
static u8 *usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;
	u8 *desc = usbd_uac_dev.ctrl_buf;

	dev->self_powered = USBD_UAC_SELF_POWERED;

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_DEVICE:
		*len = sizeof(usbd_uac_dev_desc);
		buf = usbd_uac_dev_desc;
		usb_os_memcpy((void *)desc, (void *)buf, *len);
		buf = desc;
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_uac_hs_config_desc;
			*len = sizeof(usbd_uac_hs_config_desc);
		} else {
			buf = usbd_uac_fs_config_desc;
			*len = sizeof(usbd_uac_fs_config_desc);
		}
		break;

	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		buf = usbd_uac_device_qualifier_desc;
		*len = sizeof(usbd_uac_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			buf = usbd_uac_fs_config_desc;
			*len = sizeof(usbd_uac_fs_config_desc);
		} else {
			buf = usbd_uac_hs_config_desc;
			*len = sizeof(usbd_uac_hs_config_desc);
		}
		usb_os_memcpy((void *)desc, (void *)buf, *len);
		desc[USB_CFG_DESC_OFFSET_TYPE] = USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION;
		buf = desc;
		break;

	case USB_DESC_TYPE_STRING:
		switch (req->wValue & 0xFF) {
		case USBD_IDX_LANGID_STR:
			buf = usbd_uac_lang_id_desc;
			*len = sizeof(usbd_uac_lang_id_desc);
			break;
		case USBD_IDX_MFC_STR:
			usbd_get_str_desc(USBD_UAC_MFG_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_PRODUCT_STR:
			if (speed == USB_SPEED_HIGH) {
				usbd_get_str_desc(USBD_UAC_PROD_HS_STRING, desc, len);
			} else {
				usbd_get_str_desc(USBD_UAC_PROD_FS_STRING, desc, len);
			}
			buf = desc;
			break;
		case USBD_IDX_SERIAL_STR:
			usbd_get_str_desc(USBD_UAC_SN_STRING, desc, len);
			buf = desc;
			break;
		case USBD_IDX_MS_OS_STR:
			/*Not support*/
			break;
		/* Add customer string here */
		default:
			RTK_LOGS(TAG, "[UAC] Invalid str idx %d\n", req->wValue & 0xFF);
			break;
		}
		break;

	default:
		break;
	}

	return buf;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  config: USB USB attach status
  * @retval void
  */
static void usbd_uac_status_changed(usb_dev_t *dev, u8 status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		cdev->is_ready = 0U;
	}

	if (cdev->cb->status_changed) {
		cdev->cb->status_changed(status);
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Initialize UAC device
  * @retval Status
  */
int usbd_uac_init(usbd_uac_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	cdev->ctrl_buf = (u8 *)usb_os_malloc(USBD_UAC_CTRL_BUF_SIZE);
	if (cdev->ctrl_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_exit;
	}

	cdev->isoc_out_buf = (u8 *)usb_os_malloc(USBD_UAC_ISOC_OUT_BUF_SIZE);
	if (cdev->isoc_out_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_ctrl_buf_exit;
	}

	cdev->isoc_in_buf = (u8 *)usb_os_malloc(USBD_UAC_ISOC_IN_BUF_SIZE);
	if (cdev->isoc_in_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_clean_isoc_out_buf_exit;
	}

	if (cb != NULL) {
		cdev->cb = cb;
		if (cb->init != NULL) {
			ret = cb->init();
			if (ret != HAL_OK) {
				goto init_clean_isoc_in_buf_exit;
			}
		}
	}

	usbd_register_class(&usbd_uac_driver);

	return ret;

init_clean_isoc_in_buf_exit:
	usb_os_mfree(cdev->isoc_in_buf);
	cdev->isoc_in_buf = NULL;

init_clean_isoc_out_buf_exit:
	usb_os_mfree(cdev->isoc_out_buf);
	cdev->isoc_out_buf = NULL;

init_clean_ctrl_buf_exit:
	usb_os_mfree(cdev->ctrl_buf);
	cdev->ctrl_buf = NULL;

init_exit:
	return ret;
}

/**
  * @brief  DeInitialize UAC device
  * @param  void
  * @retval Status
  */
int usbd_uac_deinit(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	cdev->is_ready = 0U;

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}

	usbd_unregister_class();

	if (cdev->ctrl_buf != NULL) {
		usb_os_mfree(cdev->ctrl_buf);
		cdev->ctrl_buf = NULL;
	}

	if (cdev->isoc_in_buf != NULL) {
		usb_os_mfree(cdev->isoc_in_buf);
		cdev->isoc_in_buf = NULL;
	}

	if (cdev->isoc_out_buf != NULL) {
		usb_os_mfree(cdev->isoc_out_buf);
		cdev->isoc_out_buf = NULL;
	}

	return HAL_OK;
}

int usbd_uac_transmit_data(u8 *buf, u16 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	if (len > USBD_UAC_ISOC_IN_BUF_SIZE) {
		len = USBD_UAC_ISOC_IN_BUF_SIZE;
	}

#if USBD_UAC_ISOC_IN_ENABLE
	usb_os_memcpy(cdev->isoc_in_buf, buf, len);
	usbd_ep_transmit(cdev->dev, USBD_UAC_ISOC_IN_EP, cdev->isoc_in_buf, len);
#endif

	return HAL_OK;
}