/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "usbd_composite_uac.h"
#include "os_wrapper.h"

/* Private defines -----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static int composite_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static int composite_uac_set_config(usb_dev_t *dev, u8 config);
static int composite_uac_clear_config(usb_dev_t *dev, u8 config);
static u8 *composite_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len);
static int composite_uac_handle_ep0_data_out(usb_dev_t *dev);
static int composite_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len);
static int composite_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int composite_uac_receive_isoc_data(void);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "COMP";

/* UAC interface descriptor */
static u8 usbd_composite_uac_itf_desc[] USB_DMA_ALIGNED = {
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                               /* Size of this descriptor (bytes) */
	USBD_UAC_IA_DESCRIPTOR,                         /* Interface Association Descriptor type */
	USBD_COMP_UAC_AC_HEADSET,                     /* First Index: Audio Control Interface Index - Headset (0) */
	0x02,                                           /* 1 Audio Control Interface, 2 Audio Streaming Interfaces (Mic + Headphones) */
	USBD_UAC_IF_CLASS_AUDIO,                        /* Audio Device Class  */
	0x00,                                           /* No subclass */
	USBD_UAC_VERSION_02_00,                         /* Audio Protocol IP version 2.00 */
	0x00,                                           /* Function string descriptor index (0) */

	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                                /* bLength */
	USB_DESC_TYPE_INTERFACE,                        /* Interface Descriptor type */
	USBD_COMP_UAC_AC_HEADSET,                     /* Audio Control Interface Index - Headset (0) */
	0x00,                                           /* bAlternateSetting */
	0x00,                                           /* No associated endpoints with this interface (uses endpoint 0) */
	USBD_UAC_CLASS_CODE,                            /* Audio Device Class */
	USBD_UAC_SUBCLASS_AUDIOCONTROL,                 /* Audio Control Interface */
	USBD_UAC_VERSION_02_00,                         /* Audio Protocol IP version 2.00 */
	0x00,                                           /* Interface string descriptor index (0) */

	/* Audio headset */
	USBD_UAC_LEN_CTRL_IF_HEADER,                    /* Size of the descriptor, in bytes  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,          /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_HEADER,             /* HEADER descriptor subtype  */
	USB_LOW_BYTE(0x0200),                           /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USBD_UAC_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER,    /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */
	USB_LOW_BYTE(USBD_UAC_AUDIO_CTRL_CLASS_SIZE),
	USB_HIGH_BYTE(USBD_UAC_AUDIO_CTRL_CLASS_SIZE),  /* Total number of bytes returned for the class-specific AudioControl interface descriptor. Includes
                                                       the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                           /* D1..0: Latency Control  */

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	USBD_UAC_LEN_CLK_SRC_DESC,                     /* Size of the descriptor, in bytes  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,         /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,      /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,  /* Constant uniquely identifying the Clock Source Entity within
                                                         the audio funcion */
	0x01,                                             /* D1..0: 01: Internal Fixed Clock
                                                         D2: 0 Clock is not synchronized to SOF
                                                         D7..3: Reserved, should set to 0   */
	0x07,                                             /* D1..0: Clock Frequency Control is present and Host programmable
                                                         D3..2: Clock Validity Control is present but read-only
                                                         D7..4: Reserved, should set to 0 */
	0x00,                                             /* This Clock Source has no association   */
	0x00,                                             /* Index of a string descriptor, describing the Clock Source Entity  */


	/* Microphone Audio Control Input Terminal Descriptor 2.0 */
	USBD_UAC_LEN_AC_IN_TTY_DESC,                      /* Size of the descriptor, in bytes  */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,            /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_INPUT_TERMINAL,       /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                             /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                         AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */
	0x00,                                             /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,  /* ID of the Clock Entity to which this Input Terminal is connected.  */
	0x06,                                             /* This Terminal's output audio channel cluster has 6 logical output channels   */
	0x3F,
	0x00,
	0x00,
	0x00,                                             /* Describes the spatial location of the logical channels:: Mono, no spatial location */
	0x00,                                             /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                             /* bmControls D1..0: Copy Protect Control is not present
                                                          D3..2: Connector Control is not present
                                                          D5..4: Overload Control is not present
                                                          D7..6: Cluster Control is not present
                                                          D9..8: Underflow Control is not present
                                                          D11..10: Overflow Control is not present
                                                          D15..12: Reserved, should set to 0*/
	0x00,                                             /* Index of a string descriptor, describing the Input Terminal.  */

	/* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_LEN_FUNC_UNIT_DESC,                      /* Size of the descriptor, in bytes  : 6 + (4 + 1) * 4 */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,            /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AC_IF_DESC_SUBTYPE_FEATURE_UNIT,         /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, /* Constant uniquely identifying the Unit within the audio function. This
                                                          value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x0F,
	0x00,
	0x00,
	0x00,                                             /* logic channel 0 bmaControls(0)(0x0000000F):  D1..0: Mute Control is present and host
                                                          programmable D3..2: Volume Control is present and host programmable D5..4: Bass
                                                          Control is not present D7..6: Mid Control is not present D9..8: Treble Control is not
                                                          present D11..10: Graphic Equalizer Control is not present D13..12: Automatic Gain
                                                          Control is not present D15..14: Delay Control is not present D17..16: Bass Control is
                                                          not present D19..18: Loudness Control is not present D21..20: Input Gain Control is
                                                          not present D23..22: Input Gain Pad Control is not present D25..24: Phase Inverter
                                                          Control is not present D27..26: Underflow Control is not present D29..28: Overflow
                                                          Control is not present D31..30: Reserved, should set to 0 */
	0x00,
	0x00,
	0x00,
	0x00,                                             /* The Controls bitmap for logical channel 1. */
	0x00,
	0x00,
	0x00,
	0x00,                                             /* The Controls bitmap for logical channel 2. */
	0x00,
	0x00,
	0x00,
	0x00,                                             /* The Controls bitmap for logical channel 3. */
	0x00,
	0x00,
	0x00,
	0x00,                                             /* The Controls bitmap for logical channel 4. */
	0x00,
	0x00,
	0x00,
	0x00,                                             /* The Controls bitmap for logical channel 5. */
	0x00,
	0x00,
	0x00,
	0x00,                                             /* The Controls bitmap for logical channel 6. */
	0x00,                                             /* Index of a string descriptor, describing this Feature Unit.   */

	/* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_LEN_AC_OUT_TTY_DESC,                     /* Size of the descriptor, in bytes   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,            /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AC_IF_DESC_SUBTYPE_OUTPUT_TERMINAL,      /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES,  /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */
	0x06,
	0x03,                                             /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                         AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field.  */
	0x00,                                             /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,/* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,  /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                             /* bmControls:   D1..0: Copy Protect Control is not present
                                                          D3..2: Connector Control is not present
                                                          D5..4: Overload Control is not present
                                                          D7..6: Underflow Control is not present
                                                          D9..8: Overflow Control is not present
                                                          D15..10: Reserved, should set to 0   */
	0x00,                                             /* Index of a string descriptor, describing the Output Terminal.  */

	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                  /* Descriptor size is 9 bytes   */
	USB_DESC_TYPE_INTERFACE,                          /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,            /* The number of this interface is 1.   */
	0x00,                                             /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                             /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USBD_UAC_IF_CLASS_AUDIO,                          /* The interface implements the Audio Interface class   */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                 /* The interface implements the AUDIOSTREAMING Subclass   */
	USBD_UAC_VERSION_02_00,                           /* The Protocol code is 32   */
	0x00,                                             /* The interface string descriptor index is 0   */

	/* Interface1  Alt Setting 1 */
	USB_LEN_IF_DESC,                                 /* Descriptor size is 9 bytes   */
	USB_DESC_TYPE_INTERFACE,                         /* INTERFACE Descriptor Type   */
	USBD_COMP_UAC_AS_HEADSET_HEADPHONES,           /* The number of this interface is 1.  */
	0x01,                                            /* The value used to select the alternate setting for this interface is 1   */
	0x01,                                            /* The number of endpoints used by this interface is 1 (excluding endpoint zero)    */
	USBD_UAC_IF_CLASS_AUDIO,                         /* The interface implements the Audio Interface class  */
	USBD_UAC_SUBCLASS_AUDIOSTREAMING,                /* The interface implements the AUDIOSTREAMING Subclass  */
	USBD_UAC_VERSION_02_00,                          /* The Protocol code is 32   */
	0x0,                                             /* The interface string descriptor index is 2  */

	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USBD_UAC_LEN_AS_IF_ALT_SET_DESC,                /* Size of the descriptor, in bytes   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,          /* CS_INTERFACE Descriptor Type  */
	USBD_UAC_AS_IF_DESC_SUBTYPE_AS_GENERAL,         /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                           /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                       D3..2: Valid Alternate Settings Control is not present
                                                       D7..4: Reserved, should set to 0   */
	0x01,                                           /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                           /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	0x06,                                           /* Number of physical channels in the AS Interface audio channel cluster */
	0x3F,
	0x00,
	0x00,
	0x00,                                            /* Describes the spatial location of the logical channels: */
	0x00,                                            /* Index of a string descriptor, describing the name of the first physical channel   */

	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USBD_UAC_LEN_AS_FRT_TYPE_ALT_SET_DESC,           /* Size of the descriptor, in bytes   */
	USBD_UAC_DESC_TYPE_AUDIO_CS_INTERFACE,           /* CS_INTERFACE Descriptor Type   */
	USBD_UAC_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,         /* FORMAT_TYPE descriptor subtype   */
	0x01,                                            /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x03,                                            /* The number of bytes occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	0x18,                                            /* The number of effectively used bits from the available bits in an audio subslot   */

	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                 /* Descriptor size is 7 bytes */
	USB_DESC_TYPE_ENDPOINT,                          /* ENDPOINT Descriptor Type*/
	USBD_COMP_UAC_ISOC_OUT_EP,                            /* This is an OUT endpoint with endpoint number 2   */
	0x0D,                                            /* Types -
                                                           Transfer: ISOCHRONOUS
                                                           Sync: Async
                                                           Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_FS_ISOC_MPS),              /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_FS_ISOC_MPS),
	USBD_UAC_FS_ISOC_OUT_INTERVAL,                   /* bInterval */

	/* Headphone Audio Data Endpoint Descriptor */
	USBD_UAC_LEN_DATA_EP_DESC,                       /* Size of this descriptor (bytes) */
	USBD_UAC_CS_ENDPOINT_DESCRIPTOR,                 /* Class-specific Endpoint Descriptor type */
	USBD_UAC_AS_EP_DESC_SUBTYPE_EP_GENERAL,          /* EP_GENERAL Descriptor subtype */
	0x00,                                            /* Can handle short packets (D7 = 0) */
	0x00,                                            /* No controls */
	0x00,                                            /* Undefined lock delay units */
	USB_LOW_BYTE(0x0000),                            /* Time it takes this endpoint to lock its */
	USB_HIGH_BYTE(0x0000),                           /* internal clock recovery circuitry */
};  /* usbd_uac_fs_config_desc */

usbd_class_driver_t usbd_composite_uac_driver = {
	.get_descriptor = composite_uac_get_descriptor,
	.set_config = composite_uac_set_config,
	.clear_config = composite_uac_clear_config,
	.setup = composite_uac_setup,
	.ep0_data_out = composite_uac_handle_ep0_data_out,
	.ep_data_in = composite_uac_handle_ep_data_in,
	.ep_data_out = composite_uac_handle_ep_data_out,
	.status_changed = NULL,
};

static usbd_composite_uac_device_t composite_uac_device;

/* Private functions ---------------------------------------------------------*/
static void composite_uac_cur_freq_req(usb_dev_t *dev, u32 frequencyHz)
{
	u32 dCur = frequencyHz;

	memcpy(dev->ctrl_buf, &dCur, sizeof(dCur));
	usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(dCur));
}

static void composite_uac_clk_valid_req(usb_dev_t *dev, u8 flag)
{
	u8 valid = flag;

	memcpy(dev->ctrl_buf, &valid, sizeof(valid));
	usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(valid));
}

static void composite_uac_freq_ctrl_range_req(usb_dev_t *dev, u32 minFrequencyHz, u32 maxFrequencyHz, u16 max_len)
{
	usbd_uac_freq_ctrl_range_t response;
	u16 len = sizeof(response);

	response.wNumSubRanges = 1;
	response.SubRanges[0].bMIN = minFrequencyHz;
	response.SubRanges[0].bMAX = maxFrequencyHz;
	response.SubRanges[0].bRES = 0;

	if (len > max_len) {
		len = max_len;
	}

	memcpy(dev->ctrl_buf, &response, len);
	usbd_ep0_transmit(dev, dev->ctrl_buf, len);
}

static void composite_uac_connect_ctrl_req(usb_dev_t *dev, u8 numChannels, u32 channelConfigMask)
{
	usbd_uac_ac_connect_ctrl_t response = {0, 0, 0};
	response.bNrChannels = numChannels;
	response.bmChannelConfig = channelConfigMask; //endian issue FIXME

	memcpy(dev->ctrl_buf, &response, sizeof(response));
	usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(response));
}

static int composite_uac_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_composite_uac_device_t *uac = &composite_uac_device;
	usbd_composite_dev_t *cdev = uac->dev;
	u8 entityId, controlSelector;
	int ret = HAL_OK;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				uac->alt_setting = (u8)(req->wValue & 0xFF);
			} else {
				ret = HAL_ERR_HW;
			}
			break;
		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				cdev->ctrl_buf[0] = uac->alt_setting;
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
		entityId = USB_HIGH_BYTE(req->wIndex);
		controlSelector = USB_HIGH_BYTE(req->wValue);

		//RTK_LOGS(TAG, RTK_LOG_DEBUG, "SETUP: id=0x%02x cs=%d\n", entityId, controlSelector);

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE: { //0x12 in clk
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						// USBD UAC mic in uses a 24 KHz sampling frequency.
						composite_uac_cur_freq_req(dev, 24000);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						// USBD UAC mic in uses a 24 KHz sampling frequency.
						composite_uac_freq_ctrl_range_req(dev, 24000, 24000, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}

			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES: { //0x15 out clk
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						composite_uac_cur_freq_req(dev, 48000);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
						composite_uac_freq_ctrl_range_req(dev, 48000, 48000, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else if (controlSelector == USBD_UAC_CS_CLK_VALID_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						composite_uac_clk_valid_req(dev, uac->cur_clk_valid);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE: {  //0x02 input
				if (controlSelector == USBD_UAC_TE_CONNECTOR_CONTROL) { //02
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) { //01
						composite_uac_connect_ctrl_req(dev, 1, 0x1);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}
			break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES: { //0x09 output
				if (controlSelector == USBD_UAC_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						composite_uac_connect_ctrl_req(dev, 6, 0x3F);    //6 channel
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}
			break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: { //0x05 out volume issue
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) { //mute ctrl  //01
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						dev->ctrl_buf[0] = uac->cur_mute;
						usbd_ep0_transmit(dev, dev->ctrl_buf, 1);
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume ctrl  //02
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {  //01  cur volume
						memcpy(dev->ctrl_buf, (u8 *) & (uac->cur_volume), 2);
						usbd_ep0_transmit(dev, dev->ctrl_buf, 2);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {  //02 volume ctrl range

						usbd_uac_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.bMIN = USBD_UAC_VOLUME_CTRL_MIN;
						response.bMAX = USBD_UAC_VOLUME_CTRL_MAX;
						response.bRES = 1;

						memcpy(dev->ctrl_buf, &response, sizeof(response));
						usbd_ep0_transmit(dev, dev->ctrl_buf, sizeof(response));
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: 0x13 wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;

				default:
					RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wIndex err %d\n", entityId);
					break;
				}
			}
		} else {  /* USB_H2D */
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES: { //0x15 Out clk
				if (controlSelector == USBD_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "Set freq err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Set freq ctrl err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: { //0x05 FU
				if (controlSelector == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR) { //mute
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "Set cur mute err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else if (controlSelector == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume
					if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
					} else if (req->bRequest == USBD_UAC_CLASS_REQ_CODE_RANGE) {
					} else {
						RTK_LOGS(TAG, RTK_LOG_WARN, "Set cur volume range err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_WARN, "Set fu err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}

				break;
			}

			default:
				RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: wIndex err %d\n", entityId);
				break;
			}
		}
		break;

	case USB_REQ_TYPE_VENDOR:
		if (req->wLength) {
			if (req->bmRequestType & 0x80U) {
				ret = uac->cb->setup(req, cdev->ctrl_buf);
				if (ret == HAL_OK) {
					usbd_ep0_transmit(dev, cdev->ctrl_buf, req->wLength);
				}
			} else {
				usb_os_memcpy((void *)&cdev->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				usbd_ep0_receive(dev, cdev->ctrl_buf, req->wLength);
			}
		} else {
			uac->cb->setup(req, cdev->ctrl_buf);
		}
		break;

	default:
		RTK_LOGS(TAG, RTK_LOG_WARN, "SETUP: bmRequestType err\n");
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

static int composite_uac_set_config(usb_dev_t *dev, u8 config)
{
	u16 ep_mps;
	int ret = HAL_OK;
	usbd_composite_uac_device_t *cdev = &composite_uac_device;

	UNUSED(config);

	cdev->alt_setting = 0U;

	/* Init ISO OUT EP */
	ep_mps = (dev->dev_speed == USB_SPEED_HIGH) ? USBD_UAC_HS_ISOC_MPS : USBD_UAC_FS_ISOC_MPS;
	usbd_ep_init(dev, USBD_COMP_UAC_ISOC_OUT_EP, USB_CH_EP_TYPE_ISOC, ep_mps);
	usbd_ep_receive(dev, USBD_COMP_UAC_ISOC_OUT_EP, cdev->isoc_out_buf, USBD_UAC_ISOC_OUT_BUF_SIZE);

	cdev->is_ready = 1U;

	return ret;
}

static int composite_uac_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_composite_uac_device_t *cdev = &composite_uac_device;

	UNUSED(config);

	cdev->is_ready = 0U;

	/* DeInit ISOC OUT EP */
	usbd_ep_deinit(dev, USBD_COMP_UAC_ISOC_OUT_EP);

	return ret;
}


static int composite_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	UNUSED(dev);
	UNUSED(ep_addr);
	UNUSED(status);

	return HAL_OK;
}
static int composite_uac_receive_isoc_data(void)
{
	usbd_composite_uac_device_t *cdev = &composite_uac_device;

	if (!cdev->is_ready) {
		return HAL_ERR_HW;
	}

	usbd_ep_receive(cdev->dev->dev, USBD_COMP_UAC_ISOC_OUT_EP, cdev->isoc_out_buf, USBD_UAC_ISOC_OUT_BUF_SIZE);

	return HAL_OK;
}

static int composite_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u16 len)
{
	usbd_composite_uac_device_t *cdev = &composite_uac_device;

	UNUSED(dev);

	if (len == 0) {
		/*RX ZLP*/
		return HAL_OK;
	}

	if (ep_addr == USBD_COMP_UAC_ISOC_OUT_EP) {
		cdev->cb->isoc_received(cdev->isoc_out_buf, len);
	}

	return composite_uac_receive_isoc_data();
}

/**
  * @brief  Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int composite_uac_handle_ep0_data_out(usb_dev_t *dev)
{
	UNUSED(dev);

	float db = 0;
	int ret = HAL_OK;
	u16 volume_value = 0;
	usbd_composite_uac_device_t *cdev = &composite_uac_device;
	usbd_composite_dev_t *mdev = cdev->dev;

	usbd_composite_uac_usr_cb_t *cb = cdev->cb;

	usb_setup_req_t *p_ctrl_req = &(mdev->ctrl_req);

	if ((((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && ((p_ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_INTERFACE))
		&& (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT)
		&& (p_ctrl_req->bRequest == USBD_UAC_CLASS_REQ_CODE_CUR)) {

		if ((cb->mute_changed != NULL)
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_MUTE_CONTROL_SELECTOR)
			&& (p_ctrl_req->wLength == 0x01)) {
			cdev->cur_mute = mdev->ctrl_buf[0];
			cb->mute_changed(cdev->cur_mute);
		} else if ((cb->volume_changed != NULL)
				   && (USB_HIGH_BYTE(p_ctrl_req->wValue) == USBD_UAC_CTRL_FU_VOLUME_CONTROL_SELECTOR)
				   && (p_ctrl_req->wLength == 0x02)) {

			volume_value = (u16)mdev->ctrl_buf[0] + ((u16)(mdev->ctrl_buf[1]) << 8);
			cdev->cur_volume = volume_value;

			//fix me
			if (!(volume_value & 0x8000)) { // 0db
				db = 0.0;
			} else if (volume_value <= WINDOWS_MIN_VOLUME) {  // -45db
				db = -45.0;
			} else {
				volume_value = 0x10000 - volume_value ;
				db = -1.0 * volume_value / 256 ;
			}
			volume_value = (u16)(100 * (45.0 + db) / 45.0);
			//covert the data to 0~100
			cb->volume_changed(USB_LOW_BYTE(volume_value));
		}
	}

	return ret;
}


static u8 *composite_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, usb_speed_type_t speed, u16 *len)
{
	u8 *buf = NULL;

	UNUSED(dev);
	UNUSED(speed);

	switch ((req->wValue >> 8) & 0xFF) {

	case USB_DESC_TYPE_CONFIGURATION:
		buf = usbd_composite_uac_itf_desc;
		*len = sizeof(usbd_composite_uac_itf_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		buf = usbd_composite_uac_itf_desc;
		*len = sizeof(usbd_composite_uac_itf_desc);
		break;

	default:
		break;
	}

	return buf;
}

/* Exported functions --------------------------------------------------------*/

int usbd_composite_uac_init(usbd_composite_dev_t *dev, usbd_composite_uac_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_composite_uac_device_t *cdev = &composite_uac_device;

	cdev->cur_volume = 0x001F;
	cdev->cur_mute = 0;
	cdev->cur_clk_valid = 1;

	cdev->isoc_out_buf = (u8 *)usb_os_malloc(USBD_UAC_ISOC_OUT_BUF_SIZE);
	if (cdev->isoc_out_buf == NULL) {
		ret = HAL_ERR_MEM;
		goto init_exit;
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

	cdev->dev = dev;

	return ret;

init_clean_isoc_in_buf_exit:
	usb_os_mfree(cdev->isoc_out_buf);
	cdev->isoc_out_buf = NULL;

init_exit:
	return ret;
}

int usbd_composite_uac_deinit(void)
{
	usbd_composite_uac_device_t *cdev = &composite_uac_device;

	cdev->is_ready = 0U;

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}

	usbd_unregister_class();

	if (cdev->isoc_out_buf != NULL) {
		usb_os_mfree(cdev->isoc_out_buf);
		cdev->isoc_out_buf = NULL;
	}

	return HAL_OK;
}
