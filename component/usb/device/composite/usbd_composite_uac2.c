/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

/* uac 2.0 spec */
#include "usbd_composite_uac.h"
#include "os_wrapper.h"
#include "usb_uac2.h"

/* Private defines -----------------------------------------------------------*/
#if USBD_COMPOSITE_UAC_DEBUG
#define USBD_UAC_DEBUG_LOOP_TIME           2000
#endif

#define USBD_COMPOSITE_ISOC_BUF_LENGTH      1024U
#define USBD_UAC_WAIT_SLICE_MS              5

#define UABD_UAC_VOL_ERR_VAL                255

#define USBD_UAC_HS_ISOC_MPS                        1024   /* High speed ISOC IN & OUT max packet size */
#define USBD_UAC_FS_ISOC_MPS                        1023   /* Full speed ISOC IN & OUT max packet size */

#define USBD_UAC_SELF_POWERED                       1U
#define USBD_UAC_LANGID_STRING                      0x0409U
#define USBD_UAC_MFG_STRING                         "Realtek"
#define USBD_UAC_PROD_HS_STRING                     "Realtek UAC2.0 (HS)"
#define USBD_UAC_PROD_FS_STRING                     "Realtek UAC2.0 (FS)"
#define USBD_UAC_SN_STRING                          "1234567890"

/**
 * Defines Audio trx buffer MAX count.
 */
#define USBD_UAC_RX_BUF_MAX_CNT                     20     /**< RX Ringbuf count */
#define USBD_UAC_TX_BUF_MAX_CNT                     10     /**< TX Ringbuf count */

/* 1ms 8 frame in high speed */
#define USBD_UAC_HS_SOF_COUNT_PER_MS                8U         /**< Number of SOF packets per millisecond in High-Speed mode. */
#define USBD_UAC_ONE_KHZ                            1000U      /**< Constant for 1 kHz. */

/**
 * @name Volume Control Range
 * @details Defines the min/max volume range. The UAC spec allows 0x8001 to 0x7FFF
 *          (-127dB to +127dB), but this range is narrowed to avoid warnings on some OS.
 */
#define USBD_UAC_VOLUME_CTRL_MIN                    0xFF42     /**< Minimum volume level (-190, corresponds to -190/256 dB). */
#define USBD_UAC_VOLUME_CTRL_MAX                    0x00BE     /**< Maximum volume level (190, corresponds to 190/256 dB). */

/*
   * default isoc bInterval
   *        ISOC
   * FS    1-16,2^(N-1)
   * HS    1-16,2^(N-1)
   * for FS, use 1; for HS use 4, which will report data per second
*/
#define USBD_UAC_HS_ISOC_OUT_BINTERVAL                 4
#define USBD_UAC_FS_ISOC_OUT_BINTERVAL                 1
#define USBD_UAC_HS_ISOC_IN_BINTERVAL                  4
#define USBD_UAC_FS_ISOC_IN_BINTERVAL                  1


/* limit */
#define USBD_UAC_HS_SAMPLING_FREQ_COUNT               USBD_UAC_SAMPLING_FREQ_MAX_COUNT

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT               1U
#elif USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT               1U
#endif
#else
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT               USBD_UAC_SAMPLING_FREQ_MAX_COUNT
#endif

// USB Audio terminal
/* USB Out Path (Host to Device, e.g., Speaker) */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES           0x15U /**< Clock Source for Speaker path. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES   0x01U /**< Input Terminal (USB Streaming). */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT         0x05U /**< Feature Unit. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES  0x09U /**< Output Terminal (Headphones). */
/* USB In Path (Device to Host, e.g., Microphone) */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE           0x12U /**< Clock Source for Mic path. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE   0x02U /**< Input Terminal (Microphone). */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT          0x08U /**< Feature Unit. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE  0x10U /**< Output Terminal (USB Streaming). */

#define USBD_UAC_POW2(n)                            (1 << (n))

/* UAC channel config */
#define USBD_UAC_GET_CH_CONFIG(ch_cnt) \
    ((ch_cnt) == 2 ? 0x03 : \
     (ch_cnt) == 4 ? 0x0F : \
     (ch_cnt) == 6 ? 0x3F : \
     (ch_cnt) == 8 ? 0xFF : \
     (ch_cnt) == 16 ? 0xFFFF : 0x03)

/* UAC terminal type */
#define USBD_UAC_GET_OT_TYPE(ch_cnt) \
    ((ch_cnt) == 2 ? 0x0301 : \
     (ch_cnt) == 4 ? 0x0304 : \
     (ch_cnt) == 6 ? 0x0304 : \
     (ch_cnt) == 8 ? 0x0307 : \
     (ch_cnt) == 16 ? 0x0307 : 0x0301)

/* Private macros ------------------------------------------------------------*/

/* AC feature uint descriptor length */
#define USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt)        (USB_UAC2_LEN_FUNC_UNIT_DESC + 1 + 4 * (ch_cnt))

/* AC IF header descriptor length */
#define USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt) \
    (USB_UAC2_LEN_CTRL_IF_HEADER + USB_UAC2_LEN_CLK_SRC_DESC + USB_UAC2_LEN_AC_IN_TTY_DESC + \
    USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt) + USB_UAC2_LEN_AC_OUT_TTY_DESC)

/* len of total Audio control interface */
#define USBD_UAC_AC_IF_LEN(ch_cnt)                 (USB_LEN_IF_DESC + USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt))

/* len of each Audio stream interface/altsetting (one EP) */
#define USBD_UAC_AS_EIF_LEN                        (USB_LEN_IF_DESC + USB_UAC2_LEN_AS_IF_ALT_SET_DESC + USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC \
                                                   + USB_LEN_EP_DESC + USB_UAC2_LEN_DATA_EP_DESC)

/* len of total Audio stream interface */
#define USBD_UAC_AS_TIF_LEN(alt_num)   (USBD_UAC_AS_EIF_LEN * (alt_num - 1) + USB_LEN_IF_DESC)

/* full speed AS alt setting num */
#define USBD_UAC_FS_AS_ALT_SETTING_NUM              5U

/* high speed AS alt setting num */
#define USBD_UAC_HS_AS_ALT_SETTING_NUM              5U

/* len of full speed total configuration descriptor buf */
#define USBD_UAC_FS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_FS_AS_ALT_SETTING_NUM))

/* len of high speed total configuration descriptor buf */
#define USBD_UAC_HS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_HS_AS_ALT_SETTING_NUM))

/* calculate full speed MPS */
#define USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, sampling_freq_hz, interval) \
    (USBD_UAC_POW2(interval-1) * (ch_cnt) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ + 1U))

/* check MPS */
#define USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, sampling_freq_hz) \
    ((USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, sampling_freq_hz)) <= USBD_UAC_FS_ISOC_MPS)

/* get full speed MPS, if MPS > limit, choose next lower sampling freq to calculate */
#define USBD_UAC_GET_FS_MPS(ch_cnt, byte_width) \
    (USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) ? \
    USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) : \
    (USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) ? \
    USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) : \
    (USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) ? \
    USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) : \
    (USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) ? \
    USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) : 0))))

/* calculate high speed MPS */
#define USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, sampling_freq_hz, interval) \
  (USBD_UAC_POW2(interval-1) * (ch_cnt) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS + 1U))

/* check MPS */
#define USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, sampling_freq_hz) \
    ((USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, sampling_freq_hz)) <= USBD_UAC_HS_ISOC_MPS)

/* get high speed MPS, if MPS > limit, choose next lower sampling freq to calculate */
#define USBD_UAC_GET_HS_MPS(ch_cnt, byte_width) \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_192K) : \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_96K) : \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_48K) : \
    (USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) ? \
    USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_SAMPLING_FREQ_44K) : 0))))

/* len of high speed total configuration descriptor buf */
#define USBD_UAC_HS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_HS_AS_ALT_SETTING_NUM))

/* len of full speed total configuration descriptor buf */
#define USBD_UAC_FS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_FS_AS_ALT_SETTING_NUM))

/* Input terminal */
#define USBD_UAC_CH_CONFIG_TYPE_LOW(ch_cnt)         (USB_LOW_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))
#define USBD_UAC_CH_CONFIG_TYPE_HIGH(ch_cnt)        (USB_HIGH_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))

/* Output terminal */
#define USBD_UAC_OT_DESC_TYPE_LOW(ch_cnt)           (USB_LOW_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))
#define USBD_UAC_OT_DESC_TYPE_HIGH(ch_cnt)          (USB_HIGH_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))

#define USBD_UAC_INIT_SUB_RANGE(sub_range, min_sampling_freq, max_sampling_freq, res) \
    do {                                               \
        (sub_range).dMIN = (min_sampling_freq);        \
        (sub_range).dMAX = (max_sampling_freq);        \
        (sub_range).dRES = (res);                      \
    } while(0)

/* Private types -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
static int usbd_composite_uac_set_config(usb_dev_t *dev, u8 config);
static int usbd_composite_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_composite_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_composite_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf);
static int usbd_composite_uac_sof(usb_dev_t *dev);
static int usbd_composite_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status);
static int usbd_composite_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int usbd_composite_uac_handle_ep0_data_out(usb_dev_t *dev);
static void usbd_composite_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
static inline u16 usbd_composite_uac_get_ring_buf_cnt(u8 speed, u8 is_in);
static u16 usbd_composite_uac_get_mps(usbd_audio_cfg_t *params, u8 speed, u8 is_in);
static u8 usbd_composite_uac_get_ch_config(u8 ch_cnt);
static inline u8 usbd_composite_uac_ep_enable(usbd_audio_cfg_t *ep);
static void usbd_composite_uac_clk_valid_req(usb_dev_t *dev, u8 flag);
static void usbd_composite_uac_cur_sampling_freq_req(usb_dev_t *dev, u32 sampling_freq);
static int usbd_composite_uac_sampling_freq_ctrl_range_req(usb_dev_t *dev, u16 max_len, u8 is_in);
static void usbd_composite_uac_connect_ctrl_req(usb_dev_t *dev, u8 ch_num, u32 ch_cfg, u16 max_len);
static int usbd_composite_uac_is_valid_sample_rate(u32 sampling_freq, u8 speed);
#if USBD_COMPOSITE_UAC_DEBUG
static void usbd_composite_uac_status_dump_thread(void *param);
static inline void usbd_composite_uac_get_audio_data_cnt(u32 audio_len);
static int usbd_composite_uac_get_read_buf_cnt(void)
{
	return 0;
}
#endif

/* Private variables ---------------------------------------------------------*/
static const char *const TAG = "UAC2";

static u32 usbd_composite_uac_isoc_out_freq[USBD_UAC_SAMPLING_FREQ_MAX_COUNT] = {USBD_UAC_SAMPLING_FREQ_48K};

static u8 usbd_composite_uac_rx_buf[USBD_COMPOSITE_ISOC_BUF_LENGTH] __attribute__((aligned(CACHE_LINE_SIZE)));
static u8 usbd_composite_uac_tx_buf[USBD_COMPOSITE_ISOC_BUF_LENGTH] __attribute__((aligned(CACHE_LINE_SIZE)));
/* Used for the trailing partial frame of data when the application performs a write operation. */
static u8 usbd_composite_uac_ringbuf_write_buf[USBD_COMPOSITE_ISOC_BUF_LENGTH];

#define UAC_IAD_DESCRIPTOR(first_interface,interface_count)                          \
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */    \
	USB_UAC2_IAD_DESCRIPTOR_TYPE,      /* Interface Association Descriptor type */   \
	first_interface,                   /* First Index: Audio Control Interface */    \
	interface_count,                   /* Audio Control + Audio Streaming  */        \
	USB_UAC2_IF_CLASS_AUDIO,           /* Audio Device Class  */                     \
	0x00,                              /* No subclass */                             \
	USB_UAC2_VERSION_02_00,            /* Audio Protocol IP version 2.00 */          \
	0x00,                              /* Function string descriptor index (0) */

#define UAC_INTERFACE_DESCRIPTOR(interface_number,alt_setting,num_endpoints, interface_subclass)   \
	USB_LEN_IF_DESC,                   /* bLength */                             \
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */           \
	interface_number,                  /* Interface Index */                     \
	alt_setting,                       /* bAlternateSetting */                   \
	num_endpoints,                     /* Endpoints num with this interface */   \
	USB_UAC2_CLASS_CODE,               /* Audio Device Class */                  \
	interface_subclass,                /* Audio Control Interface */             \
	USB_UAC2_VERSION_02_00,            /* Audio Protocol IP version 2.00 */      \
	0x00,                              /* Interface string descriptor index (0) */

#define UAC_AC_HEADER_DESCRIPTOR()   \
	USB_UAC2_LEN_CTRL_IF_HEADER,                                          /* Size of the descriptor, in byte_width  */ \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                                /* CS_INTERFACE Descriptor Type   */ \
	USB_UAC2_AC_IF_DESC_SUBTYPE_HEADER,                                   /* HEADER descriptor subtype  */ \
	USB_LOW_BYTE(0x0200),                                                 /* bcdUSB */ \
	USB_HIGH_BYTE(0x0200), \
	USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER,                          /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */ \
	USB_LOW_BYTE(USB_UAC2_LEN_CTRL_IF_HEADER + USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_OUT_DEFAULT_CH_CNT) + USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_IN_DEFAULT_CH_CNT)), \
	USB_HIGH_BYTE(USB_UAC2_LEN_CTRL_IF_HEADER  + USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_OUT_DEFAULT_CH_CNT) + USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_IN_DEFAULT_CH_CNT)), \
                                                                           /* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                           the combined length of this descriptor header and all Unit and Terminal descriptors.   */  \
	0x00,                                                                 /* D1..0: Latency Control  */

#define UAC_CLK_DESCRIPTOR(clkid)   \
	USB_UAC2_LEN_CLK_SRC_DESC,                         /* Size of the descriptor, in byte_width  */  \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,             /* CS_INTERFACE Descriptor Type  */  \
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,          /* CLOCK_SOURCE descriptor subtype  */  \
	clkid,   /* Constant uniquely identifying the Clock Source Entity within the audio funcion */   \
	0x01,                                              /* D1..0: 01: Internal Fixed Clock
                                                          D2: 0 Clock is not synchronized to SOF
                                                          D7..3: Reserved, should set to 0   */   \
	0x07,                                              /* D1..0: Clock Frequency Control is present and Host programmable
                                                          D3..2: Clock Validity Control is present but read-only
                                                          D7..4: Reserved, should set to 0 */   \
	0x00,                                              /* This Clock Source has no association   */   \
	0x00,                                              /* Index of a string descriptor, describing the Clock Source Entity  */

#define UAC_INPUT_TERTMINAL_DESCRIPTOR(term_id, type, clk_id, ch)   \
	USB_UAC2_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */  \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */  \
	USB_UAC2_AC_IF_DESC_SUBTYPE_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */  \
	term_id,                                                 /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */  \
	USB_LOW_BYTE(type),USB_HIGH_BYTE(type),                   /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */  \
	0x00,                                                    /* This Input Terminal has no association   */  \
	clk_id,         /* ID of the Clock Entity to which this Input Terminal is connected.  */  \
	ch,                              /* This Terminal's input audio channel cluster has 2 logical input channels   */  \
	USBD_UAC_CH_CONFIG_TYPE_LOW(ch),USBD_UAC_CH_CONFIG_TYPE_HIGH(ch),  \
	0x00,                                                    /* BmChannelConfig */  \
	0x00,                                                    /* Describes the spatial location of the logical channels:: Mono, no spatial location */  \
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */  \
	0x00,  \
	0x00,                                                    /* bmControls */  \
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

#define UAC_ISOC_IN_FEATURE_UNIT_DESCRIPTOR(ch, unit_id, src_id)   \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(ch),   /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */ \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */ \
	USB_UAC2_AC_IF_DESC_SUBTYPE_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */ \
	unit_id,        /* Constant uniquely identifying the Unit within the audio function. This value is used in all requests to address this Unit.  */ \
	src_id, /* ID of the Unit or Terminal to which this Feature Unit is connected. */ \
	0x0F,0x00,0x00,0x00,                                    /* logic channel 0 bmaControls(0)(0x0000000F) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[1](0x00000000), eg. 8 channels means bmaControls[1] ~ bmaControls[8] */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[2](0x00000000) */ \
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

#define UAC_ISOC_OUT_2CH_FEATURE_UNIT_DESCRIPTOR(unit_id, src_id)   \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_CH_CNT_2),         /* Size of the descriptor, in byte_width  : 6 + (2 + 1) * 4 */ \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */ \
	USB_UAC2_AC_IF_DESC_SUBTYPE_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */ \
	unit_id,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */ \
	src_id, /* ID of the Unit or Terminal to which this Feature Unit is connected. */ \
	0x0F,0x00,0x00,0x00,                                     /* logic channel 0 bmaControls(0)(0x0000000F) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[1](0x00000000), eg. 8 channels means bmaControls[1] ~ bmaControls[8] */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[2](0x00000000) */ \
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

#define UAC_ISOC_OUT_4CH_FEATURE_UNIT_DESCRIPTOR(unit_id, src_id)   \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_CH_CNT_4),   /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */ \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */ \
	USB_UAC2_AC_IF_DESC_SUBTYPE_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */ \
	unit_id,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */ \
	src_id, /* ID of the Unit or Terminal to which this Feature Unit is connected. */ \
	0x0F,0x00,0x00,0x00,                                     /* logic channel 0 bmaControls(0)(0x0000000F) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[1](0x00000000), eg. 8 channels means bmaControls[1] ~ bmaControls[8] */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[2](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[3](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[4](0x00000000) */ \
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

#define UAC_ISOC_OUT_6CH_FEATURE_UNIT_DESCRIPTOR(unit_id, src_id)   \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_CH_CNT_6),   /* Size of the descriptor, in byte_width  : 6 + (6 + 1) * 4 */ \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */ \
	USB_UAC2_AC_IF_DESC_SUBTYPE_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */ \
	unit_id,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */ \
	src_id, /* ID of the Unit or Terminal to which this Feature Unit is connected. */ \
	0x0F,0x00,0x00,0x00,                                     /* logic channel 0 bmaControls(0)(0x0000000F) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[1](0x00000000), eg. 8 channels means bmaControls[1] ~ bmaControls[8] */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[2](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[3](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[4](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[5](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[6](0x00000000) */ \
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

#define UAC_ISOC_OUT_8CH_FEATURE_UNIT_DESCRIPTOR(unit_id, src_id)   \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_CH_CNT_8),   /* Size of the descriptor, in byte_width  : 6 + (8 + 1) * 4 */ \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */ \
	USB_UAC2_AC_IF_DESC_SUBTYPE_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */ \
	unit_id,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */ \
	src_id, /* ID of the Unit or Terminal to which this Feature Unit is connected. */ \
	0x0F,0x00,0x00,0x00,                                     /* logic channel 0 bmaControls(0)(0x0000000F) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[1](0x00000000), eg. 8 channels means bmaControls[1] ~ bmaControls[8] */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[2](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[3](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[4](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[5](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[6](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[7](0x00000000) */ \
	0x00,0x00,0x00,0x00,                                     /*  bmaControls[8](0x00000000) */ \
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

#define UAC_OUTPUT_TERTMINAL_DESCRIPTOR(unit_id, type, src_id, clk_id)   \
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */ \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */ \
	USB_UAC2_AC_IF_DESC_SUBTYPE_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */ \
	unit_id, /* Constant uniquely identifying the Terminal within the audio
                                                                 function. This value is used in all requests to address this Terminal.   */ \
	USB_LOW_BYTE(type),USB_HIGH_BYTE(type),      /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                 AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field.  */ \
	0x00,                                                     /* This Output Terminal has no association  */ \
	src_id,        /* ID of the Unit or Terminal to which this Terminal is connected.  */ \
	clk_id,          /* ID of the Clock Entity to which this Output Terminal is connected  */ \
	0x00, \
	0x00,                                                     /* bmControls */ \
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */

#define UAC_AS_GENERAL_DESCRIPTOR(term_link, ch)                                                                  \
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                          /* Size of the descriptor, in byte_width   */       \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */                 \
	USB_UAC2_AS_IF_DESC_SUBTYPE_AS_GENERAL,                   /* AS_GENERAL descriptor subtype   */               \
	term_link,                                                /* Terminal ID which this interface is connected */ \
	0x00,                                                     /* bmControls  */                                   \
	0x01,                                                     /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01) */   \
	0x01,0x00,0x00,0x00,                                      /* Audio Data Format that can be Used to communicate with this interface */   \
	ch,                                                       /* Number of physical channels in the AS Interface audio channel cluster */   \
	USBD_UAC_CH_CONFIG_TYPE_LOW(ch), USBD_UAC_CH_CONFIG_TYPE_HIGH(ch),/* Describes the spatial location of the logical channels: */         \
	0x00,                                                     /* Describes the spatial location of the logical channels: */                 \
	0x00,                                                     /* Describes the spatial location of the logical channels: */                 \
	0x00,                                                     /* Index of a string descriptor, describing the name of the first physical channel   */

#define UAC_AS_FORMAT_DESCRIPTOR(ch)   \
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */ \
	USB_UAC2_DESC_TYPE_AUDIO_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */          \
	USB_UAC2_AS_IF_DESC_SUBTYPE_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */        \
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */      \
	ch,                                                      /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4. */ \
	USBD_UAC_BIT_WIDTH(ch),                                  /* The number of effectively used bits from the available bits in an audio subslot */

#define UAC_EP_DESCRIPTOR(addr,attr,binterval,mps)                                                 \
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */ \
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/         \
	addr,                                                    /* Endpoint number    */              \
	attr,                                                    /* Types -Transfer: Sync: Usage: */   \
	USB_LOW_BYTE(mps),USB_HIGH_BYTE(mps),                    /* wMaxPacketSize: */                 \
	binterval,                                               /* bInterval */

#define UAC_DATA_EP_DESCRIPTOR()                                                                          \
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */   \
	USB_UAC2_CS_ENDPOINT_DESCRIPTOR,                         /* Class-specific Endpoint Descriptor type */\
	USB_UAC2_AS_EP_DESC_SUBTYPE_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */          \
	0x00,                                                    /* Can handle short packets (D7 = 0) */      \
	0x00,                                                    /* No controls */                            \
	0x00,                                                    /* Undefined lock delay units */             \
	0x00,                                                    /* Time it takes this endpoint to lock its */\
	0x00,                                                    /* internal clock recovery circuitry */

/* UAC interface descriptor */
static const u8 usbd_composite_uac_hs_itf_desc[] = {
	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	UAC_IAD_DESCRIPTOR(USBD_COMP_UAC_AC_HEADSET, USBD_UAC_AC_IF_NUM)

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AC_HEADSET, 0x00, 0x00, USB_UAC2_SUBCLASS_AUDIOCONTROL)

	/* 4.7.2 Class-Specific Audio Control Interface Header Descriptor */
	/* Audio headset */
	UAC_AC_HEADER_DESCRIPTOR()

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	UAC_CLK_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE)
	UAC_CLK_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)

	/*---------Microphone------------*/
	/* Microphone Audio Control Input Terminal Descriptor 2.0 */
	UAC_INPUT_TERTMINAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, 0x0201, USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE, USBD_UAC_IN_DEFAULT_CH_CNT)
	/* Audio Control Feature Unit Descriptor */
	UAC_ISOC_IN_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE)
	/* Audio Control Output Terminal Descriptor */
	UAC_OUTPUT_TERTMINAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, 0x101, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE)

	/*---------Headphones-----------*/
	/* Headphones Audio Control Input Terminal Descriptor 2.0 */
	UAC_INPUT_TERTMINAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, 0x0101, USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES, USBD_UAC_OUT_DEFAULT_CH_CNT)
	/* Audio Control Feature Unit Descriptor */
#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	UAC_ISOC_OUT_8CH_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES)
#elif USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6
	UAC_ISOC_OUT_6CH_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES)
#elif USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4
	UAC_ISOC_OUT_4CH_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES)
#else
	UAC_ISOC_OUT_2CH_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES)
#endif
	/* Audio Control Output Terminal Descriptor */
	UAC_OUTPUT_TERTMINAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_GET_OT_TYPE(USBD_UAC_OUT_DEFAULT_CH_CNT), USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)

	/* Audio Streaming IN *****************************************************************/
	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_MICROPHONE, 0x00, 0x00, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_MICROPHONE, 0x01, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, USBD_UAC_IN_DEFAULT_CH_CNT)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_IN_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_IN_EP, 0x05, USBD_UAC_HS_ISOC_IN_BINTERVAL, USBD_UAC_CALC_HS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH, USBD_UAC_IN_DEFAULT_SAMPLING_FREQ, USBD_UAC_HS_ISOC_IN_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()

	/* Audio Streaming OUT *****************************************************************/
	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x00, 0x00, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x01, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_CH_CNT_2)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_OUT_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_OUT_EP, 0x0D, USBD_UAC_HS_ISOC_OUT_BINTERVAL, USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_OUT_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_HS_ISOC_OUT_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()

#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x02, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_CH_CNT_4)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_OUT_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_OUT_EP, 0x0D, USBD_UAC_HS_ISOC_OUT_BINTERVAL, USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_OUT_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_HS_ISOC_OUT_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()
#endif

#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x03, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_CH_CNT_6)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_OUT_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_OUT_EP, 0x0D, USBD_UAC_HS_ISOC_OUT_BINTERVAL, USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_OUT_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_HS_ISOC_OUT_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()
#endif

#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/4*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x04, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_CH_CNT_8)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_OUT_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_OUT_EP, 0x0D, USBD_UAC_HS_ISOC_OUT_BINTERVAL, USBD_UAC_CALC_HS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_OUT_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_HS_ISOC_OUT_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()
#endif
};  /* usbd_composite_uac_hs_itf_desc */

static const u8 usbd_composite_uac_fs_itf_desc[] = {

	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	UAC_IAD_DESCRIPTOR(USBD_COMP_UAC_AC_HEADSET, USBD_UAC_AC_IF_NUM)

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AC_HEADSET, 0x00, 0x00, USB_UAC2_SUBCLASS_AUDIOCONTROL)

	/* 4.7.2 Class-Specific Audio Control Interface Header Descriptor */
	/* Audio headset */
	UAC_AC_HEADER_DESCRIPTOR()

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	UAC_CLK_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE)
	UAC_CLK_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)

	/*---------Microphone------------*/
	/* Microphone Audio Control Input Terminal Descriptor 2.0 */
	UAC_INPUT_TERTMINAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, 0x0201, USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE, USBD_UAC_IN_DEFAULT_CH_CNT)
	/* Audio Control Feature Unit Descriptor */
	UAC_ISOC_IN_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE)
	/* Audio Control Output Terminal Descriptor */
	UAC_OUTPUT_TERTMINAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, 0x101, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE)

	/*---------Headphones-----------*/
	/* Headphones Audio Control Input Terminal Descriptor 2.0 */
	UAC_INPUT_TERTMINAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, 0x0101, USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES, USBD_UAC_OUT_DEFAULT_CH_CNT)
	/* Audio Control Feature Unit Descriptor */
#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	UAC_ISOC_OUT_8CH_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES)
#elif USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6
	UAC_ISOC_OUT_6CH_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES)
#elif USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4
	UAC_ISOC_OUT_4CH_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES)
#else
	UAC_ISOC_OUT_2CH_FEATURE_UNIT_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES)
#endif
	/* Audio Control Output Terminal Descriptor */
	UAC_OUTPUT_TERTMINAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_GET_OT_TYPE(USBD_UAC_OUT_DEFAULT_CH_CNT), USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT, USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)

	/* Audio Streaming IN *****************************************************************/
	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_MICROPHONE, 0x00, 0x00, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_MICROPHONE, 0x01, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, USBD_UAC_IN_DEFAULT_CH_CNT)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_IN_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_IN_EP, 0x05, USBD_UAC_FS_ISOC_IN_BINTERVAL, USBD_UAC_CALC_FS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH, USBD_UAC_IN_DEFAULT_SAMPLING_FREQ, USBD_UAC_FS_ISOC_IN_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()

	/* Audio Streaming OUT *****************************************************************/
	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x00, 0x00, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x01, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_CH_CNT_2)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_OUT_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_OUT_EP, 0x0D, USBD_UAC_FS_ISOC_OUT_BINTERVAL, USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_OUT_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_FS_ISOC_OUT_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()

#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x02, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_CH_CNT_4)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_OUT_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_OUT_EP, 0x0D, USBD_UAC_FS_ISOC_OUT_BINTERVAL, USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_OUT_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_FS_ISOC_OUT_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()
#endif

#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x03, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_CH_CNT_6)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_OUT_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_OUT_EP, 0x0D, USBD_UAC_FS_ISOC_OUT_BINTERVAL, USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_OUT_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_FS_ISOC_OUT_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()
#endif

#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/4*/
	/* Interface1 Descriptor alt setting 0 streaming */
	UAC_INTERFACE_DESCRIPTOR(USBD_COMP_UAC_AS_HEADSET_HEADPHONES, 0x04, 0x01, USB_UAC2_SUBCLASS_AUDIOSTREAMING)

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	UAC_AS_GENERAL_DESCRIPTOR(USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, USBD_UAC_CH_CNT_8)

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	UAC_AS_FORMAT_DESCRIPTOR(USBD_UAC_OUT_DEFAULT_BYTE_WIDTH)

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO OUT Endpoint Descriptor */
	UAC_EP_DESCRIPTOR(USBD_COMP_UAC_ISOC_OUT_EP, 0x0D, USBD_UAC_FS_ISOC_OUT_BINTERVAL, USBD_UAC_CALC_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_OUT_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_FS_ISOC_OUT_BINTERVAL))

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	UAC_DATA_EP_DESCRIPTOR()
#endif
};  /* usbd_composite_uac_fs_config_desc */

static usbd_composite_uac_device_t usbd_composite_uac_device;

/* UAC volume data from windows10 PC*/
/* Different OS transmit the same vol level, but the corresponding driver vol values are different. */
static u8 usbd_composite_uac_pc_vol_lvl[] = {0, 5, 10, 20, 30, 40, 50, 60, 65, 75, 80, 85, 90, 95, 100};
static s16 usbd_composite_uac_drv_vol[] = {-190, -170, -151, -112, -74, -37, 0, 37, 56, 93, 112, 132, 151, 170, 190};

/* Exported variables --------------------------------------------------------*/
const usbd_class_driver_t usbd_composite_uac_driver = {
	.get_descriptor = usbd_composite_uac_get_descriptor,
	.set_config = usbd_composite_uac_set_config,
	.clear_config = usbd_composite_uac_clear_config,
	.setup = usbd_composite_uac_setup,
	.ep0_data_out = usbd_composite_uac_handle_ep0_data_out,
	.ep_data_in = usbd_composite_uac_handle_ep_data_in,
	.ep_data_out = usbd_composite_uac_handle_ep_data_out,
	.status_changed = usbd_composite_uac_status_changed,
	.sof = usbd_composite_uac_sof,
};

/* Private functions ---------------------------------------------------------*/
/*
	data cnt increase step
	sof will incraase one by one, but the data will increase by binterval
*/
static u32 usbd_composite_uac_get_binterval(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_dev_t *cdev = uac->cdev;

	if (cdev->dev->dev_speed == USB_SPEED_HIGH) {
		return USBD_UAC_POW2(USBD_UAC_HS_ISOC_OUT_BINTERVAL - 1);
	} else {
		return USBD_UAC_POW2(USBD_UAC_FS_ISOC_OUT_BINTERVAL - 1);
	}

	return 1;
}

/**
  * @retval
  */
static void usbd_composite_uac_append_data(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	usb_ringbuf_manager_t *buf_list = &(buf_ctrl->buf_list);

	if (usb_ringbuf_is_full(buf_list)) { //full, force to remove a node from the list
		u8 tmp[1];
		usb_ringbuf_remove_head(buf_list, tmp, 1, NULL);
#if USBD_COMPOSITE_UAC_DEBUG
		buf_ctrl->overwrite_cnt ++;
#endif
	}

	if (usb_ringbuf_is_full(buf_list)) {
		return;
	}

#if USBD_COMPOSITE_UAC_DEBUG
	buf_ctrl->append_zlp_cnt ++;
#endif
	usb_ringbuf_add_tail(buf_list, NULL, buf_ctrl->last_xfer_len, 0);

	//used for usb read
	//if (usb_ringbuf_get_count(buf_list) >= usbd_composite_uac_get_buf_cnt())
	if (usb_ringbuf_is_empty(buf_list) == 0) { // not empty
		usb_os_sema_give(buf_ctrl->isoc_sema);
	}
}

/**
  * @brief  Get the ring buffer count based on USB speed
  * @param  speed: USB connection speed
  * @retval Number of buffer segments for audio data
  */
static u16 usbd_composite_uac_get_ring_buf_cnt(u8 speed, u8 is_in)
{
	u8 interval;
	u8 count;
	if (speed == USB_SPEED_HIGH) { //high speed
		if (is_in) {
			interval = USBD_UAC_HS_ISOC_IN_BINTERVAL;
			count = USBD_UAC_TX_BUF_MAX_CNT;
		} else {
			interval = USBD_UAC_HS_ISOC_OUT_BINTERVAL;
			count = USBD_UAC_RX_BUF_MAX_CNT;
		}

		return 8 * count / USBD_UAC_POW2(interval - 1);
	}

	if (is_in) {
		interval = USBD_UAC_FS_ISOC_IN_BINTERVAL;
		count = USBD_UAC_TX_BUF_MAX_CNT;
	} else {
		interval = USBD_UAC_FS_ISOC_OUT_BINTERVAL;
		count = USBD_UAC_RX_BUF_MAX_CNT;
	}

	return count / USBD_UAC_POW2(interval - 1);
}

/**
  * @brief  Get the MPS based on configuration parameters and speed
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Calculated MPS
  */
static u16 usbd_composite_uac_get_mps(usbd_audio_cfg_t *params, u8 speed, u8 is_in)
{
	u16 mps_value;
	u8 interval;
	if (NULL == params) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Param err\n");
		return 0;
	}

	if (speed == USB_SPEED_HIGH) {
		if (is_in) {
			interval = USBD_UAC_HS_ISOC_IN_BINTERVAL;
		} else {
			interval = USBD_UAC_HS_ISOC_OUT_BINTERVAL;
		}

		mps_value = USBD_UAC_CALC_HS_MPS(params->ch_cnt, params->byte_width, params->sampling_freq, interval);
		if ((mps_value == 0) || (mps_value > USBD_UAC_HS_ISOC_MPS)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS %d zero or exceed HS limited %d\n", mps_value, USBD_UAC_HS_ISOC_MPS);
			return 0;
		}
	} else {
		if (is_in) {
			interval = USBD_UAC_FS_ISOC_IN_BINTERVAL;
		} else {
			interval = USBD_UAC_FS_ISOC_OUT_BINTERVAL;
		}
		/* for 44.1khz or the host clk is bigger than the device */
		mps_value = USBD_UAC_CALC_FS_MPS(params->ch_cnt, params->byte_width, params->sampling_freq, interval);
		if ((mps_value == 0) || (mps_value > USBD_UAC_FS_ISOC_MPS)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS %d : zero or exceed FS limited %d\n", mps_value, USBD_UAC_FS_ISOC_MPS);
			return 0;
		}
	}

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Ch %d bw %d freq %d speed %d mps %d in %d\n", params->ch_cnt, params->byte_width, params->sampling_freq, speed, mps_value, is_in);
	return mps_value;
}

/**
  * @brief  Get the channel configuration based on UAC channel cnt
  * @param  ch_cnt: UAC channel cnt
  * @retval Bitmask representing the channel configuration
  */
static u8 usbd_composite_uac_get_ch_config(u8 ch_cnt)
{

	switch (ch_cnt) {
	case 2:
		return 0x03;
	case 4:
		return 0x0F;
	case 6:
		return 0x3F;
	case 8:
		return 0xFF;
	default:
		return 0x03;
	}

	return 0x03;
}

/**
  * @brief  Update the ep MPS
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_composite_uac_ep_update_mps(usbd_composite_uac_buf_ctrl_t *buf_ctrl, usbd_audio_cfg_t *params, u8 speed, u8 is_in)
{
	u16 ep_mps;
	// if (usbd_composite_uac_ep_enable(params))
	{
		ep_mps = usbd_composite_uac_get_mps(params, speed, is_in);
		if (ep_mps == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS check fail\n");
			return HAL_ERR_PARA;
		}

		buf_ctrl->mps = ep_mps;
	}

	return HAL_OK;
}

/**
  * @brief  Get USB audio endpoint status
  * @param  ep: PUSB audio endpoint config
  * @retval Status
  */
static inline u8 usbd_composite_uac_ep_enable(usbd_audio_cfg_t *ep)
{
	if (ep) {
		return ep->enable;
	}

	return  0;
}

/**
  * @brief  Handle UAC clock validity request
  * @param  dev: USB device instance
  * @param  flag: Clock validity flag
  * @retval void
  */
static void usbd_composite_uac_clk_valid_req(usb_dev_t *dev, u8 flag)
{
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u8 valid = flag;

	usb_os_memcpy(ep0_in->xfer_buf, &valid, sizeof(valid));
	ep0_in->xfer_len = sizeof(valid);
	usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle UAC current sampling_freq request
  * @param  dev: USB device instance
  * @param  sampling_freq: UAC current sampling_freq
  * @retval void
  */
static void usbd_composite_uac_cur_sampling_freq_req(usb_dev_t *dev, u32 sampling_freq)
{
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u32 dCur = sampling_freq;

	usb_os_memcpy(ep0_in->xfer_buf, &dCur, sizeof(dCur));
	ep0_in->xfer_len = sizeof(dCur);
	usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle sampling_freq ctrl requests
  * @param  dev: USB device instance
  * @param  max_len: Maximum allowed length for response
  * @retval Status
  */
static int usbd_composite_uac_sampling_freq_ctrl_range_req(usb_dev_t *dev, u16 max_len, u8 is_in)
{
	usb_uac2_freq_t sampling_freq_range;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u16 num_sub_ranges;
	u16 len;
	u16 data_offset = 0;

	if (dev->dev_speed  == USB_SPEED_HIGH) {
		num_sub_ranges = USBD_UAC_HS_SAMPLING_FREQ_COUNT;
	} else {
		num_sub_ranges = USBD_UAC_FS_SAMPLING_FREQ_COUNT;
	}

	if (is_in) {
		num_sub_ranges = 1;
		len = sizeof(num_sub_ranges) + num_sub_ranges * sizeof(usb_uac2_freq_t);

		usb_os_memcpy(ep0_in->xfer_buf, (u8 *)&num_sub_ranges, 2);
		data_offset += 2;

		USBD_UAC_INIT_SUB_RANGE(sampling_freq_range, USBD_UAC_IN_DEFAULT_SAMPLING_FREQ, USBD_UAC_IN_DEFAULT_SAMPLING_FREQ, 0);
		usb_os_memcpy(ep0_in->xfer_buf + data_offset, &sampling_freq_range, sizeof(sampling_freq_range));
		ep0_in->xfer_len = len;
	} else {
		len = sizeof(num_sub_ranges) + num_sub_ranges * sizeof(usb_uac2_freq_t);

		usb_os_memcpy(ep0_in->xfer_buf, (u8 *)&num_sub_ranges, 2);
		data_offset += 2;

		for (u8 i = 0; i < num_sub_ranges; i++) {
			USBD_UAC_INIT_SUB_RANGE(sampling_freq_range, usbd_composite_uac_isoc_out_freq[i], usbd_composite_uac_isoc_out_freq[i], 0);
			usb_os_memcpy(ep0_in->xfer_buf + data_offset, &sampling_freq_range, sizeof(sampling_freq_range));
			data_offset += sizeof(sampling_freq_range);
		}

		if (len > max_len) {
			len = max_len;
		}

		ep0_in->xfer_len = len;
	}

	usbd_ep_transmit(dev, ep0_in);

	return HAL_OK;
}

/**
  * @brief  Handle connect ctrl requests
  * @param  dev: USB device instance
  * @param  ch_num: USB channel num
  * @param  ch_cfg: USB channel configuration
  * @param  max_len: Maximum allowed length for response
  * @retval Status
  */
static void usbd_composite_uac_connect_ctrl_req(usb_dev_t *dev, u8 ch_num, u32 ch_cfg, u16 max_len)
{
	usb_uac2_ac_connect_ctrl_t response = {0, 0, 0};
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u16 len = sizeof(response);

	response.bNrChannels = ch_num;
	response.bmChannelConfig = ch_cfg;

	if (len > max_len) {
		len = max_len;
	}

	usb_os_memcpy(ep0_in->xfer_buf, &response, len);
	ep0_in->xfer_len = len;
	usbd_ep_transmit(dev, ep0_in);
}


/**
  * @brief  Set UAC class configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_uac_set_config(usb_dev_t *dev, u8 config)
{
	UNUSED(dev);
	UNUSED(config);
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_dev_t *cdev = uac->cdev;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl;
	usb_speed_type_t speed = cdev->dev->dev_speed;
	usbd_ep_t *ep;

	uac->alt_setting = 0U;

	/* Init ISOC OUT EP */
	buf_ctrl = &(uac->isoc_out);
	ep = &(buf_ctrl->ep);
	ep->binterval = (speed == USB_SPEED_HIGH) ? USBD_UAC_HS_ISOC_OUT_BINTERVAL : USBD_UAC_FS_ISOC_OUT_BINTERVAL;
	usbd_composite_uac_ep_update_mps(buf_ctrl, (usbd_audio_cfg_t *) & (buf_ctrl->audio_config), cdev->dev->dev_speed, 0);

	ep->mps = buf_ctrl->mps;
	usbd_ep_init(cdev->dev, ep);
	ep->xfer_buf = usbd_composite_uac_rx_buf;
	ep->xfer_len = buf_ctrl->mps;
	usbd_ep_receive(cdev->dev, ep);

	/* Init ISOC IN EP */
	buf_ctrl = &(uac->isoc_in);
	ep = &(buf_ctrl->ep);
	ep->binterval = ((speed == USB_SPEED_HIGH) ? USBD_UAC_HS_ISOC_IN_BINTERVAL : USBD_UAC_FS_ISOC_IN_BINTERVAL);
	usbd_composite_uac_ep_update_mps(buf_ctrl, (usbd_audio_cfg_t *) & (buf_ctrl->audio_config), cdev->dev->dev_speed, 1);
	ep->mps = buf_ctrl->mps;
	usbd_ep_init(cdev->dev, ep);

	if (uac->cb->set_config != NULL) {
		uac->cb->set_config();
	}

	return HAL_OK;
}

/**
  * @brief  Clear UAC configuration
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_composite_uac_clear_config(usb_dev_t *dev, u8 config)
{
	int ret = 0U;
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_ep_t *ep_out = &(uac->isoc_out.ep);
	usbd_ep_t *ep_in = &(uac->isoc_in.ep);

	UNUSED(config);

	/* DeInit ISOC IN EP */
	if (usbd_composite_uac_ep_enable(&(uac->isoc_in.audio_config))) {
		usbd_ep_deinit(dev, ep_in);
	}

	/* DeInit ISOC OUT EP */
	if (usbd_composite_uac_ep_enable(&(uac->isoc_out.audio_config))) {
		usbd_ep_deinit(dev, ep_out);
	}

	return ret;
}

/**
  * @brief  Check sampling_freq requests
  * @param  sampling_freq: Freq requests from host
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_composite_uac_is_valid_sample_rate(u32 sampling_freq, u8 speed)
{
	u8 count;

	if (speed == USB_SPEED_HIGH) {
		count = USBD_UAC_HS_SAMPLING_FREQ_COUNT;
	} else {
		count = USBD_UAC_FS_SAMPLING_FREQ_COUNT;
	}

	for (u8 i = 0; i < count; ++i) {
		if (sampling_freq == usbd_composite_uac_isoc_out_freq[i]) {
			return HAL_OK;
		}
	}

	return HAL_ERR_PARA;
}

/**
  * @brief  Calculate the PC's intended volume level based on the UAC driver value
  * @param  x_points: Array of volume levels on the Windows PC
  * @param  y_points: Corresponding array of volume levels used by the UAC driver
  * @param  num_points: Number of data points (at least 2)
  * @param  target_y: The volume level from the UAC driver to be interpolated
  * @retval The intended volume level on the Windows PC corresponding to the UAC driver value
  */
static u8 usbd_composite_uac_volume_linear_interpolation(u8 x_points[], s16 y_points[], u8 num_points, s16 target_y)
{
	u8 i;
	u8 x0;
	u8 x1;
	s16 y0;
	s16 y1;
	s32 numerator;
	s32 denominator;
	s32 x;

	if (target_y > 190 || target_y < -190) {
		return UABD_UAC_VOL_ERR_VAL;
	}

	for (i = 0; i < num_points - 1; i++) {
		y0 = y_points[i];
		y1 = y_points[i + 1];
		x0 = x_points[i];
		x1 = x_points[i + 1];

		if ((y0 <= target_y && target_y <= y1) || (y1 <= target_y && target_y <= y0)) {
			numerator = (s32)(target_y - y0) * (x1 - x0);
			denominator = (y1 - y0);
			x = (numerator + denominator / 2) / denominator + x0;
			if (x < 0) {
				x = 0;
			}
			if (x > 100) {
				x = 100;
			}
			return (u8)x;
		}
	}

	return UABD_UAC_VOL_ERR_VAL;
}

/**
  * @brief  Iso out test
  * @param  host: Host handle
  * @retval None
*/
static void usbd_composite_uac_isoc_in_process_xfer(usb_dev_t *dev)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl;
	usb_ringbuf_manager_t *buf_list;
	usbd_ep_t *ep;
	u32 read_len;
	u8 zlp = 0;

	buf_ctrl = &(uac->isoc_in);
	buf_list = &(buf_ctrl->buf_list);
	ep = &(buf_ctrl->ep);

	if (!usb_ringbuf_is_empty(buf_list)) {
		// check valid data
		read_len = usb_ringbuf_remove_head(buf_list, usbd_composite_uac_tx_buf, USBD_COMPOSITE_ISOC_BUF_LENGTH, &zlp);
		usb_os_sema_give(buf_ctrl->isoc_sema);
		if (read_len > 0) {
			ep->is_busy = 1U;
			ep->xfer_state = 1U;
			ep->xfer_buf = usbd_composite_uac_tx_buf;
			ep->xfer_len = read_len;

			usbd_ep_transmit(dev, ep);
		} else {
			RTK_LOGS(TAG, RTK_LOG_DEBUG, "Read err--------------------\n");
		}
	}
}

static void usbd_composite_uac_isoc_in_process_complete(usb_dev_t *dev)
{
	UNUSED(dev);
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_ep_t *ep;

	ep = &(uac->isoc_in.ep);

	ep->xfer_state = 0;
	ep->is_busy = 0;
}

/**
  * @brief  Handle UAC specific CTRL requests
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_composite_uac_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_usr_cb_t *cb = uac->cb;
	usbd_audio_cfg_t *audio_cfg = NULL;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	u8 entityId, controlSelector, ch_cnt, byte_width, alt_setting, fmt_change = 0;
	int ret = HAL_OK;

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "SETUP: bmRequestType=0x%02x bRequest=0x%02x wLength=0x%04x wValue=%x wIndex=%x\n",
	// 		 req->bmRequestType, req->bRequest, req->wLength, req->wValue,req->wIndex);

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				alt_setting = USB_LOW_BYTE(req->wValue);
				if ((alt_setting != uac->alt_setting) && alt_setting) {
					audio_cfg = &(uac->isoc_out.audio_config);
					fmt_change = 0;
					uac->alt_setting = alt_setting;
					RTK_LOGS(TAG, RTK_LOG_DEBUG, "Set new alt %d old %d-%d-%d\n", uac->alt_setting, audio_cfg->byte_width, audio_cfg->ch_cnt, audio_cfg->sampling_freq);
					switch (uac->alt_setting) {
					case 1:
						byte_width = 2;
						ch_cnt = 2;
						break;
#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
					case 2:
						byte_width = 2;
						ch_cnt = 4;
						break;
#endif
#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
					case 3:
						byte_width = 2;
						ch_cnt = 6;
						break;
#endif
#if USBD_UAC_OUT_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
					case 4:
						byte_width = 2;
						ch_cnt = 8;
						break;
#endif
					default:
						ret = HAL_ERR_PARA;
						break;
					}

					if (ret == HAL_OK) {
						if ((audio_cfg->byte_width != byte_width) && byte_width) {
							audio_cfg->byte_width = byte_width;
							fmt_change = 1;
							// RTK_LOGS(TAG, RTK_LOG_DEBUG, "set byte_width %d\n",byte_width);
						}
						if ((audio_cfg->ch_cnt != ch_cnt) && ch_cnt) {
							audio_cfg->ch_cnt = ch_cnt;
							fmt_change = 1;
							// RTK_LOGS(TAG, RTK_LOG_DEBUG, "set ch_cnt %d\n",ch_cnt);
						}
						//update the issue
						RTK_LOGS(TAG, RTK_LOG_INFO, "fmt_change flag %d\n", fmt_change);
						if (fmt_change) {
							if (cb->format_changed != NULL) {
								cb->format_changed(audio_cfg->sampling_freq, audio_cfg->ch_cnt, audio_cfg->byte_width);
							}
						}
					}
				}
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				if (USB_LOW_BYTE(req->wIndex) == USBD_COMP_UAC_AS_HEADSET_HEADPHONES) {
					ep0_in->xfer_buf[0] = uac->alt_setting;
					ep0_in->xfer_len = 1U;
					usbd_ep_transmit(dev, ep0_in);
				} else {
					ep0_in->xfer_buf[0] = 0U;
					ep0_in->xfer_len = 1U;
					usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		case USB_REQ_GET_STATUS:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = HAL_ERR_PARA;
			}
			break;

		default:
			ret = HAL_ERR_PARA;
			break;
		}
		break;/* case USB_REQ_TYPE_STANDARD */

	case USB_REQ_TYPE_CLASS :
		entityId = USB_HIGH_BYTE(req->wIndex);
		controlSelector = USB_HIGH_BYTE(req->wValue);

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			// RTK_LOGS(TAG, RTK_LOG_DEBUG, "D2H UAC SETUP: id=0x%02x cs=%d\n", entityId, controlSelector);
			switch (entityId) {
			/*--------------------------------------------------HEADPHONES---------------------------*/
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USB_UAC2_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						audio_cfg = &(uac->isoc_out.audio_config);
						usbd_composite_uac_cur_sampling_freq_req(dev, audio_cfg->sampling_freq);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {
						usbd_composite_uac_sampling_freq_ctrl_range_req(dev, req->wLength, 0);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC2_CS_CLK_VALID_CONTROL) {
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						usbd_composite_uac_clk_valid_req(dev, uac->cur_clk_valid);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES: //out
				if (controlSelector == USB_UAC2_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						ch_cnt = uac->isoc_out.audio_config.ch_cnt;
						usbd_composite_uac_connect_ctrl_req(dev, ch_cnt, usbd_composite_uac_get_ch_config(ch_cnt), req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT:
				if (controlSelector == USB_UAC2_CTRL_FU_MUTE_CONTROL_SELECTOR) {
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						ep0_in->xfer_buf[0] = uac->cur_mute;
						ep0_in->xfer_len = 1U;
						usbd_ep_transmit(dev, ep0_in);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC2_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume ctrl  //02
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {  //01  cur volume
						usb_os_memcpy(ep0_in->xfer_buf, (u8 *) & (uac->cur_volume), 2);
						ep0_in->xfer_len = 2U;
						usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {  //02 volume ctrl range

						usb_uac2_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = USBD_UAC_VOLUME_CTRL_MIN;
						response.wMAX = USBD_UAC_VOLUME_CTRL_MAX;
						response.wRES = 1;

						usb_os_memcpy(ep0_in->xfer_buf, &response, sizeof(response));
						ep0_in->xfer_len = sizeof(response);
						usbd_ep_transmit(dev, ep0_in);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: 0x13 wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			/*--------------------------------------------------MICROPHONE---------------------------*/
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
				if (controlSelector == USB_UAC2_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						audio_cfg = &(uac->isoc_in.audio_config);
						usbd_composite_uac_cur_sampling_freq_req(dev, audio_cfg->sampling_freq);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {
						usbd_composite_uac_sampling_freq_ctrl_range_req(dev, req->wLength, 1);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE: //in
				if (controlSelector == USB_UAC2_TE_CONNECTOR_CONTROL) {
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						usbd_composite_uac_connect_ctrl_req(dev, 1, 0x1, req->wLength);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT:
				if (controlSelector == USB_UAC2_CTRL_FU_MUTE_CONTROL_SELECTOR) {
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						ep0_in->xfer_buf[0] = uac->cur_mute;
						ep0_in->xfer_len = 1U;
						usbd_ep_transmit(dev, ep0_in);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC2_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume ctrl  //02
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {  //01  cur volume
						usb_os_memcpy(ep0_in->xfer_buf, (u8 *) & (uac->cur_volume), 2);
						ep0_in->xfer_len = 2U;
						usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {  //02 volume ctrl range

						usb_uac2_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = USBD_UAC_VOLUME_CTRL_MIN;
						response.wMAX = USBD_UAC_VOLUME_CTRL_MAX;
						response.wRES = 1;

						usb_os_memcpy(ep0_in->xfer_buf, &response, sizeof(response));
						ep0_in->xfer_len = sizeof(response);
						usbd_ep_transmit(dev, ep0_in);
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bRequest err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: 0x13 wValue err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;

			default:
				RTK_LOGS(TAG, RTK_LOG_ERROR, "D2H SETUP: wIndex err %d\n", entityId);
				break;
			}
		} else {
			/* USB_H2D */
			// RTK_LOGS(TAG, RTK_LOG_DEBUG, "H2D UAC SETUP: id=0x%02x cs=%d\n", entityId, controlSelector);
			switch (entityId) {
			/*--------------------------------------------------HEADPHONES---------------------------*/
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USB_UAC2_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {
						// Do nothing
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set sampling_freq err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Set sampling_freq ctrl err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES */

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: //0x05 FU
				if (controlSelector == USB_UAC2_CTRL_FU_MUTE_CONTROL_SELECTOR) { //mute
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {
						// Do nothing
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set cur mute err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC2_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set cur volume range err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Set fu err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT */

			/*--------------------------------------------------MICROPHONE---------------------------*/
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
				if (controlSelector == USB_UAC2_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {
						// Do nothing
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set sampling_freq err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Set sampling_freq ctrl err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_PARA;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE */

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT: //0x08 FU
				if (controlSelector == USB_UAC2_CTRL_FU_MUTE_CONTROL_SELECTOR) { //mute
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {
						// Do nothing
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set cur mute err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_PARA;
					}
				} else if (controlSelector == USB_UAC2_CTRL_FU_VOLUME_CONTROL_SELECTOR) { //volume
					if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR) {
						usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
						ep0_out->xfer_len = req->wLength;
						usbd_ep_receive(dev, ep0_out);
					} else if (req->bRequest == USB_UAC2_CLASS_REQ_CODE_RANGE) {
					} else {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Set cur volume range err %d-%d\n", entityId, req->bRequest);
						ret = HAL_ERR_HW;
					}
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Set fu err %d-%d\n", entityId, controlSelector);
					ret = HAL_ERR_HW;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT */
			default:
				RTK_LOGS(TAG, RTK_LOG_ERROR, "H2D SETUP: wIndex err %d\n", entityId);
				break;
			}
		}
		break;/* case USB_REQ_TYPE_CLASS */

	case USB_REQ_TYPE_VENDOR:
		if (req->wLength) {
			if ((req->bmRequestType & 0x80U) && (uac->cb->setup != NULL)) {
				ret = uac->cb->setup(req, ep0_in->xfer_buf);
				if (ret == HAL_OK) {
					ep0_in->xfer_len = req->wLength;
					usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				usb_os_memcpy((void *)&uac->ctrl_req, (void *)req, sizeof(usb_setup_req_t));
				ep0_out->xfer_len = req->wLength;
				usbd_ep_receive(dev, ep0_out);
			}
		} else {
			uac->cb->setup(req, NULL);
		}
		break;

	default:
		RTK_LOGS(TAG, RTK_LOG_ERROR, "SETUP: bmRequestType err\n");
		ret = HAL_ERR_HW;
		break;
	}

	return ret;
}

/**
  * @brief  uac_handle_ep0_data_out
  *         Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_composite_uac_handle_ep0_data_out(usb_dev_t *dev)
{
	UNUSED(dev);
	int ret = HAL_OK;
	u8 num_points;
	u8 target_volume;
	s16 volume_value = 0;
	u32 sampling_freq;
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_usr_cb_t *cb = uac->cb;
	usb_setup_req_t *p_ctrl_req = &(uac->ctrl_req);
	usbd_audio_cfg_t *audio_cfg = NULL;
	usbd_ep_t *ep0_out = &dev->ep0_out;

	RTK_LOGS(TAG, RTK_LOG_INFO, "EP0 Out: bmRequestType=0x%02x bRequest=0x%02x wValue=%x wIndex=%x wLength=0x%04x\n",
			 p_ctrl_req->bmRequestType, p_ctrl_req->bRequest, p_ctrl_req->wValue, p_ctrl_req->wIndex, p_ctrl_req->wLength);
	if ((((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && ((p_ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_INTERFACE))
		&& (p_ctrl_req->bRequest == USB_UAC2_CLASS_REQ_CODE_CUR)) {

		if (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) {
			if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC2_CTRL_FU_MUTE_CONTROL_SELECTOR) && (p_ctrl_req->wLength == 0x01)) {
				if (cb->mute_changed != NULL) {
					uac->cur_mute = (ep0_out->xfer_buf[0]) ? 1 : 0;
					cb->mute_changed(uac->cur_mute);
				}
			} else if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC2_CTRL_FU_VOLUME_CONTROL_SELECTOR) && (p_ctrl_req->wLength == 0x02)) {
				if (cb->volume_changed != NULL) {
					volume_value = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);
					uac->cur_volume = volume_value;

					//RTK_LOGS(TAG, RTK_LOG_DEBUG, "Set volume 0x%02x\n", volume_value);

					num_points = sizeof(usbd_composite_uac_pc_vol_lvl) / sizeof(usbd_composite_uac_pc_vol_lvl[0]);
					if (num_points != sizeof(usbd_composite_uac_drv_vol) / sizeof(usbd_composite_uac_drv_vol[0])) {
						RTK_LOGS(TAG, RTK_LOG_ERROR, "Volume arrays length err.\n");
						ret = HAL_ERR_PARA;
					} else {
						target_volume = usbd_composite_uac_volume_linear_interpolation(usbd_composite_uac_pc_vol_lvl, \
										usbd_composite_uac_drv_vol, num_points, (int)volume_value);
					}

					if (target_volume != UABD_UAC_VOL_ERR_VAL) {
						cb->volume_changed(target_volume);
					}
				}
			}
		}

		if ((USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC2_CS_SAM_FREQ_CONTROL) && (p_ctrl_req->wLength == 0x04)) {
			sampling_freq = (ep0_out->xfer_buf[3] << 24) | (ep0_out->xfer_buf[2] << 16) | (ep0_out->xfer_buf[1] << 8) | ep0_out->xfer_buf[0];

			if (usbd_composite_uac_is_valid_sample_rate(sampling_freq, dev->dev_speed) == HAL_OK) {
				audio_cfg = &(uac->isoc_out.audio_config);
				if (audio_cfg->sampling_freq != sampling_freq) {
					audio_cfg->sampling_freq = sampling_freq;

					if (cb->format_changed != NULL) {
						cb->format_changed(audio_cfg->sampling_freq, audio_cfg->ch_cnt, audio_cfg->byte_width);
					}
				}
			} else {
				// Do nothing, return HAL_ERR_PARA(STALL) will cause PC continue setting invalid sampling_freq request
			}
		}
		// To do: handle vendor
	}

	return ret;
}

static int usbd_composite_uac_sof(usb_dev_t *dev)
{
	UNUSED(dev);
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl;
	usbd_ep_t *ep;

	/* out */
	buf_ctrl = &(uac->isoc_out);
	if ((buf_ctrl->xfer_continue) && (buf_ctrl->xfer_cnt > 0)) {
		if (buf_ctrl->sof_idx == 0) {
			buf_ctrl->sof_idx = 1;
			buf_ctrl->data_idx = 0;
		} else {
			buf_ctrl->sof_idx ++;
		}

		if ((buf_ctrl->sof_idx > buf_ctrl->data_idx) && (buf_ctrl->sof_idx - buf_ctrl->data_idx >= USBD_UAC_ISOC_RX_APPEND_ZLP_DIFF_MAX)) {
			usbd_composite_uac_append_data();
			buf_ctrl->data_idx += usbd_composite_uac_get_binterval();
		}
	}

	//in
	buf_ctrl = &(uac->isoc_in);
	ep = &(buf_ctrl->ep);
	if ((buf_ctrl->xfer_continue) && (ep->xfer_state == 0)) {
		// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Data in %d %d\n",buf_ctrl->xfer_continue, buf_ctrl->mps);

		/*
			if cur_frame - last frame_num  >= interval, means we should trigger a xfer asap
			if xfer_state = idle, it means that last xfer has been done, so in sof intr, we should check whether the next frame will be the xfer frame
		*/
		usbd_composite_uac_isoc_in_process_xfer(dev);
	}

	return HAL_OK;
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_composite_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, u8 status)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl;
	usbd_ep_t *ep;
	UNUSED(dev);
	UNUSED(ep_addr);
	UNUSED(status);

	// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Read data in %d %d\n",ep_addr, status);

	buf_ctrl = &(uac->isoc_in);
	ep = &(buf_ctrl->ep);

	if ((buf_ctrl->xfer_continue == 1) && (ep_addr == ep->addr)) {
		usbd_composite_uac_isoc_in_process_complete(dev);

		if (!usb_ringbuf_is_empty(&(buf_ctrl->buf_list))) {
			usbd_composite_uac_isoc_in_process_xfer(dev);
		}
	} else {
		// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Read data in %d %d ---- wait issue\n",ep_addr, status);
	}

	return HAL_OK;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_composite_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	usb_ringbuf_manager_t *buf_list = &(buf_ctrl->buf_list);
	usbd_composite_dev_t *cdev = uac->cdev;
	usbd_ep_t *ep_out = &(buf_ctrl->ep);
	UNUSED(dev);
	int ret = HAL_OK;

	// RTK_LOGS(TAG, RTK_LOG_INFO, "Read data out %d/len %d\n",buf_ctrl->xfer_continue, len);
#if USBD_COMPOSITE_UAC_DEBUG
	buf_ctrl->xfer_done_cnt ++;
#endif

	if (buf_ctrl->xfer_continue) {
		if (ep_addr == USBD_COMP_UAC_ISOC_OUT_EP) {
			if (len == 0) { //ZLP
				ep_out->xfer_buf = usbd_composite_uac_rx_buf;
				ep_out->xfer_len = buf_ctrl->mps;
				usbd_ep_receive(cdev->dev, ep_out);
			} else {
				buf_ctrl->last_xfer_len = len;
				buf_ctrl->xfer_cnt ++;
#if USBD_COMPOSITE_UAC_DEBUG
				u32 g_rx_new_tick = usb_os_get_timestamp_us(); //us
				if ((buf_ctrl->last_xfer_tick > 0) && (g_rx_new_tick - buf_ctrl->last_xfer_tick > buf_ctrl->isoc_timeout_max_step)) {
					buf_ctrl->timeout_cnt ++;
					buf_ctrl->isoc_timeout_max_value = g_rx_new_tick - buf_ctrl->last_xfer_tick;
				}
				buf_ctrl->last_xfer_tick = g_rx_new_tick;
#endif
				buf_ctrl->data_idx += usbd_composite_uac_get_binterval();

				/*
					if no read, the function will overwrite the data, and the read_pos should be updated
				*/
				if (usb_ringbuf_is_full(buf_list)) { //full, force to remove a node from the list
					u8 tmp[1];
					usb_ringbuf_remove_head(buf_list, tmp, 1, NULL);
#if USBD_COMPOSITE_UAC_DEBUG
					buf_ctrl->overwrite_cnt ++;
#endif
				}

				usb_ringbuf_add_tail(buf_list, usbd_composite_uac_rx_buf, len, 1);

				ep_out->xfer_buf = usbd_composite_uac_rx_buf;
				ep_out->xfer_len = buf_ctrl->mps;
				usbd_ep_receive(cdev->dev, ep_out);

				//used for usb read
				//if (usb_ringbuf_get_count(buf_list) >= usbd_composite_uac_get_buf_cnt())
				if (usb_ringbuf_is_empty(buf_list) == 0) { //not empty
					usb_os_sema_give(buf_ctrl->isoc_sema);
				}
			}
		}
	} else {
		ep_out->xfer_buf = usbd_composite_uac_rx_buf;
		ep_out->xfer_len = buf_ctrl->mps;
		usbd_ep_receive(cdev->dev, ep_out);
	}

	return ret;
}

/**
  * @brief  Get descriptor callback
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 usbd_composite_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf)
{
	usb_speed_type_t speed = dev->dev_speed;
	u8 *desc = NULL;
	u16 len = 0;
	dev->self_powered = USBD_UAC_SELF_POWERED;

#if USBD_COMPOSITE_UAC_DEBUG
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	if (speed == USB_SPEED_HIGH) { //hs
		buf_ctrl->isoc_timeout_max_step = 185 * USBD_UAC_POW2(USBD_UAC_HS_ISOC_OUT_BINTERVAL - 1);
	} else {
		buf_ctrl->isoc_timeout_max_step = 1500 * USBD_UAC_POW2(USBD_UAC_FS_ISOC_OUT_BINTERVAL - 1);
	}
	buf_ctrl = &(uac->isoc_in);
	if (speed == USB_SPEED_HIGH) { //hs
		buf_ctrl->isoc_timeout_max_step = 185 * USBD_UAC_POW2(USBD_UAC_HS_ISOC_IN_BINTERVAL - 1);
	} else {
		buf_ctrl->isoc_timeout_max_step = 1500 * USBD_UAC_POW2(USBD_UAC_FS_ISOC_IN_BINTERVAL - 1);
	}
#endif

	switch (USB_HIGH_BYTE(req->wValue)) {
	case USB_DESC_TYPE_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_composite_uac_hs_itf_desc;
			len = sizeof(usbd_composite_uac_hs_itf_desc);
		} else {
			desc = (u8 *)usbd_composite_uac_fs_itf_desc;
			len = sizeof(usbd_composite_uac_fs_itf_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			desc = (u8 *)usbd_composite_uac_fs_itf_desc;
			len = sizeof(usbd_composite_uac_fs_itf_desc);
		} else {
			desc = (u8 *)usbd_composite_uac_hs_itf_desc;
			len = sizeof(usbd_composite_uac_hs_itf_desc);
		}
		usb_os_memcpy((void *)buf, (void *)desc, len);
		break;

	default:
		break;
	}

	return len;
}

/**
  * @brief  USB attach status change
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_composite_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;

	UNUSED(dev);

	if (uac->cb->status_changed) {
		uac->cb->status_changed(old_status, status);
	}
}

/**
  * @brief  Read data from a USB audio ring buffer
  * @param  buf_ctrl: Pointer to the buffer control structure
  * @param  buffer: Pointer to Data buffer
  * @param  size: Data length
  * @param  copy_len: Copy length
  * @param  pkt_cnt: copy packet count
  * @param  zero_pkt_flag: zero packet ,each bit means a zero packet
  * @retval Status 0 means get enought data, 1 means should continue to read the pkt
  */
static u32 usbd_composite_uac_read_ring_buf(usbd_composite_uac_buf_ctrl_t *buf_ctrl, u8 *buffer, u32 size, u32 *copy_len, u16 *pkt_cnt, u32 *zero_pkt_flag)
{
	usb_ringbuf_manager_t *buf_list = &(buf_ctrl->buf_list);
	u32 read_len;
	u8 valid = 0;

	do {
		/* should exit : 1) Enough data has been obtained; 2) the next data cannot be saved completely */
		if ((*copy_len >= size) || (*copy_len + buf_ctrl->mps > size)) {
			return 0;
		}

		read_len = usb_ringbuf_remove_head(buf_list, buffer + *copy_len, (size - *copy_len), &valid);
		if (read_len > 0) {
			*copy_len += read_len;
			if ((valid == 0) && zero_pkt_flag) {
				*zero_pkt_flag |= 1 << *pkt_cnt;
			}

			*pkt_cnt = *pkt_cnt + 1;
		} else {
			// RTK_LOGS(TAG, RTK_LOG_ERROR, "Buflen is zero\n");
		}
	} while (usb_ringbuf_is_empty(&(buf_ctrl->buf_list)) != 0);

	/* should return 0 : enough data has been obtained; */
	if (*copy_len >= size) {
		return 0;
	}

	return 1;
}


/**
  * @brief  Deinitialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @retval void
  */
static void usbd_composite_uac_ep_buf_ctrl_deinit(usbd_composite_uac_buf_ctrl_t *buf_ctrl)
{
	buf_ctrl->mps = 0;
	buf_ctrl->xfer_continue = 0;

	if (buf_ctrl->sema_valid) {
		buf_ctrl->sema_valid = 0;

		if (buf_ctrl->wait_sema) {
			usb_os_sema_give(buf_ctrl->isoc_sema);
		}
		do {
			usb_os_delay_us(100U);
		} while (buf_ctrl->wait_sema);

		usb_os_sema_delete(buf_ctrl->isoc_sema);
	}
	usb_ringbuf_manager_deinit(&(buf_ctrl->buf_list));
}

/**
  * @brief  Initialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  params: Pointer to audio configuration parameters structure
  * @retval Status
  */
static int usbd_composite_uac_ep_buf_ctrl_init(usbd_composite_uac_buf_ctrl_t *buf_ctrl, usbd_audio_cfg_t *params, u8 is_in)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_dev_t *cdev = uac->cdev;
	u8 speed = cdev->dev->dev_speed;
	u16 buf_list_cnt;

	if (usbd_composite_uac_ep_enable(params)) {
		buf_ctrl->mps = usbd_composite_uac_get_mps(params, speed, is_in);
		if (buf_ctrl->mps == 0) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "MPS check fail\n");
			return HAL_ERR_PARA;
		}

		buf_list_cnt = usbd_composite_uac_get_ring_buf_cnt(USB_SPEED_HIGH, is_in);
#if USBD_COMPOSITE_UAC_DEBUG
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Buf mps len %d-%d(%d %d %d), cnt %d\n", buf_ctrl->mps, CACHE_LINE_ALIGNMENT(buf_ctrl->mps), params->ch_cnt,
				 params->byte_width, params->sampling_freq, buf_list_cnt);
#endif

		usb_ringbuf_manager_init(&(buf_ctrl->buf_list), buf_list_cnt, buf_ctrl->mps, 1);

		usb_os_sema_create(&(buf_ctrl->isoc_sema));
		buf_ctrl->sema_valid = 1;
	} else {
		usbd_composite_uac_ep_buf_ctrl_deinit(buf_ctrl);
	}

	return HAL_OK;
}

/**
  * @brief  Receive UAC data
  * @param  void
  * @retval Status
  */
int usbd_composite_uac_receive_data(usbd_composite_uac_buf_ctrl_t *buf_ctrl)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usb_dev_t *dev = uac->cdev->dev;

	if (!dev->is_ready) {
		//RTK_LOGS(TAG, RTK_LOG_ERROR, "State %d err\n", dev->is_ready);
		return HAL_ERR_PARA;
	}

	if (usbd_composite_uac_ep_enable(&(buf_ctrl->audio_config))) {
		/* empty all the data list */
		usb_ringbuf_reset(&(buf_ctrl->buf_list));

		buf_ctrl->xfer_continue = 1;
		buf_ctrl->sof_idx = 0;
		buf_ctrl->data_idx = 0;

#if USBD_COMPOSITE_UAC_DEBUG
		RTK_LOGS(TAG, RTK_LOG_INFO, "First trigger sema %d cnt %d-%d \n", buf_ctrl->wait_sema, usbd_composite_uac_get_read_buf_cnt(), buf_ctrl->mps);
#endif
	} else {
		RTK_LOGS(TAG, RTK_LOG_INFO, "Not support out\n");
	}

	return HAL_OK;
}

/**
  * @brief  Wait for an isochronous buffer to become available and check USB transfer status.
  * @param  buf_ctrl: Pointer to the USB UAC buffer control structure.
  * @param  timeout_ms:  Time out
  * @retval Status
  */
static int usbd_composite_uac_wait_isoc_with_status_check(usbd_composite_uac_buf_ctrl_t *buf_ctrl, uint32_t timeout_ms)
{
	int ret = HAL_ERR_PARA;
	u32 elapsed = 0;
	u32 wait_time = 0;

	while (elapsed < timeout_ms) {
		wait_time = (timeout_ms - elapsed > USBD_UAC_WAIT_SLICE_MS) ? USBD_UAC_WAIT_SLICE_MS : (timeout_ms - elapsed);

		buf_ctrl->wait_sema = 1;
		if (usb_os_sema_take(buf_ctrl->isoc_sema, wait_time) == HAL_OK) {
			buf_ctrl->wait_sema = 0;
			return HAL_OK;
		}

		elapsed += wait_time;
	}

	buf_ctrl->wait_sema = 0;
	return ret;
}

static u32 usbh_composite_uac_next_packet_size(usbd_composite_uac_buf_ctrl_t *buf_ctrl)
{
	UNUSED(buf_ctrl);
	return 16 * 2 * 16 / 8;
	// return 16*2*16/8/8;
}
/**
  * @brief  Write data to a USB audio ring buffer
  * @param  buf_ctrl: Pointer to the buffer control structure
  * @param  buffer: Pointer to Data buffer
  * @param  size: Data length
  * @param  written_len: Copy length
  * @retval Status 0 means get enought data, 1 means should continue to write the pkt
  */
static int usbd_composite_uac_write_ring_buf(usbd_composite_uac_buf_ctrl_t *buf_ctrl, u8 *buffer, u32 size, u32 *written_len)
{
	usb_ringbuf_manager_t *handle = &(buf_ctrl->buf_list);
	u32 written_size = buf_ctrl->written;
	u32 offset = 0;
	u32 xfer_len;
	u32 can_copy_len;
	u32 copy_len;

	if (written_size) {
		xfer_len = usbh_composite_uac_next_packet_size(buf_ctrl);
		can_copy_len = xfer_len - written_size;
		copy_len = size < can_copy_len ? size : can_copy_len;

		usb_os_memcpy((void *)(usbd_composite_uac_ringbuf_write_buf + written_size), (void *)buffer, copy_len);
		buf_ctrl->written += copy_len;

		offset += copy_len;
		*written_len += copy_len;

		if (size >= copy_len) {
			size -= copy_len;
			usb_ringbuf_add_tail(handle, usbd_composite_uac_ringbuf_write_buf, xfer_len, 1);
			buf_ctrl->written = 0;
		} else {
			return 0;
		}
	}

	do {
		if (usb_ringbuf_is_full(handle)) {
			return 1;
		}

		xfer_len = usbh_composite_uac_next_packet_size(buf_ctrl);

		if (size >= xfer_len) {
			usb_ringbuf_add_tail(handle, buffer + offset, xfer_len, 1);

			*written_len += xfer_len;
			size -= xfer_len;
			offset += xfer_len;
		} else {
			break;
		}
	} while (1);

	if (size > 0) {
		if (usb_ringbuf_is_full(handle)) {
			return 1;
		}

		usb_os_memcpy((void *)(usbd_composite_uac_ringbuf_write_buf), (void *)buffer, size);
		buf_ctrl->written = size;

		*written_len += size;
	}

	return 0;
}

/* Exported functions --------------------------------------------------------*/

int usbd_composite_uac_init(usbd_composite_dev_t *cdev, usbd_composite_uac_usr_cb_t *cb)
{
	int ret = HAL_OK;
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_ep_t *ep_out = &(uac->isoc_out.ep);
	usbd_ep_t *ep_in = &(uac->isoc_in.ep);

	usb_os_memset(uac, 0x00, sizeof(usbd_composite_uac_device_t));

	uac->cur_volume = 0x001F;
	uac->cur_mute = 0;
	uac->cur_clk_valid = 1;
#if USBD_COMPOSITE_UAC_DEBUG
	uac->uac_dump_task_alive = 0;
#endif
	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_in));
	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_out));

	ep_out->addr = USBD_COMP_UAC_ISOC_OUT_EP;
	ep_out->type = USB_CH_EP_TYPE_ISOC;

	ep_in->addr = USBD_COMP_UAC_ISOC_IN_EP;
	ep_in->type = USB_CH_EP_TYPE_ISOC;

	if (cb != NULL) {
		if ((cb->in.enable == 0) && (cb->out.enable == 0)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Pls cfg UAC EP\n");
			return HAL_ERR_PARA;
		}

		uac->cb = cb;

		usb_os_memcpy(&(uac->isoc_in.audio_config), &(cb->in), sizeof(usbd_audio_cfg_t));
		usb_os_memcpy(&(uac->isoc_out.audio_config), &(cb->out), sizeof(usbd_audio_cfg_t));

		/*init isoc in audio params */
		uac->isoc_in.audio_config.ch_cnt = USBD_UAC_IN_DEFAULT_CH_CNT;
		uac->isoc_in.audio_config.sampling_freq = USBD_UAC_IN_DEFAULT_SAMPLING_FREQ;
		uac->isoc_in.audio_config.byte_width = USBD_UAC_IN_DEFAULT_BYTE_WIDTH;

		/*init isoc out audio params */
		uac->isoc_out.audio_config.ch_cnt = USBD_UAC_OUT_DEFAULT_CH_CNT;
		uac->isoc_out.audio_config.sampling_freq = USBD_UAC_SAMPLING_FREQ_48K;
		uac->isoc_out.audio_config.byte_width = USBD_UAC_OUT_DEFAULT_BYTE_WIDTH;
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "UAC cb is NULL\n");
		return HAL_ERR_PARA;
	}

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != HAL_OK) {
			return ret;
		}
	}

	uac->cdev = cdev;

#if USBD_COMPOSITE_UAC_DEBUG
	if (rtos_task_create(&(uac->uac_dump_task), ((const char *)"usbd_composite_uac_status_dump_thread"), usbd_composite_uac_status_dump_thread, NULL, 1024U,
						 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create usb status dump task fail\n");
	}
#endif

	return ret;
}

/**
  * @brief  DeInitialize UAC device
  * @param  void
  * @retval Status
  */
int usbd_composite_uac_deinit(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;

#if USBD_COMPOSITE_UAC_DEBUG
	if (uac->uac_dump_task_alive) {
		rtos_task_delete(uac->uac_dump_task);
		uac->uac_dump_task_alive = 0;
	}
#endif

	if (uac->cb != NULL) {
		if (uac->cb->deinit != NULL) {
			uac->cb->deinit();
		}
		uac->cb = NULL;
	}

	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_in));
	usbd_composite_uac_ep_buf_ctrl_deinit(&(uac->isoc_out));

	return HAL_OK;
}

/**
  * @brief  Initialize UAC device configuration
  * @param  uac_cfg: USB audio config
  * @param  is_record: 1 for recording, 0 for playback
  * @param  flag: Unused parameter
  * @retval Status
  */
u8 usbd_composite_uac_config(const usbd_audio_cfg_t *uac_cfg, u8 is_record, u32 flag)
{
	UNUSED(flag);

	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl;

	/* all the transfer should finish */
	/* TODO: re initiation the isoc buffer[usbd_composite_uac_buf_ctrl_t] */
	if (uac_cfg && usbd_composite_uac_ep_enable((usbd_audio_cfg_t *)uac_cfg)) {
		if (is_record) { //
			buf_ctrl = &(uac->isoc_in);
		} else { //
			buf_ctrl = &(uac->isoc_out);
		}
		RTK_LOGS(TAG, RTK_LOG_INFO, "UAC cfg\n");
		buf_ctrl->xfer_continue = 0;
		usb_os_memcpy(&(buf_ctrl->audio_config), uac_cfg, sizeof(usbd_audio_cfg_t));

		usbd_composite_uac_ep_buf_ctrl_deinit(buf_ctrl);
		usbd_composite_uac_ep_buf_ctrl_init(buf_ctrl, &(buf_ctrl->audio_config), is_record);
	}

	return HAL_OK;
}

/*
	audio playback
	host -> usb device -> audio player
*/
/**
  * @brief  Initializes and starts the UAC data rx process
  * @param  void
  * @retval Status
  */
u32 usbd_composite_uac_start_play(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	int ret = HAL_OK;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC play start\n");

#if USBD_COMPOSITE_UAC_DEBUG
	buf_ctrl->last_xfer_tick = 0;
#endif

	ret = usbd_composite_uac_receive_data(buf_ctrl);

	return ret;
}

/**
  * @brief  Stop UAC device play
  * @param  void
  * @retval void
  */
void usbd_composite_uac_stop_play(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "UAC stop\n");

	uac->isoc_out.xfer_continue = 0;
}

/**
  * @brief  Read UAC data
  * @param  buf: Pointer to data buffer
  * @param  size: Data length
  * @param  time_out_ms: Time out
  * @param  zero_pkt_flag: zero packet ,each bit means a zero packet
  * @retval copy len
  */
u32 usbd_composite_uac_read(u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag)
{
	usbd_composite_uac_device_t *cdev = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(cdev->isoc_out);
	u32 copy_len = 0;
	u16 pkt_cnt = 0;

	if (buf_ctrl->xfer_continue == 0) {
		return 0;
	}

	if (time_out_ms == 0) {
		if (usb_ringbuf_is_empty(&(buf_ctrl->buf_list))) { //no valid data
			return 0;
		}

		usbd_composite_uac_read_ring_buf(buf_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag);
	} else {
		do {
			// if (usb_ringbuf_get_count(&(buf_ctrl->buf_list)) < usbd_composite_uac_get_buf_cnt())
			if (usb_ringbuf_is_empty(&(buf_ctrl->buf_list))) { //empty,need wait
				//wait sema
				buf_ctrl->wait_sema = 1;
				if (usb_os_sema_take(buf_ctrl->isoc_sema, time_out_ms) != HAL_OK) {
					buf_ctrl->wait_sema = 0;
					RTK_LOGS(TAG, RTK_LOG_DEBUG, "Ringtbuf break\n");
					break;
				}

				buf_ctrl->wait_sema = 0;
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if (usbd_composite_uac_read_ring_buf(buf_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag) == 0) {
					break;
				}
			}
		} while (buf_ctrl->xfer_continue);
	}

#if USBD_COMPOSITE_UAC_DEBUG
	usbd_composite_uac_get_audio_data_cnt(copy_len);
#endif

	if (copy_len > size) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Pls check the uac cfg\n");
	}

	return copy_len;
}

/**
  * @brief  Get UAC read frame cnt
  * @param  void
  * @retval read frame cnt
  */
u32 usbd_composite_uac_get_read_frame_cnt(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);

	return usb_ringbuf_get_count(&(buf_ctrl->buf_list));
}

/**
  * @brief  Get UAC read frame cnt
  * @param  void
  * @retval return the time duration for the avail packet in us
  */
u32 usbd_composite_uac_get_read_frame_time_in_us(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);
	usbd_composite_dev_t *cdev = uac->cdev;

	if (buf_ctrl == NULL) {
		return 0;
	}

	if (cdev->dev->dev_speed == USB_SPEED_HIGH) {
		return usb_ringbuf_get_count(&(buf_ctrl->buf_list)) * USBD_UAC_POW2(USBD_UAC_HS_ISOC_OUT_BINTERVAL - 1) * (USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS);
	} else {
		return usb_ringbuf_get_count(&(buf_ctrl->buf_list)) * USBD_UAC_ONE_KHZ * USBD_UAC_POW2(USBD_UAC_FS_ISOC_OUT_BINTERVAL - 1);
	}

	return 0;
}

/*
	Audio record
	Audio record PCM -> usb device -> host
*/

/**
  * @brief  Initializes and starts the UAC data tx process
  * @param  void
  * @retval Status
  */
u32 usbd_composite_uac_start_record(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_in);
	int ret = HAL_OK;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Record start\n");

#if USBD_COMPOSITE_UAC_DEBUG
	buf_ctrl->last_xfer_tick = 0;
#endif

	ret = usbd_composite_uac_receive_data(buf_ctrl);

	return ret;
}

/**
  * @brief  Stop UAC device record
  * @param  void
  * @retval void
  */
void usbd_composite_uac_stop_record(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;

	RTK_LOGS(TAG, RTK_LOG_DEBUG, "Record stop\n");

	uac->isoc_in.xfer_continue = 0;
}

/*
  */
u32 usbd_composite_uac_write(u8 *buffer, u32 size, u32 timeout_ms)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_in);
	u32 written_len = 0;
	u32 try_len, just_written;
	u8 need_wait = 0, last_zero = 0;

	if (buf_ctrl->mps == 0 || !buf_ctrl->sema_valid) {
		return 0;
	}

	if (buf_ctrl->xfer_continue == 0) {
		return 0;
	}

	while (written_len < size && buf_ctrl->xfer_continue) {
		need_wait = 0;

		if (timeout_ms) {
			if (usb_ringbuf_is_full(&(buf_ctrl->buf_list)) || last_zero) {
				need_wait = 1;
			}
		} else {
			if (usb_ringbuf_is_full(&(buf_ctrl->buf_list)) || last_zero) {
				break;
			}
		}

		if (need_wait) {
			if (usbd_composite_uac_wait_isoc_with_status_check(buf_ctrl, timeout_ms) != HAL_OK) {
				break;
			}
			last_zero = 0;
		}

		try_len = size - written_len;
		just_written = 0;

		usbd_composite_uac_write_ring_buf(buf_ctrl, buffer + written_len, try_len, &just_written);
		// RTK_LOGS(TAG, RTK_LOG_DEBUG, "Want %d msg, wrote %d msg %d\n", try_len, just_written, usbh_composite_uac_next_packet_size(buf_ctrl));

		if (just_written > 0) {
			written_len += just_written;
			last_zero = 0;
		} else {
			//wait sema and retry
			last_zero = 1;
		}
	}

	if (written_len > size) {
		RTK_LOGS(TAG, RTK_LOG_DEBUG, "Pls check the uac write cfg\n");
	}

	return written_len;
}


#if USBD_COMPOSITE_UAC_DEBUG
/*
	Get buf count
*/
static u32 usbd_composite_uac_get_buf_cnt(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_dev_t *cdev = uac->cdev;

	if (cdev->dev->dev_speed == USB_SPEED_HIGH) {
		return USBD_UAC_HS_SOF_COUNT_PER_MS / USBD_UAC_POW2(USBD_UAC_HS_ISOC_OUT_BINTERVAL - 1);
	} else {
		return 1 / USBD_UAC_POW2(USBD_UAC_FS_ISOC_OUT_BINTERVAL - 1);
	}

	return 1;
}

/**
  * @brief  Update or manage the count of audio data bytes
  * @param  audio_len: Data len had been wrote to the audiotrack
  * @retval void
  */
static inline void usbd_composite_uac_get_audio_data_cnt(u32 audio_len)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl = &(uac->isoc_out);

	buf_ctrl->xfer_total_len += audio_len;
}
/**
  * @brief  UAC status dump thread
  * @param  param: Pointer to parameters
  * @retval void
  */
static void usbd_composite_uac_status_dump_thread(void *param)
{
	UNUSED(param);
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usbd_composite_uac_buf_ctrl_t *buf_ctrl_in = &(uac->isoc_in);
	usbd_composite_uac_buf_ctrl_t *buf_ctrl_out = &(uac->isoc_out);

	uac->uac_dump_task_alive = 1;

	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC dump thread\n");

	while (1) {
		if (uac && uac->cdev && uac->cdev->dev) {
			usb_dev_t *dev = uac->cdev->dev;
			RTK_LOGS(TAG, RTK_LOG_INFO, "USB RX %d-%d/TO %d-%d/zp %d-%d-%d/OW %d/Pkt %d-%d/len %d-%d/%d\n",
					 buf_ctrl_out->xfer_cnt, buf_ctrl_out->xfer_done_cnt,
					 buf_ctrl_out->timeout_cnt, buf_ctrl_out->isoc_timeout_max_value,
					 buf_ctrl_out->append_zlp_cnt, (u32)(buf_ctrl_out->sof_idx), (u32)(buf_ctrl_out->data_idx),
					 buf_ctrl_out->overwrite_cnt,
					 usb_ringbuf_get_count(&(buf_ctrl_out->buf_list)), usbd_composite_uac_get_buf_cnt(),
					 buf_ctrl_out->xfer_total_len, buf_ctrl_out->last_xfer_len,
					 buf_ctrl_out->xfer_continue);

			RTK_LOGS(TAG, RTK_LOG_INFO, "USB TX %d-%d/len %d-%d\n",
					 buf_ctrl_in->xfer_cnt, usb_ringbuf_get_count(&(buf_ctrl_in->buf_list)),
					 buf_ctrl_in->last_xfer_len, buf_ctrl_in->xfer_continue);

			RTK_LOGS(TAG, RTK_LOG_INFO, "USB IsrC %lld-%lld/isrT %lld-%lld\n\n",
					 dev->isr_process_time_max, dev->isr_process_time,
					 dev->isr_enter_period_max, dev->isr_enter_period);
		}

		rtos_time_delay_ms(USBD_UAC_DEBUG_LOOP_TIME);
	}

	rtos_task_delete(NULL);
}

static void composite_usbd_comp_reset_isr_time(void)
{
	usbd_composite_uac_device_t *uac = &usbd_composite_uac_device;
	usb_dev_t *dev;

	if ((NULL == uac) || (NULL == uac->cdev) || (NULL == uac->cdev->dev)) {
		return;
	}

	dev = uac->cdev->dev;

	dev->isr_process_time_max = 0;
	dev->isr_enter_period_max = 0;
}
static u32 composite_usbd_comp_hid_test(u16 argc, u8 *argv[])
{
	int status = HAL_OK;
	char *cmd;

	if (argc == 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid argument\n");
		return HAL_ERR_PARA;
	}

	cmd = (char *)argv[0];

	if (_stricmp(cmd, "reset") == 0) {
		composite_usbd_comp_reset_isr_time();
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Input cmd err %s\n", cmd);
	}

	return status;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE test_composite_usbd_hid_cmd_table[] = {
	{"uacd_comp", composite_usbd_comp_hid_test},
};

#endif
